//var _api = require('./elastos.node');
//var elastos = _api.require("./Elastos.DevSamples.Node.CarRuntime.eco", "Elastos.DevSamples.Node.CarRuntime.CCarRuntime");

var module = elastos.require('./Demo.eco');
var obj = module.createObject('CDemo');
obj.addEventHandler({
    OnEvent:function(i) {
        var s = 'OnEvent, i: ' + JSON.stringify(i);
        elastos.log(s);
    },
});
obj.doTask();