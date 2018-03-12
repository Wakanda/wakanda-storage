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
#include "napi_helpers.h"

// Other includes.
#include <boost/interprocess/sync/scoped_lock.hpp>

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

SharedStorage* SharedStorage::create(napi_env env, const char* name, const int64_t size)
{
    SharedStorage* storage = nullptr;

    try
    {
        storage = new SharedStorage(name, size);
    }
    catch (const std::exception& e)
    {
        napi_throw_error(env, nullptr, e.what());
    }

    return storage;
}

SharedStorage* SharedStorage::open(napi_env env, const char* name)
{
    SharedStorage* storage = nullptr;

    try
    {
        storage = new SharedStorage(name);
    }
    catch (const std::exception& e)
    {
        napi_throw_error(env, nullptr, e.what());
    }

    return storage;
}

bool SharedStorage::destroy(napi_env env, const char* name)
{
    bool destroyed = false;

    try
    {
        destroyed = boost::interprocess::shared_memory_object::remove(name);
    }
    catch (const std::exception& e)
    {
        napi_throw_error(env, nullptr, e.what());
    }

    return destroyed;
}

napi_status SharedStorage::setItem(napi_env env, napi_value key, napi_value value)
{
    napi_status status = napi_ok;
    napi_valuetype type = napi_undefined;
    std::string strKey;
    std::string stringValue;
    bool boolValue = false;
    double doubleValue = 0.0;

    // read key
    status = napi_helpers::getValueStringUTF8(env, key, strKey);
    // read value type
    if (status == napi_ok)
        status = napi_typeof(env, value, &type);
    // read value
    if (status == napi_ok)
    {
        switch (type)
        {
        case napi_boolean:
            status = napi_get_value_bool(env, value, &boolValue);
            break;

        case napi_number:
            status = napi_get_value_double(env, value, &doubleValue);
            break;

        case napi_string:
            status = napi_helpers::getValueStringUTF8(env, value, stringValue);
            break;

        case napi_object:
            status = napi_helpers::stringify(env, value, stringValue);
            break;

        default:
            break;
        }
    }
    if (status == napi_ok)
    {
        bool constructNewValue = false;
        boost::interprocess::string ipStrKey(strKey.c_str());
        boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex> lock(
            *m_mutex);

        ItemInfoMap::iterator itemInfo = m_itemInfoMap->find(ipStrKey);
        if (itemInfo != m_itemInfoMap->end())
        {
            // an item with the same key already exists
            if (itemInfo->second.m_itemType != type)
            {
                // the value type is different, then destroy the value and construct a
                // new one
                destroyItemValue(strKey.c_str(), itemInfo->second.m_itemType);
                itemInfo->second = {type};
                constructNewValue = true;
            }
            else
            {
                // the value type is the same, just update the value
                if (type == napi_boolean)
                {
                    bool* itemBoolValue = m_segment.find<bool>(strKey.c_str()).first;
                    *itemBoolValue = boolValue;
                }
                else if (type == napi_number)
                {
                    double* itemDoubleValue = m_segment.find<double>(strKey.c_str()).first;
                    *itemDoubleValue = doubleValue;
                    ;
                }
                else if ((type == napi_string) || (type == napi_object))
                {
                    StringValue* itemStringValue =
                        m_segment.find<StringValue>(strKey.c_str()).first;
                    itemStringValue->assign(stringValue.c_str());
                }
            }
        }
        else
        {
            // the item does not exist, create a new one
            (*m_itemInfoMap)[ipStrKey] = {type};
            constructNewValue = true;
        }

        if (constructNewValue)
        {
            if (type == napi_boolean)
            {
                m_segment.construct<bool>(strKey.c_str())(boolValue);
            }
            else if (type == napi_number)
            {
                m_segment.construct<double>(strKey.c_str())(doubleValue);
            }
            else if ((type == napi_string) || (type == napi_object))
            {
                m_segment.construct<StringValue>(strKey.c_str())(stringValue.c_str(),
                                                                 m_segment.get_segment_manager());
            }
        }
    }
    return status;
}

