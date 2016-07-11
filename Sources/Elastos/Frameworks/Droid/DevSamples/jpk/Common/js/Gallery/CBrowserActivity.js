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

// #ifndef __CBROWSERACTIVITY_H__
// #define __CBROWSERACTIVITY_H__

// #include "_CBrowserActivity.h"
// #include "app/Activity.h"
// #include "src/data/AsyncImageLoader.h"
    var AsyncImageLoader = require("./data/AsyncImageLoader.js")(aoElastos);
// #include "os/HandlerBase.h"

// using Elastos::Droid::Os::HandlerBase;
// using Elastos::Droid::App::Activity;
// using Elastos::Droid::View::IViewOnClickListener;
// using Elastos::Droid::View::IViewOnTouchListener;
// using Elastos::Droid::View::IMotionEvent;
// using Elastos::Droid::Widget::ICompoundButton;
// using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
// using Elastos::Droid::Widget::IPopupWindow;
// using Elastos::Droid::Widget::IPopupWindowOnDismissListener;
// using Elastos::Droid::Widget::IButton;
// using Elastos::Droid::Widget::IGridView;
// using Elastos::Droid::Widget::IImageView;
// using Elastos::Droid::Widget::IImageButton;
// using Elastos::Droid::Widget::ITextView;
// using Elastos::Droid::Widget::ISimpleAdapter;
// using Elastos::Droid::Widget::ISimpleAdapterViewBinder;
// using Elastos::Droid::Widget::ILinearLayout;

// namespace Gallery {

// class CBrowserActivity
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
    function MyHandler() {};

//     class MyListener
//         : public ElRefBase
//         , public IViewOnClickListener
//         , public IPopupWindowOnDismissListener
//         , public IViewOnTouchListener
//         , public ICompoundButtonOnCheckedChangeListener
//     {
//     public:
//         MyListener(
//             /* [in] */ CBrowserActivity* host);

//         CAR_INTERFACE_DECL();

//         CARAPI OnClick(
//             /* [in] */ IView* v);

//         CARAPI OnDismiss();

//         CARAPI OnTouch(
//             /* [in] */ IView* v,
//             /* [in] */ IMotionEvent* event,
//             /* [out] */ Boolean* result);

//         CARAPI OnCheckedChanged(
//             /* [in] */ ICompoundButton* buttonView,
//             /* [in] */ Boolean isChecked);

//     private:
//         CBrowserActivity* mHost;
//     };
    function MyListener() {};

//     class MyViewBinder
//         : public ElRefBase
//         , public ISimpleAdapterViewBinder
//     {
//     public:
//         MyViewBinder(
//             /* [in] */ CBrowserActivity* host);

//         CAR_INTERFACE_DECL();

//         CARAPI SetViewValue(
//             /* [in] */ IView* view,
//             /* [in] */ IInterface* data,
//             /* [in] */ const String& textRepresentation,
//             /* [out] */ Boolean* result);

//     private:
//             CBrowserActivity* mHost;
//     };
    function MyViewBinder() {};

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

//     class PictureEntry
//         : public ElRefBase
//     {
//     public:
//         String sourcePath;
//         String desc;
//     };
    function MyLoadImageCallback() {};

// friend class MyListener;
// friend class MyViewBinder;
// friend class MyLoadImageCallback;

// public:
//     CBrowserActivity();

// protected:
//     CARAPI OnCreate(
//         /* [in] */ IBundle* savedInstanceState);

//     CARAPI OnStart();

//     CARAPI OnResume();

//     CARAPI OnPause();

//     CARAPI OnStop();

//     CARAPI OnDestroy();

//     CARAPI OnBackPressed();

// private:
//     CARAPI OpenPicPopupWindow(
//         /* [in] */ IView* v);

//     CARAPI ClosePicPopupWindow();

//     CARAPI_(AutoPtr<ISimpleAdapter>) GetSimpleAdapter();

//     CARAPI_(void) MyImageLoaded(
//         /* [in] */ IDrawable* imageDrawable,
//         /* [in] */ IImageView* imageView);

// private:
//     static const String TAG;
    var TAG;
//     static const Int32 MSG_IMAGE_LOADED;
    var MSG_IMAGE_LOADED;

// private:
//     Boolean mShowPicPop;
    var mShowPicPop;

//     AutoPtr<MyListener> mMyListener;
    var mMyListener;
//     AutoPtr<IImageView> mBackButton;
    var mBackButton;
//     AutoPtr<IGridView> mGridView;
    var mGridView;
//     AutoPtr<ILinearLayout> mWallpaperLayout;
    var mWallpaperLayout;
//     // AutoPtr<IButton> mDetailButton;
    var mDetailButton;
//     AutoPtr<ILinearLayout> mDetailLayout;
    var mDetailLayout;
//     AutoPtr<ILinearLayout> mShareLayout;
    var mShareLayout;
//     AutoPtr<IButton> mPopButton;
    var mPopButton;
//     AutoPtr<IPopupWindow> mPicPopupWindow;
    var mPicPopupWindow;
