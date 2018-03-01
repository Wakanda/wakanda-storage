
#ifndef __SHARED_STORAGE_H__
#define __SHARED_STORAGE_H__


#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include <node_api.h>


template <class T>
using InterprocessAllocator = boost::interprocess::allocator<T, boost::interprocess::managed_shared_memory::segment_manager>;

struct ItemInfo
{
	napi_valuetype	item_type;
};

using ItemInfoMapMapAllocator = InterprocessAllocator< std::pair<const boost::interprocess::string, ItemInfo> >;
using ItemInfoMap = boost::interprocess::map< boost::interprocess::string, ItemInfo, std::less<boost::interprocess::string>, ItemInfoMapMapAllocator >;
using StringValue = boost::interprocess::basic_string< char, std::char_traits<char>, InterprocessAllocator<char> >;




class SharedStorage
{
public:
	SharedStorage() = delete;
	~SharedStorage();

	static	SharedStorage*		Create(napi_env env, const char* name, int64_t size);
	static	SharedStorage*		Open(napi_env env, const char* name);
	static	bool				Destroy(napi_env env, const char* name);

	napi_status					SetItem(napi_env env, napi_value key, napi_value value);
	napi_status					GetItem(napi_env env, napi_value key, napi_value* value);
	napi_status					RemoveItem(napi_env env, napi_value key);
	void						Clear();

	void						Lock();
	void						Unlock();
	bool						TryToLock();

private:
	/*
		this constructor creates a new shared memory segment
	*/
	SharedStorage(const char* name, int64_t size);

	/*
		this constructor opens an existing shared memory segment
	*/	
	SharedStorage(const char* name);

	void						_Initialize();
	bool						_DestroyItemValue(const char* key, napi_valuetype type);

	std::string name_;
	boost::interprocess::managed_shared_memory segment_;
	boost::interprocess::interprocess_recursive_mutex* mutex_;
	ItemInfoMap* item_info_map_;
};



class JsSharedStorage
{
public:
	// class definition
	static	napi_status		DefineClass(napi_env env);
	static	napi_status		Undefine(napi_env env);
	static	napi_value		Constructor(napi_env env, napi_callback_info info);

	// instance utilities
	static	napi_status		CreateInstance(napi_env env, SharedStorage* storage, napi_value* result);
	static	void			Finalize(napi_env env, void* finalize_data, void* finalize_hint);
	static	napi_status		GetStorage(napi_env env, napi_callback_info info, SharedStorage** result);

	// APIs
	static	napi_value		Create(napi_env env, napi_callback_info info);
	static	napi_value		Open(napi_env env, napi_callback_info info);
	static	napi_value		Destroy(napi_env env, napi_callback_info info);

	static	napi_value		SetItem(napi_env env, napi_callback_info info);
	static	napi_value		GetItem(napi_env env, napi_callback_info info);
	static	napi_value		RemoveItem(napi_env env, napi_callback_info info);
	static	napi_value		Clear(napi_env env, napi_callback_info info);

	static	napi_value		Lock(napi_env env, napi_callback_info info);
	static	napi_value		Unlock(napi_env env, napi_callback_info info);
	static	napi_value		TryToLock(napi_env env, napi_callback_info info);

private:
	static	napi_ref		constructor_;
};


#endif __SHARED_STORAGE_H__