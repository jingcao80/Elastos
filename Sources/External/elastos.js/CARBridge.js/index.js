
let bindings = require('bindings');

let CARBridge = bindings('CARBridge');


module.exports = {
  'version': CARBridge.version,
  'require': CARBridge.require
};

