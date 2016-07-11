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

    var EIID_IActivity = [0x1068F256,0x8292,0x2C57,[0x7E,0xCB,0xFE,0xE1,0xA0,0xA4,0x0C,0x1B]];

//--------CEditActivity.h--------begin--------
// #ifndef __CEDITACTIVITY_H__
// #define __CEDITACTIVITY_H__

// #include "_CEditActivity.h"
// #include "app/Activity.h"
// #include "src/data/AsyncImageLoader.h"
    var AsyncImageLoader = require("./data/AsyncImageLoader.js")(aoElastos);
// #include "os/HandlerBase.h"

// using Elastos::Droid::Os::HandlerBase;
// using Elastos::Droid::App::Activity;
// using Elastos::Droid::View::IViewOnClickListener;
// using Elastos::Droid::Widget::IImageButton;
// using Elastos::Droid::Widget::IImageView;
// using Elastos::Droid::Widget::ITextView;

// namespace Gallery {

// class CEditActivity
//     : public Activity
// {
// public:
//     class MyHandler : public HandlerBase
    var MyHandler;
//     {
//     public:
//         MyHandler(
    MyHandler = function(
//             /* [in] */ IWeakReference* host)
        host)
//             : mWeakHost(host)
//         {}
    {
        this.mWeakHost = host;
    }

//         CARAPI HandleMessage(
//             /* [in] */ IMessage* msg);
//     private:
//         AutoPtr<IWeakReference> mWeakHost;
//     };

//     class MyListener
//         : public ElRefBase
//         , public IViewOnClickListener
    var MyListener;
//     {
//     public:
//         MyListener(
//             /* [in] */ CEditActivity* host);

//         CAR_INTERFACE_DECL();

//         CARAPI OnClick(
//             /* [in] */ IView* v);

//     private:
//         CEditActivity* mHost;
//     };

//     class MyLoadImageCallback
//         : public ILoadImageCallback
    var MyLoadImageCallback;
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
//     CEditActivity();

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

// private:
//     static const String TAG;
    var TAG;
//     static const Int32 MSG_IMAGE_LOADED;
    var MSG_IMAGE_LOADED;
// private:
//     AutoPtr<IImageView> mBackButton;
    var mBackButton;
//     AutoPtr<IImageView> mEditView;
    var mEditView;
//     String mFilePath;
    var mFilePath;
//     AutoPtr<IHandler> mMyHandler;
    var mMyHandler;
// };

// } // namespace Gallery

// #endif // __CEDITACTIVITY_H__
//--------CEditActivity.h--------end--------

//--------CEditActivity.cpp--------begin--------

// #include "CEditActivity.h"
// #include "src/data/DataSourceHelper.h"
    var DataSourceHelper = require("./data/DataSourceHelper.js")(aoElastos);
// #include "src/data/AsyncImageLoader.h"
// #include "R.h"
// #include <elastos/Logger.h>
// #include "os/SomeArgs.h"

// using Elastos::Droid::Os::SomeArgs;
// using Elastos::Droid::Content::CIntent;
// using Elastos::Droid::Content::IContext;
// using Elastos::Droid::Content::IIntent;
// using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
// using Elastos::Droid::Graphics::Drawable::IDrawable;
// using Elastos::Droid::View::EIID_IViewOnClickListener;
// using Elastos::Droid::App::EIID_IActivity;
// using Elastos::Utility::Logging::Logger;

// namespace Gallery {

// const String CEditActivity::TAG("CEditActivity");
    TAG = "CEditActivity";
// const Int32 CEditActivity::MSG_IMAGE_LOADED = 0;
    MSG_IMAGE_LOADED = 0;

// ECode CEditActivity::MyHandler::HandleMessage(
//     /* [in] */ IMessage* msg)
// {
    MyHandler.prototype.HandleMessage = function(msg) {
//     AutoPtr<IActivity> strongObj;
        var strongObj;
//     mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
        strongObj = this.mWeakHost.Resolve(EIID_IActivity);
//     if (strongObj == NULL) {
        if (!strongObj) {
//         return NOERROR;
            return;
//     }
        }

//     CEditActivity* mHost = (CEditActivity*)strongObj.Get();
        var mHost = strongObj;

//     Int32 what;
        var what;
//     msg->GetWhat(&what);
        //TODO: what = msg.GetWhat();
        what = msg.what;
//     AutoPtr<IInterface> obj;
        var obj;
//     msg->GetObj((IInterface**)&obj);
        //TODO: obj = msg.GetObj();
        obj = msg.obj;

//     switch(what) {
        switch(what) {
//         case CEditActivity::MSG_IMAGE_LOADED: {
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
                //TODO: args.Recycle();
                args.Recycle();
//             break;
                break;
//         }
            default:
                break;
//     }
        }

//     return NOERROR;
        return;
// }
    }

// CEditActivity::MyListener::MyListener(
    MyListener = function(
//     /* [in] */ CEditActivity* host)
        host)
//     : mHost(host)
// {}
    {
        this.mHost = host;
    }

