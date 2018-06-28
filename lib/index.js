var binary = require('node-pre-gyp');
var path = require('path');
var binding_path = binary.find(path.resolve(path.join(__dirname,'../package.json')));
var binding = require(binding_path);


var TypesDescriptor = {
    "bool": {
        "id": 0
    },
    "number": {
        "id": 1
    },
    "string": {
        "id": 2
    },
    "object": {
        "id": 3,
        "beforeSet": function (object) {
            return JSON.stringify(object);
        },
        "afterGet": function (string) {
            return JSON.parse(string);
        }
    },
    "date": {
        "id": 4,
        "beforeSet": function (date) {
            return date.getTime();
        },
        "afterGet": function (number) {
            var date = new Date();
            date.setTime(number);
            return date;
        }
    },
    "buffer": {
        "id": 5,
        "beforeSet": function (buffer) {
            return buffer.toString("utf8");
        },
        "afterGet": function (string) {
            return Buffer.from(string, "utf8");
        }
    },
    "_indexes": []
};

TypesDescriptor._indexes[TypesDescriptor.bool.id] = TypesDescriptor.bool;
TypesDescriptor._indexes[TypesDescriptor.number.id] = TypesDescriptor.number;
TypesDescriptor._indexes[TypesDescriptor.string.id] = TypesDescriptor.string;
TypesDescriptor._indexes[TypesDescriptor.object.id] = TypesDescriptor.object;
TypesDescriptor._indexes[TypesDescriptor.date.id] = TypesDescriptor.date;
TypesDescriptor._indexes[TypesDescriptor.buffer.id] = TypesDescriptor.buffer;

TypesDescriptor.get = function get(value) {

    switch (typeof(value)) {

        case "boolean":
            return TypesDescriptor.bool;

        case "number":
            return TypesDescriptor.number;

        case "string":
            return TypesDescriptor.string;

        case "object":
            if (value instanceof Date) {
                return TypesDescriptor.date;
            }
            else if (value instanceof Buffer) {
                return TypesDescriptor.buffer;
            }
            else {
                return TypesDescriptor.object;
            }

        default:
            throw(new Error("Unsupported value type"));
    }
};




var SharedStorageProxy = function SharedStorageProxy(storage) {
    this.storage = storage;
};


SharedStorageProxy.prototype.set = function set(key, value) {
    if (typeof(value) != "undefined") {
        var desc = TypesDescriptor.get(value);
        if (desc) {
            if ("beforeSet" in desc) {
                value = desc.beforeSet(value);
            }
            return this.storage.set(key, {
               "type": desc.id,
               "value": value
            });
        }
    }
};


SharedStorageProxy.prototype.get = function get(key) {
    var value;
    var item = this.storage.get(key);
    if (typeof(item) != "undefined") {
        value = item.value;
        var desc = TypesDescriptor._indexes[item.type];
        if (desc && ("afterGet" in desc)) {
            value = desc.afterGet(value);
        }
    }
    return value;
};


SharedStorageProxy.prototype.remove = function remove(key) {
    this.storage.remove(key);
};


SharedStorageProxy.prototype.clear = function clear() {
    return this.storage.clear();
};


SharedStorageProxy.prototype.unlock = function unlock() {
    return this.storage.unlock();
};


SharedStorageProxy.prototype.lock = function lock() {
    return this.storage.lock();
};


SharedStorageProxy.prototype.tryLock = function tryLock() {
    return this.storage.tryLock();
};


SharedStorageProxy.create = function create(name, size) {
    var local_size = size || (1024 * 1024);
    var storage = binding.create(name, local_size);
    return new SharedStorageProxy(storage);
};


SharedStorageProxy.get = function get(name) {
    var storage = binding.get(name);
    return new SharedStorageProxy(storage);
};


SharedStorageProxy.destroy = function destroy(name) {
    return binding.destroy(name);
};



//module.exports.create = SharedStorageProxy.create;
//module.exports.get = SharedStorageProxy.get;
//module.exports.destroy = SharedStorageProxy.destroy;

module.exports = binding;