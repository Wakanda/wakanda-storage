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
 * \file    child_process.cpp
 */

#include "common_process.h"
#include "shared_storage.h"
#include <stdio.h>
#include <string>

int main(int argc, char* argv[])
{
    storage::Status status = storage::eOk;
    std::unique_ptr<storage::SharedStorage> localStorage(
        storage::SharedStorage::open(kStorageName.c_str(), status));
    if (status == storage::eOk)
    {
        localStorage->lock();

        std::unique_ptr<storage::SharedItem> item;
        status = localStorage->getItem(kChildCountKey, item);
        double count = 1;
        if (status == storage::eOk)
        {
            count = item->getDouble() + 1;
        }
        status =
            localStorage->setItem(kChildCountKey, storage::SharedItemDouble(count, std::string()));

        if (status == storage::eOk)
        {
            status = localStorage->getItem(kChildNameKey, item);

            std::string names(kChildName);
            char buffer[16];
            std::sprintf(buffer, "%i", static_cast<int>(count));
            names.append(buffer);

            if (status == storage::eOk)
            {
                std::string localNames(names);
                item->getString(names);
                names.append(";");
                names.append(localNames);
            }
            status = localStorage->setItem(kChildNameKey,
                                           storage::SharedItemString(names, std::string()));
        }

        localStorage->unlock();
    }

    return (status == storage::eOk) ? 0 : 1;
}