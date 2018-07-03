var Storage = require('..');
var assert = require('assert');

var proxyStorage = null;

var dateObject = new Date('July 1, 2018');
const buf = Buffer.from('July 1, 2018', 'utf8');
			
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
	
	describe('#Date as object ', function() {
			
		it('should return undefined', function() {
			assert.equal(undefined, proxyStorage.set('dateObject',dateObject));
	    });
		   
		it('should return string', function() {
			console.log(proxyStorage.get('dateObject'));
		    assert.equal(true, proxyStorage.get('dateObject') instanceof Object);
		});
		   
	});
	
	describe('#Date as buffer', function() {
			
		it('should return undefined', function() {
			assert.equal(undefined, proxyStorage.set('bufDate',
			));
	    });
		   
		it('should return string', function() {
			console.log(proxyStorage.get('bufDate'));
		    assert.equal(true, proxyStorage.get('bufDate') instanceof Object);
		});
		   
	});
	
    after(function() {
		Storage.destroy('proxy_storage');
	});
	
});

