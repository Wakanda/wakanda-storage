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
 * \file    basis.cpp
 */

// Local includes.
#include "catch.hpp"
#include "shared_storage.h"
#include <string>

const int64_t kSize = 1024 * 1024;


class StorageSetter
{
public:
    StorageSetter() = delete;

    StorageSetter(const char* name)
    {
        m_name.assign(name);
        storage::Status status = storage::eOk;
        m_storage.reset(storage::SharedStorage::create(name, kSize, status));
    }

    ~StorageSetter() { storage::SharedStorage::destroy(m_name.c_str()); }

    storage::SharedStorage* get() const { return m_storage.get(); }

private:
    std::string m_name;
    std::unique_ptr<storage::SharedStorage> m_storage;
};


TEST_CASE("Shared storage can be created, opened and destroyed")
{
    std::string tmpStorageName("temporary-storage");

    SECTION("Creating a shared storage")
    {
        storage::Status status = storage::eOk;
        std::unique_ptr<storage::SharedStorage> localStorage(
            storage::SharedStorage::create(tmpStorageName.c_str(), kSize, status));
        REQUIRE(status == storage::eOk);
        REQUIRE(localStorage != nullptr);
    }

    SECTION("Opening a shared storage")
    {
        storage::Status status = storage::eOk;
        std::unique_ptr<storage::SharedStorage> localStorage(
            storage::SharedStorage::open(tmpStorageName.c_str(), status));
        CHECK(status == storage::eOk);
        CHECK(localStorage != nullptr);
    }

    SECTION("Destroying a shared storage")
    {
        storage::Status status = storage::SharedStorage::destroy(tmpStorageName.c_str());
        REQUIRE(status == storage::eOk);
    }
}


TEST_CASE("Bool item can be created, read, updated and removed")
{
    StorageSetter setter("bool-storage");
    std::string key("bool-item"), tag;
    bool initialValue = false;
    storage::Status status = setter.get()->setItem(key, storage::SharedItemBool(initialValue, tag));

    SECTION("Creating a bool item")
    {
        REQUIRE(status == storage::eOk);
    }

    SECTION("Reading a bool item")
    {
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eOk);
        CHECK(item != nullptr);
        if (item != nullptr)
        {
            CHECK(item->getType() == storage::eBool);
            CHECK(item->getBool() == initialValue);
        }
    }

    SECTION("Updating a bool item")
    {
        bool newValue = true;
        storage::Status status = setter.get()->setItem(key, storage::SharedItemBool(newValue, tag));
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eOk);
        CHECK(item != nullptr);
        if (item != nullptr)
        {
            CHECK(item->getBool() == newValue);
        }
    }
	
	SECTION("Override a bool item with string value")
    {
        std::string newStringValue("this is not a boolean value");
        std::string str_tmp;
        storage::Status status = setter.get()->setItem(key, storage::SharedItemString(newStringValue, tag));
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eOk);
        CHECK(item != nullptr);
        if (item != nullptr)
        {
            item->getString(str_tmp);
            CHECK(!str_tmp.empty() == true);
            CHECK(str_tmp == newStringValue);
        }
    }

    SECTION("Override a bool item with a double value")
    {
        double double_value(3.14);
        storage::Status status = setter.get()->setItem(key, storage::SharedItemDouble(double_value, tag));
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eOk);
        CHECK(item != nullptr);
        if (item != nullptr)
        {
            CHECK(item->getDouble() == 3.14);
        }
    }

    SECTION("Removing a bool item")
    {
        status = setter.get()->removeItem(key);
        CHECK(status == storage::eOk);
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eItemNotFound);
        CHECK(item == nullptr);
    }
}


TEST_CASE("Double item can be created, read, updated and removed")
{
    StorageSetter setter("double-storage");
    std::string key("double-item"), tag;
    double initialValue = 123.456;
    storage::Status status =
        setter.get()->setItem(key, storage::SharedItemDouble(initialValue, tag));

    SECTION("Creating a double item")
    {
        REQUIRE(status == storage::eOk);
    }

    SECTION("Reading a double item")
    {
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eOk);
        CHECK(item != nullptr);
        if (item != nullptr)
        {
            CHECK(item->getType() == storage::eDouble);
            CHECK(item->getDouble() == initialValue);
        }
    }

    SECTION("Updating a double item")
    {
        double newValue = 456.789;
        storage::Status status =
            setter.get()->setItem(key, storage::SharedItemDouble(newValue, tag));
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eOk);
        CHECK(item != nullptr);
        if (item != nullptr)
        {
            CHECK(item->getType() == storage::eDouble);
            CHECK(item->getDouble() == newValue);
        }
    }

    SECTION("Override a double with a string value")
    {
        std::string str_value ("this is not a double value");
        std::string tmp_str;
        storage::Status status =
            setter.get()->setItem(key, storage::SharedItemString(str_value, tag));
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eOk);
        CHECK(item != nullptr);
        if (item != nullptr)
        {
            CHECK(item->getType() == storage::eString);
            item->getString(tmp_str);
            CHECK(tmp_str == str_value);
        }
    }

    SECTION("Removing a double item")
    {
        status = setter.get()->removeItem(key);
        CHECK(status == storage::eOk);
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eItemNotFound);
        CHECK(item == nullptr);
    }
}


