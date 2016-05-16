whmlog('=====================test_basic.js begin===================================');

var bNode = true;

var root = global||window;
root.ElastosTest || (root.ElastosTest = {});

if (bNode) {
    root.elastos = require('./elastos.node');
}

var R = {
    attr: {
    },
    drawable: {
        ic_sysbar_back : 0x7f020000,
        ic_sysbar_home : 0x7f020001,
        ic_sysbar_menu : 0x7f020002,
        icon : 0x7f020003,
    },
    id: {
        AnamtionButton : 0x7f050010,
        ConnectivityManagerButton : 0x7f050014,
        DialogButton : 0x7f050011,
        PopupWindowButton : 0x7f050012,
        PowerManagerButton : 0x7f050013,
        back : 0x7f050019,
        btn_close_popup : 0x7f05001c,
        button1 : 0x7f050004,
        button2 : 0x7f050006,
        button3 : 0x7f050005,
        buttonPanel : 0x7f050003,
        chkAndroid : 0x7f05000a,
        chkIos : 0x7f050009,
        contentPanel : 0x7f050001,
        home : 0x7f050018,
        menu : 0x7f050017,
        message : 0x7f050002,
        myEditText : 0x7f050008,
        myListView : 0x7f050015,
        myTextView : 0x7f050007,
        nav_buttons : 0x7f050016,
        parentPanel : 0x7f050000,
        popup_element : 0x7f05001a,
        radioFemale : 0x7f05000d,
        radioMale : 0x7f05000c,
        radioSex : 0x7f05000b,
        textview1 : 0x7f05001d,
        textview2 : 0x7f05001e,
        toggleButton1 : 0x7f05000e,
        toggleButton2 : 0x7f05000f,
        txtView : 0x7f05001b,
    },
    layout: {
        dialog : 0x7f030000,
        list_item : 0x7f030001,
        main : 0x7f030002,
        navigationbar : 0x7f030003,
        popupwindow : 0x7f030004,
        view : 0x7f030005,
    },
    string: {
        app_name : 0x7f040000,
    },
};


//-----------------------------define---------begin-------------------------------------

whmlog('=====================fnTestReflection begin===============1====================');

var oLog = elastos.require("/data/elastos/JSTestLog.eco", "CTestLog");
var oReflection = elastos.require("/data/elastos/JSTestReflection.eco", "CTestReflection");
var oCarRoot = elastos.require("/data/elastos/JSCarRoot.eco", "CCarRoot");
var oTestEventListener = elastos.require("/data/elastos/TextViewDemo.eco", "CTestEventListener");
var pEnqueueUIMessage = oTestEventListener.GetEnqueueUIMessagePtr();
elastos.SetEnqueueUIMessagePtr(pEnqueueUIMessage);
var oActivity = oTestEventListener.GetMainActivity();

whmlog('=====================NodeBridge test===============0====================');
var iNodeBridge = elastos.GetNodeBridge();
whmlog('=====================NodeBridge test===============1====================' + iNodeBridge +' '+ typeof(iNodeBridge));
oTestEventListener.SetNodeBridge(iNodeBridge,1);
whmlog('=====================NodeBridge test===============2====================');

//-----------------------------define---------end-------------------------------------


    var oModuleinfo_Elastos_Core = oCarRoot.Test_Require_ModuleInfo("/system/lib/Elastos.Core.eco");

    function CString (asText) {
        return classinfo__createObject(oModuleinfo_Elastos_Core,"CStringWrapper",asText);
    }


//-----------------------------config---------begin-------------------------------------


var a = [];
var s = "";
var p = 0;


var CarDataType = {
        Int16 : 1,
        Int32 : 2,
        Int64 : 3,
        Byte : 4,
        Float : 5,
        Double : 6,
        Char8 : 7,
        Char16 : 8,
        Char32 : 9,
        CString : 10,
        String : 11,
        Boolean : 12,
        EMuid : 13,
        EGuid : 14,
        ECode : 15,
        LocalPtr : 16,
        LocalType : 17,
        Enum : 18,
        StringBuf : 19,
        ArrayOf : 20,
        BufferOf : 21,
        MemoryBuf : 22,
        CppVector : 23,
        Struct : 24,
        Interface : 25,
};

var CarToJsDataTypeMap = {
    boolean:[
        CarDataType.Boolean,
    ],
    number:[
        CarDataType.Int16,
        CarDataType.Int32,
        CarDataType.Int64,
        CarDataType.Byte,
        CarDataType.Float,
        CarDataType.Double,
        CarDataType.ECode,
        CarDataType.Enum,
    ],
    string:[
        CarDataType.Char8,
        CarDataType.Char16,
        CarDataType.Char32,
        CarDataType.CString,
        CarDataType.String,
        CarDataType.StringBuf,
    ],
    array:[
        CarDataType.ArrayOf,
        CarDataType.BufferOf,
        CarDataType.MemoryBuf,
        CarDataType.CppVector,
    ],
    object:[
        CarDataType.EMuid,
        CarDataType.EGuid,
        CarDataType.LocalPtr,
        CarDataType.LocalType,
        CarDataType.Struct,
        CarDataType.Interface,
    ]
};

var ParamIOAttribute = {
        In : 0,
        CalleeAllocOut : 1,
        CallerAllocOut : 2,
}

function getDataTypeCarString(ai_datatype){
    var sRet = 'Not Defined DataType';

    //whmlog('=========getDataTypeCarString====end====CarDataType:'+typeof(CarDataType));

    for(var p in CarDataType) {
        //whmlog('=========getDataTypeCarString========'+p+'=='+CarDataType[p]+'=='+ai_datatype);
        if (CarDataType[p] == ai_datatype) {
            sRet = p;
            break;
        }
    }
    //whmlog('=========getDataTypeCarString====end====sRet:'+sRet);
    return sRet;
}

function getDataTypeJavascriptString(ai_datatype){
    var sRet = 'Not Defined DataType';

    var bFound = false;
    for(var p in CarToJsDataTypeMap) {
        var a = CarToJsDataTypeMap[p];
        for(var i = 0,im = a.length; i<im; i++) {
            if (a[i] == ai_datatype) {
                sRet = p;
                bFound = true;
                break;
            }
        }
        if (bFound) {
            break;
        }
    }
    return sRet;
}

