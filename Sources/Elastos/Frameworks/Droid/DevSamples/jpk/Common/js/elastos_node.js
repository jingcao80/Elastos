whmlog('=====================elasto_node.js begin===================================');

var sPackageName = process.argv[2];
//var sActivityName = process.argv[3];

var root = global||window;

root.Elastos = ( function () {

    var _api = require('./elastos.node');

    var _Runtime = require('./elastos_runtime.js')(_api);
    var _Module_Core = _Runtime.getModuleInfo("/system/lib/Elastos.Core.eco");
    var _Module_Droid = _Runtime.getModuleInfo("/system/lib/Elastos.Droid.Core.eco");

    var _Bridge_Native = _api.require("/data/elastos/" + sPackageName + ".eco", "CTestEventListener");
    var _Bridge = {
        init : function (aoNodeBridgeListener) {
            var pEnqueueUIMessage = _Bridge_Native.GetEnqueueUIMessagePtr();
            _api.SetEnqueueUIMessagePtr(pEnqueueUIMessage);

            var iNodeBridge = _api.GetNodeBridge();
            _Bridge_Native.SetNodeBridge(iNodeBridge,1);
            _Bridge_Native.SetNodeBridgeListener(aoNodeBridgeListener);
            _Bridge_Native.Unlock();
        },
    };

    var _bindArgs = function (aaArgs, aoArgs) {
        for (var i = 0, im = aoArgs.length; i < im; i++) aaArgs.push(aoArgs[i]);
        return aaArgs;
    };

    return {
        Test : _Bridge_Native,

        Runtime : _Runtime,

        Core : {
            Get : function () {
                return _Module_Core;
            },

            New : function () {
                var args = _bindArgs([_Module_Core], arguments);
                return Elastos.Runtime.createObject.apply(null,args);
            },

            CString : function (asText) {
                return Elastos.Core.New("CStringWrapper",asText);
            },
        },

        Droid : {
            Get : function () {
                return _Module_Droid;
            },

            New : function () {
                var args = _bindArgs([_Module_Droid], arguments);
                return Elastos.Runtime.createObject.apply(null,args);
            },
        },

        Application : {
            PackageName : sPackageName,
            RootPath : "/data/temp/node/" + sPackageName,

            R : null,

            Activities : [],
            Services : [],
            Providers : [],
            Receivers : [],

            //Activity-alias : [],
            //Meta-datas : [],
            //Permissions : [],
            //Uses-libraries : [],  ECO/NPM

            Run : function () {
                var assert = require('assert');
                var called = false;

                process.on('exit', function () {
                    assert(called);
                });

                _api.receive(5, function (err, val) {
                    assert.equal(null, err);
                    //assert.equal(10, val);
                    process.nextTick(function () {
                        called = true;
                    });
                });
            },

            Ready : function () {
                _Bridge.init(this.NodeBridgeListener);
                this.Run();
            },
        },
    }
} )();

root.Application = Elastos.Application;

Application.NodeBridgeListener = {
    OnRegistActivity : function(asPackageName, asActivityName, aoActivityInstance, aoActivityListener, aoActivityHandler, out_abResult) {
    //OnRegistActivity : function(asPackageName, asActivityName, aoActivityInstance, out_aoActivityListener, aoActivityHandler, out_abResult) {
        var _this = Elastos.Application;

        _this.R = require(_this.RootPath + "/R.js");

        var sFileName = _this.RootPath + "/" + asActivityName + ".js";
        var oActivity = {
            Name : asActivityName,
            ActivityInstance : aoActivityInstance,
            ActivityHandler : aoActivityHandler,
        };
        oActivityListener = require(sFileName)(Elastos, oActivity);
        oActivity.ActivityListener = oActivityListener;
        _this.Activities.push(oActivity);

        whmlog('=======================OnRegistActivity.SetActivityListener.begin=================================');

        //Elastos.Test.SetActivityListener(oActivityListener);
        Elastos.Test.SetActivityListener(aoActivityListener, oActivityListener);
        //out_aoActivityListener.data = oActivityListener;

        whmlog('=======================OnRegistActivity.SetActivityListener.end=================================');

        out_abResult.data = true;
    },
};

Application.Ready();

whmlog('=====================elasto_node.js end===================================');