//     AutoPtr<ISimpleAdapter> mSimpleAdapter;
    var mSimpleAdapter;
//     List< AutoPtr<PictureEntry> > mPictureEntryList;
    var mPictureEntryList = [];
//     AutoPtr<IHandler> mMyHandler;
    var mMyHandler;
// };

// } // namespace Gallery

// #endif // __CBROWSERACTIVITY_H__

//--------CxxxActivity.h----end----

//--------CxxxActivity.cpp----begin----

// #include "CBrowserActivity.h"
// #include "src/data/DataSourceHelper.h"
    var DataSourceHelper = require("./data/DataSourceHelper.js")(aoElastos);
// #include "src/data/AsyncImageLoader.h"
// #include "R.h"
    //var R = require("./R.js");
// #include <elastos/Logger.h>
// #include <elastos/StringUtils.h>
// #include "os/SomeArgs.h"

// using Elastos::Core::CBoolean;
// using Elastos::Core::IBoolean;
// using Elastos::Core::EIID_IBoolean;
// using Elastos::Core::CInteger32;
// using Elastos::Core::IInteger32;
// using Elastos::Core::EIID_IInteger32;
// using Elastos::Core::EIID_ICharSequence;
// using Elastos::Core::CStringWrapper;
// using Elastos::Core::StringUtils;
// using Elastos::IO::CFile;
// using Elastos::IO::IFile;

// using Elastos::Droid::Os::SomeArgs;
// using Elastos::Droid::Content::CIntent;
// using Elastos::Droid::Content::Res::IResources;
// using Elastos::Droid::Graphics::IColor;
// using Elastos::Droid::Graphics::IRect;
// using Elastos::Droid::Graphics::Drawable::IDrawable;
// using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
// using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
// using Elastos::Droid::Graphics::Drawable::EIID_IDrawable;
// using Elastos::Droid::View::EIID_IViewOnClickListener;
// using Elastos::Droid::View::EIID_IViewOnTouchListener;
// using Elastos::Droid::View::IViewParent;
// using Elastos::Droid::Widget::IAdapterView;
// using Elastos::Droid::Widget::EIID_IPopupWindowOnDismissListener;
// using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
// using Elastos::Droid::Widget::ICheckBox;
// using Elastos::Droid::Widget::ICheckable;
// using Elastos::Droid::Widget::CPopupWindow;
// using Elastos::Droid::Widget::IAdapter;
// using Elastos::Droid::Widget::CSimpleAdapter;
// using Elastos::Droid::Widget::EIID_ISimpleAdapterViewBinder;
// using Elastos::Droid::Widget::IRelativeLayout;
// using Elastos::Droid::App::EIID_IActivity;
// using Elastos::Droid::Utility::CParcelableObjectContainer;

// using Elastos::Utility::CObjectStringMap;
// using Elastos::Utility::IObjectStringMap;
// using Elastos::Utility::Logging::Logger;

// namespace Gallery {

// const String CBrowserActivity::TAG("CBrowserActivity");
    TAG = "CBrowserActivity: ";
// const Int32 CBrowserActivity::MSG_IMAGE_LOADED = 0;
    MSG_IMAGE_LOADED = 0;

// ECode CBrowserActivity::MyHandler::HandleMessage(
    MyHandler.prototype.HandleMessage = function(
//     /* [in] */ IMessage* msg)
        msg)
