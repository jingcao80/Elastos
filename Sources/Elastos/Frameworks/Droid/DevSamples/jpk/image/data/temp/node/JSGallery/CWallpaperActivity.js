module.exports = function(aoElastos, aoActivity){
    //common definition
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

//--------CWallpaperActivity.h----begin----

// #ifndef __CWALLPAPERACTIVITY_H__
// #define __CWALLPAPERACTIVITY_H__

// #include "_CWallpaperActivity.h"
// #include "app/Activity.h"
// #include "src/data/AsyncImageLoader.h"
    var AsyncImageLoader = require("data/AsyncImageLoader.js");
// #include "os/HandlerBase.h"

// using Elastos::Droid::Os::HandlerBase;
// using Elastos::Droid::App::Activity;
// using Elastos::Droid::View::IViewOnClickListener;
// using Elastos::Droid::Widget::IImageView;

// namespace Gallery {

// class CWallpaperActivity
//     : public Activity
// {
// public:
//     class MyHandler : public HandlerBase
//     {
//     public:
//         MyHandler(
//             /* [in] */ IWeakReference* host)
//             : mWeakHost(host)
//         {}

//         CARAPI HandleMessage(
//             /* [in] */ IMessage* msg);
//     private:
//         AutoPtr<IWeakReference> mWeakHost;
//     };
    function MyHandler = function() {};

//     class MyListener
//         : public ElRefBase
//         , public IViewOnClickListener
//     {
//     public:
//         MyListener(
//             /* [in] */ CWallpaperActivity* host);

//         CAR_INTERFACE_DECL();

//         CARAPI OnClick(
//             /* [in] */ IView* v);

//     private:
//         CWallpaperActivity* mHost;
//     };
    function MyListener = function() {};

//     class MyLoadImageCallback
//         : public ILoadImageCallback
//     {
//     public:
//         MyLoadImageCallback(
//             /* [in] */ IWeakReference* host);

//         CARAPI ImageLoaded(
//             /* [in] */ IDrawable* imageDrawable,
//             /* [in] */ IImageView* imageView);

//     private:
//         AutoPtr<IWeakReference> mWeakHost;
//     };

// friend class MyListener;
// friend class MyLoadImageCallback;

// public:
//     CWallpaperActivity();

// protected:

//     CARAPI OnCreate(
//         /* [in] */ IBundle* savedInstanceState);

//     CARAPI OnStart();

//     CARAPI OnResume();

//     CARAPI OnPause();

//     CARAPI OnStop();

//     CARAPI OnDestroy();

// private:
//     CARAPI_(void) MyImageLoaded(
//         /* [in] */ IDrawable* imageDrawable,
//         /* [in] */ IImageView* imageView);

//     CARAPI_(void) SetWallpaper();

// private:
//     static const String TAG;
    var TAG;
//     static const Int32 MSG_IMAGE_LOADED;
    var MSG_IMAGE_LOADED;

// private:
//     String mFilePath;
    var mFilePath;
//     AutoPtr<IImageView> mBackButton;
    var mBackButton;
//     AutoPtr<IImageView> mWallpaperView;
    var mWallpaperView;
//     AutoPtr<MyListener> mListener;
    var mListener;
//     AutoPtr<IHandler> mMyHandler;
    var mMyHandler;
// };

// } // namespace Gallery

// #endif // __CWALLPAPERACTIVITY_H__

//--------CWallpaperActivity.h----end----

//--------CWallpaperActivity.cpp----begin----

// #include "CWallpaperActivity.h"
// #include "src/data/DataSourceHelper.h"
    var DataSourceHelper = require("src/data/DataSourceHelper.js");
// #include "src/data/AsyncImageLoader.h"
// #include "src/util/Utils.h"
// #include "os/SomeArgs.h"
// #include "R.h"
// #include <elastos/Logger.h>

// using Elastos::Droid::Os::SomeArgs;
// using Elastos::Droid::App::CWallpaperManagerHelper;
    var CWallpaperManagerHelper = Elastos.Droid.App.CWallpaperManagerHelper;
// using Elastos::Droid::App::IWallpaperManager;
// using Elastos::Droid::App::IWallpaperManagerHelper;
// using Elastos::Droid::View::EIID_IViewOnClickListener;
// using Elastos::Droid::Widget::ITextView;
// using Elastos::Droid::Widget::IToast;
    var IToast = Elastos.Droid.Widget.IToast;
// using Elastos::Droid::App::EIID_IActivity;
// using Elastos::Utility::Logging::Logger;

// namespace Gallery {

// const String CWallpaperActivity::TAG("CWallpaperActivity");
    TAG = "CWallpaperActivity";
// const Int32 CWallpaperActivity::MSG_IMAGE_LOADED = 0;
    MSG_IMAGE_LOADED = 0;

// ECode CWallpaperActivity::MyHandler::HandleMessage(
//     /* [in] */ IMessage* msg)
    MyHandler.prototype.HandleMessage = function(msg)
// {
    {
//     AutoPtr<IActivity> strongObj;
//     mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
//     if (strongObj == NULL) {
//         return NOERROR;
//     }

//     CWallpaperActivity* mHost = (CWallpaperActivity*)strongObj.Get();

//     Int32 what;
        var what;
//     msg->GetWhat(&what);
        what = msg.what;
//     AutoPtr<IInterface> obj;
        var obj;
//     msg->GetObj((IInterface**)&obj);
        obj = msg.obj;

//     switch(what) {
        switch(what) {
//         case CWallpaperActivity::MSG_IMAGE_LOADED: {
            case MSG_IMAGE_LOADED:
//             SomeArgs* args = (SomeArgs*)obj.Get();
                var args = obj;
//             IDrawable* drawable = IDrawable::Probe(args->mArg1);
                var drawable = args.mArg1;
//             IImageView* imageView = IImageView::Probe(args->mArg2);
                var imageView = args.mArg2;
//             mHost->MyImageLoaded(drawable, imageView);
                MyImageLoaded(drawable, imageView);
//             args->Recycle();
//             break;
                break;
//         }
//     }
        }

//     return NOERROR;
        return;
// }
    }

// CAR_INTERFACE_IMPL(CWallpaperActivity::MyListener, IViewOnClickListener)

// CWallpaperActivity::MyListener::MyListener(
//     /* [in] */ CWallpaperActivity* host)
//     : mHost(host)
// {}

// ECode CWallpaperActivity::MyListener::OnClick(
//     /* [in] */ IView* v)
    MyListener.prototype.OnClick = function(v)
// {
    {
//     Int32 viewId = 0;
        var viewId = 0;
//     v->GetId(&viewId);
        viewId = v.GetId();

//     switch(viewId) {
        switch(viewId) {
//         case R::id::wallpaper_back: {
            case R.id.wallpaper_back:
//             Logger::D(TAG, "OnClick()---wallpaper_back");
                elog(TAG + "OnClick()---wallpaper_back");
//             mHost->OnBackPressed();
                oActivity.OnBackPressed();
//             break;
                break;
//         }
//         case R::id::wallpaper_ok: {
            case R.id.wallpaper_ok:
//             Logger::D(TAG, "OnClick()---wallpaper_ok");
                elog(TAG + "OnClick()---wallpaper_ok");
//             mHost->SetWallpaper();
                SetWallpaper();
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

// CWallpaperActivity::MyLoadImageCallback::MyLoadImageCallback(
//     /* [in] */ IWeakReference* host)
//     : mWeakHost(host)
// {}

// ECode CWallpaperActivity::MyLoadImageCallback::ImageLoaded(
//     /* [in] */ IDrawable* imageDrawable,
//     /* [in] */ IImageView* imageView)
    MyLoadImageCallback.prototype.ImageLoaded = function(imageDrawable, imageView)
// {
    {
//     Logger::D(TAG, "ImageLoaded()-----");
        elog(TAG + "ImageLoaded()-----");

//     AutoPtr<IActivity> strongObj;
//     mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
//     if (strongObj == NULL) {
//         return NOERROR;
//     }

//     CWallpaperActivity* mHost = (CWallpaperActivity*)strongObj.Get();
//     if (mHost->mMyHandler == NULL) {
//         return NOERROR;
//     }

//     AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        var args = {};
//     args->mArg1 = imageDrawable;
        args.mArg1 = imageDrawable;
//     args->mArg2 = imageView;
        args.mArg2 = imageView;

//     AutoPtr<IMessage> msg;
        var msg;
//     mHost->mMyHandler->ObtainMessageEx(CWallpaperActivity::MSG_IMAGE_LOADED, args, (IMessage**)&msg);
        msg = {obj:args, what:MSG_IMAGE_LOADED};
//     Boolean result;
        var result;
//     return mHost->mMyHandler->SendMessage(msg, &result);
        result = mMyHandler.HandleMessage(msg);
// }
    }

// CWallpaperActivity::CWallpaperActivity()
// {
// }

// ECode CWallpaperActivity::OnCreate(
//     /* [in] */ IBundle* savedInstanceState)
    _apt.OnCreate = function(_this, savedInstanceState)
// {
    {
//     Logger::D(TAG, "OnCreate()---");
        elog(TAG + "OnCreate()---");
//     Activity::OnCreate(savedInstanceState);
//     SetContentView(R::layout::activity_wallpaper);
        _this.SetContentView(R.layout.activity_wallpaper);

//     AutoPtr<IWeakReference> weakHost;
//     GetWeakReference((IWeakReference**)&weakHost);
//     mMyHandler = new MyHandler(weakHost);
        var mMyHandler = new MyHandler();

//     mListener = new MyListener(this);
        var mListener = new MyListener();

//     AutoPtr<IView> view = FindViewById(R::id::wallpaper_back);
//     mBackButton = IImageView::Probe(view);
//     assert(mBackButton != NULL);
        mBackButton = _this.FindViewById(R.id.wallpaper_back);
//     view = FindViewById(R::id::wallpaper_photo);
//     mWallpaperView = IImageView::Probe(view);
//     assert(mWallpaperView != NULL);
        mWallpaperView = _this.FindViewById(R.id.wallpaper_photo);
//     mBackButton->SetOnClickListener((IViewOnClickListener*)(mListener->Probe(EIID_IViewOnClickListener)));
        mBackButton.SetOnClickListener(mListener);
//     view = FindViewById(R::id::wallpaper_ok);
//     AutoPtr<ITextView> okButton = ITextView::Probe(view);
//     assert(okButton != NULL);
        var okButton = _this.FindViewById(R.id.wallpaper_ok);
//     okButton->SetOnClickListener((IViewOnClickListener*)(mListener->Probe(EIID_IViewOnClickListener)));
        okButton.SetOnClickListener(mListener);

//     AutoPtr<IIntent> intent;
//     GetIntent((IIntent**)&intent);
        var intent = _this.GetIntent();
//     if (intent != NULL) {
        if (intent) {
//         intent->GetStringExtra(DataSourceHelper::SOURCE_PATH, &mFilePath);
            mFilePath = intent.GetStringExtra(DataSourceHelper.SOURCE_PATH);
//         if (!mFilePath.IsNullOrEmpty()) {
            if (mFilePath instanceof String) {
//             Logger::D(TAG, "OnCreate()---mFilePath:", mFilePath.string());
                elog(TAG + "OnCreate()---mFilePath:" + mFilePath);
//             AutoPtr<MyLoadImageCallback> myLoadImage = new MyLoadImageCallback(weakHost);
                var myLoadImage = new MyLoadImageCallback();
//             AutoPtr<IBitmapDrawable> drawable = AsyncImageLoader::LoadDrawable(mFilePath, TRUE, mWallpaperView, myLoadImage);
                var drawable = AsyncImageLoader.LoadDrawable(mFilePath, true, mWallpaperView, myLoadImage);
//             if (drawable != NULL) {
                if (drawable) {
//                 mWallpaperView->SetImageDrawable(IDrawable::Probe(drawable));
                    mWallpaperView.SetImageDrawable(drawable);
//             }
                }
//             else {
                else {
//                 mWallpaperView->SetImageResource(R::color::main_layout_bg);
                    mWallpaperView.SetImageResource(R.color.main_layout_bg);
//             }
                }
//         }
            }
//         else {
            else {
//             Logger::E(TAG, "mFilePath is null or empty!");
                elog(TAG + "mFilePath is null or empty");
//         }
            }
//     }
        }

//     return NOERROR;
        return;
// }
    }

// ECode CWallpaperActivity::OnStart()
// {
//     Logger::D(TAG, "OnStart()---");
//     Activity::OnStart();
//     return NOERROR;
// }

// ECode CWallpaperActivity::OnResume()
// {
//     Logger::D(TAG, "OnResume()---");
//     Activity::OnResume();
//     return NOERROR;
// }

// ECode CWallpaperActivity::OnPause()
// {
//     Logger::D(TAG, "OnPause()---");
//     Activity::OnPause();
//     return NOERROR;
// }

// ECode CWallpaperActivity::OnStop()
// {
//     Logger::D(TAG, "OnStop()---");
//     Activity::OnStop();
//     return NOERROR;
// }

// ECode CWallpaperActivity::OnDestroy()
// {
//     Logger::D(TAG, "OnDestroy()---");
//     Activity::OnDestroy();
//     return NOERROR;
// }

// void CWallpaperActivity::MyImageLoaded(
//     /* [in] */ IDrawable* imageDrawable,
//     /* [in] */ IImageView* imageView)
    MyImageLoaded = function(imageDrawable, imageView)
// {
    {
//     Logger::D(TAG, "MyImageLoaded()-----");
        elog(TAG + "MyImageLoaded()-----");
//     IBitmapDrawable* drawable = IBitmapDrawable::Probe(imageDrawable);
        var drawable = imageDrawable;
//     imageView->SetImageDrawable(drawable);
        imageView.SetImageDrawable(drawable);
// }

// void CWallpaperActivity::SetWallpaper()
    SetWallpaper = function()
// {
    {
        var _this = oActivity;

//     Logger::D(TAG, "SetWallpaper()-----");
        elog(TAG + "SetWallpaper()-----");
//     AutoPtr<IDrawable> drawable;
//     mWallpaperView->GetDrawable((IDrawable**)&drawable);
//     IBitmapDrawable* bitmapDrawable = IBitmapDrawable::Probe(drawable);
        var bitmapDrawable = mWallpaperView.GetDrawable();
//     Boolean succeed = FALSE;
        var succeed = false;
//     ECode ec = NOERROR;
        var ec = ECode.NOERROR;
//     if (bitmapDrawable) {
        if (bitmapDrawable) {
//         AutoPtr<IWallpaperManagerHelper> wallpaperHelper;
//         AutoPtr<IWallpaperManager> wallpaperMgr;
//         CWallpaperManagerHelper::AcquireSingleton((IWallpaperManagerHelper**)&wallpaperHelper);
//         assert(wallpaperHelper != NULL);
            var wallpaperHelper = Droid_New("CWallpaperManagerHelper");
//         wallpaperHelper->GetInstance(this, (IWallpaperManager**)&wallpaperMgr);
//         assert(wallpaperMgr != NULL);
            var wallpaperMgr = wallpaperHelper.GetInstance(_this);
//         AutoPtr<IBitmap> bitmap;
//         bitmapDrawable->GetBitmap((IBitmap**)&bitmap);
            var bitmap = bitmapDrawable.GetBitmap();
//         ec = wallpaperMgr->SetBitmap(bitmap);
            ec = wallpaperMgr.SetBitmap(bitmap);
//         succeed = (ec == NOERROR);
            succeed = (ec == ECode.NOERROR);
//     }
        }

//     if (!succeed) {
        if (!succeed) {
//         Logger::W(TAG, "SetWallpaper()----failed---ECode:%d", ec);
            elog(TAG + "SetWallpaper()----failed---ECode:" + ec);
//         Utils::MakeToast(this, String("壁纸设置失败！"), IToast::LENGTH_LONG);
            Utils.MakeToast(_this, String"壁纸设置失败！"), IToast.LENGTH_LONG);
//     }
        }
//     else {
        else {
//         Utils::MakeToast(this, String("壁纸设置成功！"), IToast::LENGTH_LONG);
            Utils.MakeToast(_this, String("壁纸设置成功！"), IToast.LENGTH_LONG);
//     }
        }
// }
    }

// } // namespace Gallery

//--------CWallpaperActivity.cpp----end----

//--------CActivityListener----default begin----

    // _apt.OnCreate = function(_this, savedInstanceState){
    //     elog('====jso_activity_cb====OnCreate.begin====');
    // }  //OnCreate
    _apt.OnStart = function(_this){
        elog('====jso_activity_cb====OnStart.begin====');
    }
    _apt.OnResume = function(_this){
        elog('====jso_activity_cb====OnResume.begin====');
    }
    _apt.OnPause = function(_this){
        elog('====jso_activity_cb====OnPause.begin====');
    }
    _apt.OnStop = function(_this){
        elog('====jso_activity_cb====OnStop.begin====');
    }
    _apt.OnDestroy = function(_this){
        elog('====jso_activity_cb====OnDestroy.begin====');
    }
    _apt.OnActivityResult = function(_this, aiRequestCode, aiResultCode, aoIntentData){
        elog('====jso_activity_cb====OnActivityResult.begin====');
    }
    _apt.OnCreateDialog = function(_this, aiId, out_aoDialog){
        elog('====jso_activity_cb====OnCreateDialog.begin====');
    }   //OnCreateDialog
    _apt.OnCreateContextMenu = function(_this, aoMenu, aoV, aoMenuInfo){
        elog('====jso_activity_cb====OnCreateContextMenu.begin====');
    }
    _apt.CreateNavigationBar = function(_this){
        elog('====jso_activity_cb====CreateNavigationBar.begin====');
    }
    _apt.OnExchangeData = function(aiTimes ,aoInObject, aoOutObject) {
        elog('====jso_activity_cb====OnExchangeData.begin====');
    }
    _apt.OnHandleMessage = function(_this, aoMessage){
        elog('====jso_activity_cb====OnHandleMessage.begin====');
    }

//--------CActivityListener----default end----

    return new CActivityListener(oActivity);
};  //module.exports