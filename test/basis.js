var assert = require('assert');
var Storage =  require('..');

var storage = null;
var storage_copy = null;

var arr = [];
arr[0] = 1;
arr[2] = 2;
arr[3] = "Hello";
arr[5] = {'x':1, 'y':1};

var arr2 = [
	"Bonjour",
	10,
	{'a':1,'b':2},
	200
]

const obj = {
	'string': 'Hello',
	'number': 6.42,
	'bool': false,
	'object': {
		'x': 1,
		'y': 2,
		'z': 3
	}
};

const obj2 = {
	'string': 'Bonjour',
	'number': 42,
	'bool': true,
	'object': {
		'x': 10,
		'y': 200,
		'z': 3000
	}	
}

const kDate = new Date('July 1, 2018');
const kDate2 = new Date('July 1, 2017');
const kBuffer = Buffer.from('This is a buffer', 'utf8');
const kBuffer2 = Buffer.from('A buffer it is (Yoda)', 'utf8');

describe('Wakanda Storage basis', function() {
    
	before(function() {
		Storage.destroy('basis_storage');
		storage = Storage.create('basis_storage');
		storage_copy = Storage.get('basis_storage');
	});
	
	describe('#create storage', function() {
		it('should return true', function() {
			assert.equal(true, storage instanceof Object);
		});
	});
	
	describe('#value types ', function() {
		
		describe('#string values ', function() {
			
			// CREATE
			it('should return undefined', function() {
				assert.equal(undefined, storage.set('string','Hello'));
		    });
		   
			// READ
		    it('should return string', function() {
			   assert.equal('string', typeof(storage.get('string')));
		    });
		   
		    it('should return Hello', function() {
			   assert.equal('Hello', storage.get('string'));
		    });

		    // UPDATE
		   	it('should return undefined', function() {
				assert.equal(undefined, storage.set('string','Bonjour'));
		    }); 

		    it('should return Bonjour', function() {
			   assert.equal('Bonjour', storage.get('string'));
		    });		    

		    // DELETE
			it('should return undefined', function() {
				assert.equal(undefined, storage.remove('string'));
		    });

			it('should return undefined', function() {
				assert.equal(undefined, storage.get('string'));
		    });		    
		   
	    });
	   
	    describe('#number values ', function() {
        
        	// CREATE
			it('should return undefined', function() {
				assert.equal(undefined, storage.set('number',3.14));
		    });
		   
		    // READ
		    it('should return number', function() {
			   assert.equal('number', typeof(storage.get('number')));
		    });
		   
		    it('should return 3.14', function() {
			   assert.equal(3.14, storage.get('number'));
		    });

		   	// UPDATE
		   	it('should return undefined', function() {
				assert.equal(undefined, storage.set('number',6.42));
		    }); 

		    it('should return 6.42', function() {
			   assert.equal(6.42, storage.get('number'));
		    });

		    // DELETE
			it('should return undefined', function() {
				assert.equal(undefined, storage.remove('number'));
		    });

			it('should return undefined', function() {
				assert.equal(undefined, storage.get('number'));
		    });			    
		   
	    });
		
		describe('#boolean values ', function() {
        
        	// CREATE
			it('should return undefined', function() {
				assert.equal(undefined, storage.set('bool',true));
		    });
		   
		    // READ
		    it('should return boolean', function() {
			   assert.equal('boolean', typeof(storage.get('bool')));
		    });
		   
		    it('should return true', function() {
			   assert.equal(true, storage.get('bool'));
		    });

		    // UPDATE
		   	it('should return undefined', function() {
				assert.equal(undefined, storage.set('bool',false));
		    }); 

		    it('should return false', function() {
			   assert.equal(false, storage.get('bool'));
		    });	

			// DELETE
			it('should return undefined', function() {
				assert.equal(undefined, storage.remove('bool'));
		    });

			it('should return undefined', function() {
				assert.equal(undefined, storage.get('bool'));
		    });			    
		   
	    });
		
		describe('#null values ', function() {
        
        	// CREATE
			it('should return undefined', function() {
				assert.equal(undefined, storage.set('null',null));
		    });
		    
			// READ
		    it('should return object', function() {
			   assert.equal('object', typeof(storage.get('null')));
		    });
		   
		    it('should return null', function() {
			   assert.equal(null, storage.get('null'));
		    });

			// DELETE
			it('should return undefined', function() {
				assert.equal(undefined, storage.remove('null'));
		    });

			it('should return undefined', function() {
				assert.equal(undefined, storage.get('null'));
		    });			    

	    });
		
		describe('#undefined values ', function() {
        
			it('should return undefined', function() {
				assert.equal('undefined', typeof(storage.get('undefined')));
		    });
		   
	    });
		
		describe('#array values ', function() {
        
			// CREATE
			it('should return undefined', function() {
				assert.equal(undefined, storage.set('array',arr));
		    });
		   
		    // READ
		    it('should return array instance', function() {
			   assert.equal(true, Array.isArray(storage.get('array')));
		    });
		   
		    it('should return same array', function() {
			   assert.equal(JSON.stringify(arr), JSON.stringify(storage.get('array')));
		    });

		    // UPDATE
		   	it('should return undefined', function() {
				assert.equal(undefined, storage.set('array',arr2));
		    }); 

		    it('should return same array', function() {
			   assert.equal(JSON.stringify(arr2), JSON.stringify(storage.get('array')));
		    });

			// DELETE
			it('should return undefined', function() {
				assert.equal(undefined, storage.remove('array'));
		    });

			it('should return undefined', function() {
				assert.equal(undefined, storage.get('array'));
		    });	
		   
	    });
		
		describe('#object values ', function() {
        
			// CREATE
			it('should return undefined', function() {
				assert.equal(undefined, storage.set('object',obj));
		    });
		   
		    // READ
		    it('should return object', function() {
			   assert.equal('object', typeof(storage.get('object')));
		    });
		   
		    it('should return same object', function() {
			   assert.equal(JSON.stringify(obj), JSON.stringify(storage.get('object')));
		    });

		    // UPDATE
		   	it('should return undefined', function() {
				assert.equal(undefined, storage.set('object',obj2));
		    }); 

		    it('should return same object', function() {
			   assert.equal(JSON.stringify(obj2), JSON.stringify(storage.get('object')));
		    });

			// DELETE
			it('should return undefined', function() {
				assert.equal(undefined, storage.remove('object'));
		    });

			it('should return undefined', function() {
				assert.equal(undefined, storage.get('object'));
		    });	
		   
	    });

		describe('#date values ', function() {
        
            // CREATE
			it('should return undefined', function() {
				assert.equal(undefined, storage.set('date', kDate));
		    });

			// READ
		    it('should return date instance', function() {
		       assert.equal(true, storage.get('date') instanceof Date);
		    });

		    it('should return same date', function() {
		       assert.equal(kDate.getTime(), storage.get('date').getTime());
		    });

		    // UPDATE
		   	it('should return undefined', function() {
				assert.equal(undefined, storage.set('date',kDate2));
		    }); 

		    it('should return same date', function() {
		       assert.equal(kDate2.getTime(), storage.get('date').getTime());
		    });

		    // DELETE
			it('should return undefined', function() {
				assert.equal(undefined, storage.remove('date'));
		    });

			it('should return undefined', function() {
				assert.equal(undefined, storage.get('date'));
		    });
		   
	    });

		describe('#buffer values ', function() {
        	
        	// CREATE
			it('should return undefined', function() {
				assert.equal(undefined, storage.set('buffer', kBuffer));
		    });

			// READ
		    it('should return buffer instance', function() {
		       assert.equal(true, storage.get('buffer') instanceof Buffer);
		    });

		    it('should return same buffer', function() {
		       assert.equal(0, kBuffer.compare(storage.get('buffer')));
		    });

		    // UPDATE
		   	it('should return undefined', function() {
				assert.equal(undefined, storage.set('buffer',kBuffer2));
		    }); 

		    it('should return same buffer', function() {
		       assert.equal(0, kBuffer2.compare(storage.get('buffer')));
		    });

		    // DELETE
			it('should return undefined', function() {
				assert.equal(undefined, storage.remove('buffer'));
		    });

			it('should return undefined', function() {
				assert.equal(undefined, storage.get('buffer'));
		    });		    
		   
	    });

	});
	
	describe('#update values with same key', function() {
		
		it('should return undefined', function() {
			assert.equal(undefined, storage.set('anytype','Hello'));
		});
		
		it('should return undefined', function() {
			assert.equal(undefined, storage.set('anytype',3.14));
		});

		it('should return 3.14', function() {
			assert.equal(3.14, storage.get('anytype'));
	    });

		it('should return undefined', function() {
			assert.equal(undefined, storage.set('anytype',true));
		});

		it('should return true', function() {
			assert.equal(true, storage.get('anytype'));
	    });

		it('should return undefined', function() {
			assert.equal(undefined, storage.set('anytype',obj));
		});

	    it('should return same object', function() {
		   assert.equal(JSON.stringify(obj), JSON.stringify(storage.get('anytype')));
	    });

		it('should return undefined', function() {
			assert.equal(undefined, storage.set('anytype',arr));
		});

	    it('should return same array', function() {
		   assert.equal(JSON.stringify(arr), JSON.stringify(storage.get('anytype')));
	    });

	   	it('should return undefined', function() {
			assert.equal(undefined, storage.set('anytype',kDate));
	    }); 

	    it('should return same date', function() {
	       assert.equal(kDate.getTime(), storage.get('anytype').getTime());
	    });

	   	it('should return undefined', function() {
			assert.equal(undefined, storage.set('anytype',kBuffer));
	    }); 

	    it('should return same buffer', function() {
	       assert.equal(0, kBuffer.compare(storage.get('anytype')));
	    });

		it('should return undefined', function() {
			assert.equal(undefined, storage.set('anytype','Hello'));
		});

		it('should return Hello', function() {
			assert.equal('Hello', storage.get('anytype'));
		});

	});	
	
	describe('#lock', function() {
		
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
	
	describe('#populate', function() {

		// REQUIRED FOR NEXT TESTS
	   	it('should return undefined', function() {
			assert.equal(undefined, storage.set('string','Hello'));
	    });

	   	it('should return undefined', function() {
			assert.equal(undefined, storage.set('number',3.14));
	    });

	   	it('should return undefined', function() {
			assert.equal(undefined, storage.set('bool',true));
	    });

	   	it('should return undefined', function() {
			assert.equal(undefined, storage.set('null',null));
	    });

	   	it('should return undefined', function() {
			assert.equal(undefined, storage.set('array',arr));
	    });

	   	it('should return undefined', function() {
			assert.equal(undefined, storage.set('object',obj));
	    });

	   	it('should return undefined', function() {
			assert.equal(undefined, storage.set('date', kDate));
	    });

	   	it('should return undefined', function() {
			assert.equal(undefined, storage.set('buffer', kBuffer));
	    });

	});

	describe('#open storage', function() {

		it('should return true', function() {
			assert.equal(true, storage_copy instanceof Object);
		});

		it('should return Hello', function() {
			assert.equal('Hello', storage_copy.get('string'));
		});
		
		it('should return 3.14', function() {
			assert.equal(3.14, storage_copy.get('number'));
		});
	
		it('should return true', function() {
			assert.equal(true, storage_copy.get('bool'));
		});
	
		it('should return null', function() {
			assert.equal(null,storage_copy.get('null'));
		});

	    it('should return same array', function() {
		   assert.equal(JSON.stringify(arr), JSON.stringify(storage_copy.get('array')));
	    });

	    it('should return same object', function() {
		   assert.equal(JSON.stringify(obj), JSON.stringify(storage_copy.get('object')));
	    });

	    it('should return same date', function() {
	       assert.equal(kDate.getTime(), storage_copy.get('date').getTime());
	    })

	    it('should return same buffer', function() {
	       assert.equal(0, kBuffer.compare(storage_copy.get('buffer')));
	    });

	});
	
	describe('#clear', function() {
		
		it('should return undefined', function() {
			assert.equal(undefined, storage.clear());
		});
		
		it('should return undefined', function() {
			assert.equal('undefined', typeof(storage.get('string')));
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

		it('should return undefined', function() {
			assert.equal('undefined', typeof(storage.get('date')));
		});

		it('should return undefined', function() {
			assert.equal('undefined', typeof(storage.get('buffer')));
		});
		
	});

	after(function() {
		Storage.destroy('basis_storage');
	});

});


