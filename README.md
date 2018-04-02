# About wakanda-storage
Get or create a storage. This storage is shared between all your application.
# Features
- Easy to use from node server
- Supports multiple target versions of Node
# Installation
Clone the source into your local repository then build using 

``npm install --build-from-source``
# How to Use

To be able to use the addon require it on the top of your file as
   
   ``var storage = require('wakanda_storage');``

## Manipulate storage
Now you can create a new storage simply by calling create method

  ``storage.create('my_storage');``
  
You can also get an existing storage by calling getStorage method

  ``storage.getStorage('my_storage');``
## Manipulate data types

Get and set string type

  ``storage.set('string','Hello');``
  
  ``storage.get('string');``
  
Get and set number type

  ``storage.set('number',3.14);``
  
  ``storage.get('number');``

Get and set boolean type

  ``storage.set('bool', true);``
  
  ``storage.get('bool');``

Get and set null type

  ``storage.set('null', null);``
  
  ``storage.get('null');``
  
  ``...``   
## Manipulate Arrays and object  
wakanda storage gives also the ability to work with object and arrays 
### Arrays
  ``var arr = [];``
  
  ``arr[0] = 1;``
  
  ``arr[2] = 2;``
  
  ``arr[3] = "Hello";``
  
  ``arr[5] = {'x':1, 'y':1};``
  
  ``storage.set('array',arr);``
  
  ``storage.get('array')``
### Objects

``var obj = { 'string': 'Hello', 'number': 6.42, 'bool': false, 'object': { 'x': 1, 'y': 2, 'z': 3 } }; ``
``storage.set('object', obj); ``
``storage.get('object'); ``
  
# License

4D Community License
