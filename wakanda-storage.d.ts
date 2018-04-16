
interface WakandaStorage {
    /**
    * Create a storage
    * @param storageName Defines the storage name
    * @returns The created storage
    */
    create(storageName: String): WakandaStorageInstance;

    /**
    * Get an existing storage
    * @param storageName The storage to returns
    * @returns The named storage if exists
    */
    get(storageName: String): WakandaStorageInstance;

    /**
    * Destroy an existing storage
    * As `wakanda-storage` is shared between all Node processes, the storage memory is not freed until `destroy()` is called.
    * @param storageName The storage to destroy
    */
    destroy(storageName: String);
}

interface WakandaStorageInstance {

    /**
    * Set a storage key/value
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
    * Try to lock the storage. If already lock, then it returns an error
    */
    tryLock()
}