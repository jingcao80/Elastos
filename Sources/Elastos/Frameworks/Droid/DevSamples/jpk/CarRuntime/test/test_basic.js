whmlog('=====================test_basic.js begin===================================');

//----------------common config begin----------------

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

    for(var p in CarDataType) {
        if (CarDataType[p] == ai_datatype) {
            sRet = p;
            break;
        }
    }

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

function classinfo__createObject(oModuleInfo,oClassInfo){
    var newObject;

    if(typeof(oModuleInfo)=='string') {
        oModuleInfo = Elastos.Runtime.getModuleInfo(oModuleInfo);
    }
    if(typeof(oClassInfo)=='string') {
        oClassInfo = oModuleInfo.GetClassInfo(oClassInfo);
    }

    var length = arguments.length;
    if(length==2){
        newObject = oClassInfo.CreateObject();
    }
    else {
        var aConstructorInfos = oClassInfo.GetAllConstructorInfos();
        var oConstructorInfo;
        var paramCount = length - 2;
        for(var i=0, im=aConstructorInfos.length; i<im; i++){
            oConstructorInfo = aConstructorInfos[i];
            var _paramCount = oConstructorInfo.GetParamCount();
            if (_paramCount == paramCount) {
                var bSameArgs = true;
                var aParamInfos = oConstructorInfo.GetAllParamInfos();
                for(var j = 0, jm = paramCount; j<jm; j++) {
                    var paramInfo = aParamInfos[j];
                    var oTypeInfo = paramInfo.GetTypeInfo();

                    var iDataType = oTypeInfo.GetDataType();
                    var sJsDataType = getDataTypeJavascriptString(iDataType);

                    var arg_in = arguments[j+2];
                    var type_in = typeof(arg_in);
                    if (sJsDataType == type_in) continue;

                    bSameArgs = false
                    break;
                }
                if (bSameArgs) break;
            }
        }

        var bFoundConstructor = (i < im);
        if (!bFoundConstructor) {
            whmlog('====classinfo__createObject====can not find constructor ==============');
            return null;
        }

        var aParamInfos = oConstructorInfo.GetAllParamInfos();
        var oArgumentList = oConstructorInfo.CreateArgumentList();

        for(var i = 0, im = paramCount; i<im; i++) {
            var paramInfo = aParamInfos[i];
            var oTypeInfo = paramInfo.GetTypeInfo();

            var iDataType = oTypeInfo.GetDataType();
            var sJsDataType = getDataTypeJavascriptString(iDataType);

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
                    var sCarDataType = getDataTypeCarString(iDataType);
                    whmlog('==============argument type: to be finished================='+iDataType);
                    break;
            }
        }
        newObject = oConstructorInfo.CreateObject(oArgumentList);
    }
    return newObject;
}   //classinfo__createObject

//----------------common config end----------------

//----------------elastos begin----------------

var root = global||window;
//root.Elastos || (root.Elastos = {});

root.Elastos = ( function () {
    whmlog('==============env begin=================');

    var _api = require('./elastos.node');

    var _Bridge_Native = _api.require("/data/elastos/TextViewDemo.eco", "CTestEventListener");
    var _Runtime_Native = _api.require("/data/elastos/JSCarRoot.eco", "CCarRoot");

    var _getModuleInfo = function (asEcoName) {
        return _Runtime_Native.Test_Require_ModuleInfo(asEcoName);
    };

    var _Module_Core = _getModuleInfo("/system/lib/Elastos.Core.eco");
    var _Module_Droid = _getModuleInfo("/system/lib/Elastos.Droid.Core.eco");

    var _Bridge = {
        init : function (aoActivityListener) {
            var pEnqueueUIMessage = _Bridge_Native.GetEnqueueUIMessagePtr();
            _api.SetEnqueueUIMessagePtr(pEnqueueUIMessage);

            var iNodeBridge = _api.GetNodeBridge();
            _Bridge_Native.SetNodeBridge(iNodeBridge,1);

            _Bridge_Native.SetActivityListener(aoActivityListener);

            _Bridge_Native.Unlock();
        },
    };

    var _bindArgs = function (aaArgs, aoArgs) {
        var args = [];
        for (var i = 0, im = aaArgs.length; i < im; i++) args.push(aaArgs[i]);
        for (var i = 0, im = aoArgs.length; i < im; i++) args.push(aoArgs[i]);
        return args;
    };


    return {
        Runtime : {
            getModuleInfo : _getModuleInfo,
            createObject : classinfo__createObject,
        },

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
            R : null,

            Activity : {
                main : _Bridge_Native.GetMainActivity(),
            },

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

            Ready : function (aoActivityListener) {
                _Bridge.init(aoActivityListener);

                this.Run();
            },
        },

        //Activity : {
        //    main : _Bridge_Native.GetMainActivity(),
        //},
    }
} )();