napi_status SharedStorage::getItem(napi_env env, napi_value key, napi_value* value)
{
    napi_status status = napi_ok;
    std::string strKey;
    status = napi_helpers::getValueStringUTF8(env, key, strKey);
    if (status == napi_ok)
    {
        napi_valuetype value_type = napi_undefined;
        StringValue* stringValue = nullptr;
        bool* boolValue = nullptr;
        double* doubleValue = nullptr;

        boost::interprocess::string ipStrKey(strKey.c_str());

        {
            boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex>
                lock(*m_mutex);

            ItemInfoMap::iterator itemInfo = m_itemInfoMap->find(ipStrKey);
            if (itemInfo != m_itemInfoMap->end())
            {
                value_type = itemInfo->second.m_itemType;
                if (value_type == napi_boolean)
                {
                    boolValue = m_segment.find<bool>(strKey.c_str()).first;
                }
                else if (value_type == napi_number)
                {
                    doubleValue = m_segment.find<double>(strKey.c_str()).first;
                }
                else if ((value_type == napi_string) || (value_type == napi_object))
                {
                    stringValue = m_segment.find<StringValue>(strKey.c_str()).first;
                }
            }
        }

        switch (value_type)
        {
        case napi_undefined:
            status = napi_get_undefined(env, value);
            break;

        case napi_null:
            status = napi_get_null(env, value);
            break;

        case napi_boolean:
            status = napi_get_boolean(env, *boolValue, value);
            break;

        case napi_number:
            status = napi_create_double(env, *doubleValue, value);
            break;

        case napi_string:
            status = napi_create_string_utf8(env, stringValue->c_str(), NAPI_AUTO_LENGTH, value);
            break;

        case napi_object:
        {
            std::string string(stringValue->c_str());
            status = napi_helpers::parse(env, string, value);
            break;
        }

        default:
            break;
        }
    }
    return status;
}

