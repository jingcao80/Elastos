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

    var IToast__LENGTH_LONG = 1;

    var timeOnCreate;
    var timeOnStart;

    //Elastos::Droid::App::EIID_IActivity
    var EIID_IActivity = [0x1068F256,0x8292,0x2C57,[0x7E,0xCB,0xFE,0xE1,0xA0,0xA4,0x0C,0x1B]];

    var _debug = false;

//--------common definition----end----

//--------CxxxActivity.h----begin----

// #ifndef __CPHOTOACTIVITY_H__
// #define __CPHOTOACTIVITY_H__

// #include "_CPhotoActivity.h"
// #include "app/Activity.h"
// #include "os/HandlerBase.h"
// #include "src/ui/DetailsHelper.h"
    var DetailsHelper = require("./ui/DetailsHelper.js")(aoElastos);
// #include "src/data/AsyncImageLoader.h"
    var AsyncImageLoader = require("./data/AsyncImageLoader.js")(aoElastos);

// using Elastos::Droid::App::Activity;
// using Elastos::Droid::App::IDialog;
// using Elastos::Droid::View::IViewOnClickListener;
// using Elastos::Droid::Os::HandlerBase;
// using Elastos::Droid::Widget::IImageView;
// using Elastos::Droid::Widget::IImageButton;
// using Elastos::Droid::Widget::ITextView;
// using Elastos::Droid::Widget::IPopupWindow;
// using Elastos::Droid::Widget::IPopupWindowOnDismissListener;
// using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
// using Elastos::Droid::Widget::ISeekBar;
// using Elastos::Droid::Content::IDialogInterface;
// using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;

// namespace Gallery {

// class CPhotoActivity
//     : public Activity
// {
// private:
//     class MyHandler
//         : public HandlerBase
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
//         : public ICloseListener
//         , public IViewOnClickListener
//         , public IPopupWindowOnDismissListener
//         , public ISeekBarOnSeekBarChangeListener
    var MyListener;
//     {
//     public:
//         MyListener(
//             /* [in] */ CPhotoActivity* host);

//         CAR_INTERFACE_DECL();

//         CARAPI OnClick(
//             /* [in] */ IView* v);

//         CARAPI OnDismiss();

//         CARAPI OnProgressChanged(
//             /* [in] */ ISeekBar* seekBar,
//             /* [in] */ Int32 progress,
//             /* [in] */ Boolean fromUser);

//         CARAPI OnStartTrackingTouch(
//             /* [in] */ ISeekBar* seekBar);

//         CARAPI OnStopTrackingTouch(
//             /* [in] */ ISeekBar* seekBar);

//         CARAPI OnClose();

//     private:
//         CPhotoActivity* mHost;
//     };

//     class DialogListener
//         : public ElRefBase
//         , public IDialogInterfaceOnDismissListener
    var DialogListener;
//     {
//     public:
//         CAR_INTERFACE_DECL();

//         DialogListener(
//             /* [in] */ CPhotoActivity* host);

//         CARAPI OnDismiss(
//             /* [in] */ IDialogInterface* dialog);

//     private:
//         CPhotoActivity* mHost;
//     };

//     class MyLoadImageCallback
//         : public ILoadImageCallback
    var MyLoadImageCallback;
//     {
//     public:
//         MyLoadImageCallback(
//             /* [in] */ IWeakReference* host,
//             /* [in] */ const String& path,
//             /* [in] */ Boolean isHigh);

//         CARAPI ImageLoaded(
//             /* [in] */ IDrawable* imageDrawable,
//             /* [in] */ IImageView* imageView);

//     private:
//         AutoPtr<IWeakReference> mWeakHost;
//         String mPath;
//         Boolean mIsHigh;
//     };

//     class PhotoEntry
//         : public ElRefBase
    var PhotoEntry = function () {};    //no constructor
//     {
//     public:
//         String sourcePath;
//         String desc;
//     };

// friend class MyListener;
// friend class MyLoadImageCallback;

// public:
//     CPhotoActivity();
//     ~CPhotoActivity();

// protected:

//     CARAPI OnCreate(
//         /* [in] */ IBundle* savedInstanceState);

//     CARAPI OnStart();

//     CARAPI OnResume();

//     CARAPI OnPause();

//     CARAPI OnStop();

//     CARAPI OnDestroy();

//     CARAPI OnBackPressed();

//     CARAPI OnNewIntent(
//         /* [in] */ IIntent *intent);

// private:
//     CARAPI OpenZoomPopupWindow(
//         /* [in] */ IView* v);

//     CARAPI CloseZoomPopupWindow();

//     CARAPI OpenMorePopupWindow(
//         /* [in] */ IView* v);

//     CARAPI CloseMorePopupWindow();

//     CARAPI OpenShareDialog();

//     CARAPI ShareDialogOnOK();

//     CARAPI ShareDialogOnCancel();

//     CARAPI OpenDeleteDialog();

//     CARAPI DeleteDialogOnOK();

//     CARAPI DeleteDialogOnCancel();

//     CARAPI_(void) ShowDetails();

//     CARAPI_(void) HideDetails();

//     CARAPI_(void) SetCurrentPhoto();

//     CARAPI_(void) ChangeCurrentPhoto(
//         /* [in] */ Boolean isNext);

//     CARAPI_(void) MyImageLoaded(
//         /* [in] */ IDrawable* imageDrawable,
//         /* [in] */ IImageView* imageView,
//         /* [in] */ const String& path,
//         /* [in] */ Boolean isHigh);

//     CARAPI_(void) SetDetailInfo(
//         /* [in] */ const String& filePath);

//     CARAPI_(void) LoadCurrentImage(
//         /* [in] */ const String& path,
//         /* [in] */ Boolean isHighQuality);

//     CARAPI_(void) SwitchDisplayView(
//         /* [in] */ Boolean showLoading);

//     CARAPI_(void) UpdateLoadingStatus(
//         /* [in] */ Boolean reload);

//     CARAPI_(void) TriggerLoadingCallback(
//         /* [in] */ Boolean isCancel);

//     CARAPI_(Boolean) LoadPhotoEntryList(
//         /* [in] */ IIntent* intent);

// private:
//     static const String TAG;
    var TAG;
//     static const Int32 MSG_IMAGE_LOADED;
    var MSG_IMAGE_LOADED;
//     static const Int32 MSG_UPDATE_LOADING_STATUS;
    var MSG_UPDATE_LOADING_STATUS;

// private:
//     Int32 mIndex;
    var mIndex;
//     Int32 mCurrentIndex;
    var mCurrentIndex;
//     Int32 mTotalSize;
    var mTotalSize;
//     Int32 mDisplayWidth;
    var mDisplayWidth;
//     Int32 mDisplayHeight;
    var mDisplayHeight;

//     Boolean mShowMore;
    var mShowMore;
//     Boolean mShowZoom;
    var mShowZoom;
//     Boolean mShowDetails;
    var mShowDetails;
//     Boolean mViewSingleItem;
    var mViewSingleItem;

//     String mFolderPath;
    var mFolderPath;
//     String mCurrentImagePath;
    var mCurrentImagePath;

//     AutoPtr<IImageView> mUpButton;
    var mUpButton;
//     AutoPtr<ITextView> mPhotoName;
    var mPhotoName;
//     AutoPtr<IImageView> mPhotoSlideshow;
    var mPhotoSlideshow;
//     AutoPtr<IImageView> mPhotoMore;
    var mPhotoMore;
//     AutoPtr<IImageView> mPhotoView;
    var mPhotoView;
//     AutoPtr<ITextView> mPhotoLoadingText;
    var mPhotoLoadingText;
//     AutoPtr<IImageButton> mLeftButton;
    var mLeftButton;
//     AutoPtr<IImageButton> mRightButton;
    var mRightButton;
//     AutoPtr<IImageButton> mZoomButton;
    var mZoomButton;
//     AutoPtr<IImageButton> mRotateLeftButton;
    var mRotateLeftButton;
//     AutoPtr<IImageButton> mRotateRightButton;
    var mRotateRightButton;
//     AutoPtr<IImageButton> mShareButton;
    var mShareButton;
//     AutoPtr<IImageButton> mEditButton;
    var mEditButton;
//     AutoPtr<IImageButton> mDeleteButton;
    var mDeleteButton;
//     AutoPtr<IView> mTopView;
    var mTopView;
//     AutoPtr<IResources> mRes;
    var mRes;
//     AutoPtr<IPopupWindow> mZoomPopupWindow;
    var mZoomPopupWindow;
//     AutoPtr<IPopupWindow> mMorePopupWindow;
    var mMorePopupWindow;
//     AutoPtr<MyListener> mListener;
    var mListener;
//     AutoPtr<DialogListener> mDialogListener;
    var mDialogListener;
//     AutoPtr<IDialog> mShareDialog;
    var mShareDialog;
//     AutoPtr<IDialog> mDeleteDialog;
    var mDeleteDialog;
//     AutoPtr<DetailsHelper> mDetailsHelper;
    var mDetailsHelper;
//     AutoPtr< ArrayOf<String> > mDetailsArray;
    var mDetailsArray = [];
//     List< AutoPtr<PhotoEntry> > mPhotoEntryList;
    var mPhotoEntryList = [];
//     AutoPtr<IHandler> mMyHandler;
    var mMyHandler;
// };

// } // namespace Gallery

// #endif // __CPHOTOACTIVITY_H__

//--------CxxxActivity.h----end----

//--------CxxxActivity.cpp----begin----

// #include "CPhotoActivity.h"
// #include "src/data/DataSourceHelper.h"
    var DataSourceHelper = require("./data/DataSourceHelper.js")(aoElastos);
// #include "src/data/AsyncImageLoader.h"
// #include "src/util/Utils.h"
    var Utils = require("./util/Utils.js")(aoElastos);
// #include "R.h"
// #include <elastos/Logger.h>
// #include <elastos/StringBuilder.h>
// #include <elastos/StringUtils.h>
// #include <elastos/Math.h>
// #include "os/SomeArgs.h"

