module.exports = function(aoElastos, aoActivity){
    //--------common definition----begin----

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

    //--------common definition----end----

//--------CxxxActivity.h----begin----

// #ifndef __CABOUTACTIVITY_H__
// #define __CABOUTACTIVITY_H__

// #include "_CAboutActivity.h"
// #include "app/Activity.h"

// using Elastos::Droid::App::Activity;
// using Elastos::Droid::View::IViewOnClickListener;
// using Elastos::Droid::Widget::IImageButton;
// using Elastos::Droid::Widget::IImageView;
// using Elastos::Droid::Widget::ITextView;

// namespace Gallery {

// class CAboutActivity
//     : public Activity
// {
// public:
//     class MyListener
//         : public ElRefBase
//         , public IViewOnClickListener
//     {
//     public:
//         MyListener(
//              [in]  CAboutActivity* host);

//         CAR_INTERFACE_DECL();

//         CARAPI OnClick(
//             /* [in] */ IView* v);

//     private:
//         CAboutActivity* mHost;
//     };
    function MyListener() {}

// friend class MyListener;

// protected:
//     CARAPI OnCreate(
//         /* [in] */ IBundle* savedInstanceState);

//     CARAPI OnStart();

//     CARAPI OnResume();

//     CARAPI OnPause();

//     CARAPI OnStop();

//     CARAPI OnDestroy();

// private:
//     static const String TAG;
    var TAG;

// private:
//     AutoPtr<IImageView> mBackButton;
    var mBackButton;
// };

// } // namespace Gallery

// #endif // __CABOUTACTIVITY_H__

//--------CxxxActivity.h----end----

//--------CxxxActivity.cpp----begin----

// #include "CAboutActivity.h"
// #include "R.h"
// #include <elastos/Logger.h>
// #include "src/data/AsyncImageLoader.h"

// using Elastos::Core::CStringWrapper;
// using Elastos::Core::ICharSequence;
// using Elastos::Droid::Content::Pm::IPackageInfo;
// using Elastos::Droid::View::EIID_IViewOnClickListener;
// using Elastos::Droid::App::EIID_IActivity;
// using Elastos::Utility::Logging::Logger;

// namespace Gallery {

// const String CAboutActivity::TAG("CAboutActivity");
    TAG = "CAboutActivity";

// CAboutActivity::MyListener::MyListener(
//     /* [in] */ CAboutActivity* host)
//     : mHost(host)
// {}

// CAR_INTERFACE_IMPL(CAboutActivity::MyListener, IViewOnClickListener);

// ECode CAboutActivity::MyListener::OnClick(
//     /* [in] */ IView* v)
    MyListener.prototype.OnClick = function(v)
// {
    {
        var mHost = oActivity;

//     Int32 viewId = 0;
//     v->GetId(&viewId);
        var viewId = v.GetId();

//     switch(viewId) {
        switch(viewId) {
//         case R::id::about_back: {
            case R.id.about_back:
//             Logger::D(TAG, "OnClick()---about_back");
                elog(TAG + "OnClick()---about_back");
//             mHost->OnBackPressed();
                mHost.OnBackPressed();

//             break;
                break;
//         }
//         default:
            default:
//             break;
                break;
//     }
        }

//     return NOERROR;
        return;
// }
    }

// ECode CAboutActivity::OnCreate(
//     /* [in] */ IBundle* savedInstanceState)
    _apt.OnCreate = function(context, savedInstanceState)
// {
    {
        var mHost = context;

//     Logger::D(TAG, "OnCreate()---");
        elog(TAG + "OnCreate()---");
//     Activity::OnCreate(savedInstanceState);
        //mHost.super.OnCreate(savedInstanceState);
//     SetContentView(R::layout::activity_about);
        mHost.SetContentView(R.layout.activity_about);
//     AutoPtr<MyListener> l = new MyListener(this);
        var l = new MyListener();
//     AutoPtr<IView> view = FindViewById(R::id::about_back);
//     mBackButton = IImageView::Probe(view);
//     assert(mBackButton != NULL);
        mBackButton = mHost.FindViewById(R.id.about_back);
//     mBackButton->SetOnClickListener(l.Get());
        mBackButton.SetOnClickListener(l);
//     view = FindViewById(R::id::about_version);
//     AutoPtr<ITextView> versionView = ITextView::Probe(view);
//     assert(versionView != NULL);
        var versionView = mHost.FindViewById(R.id.about_version);

//     String pkgName, pkgVersion;
        var pkgName, pkgVersion;
//     AutoPtr<IPackageManager> pkgMgr;
        var pkgMgr;
//     AutoPtr<IPackageInfo> pkgInfo;
        var pkgInfo;

//     GetPackageName(&pkgName);
        pkgName = mHost.GetPackageName();
//     GetPackageManager((IPackageManager**)&pkgMgr);
        pkgMgr = mHost.GetPackageManager();
//     pkgMgr->GetPackageInfo(pkgName, 0, (IPackageInfo**)&pkgInfo);
        pkgInfo = pkgMgr.GetPackageInfo(pkgName, 0);
//     pkgInfo->GetVersionName(&pkgVersion);
        pkgVersion = pkgInfo.GetVersionName();
//     Logger::D(TAG, "OnCreate()---pkgVersion:%s", pkgVersion.string());
        elog(TAG + "OnCreate()---pkgVersion:" + pkgVersion);
//     AutoPtr<ICharSequence> cs;
        var cs;
//     CStringWrapper::New(pkgVersion, (ICharSequence**)&cs);
        cs = CString(pkgVersion);
//     versionView->SetText(cs);
        versionView.SetText(cs);

//     return NOERROR;
        return;
// }
    }

// ECode CAboutActivity::OnStart()
// {
//     Logger::D(TAG, "OnStart()---");
//     Activity::OnStart();
//     return NOERROR;
// }

// ECode CAboutActivity::OnResume()
// {
//     Logger::D(TAG, "OnResume()---");
//     Activity::OnResume();
//     return NOERROR;
// }

// ECode CAboutActivity::OnPause()
// {
//     Logger::D(TAG, "OnPause()---");
//     Activity::OnPause();
//     return NOERROR;
// }

// ECode CAboutActivity::OnStop()
// {
//     Logger::D(TAG, "OnStop()---");
//     Activity::OnStop();
//     return NOERROR;
// }

// ECode CAboutActivity::OnDestroy()
// {
//     Logger::D(TAG, "OnDestroy()---");
//     Activity::OnDestroy();
//     return NOERROR;
// }

// } // namespace Gallery

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