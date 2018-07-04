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
#include "shared_item.h"
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>

namespace storage
{

// Declarations.
class ItemInfo;

// Type defs.
template <class T>
using InterprocessAllocator =
    boost::interprocess::allocator<T, boost::interprocess::managed_shared_memory::segment_manager>;

using ItemInfoMapAllocator =
    InterprocessAllocator<std::pair<const boost::interprocess::string, ItemInfo>>;

using ItemInfoMap =
    boost::interprocess::map<boost::interprocess::string, ItemInfo,
                             std::less<boost::interprocess::string>, ItemInfoMapAllocator>;

/**
 *  @brief  Status / Error codes.
 */
enum Status
{
    eOk = 0,
    eCannotCreateStorage = 1,
    eCannotOpenStorage = 2,
    eCannotDestroyStorage = 3,
    eUnknownItemType = 4,
    eItemNotFound = 5,
    eCannotRemoveItem = 6,
    eCannotReplaceItem = 7
};

/**
 *  @brief  Information about shared items. For each item, the storage maintains its type and its
 * tag.
 */
class ItemInfo
{
public:
    // Type defs.
    using StringValue = boost::interprocess::basic_string<char, std::char_traits<char>,
                                                          InterprocessAllocator<char>>;

    /**
     * @brief  Deleted constructor.
     */
    ItemInfo() = delete;

    /**
     * @brief  Constructor.
     *
     * @param type Type of the shared item.
     * @param tag Tag associated to the shared item.
     */
    ItemInfo(ItemType type, const std::string& tag, const InterprocessAllocator<char>& allocator)
    : m_type(type), m_tag(tag.c_str(), allocator)
    {
    }

    /**
     * @brief  Get the type of the shared item.
     *
     * @return Type of the shared item.
     */
    ItemType getType() const { return m_type; }

    /**
     * @brief  Set the tag associated to the shared item.
     *
     * @param tag Tag associated to the shared item.
     */
    void setTag(const std::string& tag) { m_tag.assign(tag.c_str()); }

    /**
     * @brief  Get the tag associated to the shared item.
     *
     * @param[out] tag Tag associated to the shared item.
     */
    void getTag(std::string& tag) const { tag.assign(m_tag.c_str()); }

private:
    ItemType m_type;
    StringValue m_tag;
};

/**
 * @brief  native shared storage implementation
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
     * @param name Name of the new shared storage.
     * @param size Size in bytes of the new shared storage.
     * @param[out] status Status is eOk if creation succeeded.
     *
     * @return Pointer to a new shared storage.
     */
    static SharedStorage* create(const char* name, const int64_t size, Status& status);

    /**
     * @brief  Only open a shared storage.
     *
     * @param name Name of the shared storage to open.
     * @param[out] status Status is eOk if opening succeeded.
     *
     * @return Pointer to the opened shared storage.
     */
    static SharedStorage* open(const char* name, Status& status);

    /**
     * @brief  Destroy a shared storage.
     *
     * @param name Name of the shared storage to destroy.
     *
     * @return eOk if destruction succeeded.
     */
    static Status destroy(const char* name);

    /**
     * @brief  Insert a new item into the shared storage. Can throw error.
     *
     * @param key Key to identify the new item.
     * @param item Descriptor of the new item.
     *
     * @return eOk if inserting the item succeeded.
     */
    Status setItem(const std::string& key, const SharedItem& item);

    /**
     * @brief  Get an item already stored in the shared storage.
     *
     * @param key Key of the desired item.
     * @param[out] item Descriptor of the item.
     *
     * @return eOk if the item was found.
     */
    Status getItem(const std::string& key, std::unique_ptr<SharedItem>& item);

    /**
     * @brief  Remove an item from the shared storage. Can throw error.
     *
     * @param key Key of the desired item.
     *
     * @return eOk if removing the item succeeded.
     */
    Status removeItem(const std::string& key);

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

    std::string m_name;
    boost::interprocess::managed_shared_memory m_segment;
    boost::interprocess::interprocess_recursive_mutex* m_mutex;
    ItemInfoMap* m_itemInfoMap;
};

} // namespace storage

#endif /* SHARED_STORAGE_H_ */
