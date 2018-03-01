
#include "shared_storage.h"
#include "napi_helpers.h"
#include <boost/interprocess/sync/scoped_lock.hpp>



SharedStorage::SharedStorage(const char* name, int64_t size)
: name_(name), segment_(boost::interprocess::create_only, name, size), mutex_(nullptr), item_info_map_(nullptr)
{
	_Initialize();
}

SharedStorage::SharedStorage(const char* name)
: name_(name), segment_(boost::interprocess::open_only, name), mutex_(nullptr), item_info_map_(nullptr)
{
	_Initialize();
}

void SharedStorage::_Initialize()
{
	const char kItemInfoMapKey[] = "__item_info_map__";
	const char kStorageMutexKey[] = "__storage_mutex__";

	ItemInfoMapMapAllocator allocator(segment_.get_segment_manager());
	mutex_ = segment_.find_or_construct<boost::interprocess::interprocess_recursive_mutex>(kStorageMutexKey)();
	item_info_map_ = segment_.find_or_construct<ItemInfoMap>(kItemInfoMapKey)(std::less<boost::interprocess::string>(), allocator);
}

SharedStorage::~SharedStorage()
{
}

SharedStorage* SharedStorage::Create(napi_env env, const char* name, int64_t size)
{
	SharedStorage* storage = nullptr;

	try
	{
		storage = new SharedStorage(name, size);
	}
	catch (const std::exception& e)
	{
		napi_throw_error(env, nullptr, e.what());
	}

	return storage;
}

SharedStorage* SharedStorage::Open(napi_env env, const char* name)
{
	SharedStorage* storage = nullptr;

	try
	{
		storage = new SharedStorage(name);
	}
	catch (const std::exception& e)
	{
		napi_throw_error(env, nullptr, e.what());
	}

	return storage;
}

bool SharedStorage::Destroy(napi_env env, const char* name)
{
	bool destroyed = false;

	try
	{
		destroyed = boost::interprocess::shared_memory_object::remove(name);
	}
	catch (const std::exception& e)
	{
		napi_throw_error(env, nullptr, e.what());
	}

	return destroyed;
}

napi_status	SharedStorage::SetItem(napi_env env, napi_value key, napi_value value)
{
	napi_status status = napi_ok;
	napi_valuetype type = napi_undefined;
	std::string strkey, loc_string_value;
	bool loc_bool_value = false;
	double loc_double_value = 0.0;

	// read key
	status = napi_helpers::GetValueStringUTF8(env, key, strkey);
	// read value type
	if (status == napi_ok)
		status = napi_typeof(env, value, &type);
	// read value
	if (status == napi_ok)
	{
		switch (type)
		{
		case napi_boolean:
			status = napi_get_value_bool(env, value, &loc_bool_value);
			break;

		case napi_number:
			status = napi_get_value_double(env, value, &loc_double_value);
			break;

		case napi_string:
			status = napi_helpers::GetValueStringUTF8(env, value, loc_string_value);
			break;

		case napi_object:
			status = napi_helpers::Stringify(env, value, loc_string_value);
			break;

		default:
			break;
		}
	}
	if (status == napi_ok)
	{
		bool construct_new_value = false;
		boost::interprocess::string ipstrkey(strkey.c_str());
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex> lock(*mutex_);
		
		ItemInfoMap::iterator item_info = item_info_map_->find(ipstrkey);
		if (item_info != item_info_map_->end())
		{
			// an item with the same key already exists
			if (item_info->second.item_type != type)
			{
				// the value type is different, then destroy the value and construct a new one
				_DestroyItemValue(strkey.c_str(), item_info->second.item_type);
				item_info->second = { type };
				construct_new_value = true;
			}
			else
			{
				// the value type is the same, just update the value
				if (type == napi_boolean)
				{
					bool* item_value = segment_.find<bool>(strkey.c_str()).first;
					*item_value = loc_bool_value;
				}
				else if (type == napi_number)
				{
					double *item_value = segment_.find<double>(strkey.c_str()).first;
					*item_value = loc_double_value;;
				}
				else if ((type == napi_string) || (type == napi_object))
				{
					StringValue *item_value = segment_.find<StringValue>(strkey.c_str()).first;
					item_value->assign(loc_string_value.c_str());
				}
			}
		}
		else
		{
			// the item does not exist, create a new one
			(*item_info_map_)[ipstrkey] = { type };
			construct_new_value = true;
		}

		if (construct_new_value)
		{
			if (type == napi_boolean)
			{
				segment_.construct<bool>(strkey.c_str())(loc_bool_value);
			}
			else if (type == napi_number)
			{
				segment_.construct<double>(strkey.c_str())(loc_double_value);
			}
			else if ((type == napi_string) || (type == napi_object))
			{
				segment_.construct<StringValue>(strkey.c_str())(loc_string_value.c_str(), segment_.get_segment_manager());
			}
		}
	}
	return status;
}

