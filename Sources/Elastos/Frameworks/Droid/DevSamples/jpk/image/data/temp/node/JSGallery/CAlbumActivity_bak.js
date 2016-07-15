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

// #ifndef __CALBUMACTIVITY_H__
// #define __CALBUMACTIVITY_H__

// #include "_CAlbumActivity.h"
// #include "app/Activity.h"
// #include "src/data/AsyncImageLoader.h"
    var AsyncImageLoader = require("./data/AsyncImageLoader.js");
// #include "os/HandlerBase.h"

// using Elastos::Droid::Os::HandlerBase;
// using Elastos::Core::IRunnable;
// using Elastos::Droid::App::Activity;
// using Elastos::Droid::Os::IHandler;
// using Elastos::Droid::View::IViewOnClickListener;
// using Elastos::Droid::View::IViewOnTouchListener;
// using Elastos::Droid::Widget::IAdapterView;
// using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
// using Elastos::Droid::Widget::IArrayAdapter;
// using Elastos::Droid::Widget::IButton;
// using Elastos::Droid::Widget::ICheckBox;
// using Elastos::Droid::Widget::ICompoundButton;
// using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
// using Elastos::Droid::Widget::IGridView;
// using Elastos::Droid::Widget::IImageButton;
// using Elastos::Droid::Widget::IImageView;
// using Elastos::Droid::Widget::IListView;
// using Elastos::Droid::Widget::ILinearLayout;
// using Elastos::Droid::Widget::IPopupWindow;
// using Elastos::Droid::Widget::IPopupWindowOnDismissListener;
// using Elastos::Droid::Widget::IProgressBar;
// using Elastos::Droid::Widget::ISimpleAdapter;
// using Elastos::Droid::Widget::ISimpleAdapterViewBinder;
// using Elastos::Droid::Widget::ITextView;

// namespace Gallery {

// class CAlbumActivity
//     : public Activity
// {
// private:
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
//         , public IAdapterViewOnItemClickListener
//         , public IPopupWindowOnDismissListener
//         , public IViewOnTouchListener
//         , public ICompoundButtonOnCheckedChangeListener
//     {
//     public:
//         MyListener(
//             /* [in] */ CAlbumActivity* host);

//         CAR_INTERFACE_DECL();

//         CARAPI OnClick(
//             /* [in] */ IView* v);

//         CARAPI OnItemClick(
//             /* [in] */ IAdapterView* parent,
//             /* [in] */ IView* view,
//             /* [in] */ Int32 position,
//             /* [in] */ Int64 id);

//         CARAPI OnDismiss();

//         CARAPI OnTouch(
//             /* [in] */ IView* v,
//             /* [in] */ IMotionEvent* event,
//             /* [out] */ Boolean* result);

//         CARAPI OnCheckedChanged(
//             /* [in] */ ICompoundButton* buttonView,
//             /* [in] */ Boolean isChecked);

//     private:
//         CAlbumActivity* mHost;
//     };
        function MyListener() {};

//     class MyViewBinder
//         : public ElRefBase
//         , public ISimpleAdapterViewBinder
//     {
//     public:
//         MyViewBinder(
//             /* [in] */ CAlbumActivity* host);

//         CAR_INTERFACE_DECL();

//         CARAPI SetViewValue(
//             /* [in] */ IView* view,
//             /* [in] */ IInterface* data,
//             /* [in] */ const String& textRepresentation,
//             /* [out] */ Boolean* result);

//     private:
//         CAlbumActivity* mHost;
//     };
        function MyViewBinder() {};

//     class LoadImageInfoRunnable
//         : public ElRefBase
//         , public IRunnable
//     {
//     public:
//         LoadImageInfoRunnable(
//             /* [in] */ IWeakReference* host);

//         CAR_INTERFACE_DECL();

//         CARAPI Run();

//     private:
//         AutoPtr<IWeakReference> mWeakHost;
//     };
        function LoadImageInfoRunnable() {};

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
        function MyLoadImageCallback() {};

//     class AlbumEntry
//         : public ElRefBase
//     {
//     public:
//         String sourcePath;
//         String desc;
//         String num;
//     };
        function AlbumEntry() {};

// friend class MyListener;
// friend class MyViewBinder;
// friend class LoadImageInfoRunnable;
// friend class MyLoadImageCallback;

// public:
//     CAlbumActivity();
//     ~CAlbumActivity();

//     CARAPI_(void) MyImageLoaded(
//         /* [in] */ IDrawable* imageDrawable,
//         /* [in] */ IImageView* imageView);

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
//     CARAPI_(AutoPtr<ISimpleAdapter>) GetSimpleAdapter();

//     CARAPI_(AutoPtr<IArrayAdapter>) GetArrayAdapter();

//     CARAPI OpenAlbumPopupWindow(
//         /* [in] */ IView* v);

//     CARAPI CloseAlbumPopupWindow();

//     CARAPI_(void) LoadImageInfos();

//     CARAPI_(void) ShowProgressDialog(
//         /* [in] */ Boolean show);

//     CARAPI_(void) LoadAlbumView();

// private:
//     static const String TAG;
        var TAG;
//     static const Int32 MSG_IMAGE_LOADED;
        var MSG_IMAGE_LOADED;
//     static const Int32 MSG_LOAD_ALBUM_VIEW;
        var MSG_LOAD_ALBUM_VIEW;

// private:
//     Boolean mShowAlbum;
        var mShowAlbum;

//     AutoPtr<MyListener> mMyListener;
        var mMyListener;
//     AutoPtr<ITextView> mAboutButton;
        var mAboutButton;
//     AutoPtr<IButton> mCheckAll;
        var mCheckAll;
//     // AutoPtr<IButton> mDetailButton;
        var mDetailButton;
//     AutoPtr<ILinearLayout> mDetailLayout;
        var mDetailLayout;
