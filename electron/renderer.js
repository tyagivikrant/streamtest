/**
 * This file is loaded via the <script> tag in the index.html file and will
 * be executed in the renderer process for that window. No Node.js APIs are
 * available in this process because `nodeIntegration` is turned off and
 * `contextIsolation` is turned on. Use the contextBridge API in `preload.js`
 * to expose Node.js functionality from the main process.
 */

// loading obs.node module and calling few functions
// created context aware module since without that not even able to load the module
// still getting error that node module and electron application were built using different version of node
// Version 127 and version 130, I just installed the latest node and node-gyp
// don't know how to fix that :(

const obsnode = require('./obsnode/obs.node');
console.log(obsnode.Init());
console.log(obsnode.ObsStart());
console.log(obsnode.ObsEnd());
console.log(obsnode.ObsStream());


// tried using import when 'require' was throwing error as not context aware module, still didn't work :(
//import { Init, ObsStart, ObsEnd, ObsStream } from './obsnode/obs.node';
//console.log(Init());
//console.log(ObsStart());
//console.log(ObsEnd());
//console.log(ObsStream());
