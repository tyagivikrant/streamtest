// obs-nm.js js file for testing
// syntex: node obs-nm.js

const obsnode = require('./build/Release/obs.node');

console.log(obsnode.Init());

console.log(obsnode.ObsStart());

console.log(obsnode.ObsEnd());

console.log(obsnode.ObsStream());