// PInterface CEditActivity::MyListener::Probe(
//     /* [in]  */ REIID riid)
// {
//     if (riid == EIID_IInterface) {
//         return (PInterface)(IViewOnClickListener*)this;
//     }
//     else if (riid == EIID_IViewOnClickListener) {
//         return (IViewOnClickListener*)this;
//     }

//     return NULL;
// }

// UInt32 CEditActivity::MyListener::AddRef()
// {
//     return ElRefBase::AddRef();
// }

// UInt32 CEditActivity::MyListener::Release()
// {
//     return ElRefBase::Release();
// }

// ECode CEditActivity::MyListener::GetInterfaceID(
//     /* [in] */ IInterface *pObject,
//     /* [out] */ InterfaceID *pIID)
// {
//     if (pIID == NULL) {
//         return E_INVALID_ARGUMENT;
//     }

//     if (pObject == (IInterface*)(IViewOnClickListener*)this) {
//         *pIID = EIID_IViewOnClickListener;
//     }
//     else {
//         return E_INVALID_ARGUMENT;
//     }
//     return NOERROR;
// }

// ECode CEditActivity::MyListener::OnClick(
//     /* [in] */ IView* v)
// {
    MyListener.prototype.OnClick = function(v) {
//     Int32 viewId = 0;
        var viewId = 0;
//     v->GetId(&viewId);
        viewId = v.GetId();

//     switch(viewId) {
        switch(viewId) {
//         case R::id::edit_back: {
            case R.id.edit_back:
//             Logger::D(TAG, "OnClick()---edit_back");
                elog(TAG + "OnClick()---edit_back");
//             mHost->OnBackPressed();
                oActivity.OnBackPressed();
//             break;
                break;
//         }
//         default:
            defult:
//             break;
                break;
//     }
        }

//     return NOERROR;
        return;
// }
    }

// CEditActivity::MyLoadImageCallback::MyLoadImageCallback(
    MyLoadImageCallback = function(
//     /* [in] */ IWeakReference* host)
        host)
//     : mWeakHost(host)
// {}
    {
        this.mWeakHost = host;
    }

// ECode CEditActivity::MyLoadImageCallback::ImageLoaded(
//     /* [in] */ IDrawable* imageDrawable,
//     /* [in] */ IImageView* imageView)
    MyLoadImageCallback.prototype.ImageLoaded = function(imageDrawable, imageView)
// {
    {
//     Logger::D(TAG, "ImageLoaded()-----");
        elog(TAG + "ImageLoaded()-----");
//     AutoPtr<IActivity> strongObj;
        var strongObj;
//     mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
        strongObj = this.mWeakHost.Resolve(EIID_IActivity);
//     if (strongObj == NULL) {
        if (!strongObj) {
//         return NOERROR;
            return;
//     }
        }

//     CEditActivity* mHost = (CEditActivity*)strongObj.Get();
        var mHost = strongObj;
//     if (mHost->mMyHandler == NULL) {
        if (mMyHandler) {
//         return NOERROR;
            return;
//     }
        }

//     AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        var args = {};
//     args->mArg1 = imageDrawable;
        args.mArg1 = imageDrawable;
//     args->mArg2 = imageView;
        args.mArg2 = imageView;

//     AutoPtr<IMessage> msg;
        var msg;
//     mHost->mMyHandler->ObtainMessageEx(CEditActivity::MSG_IMAGE_LOADED, args, (IMessage**)&msg);
        //TODO: msg = mMyHandler.ObtainMessage(MSG_IMAGE_LOADED, args);
        msg = {obj:args,what:MSG_IMAGE_LOADED};
//     Boolean result;
        var result;
//     return mHost->mMyHandler->SendMessage(msg, &result);
        //TODO: result = mMyHandler.SendMessage(msg);
        result = mMyHandler.HandleMessage(msg);
// }
    }

// CEditActivity::CEditActivity()
// {
// }
    //CxxxActivity don't need to be defined, just global default in current context.

// ECode CEditActivity::OnCreate(
//     /* [in] */ IBundle* savedInstanceState)
    _apt.OnCreate = function(_this, savedInstanceState)
