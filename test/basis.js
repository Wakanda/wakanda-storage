
var destroyed = false;
var storage = null;

var _assert = function _assert(test, expected, current) {
	if (expected != current) {
		throw (new Error('test \'' + test + '\' fails (unexpected value): current is \'' + current + '\', expected is \'' + expected + '\''));
	}
}

try {
	
	var binary = require('node-pre-gyp');
    var path = require('path')
    var binding_path = binary.find(path.resolve(path.join(__dirname,'./package.json')));
    storage = require(binding_path);

	//storage = require('./../build/Release/wakanda_storage.node').create('basis_storage');
	_assert('create storage', true, storage instanceof Object);

	// test values
	storage.set('string','Hello');
	_assert('string item', 'string', typeof(storage.get('string')));
	_assert('string item', 'Hello', storage.get('string'));

	storage.set('number',3.14);
	_assert('number item', 'number', typeof(storage.get('number')));
	_assert('number item', 3.14, storage.get('number'));

	storage.set('bool', true);
	_assert('bool item', 'boolean', typeof(storage.get('bool')));
	_assert('bool item', true, storage.get('bool'));

	storage.set('null', null);
	_assert('null item', 'object', typeof(storage.get('null')));
	_assert('null item', null, storage.get('null'));

	_assert('undefined item', 'undefined', typeof(storage.get('undefined')));

	var arr = [];
	arr[0] = 1;
	arr[2] = 2;
	arr[3] = "Hello";
	arr[5] = {'x':1, 'y':1};

	storage.set('array',arr);
	_assert('array item', 'object', typeof(storage.get('array')));
	_assert('array item', true, Array.isArray(storage.get('array')));
	_assert('array item', JSON.stringify(arr), JSON.stringify(storage.get('array')));

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

	storage.set('object', obj);

	_assert('object item', 'object', typeof(storage.get('object')));
	_assert('object item', JSON.stringify(obj), JSON.stringify(storage.get('object')));

	// test update
	storage.set('string','Bonjour');
	_assert('update item', 'Bonjour', storage.get('string'));

	// test remove
	storage.remove('string');
	_assert('remove item', 'undefined', typeof(storage.get('string')));

	// test tryLock
	var locked = storage.tryLock();
	if (locked) {
		storage.unlock();
	}
	_assert('tryLock storage', true, locked);

	// test open
	const storage_copy = require(binding_path).get('basis_storage');
	_assert('open storage', true, storage_copy instanceof Object);

	_assert('number item copy', 'number', typeof(storage_copy.get('number')));
	_assert('number item copy', 3.14, storage_copy.get('number'));

	_assert('bool item copy', 'boolean', typeof(storage_copy.get('bool')));
	_assert('bool item copy', true, storage_copy.get('bool'));

	_assert('null item copy', 'object', typeof(storage_copy.get('null')));
	_assert('null item copy', null, storage_copy.get('null'));

	// test clear
	storage.clear();
	_assert('clear number item', 'undefined', typeof(storage.get('number')));
	_assert('clear bool item', 'undefined', typeof(storage.get('bool')));
	_assert('clear null item', 'undefined', typeof(storage.get('null')));
	_assert('clear array item', 'undefined', typeof(storage.get('array')));
	_assert('clear object item', 'undefined', typeof(storage.get('object')));

	// test destroy
	destroyed = require(binding_path).destroy('basis_storage');
	_assert('destroy storage', true, destroyed);
}
catch (e) {
	if (!destroyed) {
		require(binding_path).destroy('basis_storage');
	}
	throw (e);
}
