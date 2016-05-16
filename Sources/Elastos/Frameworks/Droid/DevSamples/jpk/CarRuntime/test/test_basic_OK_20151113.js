whmlog('=====================test_basic.js begin===================================');

var root = global||window;
root.ElastosTest || (root.ElastosTest = {});

root.elastos = require('./elastos.node');

var oLog = elastos.require("/data/elastos/JSTestLog.eco", "CTestLog");
var oReflection = elastos.require("/data/elastos/JSTestReflection.eco", "CTestReflection");
var oCarRoot = elastos.require("/data/elastos/JSCarRoot.eco", "CCarRoot");
var oTestEventListener = elastos.require("/data/elastos/TextViewDemo.eco", "CTestEventListener");

var oModule_Elastos_Core = oCarRoot.Test_Require_ModuleInfo("/system/lib/Elastos.Core.eco");
var oModule_Elastos_Droid = oCarRoot.Test_Require_ModuleInfo("/system/lib/Elastos.Droid.Core.eco");

var pEnqueueUIMessage = oTestEventListener.GetEnqueueUIMessagePtr();
elastos.SetEnqueueUIMessagePtr(pEnqueueUIMessage);

var iNodeBridge = elastos.GetNodeBridge();
oTestEventListener.SetNodeBridge(iNodeBridge,1);

var oActivity = oTestEventListener.GetMainActivity();

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
            oModuleInfo = oCarRoot.Test_Require_ModuleInfo(oModuleInfo);
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
                        //whmlog('==============argument type: to be finished========8.1===='+i+'====='+oTypeInfo.GetName()+'===='+iDataType+'===='+sCarDataType+'===='+sJsDataType);
                        break;
                }

            }
            newObject = oConstructorInfo.CreateObject(oArgumentList);
        }

        return newObject;
    }   //classinfo__createObject


function CString (asText) {
    return classinfo__createObject(oModule_Elastos_Core,"CStringWrapper",asText);
}


