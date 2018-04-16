`wakanda-storage` provides an easy to use Node storage, shared between all your Node processes (as for Node cluster mode).

**Warning**, as `wakanda-storage` is shared between all Node processes, the storage memory is not freed until `destroy()` is called.


# Installation

```
npm install wakanda-storage
```

# Usage

```
let storage = require('wakanda-storage');
storage.create('movieStorage');
let movies = storage.get('movieStorage');

let movieArr = ["Batman", "Superman"];  
movies.set('MyMovieCollection', movieArr);
movies.get('MyMovieCollection');
// ["Batman", "Superman"]
```

# APIs

```
/**
* Create a storage
* @param storageName Defines the storage name
* @returns The created storage
*/
storage.create(storageName : String);


/**
* Get an existing storage
* @param storageName The storage to returns
* @returns The named storage if exists
*/
let movies = storage.get(storageName : String);


/**
* Destroy an existing storage
* As `wakanda-storage` is shared between all Node processes, the storage memory is not freed until `destroy()` is called.
* @param storageName The storage to destroy
*/
storage.destroy(storageName : String);
```

```
/**
* Set a storage key/value
* @param key A storage key
* @param value A storage value
*/
movies.set(key: String, value: String | Number | Boolean | Array | Object);

/**
* Get a storage key/value
* @param key Storage key
* @return a value
*/
let aValue = movies.get(key : String);

/**
* Remove storage key
* @param key A storage key
*/
movies.remove(key : String);

/**
* Removes all storage keys/values
*/
movies.clear();

/**
* Lock storage.
* No key/value can be updated until unlock
* If already lock, then it waits until the storage is unlock.
*/
movies.lock();

/**
* Unlock storage
*/
movies.unlock()

/**
* Try to lock the storage. If already lock, then it returns an error
*/
movies.tryLock()
```

# Examples
```
// String type
storage.set('aString','Hello');
storage.get('aString');


// Number type
storage.set('aNumber', 3.14);
storage.get('aNumber');


// Boolean type
storage.set('aBool', true);
storage.get('aBool');


// null type
storage.set('aNull', null);
storage.get('aNull');


// Array type
var arr = [];  
arr[0] = 1;
arr[2] = 2;
arr[3] = "Hello";
arr[5] = {
  'x':1,
  'y':1
};
storage.set('anArray',arr);
storage.get('anArray')


// Object type
var obj = {
  'string': 'Hello',
  'number': 6.42,
  'bool': false,
  'object': {
    'x': 1,
    'y': 2,
    'z': 3
  }
};
storage.set('anObject', obj); 
storage.get('anObject');
```
