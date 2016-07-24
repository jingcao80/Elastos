var eventHandler = (function() {
    return {
        OnEvent:function(i) {
            var s = 'OnEvent, i: ' + JSON.stringify(i);
            elastos.log(s);
        },
    }
})();

var module = elastos.require('./Demo.eco');
var obj = module.createObject('CDemo');
obj.addEventHandler(eventHandler);
obj.doTask();