napi_status SharedStorage::removeItem(napi_env env, napi_value key)
{
    napi_status status = napi_ok;
    std::string strKey;
    status = napi_helpers::getValueStringUTF8(env, key, strKey);
    if (status == napi_ok)
    {
        boost::interprocess::string ipStrKey(strKey.c_str());
        boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex> lock(
            *m_mutex);

        ItemInfoMap::iterator itemInfo = m_itemInfoMap->find(ipStrKey);
        if (itemInfo != m_itemInfoMap->end())
        {
            destroyItemValue(strKey.c_str(), itemInfo->second.m_itemType);
            m_itemInfoMap->erase(itemInfo);
        }
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

bool SharedStorage::destroyItemValue(const char* key, napi_valuetype type)
{
    if (type == napi_boolean)
    {
        return m_segment.destroy<bool>(key);
    }
    else if (type == napi_number)
    {
        return m_segment.destroy<double>(key);
    }
    else if ((type == napi_string) || (type == napi_object))
    {
        return m_segment.destroy<StringValue>(key);
    }
    return false;
}



napi_ref JsSharedStorage::m_constructor = nullptr;

napi_status JsSharedStorage::define(napi_env env)
{
    std::vector<napi_property_descriptor> properties;
    properties.push_back(
        {"set", nullptr, setItem, nullptr, nullptr, nullptr, napi_default, nullptr});
    properties.push_back(
        {"get", nullptr, getItem, nullptr, nullptr, nullptr, napi_default, nullptr});
    properties.push_back(
        {"remove", nullptr, removeItem, nullptr, nullptr, nullptr, napi_default, nullptr});
    properties.push_back(
        {"clear", nullptr, clear, nullptr, nullptr, nullptr, napi_default, nullptr});
    properties.push_back({"lock", nullptr, lock, nullptr, nullptr, nullptr, napi_default, nullptr});
    properties.push_back(
        {"unlock", nullptr, unlock, nullptr, nullptr, nullptr, napi_default, nullptr});
    properties.push_back(
        {"tryLock", nullptr, tryToLock, nullptr, nullptr, nullptr, napi_default, nullptr});
    napi_value constructor = nullptr;
    napi_status status =
        napi_define_class(env, "SharedStorage", NAPI_AUTO_LENGTH, JsSharedStorage::constructor,
                          nullptr, properties.size(), properties.data(), &constructor);

    if (status == napi_ok)
    {
        status = napi_create_reference(env, constructor, 1, &JsSharedStorage::m_constructor);
    }
    return status;
}

napi_status JsSharedStorage::undefine(napi_env env)
{
    napi_status status = napi_ok;
    if (JsSharedStorage::m_constructor != nullptr)
    {
        status = napi_delete_reference(env, JsSharedStorage::m_constructor);
        JsSharedStorage::m_constructor = nullptr;
    }
    return status;
}

napi_value JsSharedStorage::constructor(napi_env env, napi_callback_info info)
{
    napi_value thisInstance = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisInstance, nullptr);
    return thisInstance;
}

napi_status JsSharedStorage::createInstance(napi_env env, SharedStorage* storage,
                                            napi_value* result)
{
    napi_value constructor = nullptr;
    napi_status status =
        napi_get_reference_value(env, JsSharedStorage::m_constructor, &constructor);
    if (status == napi_ok)
    {
        status = napi_new_instance(env, constructor, 0, nullptr, result);
        if (status == napi_ok)
        {
            status = napi_wrap(env, *result, storage, JsSharedStorage::finalize, nullptr, nullptr);
        }
    }
    return status;
}

void JsSharedStorage::finalize(napi_env env, void* data, void* hint)
{
    SharedStorage* storage = static_cast<SharedStorage*>(data);
    delete storage;
}

napi_status JsSharedStorage::getStorage(napi_env env, napi_callback_info info,
                                        SharedStorage** result)
{
    napi_value thisInstance = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisInstance, nullptr);
    if (status == napi_ok)
        status = napi_unwrap(env, thisInstance, (void**)result);

    return status;
}

napi_value JsSharedStorage::create(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_value args[2];
    size_t argsCount = 2;
    napi_status status = napi_get_cb_info(env, info, &argsCount, args, nullptr, nullptr);
    if ((status == napi_ok) && (argsCount > 0))
    {
        if (napi_helpers::isString(env, args[0]))
        {
            std::string strKey;
            status = napi_helpers::getValueStringUTF8(env, args[0], strKey);
            if (status == napi_ok)
            {
                int64_t size = 1024 * 1024;
                if (argsCount > 1)
                {
                    if (napi_helpers::isNumber(env, args[1]))
                    {
                        status = napi_get_value_int64(env, args[1], &size);
                    }
                }
                if (status == napi_ok)
                {
                    SharedStorage* storage = SharedStorage::create(env, strKey.c_str(), size);
                    if (storage != nullptr)
                    {
                        status = JsSharedStorage::createInstance(env, storage, &result);
                    }
                }
            }
        }
    }
    return result;
}

napi_value JsSharedStorage::open(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_value args[1];
    size_t argsCount = 1;
    napi_status status = napi_get_cb_info(env, info, &argsCount, args, nullptr, nullptr);
    if ((status == napi_ok) && (argsCount == 1))
    {
        if (napi_helpers::isString(env, args[0]))
        {
            std::string strKey;
            status = napi_helpers::getValueStringUTF8(env, args[0], strKey);
            if (status == napi_ok)
            {
                SharedStorage* storage = SharedStorage::open(env, strKey.c_str());
                if (storage != nullptr)
                {
                    status = JsSharedStorage::createInstance(env, storage, &result);
                }
            }
        }
    }
    return result;
}