napi_status	SharedStorage::GetItem(napi_env env, napi_value key, napi_value* value)
{
	napi_status status = napi_ok;
	std::string strkey;
	status = napi_helpers::GetValueStringUTF8(env, key, strkey);
	if (status == napi_ok)
	{
		napi_valuetype value_type = napi_undefined;
		StringValue* loc_string_value = nullptr;
		bool* loc_bool_value = nullptr;
		double* loc_double_value = nullptr;

		boost::interprocess::string ipstrkey(strkey.c_str());

		{
			boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex> lock(*mutex_);

			ItemInfoMap::iterator item_info = item_info_map_->find(ipstrkey);
			if (item_info != item_info_map_->end())
			{
				value_type = item_info->second.item_type;
				if (value_type == napi_boolean)
				{
					loc_bool_value = segment_.find<bool>(strkey.c_str()).first;
				}
				else if (value_type == napi_number)
				{
					loc_double_value = segment_.find<double>(strkey.c_str()).first;
				}
				else if ((value_type == napi_string) || (value_type == napi_object))
				{
					loc_string_value = segment_.find<StringValue>(strkey.c_str()).first;
				}
			}
		}

		switch (value_type)
		{
		case napi_undefined:
			status = napi_get_undefined(env, value);
			break;

		case napi_null:
			status = napi_get_null(env, value);
			break;

		case napi_boolean:
			status = napi_get_boolean(env, *loc_bool_value, value);
			break;

		case napi_number:
			status = napi_create_double(env, *loc_double_value, value);
			break;

		case napi_string:
			status = napi_create_string_utf8(env, loc_string_value->c_str(), NAPI_AUTO_LENGTH, value);
			break;

		case napi_object:
		{
			std::string string(loc_string_value->c_str());
			status = napi_helpers::Parse(env, string, value);
			break;
		}

		default:
			break;
		}
	}
	return status;
}

napi_status	SharedStorage::RemoveItem(napi_env env, napi_value key)
{
	napi_status status = napi_ok;
	std::string strkey;
	status = napi_helpers::GetValueStringUTF8(env, key, strkey);
	if (status == napi_ok)
	{
		boost::interprocess::string ipstrkey(strkey.c_str());
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex> lock(*mutex_);

		ItemInfoMap::iterator item_info = item_info_map_->find(ipstrkey);
		if (item_info != item_info_map_->end())
		{
			_DestroyItemValue(strkey.c_str(), item_info->second.item_type);
			item_info_map_->erase(item_info);
		}
	}
	return status;
}

void SharedStorage::Clear()
{
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_recursive_mutex> lock(*mutex_);

	for (ItemInfoMap::iterator iter = item_info_map_->begin() ; iter != item_info_map_->end(); ++iter)
	{
		_DestroyItemValue(iter->first.c_str(), iter->second.item_type);
	}
	item_info_map_->clear();
}

void SharedStorage::Lock()
{
	if (mutex_ != nullptr)
	{
		mutex_->lock();
	}
}

void SharedStorage::Unlock()
{
	if (mutex_ != nullptr)
	{
		mutex_->unlock();
	}
}

bool SharedStorage::TryToLock()
{
	if (mutex_ != nullptr)
	{
		return mutex_->try_lock();
	}
	return false;
}

bool SharedStorage::_DestroyItemValue(const char* key, napi_valuetype type)
{
	if (type == napi_boolean)
	{
		return segment_.destroy<bool>(key);
	}
	else if (type == napi_number)
	{
		return segment_.destroy<double>(key);
	}
	else if ((type == napi_string) || (type == napi_object))
	{
		return segment_.destroy<StringValue>(key);
	}
	return false;
}


napi_ref JsSharedStorage::constructor_ = nullptr;