function logs(a,tag){
    whmlog('==============logs.begin=================');
    var j=-1;
    var b=[];
    var s;
    for(var i=0,im=a.length;i<im;i++){
        s = a[i];
        if(tag && s.indexOf(tag)<0)continue;
        b.push(s);
        j++;
        if(j>18){
            oLog.Log('logs.'+(i-j)+'====: '+b.join('=='));
            j=-1;
            b=[];
        }
    }
    oLog.Log('logs.'+(i-j)+'====: '+b.join('=='));
    whmlog('==============logs.end=================');
}

function exist(a,element){
    var bExist = false;

    for (var i=0,im=a.length;i<im;i++) {
        if (a[i] == element) {
            bExist = true;
            break;
        }
    }

    return bExist;
}

//function showClassConstructorInfos(sEcoName,sClassName){
function showClassConstructorInfos(oModuleInfo,oClassInfo){
    whmlog('==============showClassConstructorInfos========0=========');
    if(typeof(oModuleInfo)=='string') {
        oModuleInfo = oCarRoot.Test_Require_ModuleInfo(oModuleInfo);
    }
    if(typeof(oClassInfo)=='string') {
        oClassInfo = oModuleInfo.GetClassInfo(oClassInfo);
    }

    whmlog('==============showClassConstructorInfos========1=========');

    //var oModuleInfo = oCarRoot.Test_Require_ModuleInfo(sEcoName);
    //var oClassInfo = oModuleInfo.GetClassInfo(sClassName);
    var aConstructorInfos = oClassInfo.GetAllConstructorInfos();
    var aInfos = [];

    whmlog('==============showClassConstructorInfos========2=========');
    for (var i=0,im=aConstructorInfos.length;i<im;i++){
        var constructorInfo = aConstructorInfos[i];
        var aParamInfos = constructorInfo.GetAllParamInfos();
        var aParamNames = [];
        for(var j=0,jm=aParamInfos.length;j<jm;j++){
            var paramInfo = aParamInfos[j];

            var oTypeInfo = paramInfo.GetTypeInfo();
            var paramName = oTypeInfo.GetName();
            var iDataType = oTypeInfo.GetDataType();
            var sCarDataType = getDataTypeCarString(iDataType);

            aParamNames.push(paramName+':'+sCarDataType);

            //aParamNames.push(paramInfo.GetName());
        }
        aInfos.push('======'+i+'========ssss.9.1.3=====constructor------'+constructorInfo.GetName()+'['+aParamNames.join('/')+']');
    }
    whmlog('==============showClassConstructorInfos========3=========');

    for (var i=0,im=aInfos.length;i<im;i++) {
        whmlog(aInfos[i]);
    }
    whmlog('==============showClassConstructorInfos========4=========');
}

function getClassNames(sEcoName) {
    var aClassNames = [];

    var oModuleInfo = (typeof(sEcoName)=='object')?sEcoName : oCarRoot.Test_Require_ModuleInfo(sEcoName);
    var oClassInfos = oModuleInfo.GetAllClassInfos();
    for(var i=0,im=oClassInfos.length;i<im;i++){
        var sName = oClassInfos[i].GetName();
        aClassNames.push(sName);
    }

    return aClassNames;
}

function getPropertyNames(o) {
    var a = [];
    for(var p in o) {
        a.push(p);
    }
    return a;
}

function existClass(sEcoName,sClassName){
    var bExist = false;

    var aClassNames = getClassNames(sEcoName);
    bExist = exist(aClassNames, sClassName);

    return bExist;
}

