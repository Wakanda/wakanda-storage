
var path = require('path');
var childProcess = require('child_process');

try {
	const execRelativePath = (process.platform === 'win32') ? 'build/Release/' : 'build/';
	const execName = (process.platform === 'win32') ? 'cpp-tests.exe' : 'cpp-tests';

    childProcess.execFileSync(
        path.join(__dirname, execRelativePath, execName),
        [],
        {
            cwd: path.join(__dirname, execRelativePath),
            stdio: 'inherit'
        }
    );
}
catch(err) {
    process.exit(1);
}