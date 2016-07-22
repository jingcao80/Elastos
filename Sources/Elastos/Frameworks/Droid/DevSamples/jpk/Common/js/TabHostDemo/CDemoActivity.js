module.exports = function(aoElastos, aoActivity){
    //common definition
    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;

    var oActivity = aoActivity.ActivityInstance;
    var oHandler = aoActivity.ActivityHandler;

    var TAG = "TabHostDemo::CDemoActivity : ";

    return {

// ECode CActivityOne::OnCreate(
        OnCreate:function(aoContext,
//     /* [in] */ IBundle* savedInstanceState)
            savedInstanceState)
// {
        {
//     Logger::I(TAG, " >> OnCreate()");
            elog(TAG + " >> OnCreate()");
//     TabActivity::OnCreate(savedInstanceState);
//     SetContentView(R::layout::tabhost);
            oActivity.SetContentView(R.layout.tabhost);

//     AutoPtr<ITabHost> tabHost;
            var tabHost;
//     GetTabHost((ITabHost**)&tabHost);
            tabHost = oActivity.GetTabHost();

//     AutoPtr<ITabSpec> page1;
            var page1;
//     tabHost->NewTabSpec(String("tab1"), (ITabSpec**)&page1);
            page1 = tabHost.NewTabSpec("tab1");
//     page1->SetIndicator(CoreUtils::Convert("叶子"));
            page1.SetIndicator(CString("叶子"));
//     page1->SetContent(R::id::leaf);
            page1.SetContent(R.id.leaf);
//     tabHost->AddTab(page1);
            tabHost.AddTab(page1);

//     AutoPtr<ITabSpec> page2;
            var page2;
//     tabHost->NewTabSpec(String("tab2"), (ITabSpec**)&page2);
            page2 = tabHost.NewTabSpec("tab2");
//     page2->SetIndicator(CoreUtils::Convert("彩叶"));
            page2.SetIndicator(CString("彩叶"));
//     page2->SetContent(R::id::colorleaf);
            page2.SetContent(R.id.colorleaf);
//     tabHost->AddTab(page2);
            tabHost.AddTab(page2);

//     AutoPtr<ITabSpec> page3;
            var page3;
//     tabHost->NewTabSpec(String("tab3"), (ITabSpec**)&page3);
            page3 = tabHost.NewTabSpec("tab1");
//     page3->SetIndicator(CoreUtils::Convert("碟子"));
            page3.SetIndicator(CString("碟子"));
//     page3->SetContent(R::id::cupple);
            page3.SetContent(R.id.cupple);
//     tabHost->AddTab(page3);
            tabHost.AddTab(page3);

//     AutoPtr<ITabHostOnTabChangeListener> listener = new MyListener(this);
//     tabHost->SetOnTabChangedListener(listener);
//     return NOERROR;
// }
            tabHost.SetOnTabChangedListener( (function(){
                return {
                    OnTabChanged : function(asTabId){
                        elog(TAG + "MyListener::OnTabChanged " + asTabId);
                    },
                };
            })() );

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
            elog('====jso_activity_cb====OnCreateDialog.begin====');
        },   //OnCreateDialog
        OnCreateContextMenu:function(aoContext, aoMenu, aoV, aoMenuInfo){
            elog('====jso_activity_cb====OnCreateContextMenu.begin====');
        },
        CreateNavigationBar:function(aoContext){
            elog('====jso_activity_cb====CreateNavigationBar.begin====');
        },
        OnExchangeData : function(inObject_0, inObject_1, inObject_2, inObject_3, inObject_4, outObject_0, outObject_1, outObject_2, outObject_3, outObject_4) {
            elog('====jso_activity_cb====OnExchangeData.begin====');


        },
    }
};