function clsFactory(aoCar,aoBase){
    var clsNew = function(){
        this.car = aoCar;
        this.base = aoBase;

        for (var prop in this.car) {
            this[prop] = (function(_this,prop){
                return function() {
                    return _this.car[prop].apply(_this.car,arguments);
                }
            })(this,prop);

            if(prop.indexOf('Get') == 0 || prop.indexOf('Set') ==0) {
                //set property accessor
            }
        }
    };

    clsNew.prototype = aoBase;

    return clsNew;
}

    function getWrapper(aoCar,aoBase){
        var oClass = clsFactory(aoCar,aoBase);
        var oWrapper = new oClass();
        return oWrapper;
    }

    function getModuleInfo(sEcoName){
        var oCar = oCarRoot.Test_Require_ModuleInfo(sEcoName);
        var oRet = getWrapper(oCar,{
            name:'ModuleInfo'
        });
        return oRet;
    }

    function getClassInfo(sEcoName,sClassName){
        var oModuleInfo = getModuleInfo(sEcoName);

        var oCar = oModuleInfo.GetClassInfo(sClassName);
        var oRet = getWrapper(oCar,{
            name:'ClassInfo'
        });
        return oRet;
    }

    function classinfo__createObject(oModuleInfo,oClassInfo){
        var newObject;

        //whmlog('==============classinfo__createObject========000=========module:'+typeof(oModuleInfo)+'=='+'class:'+oClassInfo);

        if(typeof(oModuleInfo)=='string') {
            oModuleInfo = oCarRoot.Test_Require_ModuleInfo(oModuleInfo);
        }
        //whmlog('==============classinfo__createObject========0.1========='+typeof(oModuleInfo));
        if(typeof(oClassInfo)=='string') {
            oClassInfo = oModuleInfo.GetClassInfo(oClassInfo);
        }
        //whmlog('==============classinfo__createObject========0.2=========');

        var length = arguments.length;
        if(length==2){
            //whmlog('==============classinfo__createObject========0.3=========');
            newObject = oClassInfo.CreateObject();
        }
        else {

            //whmlog('==============classinfo__createObject========1=========length:'+length);

            showClassConstructorInfos(oModuleInfo,oClassInfo);

            //whmlog('==============classinfo__createObject========1.1=========');
            var aConstructorInfos = oClassInfo.GetAllConstructorInfos();
            //whmlog('==============classinfo__createObject========1.2========='+aConstructorInfos.length);

if(0==1){
//var _oModuleInfo = oCarRoot.Test_Require_ModuleInfo("/data/elastos/JSCarRoot.eco");
//var _oClassInfo = _oModuleInfo.GetClassInfo("CTestStructInfo");
var _oModuleInfo = oCarRoot.Test_Require_ModuleInfo("/system/lib/Elastos.Droid.Core.eco");
var _oClassInfo = _oModuleInfo.GetClassInfo("CPopupWindow");
var _aConstructorInfos = _oClassInfo.GetAllConstructorInfos();
var _oConstructorInfo = _aConstructorInfos[0];
var aaa=getPropertyNames(_oConstructorInfo);
logs(aaa);
whmlog('==============classinfo__createObject========test 1.2.0========='+typeof(_oConstructorInfo.GetClassInfo));
var _oClassInfoA = _oConstructorInfo.GetClassInfo();
whmlog('==============classinfo__createObject========test 1.2.1========='+typeof(_oClassInfoA.GetName));
whmlog('==============classinfo__createObject========test 1.2.2========='+_oClassInfoA.GetName());
}   //0==1

            var oConstructorInfo;
            var paramCount = length - 2;
            for(var i=0, im=aConstructorInfos.length; i<im; i++){
                //whmlog('==============classinfo__createObject========1.3========='+i);
                oConstructorInfo = aConstructorInfos[i];
                var _paramCount = oConstructorInfo.GetParamCount();
                if (_paramCount == paramCount) {
                    var bSameArgs = true;
                    var aParamInfos = oConstructorInfo.GetAllParamInfos();
                    //whmlog('==============classinfo__createObject========1.3.0=========param num:'+aParamInfos.length);
                    for(var j = 0, jm = paramCount; j<jm; j++) {
                        //whmlog('==============classinfo__createObject========1.4.0========='+i+'=='+j);
                        var paramInfo = aParamInfos[j];
                        //whmlog('==============classinfo__createObject========1.4.1========='+i+'=='+j+'=='+typeof(paramInfo.GetTypeInfo));
                        var oTypeInfo = paramInfo.GetTypeInfo();
                        //whmlog('==============classinfo__createObject========1.4.2========='+i+'=='+j);

                        var iDataType = oTypeInfo.GetDataType();
                        var sCarDataType = getDataTypeCarString(iDataType);
                        var sJsDataType = getDataTypeJavascriptString(iDataType);
                        //whmlog('==============classinfo__createObject========1.4.3========='+i+'=='+j);

                        var arg_in = arguments[j+2];
                        var type_in = typeof(arg);
                        //whmlog('==============classinfo__createObject========1.4.4========='+i+'=='+j);

                        if (sJsDataType == type_in) continue;

                        bSameArgs = false
                        break;
                    }
                    //var bSameArgs = (j==jm);
                    if (bSameArgs) break;
                }
                //else {
                //    oConstructorInfo = null;
                //}
            }
            //whmlog('==============classinfo__createObject========2=========');
            var bFoundConstructor = (i < im);
            if (bFoundConstructor) return null;
            //whmlog('==============classinfo__createObject========3=========');

            //var oConstructorInfo = aConstructorInfos[length-3]; //to be fixed

            var aParamInfos = oConstructorInfo.GetAllParamInfos();
            //whmlog('==============classinfo__createObject========4=========');

            var oArgumentList = oConstructorInfo.CreateArgumentList();
            //whmlog('==============classinfo__createObject========5=========');

            //for(var i=0,im=length-2;i<im;i++) {
            for(var i = 0, im = paramCount; i<im; i++) {
                //whmlog('==============classinfo__createObject========6========='+i+'/'+im);
                var paramInfo = aParamInfos[i];

                //whmlog('==============classinfo__createObject========6.0========='+paramInfo.GetName());

                var oTypeInfo = paramInfo.GetTypeInfo();
                //whmlog('==============classinfo__createObject========6.1========='+typeof(oTypeInfo));
                //whmlog('==============classinfo__createObject========6.2========='+typeof(oTypeInfo.GetDataType));

                var iDataType = oTypeInfo.GetDataType();
                var sCarDataType = getDataTypeCarString(iDataType);
                var sJsDataType = getDataTypeJavascriptString(iDataType);

                //whmlog('==============classinfo__createObject========6.3===='+i+'====='+oTypeInfo.GetName()+'===='+iDataType+'===='+sCarDataType+'===='+sJsDataType);

//var aa = getPropertyNames(paramInfo);
//logs(aa);

//                whmlog('==============classinfo__createObject========7========='+i+'/'+iDataType);

//var aa = getPropertyNames(oTypeInfo);
//logs(aa);

                //whmlog('==============classinfo__createObject========8========='+i+'/'+iDataType);

if(1==0){
                var arg = arguments[i+2];
                var type = typeof(arg);
}
//SetInputArgumentOf...
//Int16/Int32/Int64/Byte/Float/Double/Char/String/Boolean/EMuid/EGuid/ECode/LocalPtr/LocalType/Enum/CarArray/StructPtr/ObjectPtr

if(1==1){
                var arg = arguments[i+2];
                switch (iDataType) {
                    case CarDataType.Int16:
                        oArgumentList.SetInputArgumentOfInt16(i,arg);
                        break;
                    case CarDataType.Int32:
                        oArgumentList.SetInputArgumentOfInt32(i,arg);
                        break;
                    case CarDataType.Int64:
                        oArgumentList.SetInputArgumentOfInt64(i,arg);
                        break;
                    case CarDataType.Byte:
                        oArgumentList.SetInputArgumentOfByte(i,arg);
                        break;
                    case CarDataType.Float:
                        oArgumentList.SetInputArgumentOfFloat(i,arg);
                        break;
                    case CarDataType.Double:
                        oArgumentList.SetInputArgumentOfDouble(i,arg);
                        break;
                    case CarDataType.Char:
                        oArgumentList.SetInputArgumentOfChar(i,arg);
                        break;
                    case CarDataType.String:
                        oArgumentList.SetInputArgumentOfString(i,arg);
                        break;
                    case CarDataType.Boolean:
                        oArgumentList.SetInputArgumentOfBoolean(i,arg);
                        break;
                    //case CarDataType.EMuid:
                    //case CarDataType.EGuid:
                    case CarDataType.ECode:
                        oArgumentList.SetInputArgumentOfECode(i,arg);
                        break;
                    //case CarDataType.LocalPtr:
                    //case CarDataType.LocalType:
                    case CarDataType.Enum:
                        oArgumentList.SetInputArgumentOfEnum(i,arg);
                        break;
                    //case CarDataType.CarArray:
                    //case CarDataType.StructPtr:
                    case CarDataType.Interface:
                        oArgumentList.SetInputArgumentOfObjectPtr(i,arguments[i+2]);
                        break;
                    default:
                        whmlog('==============argument type: to be finished================='+iDataType);
                        whmlog('==============argument type: to be finished========8.1===='+i+'====='+oTypeInfo.GetName()+'===='+iDataType+'===='+sCarDataType+'===='+sJsDataType);
                        break;
                }
}

if(1==0){
                if (type == 'string') {
                    oArgumentList.SetInputArgumentOfString(0,arguments[i+2]);
                }
                else if(type == 'object') {
                    oArgumentList.SetInputArgumentOfObjectPtr(0,arguments[i+2]);
                }
                else {
                    whmlog('==============argument type: to be finished=================');
                }
}
            }
            //whmlog('==============classinfo__createObject========9=========');
            newObject = oConstructorInfo.CreateObject(oArgumentList);
            //whmlog('==============classinfo__createObject========10=========');
        }

        return newObject;
    }   //classinfo__createObject


