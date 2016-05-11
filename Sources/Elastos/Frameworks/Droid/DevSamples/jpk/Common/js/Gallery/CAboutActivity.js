module.exports = function(aoElastos, aoActivity){
    //common definition
    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;

    var oActivity = aoActivity.ActivityInstance;
    var oHandler = aoActivity.ActivityHandler;

    //member of CAboutActivity

    //private static const:
    var TAG = "CAboutActivity";
    //private:
    var mBackButton;

//----------------------------------------------------------

    return {
        OnCreate:function(aoContext){
            whmlog('====jso_activity_cb====OnCreate.begin====0');

            oActivity.SetContentView(R.layout.activity_about);

            mBackButton = oActivity.FindViewById(R.id.about_back);
            mBackButton.SetOnClickListener({
                OnClick : function(aoView) {
                    oActivity.OnBackPressed();
                },
            });

            var versionView = oActivity.FindViewById(R.id.about_version);
            var pkgName = oActivity.GetPackageName();
            var pkgMgr = oActivity.GetPackageManager();
            var pkgInfo = pkgMgr.GetPackageInfo(pkgName, 0);
            var pkgVersion = pkgInfo.GetVersionName();

            versionView.SetText(CString(pkgVersion));
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
        //OnExchangeData : function(inObject_0, inObject_1, inObject_2, inObject_3, inObject_4, outObject_0, outObject_1, outObject_2, outObject_3, outObject_4) {
        OnExchangeData : function(aiTimes ,aoInObject, aoOutObject) {
            whmlog('====jso_activity_cb====OnExchangeData.begin====');
        },
        OnHandleMessage:function(aoContext, aoMessage){
            whmlog('====jso_activity_cb====OnHandleMessage.begin====');
        },
    }
};  //module.exports