//     AutoPtr<IImageButton> mDeleteButton;
        var mDeleteButton;
//     AutoPtr<IButton> mPopupButton;
        var mPopupButton;
//     AutoPtr<IListView> mListView;
        var mListView;
//     AutoPtr<IGridView> mGridView;
        var mGridView;
//     AutoPtr<ITextView> mEmptyView;
        var mEmptyView;
//     AutoPtr<IPopupWindow> mAlbumPopupWindow;
        var mAlbumPopupWindow;
//     AutoPtr<ISimpleAdapter> mSimpleAdapter;
        var mSimpleAdapter;
//     AutoPtr<IArrayAdapter> mArrayAdapter;
        var mArrayAdapter;
//     AutoPtr<IHandler> mHandler;
        var mHandler;
//     AutoPtr<IProgressBar> mProgressBar;
        var mProgressBar;
//     List< AutoPtr<AlbumEntry> > mAlbumEntryList;
        var mAlbumEntryList = [];
//     AutoPtr<IHandler> mMyHandler;
        var mMyHandler;
// };

// } // namespace Gallery

// #endif // __CALBUMACTIVITY_H__

//--------CxxxActivity.h----end----

//--------CxxxActivity.cpp----begin----

// #include "CAlbumActivity.h"
// #include "src/data/BucketHelper.h"
    var BucketHelper = require("./data/BucketHelper.js");
// #include "src/data/DataSourceHelper.h"
    var DataSourceHelper = require("./data/DataSourceHelper.js");
// #include "src/data/AsyncImageLoader.h"
// #include "R.h"
// #include <elastos/Logger.h>
// #include <elastos/StringBuilder.h>
// #include <elastos/StringUtils.h>
// #include "os/SomeArgs.h"

// using Elastos::Core::CBoolean;
// using Elastos::Core::IBoolean;
// using Elastos::Core::EIID_IBoolean;
// using Elastos::Core::ICharSequence;
// using Elastos::Core::EIID_ICharSequence;
// using Elastos::Core::StringBuilder;
// using Elastos::Core::StringUtils;
// using Elastos::Core::CInteger32;
// using Elastos::Core::IInteger32;
// using Elastos::Core::EIID_IInteger32;
// using Elastos::Core::CStringWrapper;
// using Elastos::Core::EIID_IRunnable;
// using Elastos::Core::Threading::CThread;
// using Elastos::Core::Threading::IThread;

// using Elastos::IO::CFile;
// using Elastos::IO::IFile;

// using Elastos::Droid::App::IActionBar;
// using Elastos::Droid::App::IIActivityManager;
// using Elastos::Droid::App::CProgressDialog;
// using Elastos::Droid::Content::CIntent;
// using Elastos::Droid::Content::IContext;
// using Elastos::Droid::Content::IIntent;
// using Elastos::Droid::Content::Pm::IPackageInfo;
// using Elastos::Droid::Graphics::IColor;
// using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
// using Elastos::Droid::Graphics::Drawable::CColorDrawable;
// using Elastos::Droid::Graphics::Drawable::EIID_IDrawable;
// using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
// using Elastos::Droid::Graphics::Drawable::IColorDrawable;
// using Elastos::Droid::Graphics::Drawable::IDrawable;
// using Elastos::Droid::Location::ILocation;
// using Elastos::Droid::Location::ILocationManager;
// using Elastos::Droid::Os::CServiceManager;
// using Elastos::Droid::Os::IServiceManager;
// using Elastos::Droid::Os::SomeArgs;
// using Elastos::Droid::View::IWindow;
// using Elastos::Droid::View::EIID_IViewOnClickListener;
// using Elastos::Droid::View::EIID_IViewOnTouchListener;
// using Elastos::Droid::View::IViewParent;
// using Elastos::Droid::Widget::IGridView;
// using Elastos::Droid::Widget::IAdapter;
// using Elastos::Droid::Widget::IBaseAdapter;
// using Elastos::Droid::Widget::IListAdapter;
// using Elastos::Droid::Widget::IImageView;
// using Elastos::Droid::Widget::ICheckable;
// using Elastos::Droid::Widget::CArrayAdapter;
// using Elastos::Droid::Widget::CSimpleAdapter;
// using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
// using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
// using Elastos::Droid::Widget::CPopupWindow;
// using Elastos::Droid::Widget::EIID_IPopupWindowOnDismissListener;
// using Elastos::Droid::Widget::EIID_ISimpleAdapterViewBinder;
// using Elastos::Droid::Widget::IRelativeLayout;
// using Elastos::Droid::App::EIID_IActivity;
// using Elastos::Droid::Utility::CParcelableObjectContainer;

// using Elastos::Utility::CObjectStringMap;
// using Elastos::Utility::IObjectStringMap;
// using Elastos::Utility::Logging::Logger;

// namespace Gallery {

// const String CAlbumActivity::TAG("CAlbumActivity");
    TAG = "CAlbumActivity";
// const Int32 CAlbumActivity::MSG_IMAGE_LOADED = 0;
    MSG_IMAGE_LOADED = 0;
// const Int32 CAlbumActivity::MSG_LOAD_ALBUM_VIEW = 1;
    MSG_LOAD_ALBUM_VIEW = 1;

// ECode CAlbumActivity::MyHandler::HandleMessage(
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

//     CAlbumActivity* mHost = (CAlbumActivity*)strongObj.Get();

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
//         case CAlbumActivity::MSG_IMAGE_LOADED: {
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
                //TODO
//             break;
                break;
//         }
//         case CAlbumActivity::MSG_LOAD_ALBUM_VIEW: {
            case MSG_LOAD_ALBUM_VIEW:
//             mHost->LoadAlbumView();
                LoadAlbumView();
//             break;
                break;
//         }
//     }
        }

//     return NOERROR;
        return;
// }
    }