//-----------------------------config---------end-------------------------------------


ElastosTest.fnTestReflection = function(looptimes) {


var jso_activity_cb = (function(oCarRoot,oModuleInfo,oTestEventListener, oActivity){
    return {
        OnCreate:function(aoContext){
            whmlog('==============jso_activity_cb===========OnCreate.begin======');

            oActivity.SetContentView(R.layout.main);

            whmlog('==============jso_activity_cb===========OnCreate.begin===1===');

            var oDialogButton = oActivity.FindViewById(R.id.DialogButton);
            var jso_button_DialogButton_cb = (function(){
                return {
                    OnClick:function(aoView){
                        whmlog('==============DialogButton clicked=======begin==========');
                        oActivity.ShowDialog(0);
                        whmlog('==============DialogButton clicked=======end==========');
                        //var dlg = OnCreateDialog(0);
                        //dlg->DispatchOnCreate(0);
                        //dlg.Show();


                    }
                }
            })();
            oDialogButton.SetOnClickListener(jso_button_DialogButton_cb);

            whmlog('==============jso_activity_cb===========OnCreate.begin===2===');


        },
        OnStart:function(aoContext){
            whmlog('==============jso_activity_cb===========OnStart.begin======');
        },
        OnResume:function(aoContext){
            whmlog('==============jso_activity_cb===========OnResume.begin======');
        },
        OnPause:function(aoContext){
            whmlog('==============jso_activity_cb===========OnPause.begin======');
        },
        OnStop:function(aoContext){
            whmlog('==============jso_activity_cb===========OnStop.begin======');
        },
        OnDestroy:function(aoContext){
            whmlog('==============jso_activity_cb===========OnDestroy.begin======');
        },
        OnActivityResult:function(aoContext, aiRequestCode, aiResultCode, aoIntentData){
            whmlog('==============jso_activity_cb===========OnActivityResult.begin======');
        },

        OnCreateDialog:function(aoContext, aiId, aoDialog){
            whmlog('==============jso_activity_cb===========OnCreateDialog.begin======'+typeof(aoDialog));
            if(typeof(aoDialog)=='object') {
                var a=[];
                for(var p in aoDialog)a.push(p);
                whmlog('==============jso_activity_cb===========OnCreateDialog.begin======prop:['+a.join(':')+']');

                if(aoDialog.data) {
                    whmlog('==============jso_activity_cb===========OnCreateDialog.begin======data.type:['+typeof(aoDialog.data)+']');
                }

                if(typeof(aoDialog.data)=='string') {
                    whmlog('==============jso_activity_cb===========OnCreateDialog.begin======data:['+aoDialog.data+']');
                }
            }

            //oTestEventListener.Unlock();
            //whmlog('==============jso_activity_cb===========OnCreateDialog.end test======');

            //return;

//--------create dialog begin--------
        whmlog('==============CActivityOne::OnCreateDialog create control begin=================');

        //var builder = classinfo__createObject(oModuleInfo, "CAlertDialogBuilder", oActivity);
        var builder = classinfo__createObject("/system/lib/Elastos.Droid.Core.eco", "CAlertDialogBuilder", oActivity);



        whmlog('==============jso_activity_cb OnCreateDialog.begin=====1============');
        builder.SetTitleEx(CString("NodeJS==普通对话框"));

        whmlog('==============jso_activity_cb OnCreateDialog.begin=====2============');
        builder.SetMessageEx(CString("这是一个普通=NodeJS=对话框"));

        whmlog('==============jso_activity_cb OnCreateDialog.begin=====3============');
        var clickListener = (function(){
            return {
                OnClick:function(aoDialogInterface, aiWhich){
                    whmlog('==============dialog clicked=================');

                    var BUTTON_POSITIVE = -1;
                    var BUTTON_NEGATIVE = -2;
                    var BUTTON_NEUTRAL = -3;

                    switch (aiWhich) {
                        case BUTTON_POSITIVE:
                            whmlog('==============点击了确定按钮=================');
                            break;
                        case BUTTON_NEGATIVE:
                            whmlog('==============点击了取消按钮=================');
                            break;
                        case BUTTON_NEUTRAL:
                            whmlog('==============点击了中立按钮=================');
                            break;
                        default:
                            break;
                    }
                }
            }
        })();

        whmlog('==============jso_activity_cb OnCreateDialog.begin=====4===========');
        builder.SetPositiveButtonEx(CString("确NODE定"), clickListener);

        whmlog('==============jso_activity_cb OnCreateDialog.begin=====5============');
        builder.SetPositiveButtonEx(CString("中JS立"), clickListener);

        whmlog('==============jso_activity_cb OnCreateDialog.begin=====6============');
        builder.SetPositiveButtonEx(CString("取OK消"), clickListener);

        whmlog('==============jso_activity_cb OnCreateDialog.begin=====7============');
        var dlg = builder.Create();
        whmlog('==============jso_activity_cb OnCreateDialog.begin=====8============');

        aoDialog.data = dlg;
        whmlog('==============jso_activity_cb OnCreateDialog.begin=====9============');

        //oTestEventListener.Unlock();
        //whmlog('==============CActivityOne::OnCreateDialog begin=====10============');

//--------create dialog end--------

        }   //OnCreateDialog

    }   //return
})(oCarRoot,oModuleInfo,oTestEventListener, oActivity);

//if (0==1) {

oTestEventListener.SetActivityListener(jso_activity_cb);

//}   //if(0==1)

whmlog('=====================test_basic.js unlock before===================================');
oTestEventListener.Unlock();
whmlog('=====================test_basic.js unlock after===================================');

//oActivity.OnCreate(NULL);
//jso_activity_cb.OnCreate(oActivity);

    //----------------loop.begin----------------
    var assert = require('assert');
    var called = false;

    process.on('exit', function () {
        assert(called);
    });

    elastos.receive(5, function (err, val) {
        whmlog('==============ssss.receive.11111111=================');
        assert.equal(null, err);
        //assert.equal(10, val);
        process.nextTick(function () {
            whmlog('==============ssss.process.nextTick.22222222=================');
            called = true;
        });
    });
    //----------------loop.end----------------


whmlog('=====================fnTestReflection end===================================');

return;




//----------------config.begin----------------

try {

//----------------------------try begin---------------------------------
if(0==1) {
    var oCStringWrapper = classinfo__createObject("/system/lib/Elastos.Core.eco","CStringWrapper","Hello NewTextView0岛礁!");

    var oModuleInfo = oCarRoot.Test_Require_ModuleInfo("/system/lib/Elastos.Droid.Core.eco");

    var oNewTextView = classinfo__createObject(oModuleInfo,"CTextView",oActivity);
    oNewTextView.SetText(oCStringWrapper);
    oNewTextView.SetTextSize(48);

    var oNewButton = classinfo__createObject(oModuleInfo,"CButton",oActivity);
    oNewButton.SetText(oCStringWrapper);


    var oLinearLayout = classinfo__createObject(oModuleInfo,"CLinearLayout",oActivity);
    oLinearLayout.SetOrientation(0);

    oLinearLayout.AddView(oNewTextView);
    oLinearLayout.AddView(oNewButton);

    oActivity.SetContentViewEx(oLinearLayout);

}

if(0==1) {

    var sText;

    //var myTextViewId = 0x7f050007;
    var oTextView = oActivity.FindViewById(R.id.myTextView);
    var sText = oTextView.GetText().ToString();
    whmlog('==============ssss.4.1========TextView:'+sText+'=========');
    //oTestCar.SetText('xyz');

    //var myEditTextId = 0x7f050008;
    var oEditText = oActivity.FindViewById(R.id.myEditText);
    sText = oEditText.GetText().ToString();
    whmlog('==============ssss.4.2========EditText:'+sText+'=========');



    var oWindowManager = oActivity.GetWindowManager();
    var oWindow = oActivity.GetWindow();
    var oIntent = oActivity.GetIntent();
    var oComponent = oIntent.GetComponent();
    whmlog('==============ssss.4.3========oComponent:'+oComponent.GetPackageName()+'=='+oComponent.GetClassName()+'=======');
    var oAction = oIntent.GetAction();
    whmlog('==============ssss.4.4========oAction:=='+oAction+'=======');


    whmlog('==============ssss.22=================');

    //var chkAndroidId = 0x7f05000a;
    var oChkAndroid = oActivity.FindViewById(R.id.chkAndroid);
    //var oCharSequence = oChkAndroid.GetText();
    //sText = oCharSequence.ToString();
    //whmlog('==============ssss.4.2========ChkAndroid:'+sText+'=========');
    //oChkAndroid.SetChecked(false);
    var jso_button_android_cb = (function(){
        return {
            OnClick:function(aoView){
                oChkAndroid.SetChecked(true);
                oChkIos.SetChecked(false);
            }
        }
    })();
    oChkAndroid.SetOnClickListener(jso_button_android_cb);

    //var chkIosId = 0x7f050009;
    var oChkIos = oActivity.FindViewById(R.id.chkIos);
    //sText = oChkIos.GetText().ToString();
    //whmlog('==============ssss.4.2========ChkIos:'+sText+'=========');
    //error:Only the original thread that created a view hierarchy can touch its views.
    //var oCStringWrapper = classinfo__createObject("/system/lib/Elastos.Core.eco","CStringWrapper","Hello NewTextView0岛礁0!");
    //oChkIos.SetText(oCStringWrapper);
    //oChkIos.SetChecked(true);
    //oChkIos.SetWidth(200);
    var jso_button_ios_cb = (function(){
        return {
            OnClick:function(aoView){
                oChkAndroid.SetChecked(false);
                oChkIos.SetChecked(true);
            }
        }
    })();
    oChkIos.SetOnClickListener(jso_button_ios_cb);

    whmlog('==============ssss.22.1=================');

    var oModuleInfo = oCarRoot.Test_Require_ModuleInfo("system/lib/Elastos.Droid.Core.eco");

    var IAnimation__RELATIVE_TO_SELF = 1;

    var oAlphaAnimation = classinfo__createObject(
        oModuleInfo,"CAlphaAnimation",
        0.3, 1.0
    );
    oAlphaAnimation.SetDuration(3000);

    var oCRotateAnimation = classinfo__createObject(
        oModuleInfo,"CRotateAnimation",
        0.0, 350.0,
        IAnimation__RELATIVE_TO_SELF,
        0.5,
        IAnimation__RELATIVE_TO_SELF,
        0.5
    );
    oCRotateAnimation.SetDuration(3000);

    var oCScaleAnimation = classinfo__createObject(
        oModuleInfo,"CScaleAnimation",
        0.2, 1.4, 0.2, 1.4,
        IAnimation__RELATIVE_TO_SELF,
        0.5,
        IAnimation__RELATIVE_TO_SELF,
        0.5
    );
    oCScaleAnimation.SetDuration(3000);

    var oCTranslateAnimation = classinfo__createObject(
        oModuleInfo,"CTranslateAnimation",
        300.0, -20.0, -10.0, 30.0
    );
    oCTranslateAnimation.SetDuration(3000);

    var oAnamtionButton = oActivity.FindViewById(R.id.AnamtionButton);
    var jso_cb_AnamtionButton = (function(){
        var oDialogButton = oActivity.FindViewById(R.id.DialogButton);
        var count = 0;
        return {
            OnClick:function(aoView){
                count = count % 4;
                if (count == 0) {
                    oDialogButton.StartAnimation(oAlphaAnimation);
                }
                else if (count == 1) {
                    oDialogButton.StartAnimation(oCRotateAnimation);
                }
                else if (count == 2) {
                    oDialogButton.StartAnimation(oCScaleAnimation);
                }
                else {
                    oDialogButton.StartAnimation(oCTranslateAnimation);
                }
                count++;
            }
        }
    })();
    oAnamtionButton.SetOnClickListener(jso_cb_AnamtionButton);

    //---------------------------PopupWindowButton begin----------------------------------------

    function OnCreatePopupWindow(){
        //Slogger::D("CActivityOne", "CActivityOne::OnCreatePopupWindow()");
        whmlog('==============OnCreatePopupWindow begin=================');

        //mPopupWindow = NULL;
        var oPopupWindow;

        //AutoPtr<ILayoutInflater> inflater;
        //GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);
        var IContext__LAYOUT_INFLATER_SERVICE = "layout_inflater";
        var oInflater = oActivity.GetSystemService(IContext__LAYOUT_INFLATER_SERVICE);

        whmlog('==============OnCreatePopupWindow begin======1===========');

        //AutoPtr<IView> layout;
        //inflater->Inflate(R::layout::popupwindow, NULL, (IView**)&layout);
        var oLayout = oInflater.Inflate(R.layout.popupwindow, null);

        whmlog('==============OnCreatePopupWindow begin======2===========');

        //CPopupWindow::New(layout, 350, 350, TRUE, (IPopupWindow**)&mPopupWindow);
        //mPopupWindow->ShowAtLocation(layout, IGravity::CENTER, 0, 0);
        var IGravity__AXIS_X_SHIFT = 0;
        var IGravity__AXIS_Y_SHIFT = 4;
        var IGravity__AXIS_SPECIFIED = 0x0001;
        whmlog('==============OnCreatePopupWindow begin======2.1===========');
        //var IGravity__CENTER_VERTICAL = IGravity__AXIS_SPECIFIED<<IGravity__AXIS_Y_SHIFT;
        var IGravity__CENTER_VERTICAL = IGravity__AXIS_SPECIFIED<<4;
        whmlog('==============OnCreatePopupWindow begin======2.2===========');
        //var IGravity__CENTER_HORIZONTAL = IGravity__AXIS_SPECIFIED<<IGravity__AXIS_X_SHIFT;
        var IGravity__CENTER_HORIZONTAL = IGravity__AXIS_SPECIFIED<<0;
        whmlog('==============OnCreatePopupWindow begin======2.3===========');
        var IGravity__CENTER = IGravity__CENTER_VERTICAL | IGravity__CENTER_HORIZONTAL;
        whmlog('==============OnCreatePopupWindow begin======3===========');
        oPopupWindow = classinfo__createObject(oModuleInfo,"CPopupWindow", oLayout, 350, 350, true);
        whmlog('==============OnCreatePopupWindow begin======4===========');


        //whmlog('==============OnCreatePopupWindow sleep======1.0===========');
        //oTestEventListener.Sleep(1);
        //while(!oPopupWindow) {
        //    whmlog('==============OnCreatePopupWindow sleep======1.1===========');
        //    oTestEventListener.Sleep(1);
        //    whmlog('==============OnCreatePopupWindow sleep======1.2===========');
        //}


        oPopupWindow.ShowAtLocation(oLayout, IGravity__CENTER, 0, 0);
        whmlog('==============OnCreatePopupWindow begin======5===========');

        //// Setup TextView
        ////
        //AutoPtr<IView> temp;
        //layout->FindViewById(R::id::txtView, (IView**)&temp);
        //AutoPtr<ITextView> textView = ITextView::Probe(temp.Get());
        //assert(textView != NULL);
        var oTextView = oLayout.FindViewById(R.id.txtView);
        whmlog('==============OnCreatePopupWindow begin======5.1===========');

        //AutoPtr<ICharSequence> cs;
        //CStringWrapper::New(String("PopupWindow 测试程序!"), (ICharSequence**)&cs);
        //textView->SetText(cs);
        var oCs = classinfo__createObject("/system/lib/Elastos.Core.eco","CStringWrapper","PopupWindow 测试程序!");
        whmlog('==============OnCreatePopupWindow begin======6===========');
        oTextView.SetText(oCs);
        whmlog('==============OnCreatePopupWindow begin======7===========');

        //// Setup Button
        ////
        //AutoPtr<IView> dismissButton;
        //layout->FindViewById(R::id::btn_close_popup, (IView**)&dismissButton);
        //assert(dismissButton != NULL);
        var oDismissButton = oLayout.FindViewById(R.id.btn_close_popup);
        whmlog('==============OnCreatePopupWindow begin======8===========');

        //AutoPtr<MyListener> l = new MyListener(this);
        //IViewOnClickListener* clickListener = (IViewOnClickListener*)l.Get();
        //dismissButton->SetOnClickListener(clickListener);
        var jso_button_DismissButton_cb = (function(ao_PopupWindow){
            return {
                //ECode CActivityOne::MyListener::OnClick(
                //    /* [in] */ IView* view)
                //{
                    //if (...) {...}
                    //else if (id == R::id::btn_close_popup) {
                    //    Slogger::D("CActivityOne", "Dismiss PopupWindow!");
                    //    mHost->mPopupWindow->Dismiss();
                    //    mHost->mPopupWindow = NULL;
                    //    return NOERROR;
                    //}
                //}
                OnClick:function(aoView){
                    whmlog('==============Dismiss PopupWindow!=======1==========');
                    ao_PopupWindow.Dismiss();
                    whmlog('==============Dismiss PopupWindow!=======2==========');
                    ao_PopupWindow = null;
                    return;
                }
            }
        })(oPopupWindow);
        oDismissButton.SetOnClickListener(jso_button_DismissButton_cb);
        whmlog('==============OnCreatePopupWindow begin======9===========');

        //return NOERROR;
        return;
    }

    //else if (id == R::id::PopupWindowButton) {
    //    Slogger::D("CActivityOne", "Show PopupWindow!");
    //    mHost->OnCreatePopupWindow();
    //}
    var oPopupWindowButton = oActivity.FindViewById(R.id.PopupWindowButton);
    var jso_button_PopupWindowButton_cb = (function(){
        return {
            OnClick:function(aoView){
                whmlog('==============PopupWindowButton clicked================='+typeof(oActivity.OnCreatePopupWindow));
                OnCreatePopupWindow();
            }
        }
    })();
    oPopupWindowButton.SetOnClickListener(jso_button_PopupWindowButton_cb);

    //---------------------------PopupWindowButton end----------------------------------------

    //---------------------------DialogButton begin----------------------------------------


    function OnCreateDialog(id) {
        whmlog('==============CActivityOne::OnCreateDialog begin=================');

        var builder = classinfo__createObject(oModuleInfo, "CAlertDialogBuilder", oActivity);

        whmlog('==============CActivityOne::OnCreateDialog begin=====1============');
        builder.SetTitleEx(CString("普通对话框"));

        whmlog('==============CActivityOne::OnCreateDialog begin=====2============');
        builder.SetMessageEx(CString("这是一个普通对话框"));

        whmlog('==============CActivityOne::OnCreateDialog begin=====3============');
        var clickListener = (function(){
            return {
                OnClick:function(aoDialogInterface, aiWhich){
                    whmlog('==============dialog clicked=================');

                    var BUTTON_POSITIVE = -1;
                    var BUTTON_NEGATIVE = -2;
                    var BUTTON_NEUTRAL = -3;

                    switch (aiWhich) {
                        case BUTTON_POSITIVE:
                            whmlog('==============点击了确定按钮=================');
                            break;
                        case BUTTON_NEGATIVE:
                            whmlog('==============点击了取消按钮=================');
                            break;
                        case BUTTON_NEUTRAL:
                            whmlog('==============点击了中立按钮=================');
                            break;
                        default:
                            break;
                    }
                }
            }
        })();

        whmlog('==============CActivityOne::OnCreateDialog begin=====4===========');
        builder.SetPositiveButtonEx(CString("确定"), clickListener);

        whmlog('==============CActivityOne::OnCreateDialog begin=====5============');
        builder.SetPositiveButtonEx(CString("中立"), clickListener);

        whmlog('==============CActivityOne::OnCreateDialog begin=====6============');
        builder.SetPositiveButtonEx(CString("取消"), clickListener);

        whmlog('==============CActivityOne::OnCreateDialog begin=====7============');
        var dlg = builder.Create();
        whmlog('==============CActivityOne::OnCreateDialog begin=====8============');

        return dlg;
    }

    var oDialogButton = oActivity.FindViewById(R.id.DialogButton);
    var jso_button_DialogButton_cb = (function(){
        return {
            OnClick:function(aoView){
                whmlog('==============DialogButton clicked=================');
                oActivity.ShowDialog(0);

                //var dlg = OnCreateDialog(0);
                //whmlog('==============DialogButton clicked=========1========');
                //dlg.DispatchOnCreate(0);
                //whmlog('==============DialogButton clicked=========2========');
                //dlg.Show();
                //whmlog('==============DialogButton clicked=========3========');
            }
        }
    })();
    oDialogButton.SetOnClickListener(jso_button_DialogButton_cb);

    //---------------------------DialogButton end----------------------------------------


    whmlog('==============ssss.23================='+typeof(oChkAndroid.SetOnClickListener));

    //----------------loop.begin----------------
    var assert = require('assert');
    var called = false;

    process.on('exit', function () {
        assert(called);
    });

    elastos.receive(5, function (err, val) {
        whmlog('==============ssss.receive.11111111=================');
        assert.equal(null, err);
        //assert.equal(10, val);
        process.nextTick(function () {
            whmlog('==============ssss.process.nextTick.22222222=================');
            called = true;
        });
    });
    //----------------loop.end----------------

    //sleep(1);

    whmlog('==============ssss.24=================');

    var events = require('events');
    whmlog('==============ssss.24.1=================');
    var emitter = new events.EventEmitter();
    whmlog('==============ssss.24.2=================');

    emitter.on('test',function(e,s){
        whmlog('========event test run=========================');
    });
    whmlog('==============ssss.24.3=================');

    emitter.emit('test','a','b');

    whmlog('==============ssss.25=================');

}

if(0==1) {
    var assert = require('assert');
    whmlog('==============ssss.25.1=================');
    var buffer = require('buffer');
    whmlog('==============ssss.25.2=================');
    var child_process = require('child_process');
    whmlog('==============ssss.25.3=================');
    var cluster = require('cluster');
    whmlog('==============ssss.25.4=================');
    //var console = require('console');
    //whmlog('==============ssss.25.5=================');
    var constants = require('constants');
    whmlog('==============ssss.25.6=================');
    //var crypto = require('crypto');
    //whmlog('==============ssss.25.7=================');
    var dgram = require('dgram');
    whmlog('==============ssss.25.8=================');
    //var dns = require('dns');
    //whmlog('==============ssss.25.9=================');
    //var domain = require('domain');
    //whmlog('==============ssss.25.10=================');
    var events = require('events');
    whmlog('==============ssss.25.11=================');
    var freelist = require('freelist');
    whmlog('==============ssss.25.12=================');
    var fs = require('fs');
    whmlog('==============ssss.25.13=================');
    //var http_common = require('_http_common');
    //whmlog('==============ssss.25.14=================');
    //var http_outgoing = require('_http_outgoing');
    //whmlog('==============ssss.25.15=================');
    var http_incoming = require('_http_incoming');
    whmlog('==============ssss.25.16=================');
    var http_agent = require('_http_agent');
    whmlog('==============ssss.25.17=================');
    //var http_client = require('_http_client');
    //whmlog('==============ssss.25.18=================');
    //var http_parser = require('_http_server');
    //whmlog('==============ssss.25.19=================');
    //var htpp = require('http');
    //whmlog('==============ssss.25.20=================');
}
if(0==1) {
    var linklist = require('_linklist');
    whmlog('==============ssss.25.21=================');
    var module = require('module');
    whmlog('==============ssss.25.22=================');
    //var native_module = require('native_module');
    //whmlog('==============ssss.25.23=================');
    var net = require('net');
    whmlog('==============ssss.25.24=================');
    var path = require('path');
    whmlog('==============ssss.25.25=================');
    var punycode = require('punycode');
    whmlog('==============ssss.25.26=================');
    var querystring = require('querystring');
    whmlog('==============ssss.25.27=================');
    var readline = require('readline');
    whmlog('==============ssss.25.28=================');
    var repl = require('repl');
    whmlog('==============ssss.25.29=================');
    var smalloc = require('smalloc');
    whmlog('==============ssss.25.30=================');
    var stream_readable = require('_stream_readable');
    whmlog('==============ssss.25.31=================');
    var stream_writable = require('_stream_writable');
    whmlog('==============ssss.25.32=================');
    var stream_duplex = require('_stream_duplex');
    whmlog('==============ssss.25.33=================');
    var stream_transform = require('_stream_transform');
    whmlog('==============ssss.25.34=================');
    var stream_passthrough = require('_stream_passthrough');
    whmlog('==============ssss.25.35=================');
    var stream = require('stream');
    whmlog('==============ssss.25.36=================');
    //var string_decoder = require('string_decoder');
    //whmlog('==============ssss.25.37=================');
    var timers = require('timers');
    whmlog('==============ssss.25.38=================');
}
if(0==1) {
    //var tls = require('tls');
    //whmlog('==============ssss.25.39=================');
    //var tls_common = require('_tls_common');
    //whmlog('==============ssss.25.40=================');
    //var tls_wrap = require('_tls_wrap');
    //whmlog('==============ssss.25.41=================');
    //var tls_legacy = require('_tls_legacy');
    //whmlog('==============ssss.25.42=================');
    //var tty_wrap = require('tty_wrap');
    //whmlog('==============ssss.25.43=================');
    var url = require('url');
    whmlog('==============ssss.25.44=================');
    var util = require('util');
    whmlog('==============ssss.25.45=================');
    var vm = require('vm');
}


    whmlog('==============ssss.27=================');

//----------------------------try end---------------------------------

    a.push("all the tests are OK!");
} catch (e) {
    a.push("test error at p = " + p);
}

if (bNode) {
    s = a.join("\n");
    whmlog('====================whmlog: test end================'+s);
}
else {
    s = a.join("<br>");
    var oMain = document.getElementById("main");
    oMain && (oMain.innerHTML = s);
}


var jso_activity_cb = (function(oTestEventListener){
    return {
        OnCreate:function(aoContext){
            whmlog('==============jso_activity_cb===========OnCreate.begin======');
        },
        OnStart:function(aoContext){
            whmlog('==============jso_activity_cb===========OnStart.begin======');
        },
        OnResume:function(aoContext){
            whmlog('==============jso_activity_cb===========OnResume.begin======');
        },
        OnPause:function(aoContext){
            whmlog('==============jso_activity_cb===========OnPause.begin======');
        },
        OnStop:function(aoContext){
            whmlog('==============jso_activity_cb===========OnStop.begin======');
        },
        OnDestroy:function(aoContext){
            whmlog('==============jso_activity_cb===========OnDestroy.begin======');
        },
        OnActivityResult:function(aoContext, aiRequestCode, aiResultCode, aoIntentData){
            whmlog('==============jso_activity_cb===========OnActivityResult.begin======');
        },

        OnCreateDialog:function(aoContext, aiId, aoDialog){
            whmlog('==============jso_activity_cb===========OnCreateDialog.begin======'+typeof(aoDialog));
            if(typeof(aoDialog)=='object') {
                var a=[];
                for(var p in aoDialog)a.push(p);
                whmlog('==============jso_activity_cb===========OnCreateDialog.begin======prop:['+a.join(':')+']');

                if(aoDialog.data) {
                    whmlog('==============jso_activity_cb===========OnCreateDialog.begin======data.type:['+typeof(aoDialog.data)+']');
                }

                if(typeof(aoDialog.data)=='string') {
                    whmlog('==============jso_activity_cb===========OnCreateDialog.begin======data:['+aoDialog.data+']');
                }
            }


//--------create dialog begin--------
        whmlog('==============CActivityOne::OnCreateDialog begin=================');

        var builder = classinfo__createObject(oModuleInfo, "CAlertDialogBuilder", oActivity);

        whmlog('==============CActivityOne::OnCreateDialog begin=====1============');
        builder.SetTitleEx(CString("普通对话框"));

        whmlog('==============CActivityOne::OnCreateDialog begin=====2============');
        builder.SetMessageEx(CString("这是一个普通对话框"));

        whmlog('==============CActivityOne::OnCreateDialog begin=====3============');
        var clickListener = (function(){
            return {
                OnClick:function(aoDialogInterface, aiWhich){
                    whmlog('==============dialog clicked=================');

                    var BUTTON_POSITIVE = -1;
                    var BUTTON_NEGATIVE = -2;
                    var BUTTON_NEUTRAL = -3;

                    switch (aiWhich) {
                        case BUTTON_POSITIVE:
                            whmlog('==============点击了确定按钮=================');
                            break;
                        case BUTTON_NEGATIVE:
                            whmlog('==============点击了取消按钮=================');
                            break;
                        case BUTTON_NEUTRAL:
                            whmlog('==============点击了中立按钮=================');
                            break;
                        default:
                            break;
                    }
                }
            }
        })();

        whmlog('==============CActivityOne::OnCreateDialog begin=====4===========');
        builder.SetPositiveButtonEx(CString("确定"), clickListener);

        whmlog('==============CActivityOne::OnCreateDialog begin=====5============');
        builder.SetPositiveButtonEx(CString("中立"), clickListener);

        whmlog('==============CActivityOne::OnCreateDialog begin=====6============');
        builder.SetPositiveButtonEx(CString("取消"), clickListener);

        whmlog('==============CActivityOne::OnCreateDialog begin=====7============');
        var dlg = builder.Create();
        whmlog('==============CActivityOne::OnCreateDialog begin=====8============');

        aoDialog.data = dlg;
        whmlog('==============CActivityOne::OnCreateDialog begin=====9============');

        oTestEventListener.Unlock();
        whmlog('==============CActivityOne::OnCreateDialog begin=====10============');


//--------create dialog end--------

        }

    }
})(oTestEventListener);
oTestEventListener.SetActivityListener(jso_activity_cb);




}    //fnTestReflection

ElastosTest.fnTestReflection(1);

whmlog('=====================test_basic.js end===================================');

