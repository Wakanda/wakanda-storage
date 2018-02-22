

#include "napi_helpers.h"


bool napi_helpers::IsString(napi_env env, napi_value value)
{
	napi_valuetype type = napi_undefined;
	napi_status status = napi_typeof(env, value, &type);
	if (status == napi_ok)
		return (type == napi_string);
	return false;
}

bool napi_helpers::IsNumber(napi_env env, napi_value value)
{
	napi_valuetype type = napi_undefined;
	napi_status status = napi_typeof(env, value, &type);
	if (status == napi_ok)
		return (type == napi_number);
	return false;
}

bool napi_helpers::IsArray(napi_env env, napi_value value)
{
	bool result = false;
	napi_status status = napi_is_array(env, value, &result);
	if (status == napi_ok)
		return result;
	return false;

}

bool napi_helpers::IsObject(napi_env env, napi_value value)
{
	napi_valuetype type = napi_undefined;
	napi_status status = napi_typeof(env, value, &type);
	if (status == napi_ok)
		return (type == napi_object);
	return false;

}

bool napi_helpers::IsFunction(napi_env env, napi_value value)
{
	napi_valuetype type = napi_undefined;
	napi_status status = napi_typeof(env, value, &type);
	if (status == napi_ok)
		return (type == napi_function);
	return false;

}

bool napi_helpers::IsNull(napi_env env, napi_value value)
{
	napi_valuetype type = napi_undefined;
	napi_status status = napi_typeof(env, value, &type);
	if (status == napi_ok)
		return (type == napi_null);
	return false;

}

bool napi_helpers::IsUndefined(napi_env env, napi_value value)
{
	napi_valuetype type = napi_undefined;
	napi_status status = napi_typeof(env, value, &type);
	if (status == napi_ok)
		return (type == napi_undefined);
	return false;

}

napi_status napi_helpers::GetValueStringUTF8(napi_env env, napi_value value, std::string& string)
{
	string.clear();
	size_t buffersize = 0;
	napi_status status = napi_get_value_string_utf8(env, value, nullptr, 0, &buffersize);
	if (status == napi_ok && buffersize > 0)
	{
		char* buffer = new char[buffersize +1];
		if (buffer != NULL)
		{
			napi_status status = napi_get_value_string_utf8(env, value, (char*)buffer, buffersize +1, nullptr);
			if (status == napi_ok)
				string.assign(buffer, buffersize);
			delete buffer;
		}
	}
	return status;
}

napi_status napi_helpers::CreateValueStringUTF8(const std::string& string, napi_env env, napi_value* value)
{
	return napi_create_string_utf8(env, string.c_str(), NAPI_AUTO_LENGTH, value);
}

napi_status napi_helpers::Stringify(napi_env env, napi_value value, std::string& string)
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
				napi_value args[1] = { value };
				napi_value result = nullptr;
				status = napi_call_function(env, json, stringify, 1, args, &result);
				if (status == napi_ok)
				{
					status = GetValueStringUTF8(env, result, string);
				}
			}
		}
	}
	return status;
}

napi_status napi_helpers::Parse(napi_env env, const std::string& string, napi_value* value)
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
				napi_value nvstring = nullptr;
				status = CreateValueStringUTF8(string, env, &nvstring);
				if (status == napi_ok)
				{
					napi_value args[1] = { nvstring };
					status = napi_call_function(env, json, parse, 1, args, value);
				}
			}
		}
	}
	return status;
}