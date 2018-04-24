
declare namespace WakandaStorage {
    /**
    * Create a storage
    * @param storageName Defines the storage name
    * @param storageSize Optionnal, Defines, the storage size in octet. Default: 1048576 octets.
    * @returns The created storage
    */
    export function create(storageName: String, storageSize? : Number): WakandaStorageInstance;

    /**
    * Get an existing storage
    * @param storageName The storage to returns
    * @returns The named storage if exists
    */
    export function get(storageName: String): WakandaStorageInstance;

    /**
    * Destroy an existing storage
    * As `wakanda-storage` is shared between all Node processes, the storage memory is not freed until `destroy()` is called.
    * @param storageName The storage to destroy
    */
    export function destroy(storageName: String);
}

declare interface WakandaStorageInstance {

    /**
    * Set a storage key/value
    * `Date` and `Buffer` are not supported.
    * @param key A storage key
    * @param value A storage value
    */
    set(key: String, value: String | Number | Boolean | Array | Object);

    /**
    * Get a storage key/value
    * @param key Storage key
    * @return a value
    */
    get(key: String): String | Number | Boolean |  Array | Object

    /**
    * Remove storage key
    * @param key A storage key
    */
    remove(key: String);

    /**
    * Removes all storage keys/values
    */
    clear();

    /**
    * Lock storage.
    * No key/value can be updated until unlock
    * If already lock, then it waits until the storage is unlock.
    */
    lock();

    /**
    * Unlock storage
    */
    unlock()

    /**
    * Try to lock the storage. If already lock, then it returns `false`
    */
    tryLock(): Boolean
}

export = WakandaStorage;