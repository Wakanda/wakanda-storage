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
class ItemDestructor;

// Type defs.
template <class T>
using InterprocessAllocator =
    boost::interprocess::allocator<T, boost::interprocess::managed_shared_memory::segment_manager>;

using ItemInfoMapAllocator =
    InterprocessAllocator<std::pair<const boost::interprocess::string, ItemInfo>>;

using ItemInfoMap =
    boost::interprocess::map<boost::interprocess::string, ItemInfo,
                             std::less<boost::interprocess::string>, ItemInfoMapAllocator>;

using CharAllocator =
    boost::interprocess::allocator<char,
                                   boost::interprocess::managed_shared_memory::segment_manager>;

using StringValue = boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator>;

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
    eCannotReplaceItem = 7,
    eCannotConstructItem = 8,
    eCannotDestroyItem = 9
};

/**
 *  @brief  Information about shared items. For each item, the storage maintains its type and its
 * tag.
 */
class ItemInfo
{
public:
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
     * @param[out] status Status is eOk if creation succeeded
     * or eCannotCreateStorage if creation failed.
     *
     * @return Pointer to a new shared storage.
     */
    static SharedStorage* create(const std::string& name, const int64_t size, Status& status);

    /**
     * @brief  Only open a shared storage.
     *
     * @param name Name of the shared storage to open.
     * @param[out] status Status is eOk if opening succeeded
     * or eCannotOpenStorage if opening failed.
     *
     * @return Pointer to the opened shared storage.
     */
    static SharedStorage* open(const std::string& name, Status& status);

    /**
     * @brief  Destroy a shared storage.
     *
     * @param name Name of the shared storage to destroy.
     *
     * @return eOk if destruction succeeded
     * or eCannotDestroyStorage if destruction failed.
     */
    static Status destroy(const std::string& name);

    /**
     * @brief  Destroy a shared storage.
     *
     * @return eOk if destruction succeeded
     * or eCannotDestroyStorage if destruction failed.
     */
    Status destroy();

    /**
     * @brief  Insert a new item into the shared storage. Can throw error.
     *
     * @param key Key to identify the new item.
     * @param item Descriptor of the new item.
     * @tparam T Value type of the item.
     *
     * @return eOk if inserting the item succeeded
     * or eCannotConstructItem if inserting the item failed
     * or eCannotReplaceItem if an item with the same key cannot be overwritten.
     */
    template <class T> Status setItem(const std::string& key, const Item<T>& item);

    /**
     * @brief  Get an item already stored in the shared storage.
     *
     * @param key Key of the desired item.
     * @param consumer Consumer of the item.
     *
     * an item consumer must implement the template method "set" where T is the item value type:
     *   template<class T>
     *   void set(const std::string& key, Item<T>& item);
     *
     * @return eOk if the item was found
     * or eItemNotFound if the item doesn't exist
     * or eUnknownItemType if the item type is unsupported.
     */
    template <class C> Status getItem(const std::string& key, C& consumer);

    /**
     * @brief  Remove an item from the shared storage. Can throw error.
     *
     * @param key Key of the desired item.
     *
     * @return eOk if removing the item succeeded
     * or eCannotRemoveItem if removing the item failed
     * or eItemNotFound if the item doesn't exist.
     */
    Status removeItem(const std::string& key);

    /**
     * @brief  Clear the shared storage. Can throw error.
     *
     * @return eOk if the storage was successfully cleared
     * or eCannotRemoveItem if removing all items failed.
     */
    Status clear();

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
    friend class ItemDestructor;

    /**
     * @brief Constructor.
     *
     * @param name Name of the new shared storage.
     * @param size Size in bytes of the new shared storage.
     */
    SharedStorage(const std::string& name, const int64_t size);

    /**
     * @brief Constructor.
     *
     * @param name Name of the new shared storage.
     */
    SharedStorage(const std::string& name);

    /**
     *  @brief  Initialize the shared storage.
     */
    void initialize();

    /**
     * @brief  Get an item already stored in the shared storage.
     *
     * @param key Key of the desired item.
     * @param info Infos related to the item.
     * @param consumer Consumer of the item.
     *
     * an item consumer must implement the template method "set" where T is the item value type:
     *   template<class T>
     *   void set(const std::string& key, Item<T>& item);
     *
     * @return eOk if the item was found
     * or eItemNotFound if the item doesn't exist
     * or eUnknownItemType if the item type is unsupported.
     */
    template <class C> Status getItem(const std::string& key, const ItemInfo& info, C& consumer);

