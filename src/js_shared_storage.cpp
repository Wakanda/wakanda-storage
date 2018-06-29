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
#include "js_shared_storage.h"
#include "napi_helpers.h"
#include "shared_storage.h"


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

napi_status JsSharedStorage::createInstance(napi_env env, storage::SharedStorage* storage,
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
    storage::SharedStorage* storage = static_cast<storage::SharedStorage*>(data);
    delete storage;
}

napi_status JsSharedStorage::getStorage(napi_env env, napi_callback_info info,
                                        storage::SharedStorage** result)
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
                    storage::Status stStatus = storage::eOk;
                    storage::SharedStorage* storage =
                        storage::SharedStorage::create(strKey.c_str(), size, stStatus);
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
                storage::Status stStatus = storage::eOk;
                storage::SharedStorage* storage =
                    storage::SharedStorage::open(strKey.c_str(), stStatus);
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
                storage::Status stStatus = storage::SharedStorage::destroy(strKey.c_str());
                status = napi_get_boolean(env, (stStatus == storage::eOk), &result);
            }
        }
    }
    return result;
}

napi_value JsSharedStorage::setItem(napi_env env, napi_callback_info info)
{
    napi_value thisInstance = nullptr;
    size_t argsCount = 3;
    napi_value args[3];
    napi_status status = napi_get_cb_info(env, info, &argsCount, args, &thisInstance, nullptr);
    if ((status == napi_ok) && (argsCount >= 2))
    {
        storage::SharedStorage* storage = nullptr;
        status = napi_unwrap(env, thisInstance, (void**)&storage);
        if (status == napi_ok)
        {
            storage::ItemDescriptor item;
            std::string key;
            napi_valuetype type = napi_undefined;

            status = napi_helpers::getValueStringUTF8(env, args[0], key);
            if (status == napi_ok)
            {
                status = napi_typeof(env, args[1], &type);
            }
            if (status == napi_ok)
            {
                switch (type)
                {
                case napi_boolean:
                {
                    bool value = false;
                    status = napi_get_value_bool(env, args[1], &value);
                    if (status == napi_ok)
                    {
                        item.m_type = storage::eBool;
                        item.m_bool = value;
                    }
                    break;
                }

                case napi_number:
                {
                    double value = 0.0;
                    status = napi_get_value_double(env, args[1], &value);
                    if (status == napi_ok)
                    {
                        item.m_type = storage::eDouble;
                        item.m_double = value;
                    }
                    break;
                }

                case napi_string:
                {
                    std::string value;
                    status = napi_helpers::getValueStringUTF8(env, args[1], value);
                    if (status == napi_ok)
                    {
                        item.m_type = storage::eString;
                        item.m_string.reset(new std::string(value));
                    }
                    break;
                }

                case napi_object:
                {
                    std::string value;
                    status = napi_helpers::stringify(env, args[1], value);
                    if (status == napi_ok)
                    {
                        item.m_type = storage::eObject;
                        item.m_string.reset(new std::string(value));
                    }
                    break;
                }

                case napi_null:
                {
                    item.m_type = storage::eNull;
                    break;
                }

                default:
                    break;
                }
            }

            if ((status == napi_ok) && (item.m_type != storage::eNone))
            {
                try
                {
                    if ((argsCount >= 3) && napi_helpers::isString(env, args[2]))
                    {
                        napi_helpers::getValueStringUTF8(env, args[2], item.m_tag);
                    }
                    storage->setItem(key, item);
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
    size_t argsCount = 2;
    napi_value args[2];
    napi_status status = napi_get_cb_info(env, info, &argsCount, args, &thisInstance, nullptr);
    if ((status == napi_ok) && (argsCount >= 1))
    {
        storage::SharedStorage* storage = nullptr;
        status = napi_unwrap(env, thisInstance, (void**)&storage);
        if (status == napi_ok)
        {
            storage::ItemDescriptor item;
            storage::Status stStatus = storage::eOk;
            std::string key;
            status = napi_helpers::getValueStringUTF8(env, args[0], key);
            if (status == napi_ok)
            {
                stStatus = storage->getItem(key, item);
            }
            if (stStatus == storage::eOk)
            {
                switch (item.m_type)
                {
                case storage::eBool:
                    status = napi_get_boolean(env, item.m_bool, &result);
                    break;

                case storage::eDouble:
                    status = napi_create_double(env, item.m_double, &result);
                    break;

                case storage::eString:
                    status = napi_create_string_utf8(env, item.m_string->c_str(), NAPI_AUTO_LENGTH,
                                                     &result);
                    break;

                case storage::eObject:
                    status = napi_helpers::parse(env, *item.m_string, &result);
                    break;

                case storage::eNull:
                    status = napi_get_null(env, &result);
                    break;

                default:
                    status = napi_get_undefined(env, &result);
                    break;
                }
            }
            if ((status == napi_ok) && (argsCount >= 2) && napi_helpers::isBool(env, args[1]))
            {
                bool withTag = false;
                status = napi_get_value_bool(env, args[1], &withTag);
                if ((status == napi_ok) && withTag)
                {
                    /**
                     * if withTag is true, result is an object:
                     * {
                     *	"value": item_value,
                     *	"tag": item_tag
                     * }
                     */
                    napi_value object = nullptr;
                    napi_value tag = nullptr;
                    status = napi_create_object(env, &object);
                    if (status == napi_ok)
                    {
                        status = napi_helpers::createValueStringUTF8(item.m_tag, env, &tag);
                    }
                    if (status == napi_ok)
                    {
                        status = napi_set_named_property(env, object, "value", result);
                    }
                    if (status == napi_ok)
                    {
                        status = napi_set_named_property(env, object, "tag", tag);
                    }
                    if (status == napi_ok)
                    {
                        result = object;
                    }
                }
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
        storage::SharedStorage* storage = nullptr;
        status = napi_unwrap(env, thisInstance, (void**)&storage);
        if (status == napi_ok)
        {
            std::string key;
            status = napi_helpers::getValueStringUTF8(env, args[0], key);
            if (status == napi_ok)
            {
                try
                {
                    storage->removeItem(key);
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
    storage::SharedStorage* storage = nullptr;
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
    storage::SharedStorage* storage = nullptr;
    napi_status status = getStorage(env, info, &storage);
    if (status == napi_ok)
    {
        storage->lock();
    }
    return nullptr;
}

napi_value JsSharedStorage::unlock(napi_env env, napi_callback_info info)
{
    storage::SharedStorage* storage = nullptr;
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
    storage::SharedStorage* storage = nullptr;
    napi_status status = getStorage(env, info, &storage);
    if (status == napi_ok)
    {
        bool locked = storage->tryToLock();
        status = napi_get_boolean(env, locked, &result);
    }
    return result;
}