// CAlbumActivity::MyListener::MyListener(
//     /* [in] */ CAlbumActivity* host)
//     : mHost(host)
// {}

// PInterface CAlbumActivity::MyListener::Probe(
//     /* [in]  */ REIID riid)
// {
//     if (riid == EIID_IInterface) {
//         return (PInterface)(IViewOnClickListener*)this;
//     }
//     else if (riid == EIID_IViewOnClickListener) {
//         return (IViewOnClickListener*)this;
//     }
//     else if (riid == EIID_IAdapterViewOnItemClickListener) {
//         return (IAdapterViewOnItemClickListener*)this;
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

// UInt32 CAlbumActivity::MyListener::AddRef()
// {
//     return ElRefBase::AddRef();
// }

// UInt32 CAlbumActivity::MyListener::Release()
// {
//     return ElRefBase::Release();
// }

// ECode CAlbumActivity::MyListener::GetInterfaceID(
//     /* [in] */ IInterface *pObject,
//     /* [out] */ InterfaceID *pIID)
// {
//     if (pIID == NULL) {
//         return E_INVALID_ARGUMENT;
//     }

//     if (pObject == (IInterface*)(IViewOnClickListener*)this) {
//         *pIID = EIID_IViewOnClickListener;
//     }
//     else if (pObject == (IInterface*)(IAdapterViewOnItemClickListener*)this) {
//         *pIID = EIID_IAdapterViewOnItemClickListener;
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

// ECode CAlbumActivity::MyListener::OnClick(
    MyListener.prototype.OnClick = function(
//     /* [in] */ IView* v)
        v)
