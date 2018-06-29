var binary = require('node-pre-gyp');
var path = require('path');
var binding_path = binary.find(path.resolve(path.join(__dirname,'../package.json')));
var binding = require(binding_path);


var TagsDescriptor = {
    "object": {
        "tag": "object",
        "beforeSet": function (object) {
            return JSON.stringify(object);
        },
        "afterGet": function (string) {
            return JSON.parse(string);
        }
    },
    "date": {
        "tag": "date",
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
        "tag": "buffer",
        "beforeSet": function (buffer) {
            return buffer.toString("utf8");
        },
        "afterGet": function (string) {
            return Buffer.from(string, "utf8");
        }
    }
};

TagsDescriptor.findByValue = function findByValue(value) {

    if (value != null) {
        switch (typeof(value)) {
            case "object":
                if (value instanceof Date) {
                    return TagsDescriptor.date;
                }
                else if (value instanceof Buffer) {
                    return TagsDescriptor.buffer;
                }
                else {
                    return TagsDescriptor.object;
                }

            default:
                return null;
        }
    }
    return null;
};

TagsDescriptor.findByTag = function findByTag(tag) {
    if (tag && (tag.length > 0) && (tag in TagsDescriptor)) {
        return TagsDescriptor[tag];
    }
    return null;
};


var SharedStorageProxy = function SharedStorageProxy(storage) {
    this.storage = storage;
};



SharedStorageProxy.prototype.set = function set(key, value) {
    if (typeof(value) != "undefined") {
        var desc = TagsDescriptor.findByValue(value);
        if (desc) {
            if ("beforeSet" in desc) {
                value = desc.beforeSet(value);
            }
            return this.storage.set(key, value, desc.tag);
        }
        else {
            return this.storage.set(key, value);
        }
    }
};


SharedStorageProxy.prototype.get = function get(key) {
    var value;
    var item = this.storage.get(key, true);
    if (typeof(item) != "undefined") {
        value = item.value;
        var desc = TagsDescriptor.findByTag(item.tag);
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



module.exports.create = SharedStorageProxy.create;
module.exports.get = SharedStorageProxy.get;
module.exports.destroy = SharedStorageProxy.destroy;
