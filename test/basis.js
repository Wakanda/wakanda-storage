var assert = require('assert');
var Storage =  require('..');

var storage = null;
var storage_copy = null;

var arr = [];
    arr[0] = 1;
	arr[2] = 2;
	arr[3] = "Hello";
	arr[5] = {'x':1, 'y':1};

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

describe('Wakanda Storage basis', function() {
    
	before(function() {
		
		Storage.destroy('basis_storage');
		storage = Storage.create('basis_storage');
		storage_copy = Storage.get('basis_storage');
	});
	
	describe('#Create storage object ', function() {
		it('should return true', function() {
			assert.equal(true, storage instanceof Object);
		});
	});
	
	describe('#data types ', function() {
		
		describe('#String values ', function() {
			
			it('should return undefined', function() {
				assert.equal(undefined, storage.set('string','Hello'));
		    });
		   
		    it('should return string', function() {
			   assert.equal('string', typeof(storage.get('string')));
		    });
		   
		    it('should return Hello', function() {
			   assert.equal('Hello', storage.get('string'));
		    });
		   
	    });
	   
	    describe('#Number values ', function() {
        
			it('should return undefined', function() {
				assert.equal(undefined, storage.set('number',3.14));
		    });
		   
		    it('should return number', function() {
			   assert.equal('number', typeof(storage.get('number')));
		    });
		   
		    it('should return 3.14', function() {
			   assert.equal(3.14, storage.get('number'));
		    });
		   
	    });
		
		describe('#boolean values ', function() {
        
			it('should return undefined', function() {
				assert.equal(undefined, storage.set('bool',true));
		    });
		   
		    it('should return boolean', function() {
			   assert.equal('boolean', typeof(storage.get('bool')));
		    });
		   
		    it('should return true', function() {
			   assert.equal(true, storage.get('bool'));
		    });
		   
	    });
		
		describe('#null values ', function() {
        
			it('should return undefined', function() {
				assert.equal(undefined, storage.set('null',null));
		    });
		   
		    it('should return object', function() {
			   assert.equal('object', typeof(storage.get('null')));
		    });
		   
		    it('should return null', function() {
			   assert.equal(null, storage.get('null'));
		    });
		   
	    });
		
		
		
		describe('#undefined values ', function() {
        
			it('should return undefined', function() {
				assert.equal('undefined', typeof(storage.get('undefined')));
		    });
		   
	    });
		
		describe('#array values ', function() {
        
			it('should return undefined', function() {
				assert.equal(undefined, storage.set('array',arr));
		    });
		   
		    it('should return object', function() {
			   assert.equal('object', typeof(storage.get('array')));
		    });
		   
		    it('should return true', function() {
			   assert.equal(true, Array.isArray(storage.get('array')));
		    });
		   
		    it('should return array', function() {
			   assert.equal(JSON.stringify(arr), JSON.stringify(storage.get('array')));
		    });
		   
	    });
		
		describe('#object values ', function() {
        
			it('should return undefined', function() {
				assert.equal(undefined, storage.set('object',obj));
		    });
		   
		    it('should return object', function() {
			   assert.equal('object', typeof(storage.get('object')));
		    });
		   
		    it('should return object', function() {
			   assert.equal(JSON.stringify(obj), JSON.stringify(storage.get('object')));
		    });
		   
	    });
	      
	});
	
	describe('#update values', function() {
		
		it('should return undefined', function() {
			assert.equal(undefined, storage.set('string','Bonjour'));
		});
		
		it('should return Bonjour', function() {
			assert.equal('Bonjour', storage.get('string'));
		});
	});	
	
	describe('#remove values', function() {
		
		it('should return undefined', function() {
			assert.equal(undefined, storage.remove('string'));
		});
		
		it('should return undefined', function() {
			assert.equal('undefined', typeof(storage.get('string')));
		});
	});	
	
	
	describe('#Lock', function() {
		
		it('should return true', function() {
			assert.equal(true, storage.tryLock());
		});
		
		it('should return undefined', function() {
			assert.equal(undefined, storage.unlock());
		});
		
		it('should return true', function() {
			assert.equal(true, storage.tryLock());
		});
		
	});	
	
	describe('#open', function() {
		
		it('should return true', function() {
			assert.equal(true, storage_copy instanceof Object);
		});
		
		it('should return number', function() {
			assert.equal('number', typeof(storage_copy.get('number')));
		});
		
		it('should return 3.14', function() {
			assert.equal(3.14, storage_copy.get('number'));
		});
		
		it('should return boolean', function() {
			assert.equal('boolean', typeof(storage_copy.get('bool')));
		});
		
		it('should return true', function() {
			assert.equal(true, storage_copy.get('bool'));
		});
		
		it('should return object', function() {
			assert.equal('object', typeof(storage_copy.get('null')));
		});
		
		it('should return null', function() {
			assert.equal(null,storage_copy.get('null'));
		});
		
	});	
	
	
	describe('#clear', function() {
		
		it('should return undefined', function() {
			assert.equal(undefined, storage.clear());
		});
		
		it('should return undefined', function() {
			assert.equal('undefined', typeof(storage.get('number')));
		});
		
		it('should return undefined', function() {
			assert.equal('undefined', typeof(storage.get('bool')));
		});
		
		it('should return undefined', function() {
			assert.equal('undefined',  typeof(storage.get('null')));
		});
		
		it('should return undefined', function() {
			assert.equal('undefined', typeof(storage.get('array')));
		});
		
		it('should return undefined', function() {
			assert.equal('undefined', typeof(storage.get('object')));
		});
		
	});	
	
	describe('#destroy', function() {
		
		it('should return true', function() {
			assert.equal(true, Storage.destroy('basis_storage'));
		});
	});
	
	after(function() {
		Storage.destroy('basis_storage');
	});

	
	
});


