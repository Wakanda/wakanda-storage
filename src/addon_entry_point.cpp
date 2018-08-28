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
 * \file    addon_entry_point.cpp
 */

// Local includes.
#include "js_shared_storage.h"

// Other includes.
#include <node_api.h>

/**
 * @brief   addon entry point: it populates exports object
 */
napi_value initModule(napi_env env, napi_value exports)
{
    napi_status status = JsSharedStorage::define(env);
    if (status == napi_ok)
    {
        napi_property_descriptor desc[] = {{"create", nullptr, JsSharedStorage::create, nullptr,
                                            nullptr, nullptr, napi_default, nullptr},
                                           {"get", nullptr, JsSharedStorage::open, nullptr, nullptr,
                                            nullptr, napi_default, nullptr},
                                           {"destroy", nullptr, JsSharedStorage::destroy, nullptr,
                                            nullptr, nullptr, napi_default, nullptr}};
        status = napi_define_properties(env, exports, 3, desc);
    }
    return exports;
}

NAPI_MODULE(NODE_MODULE_NAME, initModule)