// {
    {
//     Int32 viewId = 0;
        var viewId =0;
//     v->GetId(&viewId);
        viewId = v.GetId();

//     switch(viewId) {
        switch(viewId) {
//         case R::id::album_btn_detail: {
            case R.id.album_btn_detail:
//             Logger::D(TAG, "OnClick()---album_btn_detail");
                elog(TAG + "OnClick()---album_btn_detail");
//             break;
                break;
//         }
//         case R::id::album_about: {
            case R.id.album_about:
//             Logger::D(TAG, "OnClick()---album_about");
                elog(TAG + "OnClick()---album_about");
//             AutoPtr<IIntent> intent;
                var intent;
//             CIntent::New((IIntent**)&intent);
                intent = Droid_New("Elastos.Droid.Content.CIntent");
//             intent->SetClassNameEx(String("Gallery"), String("Gallery.CAboutActivity"));
                intent.SetClassName("Elastos.DevSamples.Node.JSGallery", "Elastos.DevSamples.Node.JSGallery.CAboutActivity");
//             if (FAILED(mHost->StartActivity(intent))) {
                try { oActivity.StartActivity(intent); } catch(e) {
//                 Logger::E(TAG, "OnClick()---StartActivity CAboutActivity failed!");
                    elog(TAG + "OnClick()--StartActivity CAboutActivity failed!");
//             }
                }
//             break;
                break;
//         }
//         case R::id::album_check_all: {
            case R.id.album_check_all:
//             Logger::D(TAG, "OnClick()---album_check_all");
                elog(TAG + "OnClick()---album_check_all");
//             // Boolean isChecked = FALSE;
//             // ICheckable::Probe(v)->IsChecked(&isChecked);
//             // Logger::D("CAlbumActivity", "OnClick---check_all---isChecked:%d", isChecked);
//             // if (isChecked) {
//             //     mHost->mDetailButton->SetEnabled(TRUE);
//             //     mHost->mDeleteButton->SetEnabled(TRUE);
//             // }
//             // else {
//             //     mHost->mDetailButton->SetEnabled(FALSE);
//             //     mHost->mDeleteButton->SetEnabled(FALSE);
//             // }
//             break;
                break;
//         }
//         case R::id::album_btn_delete: {
            case R.id.album_btn_delete:
//             Logger::D(TAG, "OnClick()---album_btn_delete");
                elog(TAG + "OnClick()---album_btn_delete");
//             break;
                break;
//         }
//         case R::id::album_btn_popup: {
            case R.id.album_btn_popup:
//             Logger::D(TAG, "OnClick()---album_btn_popup");
                elog(TAG + "OnClick()---album_btn_popup");
//             if (mHost->mShowAlbum) {
                if (mShowAlbum) {
//                 mHost->CloseAlbumPopupWindow();
                    CloseAlbumPopupWindow();
//             }
                }
//             else {
                else {}
//                 mHost->OpenAlbumPopupWindow(v);
                    OpenAlbumPopupWindow();
//             }
                }
//             break;
                break;
//         }
//         case R::id::pop_album: {
            case R.id.pop_album:
//             Logger::D(TAG, "OnClick()---pop_album");
                elog(TAG + "OnClick()---pop_album");
//             mHost->CloseAlbumPopupWindow();
                CloseAlbumPopupWindow();
//             break;
                break;
//         }
//         case R::id::pop_time: {
            case R.id.pop_time:
//             Logger::D(TAG, "OnClick()---pop_time");
                elog(TAG + "OnClick()---pop_time");
//             mHost->CloseAlbumPopupWindow();
                CloseAlbumPopupWindow();
//             break;
                break;
//         }
//         case R::id::pop_location: {
            case R.id.pop_location:
//             Logger::D(TAG, "OnClick()---pop_location");
                elog(TAG + "OnClick()---pop_location");
//             mHost->CloseAlbumPopupWindow();
                CloseAlbumPopupWindow();
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

// ECode CAlbumActivity::MyListener::OnItemClick(
    MyListener.prototype.OnItemClick = function(
//     /* [in] */ IAdapterView* parent,
        parent,
//     /* [in] */ IView* view,
        view,
//     /* [in] */ Int32 position,
        position,
//     /* [in] */ Int64 id)
        id)
// {
    {
//     Logger::D(TAG, "OnItemClick()---position = %d, id = %lld", position, id);
        elog(TAG + "OnItemClick()---position = "+position+", id = "+id);
//     return NOERROR;
        return;
// }
    }

// ECode CAlbumActivity::MyListener::OnDismiss()
    MyListener.prototype.OnDismiss = function()
// {
    {
//     Logger::D(TAG, "OnDismiss()---");
        elog(TAG + "OnDismiss()---");
//     mHost->mAlbumPopupWindow = NULL;
        mAlbumPopupWindow = null;
//     mHost->mShowAlbum = FALSE;
        mShowAlbum = false;
//     return NOERROR;
        return;
// }
    }

// ECode CAlbumActivity::MyListener::OnTouch(
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
//     AutoPtr<IViewParent> viewParent;
        var viewParent;
//     v->GetParent((IViewParent**)&viewParent);
        viewParent = v.GetParent();
//     AutoPtr<IRelativeLayout> itemLayout = IRelativeLayout::Probe(viewParent) ;
        var itemLayout = viewParent;
//     if (IMotionEvent::ACTION_DOWN == action) {
        if (IMotionEvent__ACTION_DOWN == action) {
//         Logger::D(TAG, "onTouch()--ACTION_DOWN");
            elog(TAG + "onTouch()--ACTION_DOWN");
//         itemLayout->SetBackgroundResource(R::drawable::tools_bg_d);
            itemLayout.SetBackgroundResource(R.drawable.tools_bg_d);
//     }
        }
//     else {
        else {
//         Logger::D(TAG, "onTouch()--ACTION:%d", action);
            elog(TAG + "onTouch()--ACTION:" + action);
//         AutoPtr<IResources> res;
            var res;
//         mHost->GetResources((IResources**)&res);
            res = oActivity.GetResources();
//         AutoPtr<IBitmapDrawable> bitmapDrawable;
            var bitmapDrawable;
//         CBitmapDrawable::New(res, (IBitmap*)NULL, (IBitmapDrawable**)&bitmapDrawable);
            bitmapDrawable = Droid_New("Elastos.Droid.Graphics.Drawable.CBitmapDrawable", res);
//         itemLayout->SetBackgroundDrawable(IDrawable::Probe(bitmapDrawable));
            itemLayout.SetBackgroundDrawable(bitmapDrawable);
//         AutoPtr<IViewParent> viewParent2;
            var viewParent2;
//         viewParent->GetParentEx((IViewParent**)&viewParent2);
            viewParent2 = viewParent.GetParent();
//         AutoPtr<IAdapterView> adapterView = IAdapterView::Probe(viewParent2);
            var adapterView = viewParent2;
//         Int32 index = 0;
            var index = 0;
//         adapterView->GetPositionForView(v, &index);
            index = adapterView.GetPositionForView(v);
//         Logger::D(TAG, "onTouch()--index:%d", index);
            elog(TAG + "onTouch()--index:" + index);
//         if (IMotionEvent::ACTION_UP == action) {
            if (IMotionEvent__ACTION_UP == action) {
//             AutoPtr<AlbumEntry> entry = mHost->mAlbumEntryList[index];
                var entry = mAlbumEntryList[index];
//             String folderPath = entry->sourcePath.Substring(0, entry->sourcePath.LastIndexOf(DataSourceHelper::PATH_SPLITE));
                var folderPath = entry.sourcePath.substring(0, entry.sourcePath.lastIndexOf(DataSourceHelper.PATH_SPLITE));
//             Logger::D(TAG, "onTouch()--ACTION_UP--path:%s", folderPath.string());
                elog(TAG + "onTouch()--ACTION_UP--path:" + folderPath.string());
//             AutoPtr<IIntent> intent;
                var intent;
//             CIntent::New((IIntent**)&intent);
                intent = Droid_New("Elastos.Droid.Content.CIntent");
//             intent->SetClassNameEx(String("Gallery"), String("Gallery.CBrowserActivity"));
                intent.SetClassName("Elastos.DevSamples.Node.JSGallery", "Elastos.DevSamples.Node.JSGallery.CBrowserActivity");
//             intent->PutStringExtra(DataSourceHelper::SOURCE_PATH, folderPath);
                intent.PutExtra(DataSourceHelper.SOURCE_PATH, folderPath);
//             intent->PutStringExtra(DataSourceHelper::SOURCE_DESC, entry->desc);
                intent.PutExtra(DataSourceHelper.SOURCE_DESC, entry.desc);
//             if (FAILED(mHost->StartActivity(intent))) {
                if (!oActivity.StartActivity(intent)) {
//                 Logger::E(TAG, "OnClick()---StartActivity CBrowserActivity failed!");
                    elog("OnClick()---StartActivity CBrowserActivity failed!");
//             }
                }
//         }
            }
//     }
        }
//     *result = TRUE;
        data.result = true;
//     return NOERROR;
        return;
// }
    }

