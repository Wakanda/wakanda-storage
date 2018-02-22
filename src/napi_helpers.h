
#ifndef __NAPI_HELPERS__
#define __NAPI_HELPERS__


#include <node_api.h>
#include <string>


namespace napi_helpers
{
	bool				IsString(napi_env env, napi_value value);
	bool				IsNumber(napi_env env, napi_value value);
	bool				IsArray(napi_env env, napi_value value);
	bool				IsObject(napi_env env, napi_value value);
	bool				IsFunction(napi_env env, napi_value value);
	bool				IsNull(napi_env env, napi_value value);
	bool				IsUndefined(napi_env env, napi_value value);

	napi_status			GetValueStringUTF8(napi_env env, napi_value value, std::string& string);
	napi_status			CreateValueStringUTF8(const std::string& string, napi_env env, napi_value* value);

	napi_status			Stringify(napi_env env, napi_value value, std::string& string);
	napi_status			Parse(napi_env env, const std::string& string, napi_value* value);
}


#endif // __NAPI_HELPERS__