const cluster = require('cluster');
const numCPUs = require('os').cpus().length;
const storageMgr = require('..');
var expect = require('expect.js');

if (cluster.isMaster) {

    describe('Wakanda Storage Cluster', function() {

        var storage = null;

        before(function() {
            storage = storageMgr.create('masterSharedStorage');
        });

        it("check cluster behaviour", function() {
            var testPromise = new Promise(function(resolve, reject) {
                var exitTimeout = null;
                var stoppedWorkersInterval = null;
                var stoppedWorkersCount = 0;

                var checkForStoppedWorkers = function checkForStoppedWorkers() {
                    if (stoppedWorkersCount == numCPUs) {
                        clearTimeout(exitTimeout);
                        clearInterval(stoppedWorkersInterval);
                        resolve();
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
                exitTimeout = setTimeout(resolve, 10000);
            });

            return testPromise.then(function(){
                var workersCount = storage.get('workersCount');
                describe('count workers', function() {
					it('#should return number ', function() {
						expect(workersCount).to.be.a('number');
					});
				});
				
				describe('workers count and CPU', function() {
					it('#should be equal ', function() {
						expect(workersCount).to.be(numCPUs);
					});
				});
                

                var workersInfos = storage.get('workersInfos');
				describe('workers info type', function() {
					it('#should be an array ', function() {
						expect(workersInfos).to.be.an('array');
					});
				});
                
				describe('workers info and number of CPU', function() {
					it('#should be equal ', function() {
						expect(workersInfos.length).to.be(numCPUs);         
					});
				});
                
                
                      

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
					describe('worker info', function() {
						it('#should not be null ', function() {
							expect(infos).not.to.be(null);        
						});
					});
                
                    
                }
            });
			
        });        

        after(function() {
            storageMgr.destroy('masterSharedStorage');
        });         
    });
	
}else {
    
    var storage = storageMgr.get('masterSharedStorage');
    storage.lock();
	describe('open existing storage', function() {
		it('#should be a valide object ', function() {
			expect(storage).not.to.be(null);         
		});
	});

    var workersCount = storage.get('workersCount');
    if (typeof(workersCount) == 'undefined') {
        workersCount = 1;
    }
    else {
        ++workersCount;
    }
	describe('count workers', function() {
		it('#should at least get one worker ', function() {
			expect(workersCount).not.to.be(0);         
		});
	});

    storage.set('workersCount', workersCount);

    var infos = {
        'id': cluster.worker.id,
        'pid': process.pid,
        'type': 'worker'
    };
    var workersInfos = storage.get('workersInfos');
    if (typeof(workersInfos) == 'undefined') {
        workersInfos = [infos];
    }
    else {
        workersInfos.push(infos);
    }
	describe('set workers info', function() {
		it('#should not to be empty ', function() {
			expect(workersInfos).to.not.be.empty();       
		});
	});
    storage.set('workersInfos', workersInfos);

    storage.unlock();

    process.send({
        'name': 'WORKER_WILL_STOP',
        'id': cluster.worker.id,
        'pid': process.pid
    });
}