// {
    {
//     AutoPtr<IActivity> strongObj;
//     mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
//     if (strongObj == NULL) {
//         return NOERROR;
//     }

//     CBrowserActivity* mHost = (CBrowserActivity*)strongObj.Get();

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
//         case CBrowserActivity::MSG_IMAGE_LOADED: {
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

// CBrowserActivity::MyListener::MyListener(
//     /* [in] */ CBrowserActivity* host)
//     : mHost(host)
// {}

// PInterface CBrowserActivity::MyListener::Probe(
//     /* [in]  */ REIID riid)
// {
//     if (riid == EIID_IInterface) {
//         return (PInterface)(IViewOnClickListener*)this;
//     }
//     else if (riid == EIID_IViewOnClickListener) {
//         return (IViewOnClickListener*)this;
//     }
//     else if (riid == EIID_IPopupWindowOnDismissListener) {
//         return (IPopupWindowOnDismissListener*)this;
//     }
//     else if (riid == EIID_IViewOnTouchListener) {
//         return (IViewOnTouchListener*)this;
//     }
//     else if (riid == EIID_ICompoundButtonOnCheckedChangeListener) {
//         return (ICompoundButtonOnCheckedChangeListener*)this;
//     }

//     return NULL;
// }

// UInt32 CBrowserActivity::MyListener::AddRef()
// {
//     return ElRefBase::AddRef();
// }

// UInt32 CBrowserActivity::MyListener::Release()
// {
//     return ElRefBase::Release();
// }

// ECode CBrowserActivity::MyListener::GetInterfaceID(
//     /* [in] */ IInterface *pObject,
//     /* [out] */ InterfaceID *pIID)
// {
//     if (pIID == NULL) {
//         return E_INVALID_ARGUMENT;
//     }

//     if (pObject == (IInterface*)(IViewOnClickListener*)this) {
//         *pIID = EIID_IViewOnClickListener;
//     }
//     else if (pObject == (IInterface*)(IPopupWindowOnDismissListener*)this) {
//         *pIID = EIID_IPopupWindowOnDismissListener;
//     }
//     else if (pObject == (IInterface*)(IViewOnTouchListener*)this) {
//         *pIID = EIID_IViewOnTouchListener;
//     }
//     else if (pObject == (IInterface*)(ICompoundButtonOnCheckedChangeListener*)this) {
//         *pIID = EIID_ICompoundButtonOnCheckedChangeListener;
//     }
//     else {
//         return E_INVALID_ARGUMENT;
//     }
//     return NOERROR;
// }

// ECode CBrowserActivity::MyListener::OnClick(
    MyListener.prototype.OnClick = function(
//     /* [in] */ IView* v)
        v)
// {
    {
//     Int32 viewId = 0;
        var viewId = 0;
//     v->GetId(&viewId);
        viewId = v.GetId();

//     switch(viewId) {
        switch(viewId) {
//         case R::id::pic_back: {
            case R.id.pic_back:
//             Logger::D(TAG, "OnClick()---pic_back");
                elog(TAG + "OnClick()---pic_back");
//             mHost->OnBackPressed();
                oActivity.OnBackPressed();
//             break;
                break;
//         }
//         case R::id::pic_btn_wallpaper: {
            case R.id.pic_btn_wallpaper:
//             Logger::D(TAG, "OnClick()---pic_btn_wallpaper");
                elog(TAG + "OnClick()---pic_btn_wallpaper");
//             break;
                break;
//         }
//         case R::id::pic_btn_detail: {
            case R.id.pic_btn_detail:
//             Logger::D(TAG, "OnClick()---pic_btn_detail");
                elog(TAG + "OnClick()---pic_btn_detail");
//             break;
                break;
//         }
//         case R::id::pic_btn_share: {
            case R.id.pic_btn_share:
//             Logger::D(TAG, "OnClick()---pic_btn_share");
                elog(TAG + "OnClick()---pic_btn_share");
//             break;
                break;
//         }
//         case R::id::pic_btn_popup: {
            case R.id.pic_btn_popup:
//             Logger::D(TAG, "OnClick()---pic_btn_popup");
                elog(TAG + "OnClick()---pic_btn_popup");
//             if (mHost->mShowPicPop) {
                if (mShowPicPop) {
//                 mHost->ClosePicPopupWindow();
                    ClosePicPopupWindow();
//             }
                }
//             else {
                else {
//                 mHost->OpenPicPopupWindow(v);
                    OpenPicPopupWindow(v);
//             }
                }
//             break;
                break;
//         }
//         case R::id::pic_pop_time: {
            case R.id.pic_pop_time:
//             Logger::D(TAG, "OnClick()---pic_pop_time");
                elog(TAG + "OnClick()---pic_pop_time");
//             mHost->ClosePicPopupWindow();
                ClosePicPopupWindow();
//             break;
                break;
//         }
//         case R::id::pic_pop_location: {
            case R.id.pic_pop_location:
//             Logger::D(TAG, "OnClick()---pic_pop_location");
                elog(TAG + "OnClick()---pic_pop_location");
//             mHost->ClosePicPopupWindow();
                ClosePicPopupWindow();
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

// ECode CBrowserActivity::MyListener::OnDismiss()
    MyListener.prototype.OnDismiss = function()
// {
    {
//     Logger::D(TAG, "OnDismiss()---");
        elog(TAG + "OnDismiss()---");
//     mHost->mShowPicPop = FALSE;
        mShowPicPop = false;
//     return NOERROR;
        return;
// }
    }

// ECode CBrowserActivity::MyListener::OnTouch(
    MyListener.prototype.OnTouch = function(
//     /* [in] */ IView* v,
        v,
//     /* [in] */ IMotionEvent* event,
        event,
//     /* [out] */ Boolean* result)
        result)
// {
    {
//     VALIDATE_NOT_NULL(result)
//     Int32 action = 0;
        var action = 0;
//     AutoPtr<IView> view;
        var view;
//     event->GetAction(&action);
        action = event.GetAction();
//     v->FindViewById(R::id::pic_thumb_overlay, (IView**)&view);
        view = v.FindViewById(R.id.pic_thumb_overlay);
//     AutoPtr<IImageView> overlayView = IImageView::Probe(view);
        var overlayView = view;

//     if (IMotionEvent::ACTION_DOWN == action) {
        if (IMotionEvent__ACTION_DOWN == action) {
//         Logger::D(TAG, "onTouch()--ACTION_DOWN");
            elog(TAG + "onTouch()--ACTION_DOWN");
//         overlayView->SetVisibility(IView::VISIBLE);
            overlayView.SetVisibility(IView__VISIBLE);
//     }
        }
//     else {
        else {
//         Logger::D(TAG, "onTouch()--ACTION:%d", action);
            elog(TAG + "OnTouch()--ACTION:" + action);
//         overlayView->SetVisibility(IView::GONE);
            overlayView.SetVisibility(IView__GONE);
//         AutoPtr<IViewParent> viewParent;
            var viewParent;
//         AutoPtr<IViewParent> viewParent2;
            var viewParent2;
//         v->GetParent((IViewParent**)&viewParent);
            viewParent = v.GetParent();
//         viewParent->GetParentEx((IViewParent**)&viewParent2);
            viewParent2 = viewParent.GetParent();
//         AutoPtr<IAdapterView> adapterView = IAdapterView::Probe(viewParent2);
            var adapterView = viewParent2;
//         Int32 index = 0;
            var index = 0;
//         adapterView->GetPositionForView(v, &index);
            index = adapterView.GetPositionForView(v);
//         Logger::D(TAG, "onTouch()--index:%d", index);
            elog(TAG + "OnTouch()--index:" + index);
//         if (IMotionEvent::ACTION_UP == action) {
            if (IMotionEvent__ACTION_UP == action) {
//             AutoPtr<PictureEntry> entry = mHost->mPictureEntryList[index];
                var entry = mPictureEntryList[index];
//             AutoPtr<IIntent> intent;
                var intent;
//             CIntent::New((IIntent**)&intent);
                intent = Droid_New('Elastos.Droid.Content.CIntent');
//             intent->SetClassNameEx(String("Gallery"), String("Gallery.CPhotoActivity"));
                intent.SetClassName("Elastos.DevSamples.Node.JSGallery", "Elastos.DevSamples.Node.JSGallery.CPhotoActivity");
//             intent->PutStringExtra(DataSourceHelper::SOURCE_PATH, entry->sourcePath);
                intent.PutExtra(DataSourceHelper.SOURCE_PATH, entry.sourcePath);
//             intent->PutInt32Extra(DataSourceHelper::SOURCE_INDEX, index);
                intent.PutExtra(DataSourceHelper.SOURCE_INDEX, index);
//             if (FAILED(mHost->StartActivity(intent))) {
                try {
                    oActivity.StartActivity(intent);
                } catch(e) {
//                 Logger::E(TAG, "onTouch()--StartActivity CPhotoActivity failed!");
                    elog(TAG + "OnTouch()--StartActivity CPhotoActivity failed!");
//             }
                }
//         }
            }
//     }
        }
//     *result = TRUE;
        result.data = true;
//     return NOERROR;
        return;
// }
    }

// ECode CBrowserActivity::MyListener::OnCheckedChanged(
    MyListener.prototype.OnCheckedChanged = function(
//     /* [in] */ ICompoundButton* buttonView,
        buttonView,
//     /* [in] */ Boolean isChecked)
        isChecked)
// {
    {
//     Logger::D(TAG, "OnCheckedChanged()--isChecked:%d", isChecked);
        elog(TAG + "OnCheckedChanged()--isChecked:" + isChecked);
//     return NOERROR;
        return;
// }
    }

// CBrowserActivity::MyViewBinder::MyViewBinder(
//     /* [in] */ CBrowserActivity* host)
//     : mHost(host)
// {}

// CAR_INTERFACE_IMPL(CBrowserActivity::MyViewBinder, ISimpleAdapterViewBinder)

// ECode CBrowserActivity::MyViewBinder::SetViewValue(
    MyViewBinder.prototype.SetViewValue = function(
//     /* [in] */ IView* view,
        view,
//     /* [in] */ IInterface* data,
        data,
//     /* [in] */ const String& textRepresentation,
        textRepresentation,
//     /* [out] */ Boolean* result)
        result)
// {
    {
//     // Logger::D(TAG, "MyViewBinder()---SetViewValue---");
        elog(TAG + "MyViewBinder---SetViewValue---");
//     VALIDATE_NOT_NULL(result)
//     *result = FALSE;
        result.data = false;

//     if (IRelativeLayout::Probe(view) != NULL) {
        if (CObject.hasInterface(view, "IRelativeLayout")){
//         // Logger::D(TAG, "MyViewBinder()---SetViewValue---SetOnTouchListener");
            elog(TAG + "MyViewBinder---SetViewValue---SetOnTouchListener");
//         AutoPtr<IRelativeLayout> thumbLayout = IRelativeLayout::Probe(view);
            var thumbLayout = view;
//         thumbLayout->SetOnTouchListener((IViewOnTouchListener*)(mHost->mMyListener->Probe(EIID_IViewOnTouchListener)));
            thumbLayout.SetOnTouchListener(mMyListener);
//         *result = TRUE;
            result.data = true;
//     }
        }
//     else if (IImageView::Probe(view) != NULL) {
        else if (CObject.hasInterface(view, "IImageView")) {
//         // Logger::D(TAG, "MyViewBinder()---SetViewValue---text:%s", textRepresentation.string());
            elog(TAG + "MyViewBinder()---SetViewValue---text::" + textRepresentation);
//         AutoPtr<IImageView> imageView = IImageView::Probe(view);
            var imageView = view;
//         AutoPtr<IWeakReference> weakHost;
//         mHost->GetWeakReference((IWeakReference**)&weakHost);
//         assert(weakHost != NULL);
//         AutoPtr<MyLoadImageCallback> myLoadImage = new MyLoadImageCallback(weakHost);
            var myLoadImage = new MyLoadImageCallback();
//         AutoPtr<IBitmapDrawable> drawable = AsyncImageLoader::LoadDrawable(textRepresentation, FALSE, imageView, myLoadImage);
            var drawable = AsyncImageLoader.LoadDrawable(textRepresentation, false, imageView, myLoadImage);
//         if (drawable != NULL) {
            if (drawable) {
//             imageView->SetImageDrawable(IDrawable::Probe(drawable));
                imageView.SetImageDrawable(drawable);
//         }
            }
//         else {
            else {
//             imageView->SetImageResource(R::color::divide_line_bg);
                imageView.SetImageResource(R.color.divide_line_bg);
//         }
            }
//         *result = TRUE;
            result.data = true;
//     }
        }
// //    else if (ICheckable::Probe(view) != NULL) {
// //        Logger::D(TAG, "MyViewBinder()---SetViewValue---SetOnCheckedChangeListener");
// //        AutoPtr<ICheckBox> checkBox = ICheckBox::Probe(view);
// //        checkBox->SetOnCheckedChangeListener((ICompoundButtonOnCheckedChangeListener*)
// //                        (mHost->mMyListener->Probe(EIID_ICompoundButtonOnCheckedChangeListener)));
// //        *result = FALSE;
// //    }
        else {
            elog("SetViewValue: not defined!");
        }
//     return NOERROR;
        return;
// }
    }

// CBrowserActivity::MyLoadImageCallback::MyLoadImageCallback(
//     /* [in] */ IWeakReference* host)
//     : mWeakHost(host)
// {}

// ECode CBrowserActivity::MyLoadImageCallback::ImageLoaded(
    MyLoadImageCallback.prototype.ImageLoaded = function(
//     /* [in] */ IDrawable* imageDrawable,
        imageDrawable,
//     /* [in] */ IImageView* imageView)
        imageView)
// {
    {
//     Logger::D(TAG, "ImageLoaded()-----");
        elog(TAG + "ImageLoaded()-----");
//     AutoPtr<IActivity> strongObj;
//     mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
//     if (strongObj == NULL) {
//         return NOERROR;
//     }

//     CBrowserActivity* mHost = (CBrowserActivity*)strongObj.Get();
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
//     mHost->mMyHandler->ObtainMessageEx(CBrowserActivity::MSG_IMAGE_LOADED, args, (IMessage**)&msg);
        msg = {obj:args, what:MSG_IMAGE_LOADED};
//     Boolean result;
        var result;
//     return mHost->mMyHandler->SendMessage(msg, &result);
        result = mMyHandler.HandleMessage(msg);
// }
    }

// CBrowserActivity::CBrowserActivity()
// {
// }

// ECode CBrowserActivity::OnCreate(
//     /* [in] */ IBundle* savedInstanceState)
    _apt.OnCreate = function(context, savedInstanceState)
// {
    {
        var _this = context;

//     Logger::D(TAG, "OnCreate()---");
        elog(TAG + "OnCreate()---");

//     Activity::OnCreate(savedInstanceState);
//     SetContentView(R::layout::activity_browser);
        _this.SetContentView(R.layout.activity_browser);

//     AutoPtr<IWeakReference> weakHost;
//     GetWeakReference((IWeakReference**)&weakHost);
//     mMyHandler = new MyHandler(weakHost);

//     mMyListener = new MyListener(this);
        mMyListener = new MyListener();

//     AutoPtr<IView> view = FindViewById(R::id::pic_back);
//     mBackButton = IImageView::Probe(view);
//     assert(mBackButton != NULL);
        mBackButton = _this.FindViewById(R.id.pic_back);

//     view = FindViewById(R::id::pic_folder_name);
//     AutoPtr<ITextView> albumName = ITextView::Probe(view);
//     assert(albumName != NULL);
        var albumName = _this.FindViewById(R.id.pic_folder_name);

//     view = FindViewById(R::id::pic_btn_wallpaper);
//     mWallpaperLayout = ILinearLayout::Probe(view);
//     assert(mWallpaperLayout != NULL);
        mWallpaperLayout = _this.FindViewById(R.id.pic_btn_wallpaper);

//     view = FindViewById(R::id::pic_btn_detail);
//     mDetailLayout = ILinearLayout::Probe(view);
//     assert(mDetailLayout != NULL);
        mDetailLayout = _this.FindViewById(R.id.pic_btn_detail);

//     view = FindViewById(R::id::pic_btn_share);
//     mShareLayout = ILinearLayout::Probe(view);
//     assert(mShareLayout != NULL);
        mShareLayout = _this.FindViewById(R.id.pic_btn_share);

//     view = FindViewById(R::id::pic_btn_popup);
//     mPopButton = IButton::Probe(view);
//     assert(mPopButton != NULL);
        mPopButton = _this.FindViewById(R.id.pic_btn_popup);

//     view = FindViewById(R::id::pic_gridview);
//     mGridView = IGridView::Probe(view);
//     assert(mGridView != NULL);
        mGridView = _this.FindViewById(R.id.pic_gridview);

//     AutoPtr<IIntent> intent;
//     GetIntent((IIntent**)&intent);
        var intent = _this.GetIntent();
//     if (intent != NULL) {
        //if (intent) {
        if (true) {
//         String path;
            var path;
//         intent->GetStringExtra(DataSourceHelper::SOURCE_PATH, &path);
            path = intent.GetStringExtra(DataSourceHelper.SOURCE_PATH);
            path = "/data/temp/testdisk";
//         if (!path.IsNullOrEmpty()) {
            if (typeof path == "string") {
//             String folderName;
                var folderName;
//             intent->GetStringExtra(DataSourceHelper::SOURCE_DESC, &folderName);
                //folderName = intent.GetStringExtra(DataSourceHelper.SOURCE_DESC)
                folderName = "testdisk";
//             AutoPtr<List<String> > imageItems = DataSourceHelper::GetItemList(path);
                var imageItems = DataSourceHelper.GetItemList(path);
//             if (imageItems != NULL) {
                if (imageItems instanceof Array) {
//                 AutoPtr<PictureEntry> entry;
                    var entry;
//                 List<String>::Iterator it = imageItems->Begin();
//                 for (Int32 i = 0; it != imageItems->End(); ++it, ++i) {
                    for (var i=0,im=imageItems.length;i<im;i++) {
//                     entry = new PictureEntry();
                        entry = {};
//                     entry->sourcePath = path;
                        entry.sourcePath = path;
//                     entry->sourcePath += DataSourceHelper::PATH_SPLITE;
                        entry.sourcePath += DataSourceHelper.PATH_SPLITE;
//                     entry->sourcePath += *it;
                        entry.sourcePath += imageItems[i];
//                     entry->desc = folderName;
                        entry.desc = folderName;
//                     // Logger::D(TAG, " > %d, path:%s, folderName:%s", i, entry->sourcePath.string(), entry->desc.string());
                        elog(TAG + " > " + i + ", path:" + entry.sourcePath + ", folderName:" + entry.desc);
//                     mPictureEntryList.PushBack(entry);
                        mPictureEntryList.push(entry);
//                 }
                    }
//                 AutoPtr<ICharSequence> cs;
                    var cs;
//                 CStringWrapper::New(folderName, (ICharSequence**)&cs);
                    cs = CString(folderName);
//                 albumName->SetText(cs);
                    albumName.SetText(cs);
//                 mSimpleAdapter = GetSimpleAdapter();
                    mSimpleAdapter = GetSimpleAdapter();
//                 if (mSimpleAdapter == NULL) {
                    if (!mSimpleAdapter) {
//                     Logger::W(TAG, "mSimpleAdapter is null!");
                        elog(TAG + "mSimpleAdapter is null!");
//                 }
                    }
//                 mGridView->SetAdapter(IAdapter::Probe(mSimpleAdapter));
                    mGridView.SetAdapter(mSimpleAdapter);
//             }
                }
//             else {
                else {
//                 Logger::W(TAG, "imageItems is null!");
                    elog(TAG + "imageItems is null!");
//             }
                }
//         }
            }
//     }
        }

//     AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mMyListener.Get();
        var clickListener = mMyListener;
//     mBackButton->SetOnClickListener(clickListener);
        mBackButton.SetOnClickListener(clickListener);
//     mWallpaperLayout->SetOnClickListener(clickListener);
        mWallpaperLayout.SetOnClickListener(clickListener);
//     mDetailLayout->SetOnClickListener(clickListener);
        mDetailLayout.SetOnClickListener(clickListener);
//     mShareLayout->SetOnClickListener(clickListener);
        mShareLayout.SetOnClickListener(clickListener);
//     mPopButton->SetOnClickListener(clickListener);
        mPopButton.SetOnClickListener(clickListener);

//     return NOERROR;
        return;
// }
    }

// ECode CBrowserActivity::OnStart()
// {
//     Logger::D(TAG, "OnStart()---");
//     Activity::OnStart();
//     return NOERROR;
// }

// ECode CBrowserActivity::OnResume()
// {
//     Logger::D(TAG, "OnResume()---");
//     Activity::OnResume();
//     return NOERROR;
// }

// ECode CBrowserActivity::OnPause()
// {
//     Logger::D(TAG, "OnPause()---");
//     Activity::OnPause();
//     return NOERROR;
// }

// ECode CBrowserActivity::OnStop()
// {
//     Logger::D(TAG, "OnStop()---");
//     Activity::OnStop();
//     return NOERROR;
// }

// ECode CBrowserActivity::OnDestroy()
    _apt.OnDestroy = function(context)
// {
    {
//     Logger::D(TAG, "OnDestroy()---");
        elog(TAG + "OnDestroy()---");
//     Activity::OnDestroy();
//     mSimpleAdapter = NULL;
        mSimpleAdapter = null;
//     return NOERROR;
        return;
// }
    }

// ECode CBrowserActivity::OnBackPressed()
    _apt.OnBackPressed = function(context)
// {
    {
//     Logger::D(TAG, "OnBackPressed()---");
        elog(TAG + "OnBackPressed()---");

//     if (mShowPicPop) {
        if (mShowPicPop) {
//         ClosePicPopupWindow();
            ClosePicPopupWindow();
//     }
        }
//     else {
        else {
//         Activity::OnBackPressed();
            //super.OnBackPressed();    //TODO
//     }
        }

//     return NOERROR;
        return;
// }
    }

// ECode CBrowserActivity::OpenPicPopupWindow(
    function OpenPicPopupWindow(
//     /* [in] */ IView* v)
        v)
// {
    {
//     Logger::D(TAG, "OpenPicPopupWindow()-----");
        elog(TAG + "OpenPicPopupWindow()-----");

//     AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mMyListener.Get();
        var clickListener = mMyListener;

//     AutoPtr<ILayoutInflater> inflater;
        var inflater;
//     GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);
        inflater =  oActivity.GetSystemService(IContext__LAYOUT_INFLATER_SERVICE);

//     AutoPtr<IView> layout;
        var layout;
//     inflater->Inflate(R::layout::pic_popup_dialog, NULL, (IView**)&layout);
        layout = inflater.Inflate(R.layout.pic_popup_dialog);
//     AutoPtr<IView> view;
        var view;
//     layout->FindViewById(R::id::pic_pop_time, (IView**)&view);
        view = layout.FindViewById(R.id.pic_pop_time);
//     AutoPtr<ITextView> textTime = ITextView::Probe(view);
        var textTime = view;
//     assert(textTime != NULL);
//     textTime->SetOnClickListener(clickListener);
        textTime.SetOnClickListener(clickListener);

//     view = NULL;
        view = null;
//     layout->FindViewById(R::id::pic_pop_location, (IView**)&view);
        view = layout.FindViewById(R.id.pic_pop_location);
//     AutoPtr<ITextView> textLocation = ITextView::Probe(view);
        var textLocation = view;
//     assert(textLocation != NULL);
//     textLocation->SetOnClickListener(clickListener);
        textLocation.SetOnClickListener(clickListener);

//     mPicPopupWindow = NULL;
        mPicPopupWindow = null;
//     CPopupWindow::New(layout, 80, 62, TRUE, (IPopupWindow**)&mPicPopupWindow);
        mPicPopupWindow = Droid_New("Elastos.Droid.Widget.CPopupWindow", layout, 80, 62, true);
//     mPicPopupWindow->SetTouchable(TRUE);
        mPicPopupWindow.SetTouchable(true);
//     mPicPopupWindow->SetOutsideTouchable(TRUE);
        mPicPopupWindow.SetOutsideTouchable(true);

//     AutoPtr<IResources> res;
        var res;
//     GetResources((IResources**)&res);
        res = oActivity.GetResources();
//     AutoPtr<IBitmapDrawable> bitmapDrawable;
        var bitmapDrawable;
//     CBitmapDrawable::New(res, (IBitmap*)NULL, (IBitmapDrawable**)&bitmapDrawable);
        var bitmapDrawable = Droid_New('Elastos.Droid.Graphics.Drawable.CBitmapDrawable',res);
//     mPicPopupWindow->SetBackgroundDrawable(IDrawable::Probe(bitmapDrawable));
        mPicPopupWindow.SetBackgroundDrawable(bitmapDrawable);
//     mPicPopupWindow->SetOnDismissListener((IPopupWindowOnDismissListener*)mMyListener.Get());
        mPicPopupWindow.SetOnDismissListener(mMyListener);
//     mPicPopupWindow->ShowAsDropDown(v);
        mPicPopupWindow.ShowAsDropDown(v);
//     mShowPicPop = TRUE;
        mShowPicPop = true;

//     return NOERROR;
        return;
// }
    }

// ECode CBrowserActivity::ClosePicPopupWindow()
    function ClosePicPopupWindow()
// {
    {
//     Boolean isShow = FALSE;
        var isShow = false;
//     if (mPicPopupWindow != NULL && (mPicPopupWindow->IsShowing(&isShow), isShow)) {
        if (mPicPopupWindow && (isShow = mPicPopupWindow.IsShowing())) {
//         Logger::D(TAG, "ClosePicPopupWindow()---");
            elog(TAG + "ClosePicPopupWindow()---");
//         mPicPopupWindow->Dismiss();
            mPicPopupWindow.Dismiss();
//         mPicPopupWindow = NULL;
            mPicPopupWindow = null;
//     }
        }
//     mShowPicPop = FALSE;
        mShowPicPop = false;
//     return NOERROR;
        return;
// }
    }

// AutoPtr<ISimpleAdapter> CBrowserActivity::GetSimpleAdapter()
    function GetSimpleAdapter()
// {
    {
//     Logger::D(TAG, "GetSimpleAdapter()---");
        elog(TAG + "GetSimpleAdapter()---");
//     AutoPtr<ISimpleAdapter> simpleAdapter;
        var simpleAdapter;
//     if (!mPictureEntryList.IsEmpty()) {
        if (mPictureEntryList.length) {
//         Logger::D(TAG, "GetSimpleAdapter()---PictureEntry count: %d", mPictureEntryList.GetSize());
            elog(TAG + "GetSimpleAdapter()---PictureEntry count: " + mPictureEntryList.length);
//         AutoPtr<IObjectContainer> dataList;
            var dataList;
//         CParcelableObjectContainer::New((IObjectContainer**)&dataList);
            dataList = Core_New("Elastos.Utility.CArrayList");

//         AutoPtr<PictureEntry> entry;
            var entry;
//         String key1("image");
            var key1 = "image";
//         String key2("checkbox");
            var key2 = "checkbox";
//         String key3("thumbLayout");
            var key3 = "thumbLayout";

//         List< AutoPtr<PictureEntry> >::Iterator it = mPictureEntryList.Begin();
//         for (; it != mPictureEntryList.End(); ++it) {
            for (var i=0,im=mPictureEntryList.length;i<im;i++) {
//             entry = *it;
                entry = mPictureEntryList[i];
//             AutoPtr<IObjectStringMap> map;
                var map;
//             CObjectStringMap::New((IObjectStringMap**)&map);
                map = Core_New('Elastos.Utility.CHashMap');
if(false) {
//             AutoPtr<IFile> file;
                var file;
//             CFile::New(entry->sourcePath, (IFile**)&file);
elog(TAG + "GetSimpleAdapter()----"+i+"----1.2----");
                var file = Core_New('Elastos.IO.CFile', entry.sourcePath);
//             Int64 last = 0;
                var last = 0;
//             file->LastModified(&last);
                last = file.GetLastModified();
//             Logger::D(TAG, "GetSimpleAdapter()---sort--last:%lld,--path:%s", last, entry->sourcePath.string());
                elog(TAG + "GetSimpleAdapter()---sort--last:"+last+",--path:"+entry.sourcePath);
}
//             AutoPtr<ICharSequence> cs;
                var cs;
//             CStringWrapper::New(entry->sourcePath, (ICharSequence**)&cs);
                cs = CString(entry.sourcePath);
//             map->Put(key1, cs->Probe(EIID_ICharSequence));
                map.Put( CString(key1), cs);

//             AutoPtr<IBoolean> boolValue;
                var boolValue;
//             CBoolean::New(FALSE, (IBoolean**)&boolValue);
                boolValue = Core_New('Elastos.Core.CBoolean', false);
//             map->Put(key2, boolValue->Probe(EIID_IBoolean));
                map.Put( CString(key2), boolValue);
//             dataList->Add(map);
                dataList.Add(map);
//         }
            }

//         AutoPtr< ArrayOf<String> > from = ArrayOf<String>::Alloc(3);
            var from = [];
//         (*from)[0] = key1;
            from[0] = key1;
//         (*from)[1] = key2;
            from[1] = key2;
//         (*from)[2] = key3;
            from[2] = key3;
//         AutoPtr< ArrayOf<Int32> > to = ArrayOf<Int32>::Alloc(3);
            var to = [];
//         (*to)[0] = R::id::pic_thumb;
            to[0] = R.id.pic_thumb;
//         (*to)[1] = R::id::pic_check;
            to[1] = R.id.pic_check;
//         (*to)[2] = R::id::pic_thumb_layout;
            to[2] = R.id.pic_thumb_layout;
//         CSimpleAdapter::New(this, dataList, R::layout::pic_item, from, to, (ISimpleAdapter**)&simpleAdapter);
            simpleAdapter = Droid_New('Elastos.Droid.Widget.CSimpleAdapter', oActivity, dataList, R.layout.pic_item, from, to);
//         AutoPtr<MyViewBinder> myViewBinder = new MyViewBinder(this);
            var myViewBinder = new MyViewBinder();
//         simpleAdapter->SetViewBinder(ISimpleAdapterViewBinder::Probe(myViewBinder));
            simpleAdapter.SetViewBinder(myViewBinder);
//     }
        }

//     return simpleAdapter;
        return simpleAdapter;
// }
    }

// void CBrowserActivity::MyImageLoaded(
    function MyImageLoaded(
//     /* [in] */ IDrawable* imageDrawable,
        imageDrawable,
//     /* [in] */ IImageView* imageView)
        imageView)
// {
    {
//     Logger::D(TAG, "MyImageLoaded()-----");
        elog(TAG + "MyImageLoaded()-----");
//     imageView->SetImageDrawable(imageDrawable);
        imageView.SetImageDrawable(imageDrawable);
// }
    }

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