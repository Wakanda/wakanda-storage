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
 * \file    shared_item.h
 */

#ifndef SHARED_ITEM_H_
#define SHARED_ITEM_H_

#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <string>

namespace storage
{

/**
 *  @brief  Item types.
 */
enum ItemType
{
    eNone = 0,
    eBool = 1,
    eDouble = 2,
    eString = 3,
    eNull = 4
};

/**
 * @brief  Shared item descriptor class.
 * This class doesn't contain any value and should be only used as parameter to set an item into the
 * storage and get an item from the storage.
 */
class SharedItem
{
public:
    /**
     * @brief  Deleted constructor.
     */
    SharedItem() = delete;

    /**
     * @brief  Constructor.
     *
     * @param type Type of the shared item.
     * @param tag Tag associated to the shared item.
     */

    SharedItem(const ItemType& type, const std::string& tag) : m_type(type), m_tag(tag) {}

    /**
     * @brief  Get the type of the shared item.
     *
     * @return Type of the shared item.
     */
    ItemType getType() const { return m_type; }

    /**
     * @brief  Get the tag associated to the shared item.
     *
     * @return Tag associated to the shared item.
     */
    std::string getTag() const { return m_tag; }

    /**
     * @brief  Get shared item bool value.
     *
     * @return Bool value.
     */
    virtual bool getBool() const { return false; }

    /**
     * @brief  Get shared item double value.
     *
     * @return Double value.
     */
    virtual double getDouble() const { return 0.0; }

    /**
     * @brief  Get shared item string value.
     *
     * @param[out] value String value.
     */
    virtual void getString(std::string& value) const {}

    /**
     * @brief  Construct the shared item into the given memory segment and write the value.
     *
     * @param segment Memory segment in which construct the item.
     * @param key Key of the item.
     */
    virtual void construct(boost::interprocess::managed_shared_memory& segment,
                           const std::string& key) const
    {
    }

    /**
     * @brief  Destroy the shared item into the given memory segment.
     *
     * @param segment Memory segment in which destroy the item.
     * @param key Key of the item.
     *
     * @return true if destroying the item succeeded.
     */
    virtual bool destroy(boost::interprocess::managed_shared_memory& segment,
                         const std::string& key) const
    {
        return true;
    }

    /**
     * @brief  Write the shared item value into the given memory segment.
     *
     * @param segment Memory segment in which write the value.
     * @param key Key of the item.
     */
    virtual void write(boost::interprocess::managed_shared_memory& segment,
                       const std::string& key) const
    {
    }