// ECode CAlbumActivity::MyListener::OnCheckedChanged(
    MyListener.prototype.OnCheckedChanged= function(
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

// CAlbumActivity::MyViewBinder::MyViewBinder(
//     /* [in] */ CAlbumActivity* host)
//     : mHost(host)
// {}

// CAR_INTERFACE_IMPL(CAlbumActivity::MyViewBinder, ISimpleAdapterViewBinder)

// ECode CAlbumActivity::MyViewBinder::SetViewValue(
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
//     // Logger::D(TAG, "MyViewBinder---SetViewValue");
        elog(TAG + "MyViewBinder---SetViewValue");
//     VALIDATE_NOT_NULL(result)
//     *result = FALSE;
        result.data = false;

//     if (ILinearLayout::Probe(view) != NULL) {
        if (CObject.hasInterface(view, "ILinearLayout")) {
//         // Logger::D(TAG, "MyViewBinder---SetViewValue---SetOnTouchListener");
            elog(TAG + "MyViewBinder---SetViewValue---SetOnTouchListener");
//         AutoPtr<ILinearLayout> folderLayout = ILinearLayout::Probe(view);
            var folderlayout = view;
//         folderLayout->SetOnTouchListener((IViewOnTouchListener*)(mHost->mMyListener->Probe(EIID_IViewOnTouchListener)));
            folderLayout.SetOnTouchListener(mMyListener);
//         *result = TRUE;
            result.data = true;
//     }
        }
//     else if (IImageView::Probe(view) != NULL) {
        else if (CObject.hasInterface(view, "IImageView")) {
//         Logger::D(TAG, "MyViewBinder()---SetViewValue:%s", textRepresentation.string());
            elog(TAG + "MyViewBinder()---SetViewValue:" + textRepresentation);
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
//     return NOERROR;
        return;
// }
    }

// CAlbumActivity::LoadImageInfoRunnable::LoadImageInfoRunnable(
//     /* [in] */ IWeakReference* host)
//     : mWeakHost(host)
// {}

// CAR_INTERFACE_IMPL(CAlbumActivity::LoadImageInfoRunnable, IRunnable)

// ECode CAlbumActivity::LoadImageInfoRunnable::Run()
    LoadImageInfoRunnable.prototype.Run = function()
// {
    {
//     Logger::D(TAG, "Run()-----");
        elog(TAG + "Run()-----");

//     AutoPtr<IActivity> strongObj;
//     mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
//     if (strongObj == NULL) {
//         return NOERROR;
//     }

//     CAlbumActivity* mHost = (CAlbumActivity*)strongObj.Get();
//     if (mHost->mMyHandler == NULL) {
//         return NOERROR;
//     }

//     // DataSourceHelper::SetRootPath(DataSourceHelper::ROOT_PATH);
//     AutoPtr<HashMap<String, AutoPtr<List<String> > > > pathMap = DataSourceHelper::GetImageFileMap();
        var pathMap = DataSourceHelper.GetImageFileMap();
//     AutoPtr<List<String> > pathList = DataSourceHelper::GetImagePathList();
        var pathList = DataSourceHelper.GetImagePathList();
//     if (pathList != NULL) {
        if (pathList.length) {
//         Logger::D(TAG, "Run()-----1");
            elog(TAG + "Run()-----1");
//         AutoPtr<List<String> > fileList;
            var fileList;
//         AutoPtr<AlbumEntry> entry;
            var entry;
//         List<String>::Iterator it = pathList->Begin();
//         for (Int32 i = 0; it != pathList->End(); ++it, ++i) {
            for (var i=0,im=pathList.length;i<im;i++) {
//             entry = new AlbumEntry();
//             String path(*it);
//             HashMap<String, AutoPtr<List<String> > >::Iterator iter = pathMap->Find(path);
//             fileList = iter->mSecond;
//             entry->sourcePath = path;
//             entry->sourcePath += DataSourceHelper::PATH_SPLITE;
//             entry->sourcePath += fileList->GetFront();
//             Int32 lastIndex = path.LastIndexOf(DataSourceHelper::PATH_SPLITE);
//             entry->desc = path.Substring(lastIndex + 1, path.GetLength());
//             Int32 size = fileList->GetSize();
//             entry->num = StringUtils::Int32ToString(size);
//             mHost->mAlbumEntryList.PushBack(entry);

//             // Logger::D(TAG, "Album %d: sourcePath:%s, desc:%s, num:%s",
//             //     i, entry->sourcePath.string(), entry->desc.string(), entry->num.string());
//         }
            }

//         Boolean result = FALSE;
            var result = false;
//         mHost->mMyHandler->SendEmptyMessage(MSG_LOAD_ALBUM_VIEW, &result);
            result = mMyHandler.HandleMessage({what:MSG_LOAD_ALBUM_VIEW});
//     }
        }
//     return NOERROR;
        return;
// }
    }

// CAlbumActivity::MyLoadImageCallback::MyLoadImageCallback(
//     /* [in] */ IWeakReference* host)
//     : mWeakHost(host)
// {}

// ECode CAlbumActivity::MyLoadImageCallback::ImageLoaded(
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

//     CAlbumActivity* mHost = (CAlbumActivity*)strongObj.Get();
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
//     mHost->mMyHandler->ObtainMessageEx(CAlbumActivity::MSG_IMAGE_LOADED, args, (IMessage**)&msg);
        msg = {obj:args, what:MSG_IMAGE_LOADED};
//     Boolean result;
        var result;
//     return mHost->mMyHandler->SendMessage(msg, &result);
        result = mMyHandler.HandleMessage(msg);
// }
    }

// void CAlbumActivity::MyImageLoaded(
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

// CAlbumActivity::CAlbumActivity()
// {
//     Logger::D(TAG, "CAlbumActivity(): %p", this);
// }

// CAlbumActivity::~CAlbumActivity()
// {
//     Logger::D(TAG, "~CAlbumActivity(): %p", this);
// }

// ECode CAlbumActivity::OnCreate(
    _apt.OnCreate = function(context,
//     /* [in] */ IBundle* savedInstanceState)
        savedInstanceState)
