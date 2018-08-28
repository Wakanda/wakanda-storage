
var path = require('path');
var childProcess = require('child_process');

try {
    childProcess.execFileSync(
        path.join(__dirname, 'build/Release/', 'cpp-tests.exe'),
        [],
        {
            cwd: path.join(__dirname, 'build/Release'),
            stdio: 'inherit'
        }
    );
}
catch(err) {
    process.exit(1);
}