napi_status JsSharedStorage::DefineClass(napi_env env)
{
	std::vector<napi_property_descriptor> properties;
	properties.push_back({ "set", nullptr, SetItem, nullptr, nullptr, nullptr, napi_default, nullptr });
	properties.push_back({ "get", nullptr, GetItem, nullptr, nullptr, nullptr, napi_default, nullptr });
	properties.push_back({ "remove", nullptr, RemoveItem, nullptr, nullptr, nullptr, napi_default, nullptr });
	properties.push_back({ "clear", nullptr, Clear, nullptr, nullptr, nullptr, napi_default, nullptr });
	properties.push_back({ "lock", nullptr, Lock, nullptr, nullptr, nullptr, napi_default, nullptr });
	properties.push_back({ "unlock", nullptr, Unlock, nullptr, nullptr, nullptr, napi_default, nullptr });
	properties.push_back({ "tryLock", nullptr, TryToLock, nullptr, nullptr, nullptr, napi_default, nullptr });
	napi_value constructor = nullptr;
	napi_status status = napi_define_class(
		env,
		"SharedStorage",
		NAPI_AUTO_LENGTH,
		JsSharedStorage::Constructor,
		nullptr,
		properties.size(),
		properties.data(),
		&constructor
	);

	if (status == napi_ok)
	{
		status = napi_create_reference(env, constructor, 1, &JsSharedStorage::constructor_);
	}
	return status;

}

napi_status JsSharedStorage::Undefine(napi_env env)
{
	napi_status status = napi_ok;
	if (JsSharedStorage::constructor_ != nullptr)
	{
		status = napi_delete_reference(env, JsSharedStorage::constructor_);
		JsSharedStorage::constructor_ = nullptr;
	}
	return status;

}

napi_value JsSharedStorage::Constructor(napi_env env, napi_callback_info info)
{
	napi_value this_obj = nullptr;
	napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &this_obj, nullptr);
	return this_obj;

}

napi_status JsSharedStorage::CreateInstance(napi_env env, SharedStorage* storage, napi_value* result)
{
	napi_value constructor = nullptr;
	napi_status status = napi_get_reference_value(env, JsSharedStorage::constructor_, &constructor);
	if (status == napi_ok)
	{
		status = napi_new_instance(env, constructor, 0, nullptr, result);
		if (status == napi_ok)
		{
			status = napi_wrap(env, *result, storage, JsSharedStorage::Finalize, nullptr, nullptr);
		}
	}
	return status;

}

void JsSharedStorage::Finalize(napi_env env, void* finalize_data, void* finalize_hint)
{
	SharedStorage* storage = (SharedStorage*)finalize_data;
	delete storage;
}

napi_status JsSharedStorage::GetStorage(napi_env env, napi_callback_info info, SharedStorage** result)
{
	napi_value this_obj = nullptr;
	napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &this_obj, nullptr);
	if (status == napi_ok)
		status = napi_unwrap(env, this_obj, (void**)result);

	return status;
}

napi_value JsSharedStorage::Create(napi_env env, napi_callback_info info)
{
	napi_value result = nullptr;
	napi_value args[2];
	size_t args_count = 2;
	napi_status status = napi_get_cb_info(env, info, &args_count, args, nullptr, nullptr);
	if ((status == napi_ok) && (args_count > 0))
	{
		if (napi_helpers::IsString(env, args[0]))
		{
			std::string strkey;
			status = napi_helpers::GetValueStringUTF8(env, args[0], strkey);
			if (status == napi_ok)
			{
				int64_t size = 1024 * 1024;
				if (args_count > 1)
				{
					if (napi_helpers::IsNumber(env, args[1]))
					{
						status = napi_get_value_int64(env, args[1], &size);
					}
				}
				if (status == napi_ok)
				{
					SharedStorage* storage = SharedStorage::Create(env, strkey.c_str(), size);
					if (storage != nullptr)
					{
						status = JsSharedStorage::CreateInstance(env, storage, &result);
					}
				}
			}
		}
	}
	return result;
}

napi_value JsSharedStorage::Open(napi_env env, napi_callback_info info)
{
	napi_value result = nullptr;
	napi_value args[1];
	size_t args_count = 1;
	napi_status status = napi_get_cb_info(env, info, &args_count, args, nullptr, nullptr);
	if ((status == napi_ok) && (args_count == 1))
	{
		if (napi_helpers::IsString(env, args[0]))
		{
			std::string strkey;
			status = napi_helpers::GetValueStringUTF8(env, args[0], strkey);
			if (status == napi_ok)
			{
				SharedStorage* storage = SharedStorage::Open(env, strkey.c_str());
				if (storage != nullptr)
				{
					status = JsSharedStorage::CreateInstance(env, storage, &result);
				}
			}
		}
	}
	return result;
}