    /**
     * @brief  Create and add infos according to the passed item.
     *
     * @param key Key of the item.
     * @param item Item for which add the infos.
     * @tparam T Value type of the item.
     */
    template <class T> void addItemInfo(const boost::interprocess::string& key, const Item<T> item);

    /**
     * @brief  Update the infos which are related to the passed item.
     *
     * @param info Infos related to the item.
     * @param item Item for which update the infos.
     * @tparam T Value type of the item.
     */
    template <class T> void updateItemInfo(ItemInfo& info, const Item<T> item);

    /**
     * @brief  Construct the item into the memory segment and write the value.
     *
     * @param key Key of the item.
     * @param value Value of the item.
     *
     * @return eOk if instantiating the item succeeded
     * or eCannotConstructItem if instantiating the item failed.
     */
    template <class T> Status constructItemValue(const std::string& key, const T& value);

    /**
     * @brief  Destroy the item into the memory segment.
     *
     * @param key Key of the item.
     *
     * @return eOk if destroying the item succeeded
     * or eCannotDestroyItem if destroying the item failed.
     */
    template <class T> Status destroyItemValue(const std::string& key);

    /**
     * @brief  Update the item value into the memory segment.
     *
     * @param key Key of the item.
     * @param value Value of the item.
     *
     * @return eOk if updating the item succeeded
     * or eItemNotFound if the item doesn't exists.
     */
    template <class T> Status updateItemValue(const std::string& key, const T& value);

    /**
     * @brief  Read the  item value from the memory segment.
     *
     * @param key Key of the item.
     * @param[out] value Value of the item.
     *
     * @return eOk if reading the item succeeded
     * or eItemNotFound if the item doesn't exists.
     */
    template <class T> Status readItemValue(const std::string& key, T& value);

    std::string m_name;
    boost::interprocess::managed_shared_memory m_segment;
    boost::interprocess::interprocess_recursive_mutex* m_mutex;
    ItemInfoMap* m_itemInfoMap;
};


/**
 * @brief  Item destructor compliant with item consumer specifications.
 */
class ItemDestructor
{
public:
    /**
     * @brief Deleted destructor.
     */
    ItemDestructor() = delete;

    /**
     * @brief  Constructor.
     *
     * @param storage Storage in which destroy the item value.
     */
    ItemDestructor(SharedStorage* storage) : m_storage(storage), m_status(eOk) {}

    /**
     * @brief  Get the status of item value destruction.
     *
     * @return eOk if destroying the item succeeded
     * or eCannotDestroyItem if destroying the item failed.
     */
    Status getStatus() const { return m_status; }

    /**
     * @brief  Destroy the item value.
     *
     * @param key Key of the item.
     * @param item Item description.
     * @tparam T Value type of the item.
     */
    template <class T> void set(const std::string& key, Item<T>& item)
    {
        m_status = m_storage->destroyItemValue<T>(key);
    }

private:
    SharedStorage* m_storage;
    Status m_status;
};


template <class T> inline Status SharedStorage::setItem(const std::string& key, const Item<T>& item)
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
            ItemDestructor itemDestructor(this);
            status = getItem<ItemDestructor>(key, info->second, itemDestructor);
            if (status == eOk)
            {
                status = itemDestructor.getStatus();
            }
            if ((status == eOk) || (status == eItemNotFound))
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
            status = updateItemValue<T>(key, item.getValue());
            if (status == eItemNotFound)
            {
                (*m_itemInfoMap).erase(info);
                constructNewValue = true;
            }
            else if (status == eOk)
            {
                updateItemInfo<T>(info->second, item);
            }
        }
    }
    else
    {
        // the item does not exist, create a new one
        constructNewValue = true;
    }

    if (constructNewValue)
    {
        status = constructItemValue<T>(key, item.getValue());
        if (status == eOk)
        {
            addItemInfo<T>(ipStrKey, item);
        }
    }

    return status;
}

template <class C> inline Status SharedStorage::getItem(const std::string& key, C& consumer)
{
    Status status = eOk;
    std::string strKey;
    boost::interprocess::string ipStrKey(key.c_str());
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex> lock(
        *m_mutex);

    ItemInfoMap::iterator info = m_itemInfoMap->find(ipStrKey);
    if (info != m_itemInfoMap->end())
    {
        status = getItem<C>(key, info->second, consumer);
    }
    else
    {
        status = eItemNotFound;
    }

    return status;
}