// {
    {
//     Logger::D(TAG, "OnCreate()-----");
        elog(TAG + "OnCreate()-----");

//     Activity::OnCreate(savedInstanceState);
        //TODO
//     SetContentView(R::layout::activity_album);
        context.SetContentView(R.LastIndexOf.activity_album);

//     AutoPtr<IWeakReference> weakHost;
//     GetWeakReference((IWeakReference**)&weakHost);
//     mMyHandler = new MyHandler(weakHost);

//     // Init image file map.
//     DataSourceHelper::SetRootPath(DataSourceHelper::ROOT_PATH);

//     mMyListener = new MyListener(this);
        mMyListener = new MyListener();

//     AutoPtr<IView> view = FindViewById(R::id::album_btn_popup);
//     mPopupButton = IButton::Probe(view);
//     assert(mPopupButton != NULL);
        mPopupButton = context.FindViewById(R.id.album_btn_popup);

//     view = FindViewById(R::id::album_gridview);
//     mGridView = IGridView::Probe(view);
//     assert(mGridView != NULL);
        mGridView = context.FindViewById(R.id.album_gridview);

//     view = FindViewById(R::id::album_empty);
//     mEmptyView = ITextView::Probe(view);
//     assert(mEmptyView != NULL);
        mEmptyView = context.FindViewById(R.id.album_empty);

//     view = FindViewById(R::id::album_about);
//     mAboutButton = ITextView::Probe(view);
//     assert(mAboutButton != NULL);
        mAboutButton = context.FindViewById(R.id.album_about);

//     view = FindViewById(R::id::album_check_all);
//     mCheckAll = IButton::Probe(view);
//     assert(mCheckAll != NULL);
        mCheckAll = context.FindViewById(R.id.album_check_all);

//     view = FindViewById(R::id::album_btn_detail);
//     mDetailLayout = ILinearLayout::Probe(view);
//     assert(mDetailLayout != NULL);
        mDetailLayout = context.FindViewById(R.id.album_btn_detail);

//     view = FindViewById(R::id::album_btn_delete);
//     mDeleteButton = IImageButton::Probe(view);
//     assert(mDeleteButton != NULL);
        mDeleteButton = context.FindViewById(R.id.album_btn_delete);

//     view = FindViewById(R::id::album_progress_bar);
//     mProgressBar = IProgressBar::Probe(view);
//     assert(mProgressBar != NULL);
        mProgressBar = context.FindViewById(R.id.album_progress_bar);

//     LoadImageInfos();
        LoadImageInfos();
//     return NOERROR;
        return;
// }
    }

// ECode CAlbumActivity::OnStart()
// {
//     Logger::D(TAG, "OnStart()-----");
//     Activity::OnStart();
//     return NOERROR;
// }

// ECode CAlbumActivity::OnResume()
// {
//     Logger::D(TAG, "OnResume()-----");
//     Activity::OnResume();
//     return NOERROR;
// }

// ECode CAlbumActivity::OnPause()
// {
//     Logger::D(TAG, "OnPause()-----");
//     Activity::OnPause();
//     return NOERROR;
// }

// ECode CAlbumActivity::OnStop()
// {
//     Logger::D(TAG, "OnStop()-----");
//     Activity::OnStop();
//     return NOERROR;
// }

// ECode CAlbumActivity::OnDestroy()
    _apt.OnDestroy = function(context)
// {
    {
//     Logger::D(TAG, "OnDestroy()-----");
        elog(TAG + "OnDestroy()-----");
//     Activity::OnDestroy();
        //TODO
//     AutoPtr<AlbumEntry> entry;
        var entry;
//     AutoPtr< ArrayOf<String> > albumArray = ArrayOf<String>::Alloc(mAlbumEntryList.GetSize());
        var albumArray = [];
//     List< AutoPtr<AlbumEntry> >::Iterator it = mAlbumEntryList.Begin();
//     for (Int32 i = 0; it != mAlbumEntryList.End(); ++it, ++i) {
        for (var i=0,im=mAlbumEntryList.length;i<im;i++) {
//         entry = *it;
            entry = mAlbumEntryList[i];
//         (*albumArray)[i] = entry->sourcePath;
            albumEntry[i] = entry.sourcePath;
//     }
        }
//     AsyncImageLoader::UpdateImageCache(albumArray);
        AsyncImageLoader.UpdateImageCache(albumArray);
//     return NOERROR;
        return;
// }
    }

// ECode CAlbumActivity::OnBackPressed()
// {
//     Logger::D(TAG, "OnBackPressed()---");
//     Activity::OnBackPressed();
//     return NOERROR;
// }

// AutoPtr<ISimpleAdapter> CAlbumActivity::GetSimpleAdapter()
    function GetSimpleAdapter()
