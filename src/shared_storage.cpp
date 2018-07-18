/*
 * This file is part of Wakanda software, licensed by 4D under
 *  ( i ) the GNU General Public License version 3 ( GNU GPL v3 ), or
 *  ( ii ) the Affero General Public License version 3 ( AGPL v3 ) or
 *  ( iii ) a commercial license.
 * This file remains the exclusive property of 4D and/or its licensors
 * and is protected by national and international legislations.
 * In any event, Licensee's compliance with the terms and conditions
 * of the applicable license constitutes a prerequisite to any use of this file.
 * Except as otherwise expressly stated in the applicable license,
 * such license does not include any other license or rights on this file,
 * 4D's and/or its licensors' trademarks and/or other proprietary rights.
 * Consequently, no title, copyright or other proprietary rights
 * other than those specified in the applicable license is granted.
 */

/**
 * \file    shared_storage.cpp
 */

// Local includes.
#include "shared_storage.h"

// Other includes.
#include <boost/interprocess/sync/scoped_lock.hpp>

namespace storage
{

SharedStorage::SharedStorage(const char* name, const int64_t size)
: m_name(name), m_segment(boost::interprocess::create_only, name, size), m_mutex(nullptr),
  m_itemInfoMap(nullptr)
{
    initialize();
}

SharedStorage::SharedStorage(const char* name)
: m_name(name), m_segment(boost::interprocess::open_only, name), m_mutex(nullptr),
  m_itemInfoMap(nullptr)
{
    initialize();
}

void SharedStorage::initialize()
{
    const char kItemInfoMapKey[] = "__item_info_map__";
    const char kStorageMutexKey[] = "__storage_mutex__";

    ItemInfoMapAllocator allocator(m_segment.get_segment_manager());
    m_mutex = m_segment.find_or_construct<boost::interprocess::interprocess_recursive_mutex>(
        kStorageMutexKey)();
    m_itemInfoMap = m_segment.find_or_construct<ItemInfoMap>(kItemInfoMapKey)(
        std::less<boost::interprocess::string>(), allocator);
}

SharedStorage::~SharedStorage() {}

SharedStorage* SharedStorage::create(const char* name, const int64_t size, Status& status)
{
    SharedStorage* storage = nullptr;
    status = eOk;

    try
    {
        storage = new SharedStorage(name, size);
    }
    catch (const std::exception&)
    {
        status = eCannotCreateStorage;
    }

    return storage;
}

SharedStorage* SharedStorage::open(const char* name, Status& status)
{
    SharedStorage* storage = nullptr;
    status = eOk;

    try
    {
        storage = new SharedStorage(name);
    }
    catch (const std::exception&)
    {
        status = eCannotOpenStorage;
    }

    return storage;
}

Status SharedStorage::destroy(const char* name)
{
    bool destroyed = false;

    try
    {
        destroyed = boost::interprocess::shared_memory_object::remove(name);
    }
    catch (const std::exception&)
    {
    }

    return destroyed ? eOk : eCannotDestroyStorage;
}

Status SharedStorage::setItem(const std::string& key, const SharedItem& item)
{
    Status status = eOk;
    bool constructNewValue = false;
    boost::interprocess::string ipStrKey(key.c_str());
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex> lock(
        *m_mutex);

    ItemInfoMap::iterator info = m_itemInfoMap->find(ipStrKey);
    if (info != m_itemInfoMap->end())
    {
        // an item with the same key already exists
        if (info->second.getType() != item.getType())
        {
            // the value type is different, then destroy the value and construct a
            // new one
            std::unique_ptr<SharedItem> itemToRemove;
            createSharedItem(info->second.getType(), std::string(), itemToRemove);
            if ((itemToRemove != nullptr) && itemToRemove->destroy(m_segment, key))
            {
                (*m_itemInfoMap).erase(info);
                constructNewValue = true;
            }
            else
            {
                status = eCannotReplaceItem;
            }
        }
        else
        {
            // the value type is the same, just update the value and the tag
            item.write(m_segment, key);

            info->second.setTag(item.getTag());
        }
    }
    else
    {
        // the item does not exist, create a new one
        constructNewValue = true;
    }

    if (constructNewValue)
    {
        item.construct(m_segment, key);

        ItemInfo info(item.getType(), item.getTag(),
                      InterprocessAllocator<char>(m_segment.get_segment_manager()));
        (*m_itemInfoMap)
            .insert(std::pair<const boost::interprocess::string, ItemInfo>(ipStrKey, info));
    }

    return status;
}

Status SharedStorage::getItem(const std::string& key, std::unique_ptr<SharedItem>& item)
{
    Status status = eOk;
    std::string strKey;
    boost::interprocess::string ipStrKey(key.c_str());
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex> lock(
        *m_mutex);

    ItemInfoMap::iterator info = m_itemInfoMap->find(ipStrKey);
    if (info != m_itemInfoMap->end())
    {
        std::string tag;
        info->second.getTag(tag);
        createSharedItem(info->second.getType(), tag, item);
        if (item != nullptr)
        {
            item->read(m_segment, key);
        }
        else
        {
            status = eUnknownItemType;
        }
    }
    else
    {
        status = eItemNotFound;
    }

    return status;
}

Status SharedStorage::removeItem(const std::string& key)
{
    Status status = eOk;
    boost::interprocess::string ipStrKey(key.c_str());
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex> lock(
        *m_mutex);

    ItemInfoMap::iterator info = m_itemInfoMap->find(ipStrKey);
    if (info != m_itemInfoMap->end())
    {
        std::unique_ptr<SharedItem> item;
        createSharedItem(info->second.getType(), std::string(), item);
        if (item != nullptr)
        {
            if (item->destroy(m_segment, key))
            {
                m_itemInfoMap->erase(info);
            }
            else
            {
                status = eCannotRemoveItem;
            }
        }
        else
        {
            status = eUnknownItemType;
        }
    }
    else
    {
        eItemNotFound;
    }

    return status;
}

void SharedStorage::clear()
{
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex> lock(
        *m_mutex);

    for (ItemInfoMap::iterator iter = m_itemInfoMap->begin(); iter != m_itemInfoMap->end(); ++iter)
    {
        std::unique_ptr<SharedItem> item;
        createSharedItem(iter->second.getType(), std::string(), item);
        if (item != nullptr)
        {
            item->destroy(m_segment, std::string(iter->first.c_str()));
        }
    }
    m_itemInfoMap->clear();
}

void SharedStorage::lock()
{
    if (m_mutex != nullptr)
    {
        m_mutex->lock();
    }
}

void SharedStorage::unlock()
{
    if (m_mutex != nullptr)
    {
        m_mutex->unlock();
    }
}

bool SharedStorage::tryToLock()
{
    if (m_mutex != nullptr)
    {
        return m_mutex->try_lock();
    }
    return false;
}

} // namespace storage