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
 * \file    napi_helpers.h
 */


#ifndef NAPI_HELPERS_H_
#define NAPI_HELPERS_H_

#include <node_api.h>
#include <string>


namespace napi_helpers
{
/**
 * @brief  string type checking.
 *
 * @param env Nodejs environment handler.
 * @param value Value for which the type is checked.
 *
 * @return true if the value is string.
 */
bool isString(napi_env env, napi_value value);

/**
 * @brief  number type checking.
 *
 * @param env Nodejs environment handler.
 * @param value Value for which the type is checked.
 *
 * @return true if the value is number.
 */
bool isNumber(napi_env env, napi_value value);

/**
 * @brief  array type checking.
 *
 * @param env Nodejs environment handler.
 * @param value Value for which the type is checked.
 *
 * @return true if the value is array.
 */
bool isArray(napi_env env, napi_value value);

/**
 * @brief  object type checking.
 *
 * @param env Nodejs environment handler.
 * @param value Value for which the type is checked.
 *
 * @return true if the value is object.
 */
bool isObject(napi_env env, napi_value value);

/**
 * @brief  function type checking.
 *
 * @param env Nodejs environment handler.
 * @param value Value for which the type is checked.
 *
 * @return true if the value is function.
 */
bool isFunction(napi_env env, napi_value value);

/**
 * @brief  null type checking.
 *
 * @param env Nodejs environment handler.
 * @param value Value for which the type is checked.
 *
 * @return true if the value is null.
 */
bool isNull(napi_env env, napi_value value);

/**
 * @brief  undefined type checking.
 *
 * @param env Nodejs environment handler.
 * @param value Value for which the type is checked.
 *
 * @return true if the value is undefined.
 */
bool isUndefined(napi_env env, napi_value value);

/**
 * @brief  boolean type checking.
 *
 * @param env Nodejs environment handler.
 * @param value Value for which the type is checked.
 *
 * @return true if the value is boolean.
 */
bool isBool(napi_env env, napi_value value);

/**
 * @brief read a string from a value.
 *
 * @param env Nodejs environment handler.
 * @param value Value from which read the string.
 * @param[out] string Read string.
 *
 * @return napi_ok if reading the string succeeded.
 */
napi_status getValueStringUTF8(napi_env env, napi_value value, std::string& string);

/**
 * @brief  create a string value.
 *
 * @param string String used to create the value.
 * @param env Nodejs environment handler.
 * @param[out] value Created string value.
 *
 * @return napi_ok if creating the string value succeeded.
 */
napi_status createValueStringUTF8(const std::string& string, napi_env env, napi_value* value);

/**
 * @brief  Stringify a value.
 *
 * @param env Nodejs environment handler.
 * @param value Value to stringify.
 * @param[out] string Stringifying result.
 *
 * @return napi_ok is stringifying the value succeeded.
 */
napi_status stringify(napi_env env, napi_value value, std::string& string);

/**
 * @brief	Parse a string.
 *
 * @param env Nodejs environment handler.
 * @param string String to parse.
 * @param[out] value Parsing result.
 *
 * @return napi_ok is parsing the string succeeded.
 */
napi_status parse(napi_env env, const std::string& string, napi_value* value);

} // namespace napi_helpers

#endif /* NAPI_HELPERS_H_ */
