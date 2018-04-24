# wakanda-storage

`wakanda-storage` provides an easy to use Node storage, shared between all your Node processes (as for Node cluster mode).

**Be aware**, `wakanda-storage` is shared between all Node processes. The storage memory is not freed until `destroy()` is called.


## Installation

```
npm install wakanda-storage
```

## Usage

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

## API

### create(storageName: String, storageSize?: Number): Storage

Create a storage.
By default, the storage size is defined to 1048576 octets.

```
let movies = Storage.create('movieStorage');
```

### get(storageName: String): Storage

Get an existing storage

```
let movies = Storage.get('movieStorage');
```

### destroy(storageName: String)

Destroy an existing storage
As `wakanda-storage` is shared between all Node processes, the storage memory is not freed until `destroy()` is called.

```
Storage.destroy('movieStorage');
```

### storage.set(key: String, value: String | Number | Boolean | Array | Object)

Set a storage key/value.
`Date` and `Buffer` are not supported.

```
movies.set('total', 30);
```

### storage.get(key: String): String | Number | Boolean | Array | Object

Get a storage key/value

```
let totalMovies = movies.get('total');
```

### storage.remove(key: String)

Remove a storage key

```
movies.remove('total');
```

### storage.clear()

Removes all keys/values from the storage

```
movies.clear();
```

### storage.lock()

Lock storage.
Storage cannot be update except by the thread who lock it.
If already lock, then it waits until the storage is unlock.

```
movies.lock();
```

### storage.unlock()

Unlock storage

```
movies.unlock();
```

### storage.tryLock(): Boolean

Try to lock the storage. If already lock, then it returns `false`.

```
movies.tryLock();
```
