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
            //----------------SetContentView begin----------------
            oActivity.SetContentView(R.layout.main);
            //----------------SetContentView end----------------

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
            //----------------myTextView end----------------

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
            //----------------myEditText end----------------

            //----------------chkAndroid/chkIos begin----------------
            var oChkAndroid = oActivity.FindViewById(R.id.chkAndroid);
            var oChkIos = oActivity.FindViewById(R.id.chkIos);
            var oChkUbuntu = oActivity.FindViewById(R.id.chkUbuntu);

            var jso_button_group_cb = {
                OnClick:function(aoView){
                    oChkAndroid.SetChecked(false);
                    oChkIos.SetChecked(false);
                    oChkUbuntu.SetChecked(false);
                    aoView.SetChecked(true);

                    _showInfo("========CActivityOne.js====chkButton====clicked========");
                }
            };
            oChkAndroid.SetOnClickListener(jso_button_group_cb);
            oChkIos.SetOnClickListener(jso_button_group_cb);
            oChkUbuntu.SetOnClickListener(jso_button_group_cb);
            //----------------chkAndroid/chkIos/chkUbuntu end----------------

            //----------------AnamtionButton begin----------------
            oActivity.FindViewById(R.id.AnamtionButton).SetOnClickListener( (function(){
                var iSelf = 1;  //RELATIVE_TO_SELF;
                var oAnimation = {
                    Alpha :     Droid_New("Elastos.Droid.View.Animation.CAlphaAnimation", 0.3, 1.0),
                    Rotate :    Droid_New("Elastos.Droid.View.Animation.CRotateAnimation", 0.0, 350.0, iSelf, 0.5, iSelf, 0.5),
                    Scale :     Droid_New("Elastos.Droid.View.Animation.CScaleAnimation", 0.2, 1.4, 0.2, 1.4, iSelf, 0.5, iSelf, 0.5),
                    Translate : Droid_New("Elastos.Droid.View.Animation.CTranslateAnimation", 300.0, -20.0, -10.0, 30.0)
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
                        elog("========CActivityOne.js====Annimation::OnClick====begin========");
                        count = count % 4;
                        var key = keys[count];
                        oButton.StartAnimation(oAnimation[key]);
                        count++;
                    }
                }
            })() );
            //----------------AnamtionButton end----------------

            //----------------PopupWindowButton begin----------------
            function OnCreatePopupWindow(){
                _showInfo("====PopupWindow will be shown!");

                var IGravity__AXIS_X_SHIFT = 0;
                var IGravity__AXIS_Y_SHIFT = 4;
                var IGravity__AXIS_SPECIFIED = 0x0001;
                var IGravity__CENTER_VERTICAL = IGravity__AXIS_SPECIFIED<<4;
                var IGravity__CENTER_HORIZONTAL = IGravity__AXIS_SPECIFIED<<0;
                var IGravity__CENTER = IGravity__CENTER_VERTICAL | IGravity__CENTER_HORIZONTAL;

                var IContext__LAYOUT_INFLATER_SERVICE = "layout_inflater";

                var oInflater = oActivity.GetSystemService(IContext__LAYOUT_INFLATER_SERVICE);
                var oLayout = oInflater.Inflate(R.layout.popupwindow, null);
                var oPopupWindow = Droid_New("Elastos.Droid.Widget.CPopupWindow", oLayout, 350, 350, true);
                oPopupWindow.ShowAtLocation(oLayout, IGravity__CENTER, 0, 0);

                var oTextView = oLayout.FindViewById(R.id.txtView);
                oTextView.SetText(CString("PopupWindow By Node.JS!!!"));

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

                var oDismissButton = oLayout.FindViewById(R.id.btn_close_popup);
                oDismissButton.SetOnClickListener(jso_button_DismissButton_cb);

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

            //----------------myListView begin----------------
            var oListView = oActivity.FindViewById(R.id.myListView);
            var oDataList = Core_New("Elastos.Utility.CArrayList");

            for (var i=0; i< 15; i++) {
                var s = CString("Item " + i);
                oDataList.Add(s);
            }

            var oAdapter = Droid_New("Elastos.Droid.Widget.CArrayAdapter", oActivity, R.layout.list_item, oDataList);
            oListView.SetAdapter(oAdapter);

            var oDrawable = Droid_New("Elastos.Droid.Graphics.Drawable.CColorDrawable", 0xFF0000FF);
            oListView.SetDivider(oDrawable);

            oListView.SetDividerHeight(1);

            var jso_ListView_cb = (function(){
                var iPosition = 0;
                var sEpkNames = ["ImageViewDemo", "Gallery"];

                function ShowAlert(asInfo) {
                    var oBuilder = Droid_New("Elastos.Droid.App.CAlertDialogBuilder", oActivity);
                    oBuilder.SetTitle( CString("CAR NODE DUANG!") );
                    oBuilder.SetMessage( CString(asInfo) );

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

                    oBuilder.SetPositiveButton( CString("确定"), jso_Alert_cb);

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

            oListView.SetOnItemClickListener(jso_ListView_cb);
            //----------------myListView end----------------

            //----------------NavigationBar begin----------------

            //CreateNavigationBar();
            //ECode CActivityOne::CreateNavigationBar(){...}

            //----------------NavigationBar end----------------

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

            //builder.SetTitleEx(CString("NodeJS对话框"));
            builder.SetTitle(CString("NodeJS对话框"));
            //builder.SetMessageEx(CString("这是一个NodeJS对话框"));
            builder.SetMessage(CString("这是一个NodeJS对话框"));

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

            builder.SetPositiveButton(CString("确定EPK"), clickListener);
            builder.SetNeutralButton(CString("中立NODE"), clickListener);
            builder.SetNegativeButton(CString("取消JS"), clickListener);

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