napi_value JsSharedStorage::Destroy(napi_env env, napi_callback_info info)
{
	napi_value result = nullptr;
	napi_value args[1];
	size_t args_count = 1;
	napi_status status = napi_get_cb_info(env, info, &args_count, args, nullptr, nullptr);
	if ((status == napi_ok) && (args_count == 1))
	{
		if (napi_helpers::IsString(env, args[0]))
		{
			std::string strkey;
			status = napi_helpers::GetValueStringUTF8(env, args[0], strkey);
			if (status == napi_ok)
			{
				bool destroyed = SharedStorage::Destroy(env, strkey.c_str());
				status = napi_get_boolean(env, destroyed, &result);
			}
		}
	}
	return result;
}

napi_value JsSharedStorage::SetItem(napi_env env, napi_callback_info info)
{
	napi_value this_obj = nullptr;
	size_t args_count = 2;
	napi_value args[2];
	napi_status status = napi_get_cb_info(env, info, &args_count, args, &this_obj, nullptr);
	if ((status == napi_ok) && (args_count == 2))
	{
		if (napi_helpers::IsString(env, args[0]))
		{
			SharedStorage* storage = nullptr;
			status = napi_unwrap(env, this_obj, (void**)&storage);
			if (status == napi_ok)
			{
				try
				{
					storage->SetItem(env, args[0], args[1]);
				}
				catch (const std::exception& e)
				{
					napi_throw_error(env, nullptr, e.what());
				}
			}
		}
	}

	return nullptr;
}

napi_value JsSharedStorage::GetItem(napi_env env, napi_callback_info info)
{
	napi_value result = nullptr;
	napi_value this_obj = nullptr;
	size_t args_count = 1;
	napi_value args[1];
	napi_status status = napi_get_cb_info(env, info, &args_count, args, &this_obj, nullptr);
	if ((status == napi_ok) && (args_count == 1))
	{
		if (napi_helpers::IsString(env, args[0]))
		{
			SharedStorage* storage = nullptr;
			status = napi_unwrap(env, this_obj, (void**)&storage);
			if (status == napi_ok)
			{
				storage->GetItem(env, args[0], &result);
			}
		}
	}
	return result;
}

napi_value JsSharedStorage::RemoveItem(napi_env env, napi_callback_info info)
{
	napi_value this_obj = nullptr;
	size_t args_count = 1;
	napi_value args[1];
	napi_status status = napi_get_cb_info(env, info, &args_count, args, &this_obj, nullptr);
	if ((status == napi_ok) && (args_count == 1))
	{
		if (napi_helpers::IsString(env, args[0]))
		{
			SharedStorage* storage = nullptr;
			status = napi_unwrap(env, this_obj, (void**)&storage);
			if (status == napi_ok)
			{
				try
				{
					storage->RemoveItem(env, args[0]);
				}
				catch (const std::exception& e)
				{
					napi_throw_error(env, nullptr, e.what());
				}
			}
		}
	}
	return nullptr;
}

napi_value JsSharedStorage::Clear(napi_env env, napi_callback_info info)
{
	SharedStorage* storage = nullptr;
	napi_status status = GetStorage(env, info, &storage);
	if (status == napi_ok)
	{
		try
		{
			storage->Clear();
		}
		catch (const std::exception& e)
		{
			napi_throw_error(env, nullptr, e.what());
		}
	}
	return nullptr;
}

napi_value JsSharedStorage::Lock(napi_env env, napi_callback_info info)
{
	SharedStorage* storage = nullptr;
	napi_status status = GetStorage(env, info, &storage);
	if (status == napi_ok)
	{
		storage->Lock();
	}
	return nullptr;
}

napi_value JsSharedStorage::Unlock(napi_env env, napi_callback_info info)
{
	SharedStorage* storage = nullptr;
	napi_status status = GetStorage(env, info, &storage);
	if (status == napi_ok)
	{
		storage->Unlock();
	}
	return nullptr;
}

napi_value JsSharedStorage::TryToLock(napi_env env, napi_callback_info info)
{
	napi_value result = nullptr;
	SharedStorage* storage = nullptr;
	napi_status status = GetStorage(env, info, &storage);
	if (status == napi_ok)
	{
		bool locked = storage->TryToLock();
		napi_status status = napi_get_boolean(env, locked, &result);
	}
	return result;
}
