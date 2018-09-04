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


namespace storage
{

SharedStorage::SharedStorage(const std::string& name, const int64_t size)
: m_name(name), m_segment(boost::interprocess::create_only, name.c_str(), size), m_mutex(nullptr),
  m_itemInfoMap(nullptr)
{
    initialize();
}

SharedStorage::SharedStorage(const std::string& name)
: m_name(name), m_segment(boost::interprocess::open_only, name.c_str()), m_mutex(nullptr),
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

SharedStorage* SharedStorage::create(const std::string& name, const int64_t size, Status& status)
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

SharedStorage* SharedStorage::open(const std::string& name, Status& status)
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

Status SharedStorage::destroy(const std::string& name)
{
    bool destroyed = boost::interprocess::shared_memory_object::remove(name.c_str());
    return destroyed ? eOk : eCannotDestroyStorage;
}

Status SharedStorage::destroy()
{
    bool destroyed = boost::interprocess::shared_memory_object::remove(m_name.c_str());
    return destroyed ? eOk : eCannotDestroyStorage;
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
        ItemDestructor itemDestructor(this);
        status = getItem<ItemDestructor>(key, info->second, itemDestructor);
        if (status == eOk)
        {
            status = itemDestructor.getStatus();
        }
        if ((status == eOk) || (status == eItemNotFound))
        {
            (*m_itemInfoMap).erase(info);
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

Status SharedStorage::clear()
{
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex> lock(
        *m_mutex);

    Status status = eOk;
    for (ItemInfoMap::iterator iter = m_itemInfoMap->begin();
         (iter != m_itemInfoMap->end()) && (status == eOk); ++iter)
    {
        ItemDestructor itemDestructor(this);
        status =
            getItem<ItemDestructor>(std::string(iter->first.c_str()), iter->second, itemDestructor);
        if (status == eOk)
        {
            status = itemDestructor.getStatus();
        }
        if (status == eItemNotFound)
        {
            status = eOk;
        }
    }
    if (status == eOk)
    {
        m_itemInfoMap->clear();
    }
    else
    {
        status = eCannotRemoveItem;
    }
    return status;
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