template <class C>
inline Status SharedStorage::getItem(const std::string& key, const ItemInfo& info, C& consumer)
{
    Status status = eOk;
    std::string tag;
    info.getTag(tag);

    switch (info.getType())
    {
    case eBool:
    {
        bool value = false;
        status = readItemValue<bool>(key, value);
        if (status == eOk)
        {
            Item<bool> item(value, tag);
            consumer.template set<bool>(key, item);
        }
        break;
    }

    case eDouble:
    {
        double value = 0.0;
        status = readItemValue<double>(key, value);
        if (status == eOk)
        {
            Item<double> item(value, tag);
            consumer.template set<double>(key, item);
        }
        break;
    }

    case eString:
    {
        std::string value;
        status = readItemValue<std::string>(key, value);
        if (status == eOk)
        {
            Item<std::string> item(value, tag);
            consumer.template set<std::string>(key, item);
        }
        break;
    }

    default:
        status = eUnknownItemType;
        break;
    }

    return status;
}

template <class T>
inline void SharedStorage::addItemInfo(const boost::interprocess::string& key, const Item<T> item)
{
    ItemInfo info(item.getType(), item.getTag(),
                  InterprocessAllocator<char>(m_segment.get_segment_manager()));
    (*m_itemInfoMap).insert(std::pair<const boost::interprocess::string, ItemInfo>(key, info));
}

template <class T> inline void SharedStorage::updateItemInfo(ItemInfo& info, const Item<T> item)
{
    info.setTag(item.getTag());
}

template <class T>
inline Status SharedStorage::constructItemValue(const std::string& key, const T& value)
{
    T* obj = nullptr;
    try
    {
        obj = m_segment.construct<T>(key.c_str())(value);
    }
    catch (const std::exception&)
    {
    }
    return (obj != nullptr) ? eOk : eCannotConstructItem;
}

template <class T> inline Status SharedStorage::destroyItemValue(const std::string& key)
{
    bool done = false;
    try
    {
        done = m_segment.destroy<T>(key.c_str());
    }
    catch (const std::exception&)
    {
    }
    return (done) ? eOk : eCannotDestroyItem;
}

template <class T>
inline Status SharedStorage::updateItemValue(const std::string& key, const T& value)
{
    T* localValue = m_segment.find<T>(key.c_str()).first;
    if (localValue != nullptr)
    {
        *localValue = value;
        return eOk;
    }
    return eItemNotFound;
}

template <class T> Status SharedStorage::readItemValue(const std::string& key, T& value)
{
    const T* localValue = m_segment.find<T>(key.c_str()).first;
    if (localValue != nullptr)
    {
        value = *localValue;
        return eOk;
    }
    return eItemNotFound;
}


/**
 * @brief  String values specializations.
 */

template <>
inline Status SharedStorage::constructItemValue<std::string>(const std::string& key,
                                                             const std::string& value)
{
    StringValue* obj = nullptr;
    try
    {
        obj = m_segment.construct<StringValue>(key.c_str())(value.c_str(),
                                                            m_segment.get_segment_manager());
    }
    catch (const std::exception&)
    {
    }
    return (obj != nullptr) ? eOk : eCannotConstructItem;
}

template <> inline Status SharedStorage::destroyItemValue<std::string>(const std::string& key)
{
    bool done = false;
    try
    {
        done = m_segment.destroy<StringValue>(key.c_str());
    }
    catch (const std::exception&)
    {
    }
    return (done) ? eOk : eCannotDestroyItem;
}

template <>
inline Status SharedStorage::updateItemValue<std::string>(const std::string& key,
                                                          const std::string& value)
{
    StringValue* localValue = m_segment.find<StringValue>(key.c_str()).first;
    if (localValue != nullptr)
    {
        localValue->assign(value.c_str());
        return eOk;
    }
    return eItemNotFound;
}

template <>
inline Status SharedStorage::readItemValue<std::string>(const std::string& key, std::string& value)
{
    const StringValue* localValue = m_segment.find<StringValue>(key.c_str()).first;
    if (localValue != nullptr)
    {
        value.assign(localValue->c_str());
        return eOk;
    }
    return eItemNotFound;
}


} // namespace storage

#endif /* SHARED_STORAGE_H_ */