// using Elastos::Droid::Os::SomeArgs;
// using Elastos::Core::CStringWrapper;
// using Elastos::Core::StringBuilder;
// using Elastos::Core::StringUtils;
// using Elastos::Droid::App::CDialog;
// using Elastos::Droid::App::IIActivityManager;
// using Elastos::Droid::Content::CIntent;
// using Elastos::Droid::Content::IContext;
// using Elastos::Droid::Content::IIntent;
// using Elastos::Droid::Content::Res::IResources;
// using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
// using Elastos::Droid::Graphics::Drawable::IDrawable;
// using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
// using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
// using Elastos::Droid::Graphics::Drawable::EIID_IDrawable;
// using Elastos::Droid::Os::CServiceManager;
// using Elastos::Droid::Os::IServiceManager;
// using Elastos::Droid::Utility::CDisplayMetrics;
// using Elastos::Droid::Utility::IDisplayMetrics;
// using Elastos::Droid::View::EIID_IViewOnClickListener;
// using Elastos::Droid::View::IDisplay;
// using Elastos::Droid::View::IGravity;
// using Elastos::Droid::Widget::CPopupWindow;
// using Elastos::Droid::Widget::EIID_IPopupWindowOnDismissListener;
// using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
// using Elastos::Droid::Widget::IBaseAdapter;
// using Elastos::Droid::Widget::IButton;
// using Elastos::Droid::Widget::IGridView;
// using Elastos::Droid::Widget::IToast;
// using Elastos::Droid::App::EIID_IActivity;
// using Elastos::IO::CFile;
// using Elastos::IO::IFile;
// using Elastos::Text::CSimpleDateFormat;
// using Elastos::Text::ISimpleDateFormat;
// using Elastos::Utility::CDate;
// using Elastos::Utility::IDate;
// using Elastos::Utility::Logging::Logger;

// namespace Gallery {

// const String CPhotoActivity::TAG("CPhotoActivity");
    TAG = "CPhotoActivity: ";
// const Int32 CPhotoActivity::MSG_IMAGE_LOADED = 0;
    MSG_IMAGE_LOADED = 0;
// const Int32 CPhotoActivity::MSG_UPDATE_LOADING_STATUS = 1;
    MSG_UPDATE_LOADING_STATUS = 1;

// ECode CPhotoActivity::MyHandler::HandleMessage(
    MyHandler.prototype.HandleMessage = function(
//     /* [in] */ IMessage* msg)
        msg)
// {
    {
//     AutoPtr<IActivity> strongObj;
        //var strongObj;
//     mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
        //strongObj = this.mWeakHost.Resolve(EIID_IActivity);
//     if (strongObj == NULL) {
        //if (!strongObj) {
//         return NOERROR;
            //return;
//     }
        //}

//     CPhotoActivity* mHost = (CPhotoActivity*)strongObj.Get();
        //var mHost = strongObj;

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
//         case CPhotoActivity::MSG_IMAGE_LOADED: {
            case MSG_IMAGE_LOADED:
//             SomeArgs* args = (SomeArgs*)obj.Get();
                var args = obj;
//             IDrawable* drawable = IDrawable::Probe(args->mArg1);
                var drawable = args.mArg1;
//             IImageView* imageView = IImageView::Probe(args->mArg2);
                var imageView = args.mArg2;
//             ICharSequence* seq = ICharSequence::Probe(args->mArg3);
                var seq = args.mArg3;
//             String path;
                var path;
//             seq->ToString(&path);
                path = seq.ToString();
//             Boolean isHigh = args->mArgi1 != 0;
                var isHigh = args.mArgi1 != 0;
//             mHost->MyImageLoaded(drawable, imageView, path, isHigh);
                MyImageLoaded(drawable, imageView, path, isHigh);
//             args->Recycle();
                //
//             break;
                break;
//         }
//         case CPhotoActivity::MSG_UPDATE_LOADING_STATUS: {
            case MSG_UPDATE_LOADING_STATUS:
//             Int32 arg1;
                var arg1;
//             msg->GetArg1(&arg1);
                arg1 = msg.arg1;
//             mHost->UpdateLoadingStatus(arg1 == 1);
                UpdateLoadingStatus(arg1 == 1);
//             break;
                break;
//         }
//     }
        }

//     return NOERROR;
        return;
// }
    }

// CPhotoActivity::MyListener::MyListener(
    MyListener = function(
//     /* [in] */ CPhotoActivity* host)
        host)
//     : mHost(host)
// {}
    {
        this.mHost = host;
    }

// PInterface CPhotoActivity::MyListener::Probe(
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
//     else if (riid == EIID_ISeekBarOnSeekBarChangeListener) {
//         return (ISeekBarOnSeekBarChangeListener*)this;
//     }
//     else if (riid == EIID_ICloseListener) {
//         return (PInterface)(ICloseListener*)this;
//     }

//     return NULL;
// }

// UInt32 CPhotoActivity::MyListener::AddRef()
// {
//     return ElRefBase::AddRef();
// }

// UInt32 CPhotoActivity::MyListener::Release()
// {
//     return ElRefBase::Release();
// }

// ECode CPhotoActivity::MyListener::GetInterfaceID(
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
//     else if (pObject == (IInterface*)(ISeekBarOnSeekBarChangeListener*)this) {
//         *pIID = EIID_ISeekBarOnSeekBarChangeListener;
//     }
//     else if (pObject == (IInterface*)(ICloseListener*)this) {
//         *pIID = EIID_ICloseListener;
//     }
//     else {
//         return E_INVALID_ARGUMENT;
//     }
//     return NOERROR;
// }