TEST_CASE("String item can be created, read, updated and removed")
{
    StorageSetter setter("string-storage");
    std::string key("string-item"), tag;
    std::string initialValue(
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit.Etiam at leo vel tortor tristique "
        "faucibus et ullamcorper erat.Nunc sed aliquam mauris.Aenean at ipsum vel quam dignissim "
        "imperdiet.Curabitur a elit ut augue congue viverra.Vivamus ac leo consequat, consequat "
        "turpis a, porttitor ipsum.Pellentesque turpis erat, lobortis ut ipsum in, vulputate "
        "laoreet arcu.Mauris a nunc mauris.Nunc tincidunt nunc ac diam tempus, eu hendrerit lectus "
        "semper.Maecenas orci metus, viverra et vestibulum quis, eleifend eget enim.Quisque "
        "laoreet risus eu dui vulputate gravida.Aenean ullamcorper elit eget tellus mattis, sit "
        "amet vehicula urna vulputate.Praesent pharetra, sapien sit amet condimentum vestibulum, "
        "justo purus hendrerit ipsum, nec molestie massa velit ac metus.Aenean quis orci et diam "
        "commodo aliquet.Mauris id pretium velit.Duis libero justo, gravida id rhoncus at, "
        "consectetur porta nulla.Nullam eget viverra mi.");
    storage::Status status =
        setter.get()->setItem(key, storage::SharedItemString(initialValue, tag));

    SECTION("Creating a string item")
    {
        REQUIRE(status == storage::eOk);
    }

    SECTION("Reading a string item")
    {
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eOk);
        CHECK(item != nullptr);
        if (item != nullptr)
        {
            CHECK(item->getType() == storage::eString);
            std::string stringValue;
            item->getString(stringValue);
            CHECK(stringValue == initialValue);
        }
    }

    SECTION("Updating a string item")
    {
        std::string newValue(
            "Pellentesque vulputate felis bibendum dolor euismod, quis vulputate est tristique. "
            "Pellentesque bibendum orci eget libero cursus iaculis. In hac habitasse platea "
            "dictumst. Pellentesque volutpat urna non lorem bibendum tincidunt. Sed gravida erat "
            "nibh, quis convallis turpis ultricies et. Interdum et malesuada fames ac ante ipsum "
            "primis in faucibus. Donec eget neque vel felis accumsan porta. Ut facilisis, lectus "
            "ut tristique eleifend, felis lacus malesuada nisi, non pharetra sem nisi quis lorem. "
            "Maecenas pretium enim ut rhoncus commodo. Mauris eget tincidunt metus, a venenatis "
            "leo. Duis enim eros, hendrerit et lectus rhoncus, dictum convallis nisi. Donec "
            "porttitor congue ipsum, quis mollis sapien malesuada sit amet. Ut blandit semper "
            "tortor, non interdum sem. Sed porta neque luctus ultrices luctus. In rhoncus vel "
            "turpis a vehicula. Ut ornare ligula vestibulum nulla suscipit, et bibendum massa "
            "dapibus.");
        storage::Status status =
            setter.get()->setItem(key, storage::SharedItemString(newValue, tag));
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eOk);
        CHECK(item != nullptr);
        if (item != nullptr)
        {
            CHECK(item->getType() == storage::eString);
            std::string stringValue;
            item->getString(stringValue);
            CHECK(stringValue == newValue);
        }
    }

    SECTION("Override a string with a double value")
    {
        double double_value = 3.14;
        storage::Status status =
            setter.get()->setItem(key, storage::SharedItemDouble(3.14, tag));
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eOk);
        CHECK(item != nullptr);
        if (item != nullptr)
        {
            CHECK(item->getType() == storage::eDouble);
            CHECK(item->getDouble() == double_value);
        }
    }

    SECTION("Removing a string item")
    {
        status = setter.get()->removeItem(key);
        CHECK(status == storage::eOk);
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eItemNotFound);
        CHECK(item == nullptr);
    }
}


TEST_CASE("Null item can be created, read, and removed")
{
    StorageSetter setter("null-storage");
    std::string key("null-item"), tag;
    storage::Status status = setter.get()->setItem(key, storage::SharedItemNull(tag));

    SECTION("Creating a null item")
    {
        REQUIRE(status == storage::eOk);
    }

    SECTION("Reading a null item")
    {
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eOk);
        CHECK(item != nullptr);
        if (item != nullptr)
        {
            CHECK(item->getType() == storage::eNull);
        }
    }

    SECTION("Removing a null item")
    {
        status = setter.get()->removeItem(key);
        CHECK(status == storage::eOk);
        std::unique_ptr<storage::SharedItem> item;
        status = setter.get()->getItem(key, item);
        CHECK(status == storage::eItemNotFound);
        CHECK(item == nullptr);
    }
}