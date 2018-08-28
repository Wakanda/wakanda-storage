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
#include "common_process.h"
#include "shared_storage.h"
#include <boost/filesystem.hpp>
#include <boost/process/child.hpp>
#include <chrono>
#include <future>
#include <string>

const int64_t kSize = 1024 * 1024;



class ItemConsumer
{
public:
    ItemConsumer() : m_type(storage::eNone), m_bool(false), m_double(0){};

    storage::ItemType getType() const { return m_type; }
    operator bool() const { return m_bool; }
    operator double() const { return m_double; }
    operator std::string() const { return m_string; }

    template <class T> void set(const std::string& key, storage::Item<T>& item) {}

    storage::ItemType m_type;
    bool m_bool;
    double m_double;
    std::string m_string;
};

template <> void ItemConsumer::set<bool>(const std::string& key, storage::Item<bool>& item)
{
    m_type = item.getType();
    m_bool = item.getValue();
}

template <> void ItemConsumer::set<double>(const std::string& key, storage::Item<double>& item)
{
    m_type = item.getType();
    m_double = item.getValue();
}

template <>
void ItemConsumer::set<std::string>(const std::string& key, storage::Item<std::string>& item)
{
    m_type = item.getType();
    m_string = item.getValue();
}



class StorageSetter
{
public:
    StorageSetter() = delete;

    StorageSetter(const std::string& name)
    {
        storage::Status status = storage::eOk;
        m_storage.reset(storage::SharedStorage::create(name, kSize, status));
    }

    storage::SharedStorage* get() const { return m_storage.get(); }

private:
    struct StorageDeleter
    {
        void operator()(storage::SharedStorage* storage)
        {
            storage->destroy();
            delete storage;
        }
    };

    std::unique_ptr<storage::SharedStorage, StorageDeleter> m_storage;
};


TEST_CASE("Shared storage can be created, opened and destroyed")
{
    std::string tmpStorageName("temporary-storage");

    SECTION("Creating a shared storage")
    {
        storage::Status status = storage::eOk;
        std::unique_ptr<storage::SharedStorage> localStorage(
            storage::SharedStorage::create(tmpStorageName, kSize, status));
        REQUIRE(status == storage::eOk);
        REQUIRE(localStorage != nullptr);
    }

    SECTION("Opening a shared storage")
    {
        storage::Status status = storage::eOk;
        std::unique_ptr<storage::SharedStorage> localStorage(
            storage::SharedStorage::open(tmpStorageName, status));
        CHECK(status == storage::eOk);
        CHECK(localStorage != nullptr);
    }

    SECTION("Destroying a shared storage")
    {
        storage::Status status = storage::SharedStorage::destroy(tmpStorageName);
        REQUIRE(status == storage::eOk);
    }
}


TEST_CASE("Bool item can be created, read, updated and removed")
{
    StorageSetter setter(std::string("bool-storage"));
    std::string key("bool-item"), tag;
    bool initialValue = false;
    storage::Status status = setter.get()->setItem(key, storage::Item<bool>(initialValue, tag));

    SECTION("Creating a bool item") { REQUIRE(status == storage::eOk); }

    SECTION("Reading a bool item")
    {
        ItemConsumer consumer;
        status = setter.get()->getItem<ItemConsumer>(key, consumer);
        CHECK(status == storage::eOk);
        CHECK(consumer.getType() == storage::eBool);
        CHECK(static_cast<bool>(consumer) == initialValue);
    }

    SECTION("Updating a bool item")
    {
        bool newValue = true;
        storage::Status status = setter.get()->setItem(key, storage::Item<bool>(newValue, tag));
        ItemConsumer consumer;
        status = setter.get()->getItem<ItemConsumer>(key, consumer);
        CHECK(status == storage::eOk);
        CHECK(static_cast<bool>(consumer) == newValue);
    }

    SECTION("Override a bool item with string value")
    {
        std::string newStringValue("this is not a boolean value");
        storage::Status status =
            setter.get()->setItem(key, storage::Item<std::string>(newStringValue, tag));
        ItemConsumer consumer;
        status = setter.get()->getItem<ItemConsumer>(key, consumer);
        CHECK(status == storage::eOk);
        std::string str_tmp = static_cast<std::string>(consumer);
        CHECK(!str_tmp.empty() == true);
        CHECK(str_tmp == newStringValue);
    }

    SECTION("Override a bool item with a double value")
    {
        double double_value(3.14);
        storage::Status status =
            setter.get()->setItem(key, storage::Item<double>(double_value, tag));
        ItemConsumer consumer;
        status = setter.get()->getItem<ItemConsumer>(key, consumer);
        CHECK(status == storage::eOk);
        CHECK(static_cast<double>(consumer) == 3.14);
    }

    SECTION("Removing a bool item")
    {
        status = setter.get()->removeItem(key);
        CHECK(status == storage::eOk);
        ItemConsumer consumer;
        status = setter.get()->getItem<ItemConsumer>(key, consumer);
        CHECK(status == storage::eItemNotFound);
        CHECK(consumer.getType() == storage::eNone);
    }
}


