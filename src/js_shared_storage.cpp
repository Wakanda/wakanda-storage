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
#include <stdio.h>


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
                        storage::SharedStorage::create(strKey, size, stStatus);
                    if (stStatus == storage::eOk)
                    {
                        status = JsSharedStorage::createInstance(env, storage, &result);
                    }
                    else
                    {
                        throw_error(env, stStatus, strKey);
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
                storage::SharedStorage* storage = storage::SharedStorage::open(strKey, stStatus);
                if (stStatus == storage::eOk)
                {
                    status = JsSharedStorage::createInstance(env, storage, &result);
                }
                else
                {
                    throw_error(env, stStatus, strKey);
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
                storage::Status stStatus = storage::SharedStorage::destroy(strKey);
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
            std::string key, tag;
            napi_valuetype type = napi_undefined;

            status = napi_helpers::getValueStringUTF8(env, args[0], key);
            if ((argsCount >= 3) && napi_helpers::isString(env, args[2]))
            {
                status = napi_helpers::getValueStringUTF8(env, args[2], tag);
            }
            if (status == napi_ok)
            {
                status = napi_typeof(env, args[1], &type);
            }
            if (status == napi_ok)
            {
                storage::Status stStatus = storage::eOk;

                switch (type)
                {
                case napi_boolean:
                {
                    bool value = false;
                    status = napi_get_value_bool(env, args[1], &value);
                    if (status == napi_ok)
                    {
                        stStatus = storage->setItem<bool>(key, storage::Item<bool>(value, tag));
                    }
                    break;
                }

                case napi_number:
                {
                    double value = 0.0;
                    status = napi_get_value_double(env, args[1], &value);
                    if (status == napi_ok)
                    {
                        stStatus = storage->setItem<double>(key, storage::Item<double>(value, tag));
                    }
                    break;
                }

                case napi_string:
                {
                    std::string value;
                    status = napi_helpers::getValueStringUTF8(env, args[1], value);
                    if (status == napi_ok)
                    {
                        stStatus = storage->setItem<std::string>(
                            key, storage::Item<std::string>(value, tag));
                    }
                    break;
                }

                default:
                    napi_throw_error(env, nullptr, "unsupported value type.");
                    break;
                }

                if (stStatus != storage::eOk)
                {
                    throw_error(env, stStatus, key);
                }
            }
        }
    }

    return nullptr;
}


/**
 * @brief  Item consumer compliant with item consumer specifications.
 */
class ItemConsumer
{
public:
    /**
     * @brief  Deleted constructor.
     */
    ItemConsumer() = delete;

    /**
     * @brief  Constructor.
     *
     * @param env Nodejs environment handler.
     */
    ItemConsumer(napi_env env) : m_env(env), m_status(napi_ok), m_value(nullptr) {}

    /**
     * @brief  Get the status of napi_value creation.
     *
     * @return napi_ok if the napi_value has been created from the consummed item.
     */
    napi_status getStatus() const { return m_status; }

    /**
     * @brief  Get the value created from the consummed item.

     * @return created napi_value or nullptr.
     */
    napi_value getValue() const { return m_value; }

    /**
     * @brief  Get the tag of the consummed item.
     *
     * @return tag of the consummed item.
     */
    const std::string& getTag() const { return m_tag; }

    /**
     * @brief  Create a napi_value from the passed item. Generic implementation.
     *
     * @param key Key of the item.
     * @param item Item description.
     * @tparam T Value type of the item.
     */
    template <class T> void set(const std::string& key, storage::Item<T>& item)
    {
        // for unknown type
        m_status = napi_get_undefined(m_env, &m_value);
    }


private:
    napi_env m_env;
    napi_status m_status;
    napi_value m_value;
    std::string m_tag;
};

/**
 * @brief  Bool values specialization.
 */
template <> void ItemConsumer::set<bool>(const std::string& key, storage::Item<bool>& item)
{
    m_status = napi_get_boolean(m_env, item.getValue(), &m_value);
    m_tag = item.getTag();
}