var jso_activity_cb = (function(){
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
                        whmlog('====jso_activity_cb====MyEditText.OnKey.Start====');
                        //TODO: the listener also used in DialogButton

                        //Int32 id;
                        //view->GetId(&id);
                        var iId = aoView.GetId();

                        //Int32 action;
                        //event->GetAction(&action);
                        var iAction = aoEvent.GetAction();
                        //Char32 label;
                        //event->GetDisplayLabel(&label);
                        var sLabel = aoEvent.GetDisplayLabel();

                        //Slogger::D("CActivityOne", "CActivityOne::MyListener::OnKey: view %08x, keyCode: %d, '%c'",
                        //    id, keyCode, (char)label);
                        whmlog("====CActivityOne::MyListener::OnKey: view '" + iId + "', keyCode: '" + aiKeyCode + "','" + sLabel + "'");

                        // if (action == IKeyEvent::ACTION_DOWN) {
                        //     if (keyCode == IKeyEvent::KEYCODE_0) {
                        //         Slogger::D("CActivityOne", "Key 0 is down.");
                        //     }
                        // }
                        // else if (action == IKeyEvent::ACTION_UP) {
                        // }
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

                        // if (result) {
                        //     *result = FALSE;
                        // }
                        out_abResult.data = false;

                        //return NOERROR;
                        return;
                    }
                }
            })();
            oMyTextView.SetOnKeyListener(jso_button_MyEditText_OnKeyListener);
            //----------------myEditText begin----------------

            //----------------chkAndroid begin----------------
            var oChkAndroid = oActivity.FindViewById(R.id.chkAndroid);
            var jso_button_android_cb = (function(){
                return {
                    OnClick:function(aoView){
                        oChkAndroid.SetChecked(true);
                        oChkIos.SetChecked(false);
                    }
                }
            })();
            oChkAndroid.SetOnClickListener(jso_button_android_cb);
            //----------------chkAndroid end----------------

            //----------------chkIos begin----------------
            var oChkIos = oActivity.FindViewById(R.id.chkIos);
            var jso_button_ios_cb = (function(){
                return {
                    OnClick:function(aoView){
                        oChkAndroid.SetChecked(false);
                        oChkIos.SetChecked(true);
                    }
                }
            })();
            oChkIos.SetOnClickListener(jso_button_ios_cb);
            //----------------chkIos end----------------

            //----------------AnamtionButton begin----------------
            var IAnimation__RELATIVE_TO_SELF = 1;

            var oAlphaAnimation = classinfo__createObject(
                oModule_Elastos_Droid,"CAlphaAnimation",
                0.3, 1.0
            );
            oAlphaAnimation.SetDuration(3000);

            var oCRotateAnimation = classinfo__createObject(
                oModule_Elastos_Droid,"CRotateAnimation",
                0.0, 350.0,
                IAnimation__RELATIVE_TO_SELF,
                0.5,
                IAnimation__RELATIVE_TO_SELF,
                0.5
            );
            oCRotateAnimation.SetDuration(3000);

            var oCScaleAnimation = classinfo__createObject(
                oModule_Elastos_Droid,"CScaleAnimation",
                0.2, 1.4, 0.2, 1.4,
                IAnimation__RELATIVE_TO_SELF,
                0.5,
                IAnimation__RELATIVE_TO_SELF,
                0.5
            );
            oCScaleAnimation.SetDuration(3000);

            var oCTranslateAnimation = classinfo__createObject(
                oModule_Elastos_Droid,"CTranslateAnimation",
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
                var oPopupWindow = classinfo__createObject(
                    oModule_Elastos_Droid,"CPopupWindow",
                    oLayout, 350, 350, true
                );

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

            //----------------myListView being----------------

            //temp = FindViewById(R::id::myListView);
            //mListView = IListView::Probe(temp);
            //assert(mListView != NULL);
            var oListView = oActivity.FindViewById(R.id.myListView);

            //AutoPtr<IObjectContainer> dataList;
            //CParcelableObjectContainer::New((IObjectContainer**)&dataList);
            //TODO: bug when just two parameters
            var oDataList = classinfo__createObject(oModule_Elastos_Droid,"CParcelableObjectContainer");
            //var oDataList = elastos.require("/system/lib/Elastos.Droid.Core.eco","CParcelableObjectContainer");

            //for (Int32 i = 0; i < 15; ++i) {
            for (var i = 0; i < 5; ++i) {
                //StringBuilder sb("Item ");
                //sb += i;
                //AutoPtr<ICharSequence> seq = sb.ToCharSequence();
                //dataList->Add(seq->Probe(EIID_IInterface));
                oDataList.Add( CString('Item--' + i) );
            }

            //AutoPtr<IArrayAdapter> adapter;
            //CArrayAdapter::New(this, R::layout::list_item, dataList, (IArrayAdapter**)&adapter);
            //assert(adapter != NULL);
            var oAdapter = classinfo__createObject(
                oModule_Elastos_Droid,"CArrayAdapter",
                oActivity, R.layout.list_item, oDataList
            );

            //mListView->SetAdapter(adapter.Get());
            oListView.SetAdapter(oAdapter);

            //AutoPtr<IColorDrawable> drawable;
            //CColorDrawable::New(0xFF0000FF, (IColorDrawable**)&drawable);
            //assert(drawable != NULL);
            var oDrawable = classinfo__createObject(
                oModule_Elastos_Droid,"CColorDrawable",
                0xFF0000FF
            );
            //mListView->SetDivider(drawable);
            oListView.SetDivider(oDrawable);

            //mListView->SetDividerHeight(1);
            oListView.SetDividerHeight(1);

            var jso_ListView_cb = (function(){
                function ShowAlert(asInfo) {
                    var oBuilder = classinfo__createObject(oModule_Elastos_Droid,"CAlertDialogBuilder", oActivity);
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

                    // ECode CActivityOne::MyListener::OnItemLongClick(
                    //     /* [in] */ IAdapterView* parent,
                    //     /* [in] */ IView* view,
                    //     /* [in] */ Int32 position,
                    //     /* [in] */ Int64 id,
                    //     /* [out] */ Boolean* result)
                    // {
                    //     Slogger::D("CActivityOne", "OnItemLongClick position = %d, id = %lld", position, id);
                    //     *result = TRUE;
                    //     return NOERROR;
                    // }
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
        OnCreateDialog:function(aoContext, aiId, aoDialog){
            var builder = classinfo__createObject(oModule_Elastos_Droid, "CAlertDialogBuilder", oActivity);

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
            aoDialog.data = dlg;

            return;
        },   //OnCreateDialog
        OnCreateContextMenu:function(aoContext, aoMenu, aoV, aoMenuInfo){
            whmlog('====jso_activity_cb====OnCreateContextMenu.begin====');
        },
        CreateNavigationBar:function(aoContext){
            whmlog('====jso_activity_cb====CreateNavigationBar.begin====');
        }
    }
})();

oTestEventListener.SetActivityListener(jso_activity_cb);

oTestEventListener.Unlock();

function loop() {
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
}
loop();

whmlog('=====================test_basic.js end===================================');
