const assert = require('assert');
const cluster = require('cluster');
const numCPUs = require('os').cpus().length;
const storageMgr = require('..');

var storage = null;
var stoppedWorkersCount = 0;


var realCode = function realCode ( callback) {
		
		if (cluster.isMaster) {
			
			var exitTimeout = null;
			var stoppedWorkersInterval = null;
			
			var beforeExit = function beforeExit() {
				
				var workersCount = storage.get('workersCount');
				var workersInfos = storage.get('workersInfos');
				
				var findWorkerInfos = function findWorkerInfos(id) {
					var infos = null;
					for (var iter = 0, len = workersInfos.length ; (iter < len) && (infos == null) ; ++iter) {
						if (workersInfos[iter].id == id) {
							infos = workersInfos[iter];
						}
					}
					return infos;
				};

				for (const id in cluster.workers) {
					var infos = findWorkerInfos(id);
					if (infos == null) {
						throw (new Error('cannot found infos for worker ' + id.toString()));
					}
				}
				
				

				storageMgr.destroy('masterSharedStorage');
				process.exit();
			
			}
			
			var exitTimeoutCallback = function exitTimeoutCallback() {
				clearInterval(stoppedWorkersInterval);
				cluster.disconnect(beforeExit);
			};
			
			var checkForStoppedWorkers = function checkForStoppedWorkers() {
				
				if (stoppedWorkersCount == numCPUs) {
					clearTimeout(exitTimeout);
					clearInterval(stoppedWorkersInterval);
					beforeExit();

				}
			};

			for (let i = 0; i < numCPUs; i++) {
				cluster.fork();
			}

			for (const id in cluster.workers) {
				cluster.workers[id].on('message', function(msg) {
					if (msg.name && msg.name == 'WORKER_WILL_STOP') {
						++stoppedWorkersCount;
					}
				});
			}

			stoppedWorkersInterval = setInterval(checkForStoppedWorkers, 500);
			exitTimeout = setTimeout(beforeExit, 10000);
			
			
			
		
		
		}else {
			
			storage = storageMgr.get('masterSharedStorage');
			storage.lock()
			
			var workersCount = storage.get('workersCount');
			if (typeof(workersCount) == 'undefined') {
				workersCount = 1;
			}else {
				++workersCount;
			}
			
			storage.set('workersCount', workersCount);
			
			var infos = {
				'id': cluster.worker.id,
				'pid': process.pid,
				'type': 'worker'
			};
			
			var workersInfos = storage.get('workersInfos');
			
			if (typeof(workersInfos) == 'undefined') {
				workersInfos = [infos];
			}else {
				workersInfos.push(infos);
			}
			
			storage.set('workersInfos', workersInfos);
			
			storage.unlock();

			process.send({
				'name': 'WORKER_WILL_STOP',
				'id': cluster.worker.id,
				'pid': process.pid
			});
			
		}
		callback();
};




	
describe('Wakanda storage cluster', function() {
    
	// Before 
	before(function(done) {
		
		storageMgr.destroy('masterSharedStorage');
		storage = storageMgr.create('masterSharedStorage');
		done();
	});
	
	
	describe('#check for stopped workers ', function() {
		it('should return true', function(done) {
			this.timeout(20000);
			realCode(function (){
				assert.equal(numCPUs, stoppedWorkersCount);  
				done();
			});
			
		});
		
	});
	
	// End 	
	after(function(done) {
		storageMgr.destroy('storageMgr');
		done();
	});
	
});



