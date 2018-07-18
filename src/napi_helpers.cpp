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
 * \file    napi_helpers.cpp
 */


#include "napi_helpers.h"
#include <vector>


bool napi_helpers::isString(napi_env env, napi_value value)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, value, &type);
    if (status == napi_ok)
        return (type == napi_string);
    return false;
}

bool napi_helpers::isNumber(napi_env env, napi_value value)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, value, &type);
    if (status == napi_ok)
        return (type == napi_number);
    return false;
}

bool napi_helpers::isArray(napi_env env, napi_value value)
{
    bool result = false;
    napi_status status = napi_is_array(env, value, &result);
    if (status == napi_ok)
        return result;
    return false;
}

bool napi_helpers::isObject(napi_env env, napi_value value)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, value, &type);
    if (status == napi_ok)
        return (type == napi_object);
    return false;
}

bool napi_helpers::isFunction(napi_env env, napi_value value)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, value, &type);
    if (status == napi_ok)
        return (type == napi_function);
    return false;
}

bool napi_helpers::isNull(napi_env env, napi_value value)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, value, &type);
    if (status == napi_ok)
        return (type == napi_null);
    return false;
}

bool napi_helpers::isUndefined(napi_env env, napi_value value)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, value, &type);
    if (status == napi_ok)
        return (type == napi_undefined);
    return false;
}

bool napi_helpers::isBool(napi_env env, napi_value value)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, value, &type);
    if (status == napi_ok)
        return (type == napi_boolean);
    return false;
}

napi_status napi_helpers::getValueStringUTF8(napi_env env, napi_value value, std::string& string)
{
    string.clear();
    size_t bufferSize = 0;
    napi_status status = napi_get_value_string_utf8(env, value, nullptr, 0, &bufferSize);
    if (status == napi_ok && bufferSize > 0)
    {
        std::vector<char> buffer(bufferSize + 1);
        napi_status status =
            napi_get_value_string_utf8(env, value, buffer.data(), bufferSize + 1, nullptr);
        if (status == napi_ok)
            string.assign(buffer.data(), bufferSize);
    }
    return status;
}

napi_status napi_helpers::createValueStringUTF8(const std::string& string, napi_env env,
                                                napi_value* value)
{
    return napi_create_string_utf8(env, string.c_str(), NAPI_AUTO_LENGTH, value);
}

napi_status napi_helpers::stringify(napi_env env, napi_value value, std::string& string)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status == napi_ok)
    {
        napi_value json = nullptr;
        status = napi_get_named_property(env, global, "JSON", &json);
        if (status == napi_ok)
        {
            napi_value stringify = nullptr;
            status = napi_get_named_property(env, json, "stringify", &stringify);
            if (status == napi_ok)
            {
                napi_value args[1] = {value};
                napi_value result = nullptr;
                status = napi_call_function(env, json, stringify, 1, args, &result);
                if (status == napi_ok)
                {
                    status = getValueStringUTF8(env, result, string);
                }
            }
        }
    }
    return status;
}

napi_status napi_helpers::parse(napi_env env, const std::string& string, napi_value* value)
{
    napi_value global = nullptr;
    napi_status status = napi_get_global(env, &global);
    if (status == napi_ok)
    {
        napi_value json = nullptr;
        status = napi_get_named_property(env, global, "JSON", &json);
        if (status == napi_ok)
        {
            napi_value parse = nullptr;
            status = napi_get_named_property(env, json, "parse", &parse);
            if (status == napi_ok)
            {
                napi_value napiString = nullptr;
                status = createValueStringUTF8(string, env, &napiString);
                if (status == napi_ok)
                {
                    napi_value args[1] = {napiString};
                    status = napi_call_function(env, json, parse, 1, args, value);
                }
            }
        }
    }
    return status;
}
