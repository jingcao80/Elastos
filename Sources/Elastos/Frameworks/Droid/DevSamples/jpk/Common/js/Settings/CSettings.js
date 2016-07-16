module.exports = function(aoElastos, aoActivity){
//--------common definition----begin----
    var CObject = aoElastos.CObject;

    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;

    var oActivity = aoActivity.ActivityInstance;
    var oHandler = aoActivity.ActivityHandler;

    function CActivityListener(host) {
        this.mHost = host;
    }
    var _apt = CActivityListener.prototype;

    var IView__VISIBLE = 0x00000000;
    var IView__INVISIBLE = 0x00000004;
    var IView__GONE = 0x00000008;
    var IMotionEvent__ACTION_DOWN = 0;
    var IMotionEvent__ACTION_UP = 1;
    var IContext__LAYOUT_INFLATER_SERVICE = "layout_inflater";

//--------common definition----end----

//--------CxxxActivity.h----begin----

//--------CxxxActivity.h----end----

//--------CxxxActivity.cpp----begin----

    _apt.OnCreate = function(context, savedInstanceState){
        elog('====jso_activity_cb====OnCreate.begin====');

        context.SetContentView(R.layout.settings_main_dashboard);
        context.SetContentView(R.layout.settings_main_prefs);

        elog('====jso_activity_cb====OnCreate.end====');
    }  //OnCreate

//--------CxxxActivity.cpp----end----

//--------CActivityListener----default begin----

    // _apt.OnCreate = function(context, savedInstanceState){
    //     elog('====jso_activity_cb====OnCreate.begin====');
    // }  //OnCreate
    _apt.OnStart = function(context){
        elog('====jso_activity_cb====OnStart.begin====');
    }
    _apt.OnResume = function(context){
        elog('====jso_activity_cb====OnResume.begin====');
    }
    _apt.OnPause = function(context){
        elog('====jso_activity_cb====OnPause.begin====');
    }
    _apt.OnStop = function(context){
        elog('====jso_activity_cb====OnStop.begin====');
    }
    _apt.OnDestroy = function(context){
        elog('====jso_activity_cb====OnDestroy.begin====');
    }
    _apt.OnActivityResult = function(context, aiRequestCode, aiResultCode, aoIntentData){
        elog('====jso_activity_cb====OnActivityResult.begin====');
    }
    _apt.OnCreateDialog = function(context, aiId, out_aoDialog){
        elog('====jso_activity_cb====OnCreateDialog.begin====');
    }   //OnCreateDialog
    _apt.OnCreateContextMenu = function(context, aoMenu, aoV, aoMenuInfo){
        elog('====jso_activity_cb====OnCreateContextMenu.begin====');
    }
    _apt.CreateNavigationBar = function(context){
        elog('====jso_activity_cb====CreateNavigationBar.begin====');
    }
    _apt.OnExchangeData = function(aiTimes ,aoInObject, aoOutObject) {
        elog('====jso_activity_cb====OnExchangeData.begin====');
    }
    _apt.OnHandleMessage = function(context, aoMessage){
        elog('====jso_activity_cb====OnHandleMessage.begin====');
    }

//--------CActivityListener----default end----

    return new CActivityListener(oActivity);
};  //module.exports