/**
 * @brief  Double values specialization.
 */
template <> void ItemConsumer::set<double>(const std::string& key, storage::Item<double>& item)
{
    m_status = napi_create_double(m_env, item.getValue(), &m_value);
    m_tag = item.getTag();
}

/**
 * @brief  String values specialization.
 */
template <>
void ItemConsumer::set<std::string>(const std::string& key, storage::Item<std::string>& item)
{
    m_status = napi_create_string_utf8(m_env, item.getValue().c_str(), NAPI_AUTO_LENGTH, &m_value);
    m_tag = item.getTag();
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
            ItemConsumer consumer(env);
            storage::Status stStatus = storage::eOk;
            std::string key;
            status = napi_helpers::getValueStringUTF8(env, args[0], key);
            if (status == napi_ok)
            {
                stStatus = storage->getItem<ItemConsumer>(key, consumer);
            }
            if (stStatus == storage::eOk)
            {
                result = consumer.getValue();

                if ((consumer.getStatus() == napi_ok) && (argsCount >= 2) &&
                    napi_helpers::isBool(env, args[1]))
                {
                    bool withTag = false;
                    status = napi_get_value_bool(env, args[1], &withTag);
                    if ((status == napi_ok) && withTag)
                    {
                        /**
                         * if withTag is true, result is an object:
                         * {
                         *	 "value": itemValue,
                         *	 "tag": itemBag
                         * }
                         */
                        napi_value object = nullptr;
                        napi_value tag = nullptr;
                        status = napi_create_object(env, &object);
                        if (status == napi_ok)
                        {
                            status =
                                napi_helpers::createValueStringUTF8(consumer.getTag(), env, &tag);
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
                storage::Status stStatus = storage->removeItem(key);
                if (stStatus != storage::eOk)
                {
                    throw_error(env, stStatus, key);
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
        storage::Status stStatus = storage->clear();
        if (stStatus != storage::eOk)
        {
            throw_error(env, stStatus);
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

napi_status JsSharedStorage::throw_error(napi_env env, unsigned int status)
{
    return throw_error(env, status, std::string());
}

napi_status JsSharedStorage::throw_error(napi_env env, unsigned int status,
                                         const std::string& identifier)
{
    napi_status result = napi_ok;
    std::string message;
    std::string decoratedIdentifier;

    if (!identifier.empty())
    {
        decoratedIdentifier = " \"" + identifier + "\"";
    }

    switch (status)
    {
    case storage::eOk:
        break;

    case storage::eCannotCreateStorage:
        message = "cannot create the storage" + decoratedIdentifier + ". It may already exist.";
        break;

    case storage::eCannotOpenStorage:
        message = "cannot open the storage" + decoratedIdentifier + ". It may not exist.";
        break;

    case storage::eCannotConstructItem:
        message = "cannot set the item" + decoratedIdentifier + ". The storage may be full.";
        break;

    case storage::eCannotReplaceItem:
        message = "cannot set the item" + decoratedIdentifier +
                  ". An item with the same key exists and cannot be removed.";
        break;

    case storage::eCannotRemoveItem:
        message = "cannot remove the item" + decoratedIdentifier + ".";
        break;

    case storage::eCannotClearStorage:
        message = "cannot remove all items in the storage.";
        break;

    default:
        result = napi_throw_error(env, nullptr, "internal storage error.");
        break;
    }

    if (!message.empty())
    {
        napi_value errorMsg = nullptr;
        napi_value errorCode = nullptr;
        napi_value error = nullptr;

        result = napi_helpers::createValueStringUTF8(message, env, &errorMsg);
        if (result == napi_ok)
        {
            char buffer[16];
            if (snprintf(buffer, 16, "%d", status) > 0)
            {
                result = napi_helpers::createValueStringUTF8(std::string(buffer), env, &errorCode);
            }
        }
        if (result == napi_ok)
        {
            result = napi_create_error(env, errorCode, errorMsg, &error);
        }
        if (result == napi_ok)
        {
            result = napi_throw(env, error);
        }
    }

    return result;
}