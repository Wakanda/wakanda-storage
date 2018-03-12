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
 * \file    shared_storage.h
 */

#ifndef SHARED_STORAGE_H_
#define SHARED_STORAGE_H_


// Includes.
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include <node_api.h>

// Declarations.
struct ItemInfo;

// Type defs.
template <class T>
using InterprocessAllocator =
    boost::interprocess::allocator<T, boost::interprocess::managed_shared_memory::segment_manager>;

using ItemInfoMapAllocator =
    InterprocessAllocator<std::pair<const boost::interprocess::string, ItemInfo>>;

using ItemInfoMap =
    boost::interprocess::map<boost::interprocess::string, ItemInfo,
                             std::less<boost::interprocess::string>, ItemInfoMapAllocator>;

using StringValue =
    boost::interprocess::basic_string<char, std::char_traits<char>, InterprocessAllocator<char>>;

/**
 *  @brief Information about stored items.
 */
struct ItemInfo
{
    napi_valuetype m_itemType;
};



/**
 * @brief  nativeshared storage implementation
 */
class SharedStorage
{
public:
    /**
     * @brief  Deleted constructor.
     */
    SharedStorage() = delete;

    /**
     * @brief  Destructor.
     */
    ~SharedStorage();

    /**
     * @brief  Create a shared storage.
     *
     * @param env Nodejs environment handler.
     * @param name Name of the new shared storage.
     * @param size Size in bytes of the new shared storage.
     *
     * @return Pointer to a new shared storage.
     */
    static SharedStorage* create(napi_env env, const char* name, const int64_t size);

    /**
     * @brief  Only open a shared storage.
     *
     * @param env Nodejs environment handler.
     * @param name Name of the shared storage to open.
     *
     * @return Pointer to the opened shared storage.
     */
    static SharedStorage* open(napi_env env, const char* name);

    /**
     * @brief  Destroy a shared storage.
     *
     * @param env Nodejs environment handler.
     * @param name Name of the shared storage to destroy.
     *
     * @return true if destruction succeeded.
     */
    static bool destroy(napi_env env, const char* name);

    /**
     * @brief  Insert a new item into the shared storage. Can throw error.
     *
     * @param env Nodejs environment handler.
     * @param key Key to identify the new item.
     * @param value Value of the new item.
     *
     * @return napi_ok if resolving the key and the value succeeded.
     */
    napi_status setItem(napi_env env, napi_value key, napi_value value);

    /**
     * @brief  Get an item already stored in the shared storage.
     *
     * @param env Nodejs environment handler.
     * @param key Key of the desired item.
     * @param[out] value Value of the item.
     *
     * @return napi_ok if resolving the key and populating the value succeeded.
     */
    napi_status getItem(napi_env env, napi_value key, napi_value* value);

    /**
     * @brief  Remove an item from the shared storage. Can throw error.
     *
     * @param env Nodejs environment handler.
     * @param key Key of the desired item.
     *
     * @return napi_ok if resolving the key succeeded.
     */
    napi_status removeItem(napi_env env, napi_value key);

    /**
     * @brief  Clear the shared storage. Can throw error.
     */
    void clear();

    /**
     * @brief  Lock writing on the shared storage.
     */
    void lock();

    /**
     * @brief  Unlock writing on the shared storage.
     */
    void unlock();

    /**
     * @brief Try to lock writing on the shared storage.
     *
     * @return true if locking succeeded.
     */
    bool tryToLock();

private:
    /**
     * @brief Constructor.
     *
     * @param name Name of the new shared storage.
     * @param size Size in bytes of the new shared storage.
     */
    SharedStorage(const char* name, const int64_t size);

    /**
     * @brief Constructor.
     *
     * @param name Name of the new shared storage.
     */
    SharedStorage(const char* name);

    /**
     *  @brief  Initialize the shared storage.
     */
    void initialize();

    /**
     * @brief  Destroy only the value of a given stored item.
     *
     * @param key Key of the desired item.
     * @param type Type of the item's value to destroy.
     *
     * @return true if value destruction succeeded.
     */
    bool destroyItemValue(const char* key, const napi_valuetype type);

    std::string m_name;
    boost::interprocess::managed_shared_memory m_segment;
    boost::interprocess::interprocess_recursive_mutex* m_mutex;
    ItemInfoMap* m_itemInfoMap;
};



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
    static napi_status createInstance(napi_env env, SharedStorage* storage, napi_value* result);

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
    static napi_status getStorage(napi_env env, napi_callback_info info, SharedStorage** result);

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
    static napi_ref m_constructor;
};

#endif /* SHARED_STORAGE_H_ */
