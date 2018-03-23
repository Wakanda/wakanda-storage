
const cluster = require('cluster');
const numCPUs = require('os').cpus().length;
const binary = require('node-pre-gyp');
const path = require('path')
const binding_path = binary.find(path.resolve(path.join(__dirname,'./../package.json')));
const storageMgr = require(binding_path);

if (cluster.isMaster) {

	var storage = storageMgr.create('masterSharedStorage');

	var exitTimeout = null;
	var stoppedWorkersInterval = null;
	var stoppedWorkersCount = 0;

	var beforeExit = function beforeExit() {

		try {
			var workersCount = storage.get('workersCount');
			if (typeof(workersCount) == 'undefined') {
				throw (new Error('cannot found \'workersCount\' item into storage'));
			}
			if (workersCount != numCPUs) {
				throw (new Error('unexpected \'workersCount\' item value'));
			}

			var workersInfos = storage.get('workersInfos');
			if (typeof(workersInfos) == 'undefined') {
				throw (new Error('cannot found \'workersInfos\' item into storage'));
			}
			if (!Array.isArray(workersInfos)) {
				throw (new Error('unexpected \'workersInfos\' item type'));
			}
			if (workersInfos.length != numCPUs) {
				throw (new Error('unexpected \'workersInfos\' item length'));
			}
			
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
		catch (e) {
			storageMgr.destroy('masterSharedStorage');
			throw (e);
		}
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
}
else {
	
	var storage = storageMgr.get('masterSharedStorage');
	storage.lock();

	var workersCount = storage.get('workersCount');
	if (typeof(workersCount) == 'undefined') {
		workersCount = 1;
	}
	else {
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
	}
	else {
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