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
 * \file    js_shared_storage.h
 */

#ifndef JS_SHARED_STORAGE_H_
#define JS_SHARED_STORAGE_H_

namespace storage
{
class SharedStorage;
} // namespace storage

// Includes.
#include <node_api.h>
#include <string>


/**
 * @brief  shared storage implementation for JavaScript
 */
class JsSharedStorage
{
public:
    /**
     * @brief  Define the SharedStorage class at JavaScript level.
     *
     * @param env Nodejs environment handler.
     *
     * @return napi_ok if defining the SharedStorage class succeeded.
     */
    static napi_status define(napi_env env);

    /**
     * @brief  Undefine the SharedStorage class at JavaScript level.
     *
     * @param env Nodejs environment handler.
     *
     * @return napi_ok undefining the SharedStorage class succeeded.
     */
    static napi_status undefine(napi_env env);

    /**
     * @brief  Constructor callback.
     *
     * @param env Nodejs environment handler.
     * @param info Constructor parameters.
     *
     * @return this object.
     */
    static napi_value constructor(napi_env env, napi_callback_info info);

    /**
     * @brief  Create JavaScript instance and wrap native SharedStorage.
     *
     * @param env Nodejs environment handler.
     * @param storage Native SharedStorage pointer.
     * @param[out] result JavaScript instance.
     *
     * @return napi_ok if creating the instance and wrapping the native
     * SharedStorage succeeded.
     */
    static napi_status createInstance(napi_env env, storage::SharedStorage* storage,
                                      napi_value* result);

    /**
     * @brief  Finalize callback for garbage collection.
     *
     * @param env Nodejs environment handler.
     * @param data Native SharedStorage pointer.
     * @param hint Contextual hint.
     */
    static void finalize(napi_env env, void* data, void* hint);

    /**
     * @brief  Retrieve native SharedStorage from JavaScript instance.
     *
     * @param env Nodejs environment handler
     * @param info Callback parameters.
     * @param[out] result Native SharedStorage pointer.
     *
     * @return napi_ok if retrieving the native SharedStorage succeeded.
     */
    static napi_status getStorage(napi_env env, napi_callback_info info,
                                  storage::SharedStorage** result);

    /**
     * @brief  Create a new storage.
     *
     * @param env Nodejs environment handler.
     * @param info Callback parameters.
     *
     * @return SharedStorage JavaScript instance.
     */
    static napi_value create(napi_env env, napi_callback_info info);

    /**
     * @brief  Open an existing storage.
     *
     * @param env  Nodejs environment handler.
     * @param info Callback parameters.
     *
     * @return SharedStorage JavaScript instance.
     */
    static napi_value open(napi_env env, napi_callback_info info);

    /**
     * @brief  Destroy an existing storage.
     *
     * @param env Nodejs environment handler.
     * @param info Callback parameters.
     *
     * @return boolean value, true if destroying the storage succeeded.
     */
    static napi_value destroy(napi_env env, napi_callback_info info);

    /**
     * @brief  Set an item.
     *
     * @param env Nodejs environment handler.
     * @param info Callback parameters.
     *
     * @return nullptr.
     */
    static napi_value setItem(napi_env env, napi_callback_info info);

    /**
     * @brief  Get an item.
     *
     * @param env Nodejs environment handler.
     * @param info Callback parameters.
     *
     * @return item value or nullptr if the item was not found.
     */
    static napi_value getItem(napi_env env, napi_callback_info info);

    /**
     * @brief  Remove an item.
     *
     * @param env Nodejs environment handler.
     * @param info Callback parameters.
     *
     * @return nullptr.
     */
    static napi_value removeItem(napi_env env, napi_callback_info info);

    /**
     * @brief  Remove all the items.
     *
     * @param env Nodejs environment handler.
     * @param info Callback parameters.
     *
     * @return nullptr.
     */
    static napi_value clear(napi_env env, napi_callback_info info);

    /**
     * @brief  Lock the storage for writing and reading items.
     *
     * @param env Nodejs environment handler.
     * @param info Callback parameters.
     *
     * @return nullptr.
     */
    static napi_value lock(napi_env env, napi_callback_info info);

    /**
     * @brief  Unlock the storage for writing and reading items.
     *
     * @param env Nodejs environment handler.
     * @param info Callback parameters.
     *
     * @return nullptr.
     */
    static napi_value unlock(napi_env env, napi_callback_info info);

    /**
     * @brief  Try to lock the storage for writing and reading items.
     *
     * @param env Nodejs environment handler.
     * @param info Callback parameters.
     *
     * @return boolean value, true if locking the storage succeeded.
     */
    static napi_value tryToLock(napi_env env, napi_callback_info info);

private:
    /**
     * @brief  Throw JavaScript exception according to the passed status.
     *
     * @param env Nodejs environment handler.
     * @param status Status for which throw the exception.
     *
     * @return napi_ok if throwing the exception succeeded.
     */
    static napi_status throw_error(napi_env env, unsigned int status);

    /**
     * @brief  Throw JavaScript exception according to the passed status.
     *
     * @param env Nodejs environment handler.
     * @param status Status for which throw the exception.
     * @param identifier String that identifies the storage or the item.
     *
     * @return napi_ok if throwing the exception succeeded.
     */
    static napi_status throw_error(napi_env env, unsigned int status,
                                   const std::string& identifier);

    static napi_ref m_constructor;
};

#endif /* JS_SHARED_STORAGE_H_ */
