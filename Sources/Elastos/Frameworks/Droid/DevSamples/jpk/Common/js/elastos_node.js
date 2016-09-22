var sPackageName = process.argv[2];
//var sActivityName = process.argv[3];

var root = global||window;

root.Elastos = ( function () {
    var _Elastos = {
        CObject: {},

        CReflection: {},

        //Runtime_Native : _Runtime_Native,

        //Test : _Bridge_Native,

        //Runtime : _Runtime,

        Core : {
            Get : function () {
                return _Module_Core;
            },

            New : function () {
                var args = _bindArgs([_Module_Core], arguments);
                return Elastos.Runtime.createObject.apply(null,args);
            },

            CString : function (asText) {
                return Elastos.Core.New("Elastos.Core.CString",asText);
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
            //RootPath : "/data/temp/node/" + sPackageName,
            RootPath : "/data/temp/node/" + sPackageName.split(".").pop(),

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

                elog("========Run.begin.1========typeof _api.receive:" + typeof _api.receive);

                _Bridge_Native.Unlock();

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
    };   //Elastos
//elog("========elastos_node.js========begin====2====");

    var _bindArgs = function (aaArgs, aoArgs) {
        for (var i = 0, im = aoArgs.length; i < im; i++) aaArgs.push(aoArgs[i]);
        return aaArgs;
    };

    var _api = require('../../bin/elastos.node');

    var _Runtime_Native = _api.require("/data/temp/node/bin/Elastos.DevSamples.Node.CarRuntime.eco", "Elastos.DevSamples.Node.CarRuntime.CCarRuntime");

    _Elastos.Runtime_Native = _Runtime_Native;

    var _Runtime = require('./elastos_runtime.js')(_Elastos);

    var _Module_Core = _Runtime.getModuleInfo("/system/lib/Elastos.CoreLibrary.eco");
    var _Module_Droid = _Runtime.getModuleInfo("/system/lib/Elastos.Droid.Core.eco");
    var _Bridge_Native = _api.require("/data/elastos/" + sPackageName + ".eco", sPackageName + ".CNodeListener");

    var _Bridge = {
        init : function (aoNodeBridgeListener) {
            var pEnqueueUIMessage = _Bridge_Native.GetEnqueueUIMessagePtr();
            _api.SetEnqueueUIMessagePtr(pEnqueueUIMessage);

            var iNodeBridge = _api.GetNodeBridge();
            _Bridge_Native.SetNodeBridge(iNodeBridge,1);
            _Bridge_Native.SetNodeBridgeListener(aoNodeBridgeListener);
            //_Bridge_Native.Unlock();
        },
    };

    _Elastos.Test = _Bridge_Native;

    _Elastos.Runtime = _Runtime;

    _Elastos.CObject.showMethods = function(aoCar,asTag) {
        var a = [];
        for (var p in aoCar){
            var bAdd = true;
            if (typeof asTag == "string") {
                if (p.indexOf(asTag) < 0) bAdd = false;
            }
            if (bAdd) a.push(p);
        }
        var s = "====methods====[" + a.join("][") + "]";
        elog(s);
    }

    _Elastos.CObject.getClassInfo = function(aoCar) {
        return _Runtime_Native.Test_CObject_ReflectClassInfo(aoCar);
    }
    _Elastos.CObject.hasInterface = function(aoCar, asName) {
        var hasInterface = false;

        var oClassInfo = this.getClassInfo(aoCar);
        var oInterfaces = oClassInfo.GetAllInterfaceInfos();
        for (var i=0,im=oInterfaces.length;i<im;i++){
            var sName = oInterfaces[i].GetName();
            if (sName == asName) {
                hasInterface = true;
                break;
            }
        }
        return hasInterface;
    }

    return _Elastos;
} )();

root.Application = Elastos.Application;

Application.NodeBridgeListener = {
    OnRegisterActivity : function(asPackageName, asActivityName, aoActivityInstance, aoActivityListener, aoActivityHandler, out_abResult) {
        var _this = Elastos.Application;

        _this.R = require(_this.RootPath + "/R.js");

        var sFileName = _this.RootPath + "/" + asActivityName + ".js";
        var oActivity = {
            Name : asActivityName,
            ActivityInstance : aoActivityInstance,
            ActivityHandler : aoActivityHandler,
        };

        var oActivityListener = require(sFileName)(Elastos, oActivity);
        oActivity.ActivityListener = oActivityListener;
        _this.Activities.push(oActivity);

        Elastos.Test.SetActivityListener(aoActivityListener, oActivityListener);

        out_abResult.data = true;
    },

    OnRegisterCalculatorEditText : function(aoContext, aoControl, aoListener, out_abResult) {
        elog("OnRegisterCalculatorEditText======begin======");

        var sPkgName = aoContext.GetPackageName();
        var sClsName = Elastos.CObject.getClassInfo(aoControl).GetName();
        sClsName = sClsName.slice(-sClsName.length+1);

        var sFileName = sPkgName.split(".").join("/");
        sFileName = "/data/temp/node/Common/js/" + sFileName + "/" + sClsName + ".js";

        elog("OnRegisterCalculatorEditText======filename======"+sFileName);

        var aPath = sPkgName.split(".");
        var sPath;
        var oPath = root;
        for (var i=0,im=aPath.length; i<im; i++) {
            sPath = aPath[i];
            oPath = oPath[sPath] || (oPath[sPath] = {});
        }

        //var oListener = require(sFileName)(Elastos, oActivity);
        var oListener = require(sFileName)(Elastos, aoContext);
        oPath[sClsName] = oListener;

        Elastos.Test.SetCalculatorEditTextListener(aoListener, oListener);

        out_abResult.data = true;

        elog("OnRegisterCalculatorEditText======end======");
    },

    OnRegisterCalculatorPadViewPager : function(aoContext, aoControl, aoListener, out_abResult) {
        elog("OnRegisterCalculatorPadViewPager======begin======");

        var sPkgName = aoContext.GetPackageName();
        var sClsName = Elastos.CObject.getClassInfo(aoControl).GetName();
        sClsName = sClsName.slice(-sClsName.length+1);

        var sFileName = sPkgName.split(".").join("/");
        sFileName = "/data/temp/node/Common/js/" + sFileName + "/" + sClsName + ".js";

        elog("OnRegisterCalculatorPadViewPager======filename======"+sFileName);

        var aPath = sPkgName.split(".");
        var sPath;
        var oPath = root;
        for (var i=0,im=aPath.length; i<im; i++) {
            sPath = aPath[i];
            oPath = oPath[sPath] || (oPath[sPath] = {});
        }

        //var oListener = require(sFileName)(Elastos, oActivity);
        var oListener = require(sFileName)(Elastos, aoContext);
        oPath[sClsName] = oListener;

        Elastos.Test.SetCalculatorPadViewPagerListener(aoListener, oListener);

        out_abResult.data = true;

        elog("OnRegisterCalculatorPadViewPager======end======");
    },

    OnRegisterCalculatorPadLayout : function(aoContext, aoControl, aoListener, out_abResult) {
        elog("OnRegisterCalculatorPadLayout======begin======");

        var sPkgName = aoContext.GetPackageName();
        var sClsName = Elastos.CObject.getClassInfo(aoControl).GetName();
        sClsName = sClsName.slice(-sClsName.length+1);

        var sFileName = sPkgName.split(".").join("/");
        sFileName = "/data/temp/node/Common/js/" + sFileName + "/" + sClsName + ".js";

        elog("OnRegisterCalculatorPadLayout======filename======"+sFileName);

        var aPath = sPkgName.split(".");
        var sPath;
        var oPath = root;
        for (var i=0,im=aPath.length; i<im; i++) {
            sPath = aPath[i];
            oPath = oPath[sPath] || (oPath[sPath] = {});
        }

        //var oListener = require(sFileName)(Elastos, oActivity);
        var oListener = require(sFileName)(Elastos, aoContext);
        oPath[sClsName] = oListener;

        Elastos.Test.SetCalculatorPadViewPagerListener(aoListener, oListener);

        out_abResult.data = true;

        elog("OnRegisterCalculatorPadLayout======end======");
    },
};

Application.Ready();