// {
    {
//     Logger::D(TAG, "OnCreate()---");
        elog(TAG + "OnCreate()---");
//     Activity::OnCreate(savedInstanceState);
        //super.OnCreate(savedInstanceState);
//     SetContentView(R::layout::activity_edit);
        _this.SetContentView(R.layout.activity_edit);

//     AutoPtr<IWeakReference> weakHost;
        var weakHost;
//     GetWeakReference((IWeakReference**)&weakHost);
        weakHost = _this.GetWeakReference();
//     mMyHandler = new MyHandler(weakHost);
        mMyHandler = new MyHandler(weakHost);

//     AutoPtr<MyListener> l = new MyListener(this);
        var l = new MyListener(_this);

//     AutoPtr<IView> view = FindViewById(R::id::edit_back);
        var view = _this.FindViewById(R.id.edit_back);
//     mBackButton = IImageView::Probe(view);
        mBackButton = view;
//     assert(mBackButton != NULL);
        //TODO:asset
//     view = FindViewById(R::id::edit_photo);
        view = _this.FindViewById(R.id.edit_photo);
//     mEditView = IImageView::Probe(view);
        mEditView = view;
//     assert(mEditView != NULL);
//     mBackButton->SetOnClickListener(l.Get());
        mBackButton.SetOnClickListener(l);

//     AutoPtr<IIntent> intent;
        var intent;
//     GetIntent((IIntent**)&intent);
        intent = _this.GetIntent();
//     if (intent != NULL) {
        if (intent) {
//         intent->GetStringExtra(DataSourceHelper::SOURCE_PATH, &mFilePath);
            mFilePath = intent.GetStringExtra(DataSourceHelper.SOURCE_PATH);
//         if (!mFilePath.IsNullOrEmpty()) {
            if (typeof mFilePath == "string") {
//             Logger::D(TAG, "OnCreate()---mFilePath:", mFilePath.string());
                elog(TAG + "OnCreate()---mFilePath:" + mFilePath);
//             AutoPtr<MyLoadImageCallback> myLoadImage = new MyLoadImageCallback(weakHost);
                var myLoadImage = new MyLoadImageCallback(weakHost);
//             AutoPtr<IBitmapDrawable> drawable = AsyncImageLoader::LoadDrawable(mFilePath, TRUE, mEditView, myLoadImage);
                var drawable = AsyncImageLoader.LoadDrawable(mFilePath, true, mEditView, myLoadImage);
//             if (drawable != NULL) {
                if (drawable) {
//                 mEditView->SetImageDrawable(IDrawable::Probe(drawable));
                    mEditView.SetImageDrawable(drawable);
//             }
                }
//             else {
                else {
//                 mEditView->SetImageResource(R::color::main_layout_bg);
                    mEditView.SetImageResource(R.color.main_layout_bg);
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

// ECode CEditActivity::OnStart()
    _apt.OnStart = function(_this)
// {
    {
//     Logger::D(TAG, "OnStart()---");
        elog(TAG + "OnStart()---");
//     Activity::OnStart();
        //super.Onstart();
//     return NOERROR;
        return;
// }
    }

// ECode CEditActivity::OnResume()
    _apt.OnResume = function(_this)
// {
    {
//     Logger::D(TAG, "OnResume()---");
        elog(TAG + "OnResume---");
//     Activity::OnResume();
        //super.OnResume();
//     return NOERROR;
        return;
// }
    }

// ECode CEditActivity::OnPause()
    _apt.OnPause = function(_this)
// {
    {
//     Logger::D(TAG, "OnPause()---");
        elog(TAG + "OnPause---");
//     Activity::OnPause();
        //super.OnPause();
//     return NOERROR;
        return;
// }
    }

// ECode CEditActivity::OnStop()
    _apt.OnStop = function(_this)
// {
    {
//     Logger::D(TAG, "OnStop()---");
        elog(TAG + "OnStop---");
//     Activity::OnStop();
        //super.OnStop();
//     return NOERROR;
        return;
// }
    }

// ECode CEditActivity::OnDestroy()
    _apt.OnDestroy = function(_this)
// {
    {
//     Logger::D(TAG, "OnDestroy()---");
        elog(TAG + "OnDestroy---");
//     Activity::OnDestroy();
        //super.OnDestroy();
//     return NOERROR;
        return;
// }
    }

// void CEditActivity::MyImageLoaded(
//     /* [in] */ IDrawable* imageDrawable,
//     /* [in] */ IImageView* imageView)
    MyImageLoaded = function(imageDrawable, imageView)
// {
    {
//     Logger::D(TAG, "MyImageLoaded()-----");
        elog(TAG + "MyImageLoaded()-----");
//     imageView->SetImageDrawable(imageDrawable);
        imageView.SetImageDrawable(imageDrawable);
// }
    }

// } // namespace Gallery

//--------CEditActivity.cpp--------end--------

//--------CActivityListener--------default begin--------

    _apt.OnActivityResult = function(aoContext, aiRequestCode, aiResultCode, aoIntentData) {
            elog('====jso_activity_cb====OnActivityResult.begin====');
    }
    _apt.OnCreateDialog = function(aoContext, aiId, out_aoDialog) {
            elog('====jso_activity_cb====OnCreateDialog.begin====');
    }   //OnCreateDialog
    _apt.OnCreateContextMenu = function(aoContext, aoMenu, aoV, aoMenuInfo) {
            elog('====jso_activity_cb====OnCreateContextMenu.begin====');
    }
    _apt.CreateNavigationBar = function(aoContext) {
            elog('====jso_activity_cb====CreateNavigationBar.begin====');
    }
    _apt.OnExchangeData = function(aiTimes ,aoInObject, aoOutObject) {
            elog('====jso_activity_cb====OnExchangeData.begin====');
    }
    _apt.OnHandleMessage = function(aoContext, aoMessage) {
        elog('====jso_activity_cb====OnHandleMessage.begin====');
    }

//--------CActivityListener--------default end--------

    return new CActivityListener(oActivity);
};  //module.exports