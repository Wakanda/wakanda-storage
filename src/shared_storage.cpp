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

Status SharedStorage::setItem(const std::string& key, const ItemDescriptor& item)
{
    Status status = eOk;
    bool constructNewValue = false;
    boost::interprocess::string ipStrKey(key.c_str());
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex> lock(
        *m_mutex);

    ItemInfoMap::iterator itemInfo = m_itemInfoMap->find(ipStrKey);
    if (itemInfo != m_itemInfoMap->end())
    {
        // an item with the same key already exists
        if (itemInfo->second.m_itemType != item.m_type)
        {
            // the value type is different, then destroy the value and construct a
            // new one
            if (destroyItemValue(key.c_str(), itemInfo->second.m_itemType))
            {
                itemInfo->second = {item.m_type};
                constructNewValue = true;
            }
            else
            {
                status = eCannotReplaceItem;
            }
        }
        else
        {
            // the value type is the same, just update the value
            if (item.m_type == eBool)
            {
                bool* value = m_segment.find<bool>(key.c_str()).first;
                *value = item.m_bool;
            }
            else if (item.m_type == eDouble)
            {
                double* value = m_segment.find<double>(key.c_str()).first;
                *value = item.m_double;
            }
            else if ((item.m_type == eString) || (item.m_type == eObject))
            {
                if (item.m_string != nullptr)
                {
                    StringValue* value = m_segment.find<StringValue>(key.c_str()).first;
                    value->assign(item.m_string->c_str());
                }
            }
            else if (item.m_type == eNull)
            {
                // nothing to do
            }
            else
            {
                status = eUnknownItemType;
            }
        }
    }
    else
    {
        // the item does not exist, create a new one
        (*m_itemInfoMap)[ipStrKey] = {item.m_type};
        constructNewValue = true;
    }

    if (constructNewValue)
    {
        if (item.m_type == eBool)
        {
            m_segment.construct<bool>(key.c_str())(item.m_bool);
        }
        else if (item.m_type == eDouble)
        {
            m_segment.construct<double>(key.c_str())(item.m_double);
        }
        else if ((item.m_type == eString) || (item.m_type == eObject))
        {
            if (item.m_string != nullptr)
            {
                m_segment.construct<StringValue>(key.c_str())(item.m_string->c_str(),
                                                              m_segment.get_segment_manager());
            }
        }
        else if (item.m_type == eNull)
        {
            // nothing to do
        }
        else
        {
            status = eUnknownItemType;
        }
    }

    return status;
}

Status SharedStorage::getItem(const std::string& key, ItemDescriptor& item)
{
    Status status = eOk;
    std::string strKey;
    boost::interprocess::string ipStrKey(key.c_str());
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex> lock(
        *m_mutex);

    ItemInfoMap::iterator itemInfo = m_itemInfoMap->find(ipStrKey);
    if (itemInfo != m_itemInfoMap->end())
    {
        if (itemInfo->second.m_itemType == eBool)
        {
            item.m_type = eBool;
            item.m_bool = *m_segment.find<bool>(key.c_str()).first;
        }
        else if (itemInfo->second.m_itemType == eDouble)
        {
            item.m_type = eDouble;
            item.m_double = *m_segment.find<double>(key.c_str()).first;
        }
        else if ((itemInfo->second.m_itemType == eString) ||
                 (itemInfo->second.m_itemType == eObject))
        {
            item.m_type = itemInfo->second.m_itemType;
            StringValue* stringValue = m_segment.find<StringValue>(key.c_str()).first;
            item.m_string.reset(new std::string(stringValue->c_str()));
        }
        else if (itemInfo->second.m_itemType == eNull)
        {
            item.m_type = eNull;
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

    ItemInfoMap::iterator itemInfo = m_itemInfoMap->find(ipStrKey);
    if (itemInfo != m_itemInfoMap->end())
    {
        if (destroyItemValue(key.c_str(), itemInfo->second.m_itemType))
        {
            m_itemInfoMap->erase(itemInfo);
        }
        else
        {
            status = eCannotRemoveItem;
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
        destroyItemValue(iter->first.c_str(), iter->second.m_itemType);
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

bool SharedStorage::destroyItemValue(const char* key, ItemType type)
{
    if (type == eBool)
    {
        return m_segment.destroy<bool>(key);
    }
    else if (type == eDouble)
    {
        return m_segment.destroy<double>(key);
    }
    else if ((type == eString) || (type == eObject))
    {
        return m_segment.destroy<StringValue>(key);
    }
    else if (type == eNull)
    {
        // nothing to do
    }
    return false;
}

} // namespace storage