napi_value JsSharedStorage::destroy(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_value args[1];
    size_t argsCount = 1;
    napi_status status = napi_get_cb_info(env, info, &argsCount, args, nullptr, nullptr);
    if ((status == napi_ok) && (argsCount == 1))
    {
        if (napi_helpers::isString(env, args[0]))
        {
            std::string strKey;
            status = napi_helpers::getValueStringUTF8(env, args[0], strKey);
            if (status == napi_ok)
            {
                bool destroyed = SharedStorage::destroy(env, strKey.c_str());
                status = napi_get_boolean(env, destroyed, &result);
            }
        }
    }
    return result;
}

napi_value JsSharedStorage::setItem(napi_env env, napi_callback_info info)
{
    napi_value thisInstance = nullptr;
    size_t argsCount = 2;
    napi_value args[2];
    napi_status status = napi_get_cb_info(env, info, &argsCount, args, &thisInstance, nullptr);
    if ((status == napi_ok) && (argsCount == 2))
    {
        if (napi_helpers::isString(env, args[0]))
        {
            SharedStorage* storage = nullptr;
            status = napi_unwrap(env, thisInstance, (void**)&storage);
            if (status == napi_ok)
            {
                try
                {
                    storage->setItem(env, args[0], args[1]);
                }
                catch (const std::exception& e)
                {
                    napi_throw_error(env, nullptr, e.what());
                }
            }
        }
    }

    return nullptr;
}

napi_value JsSharedStorage::getItem(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_value thisInstance = nullptr;
    size_t argsCount = 1;
    napi_value args[1];
    napi_status status = napi_get_cb_info(env, info, &argsCount, args, &thisInstance, nullptr);
    if ((status == napi_ok) && (argsCount == 1))
    {
        if (napi_helpers::isString(env, args[0]))
        {
            SharedStorage* storage = nullptr;
            status = napi_unwrap(env, thisInstance, (void**)&storage);
            if (status == napi_ok)
            {
                storage->getItem(env, args[0], &result);
            }
        }
    }
    return result;
}

napi_value JsSharedStorage::removeItem(napi_env env, napi_callback_info info)
{
    napi_value thisInstance = nullptr;
    size_t argsCount = 1;
    napi_value args[1];
    napi_status status = napi_get_cb_info(env, info, &argsCount, args, &thisInstance, nullptr);
    if ((status == napi_ok) && (argsCount == 1))
    {
        if (napi_helpers::isString(env, args[0]))
        {
            SharedStorage* storage = nullptr;
            status = napi_unwrap(env, thisInstance, (void**)&storage);
            if (status == napi_ok)
            {
                try
                {
                    storage->removeItem(env, args[0]);
                }
                catch (const std::exception& e)
                {
                    napi_throw_error(env, nullptr, e.what());
                }
            }
        }
    }
    return nullptr;
}

napi_value JsSharedStorage::clear(napi_env env, napi_callback_info info)
{
    SharedStorage* storage = nullptr;
    napi_status status = getStorage(env, info, &storage);
    if (status == napi_ok)
    {
        try
        {
            storage->clear();
        }
        catch (const std::exception& e)
        {
            napi_throw_error(env, nullptr, e.what());
        }
    }
    return nullptr;
}

napi_value JsSharedStorage::lock(napi_env env, napi_callback_info info)
{
    SharedStorage* storage = nullptr;
    napi_status status = getStorage(env, info, &storage);
    if (status == napi_ok)
    {
        storage->lock();
    }
    return nullptr;
}

napi_value JsSharedStorage::unlock(napi_env env, napi_callback_info info)
{
    SharedStorage* storage = nullptr;
    napi_status status = getStorage(env, info, &storage);
    if (status == napi_ok)
    {
        storage->unlock();
    }
    return nullptr;
}

napi_value JsSharedStorage::tryToLock(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    SharedStorage* storage = nullptr;
    napi_status status = getStorage(env, info, &storage);
    if (status == napi_ok)
    {
        bool locked = storage->tryToLock();
        status = napi_get_boolean(env, locked, &result);
    }
    return result;
}