TEST_CASE("Double item can be created, read, updated and removed")
{
    StorageSetter setter(std::string("double-storage"));
    std::string key("double-item"), tag;
    double initialValue = 123.456;
    storage::Status status = setter.get()->setItem(key, storage::Item<double>(initialValue, tag));

    SECTION("Creating a double item") { REQUIRE(status == storage::eOk); }

    SECTION("Reading a double item")
    {
        ItemConsumer consumer;
        status = setter.get()->getItem<ItemConsumer>(key, consumer);
        CHECK(status == storage::eOk);
        CHECK(consumer.getType() == storage::eDouble);
        CHECK(static_cast<double>(consumer) == initialValue);
    }

    SECTION("Updating a double item")
    {
        double newValue = 456.789;
        storage::Status status = setter.get()->setItem(key, storage::Item<double>(newValue, tag));
        ItemConsumer consumer;
        status = setter.get()->getItem<ItemConsumer>(key, consumer);
        CHECK(status == storage::eOk);
        CHECK(consumer.getType() == storage::eDouble);
        CHECK(static_cast<double>(consumer) == newValue);
    }

    SECTION("Override a double with a string value")
    {
        std::string str_value("this is not a double value");
        storage::Status status =
            setter.get()->setItem(key, storage::Item<std::string>(str_value, tag));
        ItemConsumer consumer;
        status = setter.get()->getItem<ItemConsumer>(key, consumer);
        CHECK(status == storage::eOk);
        CHECK(consumer.getType() == storage::eString);
        std::string tmp_str = static_cast<std::string>(consumer);
        CHECK(tmp_str == str_value);
    }

    SECTION("Removing a double item")
    {
        status = setter.get()->removeItem(key);
        CHECK(status == storage::eOk);
        ItemConsumer consumer;
        status = setter.get()->getItem<ItemConsumer>(key, consumer);
        CHECK(status == storage::eItemNotFound);
        CHECK(consumer.getType() == storage::eNone);
    }
}


TEST_CASE("String item can be created, read, updated and removed")
{
    StorageSetter setter(std::string("string-storage"));
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
        setter.get()->setItem(key, storage::Item<std::string>(initialValue, tag));

    SECTION("Creating a string item") { REQUIRE(status == storage::eOk); }

    SECTION("Reading a string item")
    {
        ItemConsumer consumer;
        status = setter.get()->getItem<ItemConsumer>(key, consumer);
        CHECK(status == storage::eOk);
        CHECK(consumer.getType() == storage::eString);
        std::string stringValue = static_cast<std::string>(consumer);
        CHECK(stringValue == initialValue);
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
            setter.get()->setItem(key, storage::Item<std::string>(newValue, tag));
        ItemConsumer consumer;
        status = setter.get()->getItem<ItemConsumer>(key, consumer);
        CHECK(status == storage::eOk);
        CHECK(consumer.getType() == storage::eString);
        std::string stringValue = static_cast<std::string>(consumer);
        CHECK(stringValue == newValue);
    }

    SECTION("Override a string with a double value")
    {
        double double_value = 3.14;
        storage::Status status = setter.get()->setItem(key, storage::Item<double>(3.14, tag));
        ItemConsumer consumer;
        status = setter.get()->getItem<ItemConsumer>(key, consumer);
        CHECK(status == storage::eOk);
        CHECK(consumer.getType() == storage::eDouble);
        CHECK(static_cast<double>(consumer) == double_value);
    }

    SECTION("Removing a string item")
    {
        status = setter.get()->removeItem(key);
        CHECK(status == storage::eOk);
        ItemConsumer consumer;
        status = setter.get()->getItem<ItemConsumer>(key, consumer);
        CHECK(status == storage::eItemNotFound);
        CHECK(consumer.getType() == storage::eNone);
    }
}


TEST_CASE("Items can be created, read, in multi-processus environment")
{
    SECTION("Performing concurrent read and write accesses from several processes")
    {
        StorageSetter setter(kStorageName);

        REQUIRE(setter.get() != nullptr);

        unsigned int childsCount = std::thread::hardware_concurrency() * 2;
        boost::filesystem::path childPath(gExecutablePath);
        childPath = childPath.parent_path();
#if _WINDOWS
        childPath /= boost::filesystem::path("child-process.exe");
#else
        childPath /= boost::filesystem::path("child-process");
#endif

        auto launchChildren = [childsCount, childPath]() {
            std::vector<std::unique_ptr<boost::process::child>> children;

            for (unsigned int iter = 0; iter < childsCount; ++iter)
            {
                boost::process::child* child = new boost::process::child(childPath);
                children.push_back(std::unique_ptr<boost::process::child>(child));
            }

            for (auto iter = children.begin(); iter != children.end(); ++iter)
            {
                (*iter)->wait();
            }
        };

        auto childrenFuture = std::async(std::launch::async, launchChildren);

        childrenFuture.wait_for(std::chrono::milliseconds(10000));

        ItemConsumer consumer;
        storage::Status status = setter.get()->getItem<ItemConsumer>(kChildCountKey, consumer);
        CHECK(status == storage::eOk);

        if (status == storage::eOk)
        {
            CHECK(consumer.getType() == storage::eDouble);
            CHECK(static_cast<double>(consumer) == childsCount);
        }

        status = setter.get()->getItem<ItemConsumer>(kChildNameKey, consumer);
        CHECK(status == storage::eOk);

        if (status == storage::eOk)
        {
            CHECK(consumer.getType() == storage::eString);

            std::string names = static_cast<std::string>(consumer);
            for (unsigned int iter = 1; iter <= childsCount; ++iter)
            {
                std::string name(kChildName);
                char buffer[16];
                std::sprintf(buffer, "%i", iter);
                name.append(buffer);
                CHECK(names.find(name) != std::string::npos);
            }
        }
    }
}