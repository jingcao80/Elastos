module.exports = function(aoElastos, aoActivity){
    //common definition
    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;

    var oActivity = aoActivity.ActivityInstance;
    var oHandler = aoActivity.ActivityHandler;

    var ImageViewScaleType = {
        MATRIX :        0,
        FIT_XY :        1,
        FIT_START :     2,
        FIT_CENTER :    3,
        FIT_END :       4,
        CENTER :        5,
        CENTER_CROP :   6,
        CENTER_INSIDE : 7,
    };

    return {
        OnCreate:function(aoContext, aoSavedInstanceState){
            whmlog('====jso_activity_cb====OnCreate.begin====0====oActivity:'+typeof(oActvity));

            //var oActivity = aoContext;

            oActivity.SetContentView(R.layout.main);

            var view = oActivity.FindViewById(R.id.ImageView);
            view.SetImageResource(R.drawable.earth);

            view.SetScaleType(ImageViewScaleType.FIT_CENTER);

            view.SetOnClickListener( (function(){
                var IContext__ACTIVITY_SERVICE = "activity";

                return {
                    OnClick : function(aoView){
                        var sm = Droid_New('CServiceManager');
                        var am = sm.GetService(IContext__ACTIVITY_SERVICE);

                        var intent = Droid_New('CIntent');
                        intent.SetPackage("Gallery");
                        intent.SetAction("android.intent.action.MAIN");

                        var status = am.StartActivity(null, intent, '', null, '', 0, 0, '', null, null);
                    },
                };
            })() );
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
            whmlog('====jso_activity_cb====OnCreateDialog.begin====');
        },   //OnCreateDialog
        OnCreateContextMenu:function(aoContext, aoMenu, aoV, aoMenuInfo){
            whmlog('====jso_activity_cb====OnCreateContextMenu.begin====');
        },
        CreateNavigationBar:function(aoContext){
            whmlog('====jso_activity_cb====CreateNavigationBar.begin====');
        },
        OnExchangeData : function(inObject_0, inObject_1, inObject_2, inObject_3, inObject_4, outObject_0, outObject_1, outObject_2, outObject_3, outObject_4) {
            whmlog('====jso_activity_cb====OnExchangeData.begin====');


        },
    }
};