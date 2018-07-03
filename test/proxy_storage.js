var Storage = require('..');
var assert = require('assert');

var proxyStorage = null;

var dateObject = new Date('July 1, 2018');
const buf = Buffer.from('This is a buffer', 'utf8');
			
describe('proxy storage', function() {

    
    before(function() {
		Storage.destroy('proxy_storage');
		proxyStorage = Storage.create('proxy_storage', 1024);
    });

   describe('#Create storage object ', function() {
		it('should return true', function() {
			assert.equal(true, proxyStorage instanceof Object);
		});
	});
	
	describe('#set and get Date Object ', function() {
			
		it('should return undefined', function() {
			assert.equal(undefined, proxyStorage.set('dateObject',dateObject));
	    });
		   
		it('should return true', function() {
			
		    assert.equal(true, proxyStorage.get('dateObject') instanceof Date);
		});
		   
	});
	
	describe('#set and get buffer Object', function() {
			
		it('should return undefined', function() {
			assert.equal(undefined, proxyStorage.set('bufferObject',buf));
	    });
		   
		it('should return true', function() {
		    assert.equal(true, proxyStorage.get('bufferObject') instanceof Buffer);
		});
		   
	});
	
    after(function() {
		Storage.destroy('proxy_storage');
	});
	
});

