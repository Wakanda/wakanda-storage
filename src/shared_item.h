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
    eString = 3
};


/**
 * @brief  Item descriptor class.
 * This class describes a shared item. Item instance is used as parameter to set an item into the
 * storage and get an item from the storage.
 */
template <class T> class Item
{
public:
    /**
     * @brief  Default constructor.
     */
    Item() : m_type(eNone) {}

    /**
     * @brief  Constructor.
     *
     * @param tag Tag associated to the item.
     */
    Item(const std::string& tag) : m_type(eNone), m_tag(tag) {}

    /**
     * @brief  Constructor.
     *
     * @param value Value of the item.
     * @param tag Tag associated to the item.
     */
    Item(const T& value, const std::string& tag) : m_value(value), m_type(eNone), m_tag() {}

    /**
     * @brief  Get the value of the item.
     *
     * @return Value of the item.
     */
    const T& getValue() const { return m_value; }

    /**
     * @brief  Set the value of the item.
     *
     * @param value Value of the item.
     */
    void setValue(const T& value) { m_value = value; }

    /**
     * @brief  Get the type of the item.
     *
     * @return Type of the item.
     */
    const ItemType& getType() const { return m_type; }

    /**
     * @brief  Get the tag associated to the item.
     *
     * @return Tag associated to the item.
     */
    const std::string& getTag() const { return m_tag; }

    /**
     * @brief  Set the tag associated to the item.
     *
     * @param tag Tag associated to the item.
     */
    void setTag(const std::string& tag) { m_tag = tag; }

protected:
    T m_value;
    ItemType m_type;
    std::string m_tag;
};


/**
 * @brief  Bool value specializations.
 */
template <> inline Item<bool>::Item() : m_value(false), m_type(eBool) {}

template <>
inline Item<bool>::Item(const std::string& tag) : m_value(false), m_type(eBool), m_tag(tag)
{
}

template <>
inline Item<bool>::Item(const bool& value, const std::string& tag)
: m_value(value), m_type(eBool), m_tag(tag)
{
}

/**
 * @brief  Double values specializations.
 */
template <> inline Item<double>::Item() : m_value(0.0), m_type(eDouble) {}

template <>
inline Item<double>::Item(const std::string& tag) : m_value(0.0), m_type(eDouble), m_tag(tag)
{
}

template <>
inline Item<double>::Item(const double& value, const std::string& tag)
: m_value(value), m_type(eDouble), m_tag(tag)
{
}

/**
 * @brief  String values specializations.
 */
template <> inline Item<std::string>::Item() : m_type(eString) {}

template <> inline Item<std::string>::Item(const std::string& tag) : m_type(eString), m_tag(tag) {}

template <>
inline Item<std::string>::Item(const std::string& value, const std::string& tag)
: m_value(value), m_type(eString), m_tag(tag)
{
}

} // namespace storage

#endif /* SHARED_STORAGE_H_ */