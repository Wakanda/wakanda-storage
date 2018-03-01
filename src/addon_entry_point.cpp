
#include <node_api.h>
#include "shared_storage.h"



napi_value InitModule(napi_env env, napi_value exports)
{
	napi_status status = JsSharedStorage::DefineClass(env);
	if (status == napi_ok)
	{
		napi_property_descriptor desc[] = {
			{ "create", nullptr, JsSharedStorage::Create, nullptr, nullptr, nullptr, napi_default, nullptr},
			{ "get", nullptr, JsSharedStorage::Open, nullptr, nullptr, nullptr, napi_default, nullptr },
			{ "destroy", nullptr, JsSharedStorage::Destroy, nullptr, nullptr, nullptr, napi_default, nullptr }
		};
		status = napi_define_properties(env, exports, 3, desc);
	}
	return exports;
}


NAPI_MODULE(NODE_MODULE_NAME, InitModule)