// {
    {
//     Logger::D(TAG, "GetSimpleAdapter()-----");
        elog(TAG + "GetSimpleAdapter()-----");
//     AutoPtr<ISimpleAdapter> simpleAdapter;
        var simpleAdapter;
//     if (!mAlbumEntryList.IsEmpty()) {
        if (mAlbumEntryList.length) {
//         Logger::D(TAG, "GetSimpleAdapter()---AlbumEntry count: %d", mAlbumEntryList.GetSize());
            elog(TAG + "GetSimpleAdapter()---AlbumEntry count:" + mAlbumEntryList.length);
//         AutoPtr<IObjectContainer> dataList;
            var dataList;
//         CParcelableObjectContainer::New((IObjectContainer**)&dataList);
            dataList = Core_New("Elastos.Utility.CArrayList");
//         AutoPtr<IObjectStringMap> map;
            var map;
//         AutoPtr<IBoolean> boolValue;
            var boolValue;
//         AutoPtr<AlbumEntry> entry;
            var entry;
//         AutoPtr<ICharSequence> cs;
            var cs;
//         String key1("cover");
            var key1 = "cover";
//         String key2("count");
            var key2 = "count";
//         String key3("checkbox");
            var key3 = "checkbox";
//         String key4("name");
            var key4 = "name";
//         String key5("folderlayout");
            var key5 = "folderlayout";
//         List< AutoPtr<AlbumEntry> >::Iterator it = mAlbumEntryList.Begin();
//         for (Int32 i = 0; it != mAlbumEntryList.End(); ++it, ++i) {
            for (var i=0,im=mAlbumEntryList.length;i<im;i++) {
//             entry = *it;
                entry = mAlbumEntryList[i];
//             map = NULL;
                map = null;
//             CObjectStringMap::New((IObjectStringMap**)&map);
                map = Core_New('Elastos.Utility.CHashMap');

//             cs = NULL;
                cs = null;
//             CStringWrapper::New(entry->sourcePath, (ICharSequence**)&cs);
                cs = CString(entry.sourcePath);
//             map->Put(key1, cs);
                map.Put( CString(key1), cs );

//             cs = NULL;
                cs = null;
//             CStringWrapper::New(entry->num, (ICharSequence**)&cs);
                cs = CString(entry.num);
//             map->Put(key2, cs);
                map.Put( CString(key2), cs );

//             boolValue = NULL;
                var boolValue = null;
//             CBoolean::New(FALSE, (IBoolean**)&boolValue);
                boolValue = Core_New('Elastos.Core.CBoolean', false);
//             map->Put(key3, boolValue);
                map.Put( CString(key3), boolValue );

//             cs = NULL;
                cs = null;
//             CStringWrapper::New(entry->desc, (ICharSequence**)&cs);
                cs = CString(entry.desc);
//             map->Put(key4, cs);
                map.Put( CString(key4), cs );

//             dataList->Add(map);
                dataList.Add(map);

//             // Logger::D(TAG, " > album %d, path:%s, num:%s, desc:%s",
//             //     i, entry->sourcePath.string(), entry->num.string(), entry->desc.string());
//         }
            }

//         AutoPtr<ArrayOf<String> > from = ArrayOf<String>::Alloc(5);
            var from = [];
//         (*from)[0] = key1;
            from[0] = key1;
//         (*from)[1] = key2;
            from[1] = key2;
//         (*from)[2] = key3;
            from[2] = key3;
//         (*from)[3] = key4;
            from[3] = key4;
//         (*from)[4] = key5;
            from[4] = key5;
//         AutoPtr<ArrayOf<Int32> > to = ArrayOf<Int32>::Alloc(5);
            var to = [];
//         (*to)[0] = R::id::album_set_cover;
            to[0] = R::id::album_set_cover;
//         (*to)[1] = R::id::album_count;
            to[1] = R::id::album_count;
//         (*to)[2] = R::id::album_set_check;
            to[2] = R::id::album_set_check;
//         (*to)[3] = R::id::album_set_desc;
            to[3] = R::id::album_set_desc;
//         (*to)[4] = R::id::album_cover_layout;
            to[4] = R::id::album_cover_layout;

//         CSimpleAdapter::New(this, dataList, R::layout::album_item, from, to, (ISimpleAdapter**)&simpleAdapter);
            simpleAdapter = Droid_New('Elastos.Droid.Widget.CSimpleAdapter', oActivity, dataList, R.layout.album_item, from, to);
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

// AutoPtr<IArrayAdapter> CAlbumActivity::GetArrayAdapter()
    function GetArrayAdapter()
// {
    {
//     Logger::D(TAG, "GetArrayAdapter()-----");
        elog(TAG + "GetArrayAdapter()-----");
//     AutoPtr<IObjectContainer> dataList;
        var dataList;
//     CParcelableObjectContainer::New((IObjectContainer**)&dataList);
        dataList = Core_New("Elastos.Utility.CArrayList");
//     for (Int32 i = 0; i < 100; ++i) {
        for (var i = 0; i < 100; i++) {
//         StringBuilder sb("Item ");
            var sb = "Item ";
//         sb += i;
            sb += i;
//         AutoPtr<ICharSequence> seq = sb.ToCharSequence();
            var seq = CString(sb);
//         dataList->Add(seq);
            dataList.Add(seq);
//     }
        }

//     AutoPtr<IArrayAdapter> arrayAdapter;
        var arrayAdapter;
//     CArrayAdapter::New(this, R::layout::test_item, dataList, (IArrayAdapter**)&arrayAdapter);
        var arrayAdapter = Droid_New("Elastos.Droid.Widget.CArrayAdapter",oActivity, R.layout.test_item, dataList);
//     return arrayAdapter;
        return arrayAdapter;
// }
    }

// ECode CAlbumActivity::OpenAlbumPopupWindow(
    funcion OpenAlbumPopupWindow(
//     /* [in] */ IView* v)
        v)
// {
    {
//     Logger::D(TAG, "OpenAlbumPopupWindow()-----");
        elog(TAG + "OpenAlbumPopupWindow()-----");
//     AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mMyListener.Get();
        var clickListener = mMyListener;

//     AutoPtr<ILayoutInflater> inflater;
        var inflater;
//     GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);
        inflater = oActivity.GetSystemService(IContext__LAYOUT_INFLATER_SERVICE);

//     AutoPtr<IView> layout;
        var layout;
//     inflater->Inflate(R::layout::album_popup_dialog, NULL, (IView**)&layout);
        layout = inflater.Inflate(R.layout.album_popup_dialog, null);
//     AutoPtr<IView> view;
//     layout->FindViewById(R::id::pop_album, (IView**)&view);
//     AutoPtr<ITextView> popupAlbum = ITextView::Probe(view);
//     assert(popupAlbum != NULL);
        var popupAlbum = layout.FindViewById(R.id.pop_album);
//     popupAlbum->SetOnClickListener(clickListener);
        popupAlbum.SetOnClickListener(clickListener);

//     view = NULL;
//     layout->FindViewById(R::id::pop_time, (IView**)&view);
//     AutoPtr<ITextView> popupTime = ITextView::Probe(view);
//     assert(popupTime != NULL);
        var popupTime = layout.FindViewById(R.id.pop_time);
//     popupTime->SetOnClickListener(clickListener);
        popupTime.SetOnClickListener(clickListener);

//     view = NULL;
//     layout->FindViewById(R::id::pop_location, (IView**)&view);
//     AutoPtr<ITextView> popupLocation = ITextView::Probe(view);
//     assert(popupLocation != NULL);
        var popupLocation = layout.FindViewById(R.id.pop_location);
//     popupLocation->SetOnClickListener(clickListener);
        popupLocation.SetOnClickListener(clickListener);

//     assert(mAlbumPopupWindow.Get() == NULL);
//     CPopupWindow::New(layout, 80, 93, TRUE, (IPopupWindow**)&mAlbumPopupWindow);
        mAlbumPopupWindow = Droid_New('Elastos.Droid.Widget.CPopupWindow', layout, 80, 93, true);
//     mAlbumPopupWindow->SetTouchable(TRUE);
        mAlbumPopupWindow.SetTouchable(true);
//     mAlbumPopupWindow->SetOutsideTouchable(TRUE);
        mAlbumPopupWindow.SetOutsideTouchable(true);

//     AutoPtr<IResources> res;
        var res;
//     GetResources((IResources**)&res);
        res = oActivity.GetResources();
//     AutoPtr<IBitmapDrawable> bitmapDrawable;
        var bitmapDrawable;
//     CBitmapDrawable::New(res, (IBitmap*)NULL, (IBitmapDrawable**)&bitmapDrawable);
        var bitmapDrawable = Droid_New("Elastos.Droid.Graphics.Drawable.CBitmapDrawable", res);
//     mAlbumPopupWindow->SetBackgroundDrawable(IDrawable::Probe(bitmapDrawable));
        mAlbumPopupWindow.SetBackgroundDrawable(bitmapDrawable);
//     mAlbumPopupWindow->SetOnDismissListener((IPopupWindowOnDismissListener*)mMyListener.Get());
        mAlbumPopupWindow.SetOnDismissListener(mMyListener);
//     mAlbumPopupWindow->ShowAsDropDown(v);
        mAlbumPopupWindow.ShowAsDropDown(v);
//     mShowAlbum = TRUE;
        mShowAlbum = true;

//     return NOERROR;
        return;
// }
    }

// ECode CAlbumActivity::CloseAlbumPopupWindow()
    function CloseAlbumPopupWindow()
// {
    {
//     Boolean isShow = FALSE;
        var isShow = false;
//     if (mAlbumPopupWindow != NULL && (mAlbumPopupWindow->IsShowing(&isShow), isShow)) {
        if (mAlbumPopupWindow && (isShow = mAlbumPopupWindow.IsShowing())) {
//         Logger::D(TAG, "CloseAlbumPopupWindow()---");
            elog(TAG + "CloseAlbumPopupWindow()---");
//         mAlbumPopupWindow->Dismiss();
            mAlbumPopupWindow.Dismiss();
//         mShowAlbum = FALSE;
            mShowAlbum = false;
//     }
        }

//     return NOERROR;
        return;
// }
    }

// void CAlbumActivity::LoadImageInfos()
    function LoadImageInfos()
// {
    {
//     Logger::D(TAG, "LoadImageInfos()----");
        elog(TAG + "LoadImageInfos()----");

//     AutoPtr<IWeakReference> weakHost;
//     GetWeakReference((IWeakReference**)&weakHost);

//     AutoPtr<IThread> loadThread;
        var loadThread;
//     AutoPtr<LoadImageInfoRunnable> loadRunnable = new LoadImageInfoRunnable(weakHost);
        var loadRunnable = new LoadImageInfoRunnable();
//     CThread::New(IRunnable::Probe(loadRunnable), (IThread**)&loadThread);
        loadThread = Droid_New("CThread", loadRunnable);
//     assert(loadThread != NULL);
//     loadThread->Start();
        loadThread.Start();
// }
    }

// void CAlbumActivity::LoadAlbumView()
    function LoadAlbumView()
// {
    {
//     Logger::D(TAG, "LoadAlbumView()-----");
        elog(TAG + "LoadAlbumView()-----");
//     mProgressBar->SetVisibility(IView::GONE);
        mProgressBar.SetVisibility(IView__GONE);
//     mSimpleAdapter = GetSimpleAdapter();
        mSimpleAdapter = GetSimpleAdapter();

//     if (mSimpleAdapter != NULL) {
        if (mSimpleAdapter) {
//         mGridView->SetVisibility(IView::VISIBLE);
            mGridView.SetVisibility(IView__VISIBLE);
//         mGridView->SetAdapter(IAdapter::Probe(mSimpleAdapter));
            mGridView.SetAdapter(mSimpleAdapter);
//         // mSimpleAdapter->NotifyDataSetChanged();
//     }
        }
//     else {
        else {
//         Logger::D(TAG, "LoadAlbumView()----mSimpleAdapter is empty");
            elog(TAG + "LoadAlbumView()----mSimpleAdapter is empty");
//         mEmptyView->SetVisibility(IView::VISIBLE);
            mEmptyView.SetVisibility(IView__VISIBLE);
//         mGridView->SetVisibility(IView::GONE);
            mGridView.SetVisibility(IView__GONE);
//     }
        }

//     AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mMyListener.Get();
        var clickListener = mMyListener;
//     mPopupButton->SetOnClickListener(clickListener);
        mPopupButton.SetOnClickListener(clickListener);
//     mDeleteButton->SetOnClickListener(clickListener);
        mDeleteButton.SetOnClickListener(clickListener);
//     mCheckAll->SetOnClickListener(clickListener);
        mCheckAll.SetOnClickListener(clickListener);
//     mDetailLayout->SetOnClickListener(clickListener);
        mDetailLayout.SetOnClickListener(clickListener);
//     mDeleteButton->SetOnClickListener(clickListener);
        mDeleteButton.SetOnClickListener(clickListener);
//     mAboutButton->SetOnClickListener(clickListener);
        mAboutButton.SetOnClickListener(clickListener);
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
    // _apt.OnDestroy = function(context){
    //     elog('====jso_activity_cb====OnDestroy.begin====');
    // }
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