
#ifndef __SHARED_STORAGE_H__
#define __SHARED_STORAGE_H__


#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include <node_api.h>



typedef struct
{
	napi_valuetype	item_type;
} ItemInfo;

typedef std::pair<const boost::interprocess::string, ItemInfo> ItemInfoMapValueType;
typedef boost::interprocess::allocator<ItemInfoMapValueType, boost::interprocess::managed_shared_memory::segment_manager>  ItemInfoMapMapAllocator;
typedef boost::interprocess::map<boost::interprocess::string, ItemInfo, std::less<boost::interprocess::string>, ItemInfoMapMapAllocator> ItemInfoMap;

typedef boost::interprocess::allocator<void, boost::interprocess::managed_shared_memory::segment_manager> VoidAllocator;
typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager> CharAllocator;
typedef boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator> StringValue;



class SharedStorage
{
public:
	~SharedStorage();

	static	SharedStorage*		Create(napi_env env, const char* name, int64_t size);
	static	SharedStorage*		Open(napi_env env, const char* name);

	napi_status					SetItem(napi_env env, napi_value key, napi_value value);
	napi_status					GetItem(napi_env env, napi_value key, napi_value* value);
	napi_status					RemoveItem(napi_env env, napi_value key);
	void						Clear(napi_env env);

	void						Lock(napi_env env);
	void						Unlock(napi_env env);
	bool						TryToLock(napi_env env);

	bool						Destroy(napi_env env);

private:
	SharedStorage();
	SharedStorage(const char* name, boost::interprocess::managed_shared_memory* segment, bool segmentOwner);

	bool						_DestroyItemValue(const char* key, napi_valuetype type);

	std::string name_;
	boost::interprocess::managed_shared_memory* segment_;
	boost::interprocess::interprocess_recursive_mutex* mutex_;
	ItemInfoMap* item_info_map_;
	bool segmentowner_;
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

	static	napi_value		SetItem(napi_env env, napi_callback_info info);
	static	napi_value		GetItem(napi_env env, napi_callback_info info);
	static	napi_value		RemoveItem(napi_env env, napi_callback_info info);
	static	napi_value		Clear(napi_env env, napi_callback_info info);

	static	napi_value		Lock(napi_env env, napi_callback_info info);
	static	napi_value		Unlock(napi_env env, napi_callback_info info);
	static	napi_value		TryToLock(napi_env env, napi_callback_info info);

	static	napi_value		Destroy(napi_env env, napi_callback_info info);

private:
	static	napi_ref		constructor_;
};


#endif __SHARED_STORAGE_H__