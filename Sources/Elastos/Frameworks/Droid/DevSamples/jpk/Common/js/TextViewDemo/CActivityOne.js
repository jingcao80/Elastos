elog("========CActivityOne.js========begin========");

module.exports = function(aoElastos, aoActivity){
    //common definition
    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;

    var oActivity = aoActivity.ActivityInstance;
    var oHandler = aoActivity.ActivityHandler;

    function _showInfo(s) {
        s = "    Info: " + s;
        var oInfo = oActivity.FindViewById(R.id.myTextView);
        oInfo.SetText(CString(s));
        elog(s);
    }

    function _startEPK(asName) {
        var IContext__ACTIVITY_SERVICE = "activity";
        var sm = Droid_New('Elastos.Droid.Os.CServiceManager');
        var am = sm.GetService(IContext__ACTIVITY_SERVICE);

        var intent = Droid_New('Elastos.Droid.Content.CIntent');
        intent.SetPackage(asName);
        intent.SetAction("android.intent.action.MAIN");

        var status = am.StartActivity(null, intent, '', null, '', 0, 0, '', null, null);
    }

    return {
        //OnCreate:function(aoContext){
        OnCreate:function(aoContext, aoSavedInstanceState){
            elog("========CActivityOne.js====OnCreate====begin========");

            //----------------SetContentView begin----------------
            oActivity.SetContentView(R.layout.main);
            //----------------SetContentView end----------------

            elog("========CActivityOne.js====OnCreate====begin.1========");

            //----------------myTextView begin----------------
            var oMyTextView = oActivity.FindViewById(R.id.myTextView);
            var jso_button_MyTextView_cb = (function(){
                return {
                    OnClick:function(aoView){
                        _showInfo('====jso_activity_cb====MyTextView.OnClick.Start====');
                    }
                }
            })();
            oMyTextView.SetOnClickListener(jso_button_MyTextView_cb);
            //----------------myTextView begin----------------

            elog("========CActivityOne.js====OnCreate====begin.2========");

            //----------------myEditText begin----------------
            var oMyEditText = oActivity.FindViewById(R.id.myEditText);
            var jso_button_MyEditText_OnKeyListener = (function(){
                return {
                    OnKey:function(aoView, aiKeyCode, aoEvent, out_abResult){
                        //TODO: the listener also used in DialogButton

                        var iId = aoView.GetId();
                        var iAction = aoEvent.GetAction();
                        var sLabel = aoEvent.GetDisplayLabel();

                        _showInfo("====CActivityOne::MyListener::OnKey: view '" + iId + "', keyCode: '" + aiKeyCode + "','" + sLabel + "'");

                        var KeyEvent__ACTION_DOWN = 0;
                        var KeyEvent__ACTION_UP   = 1;
                        var KeyEvent__KEYCODE_0   = 7;
                        if (iAction == IKeyEvent__ACTION_DOWN) {
                            if (aiKeyCode == IKeyEvent__KEYCODE_0) {
                                _showInfo("====CActivityOne====Key 0 is down.");
                            }
                        }
                        else if (iAction == IKeyEvent__ACTION_UP) {
                                _showInfo("====CActivityOne====IKeyEvent__ACTION_UP received.");
                        }

                        out_abResult.data = false;

                        return;
                    }
                }
            })();
            oMyTextView.SetOnKeyListener(jso_button_MyEditText_OnKeyListener);
            //----------------myEditText begin----------------

            elog("========CActivityOne.js====OnCreate====begin.3========");

            //----------------chkAndroid/chkIos begin----------------
            var oChkAndroid = oActivity.FindViewById(R.id.chkAndroid);
            var oChkIos = oActivity.FindViewById(R.id.chkIos);
            var oChkUbuntu = oActivity.FindViewById(R.id.chkUbuntu);

            var jso_button_group_cb = {
                OnClick:function(aoView){
                    //var id = aoView.GetId();
                    //var check = ( id == R.id.chkAndroid );
                    //_showInfo( (check?"Android":"Ios") + " button clicked" );
                    //oChkAndroid.SetChecked(check);
                    //oChkIos.SetChecked(!check);

                    oChkAndroid.SetChecked(false);
                    oChkIos.SetChecked(false);
                    oChkUbuntu.SetChecked(false);
                    aoView.SetChecked(true);

                    elog("========CActivityOne.js====chkButton====clicked====0====");
                    _showInfo("========CActivityOne.js====chkButton====clicked====1====");
                    elog("========CActivityOne.js====chkButton====clicked====2====");

                }
            };
            oChkAndroid.SetOnClickListener(jso_button_group_cb);
            oChkIos.SetOnClickListener(jso_button_group_cb);
            oChkUbuntu.SetOnClickListener(jso_button_group_cb);
            //----------------chkAndroid/chkIos end----------------

            elog("========CActivityOne.js====OnCreate====begin.4========");

            //----------------AnamtionButton begin----------------
            oActivity.FindViewById(R.id.AnamtionButton).SetOnClickListener( (function(){
                elog("========CActivityOne.js====OnCreate====begin.4.1========");

                var iSelf = 1;  //RELATIVE_TO_SELF;
                var oAnimation = {
                    Alpha :     Droid_New("Elastos.Droid.View.Animation.CAlphaAnimation", 0.3, 1.0),
                    Rotate :    Droid_New("Elastos.Droid.View.Animation.CRotateAnimation", 0.0, 350.0, iSelf, 0.5, iSelf, 0.5),
                    Scale :     Droid_New("Elastos.Droid.View.Animation.CScaleAnimation", 0.2, 1.4, 0.2, 1.4, iSelf, 0.5, iSelf, 0.5),
                    Translate : Droid_New("Elastos.Droid.View.Animation.CTranslateAnimation", 300.0, -20.0, -10.0, 30.0)
                };

                elog("========CActivityOne.js====OnCreate====begin.4.2========");

                var keys = [];  //Object.keys()
                for (var prop in oAnimation) {
                    elog("========CActivityOne.js====OnCreate====begin.4.2.1========"+prop);
                    keys.push(prop);
                    elog("========CActivityOne.js====OnCreate====begin.4.2.2.1========"+typeof oAnimation[prop]);
                    elog("========CActivityOne.js====OnCreate====begin.4.2.2.2========"+typeof oAnimation[prop].SetDuration);
                    oAnimation[prop].SetDuration(3000);
                    elog("========CActivityOne.js====OnCreate====begin.4.2.3========"+prop);
                }

                elog("========CActivityOne.js====OnCreate====begin.4.3========");

                var oButton = oActivity.FindViewById(R.id.DialogButton);
                var count = 0;

                elog("========CActivityOne.js====OnCreate====begin.4.4========");

                return {
                    OnClick:function(aoView){
                        elog("========CActivityOne.js====Annimation::OnClick====begin========");
                        count = count % 4;
                        var key = keys[count];
                        //_showInfo("====" + key + " Animation Selected!");
                        oButton.StartAnimation(oAnimation[key]);
                        count++;
                    }
                }
            })() );
            //----------------AnamtionButton end----------------

            elog("========CActivityOne.js====OnCreate====begin.5========");

            //----------------PopupWindowButton begin----------------
            function OnCreatePopupWindow(){
                elog("========CActivityOne.js====OnCreate====OnCreatePopupWindow====1====");

                _showInfo("====PopupWindow will be shown!");

                var IGravity__AXIS_X_SHIFT = 0;
                var IGravity__AXIS_Y_SHIFT = 4;
                var IGravity__AXIS_SPECIFIED = 0x0001;
                var IGravity__CENTER_VERTICAL = IGravity__AXIS_SPECIFIED<<4;
                var IGravity__CENTER_HORIZONTAL = IGravity__AXIS_SPECIFIED<<0;
                var IGravity__CENTER = IGravity__CENTER_VERTICAL | IGravity__CENTER_HORIZONTAL;

                var IContext__LAYOUT_INFLATER_SERVICE = "layout_inflater";

                elog("========CActivityOne.js====OnCreate====OnCreatePopupWindow====2====");

                var oInflater = oActivity.GetSystemService(IContext__LAYOUT_INFLATER_SERVICE);
                elog("========CActivityOne.js====OnCreate====OnCreatePopupWindow====2.1====");
                var oLayout = oInflater.Inflate(R.layout.popupwindow, null);

                elog("========CActivityOne.js====OnCreate====OnCreatePopupWindow====3====");

                var oPopupWindow = Droid_New("Elastos.Droid.Widget.CPopupWindow", oLayout, 350, 350, true);

                elog("========CActivityOne.js====OnCreate====OnCreatePopupWindow====4====");

                oPopupWindow.ShowAtLocation(oLayout, IGravity__CENTER, 0, 0);

                elog("========CActivityOne.js====OnCreate====OnCreatePopupWindow====5====");

                var oTextView = oLayout.FindViewById(R.id.txtView);
                //textView->SetText(CoreUtils::Convert("PopupWindow 测试程序!"));
                elog("========CActivityOne.js====OnCreate====OnCreatePopupWindow====6====");
                oTextView.SetText(CString("PopupWindow 测试程序!"));
                elog("========CActivityOne.js====OnCreate====OnCreatePopupWindow====7====");

                var jso_button_DismissButton_cb = (function(ao_PopupWindow){
                    return {
                        OnClick:function(aoView){
                            _showInfo("====PopupWindow Dismiss!");
                            ao_PopupWindow.Dismiss();
                            ao_PopupWindow = null;
                            return;
                        }
                    }
                })(oPopupWindow);

                elog("========CActivityOne.js====OnCreate====OnCreatePopupWindow====8====");
                var oDismissButton = oLayout.FindViewById(R.id.btn_close_popup);
                elog("========CActivityOne.js====OnCreate====OnCreatePopupWindow====9====");
                oDismissButton.SetOnClickListener(jso_button_DismissButton_cb);
                elog("========CActivityOne.js====OnCreate====OnCreatePopupWindow====10====");

                return;
            }

            var oPopupWindowButton = oActivity.FindViewById(R.id.PopupWindowButton);
            var jso_button_PopupWindowButton_cb = (function(){
                return {
                    OnClick:function(aoView){
                        _showInfo("====PopupWindow button clicked!");
                        OnCreatePopupWindow();
                    }
                }
            })();
            oPopupWindowButton.SetOnClickListener(jso_button_PopupWindowButton_cb);
            //----------------PopupWindowButton end----------------

            elog("========CActivityOne.js====OnCreate====begin.6========");

            //----------------DialogButton begin----------------
            var oDialogButton = oActivity.FindViewById(R.id.DialogButton);
            var jso_button_DialogButton_cb = (function(){
                return {
                    OnClick:function(aoView){
                        _showInfo("====Dialog button clicked!");
                        oActivity.ShowDialog(0);
                    }
                }
            })();
            oDialogButton.SetOnClickListener(jso_button_DialogButton_cb);
            //----------------DialogButton end----------------

            elog("========CActivityOne.js====OnCreate====begin.7========");

            //----------------ConnectivityManagerButton begin----------------
            function OnTestConnectivityManager(){
                elog('====jso_activity_cb====OnTestConnectivityManager.begin====');
                //TODO
            }
            var oConnectivityManagerButton = oActivity.FindViewById(R.id.ConnectivityManagerButton);
            var jso_button_ConnectivityManagerButton_cb = (function(){
                return {
                    OnClick:function(aoView){
                        _showInfo("====ConnectivityManagerButton button clicked!");
                        OnTestConnectivityManager();
                    }
                }
            })();
            oConnectivityManagerButton.SetOnClickListener(jso_button_ConnectivityManagerButton_cb);
            //----------------ConnectivityManagerButton end----------------

            elog("========CActivityOne.js====OnCreate====begin.8========");

            //----------------PowerManagerButton begin----------------
            function OnTestPowerManager(){
                elog('====jso_activity_cb====OnTestPowerManager.begin====');
                //TODO
            }
            var oPowerManagerButton = oActivity.FindViewById(R.id.PowerManagerButton);
            var jso_button_PowerManagerButton_cb = (function(){
                return {
                    OnClick:function(aoView){
                        _showInfo("====PowerManagerButton button clicked!");
                        OnTestPowerManager();
                    }
                }
            })();
            oPowerManagerButton.SetOnClickListener(jso_button_PowerManagerButton_cb);
            //----------------PowerManagerButton end----------------

            elog("========CActivityOne.js====OnCreate====begin.9========");

            //----------------myListView begin----------------
            elog("========CActivityOne.js====OnCreate====begin.9.1========");
            var oListView = oActivity.FindViewById(R.id.myListView);
            elog("========CActivityOne.js====OnCreate====begin.9.2========");

            // var oDataList = Droid_New("Elastos.Droid.Utility.CParcelableObjectContainer");
            // elog("========CActivityOne.js====OnCreate====begin.9.3========");
            // for (var i = 0; i < 5; ++i) {
            //     var s = 'Item--' + i;
            //     if (i == 0) {
            //         s += " Click to start ImageViewDemo EPK!";
            //     }
            //     else if (i == 1) {
            //         s += " Click to start Gallery EPK!";
            //     }
            //     oDataList.Add( CString(s) );
            // }


            //var oDataList = Droid_New("Elastos.Utility.CArrayList");
            var oDataList = Core_New("Elastos.Utility.CArrayList");

            elog("========CActivityOne.js====OnCreate====begin.9.3========");
            for (var i=0; i< 2; i++) {
                elog("========CActivityOne.js====OnCreate====begin.9.3.0========"+i);
                var s = "Item " + i;
                elog("========CActivityOne.js====OnCreate====begin.9.3.1========");
                var s1 = CString(s);
                elog("========CActivityOne.js====OnCreate====begin.9.3.2========" + s1.ToString());
                oDataList.Add(s1);
                elog("========CActivityOne.js====OnCreate====begin.9.3.3========");
                //oDataList.add(CString("Item " + i));
            }

            elog("========CActivityOne.js====OnCreate====begin.9.4========");

            var oAdapter = Droid_New("Elastos.Droid.Widget.CArrayAdapter", oActivity, R.layout.list_item, oDataList);
            elog("========CActivityOne.js====OnCreate====begin.9.5========");

            oListView.SetAdapter(oAdapter);
            elog("========CActivityOne.js====OnCreate====begin.9.6========");

            var oDrawable = Droid_New("Elastos.Droid.Graphics.Drawable.CColorDrawable", 0xFF0000FF);
            elog("========CActivityOne.js====OnCreate====begin.9.7========");

            oListView.SetDivider(oDrawable);
            elog("========CActivityOne.js====OnCreate====begin.9.8========");

            oListView.SetDividerHeight(1);
            elog("========CActivityOne.js====OnCreate====begin.9.9========");

            var jso_ListView_cb = (function(){
                var iPosition = 0;
                var sEpkNames = ["ImageViewDemo", "Gallery"];

                function ShowAlert(asInfo) {
                    var oBuilder = Droid_New("Elastos.Droid.App.CAlertDialogBuilder", oActivity);

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
                                        _showInfo('====Alert========点击了确定按钮========');
                                        if (iPosition < 2) {
                                            _startEPK(sEpkNames[iPosition]);
                                        }
                                        break;
                                    case BUTTON_NEGATIVE:
                                        _showInfo('====Alert========点击了取消按钮========');
                                        break;
                                    case BUTTON_NEUTRAL:
                                        _showInfo('====Alert========点击了中立按钮========');
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
                        iPosition = aiPosition;

                        _showInfo("====CActivityOne::OnItemClick position = "+ iPosition  + ", id = " + aiId);

                        ShowAlert("Choosed item " + iPosition + " with NodeJS");

                        if (iPosition == 0) {
                            _showInfo("confirm to start ImageViewDemo EPK!");
                        }
                        else if (iPosition == 0) {
                            _showInfo("confirm to start Gallery EPK!");
                        }
                    },
                    OnItemLongClick:function(aoParent, aoView, aiPosition, aiId, out_aiResult){
                        _showInfo("====CActivityOne::OnItemLongClick position = "+ aiPosition  + ", id = " + oId);
                        out_aiResult.data = true;
                    }
                }
            })();
            elog("========CActivityOne.js====OnCreate====begin.9.10========");

            oListView.SetOnItemClickListener(jso_ListView_cb);
            //----------------myListView end----------------

            elog("========CActivityOne.js====OnCreate====begin.10========");

            //----------------NavigationBar begin----------------

            //CreateNavigationBar();
            //ECode CActivityOne::CreateNavigationBar(){...}

            //----------------NavigationBar end----------------

            elog("========CActivityOne.js====OnCreate====end========");

            return;
        },  //OnCreate
        OnStart:function(aoContext){
            elog('====jso_activity_cb====OnStart.begin====');
        },
        OnResume:function(aoContext){
            elog('====jso_activity_cb====OnResume.begin====');
        },
        OnPause:function(aoContext){
            elog('====jso_activity_cb====OnPause.begin====');
        },
        OnStop:function(aoContext){
            elog('====jso_activity_cb====OnStop.begin====');
        },
        OnDestroy:function(aoContext){
            elog('====jso_activity_cb====OnDestroy.begin====');
        },
        OnActivityResult:function(aoContext, aiRequestCode, aiResultCode, aoIntentData){
            elog('====jso_activity_cb====OnActivityResult.begin====');
        },
        OnCreateDialog:function(aoContext, aiId, out_aoDialog){
            var builder = Droid_New("Elastos.Droid.App.CAlertDialogBuilder", oActivity);

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
                                _showInfo('==============点击了确W定按钮=================');
                                break;
                            case BUTTON_NEGATIVE:
                                _showInfo('==============点击了取H消按钮=================');
                                break;
                            case BUTTON_NEUTRAL:
                                _showInfo('==============点击了中M立按钮=================');
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
            _showInfo('====jso_activity_cb====OnCreateContextMenu.begin====');
        },
        CreateNavigationBar:function(aoContext){
            _showInfo('====jso_activity_cb====CreateNavigationBar.begin====');
        },
    }
};

elog("========CActivityOne.js========end========");