    /**
     * @brief  Read the shared item value from the given memory segment.
     *
     * @param segment Memory segment from which read the value.
     * @param key Key of the item.
     */
    virtual void read(boost::interprocess::managed_shared_memory& segment, const std::string& key)
    {
    }

protected:
    ItemType m_type;
    std::string m_tag;
};


/**
 * @brief  Create a shared item instance from a given type.
 *
 * @param type Type of the new shared item.
 * @param tag Tag associated to the new shared item.
 * @param[out] item Created shared item.
 */
void createSharedItem(const ItemType type, const std::string& tag,
                      std::unique_ptr<SharedItem>& item);


/**
 *  @brief Shared item template class.
 */
template <class T> class SharedItemValue : public SharedItem
{
public:
    // Type defs.
    using CharAllocator =
        boost::interprocess::allocator<char,
                                       boost::interprocess::managed_shared_memory::segment_manager>;

    using StringValue =
        boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator>;

    /**
     * @brief  Deleted constructor.
     */
    SharedItemValue() = delete;

    /**
     * @brief  Constructor.
     *
     * @param tag Tag associated to the shared item.
     */
    SharedItemValue(const std::string& tag) : SharedItem(eNone, tag) {}

    /**
     * @brief  Constructor.
     *
     * @param value Value of the shared item.
     * @param tag Tag associated to the shared item.
     */
    SharedItemValue(const T& value, const std::string& tag) : SharedItem(eNone, tag) {}

    /**
     * @brief  Get shared item bool value. Generic implementation.
     *
     * @return Bool value.
     */
    bool getBool() const override { return false; }

    /**
     * @brief  Get shared item double value. Generic implementation.
     *
     * @return Double value.
     */
    double getDouble() const override { return 0.0; }

    /**
     * @brief  Get shared item string value. Generic implementation.
     *
     * @param[out] value String value.
     */
    void getString(std::string& value) const override {}

    /**
     * @brief  Construct the shared item into the given memory segment and write the value. Generic
     * implementation.
     *
     * @param segment Memory segment in which construct the item.
     * @param key Key of the item.
     */
    void construct(boost::interprocess::managed_shared_memory& segment,
                   const std::string& key) const override
    {
        segment.construct<T>(key.c_str())(m_value);
    }

    /**
     * @brief  Destroy the shared item into the given memory segment. Generic implementation.
     *
     * @param segment Memory segment in which destroy the item.
     * @param key Key of the item.
     *
     * @return true if destroying the item succeeded.
     */
    bool destroy(boost::interprocess::managed_shared_memory& segment,
                 const std::string& key) const override
    {
        return segment.destroy<T>(key.c_str());
    }

    /**
     * @brief  Write the shared item value into the given memory segment. Generic implementation.
     *
     * @param segment Memory segment in which write the value.
     * @param key Key of the item.
     */
    void write(boost::interprocess::managed_shared_memory& segment,
               const std::string& key) const override
    {
        T* localValue = segment.find<T>(key.c_str()).first;
        *localValue = m_value;
    }

    /**
     * @brief  Read the shared item value from the given memory segment. Generic implementation.
     *
     * @param segment Memory segment from which read the value.
     * @param key Key of the item.
     */
    void read(boost::interprocess::managed_shared_memory& segment, const std::string& key) override
    {
        m_value = *segment.find<T>(key.c_str()).first;
    }

protected:
    T m_value;
};

/**
 * @brief  Bool value specializations.
 */
template <>
SharedItemValue<bool>::SharedItemValue(const std::string& tag)
: SharedItem(eBool, tag), m_value(false)
{
}

template <>
SharedItemValue<bool>::SharedItemValue(const bool& value, const std::string& tag)
: SharedItem(eBool, tag), m_value(value)
{
}

template <> bool SharedItemValue<bool>::getBool() const { return m_value; }

/**
 * @brief  Double values specializations.
 */
template <>
SharedItemValue<double>::SharedItemValue(const std::string& tag)
: SharedItem(eDouble, tag), m_value(0.0)
{
}

template <>
SharedItemValue<double>::SharedItemValue(const double& value, const std::string& tag)
: SharedItem(eDouble, tag), m_value(value)
{
}

template <> double SharedItemValue<double>::getDouble() const { return m_value; }

/**
 * @brief  String values specializations.
 */
template <>
SharedItemValue<std::string>::SharedItemValue(const std::string& tag) : SharedItem(eString, tag)
{
}

template <>
SharedItemValue<std::string>::SharedItemValue(const std::string& value, const std::string& tag)
: SharedItem(eString, tag), m_value(value)
{
}

template <> void SharedItemValue<std::string>::getString(std::string& value) const
{
    value.assign(m_value.c_str());
}

template <>
void SharedItemValue<std::string>::construct(boost::interprocess::managed_shared_memory& segment,
                                             const std::string& key) const
{
    segment.construct<StringValue>(key.c_str())(m_value.c_str(), segment.get_segment_manager());
}

template <>
bool SharedItemValue<std::string>::destroy(boost::interprocess::managed_shared_memory& segment,
                                           const std::string& key) const
{
    return segment.destroy<StringValue>(key.c_str());
}

template <>
void SharedItemValue<std::string>::write(boost::interprocess::managed_shared_memory& segment,
                                         const std::string& key) const
{
    StringValue* localValue = segment.find<StringValue>(key.c_str()).first;
    localValue->assign(m_value.c_str());
}

template <>
void SharedItemValue<std::string>::read(boost::interprocess::managed_shared_memory& segment,
                                        const std::string& key)
{
    StringValue* localValue = segment.find<StringValue>(key.c_str()).first;
    m_value.assign(localValue->c_str());
}


/**
 * @brief  Shared item type defs.
 */
using SharedItemBool = SharedItemValue<bool>;
using SharedItemDouble = SharedItemValue<double>;
using SharedItemString = SharedItemValue<std::string>;


/**
 * @brief  Shared item implementation for null values.
 */
class SharedItemNull : public SharedItem
{
public:
    /**
     * @brief  Deleted constructor.
     */
    SharedItemNull() = delete;

    /**
     * @brief  Constructor
     *
     * @param tag Tag associated to the shared item.
     */
    SharedItemNull(const std::string& tag) : SharedItem(eNull, tag) {}
};

} // namespace storage

#endif /* SHARED_STORAGE_H_ */