root.Application = Elastos.Application;

//----------------elastos end----------------

Elastos.Application.R = {
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

//----------------EPK JS CODE BEGIN----------

var jso_activity_cb = (function(aoElastos){
    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;

    var R = aoElastos.Application.R;

    var oActivity = aoElastos.Application.Activity.main

    return {
        OnCreate:function(aoContext){
            //----------------SetContentView begin----------------
            oActivity.SetContentView(R.layout.main);
            //----------------SetContentView end----------------

            //----------------myTextView begin----------------
            var oMyTextView = oActivity.FindViewById(R.id.myTextView);
            var jso_button_MyTextView_cb = (function(){
                return {
                    OnClick:function(aoView){
                        whmlog('====jso_activity_cb====MyTextView.OnClick.Start====');
                        //TODO
                    }
                }
            })();
            oMyTextView.SetOnClickListener(jso_button_MyTextView_cb);
            //----------------myTextView begin----------------

            //----------------myEditText begin----------------
            var oMyEditText = oActivity.FindViewById(R.id.myEditText);
            var jso_button_MyEditText_OnKeyListener = (function(){
                return {
                    OnKey:function(aoView, aiKeyCode, aoEvent, out_abResult){
                        //TODO: the listener also used in DialogButton

                        var iId = aoView.GetId();
                        var iAction = aoEvent.GetAction();
                        var sLabel = aoEvent.GetDisplayLabel();

                        whmlog("====CActivityOne::MyListener::OnKey: view '" + iId + "', keyCode: '" + aiKeyCode + "','" + sLabel + "'");

                        var KeyEvent__ACTION_DOWN = 0;
                        var KeyEvent__ACTION_UP   = 1;
                        var KeyEvent__KEYCODE_0   = 7;
                        if (iAction == IKeyEvent__ACTION_DOWN) {
                            if (aiKeyCode == IKeyEvent__KEYCODE_0) {
                                whmlog("====CActivityOne====Key 0 is down.");
                            }
                        }
                        else if (iAction == IKeyEvent__ACTION_UP) {
                                whmlog("====CActivityOne====IKeyEvent__ACTION_UP received.");
                        }

                        out_abResult.data = false;

                        return;
                    }
                }
            })();
            oMyTextView.SetOnKeyListener(jso_button_MyEditText_OnKeyListener);
            //----------------myEditText begin----------------

            //----------------chkAndroid/chkIos begin----------------
            var oChkAndroid = oActivity.FindViewById(R.id.chkAndroid);
            var oChkIos = oActivity.FindViewById(R.id.chkIos);
            var jso_button_group_cb = {
                OnClick:function(aoView){
                    var id = aoView.GetId();
                    var check = ( id == R.id.chkAndroid );
                    oChkAndroid.SetChecked(check);
                    oChkIos.SetChecked(!check);
                }
            };
            oChkAndroid.SetOnClickListener(jso_button_group_cb);
            oChkIos.SetOnClickListener(jso_button_group_cb);
            //----------------chkAndroid/chkIos end----------------

            //----------------AnamtionButton begin----------------
            oActivity.FindViewById(R.id.AnamtionButton).SetOnClickListener( (function(){
                var iSelf = 1;  //RELATIVE_TO_SELF;
                var oAnimation = {
                    Alpha :     Droid_New("CAlphaAnimation", 0.3, 1.0),
                    Rotate :    Droid_New("CRotateAnimation", 0.0, 350.0, iSelf, 0.5, iSelf, 0.5),
                    Scale :     Droid_New("CScaleAnimation", 0.2, 1.4, 0.2, 1.4, iSelf, 0.5, iSelf, 0.5),
                    Translate : Droid_New("CTranslateAnimation", 300.0, -20.0, -10.0, 30.0)
                };
                var keys = [];  //Object.keys()
                for (var prop in oAnimation) {
                    keys.push(prop);
                    oAnimation[prop].SetDuration(3000);
                }
                var oButton = oActivity.FindViewById(R.id.DialogButton);
                var count = 0;

                return {
                    OnClick:function(aoView){
                        count = count % 4;
                        oButton.StartAnimation(oAnimation[keys[count]]);
                        count++;
                    }
                }
            })() );
            //----------------AnamtionButton end----------------

            //----------------PopupWindowButton begin----------------
            function OnCreatePopupWindow(){
                var IGravity__AXIS_X_SHIFT = 0;
                var IGravity__AXIS_Y_SHIFT = 4;
                var IGravity__AXIS_SPECIFIED = 0x0001;
                var IGravity__CENTER_VERTICAL = IGravity__AXIS_SPECIFIED<<4;
                var IGravity__CENTER_HORIZONTAL = IGravity__AXIS_SPECIFIED<<0;
                var IGravity__CENTER = IGravity__CENTER_VERTICAL | IGravity__CENTER_HORIZONTAL;

                var IContext__LAYOUT_INFLATER_SERVICE = "layout_inflater";
                var oInflater = oActivity.GetSystemService(IContext__LAYOUT_INFLATER_SERVICE);
                var oLayout = oInflater.Inflate(R.layout.popupwindow, null);

                var oPopupWindow = Droid_New("CPopupWindow", oLayout, 350, 350, true);

                oPopupWindow.ShowAtLocation(oLayout, IGravity__CENTER, 0, 0);

                var oTextView = oLayout.FindViewById(R.id.txtView);
                oTextView.SetText(CString("PopupWindow 测试程序!"));

                var jso_button_DismissButton_cb = (function(ao_PopupWindow){
                    return {
                        OnClick:function(aoView){
                            ao_PopupWindow.Dismiss();
                            ao_PopupWindow = null;
                            return;
                        }
                    }
                })(oPopupWindow);
                var oDismissButton = oLayout.FindViewById(R.id.btn_close_popup);
                oDismissButton.SetOnClickListener(jso_button_DismissButton_cb);

                return;
            }

            var oPopupWindowButton = oActivity.FindViewById(R.id.PopupWindowButton);
            var jso_button_PopupWindowButton_cb = (function(){
                return {
                    OnClick:function(aoView){
                        OnCreatePopupWindow();
                    }
                }
            })();
            oPopupWindowButton.SetOnClickListener(jso_button_PopupWindowButton_cb);
            //----------------PopupWindowButton end----------------

            //----------------DialogButton begin----------------
            var oDialogButton = oActivity.FindViewById(R.id.DialogButton);
            var jso_button_DialogButton_cb = (function(){
                return {
                    OnClick:function(aoView){
                        oActivity.ShowDialog(0);
                    }
                }
            })();
            oDialogButton.SetOnClickListener(jso_button_DialogButton_cb);
            //----------------DialogButton end----------------

            //----------------ConnectivityManagerButton begin----------------
            function OnTestConnectivityManager(){
                whmlog('====jso_activity_cb====OnTestConnectivityManager.begin====');
                //TODO
            }
            var oConnectivityManagerButton = oActivity.FindViewById(R.id.ConnectivityManagerButton);
            var jso_button_ConnectivityManagerButton_cb = (function(){
                return {
                    OnClick:function(aoView){
                        OnTestConnectivityManager();
                    }
                }
            })();
            oConnectivityManagerButton.SetOnClickListener(jso_button_ConnectivityManagerButton_cb);
            //----------------ConnectivityManagerButton end----------------

            //----------------PowerManagerButton begin----------------
            function OnTestPowerManager(){
                whmlog('====jso_activity_cb====OnTestPowerManager.begin====');
                //TODO
            }
            var oPowerManagerButton = oActivity.FindViewById(R.id.PowerManagerButton);
            var jso_button_PowerManagerButton_cb = (function(){
                return {
                    OnClick:function(aoView){
                        OnTestPowerManager();
                    }
                }
            })();
            oPowerManagerButton.SetOnClickListener(jso_button_PowerManagerButton_cb);
            //----------------PowerManagerButton end----------------

            //----------------myListView begin----------------

            var oListView = oActivity.FindViewById(R.id.myListView);
            var oDataList = Droid_New("CParcelableObjectContainer");

            for (var i = 0; i < 5; ++i) {
                oDataList.Add( CString('Item--' + i) );
            }

            var oAdapter = Droid_New("CArrayAdapter", oActivity, R.layout.list_item, oDataList);
            oListView.SetAdapter(oAdapter);

            var oDrawable = Droid_New("CColorDrawable", 0xFF0000FF);
            oListView.SetDivider(oDrawable);

            oListView.SetDividerHeight(1);

            var jso_ListView_cb = (function(){
                function ShowAlert(asInfo) {
                    var oBuilder = Droid_New("CAlertDialogBuilder", oActivity);
                    oBuilder.SetTitleEx( CString("CAR NODE DUANG!") );
                    oBuilder.SetMessageEx( CString(asInfo) );

                    var jso_Alert_cb = (function(){
                        return {
                            OnClick:function(aoDialogInterface, aiWhich){
                                var BUTTON_POSITIVE = -1;
                                var BUTTON_NEGATIVE = -2;
                                var BUTTON_NEUTRAL = -3;

                                switch (aiWhich) {
                                    case BUTTON_POSITIVE:
                                        whmlog('====Alert========点击了确定按钮========');
                                        break;
                                    case BUTTON_NEGATIVE:
                                        whmlog('====Alert========点击了取消按钮========');
                                        break;
                                    case BUTTON_NEUTRAL:
                                        whmlog('====Alert========点击了中立按钮========');
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    })();

                    oBuilder.SetPositiveButtonEx( CString("确定"), jso_Alert_cb);

                    var oDialog = oBuilder.Create();
                    oDialog.Show();

                    return;
                }

                return {
                    OnItemClick:function(aoParent, aoView, aiPosition, aiId){
                        whmlog("====CActivityOne::OnItemClick position = "+ aiPosition  + ", id = " + aiId);
                        ShowAlert("Choosed item " + aiPosition + " with NodeJS");
                    },
                    OnItemLongClick:function(aoParent, aoView, aiPosition, aiId, out_aiResult){
                        whmlog("====CActivityOne::OnItemLongClick position = "+ aiPosition  + ", id = " + oId);
                        out_aiResult.data = true;
                    }
                }
            })();
            oListView.SetOnItemClickListener(jso_ListView_cb);

            //----------------myListView end----------------

            //----------------NavigationBar begin----------------

            //CreateNavigationBar();
            //ECode CActivityOne::CreateNavigationBar(){...}

            //----------------NavigationBar end----------------

            return;
        },  //OnCreate
        OnStart:function(aoContext){
            whmlog('====jso_activity_cb====OnStart.begin====');
        },
        OnResume:function(aoContext){
            whmlog('====jso_activity_cb====OnResume.begin====');
        },
        OnPause:function(aoContext){
            whmlog('====jso_activity_cb====OnPause.begin====');
        },
        OnStop:function(aoContext){
            whmlog('====jso_activity_cb====OnStop.begin====');
        },
        OnDestroy:function(aoContext){
            whmlog('====jso_activity_cb====OnDestroy.begin====');
        },
        OnActivityResult:function(aoContext, aiRequestCode, aiResultCode, aoIntentData){
            whmlog('====jso_activity_cb====OnActivityResult.begin====');
        },
        OnCreateDialog:function(aoContext, aiId, out_aoDialog){
            var builder = Droid_New("CAlertDialogBuilder", oActivity);

            builder.SetTitleEx(CString("NodeJS对话框"));
            builder.SetMessageEx(CString("这是一个NodeJS对话框"));

            var clickListener = (function(){
                return {
                    OnClick:function(aoDialogInterface, aiWhich){
                        var BUTTON_POSITIVE = -1;
                        var BUTTON_NEGATIVE = -2;
                        var BUTTON_NEUTRAL = -3;

                        switch (aiWhich) {
                            case BUTTON_POSITIVE:
                                whmlog('==============点击了确W定按钮=================');
                                break;
                            case BUTTON_NEGATIVE:
                                whmlog('==============点击了取H消按钮=================');
                                break;
                            case BUTTON_NEUTRAL:
                                whmlog('==============点击了中M立按钮=================');
                                break;
                            default:
                                break;
                        }
                    }
                }
            })();

            builder.SetPositiveButtonEx(CString("确定EPK"), clickListener);
            builder.SetNeutralButtonEx(CString("中立NODE"), clickListener);
            builder.SetNegativeButtonEx(CString("取消JS"), clickListener);

            var dlg = builder.Create();
            out_aoDialog.data = dlg;

            return;
        },   //OnCreateDialog
        OnCreateContextMenu:function(aoContext, aoMenu, aoV, aoMenuInfo){
            whmlog('====jso_activity_cb====OnCreateContextMenu.begin====');
        },
        CreateNavigationBar:function(aoContext){
            whmlog('====jso_activity_cb====CreateNavigationBar.begin====');
        },
    }
})(Elastos);

Application.Ready(jso_activity_cb);

whmlog('=====================test_basic.js end===================================');
