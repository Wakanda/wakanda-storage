`wakanda-storage` provides an easy to use Node storage, shared between all your Node processes (as for Node cluster mode).

**Be aware**, `wakanda-storage` is shared between all Node processes. The storage memory is not freed until `destroy()` is called.


# Installation

```
npm install wakanda-storage
```

# Usage

```
let Storage = require('wakanda-storage');
let movies = Storage.create('movieStorage');
// let movies = Storage.get('movieStorage');

let movieArr = ["Batman", "Superman"];  
movies.set('MyMovieCollection', movieArr);
movies.get('MyMovieCollection');
// ["Batman", "Superman"]

Storage.destroy('movieStorage');
```

# APIs

```
/**
* Create a storage
* @param storageName Defines the storage name
* @returns The created storage
*/
Storage.create(storageName : String);


/**
* Get an existing storage
* @param storageName The storage to returns
* @returns The named storage if exists
*/
let movies = Storage.get(storageName : String);


/**
* Destroy an existing storage
* As `wakanda-storage` is shared between all Node processes, the storage memory is not freed until `destroy()` is called.
* @param storageName The storage to destroy
*/
Storage.destroy(storageName : String);
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
* Remove a storage key
* @param key A storage key
*/
movies.remove(key : String);

/**
* Removes all keys/values from the storage 
*/
movies.clear();

/**
* Lock storage.
* Storage cannot be update except by the thread who lock it.
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
let Storage = require('wakanda-storage');
let movies = Storage.create('movieStorage');

// String type
movies.set('aString','Hello');
movies.get('aString');


// Number type
movies.set('aNumber', 3.14);
movies.get('aNumber');


// Boolean type
movies.set('aBool', true);
movies.get('aBool');


// null type
movies.set('aNull', null);
movies.get('aNull');


// Array type
var arr = [];  
arr[0] = 1;
arr[2] = 2;
arr[3] = "Hello";
arr[5] = {
  'x':1,
  'y':1
};
movies.set('anArray',arr);
movies.get('anArray')


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
movies.set('anObject', obj); 
movies.get('anObject');
```