// ECode CPhotoActivity::MyListener::OnClick(
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
        switch (viewId) {
//         case R::id::photo_up: {
            case R.id.photo_up :
//             Logger::D(TAG, "OnClick()---photo_up");
                elog(TAG + "OnClick()---photo_up");
//             mHost->OnBackPressed();
                OnBackPressed();
//             break;
                break;
//         }
//         case R::id::photo_slideshow: {
            case R.id.photo_slideshow :
//             Logger::D(TAG, "OnClick()---photo_slideshow");
                elog(TAG + "OnClick()---photo_slideshow");
//             break;
                break;
//         }
//         case R::id::photo_more: {
            case R.id.photo_more :
//             Logger::D(TAG, "OnClick()---photo_more");
                elog(TAG + "OnClick()---photo_more");
//             if (mHost->mShowMore) {
                if (mShowMore) {
//                 mHost->CloseMorePopupWindow();
                    CloseMorePopupWindow();
//             }
                }
//             else {
                else {
//                 mHost->OpenMorePopupWindow(v);
                    OpenMorePopupWindow(v);
//             }
                }
//             break;
                break;
//         }
//         case R::id::photo_detail: {
            case R.id.photo_detail :
//             Logger::D(TAG, "OnClick()---photo_detail");
                elog(TAG + "OnClick()---photo_detail");
//             break;
                break;
//         }
//         case R::id::img_btn_left: {
            case R.id.img_btn_left :
//             Logger::D(TAG, "OnClick()---img_btn_left--mCurrentIndex:%d",mHost->mCurrentIndex);
                elog(TAG + "OnClick()---img_btn_left--mCurrentIndex:" + mCurrentIndex);
//             mHost->ChangeCurrentPhoto(FALSE);
                ChangeCurrentPhoto(false);
//             break;
                break;
//         }
//         case R::id::img_btn_right: {
            case R.id.img_btn_right :
//             Logger::D(TAG, "OnClick()---img_btn_right--mCurrentIndex:%d",mHost->mCurrentIndex);
                elog(TAG + "OnClick()---img_btn_right--mCurrentIndex:" + mCurrentIndex);
//             mHost->ChangeCurrentPhoto(TRUE);
                ChangeCurrentPhoto(true);
//             break;
                break;
//         }
//         case R::id::img_btn_zoom: {
            case R.id.img_btn_zoom :
//             Logger::D(TAG, "OnClick()---img_btn_zoom");
                elog(TAG + "OnClick()---img_btn_zoom");
//             if (mHost->mShowZoom) {
                if (mShowZoom) {
//                 mHost->CloseZoomPopupWindow();
                    CloseZoomPopupWindow();
//             }
                }
//             else {
                else {
//                 mHost->OpenZoomPopupWindow(v);
                    OpenZoomPopupWindow(v);
//             }
                }
//             break;
                break;
//         }
//         case R::id::img_btn_rotate_left: {
            case R.id.img_btn_rotate_left :
//             Logger::D(TAG, "OnClick()---img_btn_rotate_left");
                elog(TAG + "OnClick()---img_btn_rotate_left");
//             break;
                break;
//         }
//         case R::id::img_btn_rotate_right: {
            case R.id.img_btn_rotate_right :
//             Logger::D(TAG, "OnClick()---img_btn_rotate_right");
                elog(TAG + "OnClick()---img_btn_rotate_right");
//             break;
                break;
//         }
//         case R::id::img_btn_share: {
            case R.id.img_btn_share :
//             Logger::D(TAG, "OnClick()---img_btn_share");
                elog(TAG + "OnClick()---img_btn_share");
//             mHost->OpenShareDialog();
                OpenShareDialog();
//             break;
                break;
//         }
//         case R::id::img_btn_edit: {
            case R.id.img_btn_edit :
//             Logger::D(TAG, "OnClick()---img_btn_edit");
                elog(TAG + "OnClick()---img_btn_edit");
//             AutoPtr<PhotoEntry> entry = mHost->mPhotoEntryList[mHost->mCurrentIndex];
                var entry = mPhotoEntryList[mCurrentIndex];
//             AutoPtr<IIntent> intent;
//             CIntent::New((IIntent**)&intent);
                var intent = Droid_New("Elastos.Droid.Content.CIntent");
//             intent->SetClassNameEx(String("Gallery"), String("Gallery.CEditActivity"));
                intent.SetClassName("Elastos.DevSamples.Node.JSGallery", "Elastos.DevSamples.Node.JSGallery.CEditActivity");
//             intent->PutStringExtra(DataSourceHelper::SOURCE_PATH, entry->sourcePath);
                intent.PutExtra(DataSourceHelper.SOURCE_PATH, entry.sourcePath);
//             if (FAILED(mHost->StartActivity(intent))) {
                try {oActivity.StartActivity(intent)} catch(e) {
//                 Logger::E(TAG, "OnClick()---StartActivity CEditActivity failed!");
                    elog(TAG + "OnClick()---StartActivity CEditActivity failed!");
//             }
                }
//             break;
                break;
//         }
//         case R::id::img_btn_delete: {
            case R.id.img_btn_delete :
//             Logger::D(TAG, "OnClick()---img_btn_delete");
                elog(TAG + "OnClick()---img_btn_delete");
//             mHost->OpenDeleteDialog();
                OpenDeleteDialog();
//             break;
                break;
//         }
//         case R::id::more_pop_wallpaper: {
            case R.id.more_pop_wallpaper :
//             Logger::D(TAG, "OnClick()---more_pop_wallpaper");
                elog(TAG + "OnClick()---more_pop_wallpaper");
//             mHost->CloseMorePopupWindow();
                CloseMorePopupWindow();
//             AutoPtr<PhotoEntry> entry = mHost->mPhotoEntryList[mHost->mCurrentIndex];
                var entry = mPhotoEntryList[mCurrentIndex];
//             AutoPtr<IIntent> intent;
//             CIntent::New((IIntent**)&intent);
                var intent = Droid_New("Elastos.Droid.Content.CIntent");
//             intent->SetClassNameEx(String("Gallery"), String("Gallery.CWallpaperActivity"));
                intent.SetClassName("Elastos.DevSamples.Node.JSGallery", "Elastos.DevSamples.Node.JSGallery.CWallpaperActivity");
//             intent->PutStringExtra(DataSourceHelper::SOURCE_PATH, entry->sourcePath);
                intent.PutStringExtra(DataSourceHelper.SOURCE_PATH, entry.sourcePath);
//             if (FAILED(mHost->StartActivity(intent))) {
                try {StartActivity(intent)} catch(e) {
//                 Logger::E(TAG, "OnClick()---StartActivity CWallpaperActivity failed!");
                    elog(TAG + "OnClick()---StartActivity CWallpaperActivity failed!");
//             }
                }
//             break;
                break;
//         }
//         case R::id::more_pop_info: {
            case R.id.more_pop_info :
//             Logger::D(TAG, "OnClick()---more_pop_info");
                elog(TAG + "OnClick()---more_pop_info");
//             mHost->CloseMorePopupWindow();
                CloseMorePopupWindow();
//             if (mHost->mShowDetails) {
                if (mShowDetails) {
//                 mHost->HideDetails();
                    HideDetails();
//             }
                }
//             else {
                else {
//                 mHost->ShowDetails();
                    ShowDetails();
//             }
                }
//             break;
                break;
//         }
//         case R::id::share_dialog_btn_cancel: {
            case R.id.share_dialog_btn_cancel :
//             Logger::D(TAG, "OnClick()---share_dialog_btn_cancel");
                elog(TAG + "OnClick()---share_dialog_btn_cancel");
//             mHost->ShareDialogOnCancel();
                ShareDialogOnCancel();
//             break;
//         }
//         case R::id::delete_dialog_btn_ok: {
            case R.id.delete_dialog_btn_ok :
//             Logger::D(TAG, "OnClick()---delete_dialog_btn_ok");
                elog(TAG + "OnClick()---delete_dialog_btn_ok");
//             mHost->DeleteDialogOnOK();
                DeleteDialogOnOK();
//             break;
                break;
//         }
//         case R::id::delete_dialog_btn_cancel: {
            case R.id.delete_dialog_btn_cancel :
//             Logger::D(TAG, "OnClick()---delete_dialog_btn_cancel");
                elog(TAG + "OnClick()---delete_dialog_btn_cancel");
//             mHost->DeleteDialogOnCancel();
                DeleteDialogOnCancel();
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

// ECode CPhotoActivity::MyListener::OnDismiss()
    MyListener.prototype.OnDismiss = function()
// {
    {
//     Logger::D(TAG, "OnDismiss()---: TODO");
        elog(TAG + "OnDismiss()---: TODO");
//     if (mHost->mZoomPopupWindow) {
        if (mZoomPopupWindow) {
//         mHost->mZoomPopupWindow = NULL;
            mZoomPopupWindow = null;
//         mHost->mShowZoom = FALSE;
            mShowZoom = false;
//     }
        }

//     if (mHost->mMorePopupWindow) {
        if (mMorePopupWindow) {
//         mHost->mMorePopupWindow = NULL;
            mMorePopupWindow = null;
//         mHost->mShowMore = FALSE;
            mShowMore = false;
//     }
        }
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::MyListener::OnProgressChanged(
    MyListener.prototype.OnProgressChanged = function(
//     /* [in] */ ISeekBar* seekBar,
        seekBar,
//     /* [in] */ Int32 progress,
        progress,
//     /* [in] */ Boolean fromUser)
        fromUser)
// {
    {
//     Logger::D(TAG, "OnProgressChanged()---progress:%d", progress);
        elog(TAG + "OnProgressChanged()---progress:" + progress);
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::MyListener::OnStartTrackingTouch(
    MyListener.prototype.OnStartTrackingTouch = function(
//     /* [in] */ ISeekBar* seekBar)
        seekBar)
// {
    {
//     Logger::D(TAG, "OnStartTrackingTouch()---");
        elog(TAG + "OnStartTrackingTouch()---");
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::MyListener::OnStopTrackingTouch(
    MyListener.prototype.OnStopTrackingTouch = function(
//     /* [in] */ ISeekBar* seekBar)
        seekBar)
// {
    {
//     Logger::D(TAG, "OnStopTrackingTouch()---");
        elog(TAG + "OnStopTrackingTouch()---");
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::MyListener::OnClose()
    MyListener.prototype.OnClose = function()
// {
    {
//     Logger::D(TAG, "OnClose()---");
        elog(TAG + "OnClose()---");
//     mHost->HideDetails();
        HideDetails();
//     return NOERROR;
        return;
// }
    }

// CAR_INTERFACE_IMPL(CPhotoActivity::DialogListener, IDialogInterfaceOnDismissListener)

// CPhotoActivity::DialogListener::DialogListener(
    DialogListener = function(
//     /* [in] */ CPhotoActivity* host)
        host)
//     : mHost(host)
// {
    {
        this.mHost = host;
// }
    }

// ECode CPhotoActivity::DialogListener::OnDismiss(
    DialogListener.prototype.OnDismiss = function (
//     /* [in] */ IDialogInterface* dialog)
        dialog)
// {
    {
//     if (IDialogInterface::Probe(mHost->mShareDialog) == dialog) {
        if (mShareDialog == dialog) {
//         Logger::D(TAG, "OnDismissShareDialog---: %p", mHost->mShareDialog.Get());
            elog("TAG" + "OnDismissShareDialog---: ") + mShareDialog;
//         mHost->mShareDialog = NULL;
            mShareDialog = null;
//     }
        }
//     else if (IDialogInterface::Probe(mHost->mDeleteDialog) == dialog) {
        else if (mDeleteDialog == dialog) {
//         Logger::D(TAG, "OnDismissDeleteDialog---: %p", mHost->mDeleteDialog.Get());
            elog(TAG + "OnDismissDeleteDialog---: " + mDeleteDialog);
//         mHost->mDeleteDialog = NULL;
            mDeleteDialog = null;
//     }
        }

//     return NOERROR;
        return;
// }
    }

// CPhotoActivity::MyLoadImageCallback::MyLoadImageCallback(
    MyLoadImageCallback = function(
//     /* [in] */ IWeakReference* host,
        host,
//     /* [in] */ const String& path,
        path,
//     /* [in] */ Boolean isHigh)
        isHigh)
//     : mWeakHost(host)
//     , mPath(path)
//     , mIsHigh(isHigh)
// {}
    {
        this.mWeakHost = host;
        this.mPath = path;
        this.mIsHigh = isHigh;
    }

// ECode CPhotoActivity::MyLoadImageCallback::ImageLoaded(
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

//     CPhotoActivity* mHost = (CPhotoActivity*)strongObj.Get();
//     if (mHost->mMyHandler == NULL) {
//         return NOERROR;
//     }

//     AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        var args = {};
//     args->mArg1 = imageDrawable;
        args.mArg1 = imageDrawable;
//     args->mArg2 = imageView;
        args.mArg2 = imageView;
//     AutoPtr<ICharSequence> seq;
        var seq;
//     CStringWrapper::New(mPath, (ICharSequence**)&seq);
        seq = CString(this.mPath);
//     args->mArg3 = seq;
        args.mArg3 = seq;
//     args->mArgi1 = mIsHigh ? 1 : 0;
        args.mArgi1 = this.mIsHigh ? 1 : 0;

//     AutoPtr<IMessage> msg;
        var msg;
//     mHost->mMyHandler->ObtainMessageEx(CPhotoActivity::MSG_IMAGE_LOADED, args, (IMessage**)&msg);
        msg = {what:MSG_IMAGE_LOADED, obj:args};
//     Boolean result;
        var result;
//     return mHost->mMyHandler->SendMessage(msg, &result);
        result = mMyHandler.HandleMessage(msg);
        return result;
// }
    }

// CPhotoActivity::CPhotoActivity()
    function CPhotoActivity(
//     : mIndex(0)
        amIndex,
//     , mCurrentIndex(0)
        amCurrentIndex,
//     , mTotalSize(0)
        amTotalSize,
//     , mDisplayWidth(0)
        amDisplayWidth,
//     , mDisplayHeight(0)
        amDisplayHeight,
//     , mShowMore(FALSE)
        amShowMore,
//     , mShowZoom(FALSE)
        amShowZoom,
//     , mShowDetails(FALSE)
        amShowDetails,
//     , mViewSingleItem(FALSE)
        amViewSingleItem)
// {
    {
        mIndex = amIndex || 0;
        mCurrentIndex = amCurrentIndex || 0;
        mTotalSize = amTotalSize || 0
        mDisplayWidth = amDisplayWidth || 0;
        mDisplayHeight = amDisplayHeight || 0;
        mShowMore = amShowMore || false;
        mShowZoom = amShowZoom || false;
        mShowDetails = amShowDetails || false;
        mViewSingleItem = amViewSingleItem || false;

//     Logger::D(TAG, "CPhotoActivity(): %p", this);
        elog(TAG + "CPhotoActivity(): " + typeof this);
// }
    }
    var mCPhotoActivity = new CPhotoActivity();

// CPhotoActivity::~CPhotoActivity()
// {
//     Logger::D(TAG, "~CPhotoActivity(): %p", this);
// }

// ECode CPhotoActivity::OnCreate(
    _apt.OnCreate = function(context,
//     /* [in] */ IBundle* savedInstanceState)
    savedInstanceState)
// {
    {
        timeOnCreate = new Date().getTime();

//     Logger::D(TAG, "OnCreate()---");
        elog(TAG + "OnCreate()---");
//     Activity::OnCreate(savedInstanceState);
        //TODO:super.OnCreate(savedInstanceState);
//     SetContentView(R::layout::activity_photo);
        context.SetContentView(R.layout.activity_photo);

//     AutoPtr<IWeakReference> weakHost;
        var weakHost;
//     GetWeakReference((IWeakReference**)&weakHost);
        weakHost = oActivity.GetWeakReference();
//     mMyHandler = new MyHandler(weakHost);
        mMyHandler = new MyHandler(weakHost);

//     mListener = new MyListener(this);
        mListener = new MyListener(oActivity);
//     mDialogListener = new DialogListener(this);
        mDialogListener = new DialogListener(oActivity);
//     mDetailsArray = ArrayOf<String>::Alloc(6);
        mDetailsArray = [];

        mPhotoSlideshow = context.FindViewById(R.id.photo_slideshow);

//     AutoPtr<IIntent> intent;
        var intent;
//     GetIntent((IIntent**)&intent);
        intent = context.GetIntent();
//     if (!LoadPhotoEntryList(intent))
//         return NOERROR;
        //if (!LoadPhotoEntryList(intent)) return;
        if (!LoadPhotoEntryList(intent)) {
            return;
        }
        else {
            //
        }

//     AutoPtr<IView> view = FindViewById(R::id::photo_up);
//     mUpButton = IImageView::Probe(view);
//     assert(mUpButton != NULL);
        mUpButton = context.FindViewById(R.id.photo_up);

//     view = FindViewById(R::id::photo_name);
//     mPhotoName = ITextView::Probe(view);
//     assert(mPhotoName != NULL);
        mPhotoName = context.FindViewById(R.id.photo_name);

//     view = FindViewById(R::id::photo_slideshow);
//     mPhotoSlideshow = IImageView::Probe(view);
//     assert(mPhotoSlideshow != NULL);
        //mPhotoSlideshow = context.FindViewById(R.id.photo_slideshow);

//     view = FindViewById(R::id::photo_more);
//     mPhotoMore = IImageView::Probe(view);
//     assert(mPhotoMore != NULL);
        mPhotoMore = context.FindViewById(R.id.photo_more);

//     view = FindViewById(R::id::photo_detail);
//     mPhotoView = IImageView::Probe(view);
//     assert(mPhotoView != NULL);
        mPhotoView = context.FindViewById(R.id.photo_detail);

//     view = FindViewById(R::id::photo_loading);
//     mPhotoLoadingText = ITextView::Probe(view);
//     assert(mPhotoLoadingText != NULL);
        mPhotoLoadingText = context.FindViewById(R.id.photo_loading);

//     view = FindViewById(R::id::img_btn_left);
//     mLeftButton = IImageButton::Probe(view);
//     assert(mLeftButton != NULL);
        mLeftButton = context.FindViewById(R.id.img_btn_left);

//     view = FindViewById(R::id::img_btn_right);
//     mRightButton = IImageButton::Probe(view);
//     assert(mRightButton != NULL);
        mRightButton = context.FindViewById(R.id.img_btn_right);

//     view = FindViewById(R::id::img_btn_zoom);
//     mZoomButton = IImageButton::Probe(view);
//     assert(mZoomButton != NULL);
        mZoomButton = context.FindViewById(R.id.img_btn_zoom);

//     view = FindViewById(R::id::img_btn_rotate_left);
//     mRotateLeftButton = IImageButton::Probe(view);
//     assert(mRotateLeftButton != NULL);
        mRotateLeftButton = context.FindViewById(R.id.img_btn_rotate_left);

//     view = FindViewById(R::id::img_btn_rotate_right);
//     mRotateRightButton = IImageButton::Probe(view);
//     assert(mRotateRightButton != NULL);
        mRotateRightButton = context.FindViewById(R.id.img_btn_rotate_right);

//     view = FindViewById(R::id::img_btn_share);
//     mShareButton = IImageButton::Probe(view);
//     assert(mShareButton != NULL);
        mShareButton = context.FindViewById(R.id.img_btn_share);

//     view = FindViewById(R::id::img_btn_edit);
//     mEditButton = IImageButton::Probe(view);
//     assert(mEditButton != NULL);
        mEditButton = context.FindViewById(R.id.img_btn_edit);

//     view = FindViewById(R::id::img_btn_delete);
//     mDeleteButton = IImageButton::Probe(view);
//     assert(mDeleteButton != NULL);
        mDeleteButton = context.FindViewById(R.id.img_btn_delete);

//     AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mListener.Get();
        var clickListener = mListener;
//     mUpButton->SetOnClickListener(clickListener);
        mUpButton.SetOnClickListener(clickListener);
//     mPhotoSlideshow->SetOnClickListener(clickListener);
        mPhotoSlideshow.SetOnClickListener(clickListener);
//     mPhotoMore->SetOnClickListener(clickListener);
        mPhotoMore.SetOnClickListener(clickListener);
//     mLeftButton->SetOnClickListener(clickListener);
        mLeftButton.SetOnClickListener(clickListener);
//     mRightButton->SetOnClickListener(clickListener);
        mRightButton.SetOnClickListener(clickListener);
//     mZoomButton->SetOnClickListener(clickListener);
        mZoomButton.SetOnClickListener(clickListener);
//     mRotateLeftButton->SetOnClickListener(clickListener);
        mRotateLeftButton.SetOnClickListener(clickListener);
//     mRotateRightButton->SetOnClickListener(clickListener);
        mRotateRightButton.SetOnClickListener(clickListener);
//     mShareButton->SetOnClickListener(clickListener);
        mShareButton.SetOnClickListener(clickListener);
//     mEditButton->SetOnClickListener(clickListener);
        mEditButton.SetOnClickListener(clickListener);
//     mDeleteButton->SetOnClickListener(clickListener);
        mDeleteButton.SetOnClickListener(clickListener);

//     if (mViewSingleItem) {
        if (mViewSingleItem) {
//         mUpButton->SetVisibility(IView::INVISIBLE);
            mUpButton.SetVisibility(IView__INVISIBLE);
//         mPhotoSlideshow->SetVisibility(IView::INVISIBLE);
            mPhotoSlideshow.SetVisibility(IView__INVISIBLE);
//     }
        }

//     // for test
        //for test
//     AutoPtr<IDisplayMetrics> dm;
        var dm;
//     AutoPtr<IDisplay> dis;
        var dis;
//     CDisplayMetrics::New((IDisplayMetrics**)&dm);
        dm = Droid_New("Elastos.Droid.Utility.CDisplayMetrics");
//     GetWindowManager()->GetDefaultDisplay((IDisplay**)&dis);
        dis = context.GetWindowManager().GetDefaultDisplay();
//     dis->GetMetrics(dm);
        dis.GetMetrics(dm);
//     Int32 widthPixels = 0, heightPixels = 0;
        var widthPixels = 0, heightPixels = 0;
//     dm->GetWidthPixels(&widthPixels);
        widthPixels = dm.GetWidthPixels();

//     dm->GetHeightPixels(&heightPixels);
        heightPixels = dm.GetHeightPixels();
//     mDisplayWidth = widthPixels - 128;
        mDisplayWidth = widthPixels - 128;
//     mDisplayHeight = heightPixels - 128;
        mDisplayHeight = heightPixels - 128;
//     Logger::D(TAG, "OnCreate()---widthPixels:%d,heightPixels:%d,mDisplayWidth:%d,mDisplayHeight:%d",
//         widthPixels, heightPixels, mDisplayWidth, mDisplayHeight);
        elog(TAG + "OnCreate()---widthPixels:"+widthPixels+",heightPixels:"+heightPixels+",mDisplayWidth:"+mDisplayWidth+",mDisplayHeight:"+mDisplayHeight);

//     SetCurrentPhoto();
        SetCurrentPhoto();

//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::OnStart()
// {
//     Logger::D(TAG, "OnStart()---");
//     Activity::OnStart();
//     return NOERROR;
// }

// ECode CPhotoActivity::OnResume()
// {
//     Logger::D(TAG, "OnResume()---");
//     Activity::OnResume();
//     return NOERROR;
// }

// ECode CPhotoActivity::OnPause()
// {
//     Logger::D(TAG, "OnPause()---");
//     Activity::OnPause();
//     return NOERROR;
// }

// ECode CPhotoActivity::OnStop()
// {
//     Logger::D(TAG, "OnStop()---");
//     Activity::OnStop();
//     return NOERROR;
// }

// ECode CPhotoActivity::OnDestroy()
// {
//     Logger::D(TAG, "OnDestroy()---");
//     Activity::OnDestroy();
//     return NOERROR;
// }

// ECode CPhotoActivity::OnBackPressed()
    function OnBackPressed()
// {
    {
//     Logger::D(TAG, "OnBackPressed()---");
        elog(TAG + "OnBackPressed()---");

//     if (mShowDetails) {
        if (mShowDetails) {
//         HideDetails();
            HideDetails();
//     }
        }
//     else if (mShowMore) {
        else if (mShowMore) {
//         CloseMorePopupWindow();
            CloseMorePopupWindow();
//     }
        }
//     else if (mShowZoom) {
        else if (mShowZoom) {
//         CloseZoomPopupWindow();
            CloseZoomPopupWindow();
//     }
        }
//     else {
        else {
//         Activity::OnBackPressed();
            oActivity.OnBackPressed();
//     }
        }

//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::OpenZoomPopupWindow(
    function OpenZoomPopupWindow(
//     /* [in] */ IView* v)
        v)
// {
    {
//     Logger::D(TAG, "OpenZoomPopupWindow()---");
        elog(TAG + "OpenZoomPopupWindow()---");

//     AutoPtr<ILayoutInflater> inflater;
        var inflater;
//     GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);
        inflater = GetSystemService(IContext__LAYOUT_INFLATER_SERVICE);

//     AutoPtr<IView> layout;
        var layout;
//     inflater->Inflate(R::layout::zoom_popup_dialog, NULL, (IView**)&layout);
        layout = inflater.Inflate(R.layout.zoom_popup_dialog);
//     AutoPtr<IView> view;
        var view;
//     layout->FindViewById(R::id::zoom_seeker, (IView**)&view);
        view = layout.FindViewById(R.id.zoom_seeker);
//     AutoPtr<ISeekBar> seekBar = ISeekBar::Probe(view);
        var seekBar = view;
//     assert(seekBar != NULL);
        if (!seekBar) return;
//     seekBar->SetOnSeekBarChangeListener((ISeekBarOnSeekBarChangeListener*)(mListener.Get()));
        seekBar.SetOnSeekBarChangeListener(mListener);

//     assert(mZoomPopupWindow.Get() == NULL);
        if (!mZoomPopupWindow) return;
//     CPopupWindow::New(layout, 400, 50, TRUE, (IPopupWindow**)&mZoomPopupWindow);
        mZoomPopupWindow = Droid_New("Elastos.Droid.Widget.CPopupWindow", layout, 400, 50, true);
//     mZoomPopupWindow->SetTouchable(TRUE);
        mZoomPopupWindow.SetTouchable(true);
//     mZoomPopupWindow->SetOutsideTouchable(TRUE);
        mZoomPopupWindow.SetOutsideTouchable(true);

//     AutoPtr<IResources> res;
        var res;
//     GetResources((IResources**)&res);
        res = GetResources();
//     AutoPtr<IBitmapDrawable> bitmapDrawable;
        var bitmapDrawable;
//     CBitmapDrawable::New(res, (IBitmap*)NULL, (IBitmapDrawable**)&bitmapDrawable);
        bitmapDrawable = Droid_New("Elastos.Droid.Graphics.Drawable.CBitmapDrawable", res);
//     mZoomPopupWindow->SetBackgroundDrawable(IDrawable::Probe(bitmapDrawable));
        mZoomPopupWindow.SetBackgroundDrawable(bitmapDrawable);
//     mZoomPopupWindow->SetOnDismissListener((IPopupWindowOnDismissListener*)mListener.Get());
        mZoomPopupWindow.SetOnDismissListener(mListener);
//     mZoomPopupWindow->ShowAtLocation(v, IGravity::NO_GRAVITY, 310, 561);
        mZoomPopupWindow.ShowAtLocation(v, IGravity__NO_GRAVITY, 310, 561);
//     mShowZoom = TRUE;
        mShowZoom = true;

//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::CloseZoomPopupWindow()
    function CloseZoomPopupWindow()
// {
    {
//     Boolean isShow = FALSE;
        var isShow = false;
//     if (mZoomPopupWindow != NULL && (mZoomPopupWindow->IsShowing(&isShow), isShow)) {
        if (mZoomPopupWindow && (isShow = mZoomPopupWindow.IsShowing())) {
//         Logger::D(TAG, "CloseZoomPopupWindow()---");
            elog(TAG + "CloseZoomPopupWindow()---");
//         mZoomPopupWindow->Dismiss();
            mZoomPopupWindow.Dismiss();
//         mShowZoom = FALSE;
            mShowZoom = false;
//     }
        }
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::OpenMorePopupWindow(
    function OpenMorePopupWindow(
//     /* [in] */ IView* v)
        v)
// {
    {
//     Logger::D(TAG, "OpenMorePopupWindow()---");
        elog(TAG + "OpenMorePopupWindow()---");
//     AutoPtr<ILayoutInflater> inflater;
        var inflater;
//     GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);
        inflater = GetSystemService(IContext__LAYOUT_INFLATER_SERVICE);

//     AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mListener.Get();
        var clickListener = mListener;
//     AutoPtr<IView> layout;
        var layout;
//     inflater->Inflate(R::layout::more_popup_dialog, NULL, (IView**)&layout);
        layout = inflater.Inflate(R.layout.more_popup_dialog, null);
//     AutoPtr<IView> view;
        var view;
//     layout->FindViewById(R::id::more_pop_wallpaper, (IView**)&view);
        view = layout.FindViewById(R.id.more_pop_wallpaper);
//     AutoPtr<ITextView> popupWallpaper = ITextView::Probe(view);
        var popupWallpaper = view;
//     assert(popupWallpaper != NULL);
        if(!popupWallpaper) return;
//     popupWallpaper->SetOnClickListener(clickListener);
        popupWallpaper.SetOnDismissListener(clickListener);

//     view = NULL;
        view = null;
//     layout->FindViewById(R::id::more_pop_info, (IView**)&view);
        view = layout.FindViewById(R.id.more_pop_info);
//     AutoPtr<ITextView> popupInfo = ITextView::Probe(view);
        var popupInfo = view;
//     assert(popupInfo != NULL);
        if(!popupInfo) return;
//     popupInfo->SetOnClickListener(clickListener);
        popupInfo.SetOnClickListener(clickListener);

//     assert(mMorePopupWindow.Get() == NULL);
        if(!mMorePopupWindow) return;
//     CPopupWindow::New(layout, 80, 63, TRUE, (IPopupWindow**)&mMorePopupWindow);
        mMorePopupWindow = Droid_New("Elastos.Droid.Widget.CPopupWindow", layout, 80, 63,true);
//     mMorePopupWindow->SetTouchable(TRUE);
        mMorePopupWindow.SetTouchable(true);
//     mMorePopupWindow->SetOutsideTouchable(TRUE);
        mMorePopupWindow.SetOutsideTouchable(true);

//     AutoPtr<IResources> res;
        var res;
//     GetResources((IResources**)&res);
        res = GetResources();
//     AutoPtr<IBitmapDrawable> bitmapDrawable;
        var bitmapDrawable;
//     CBitmapDrawable::New(res, (IBitmap*)NULL, (IBitmapDrawable**)&bitmapDrawable);
        bitmapDrawable = Droid_New("Elastos.Droid.Graphics.Drawable.CBitmapDrawable", res);
//     mMorePopupWindow->SetBackgroundDrawable(IDrawable::Probe(bitmapDrawable));
        mMorePopupWindow.SetBackgroundDrawable(bitmapDrawable);
//     mMorePopupWindow->SetOnDismissListener((IPopupWindowOnDismissListener*)mListener.Get());
        mMorePopupWindow.SetOnDismissListener(mListener);
//     mMorePopupWindow->ShowAtLocation(v, IGravity::NO_GRAVITY, 1180, 56);
        mMorePopupWindow.ShowAtLocation(v, IGravity__NO_GRAVITY, 1180, 56);
//     mShowMore = TRUE;
        mShowMore = true;

//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::CloseMorePopupWindow()
    function CloseMorePopupWindow()
// {
    {
//     Boolean isShow = FALSE;
        var isShow = false;
//     if (mMorePopupWindow != NULL && (mMorePopupWindow->IsShowing(&isShow), isShow)) {
        if (mMorePopupWindow && (isShow = mMorePopupWindow.IsShowing())) {
//         Logger::D(TAG, "CloseMorePopupWindow()---");
            elog(TAG + "CloseMorePopupWindow()---");
//         mMorePopupWindow->Dismiss();
            mMorePopupWindow.Dismiss();
//         mShowMore = FALSE;
            mShowMore = false;
//     }
        }
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::OpenShareDialog()
    function OpenShareDialog()
// {
    {
//     Logger::D(TAG, "OpenShareDialog()--- : prev : %p", mShareDialog.Get());
        elog(TAG +"OpenShareDialog()--- : prev :" + mShareDialog);
//     assert(mShareDialog.Get() == 0);
//     CDialog::New(this, R::style::MyDialog, (IDialog**)&mShareDialog);
        mShareDialog = Droid_New("Elastos.Droid.App.CDialog", this, R.style.MyDialog);
//     mShareDialog->SetOnDismissListener((IDialogInterfaceOnDismissListener*)mDialogListener.Get());
        mShareDialog.SetOnDismissListener(mDialogListener);
//     Logger::D(TAG, "OpenShareDialog()--- : new : %p", mShareDialog.Get());
        elog(TAG + "OpenShareDialog()--- : new : " + mShareDialog);
//     mShareDialog->SetContentView(R::layout::share_dialog);
        mShareDialog.SetContentView(R.layout.share_dialog);

//     AutoPtr<IView> view;
//     mShareDialog->FindViewById(R::id::share_dialog_btn_cancel, (IView**)&view);
//     AutoPtr<IButton> cancelButton = IButton::Probe(view);
        var cancelButton = mShareDialog.FindViewById(R.id.share_dialog_btn_cancel);
//     AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mListener.Get();
//     cancelButton->SetOnClickListener(clickListener);
        cancelButton.SetOnClickListener(mListener);

//     mShareDialog->Show();
        mShareDialog.Show();
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::ShareDialogOnOK()
    function ShareDialogOnOK()
// {
    {
//     ShareDialogOnCancel();
        ShareDialogOnCancel();
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::ShareDialogOnCancel()
    function ShareDialogOnCancel()
// {
    {
//     if (mShareDialog) {
        if (mShareDialog) {
//         Logger::D(TAG, "ShareDialogOnCancel---: %p", mShareDialog.Get());
            elog(TAG + "ShareDialogOnCancel---: " + mShareDialog);
//         mShareDialog->Cancel();
            mShareDialog.Cancel();
//     }
        }
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::DeleteDialogOnOK()
    function DeleteDialogOnOK()
// {
    {
//     DeleteDialogOnCancel(); //TODO
        DeleteDialogOnCancel();
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::DeleteDialogOnCancel()
    function DeleteDialogOnCancel()
// {
    {
//     if (mDeleteDialog) {
        if (mDeleteDialog) {
//         Logger::D(TAG, "DeleteDialogOnCancel---: %p", mDeleteDialog.Get());
            elog(TAG + "DeleteDialogOnCancel---: " + mDeleteDialog);
//         mDeleteDialog->Cancel();
            mDeleteDialog.Cancel();
//     }
        }
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::OpenDeleteDialog()
    function OpenDeleteDialog()
// {
    {
//     Logger::D(TAG, "OpenDeleteDialog()--- : prev : %p", mDeleteDialog.Get());
        elog(TAG + "OpenDeleteDialog()---: prev : " + mDeleteDialog);
//     assert(mDeleteDialog.Get() == 0);
//     CDialog::New(this, R::style::MyDialog, (IDialog**)&mDeleteDialog);
        mDeleteDialog = Droid_New("Elastos.Droid.App.CDialog", R.style.MyDialog);
//     mDeleteDialog->SetOnDismissListener((IDialogInterfaceOnDismissListener*)mDialogListener.Get());
        mDeleteDialog.SetOnDismissListener(mDialogListener);
//     Logger::D(TAG, "OpenShareDialog()--- : new : %p", mDeleteDialog.Get());
        elog(TAG + "OpenShareDialog()--- : new : " + mDeleteDialog);
//     mDeleteDialog->SetContentView(R::layout::delete_dialog);
        mDeleteDialog.SetContentView(R.layout.delete_dialog);

//     AutoPtr<IView> view;
//     mDeleteDialog->FindViewById(R::id::delete_dialog_btn_ok, (IView**)&view);
//     AutoPtr<IButton> okButton = IButton::Probe(view);
        var okButton = mDeleteDialog.FindViewById(R.id.delete_dialog_btn_ok);
//     AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mListener.Get();
//     okButton->SetOnClickListener(clickListener);
        okButton.SetOnClickListener(mListener);

//     view = NULL;
//     mDeleteDialog->FindViewById(R::id::delete_dialog_btn_cancel, (IView**)&view);
//     AutoPtr<IButton> cancelButton = IButton::Probe(view);
        var cancelButton = mDeleteDialog.FindViewById(R.id.delete_dialog_btn_cancel);
//     cancelButton->SetOnClickListener(clickListener);
        cancelButton.SetOnClickListener(mListener);

//     mDeleteDialog->Show();
        mDeleteDialog.Show();
//     return NOERROR;
        return;
// }
    }

// void CPhotoActivity::ShowDetails()
    function ShowDetails()
// {
    {
//     mShowDetails = TRUE;
        mShowDetails = true;
//     if (mDetailsHelper == NULL) {
        if (!mDetailsHelper) {
//         mDetailsHelper = new DetailsHelper(this, mDetailsArray);
            mDetailsHelper = new DetailsHelper();
//         AutoPtr<ICloseListener> closeListener = (ICloseListener*)mListener.Get();
            var closeListener = mListener;
//         mDetailsHelper->SetCloseListener(closeListener);
            mDetailsHelper.SetCloseListener(closeListener);
//     }
        }
//     mDetailsHelper->Show();
        mDetailsHelper.Show();
// }
    }

// void CPhotoActivity::HideDetails()
    function HideDetails()
// {
    {
//     mShowDetails = FALSE;
        mShowDetails = false;
//     mDetailsHelper->Hide();
        mDetailsHelper.Hide();
// }
    }

// void CPhotoActivity::SetCurrentPhoto()
    SetCurrentPhoto = function ()
// {
    {
//     Logger::D(TAG, "SetCurrentPhoto()---");
        elog(TAG + "SetCurrentPhoto()---" + mCurrentIndex);

//     if (mCurrentIndex > 0) {
        if (mCurrentIndex > 0) {
//         mLeftButton->SetImageResource(R::drawable::photo_arrow_left);
            mLeftButton.SetImageResource(R.drawable.photo_arrow_left);
//     }
        }
//     else {
        else {
//         mLeftButton->SetImageResource(R::drawable::photo_arrow_left_disable);
            mLeftButton.SetImageResource(R.drawable.photo_arrow_left_disable);
//     }
        }
//     if (mCurrentIndex >= (mTotalSize - 1)) {
        if (mCurrentIndex >= mTotalSize - 1) {
//         mRightButton->SetImageResource(R::drawable::photo_arrow_right_disable);
            mRightButton.SetImageResource(R.drawable.photo_arrow_right_disable);
//     }
        }
//     else {
        else {
//         mRightButton->SetImageResource(R::drawable::photo_arrow_right);
            mRightButton.SetImageResource(R.drawable.photo_arrow_right);
//     }
        }

//     mLeftButton->SetVisibility(mViewSingleItem ? IView::INVISIBLE : IView::VISIBLE);
        mLeftButton.SetVisibility(mViewSingleItem ? IView__INVISIBLE : IView__VISIBLE)
//     mRightButton->SetVisibility(mViewSingleItem ? IView::INVISIBLE : IView::VISIBLE);
        mRightButton.SetVisibility(mViewSingleItem ? IView__INVISIBLE : IView__VISIBLE)

//     AutoPtr<PhotoEntry> entry = mPhotoEntryList[mCurrentIndex];
        var entry = mPhotoEntryList[mCurrentIndex];
//     AutoPtr<ICharSequence> cs;
//     CStringWrapper::New(entry->desc, (ICharSequence**)&cs);
        var cs = CString(entry.desc);
//     mPhotoName->SetText(cs);
        mPhotoName.SetText(cs);
//     mCurrentImagePath = entry->sourcePath;
        mCurrentImagePath = entry.sourcePath;

//     Boolean isNeed = AsyncImageLoader::NeedLoadHighDrawable(mCurrentImagePath);
        var isNeed = AsyncImageLoader.NeedLoadHighDrawable(mCurrentImagePath);
//     if (isNeed) {
        if (isNeed) {
//         LoadCurrentImage(mCurrentImagePath, TRUE);
            LoadCurrentImage(mCurrentImagePath, true);
//     }
        }
//     else {
        else {
//         LoadCurrentImage(mCurrentImagePath, FALSE);
            LoadCurrentImage(mCurrentImagePath, false);
//     }
        }

//     SetDetailInfo(mCurrentImagePath);
        SetDetailInfo(mCurrentImagePath);
// }
    }

// void CPhotoActivity::ChangeCurrentPhoto(
    function ChangeCurrentPhoto(
//     /* [in] */ Boolean isNext)
        isNext)
// {
    {
//     if (isNext) {
        if (isNext) {
//         Int32 tmp = mTotalSize - 1;
            var tmp = mTotalSize - 1;
//         if (mCurrentIndex >= tmp) return;
            if (mCurrentIndex >= tmp) return;
//         mCurrentIndex++;
            mCurrentIndex++;
//         mCurrentIndex = tmp > mCurrentIndex ? mCurrentIndex : tmp;
            mCurrentIndex = tmp > mCurrentIndex ? mCurrentIndex : tmp;
//     }
        }
//     else {
        else {
//         if (mCurrentIndex <= 0) return;
            if (mCurrentIndex <= 0) return;
//         mCurrentIndex--;
            mCurrentIndex--;
//         mCurrentIndex = mCurrentIndex > 0 ? mCurrentIndex : 0;
            mCurrentIndex = mCurrentIndex > 0 ? mCurrentIndex : 0;
//     }
        }

//     SetCurrentPhoto();
        SetCurrentPhoto();
// }
    }

// void CPhotoActivity::MyImageLoaded(
    function MyImageLoaded(
//     /* [in] */ IDrawable* imageDrawable,
        imageDrawable,
//     /* [in] */ IImageView* imageView,
        imageView,
//     /* [in] */ const String& path,
        path,
//     /* [in] */ Boolean isHigh)
        isHigh)
// {
    {
//     Logger::D(TAG, "MyImageLoaded()-----path:%s,isHigh:%d", path.string(), isHigh);
        elog(TAG + "MyImageLoaded()-----path:"+path+",isHigh:"+isHigh);
//     if (!mCurrentImagePath.Equals(path)) {
        if (mCurrentImagePath != path) {
//         Logger::W(TAG, "MyImageLoaded()---not current path, drop!");
            elog(TAG + "MyImageLoaded()---not current path, drop!");
//         return;
            return;
//     }
        }

//     SwitchDisplayView(FALSE);
        SwitchDisplayView(false);
//     Int32 w = 0;
//     Int32 h = 0;
        var r = {w:0,h:0};

//     if (isHigh) {
        if (isHigh) {
//         Boolean ret = AsyncImageLoader::GetOrigionWidthAndHeight(path, &w, &h);
            var ret = AsyncImageLoader.GetOrigionWidthAndHeight(path,r);
//         if (!ret) {
            if (!ret) {
//             Logger::W(TAG, "MyImageLoaded()---GetOrigionWidthAndHeight---failed!");
                elog(TAG + "MyImageLoaded()---GetOrigionWidthAndHeight---failed!");
//             imageDrawable->GetIntrinsicWidth(&w);
                r.w = imageView.GetIntrinsicWidth();
//             imageDrawable->GetIntrinsicHeight(&h);
                r.h = imageView.GetIntrinsicHeight();
//         }
            }
//     }
        }
//     else {
        else {
//         imageDrawable->GetIntrinsicWidth(&w);
            r.w = imageDrawable.GetIntrinsicWidth();
//         imageDrawable->GetIntrinsicHeight(&h);
            r.h = imageDrawable.GetIntrinsicHeight();
//     }
        }

//     Logger::D(TAG, "MyImageLoaded()---w:%d,h:%d", w, h);
        elog(TAG + "MyImageLoaded()---w:"+r.w+",h:"+r.h);
//     (*mDetailsArray)[2] = StringUtils::Int32ToString(w);
        mDetailsArray[2] = r.w+'';
//     (*mDetailsArray)[3] = StringUtils::Int32ToString(h);
        mDetailsArray[3] = r.h+'';
//     imageView->SetImageDrawable(imageDrawable);
        imageView.SetImageDrawable(imageDrawable);
// }
    }

// void CPhotoActivity::SetDetailInfo(
    function SetDetailInfo(
//     /* [in] */ const String& filePath)
        filePath)
// {
    {
//     (*mDetailsArray)[0] = filePath.Substring(filePath.LastIndexOf(DataSourceHelper::PATH_SPLITE) + 1,
//         filePath.LastIndexOf("."));
        mDetailsArray[0] = filePath.substring(filePath.lastIndexOf(DataSourceHelper.PATH_SPLITE) + 1,
            filePath.lastIndexOf("."));
//     (*mDetailsArray)[5] = filePath;
        mDetailsArray[5] = filePath;
//     Logger::D(TAG, "SetDetailInfo()---filePath:%s", filePath.string());
        elog(TAG, "SetDetailInfo()---filePath:" + filePath);
//     AutoPtr<IFile> file;
        var file;
//     CFile::New(filePath, (IFile**)&file);
        file = Core_New("Elastos.IO.CFile", filePath);
//     if (file != NULL) {
        if (file) {
//         Int64 last = 0;
            var last = 0;
//         file->LastModified(&last);
            last = file.GetLastModified();
//         AutoPtr<IDate> date;
            var data;
//         CDate::New(last, (IDate**)&date);
            date = Core_New("Elastos.Utility.CDate",last);
//         AutoPtr<ISimpleDateFormat> sdf;
            var sdf;
//         CSimpleDateFormat::New(String("yyyy-MM-dd HH:mm"), (ISimpleDateFormat**)&sdf);
            sdf = Core_New("Elastos.Text.CSimpleDateFormat","yyyy-MM-dd HH:mm");
//         String str;
            var str;
//         sdf->FormatDate(date, &str);
            str = sdf.Format(date);
//         (*mDetailsArray)[1] = str;
            mDetailsArray[1] = str;
//         Int64 len = 0;
            var len = 0;
//         file->GetLength(&len);
            var len = file.GetLength();
//         StringBuilder sizeStr;
            var sizeStr;
//         Int32 n = len / 1024;
            var n = len / 1024;
//         if (n > 0) {
            if (n > 0) {
//             Int32 i = len / (1024 * 1024);
                var i = len / (1024 * 1024);
//             if (i > 0) {
                if (i > 0) {
//                 Float j = (((Float)(len % (1024 * 1024))) / (1024 * 1024)) * 10;
                    var j = (len % (1024 * 1024) / (1024 * 1024)) * 10;
//                 Int32 m = Elastos::Core::Math::Round(j);
                    var m = Math.round(j);
//                 sizeStr += StringUtils::Int32ToString(i);
                    sizeStr += i;
//                 sizeStr += ".";
                    sizeStr += ".";
//                 sizeStr += StringUtils::Int32ToString(m);
                    sizeStr += m;
//                 sizeStr += "MB";
                    sizeStr += "MB";
//             }
                }
//             else {
                else {
//                 Float j = (((Float)(len % 1024)) / 1024) * 10;
                    var j = ((len % 1024) / 1024) * 10;
//                 Int32 m = Elastos::Core::Math::Round(j);
                    var m = Math.round(j);
//                 sizeStr += StringUtils::Int32ToString(n);
                    sizeStr += n;
//                 sizeStr += ".";
                    sizeStr += ".";
//                 sizeStr += StringUtils::Int32ToString(m);
                    sizeStr += m;
//                 sizeStr += "KB";
                    sizeStr += "KB";
//             }
                }
//         }
            }
//         else {
            else {
//             sizeStr += StringUtils::Int64ToString(len);
                sizeStr += len;
//             sizeStr += "B";
                sizeStr += "B";
//         }
            }
//         (*mDetailsArray)[4] = sizeStr.ToString();
            mDetailsArray[4] = sizeStr;
//     }
        }
// }
    }

// void CPhotoActivity::LoadCurrentImage(
    function LoadCurrentImage(
//     /* [in] */ const String& path,
        path,
//     /* [in] */ Boolean isHighQuality)
        isHighQuality)
// {
    {
//     Logger::D(TAG, "LoadCurrentImage()---isHighQuality:%d", isHighQuality);
        elog([
            TAG,
            "LoadCurrentImage()---",
            JSON.stringify({
                isHighQuality:isHighQuality,
            }),
        ].join(""));
//     SwitchDisplayView(FALSE);
        SwitchDisplayView(false);
//     if (isHighQuality) {
        if (isHighQuality) {
//         AutoPtr<IWeakReference> weakHost;
            var weakHost;
//         GetWeakReference((IWeakReference**)&weakHost);
            var weakHost = oActivity.GetWeakReference();
//         assert(weakHost != NULL);
//         AutoPtr<MyLoadImageCallback> myLoadImage = new MyLoadImageCallback(weakHost, path, TRUE);
            var myLoadImage = new MyLoadImageCallback(weakHost, path, true);
//         AutoPtr<IBitmapDrawable> drawable = AsyncImageLoader::LoadDrawable(path, TRUE, mPhotoView, myLoadImage);
            var drawable = AsyncImageLoader.LoadDrawable(path, true, mPhotoView, myLoadImage);
//         if (drawable != NULL) {
            if (drawable) {
//             mPhotoView->SetImageDrawable(IDrawable::Probe(drawable));
                mPhotoView.SetImageDrawable(drawable);
//             Int32 w = 0;
//             Int32 h = 0;
                var r = {w:0, h:0};
//             Boolean ret = AsyncImageLoader::GetOrigionWidthAndHeight(path, &w, &h);
                var ret = AsyncImageLoader.GetOrigionWidthAndHeight(path, r);
//             if (ret) {
                if (ret) {
//                 Logger::D(TAG, "LoadCurrentImage()---HighQuality---w:%d,h:%d", w, h);
                    elog([
                        TAG,
                        "LoadCurrentImage()---HighQuality---",
                        JSON.stringify(r),
                    ].join(""));
//                 (*mDetailsArray)[2] = StringUtils::Int32ToString(w);
                    mDetailsArray[2] = r.w + '';
//                 (*mDetailsArray)[3] = StringUtils::Int32ToString(h);
                    mDetailsArray[3] = r.h + '';
//             }
                }
//             else {
                else {
//                 Logger::W(TAG, "LoadCurrentImage()---GetOrigionWidthAndHeight--failed!");
                    elog(TAG + "LoadCurrentImage()---GetOrigionWidthAndHeight--failed!");
//             }
                }
//         }
            }
//         else {
            else {
//             SwitchDisplayView(TRUE);
                SwitchDisplayView(true);
//         }
            }
//     }
        }
//     else {
        else {
//         AutoPtr<IWeakReference> weakHost;
            var weakHost;
//         GetWeakReference((IWeakReference**)&weakHost);
            weakHost = oActivity.GetWeakReference();
//         assert(weakHost != NULL);
//         AutoPtr<MyLoadImageCallback> myLoadImage = new MyLoadImageCallback(weakHost, path, FALSE);
            var myLoadImage = new MyLoadImageCallback(weakHost, path, false);
//         AutoPtr<IBitmapDrawable> drawable = AsyncImageLoader::LoadDrawable(path, FALSE, mPhotoView, myLoadImage);
            var drawable = AsyncImageLoader.LoadDrawable(path, false, mPhotoView, myLoadImage);
//         if (drawable != NULL) {
            if (drawable) {
//             mPhotoView->SetImageDrawable(IDrawable::Probe(drawable));
                mPhotoView.SetImageDrawable(drawable);
//             Int32 w = 0;
                var w = 0;
//             Int32 h = 0;
                var h = 0;
//             drawable->GetIntrinsicWidth(&w);
                w = drawable.GetIntrinsicWidth();
//             drawable->GetIntrinsicHeight(&h);
                h = drawable.GetIntrinsicHeight();
//             Logger::D(TAG, "LoadCurrentImage()---w:%d,h:%d", w, h);
                elog (TAG + "LoadCurrentImage()---w:"+w+",h:"+h);
                elog([
                    TAG,
                    "LoadCurrentImage()---",
                    JSON.stringify({
                        w:w,
                        h:h,
                    }),
                ].join(""));
//             (*mDetailsArray)[2] = StringUtils::Int32ToString(w);
                mDetailsArray[2] = w + '';
//             (*mDetailsArray)[3] = StringUtils::Int32ToString(h);
                mDetailsArray[3] = h + '';
//         }
            }
//         else {
            else {
//             SwitchDisplayView(TRUE);
                SwitchDisplayView(true);
//         }
            }
//     }
        }
// }
    }

// void CPhotoActivity::SwitchDisplayView(
    function SwitchDisplayView(
//     /* [in] */ Boolean showLoading)
        showLoading)
// {
    {
//     if (showLoading) {
        if (showLoading) {
//         mPhotoView->SetVisibility(IView::INVISIBLE);
            mPhotoView.SetVisibility(IView__INVISIBLE);
//         mPhotoLoadingText->SetVisibility(IView::VISIBLE);
            mPhotoLoadingText.SetVisibility(IView__VISIBLE);
//         TriggerLoadingCallback(FALSE);
            TriggerLoadingCallback(false);
//     }
        }
//     else {
        else {
//         mPhotoLoadingText->SetVisibility(IView::INVISIBLE);
            mPhotoLoadingText.SetVisibility(IView__INVISIBLE);
//         mPhotoView->SetVisibility(IView::VISIBLE);
            mPhotoView.SetVisibility(IView__VISIBLE);
//         UpdateLoadingStatus(TRUE);
            UpdateLoadingStatus(true);
//     }
        }
// }
    }

// void CPhotoActivity::UpdateLoadingStatus(
    function UpdateLoadingStatus(
//     /* [in] */ Boolean needReload)
        needReload)
// {
    {
//     AutoPtr<ICharSequence> cs;
        var cs;
//     StringBuilder newText("图片加载中 ");
        var newText = "图片加载中 ";
//     if (needReload) {
        if (needReload) {
//         TriggerLoadingCallback(TRUE);
            TriggerLoadingCallback(true);
//         newText += "...";
            newText += "...";
//         cs = newText.ToCharSequence();
            cs = CString(newText);
//         mPhotoLoadingText->SetText(cs);
            mPhotoLoadingText.SetText(cs);
//     }
        }
//     else {
        else {
//         Int32 ret = IView::GONE;
            var ret = IView__GONE;
//         mPhotoLoadingText->GetVisibility(&ret);
            ret = mPhotoLoadingText.GetVisibility();
//         if (ret == IView::VISIBLE) {
            if (ret == IView__VISIBLE) {
//             mPhotoLoadingText->GetText((ICharSequence**)&cs);
                var cs = mPhotoLoadingText.GetText();
//             String oldText;
                var oldText;
//             cs->ToString(&oldText);
                oldText = cs.ToString();
//             Int32 firstIndex = oldText.IndexOf(".");
                var firstIndex = oldText.indexOf(".");
//             Int32 lastIndex = oldText.LastIndexOf(".");
                var lastIndex = oldText.lastIndexOf(".");
//             Int32 num = lastIndex - firstIndex;
                var num = lastIndex - firstIndex;
//             Logger::D(TAG, "UpdateLoadingStatus()---oldText:%s,firstIndex:%d,num:%d", oldText.string(), firstIndex, num);
                elog(TAG + "UpdateLoadingStatus()---oldText:" + oldText + ",firstIndex:" + firstIndex + ",num:" + num);
//             if (num == 0) {
                if (num == 0) {
//                 if (firstIndex > 0) {
                    if (firstIndex > 0) {
//                     newText += ".. ";
                        newText += "..";
//                 }
                    }
//                 else {
                    else {
//                     newText += ".  ";
                        newText += ".  ";
//                 }
                    }
//             }
                }
//             else if (num == 1) {
                else if (num == 1) {
//                 newText += "...";
                    newText += "...";
//             }
                }
//             else if (num == 2) {
                else if (num == 2) {
//                 newText += "   ";
                    newText += "   ";
//             }
                }
//             cs = newText.ToCharSequence();
                cs = CString(newText);
//             mPhotoLoadingText->SetText(cs);
                mPhotoLoadingText.SetText(cs);
//             TriggerLoadingCallback(FALSE);
                TriggerLoadingCallback(false);
//         }
            }
//         else {
            else {
//             Logger::D(TAG, "UpdateLoadingStatus()---stop:%d", ret);
                elog(TAG + "UpdateLoadingStatus()---stop:" + ret);
//         }
            }
//     }
        }
// }
    }

// void CPhotoActivity::TriggerLoadingCallback(
    function TriggerLoadingCallback(
//     /* [in] */ Boolean isCancel)
        isCancel)
// {
    {
//     Logger::D(TAG, "TriggerLoadingCallback()---isCancel:%d", isCancel);
        elog(TAG + "TriggerLoadingCallback()---isCancel:" + isCancel);
//     if (isCancel) {
        if (isCancel) {
//         mHandler->RemoveMessages(MSG_UPDATE_LOADING_STATUS);
            //TODO:
            //mHandler.RemoveMessages(MSG_UPDATE_LOADING_STATUS);
//     }
        }
//     else {
        else {
//         Boolean needReload = FALSE;
            var needReload = false;
//         AutoPtr<IMessage> msg;
            var msg;
//         mMyHandler->ObtainMessageEx2(MSG_UPDATE_LOADING_STATUS,
//             needReload ? 1 : 0, 0, (IMessage**)&msg);
            //TODO:
            //msg = mMyHandler.ObtainMessageEx2(MSG_UPDATE_LOADING_STATUS,
            //    needReload ? 1 : 0, 0);
            msg = {what:MSG_UPDATE_LOADING_STATUS, arg1:(needReload ? 1 : 0), arg2:0};
//         Boolean result;
//         mMyHandler->SendMessageDelayed(msg, 1000, &result);
            //TODO:
            //var result = mMyHandler.SendMessageDelayed(msg, 1000);
            mMyHandler.HandleMessage(msg);
//     }
        }
// }
    }

// ECode CPhotoActivity::OnNewIntent(
    _apt.OnNewIntent = function(context,
//     /* [in] */ IIntent *intent)
        intent)
// {
    {
        elog(TAG + "====OnNewIntent.begin====");
//     mPhotoEntryList.Clear();
        mPhotoEntryList = [];
//     if (!LoadPhotoEntryList(intent))
        if (!LoadPhotoEntryList(intent)) {
//         return NOERROR;
            return;
        }

//     mUpButton->SetVisibility(mViewSingleItem ? IView::INVISIBLE : IView::VISIBLE);
        mUpButton.SetVisibility(mViewSingleItem ? IView__INVISIBLE : IView__VISIBLE);
//     mPhotoSlideshow->SetVisibility(mViewSingleItem ? IView::INVISIBLE : IView::VISIBLE);
        mPhotoSlideshow,SetVisibility(mViewSingleItem ? IView__INVISIBLE : IView__VISIBLE);

//     SetCurrentPhoto();
        SetCurrentPhoto();
//     return NOERROR;
        return;
// }
    }

// Boolean CPhotoActivity::LoadPhotoEntryList(
    function LoadPhotoEntryList(
//     /* [in] */ IIntent* intent)
        intent)
// {
    {
//     if (intent != NULL) {
        //if (intent) {
        if (true) {
//         String action;
            var action;
//         String filePath;
            var filePath;
if (!_debug) {
//         intent->GetAction(&action);
            action = intent.GetAction();
//         Logger::D(TAG, "action:%s", action.string());
            elog(TAG + "action:" + action);
//         AutoPtr<IUri> uri;
            var uri;
//         intent->GetData((IUri**)&uri);
            uri = intent.GetData();
}
            //var uri = Droid_New("Elastos.Droid.Net.CStringUri", "file:///data/temp/testdisk/icon_03.jpg");
//         if (uri != NULL) {
            //if (uri) {
            if (false) {
//             String scheme;
//             uri->GetScheme(&scheme);
                var scheme = uri.GetScheme();
//             if (scheme.Equals("file")) {
                if (scheme == "file") {
//                 uri->GetPath(&filePath);
                    filePath = uri.GetPath();
//                 Boolean isImage = DataSourceHelper::IsImageFile(filePath);
                    var isImage = DataSourceHelper.IsImageFile(filePath);
//                 if (isImage) {
                    if (isImage) {
//                     AutoPtr<PhotoEntry> entry = new PhotoEntry();
                        var entry = new PhotoEntry();
//                     entry->sourcePath = filePath;
                        entry.sourcePath = filePath;
//                     entry->desc = filePath.Substring(filePath.LastIndexOf(DataSourceHelper::PATH_SPLITE) + 1,
//                         filePath.GetLength());
                        //entry.desc = filePath.split(DataSourceHelper.PATH_SPLITE).pop();
                        entry.desc = filePath.substring(filePath.lastIndexOf(DataSourceHelper.PATH_SPLITE) + 1, filePath.length);
//                     Logger::D(TAG, "single photo, path:%s, desc:%s", entry->sourcePath.string(), entry->desc.string());
                        elog(TAG + "single photo, path:"+entry.sourcePath+", desc:"+entry.desc);
//                     mPhotoEntryList.PushBack(entry);
                        mPhotoEntryList.push(entry);
//                     mCurrentIndex = 0;
                        mCurrentIndex = 0;
//                     mTotalSize = mPhotoEntryList.GetSize();
                        mTotalSize = mPhotoEntryList.length;
//                     mViewSingleItem = TRUE;
                        mViewSingleItem = true;
//                 }
                    }
//                 else {
                    else {
//                     Logger::W(TAG, "image type is not support! filePath:%s", filePath.string());
                        elog(TAG + "image type is not support! filePath:"+filePath);
//                     Utils::MakeToast(this, String("不支持显示该类型的文件！"), IToast::LENGTH_LONG);
                        Utils.MakeToast(oActivity,"不支持显示该类型的文件！");
//                     Finish();
                        oActivity.Finish();
//                     return FALSE;
                        return false;
//                 }
                    }
//             }
                }
//             else {
                else {
//                 Logger::W(TAG, "scheme is not a file! scheme:%s", scheme.string());
                    elog(TAG + "scheme is not a file! scheme:" + scheme);
//             }
                }
//         }
            }
//         else {
            else {
//             intent->GetStringExtra(DataSourceHelper::SOURCE_PATH, &filePath);
if (_debug) {
                filePath = "/data/temp/testdisk/icon_01.jpeg";
}
else {
                filePath = intent.GetStringExtra(DataSourceHelper.SOURCE_PATH);
}
//             if (!filePath.IsNullOrEmpty()) {
                if (filePath.length) {
//                 intent->GetInt32Extra(DataSourceHelper::SOURCE_INDEX, 0, &mCurrentIndex);
if (_debug) {
                    mCurrentIndex = 0;
}
else {
                    mCurrentIndex = intent.GetInt16Extra(DataSourceHelper.SOURCE_INDEX, 0);
}
//                 mFolderPath = filePath.Substring(0, filePath.LastIndexOf(DataSourceHelper::PATH_SPLITE));
                    mFolderPath = filePath.substring(0, filePath.lastIndexOf(DataSourceHelper.PATH_SPLITE));
//                 AutoPtr<List<String> > imageItems = DataSourceHelper::GetItemList(mFolderPath);
                    var imageItems = DataSourceHelper.GetItemList(mFolderPath);
//                 if (imageItems != NULL) {
                    if (imageItems) {
//                     AutoPtr<PhotoEntry> entry;
                        var entry;
//                     List<String>::Iterator it = imageItems->Begin();
//                     for (Int32 i = 0; it != imageItems->End(); ++it, ++i) {
                        mPhotoEntryList = [];
                        for (var i=0,im=imageItems.length;i<im;i++) {
//                         entry = new PhotoEntry();
                            entry = {};
//                         entry->sourcePath = mFolderPath;
                            entry.sourcePath = mFolderPath;
//                         entry->sourcePath += DataSourceHelper::PATH_SPLITE;
                            entry.sourcePath += DataSourceHelper.PATH_SPLITE;
//                         entry->sourcePath += *it;
                            entry.sourcePath += imageItems[i]
//                         entry->desc = (*it).Substring(0, (*it).GetLength());
                            entry.desc = imageItems[i];
//                         Logger::D(TAG, " > %d, path:%s, desc:%s", i, entry->sourcePath.string(), entry->desc.string());
                            elog(TAG + " > "+i+", path:"+entry.sourcePath+", desc:"+entry.desc);
//                         mPhotoEntryList.PushBack(entry);
                            mPhotoEntryList.push(entry);
//                     }
                        }
//                     mTotalSize = mPhotoEntryList.GetSize();
                        mTotalSize = mPhotoEntryList.length;
//                     mViewSingleItem = FALSE;
                        mViewSingleItem = false;
//                 }
                    }
//                 else {
                    else {
//                     Logger::W(TAG, "imageItems is null!");
                        elog(TAG + "imageItems is null!");
//                 }
                    }
//             }
                }
//         }
            }
//     }
        }

//     if (mPhotoEntryList.IsEmpty()) {
        if (!mPhotoSlideshow) {
//         Logger::W(TAG, "mPhotoEntryList size is zero!");
            elog(TAG + "mPhotoSlideshow size is zero!");
//         Utils::MakeToast(this, String("没有图片文件！"), IToast::LENGTH_LONG);
            Utils.MakeToast(oActivity, "没有图片文件！", IToast__LENGTH_LONG);
//         Finish();
            oActivity.Finish();
//         return FALSE;
            return false;
//     }
        }

//     return TRUE;
        return true;
// }
    }

// } // namespace Gallery

//--------CxxxActivity.cpp----end----

//--------CActivityListener----default begin----

    // _apt.OnCreate = function(context, savedInstanceState){
    //     elog('====jso_activity_cb====OnCreate.begin====');
    // }  //OnCreate
    _apt.OnStart = function(context){
        timeOnStart = new Date().getTime();
        var t = timeOnStart - timeOnCreate;

        elog('====jso_activity_cb====OnStart.begin====time:'+t);
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