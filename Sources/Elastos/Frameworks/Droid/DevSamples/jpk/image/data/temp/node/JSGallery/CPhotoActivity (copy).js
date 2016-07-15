module.exports = function(aoElastos, aoActivity){
    //common definition
    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;

    var oActivity = aoActivity.ActivityInstance;
    var oHandler = aoActivity.ActivityHandler;

    //member of CPhotoActivity

// private:
//     static const String TAG;
    var TAG = "CPhotoActivity";
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
    var mDetailsArray;
//     List< AutoPtr<PhotoEntry> > mPhotoEntryList;
    var mPhotoEntryList;
//     AutoPtr<IHandler> mMyHandler;
    var mMyHandler;

    //method of CPhotoActivity

    //LoadPhotoEntryList
    //SetCurrentPhoto
    //HideDetails

    //--------MyListener--------begin--------

// CPhotoActivity::MyListener::MyListener(
//      [in]  CPhotoActivity* host)
//     : mHost(host)
// {
    function MyListener = function(host){
        this.mHost = host;
// }
    };

// ECode CPhotoActivity::MyListener::OnClick(
//     /* [in] */ IView* v)
// {
    MyListener.prototype.OnClick = function(v) {
//     Int32 viewId = 0;
//     v->GetId(&viewId);
        var viewId = v.GetId();

//     switch(viewId) {
        switch (viewId) {
//         case R::id::photo_up: {
            case R.id.photo_up :
//             Logger::D(TAG, "OnClick()---photo_up");
                elog(TAG + "OnClick()---photo_up");
//             mHost->OnBackPressed();
                this.mHost.OnBackPressed();
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
            case
//             Logger::D(TAG, "OnClick()---photo_more");
                elog(TAG + "OnClick()---photo_more");
//             if (mHost->mShowMore) {
                if (this.mHost.mShowMore) {
//                 mHost->CloseMorePopupWindow();
                    this.mHost.CloseMorePopupWindow();
//             }
                }
//             else {
                else {
//                 mHost->OpenMorePopupWindow(v);
                    this.mHost.OpenMorePopupWindow(v);
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
                elog(TAG + "OnClick()---img_btn_left--mCurrentIndex:" + mHost.mCurrentIndex);
//             mHost->ChangeCurrentPhoto(FALSE);
                this.mHost.ChangeCurrentPhoto(false);
//             break;
                break;
//         }
//         case R::id::img_btn_right: {
            case R.id.img_btn_right :
//             Logger::D(TAG, "OnClick()---img_btn_right--mCurrentIndex:%d",mHost->mCurrentIndex);
                elog(TAG + "OnClick()---img_btn_right--mCurrentIndex:" + mHost.mCurrentIndex);
//             mHost->ChangeCurrentPhoto(TRUE);
                this.mHost.ChangeCurrentPhoto(true);
//             break;
                break;
//         }
//         case R::id::img_btn_zoom: {
            case R.id.img_btn_zoom :
//             Logger::D(TAG, "OnClick()---img_btn_zoom");
                elog(TAG + "OnClick()---img_btn_zoom");
//             if (mHost->mShowZoom) {
                if (this.mHost.mShowZoom) {
//                 mHost->CloseZoomPopupWindow();
                    this.mHost.CloseZoomPopupWindow();
//             }
                }
//             else {
                else {
//                 mHost->OpenZoomPopupWindow(v);
                    this.mHost.OpenZoomPopupWindow(v);
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
                this.mHost.OpenShareDialog();
//             break;
                break;
//         }
//         case R::id::img_btn_edit: {
            case R.id.img_btn_edit :
//             Logger::D(TAG, "OnClick()---img_btn_edit");
                elog(TAG + "OnClick()---" + img_btn_edit);
//             AutoPtr<PhotoEntry> entry = mHost->mPhotoEntryList[mHost->mCurrentIndex];
                var entry = mHost.mPhotoEntryList[mHost.mCurrentIndex];
//             AutoPtr<IIntent> intent;
//             CIntent::New((IIntent**)&intent);
                var intent = Droid_New("CIntent");
//             intent->SetClassNameEx(String("Gallery"), String("Gallery.CEditActivity"));
                intent.SetClassName("Gallery","Gallery.CEditActivity");
//             intent->PutStringExtra(DataSourceHelper::SOURCE_PATH, entry->sourcePath);
                intent.PutStringExtra(DataSourceHelper.SOURCE_PATH, entry.sourcePath);
//             if (FAILED(mHost->StartActivity(intent))) {
                try {mHost.StartActivity(intent)} catch(e) {
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
                mHost.OpenDeleteDialog();
//             break;
                break;
//         }
//         case R::id::more_pop_wallpaper: {
            case R.id.more_pop_wallpaper :
//             Logger::D(TAG, "OnClick()---more_pop_wallpaper");
                elog(TAG + "OnClick()---more_pop_wallpaper");
//             mHost->CloseMorePopupWindow();
                mHost.CloseMorePopupWindow();
//             AutoPtr<PhotoEntry> entry = mHost->mPhotoEntryList[mHost->mCurrentIndex];
                var entry = mHost.mPhotoEntryList[mHost.mCurrentIndex];
//             AutoPtr<IIntent> intent;
//             CIntent::New((IIntent**)&intent);
                var intent = Droid_New("CIntent");
//             intent->SetClassNameEx(String("Gallery"), String("Gallery.CWallpaperActivity"));
                intent.SetClassName("Gallery", "Gallery.CWallpaperActivity");
//             intent->PutStringExtra(DataSourceHelper::SOURCE_PATH, entry->sourcePath);
                intent.PutStringExtra(DataSourceHelper.SOURCE_PATH, entry.sourcePath);
//             if (FAILED(mHost->StartActivity(intent))) {
                try {mHost.StartActivity(intent)} catch(e) {
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
                mHost.CloseMorePopupWindow();
//             if (mHost->mShowDetails) {
                if (mHost.mShowDetails) {
//                 mHost->HideDetails();
                    mHost.HideDetails();
//             }
                }
//             else {
                else {
//                 mHost->ShowDetails();
                    mHost.ShowDetails();
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
                mHost.ShareDialogOnCancel();
//             break;
//         }
//         case R::id::delete_dialog_btn_ok: {
            case R.id.delete_dialog_btn_ok :
//             Logger::D(TAG, "OnClick()---delete_dialog_btn_ok");
                elog(TAG + "OnClick()---delete_dialog_btn_ok");
//             mHost->DeleteDialogOnOK();
                mHost.DeleteDialogOnOK();
//             break;
                break;
//         }
//         case R::id::delete_dialog_btn_cancel: {
            case R.id.delete_dialog_btn_cancel :
//             Logger::D(TAG, "OnClick()---delete_dialog_btn_cancel");
                elog(TAG + "OnClick()---delete_dialog_btn_cancel");
//             mHost->DeleteDialogOnCancel();
                mHost.DeleteDialogOnCancel();
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
// {
    MyListener.prototype.OnDismiss = function() {
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
//     /* [in] */ ISeekBar* seekBar,
//     /* [in] */ Int32 progress,
//     /* [in] */ Boolean fromUser)
// {
    MyListener.prototype.OnProgressChanged = function(seekBar, progress, fromUser) {
//     Logger::D(TAG, "OnProgressChanged()---progress:%d", progress);
        elog(TAG + "OnProgressChanged()---progress:" + progress);
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::MyListener::OnStartTrackingTouch(
//     /* [in] */ ISeekBar* seekBar)
// {
    MyListener.prototype.OnStartTrackingTouch = function(seekBar) {
//     Logger::D(TAG, "OnStartTrackingTouch()---");
        elog(TAG + "OnStartTrackingTouch()---");
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::MyListener::OnStopTrackingTouch(
//     /* [in] */ ISeekBar* seekBar)
// {
    MyListener.prototype.OnStopTrackingTouch = function(seekBar) {
//     Logger::D(TAG, "OnStopTrackingTouch()---");
        elog(TAG + "OnStopTrackingTouch()---");
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::MyListener::OnClose()
// {
    MyListener.prototype.OnClose = function() {
//     Logger::D(TAG, "OnClose()---");
        elog(TAG + "OnClose()---");
//     mHost->HideDetails();
        mHose.HideDetails();
//     return NOERROR;
        return;
// }
    }

    //--------MyListener--------end--------

    //--------DialogListener--------begin--------

// CAR_INTERFACE_IMPL(CPhotoActivity::DialogListener, IDialogInterfaceOnDismissListener)

// CPhotoActivity::DialogListener::DialogListener(
//     /* [in] */ CPhotoActivity* host)
//     : mHost(host)
// {
    function DialogListener (host) {
        this.mHost = host;
// }
    }

// ECode CPhotoActivity::DialogListener::OnDismiss(
//     /* [in] */ IDialogInterface* dialog)
// {
    DialogListener.prototype.OnDismiss = function (dialog) {
//     if (IDialogInterface::Probe(mHost->mShareDialog) == dialog) {
        if (this.mHost.mShareDialog == dialog) {
//         Logger::D(TAG, "OnDismissShareDialog---: %p", mHost->mShareDialog.Get());
            elog("TAG" + "OnDismissShareDialog---: ") + mHost.mShareDialog;
//         mHost->mShareDialog = NULL;
            mHost.mShareDialog = null;
//     }
        }
//     else if (IDialogInterface::Probe(mHost->mDeleteDialog) == dialog) {
        else if (mHost.mDeleteDialog == dialog) {
//         Logger::D(TAG, "OnDismissDeleteDialog---: %p", mHost->mDeleteDialog.Get());
            elog(TAG + "OnDismissDeleteDialog---: " + mHost.mDeleteDialog);
//         mHost->mDeleteDialog = NULL;
            mHost.mDeleteDialog = null;
//     }
        }

//     return NOERROR;
        return;
// }
    }

    //--------DialogListener--------end--------

    //--------MyLoadImageCallback--------begin--------

// CPhotoActivity::MyLoadImageCallback::MyLoadImageCallback(
//     /* [in] */ IWeakReference* host,
//     /* [in] */ const String& path,
//     /* [in] */ Boolean isHigh)
//     : mWeakHost(host)
//     , mPath(path)
//     , mIsHigh(isHigh)
// {
    function MyLoadImageCallback = function(host, path, isHigh) {
        this.mHost = host;
        this.mPath = path;
        this.mIsHight = isHigh;
// }
    }

// ECode CPhotoActivity::MyLoadImageCallback::ImageLoaded(
//     /* [in] */ IDrawable* imageDrawable,
//     /* [in] */ IImageView* imageView)
// {
    MyLoadImageCallback.prototype.ImageLoaded = function(imageDrawable, imageView) {
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
//     args->mArg1 = imageDrawable;
//     args->mArg2 = imageView;
//     AutoPtr<ICharSequence> seq;
//     CStringWrapper::New(mPath, (ICharSequence**)&seq);
//     args->mArg3 = seq;
//     args->mArgi1 = mIsHigh ? 1 : 0;
        var args = {
            mArg1 : imageDrawable,
            mArg2 : imageView,
            mArg3 : CString(mPath),
            mArgi1 : this.mIsHigh ? 1 : 0,
        }

//     AutoPtr<IMessage> msg;
//     mHost->mMyHandler->ObtainMessageEx(CPhotoActivity::MSG_IMAGE_LOADED, args, (IMessage**)&msg);
        mMyHandler.ObtainMessage(MSG_IMAGE_LOADED, args);
//     Boolean result;
//     return mHost->mMyHandler->SendMessage(msg, &result);
        var result = mMyHandler.SendMessage(msg);
// }
    }

    //--------MyLoadImageCallback--------end--------

    //--------Activity begin----------------

// CPhotoActivity::CPhotoActivity()
    function CPhotoActivity(
//     : mIndex(0)
        mIndex,
//     , mCurrentIndex(0)
        mCurrentIndex,
//     , mTotalSize(0)
        mTotalSize,
//     , mDisplayWidth(0)
        mDisplayWidth,
//     , mDisplayHeight(0)
        mDisplayHeight,
//     , mShowMore(FALSE)
//     , mShowZoom(FALSE)
//     , mShowDetails(FALSE)
//     , mViewSingleItem(FALSE)
// {
    ) {
        this.mIndex = mIndex || 0;
        this.mCurrentIndex = mCurrentIndex || 0;
        this.mTotalSize = mTotalSize || 0;
        this.mDisplayWidth = mDisplayWidth || 0;
        this.mDisplayHeight = mDisplayHeight || 0;

        this.mShowMore = false;
        this.mShowZoom = false;
        this.ShowDetails = false;
        this.mViewSingleItem = false;
//     Logger::D(TAG, "CPhotoActivity(): %p", this);
        elog(TAG + "CPhotoActivity(): " + TAG);
// }
    }

    var _apt = CActivityListener.prototype;

// CPhotoActivity::~CPhotoActivity()
// {
    _apt.Destroy = function () {
//     Logger::D(TAG, "~CPhotoActivity(): %p", this);
        elog(TAG + "~CPhotoActivity(): " + TAG);
// }
    }

// ECode CPhotoActivity::OnBackPressed()
// {
    _apt.OnBackPressed = function() {
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
            //TODO
//     }
        }

//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::OpenZoomPopupWindow(
//     /* [in] */ IView* v)
// {
    _apt.OpenZoomPopupWindow = function(v) {
//     Logger::D(TAG, "OpenZoomPopupWindow()---");
        elog(TAG + "OpenZoomPopupWindow()---");

//     AutoPtr<ILayoutInflater> inflater;
//     GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);
        var inflater = GetSystemService(IContext__LAYOUT_INFLATER_SERVICE);

//     AutoPtr<IView> layout;
//     inflater->Inflate(R::layout::zoom_popup_dialog, NULL, (IView**)&layout);
        var layout = inflater.Inflate(R.layout.zoom_popup_dialog);
//     AutoPtr<IView> view;
//     layout->FindViewById(R::id::zoom_seeker, (IView**)&view);
//     AutoPtr<ISeekBar> seekBar = ISeekBar::Probe(view);
//     assert(seekBar != NULL);
        var seekBar = layout.FindViewById(R.id.zoom_seeker);
//     seekBar->SetOnSeekBarChangeListener((ISeekBarOnSeekBarChangeListener*)(mListener.Get()));
        seekBar.SetOnSeekBarChangeListener(mHost.mListener);

//     assert(mZoomPopupWindow.Get() == NULL);
//     CPopupWindow::New(layout, 400, 50, TRUE, (IPopupWindow**)&mZoomPopupWindow);
        mZoomPopupWindow = Droid_New(layout, 400, 50, true);
//     mZoomPopupWindow->SetTouchable(TRUE);
        mZoomPopupWindow.SetTouchable(true);
//     mZoomPopupWindow->SetOutsideTouchable(TRUE);
        mZoomPopupWindow.SetOutsideTouchable(true);

//     AutoPtr<IResources> res;
//     GetResources((IResources**)&res);
        var res = mHost.GetResources();
//     AutoPtr<IBitmapDrawable> bitmapDrawable;
//     CBitmapDrawable::New(res, (IBitmap*)NULL, (IBitmapDrawable**)&bitmapDrawable);
        var bitmapDrawable = Droid_New("CBitmapDrawable", res);
//     mZoomPopupWindow->SetBackgroundDrawable(IDrawable::Probe(bitmapDrawable));
        mZoomPopupWindow.SetBackgroundDrawable(bitmapDrawable);
//     mZoomPopupWindow->SetOnDismissListener((IPopupWindowOnDismissListener*)mListener.Get());
        mZoomPopupWindow.SetOnDismissListener(mHost.mListener);
//     mZoomPopupWindow->ShowAtLocation(v, IGravity::NO_GRAVITY, 310, 561);
        mZoomPopupWindow.ShowAtLocation(v, IGravity__NO_GRAVITY, 310, 561);
//     mShowZoom = TRUE;
        mShowZoom = true;

//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::CloseZoomPopupWindow()
// {
    _apt.CloseZoomPopupWindow = function() {
//     Boolean isShow = FALSE;
//     if (mZoomPopupWindow != NULL && (mZoomPopupWindow->IsShowing(&isShow), isShow)) {
        if (mZoomPopupWindow && mZoomPopupWindow.IsShowing()) {
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
//     /* [in] */ IView* v)
// {
    _apt.OpenMorePopupWindow = function(v) {
//     Logger::D(TAG, "OpenMorePopupWindow()---");
        elog(TAG + "OpenMorePopupWindow()---");
//     AutoPtr<ILayoutInflater> inflater;
//     GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);
        var inflater = GetSystemService(IContext__LAYOUT_INFLATER_SERVICE);

//     AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mListener.Get();
//     AutoPtr<IView> layout;
//     inflater->Inflate(R::layout::more_popup_dialog, NULL, (IView**)&layout);
        var layout = inflater.Inflate(R.layout.more_popup_dialog);
//     AutoPtr<IView> view;
//     layout->FindViewById(R::id::more_pop_wallpaper, (IView**)&view);
//     AutoPtr<ITextView> popupWallpaper = ITextView::Probe(view);
//     assert(popupWallpaper != NULL);
        var popupWallpaper = layout.FindViewById(R.id.more_pop_wallpaper);
//     popupWallpaper->SetOnClickListener(clickListener);
        popupWallpaper.SetOnDismissListener(mListener);

//     view = NULL;
//     layout->FindViewById(R::id::more_pop_info, (IView**)&view);
//     AutoPtr<ITextView> popupInfo = ITextView::Probe(view);
//     assert(popupInfo != NULL);
        var popupInfo = layout.FindViewById(R.id.more_pop_info);
//     popupInfo->SetOnClickListener(clickListener);
        popupInfo.SetOnClickListener(mListener);

//     assert(mMorePopupWindow.Get() == NULL);
//     CPopupWindow::New(layout, 80, 63, TRUE, (IPopupWindow**)&mMorePopupWindow);
        mMorePopupWindow = Droid_New("CPopupWindow", layout, 80, 63,true);
//     mMorePopupWindow->SetTouchable(TRUE);
        mMorePopupWindow.SetTouchable(true);
//     mMorePopupWindow->SetOutsideTouchable(TRUE);
        mMorePopupWindow.SetOutsideTouchable(true);

//     AutoPtr<IResources> res;
//     GetResources((IResources**)&res);
        var res = GetResources();
//     AutoPtr<IBitmapDrawable> bitmapDrawable;
//     CBitmapDrawable::New(res, (IBitmap*)NULL, (IBitmapDrawable**)&bitmapDrawable);
        var bitmapDrawable = Droid_New("CBitmapDrawable", res);
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
// {
    _apt.CloseMorePopupWindow = function() {
//     Boolean isShow = FALSE;
//     if (mMorePopupWindow != NULL && (mMorePopupWindow->IsShowing(&isShow), isShow)) {
        if (mMorePopupWindow && mMorePopupWindow.IsShowing()) {
//         Logger::D(TAG, "CloseMorePopupWindow()---");
            elog(TAG + "CloseMorePopupWindow()---");
//         mMorePopupWindow->Dismiss();
            mMorePopupWindow.Dismiss()''
//         mShowMore = FALSE;
            mShowMore = false;
//     }
        }
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::OpenShareDialog()
// {
    _apt.OpenShareDialog = function() {
//     Logger::D(TAG, "OpenShareDialog()--- : prev : %p", mShareDialog.Get());
        elog(TAG +"OpenShareDialog()--- : prev :" + mShareDialog);
//     assert(mShareDialog.Get() == 0);
//     CDialog::New(this, R::style::MyDialog, (IDialog**)&mShareDialog);
        mShareDialog = Droid_New("CDialog", this, R.style.MyDialog);
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
// {
    _apt.ShareDialogOnOK = function() {
//     ShareDialogOnCancel(); //TODO
        ShareDialogOnCancel();
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::ShareDialogOnCancel()
// {
    _apt.ShareDialogOnCancel = function() {
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
// {
    _apt.DeleteDialogOnOK = function () {
//     DeleteDialogOnCancel(); //TODO
        DeleteDialogOnCancel();
//     return NOERROR;
        reeturn;
// }
    }

// ECode CPhotoActivity::DeleteDialogOnCancel()
// {
    _apt.DeleteDialogOnCancel = function() {
//     if (mDeleteDialog) {
        if (mDeleteDialog) {
//         Logger::D(TAG, "DeleteDialogOnCancel---: %p", mDeleteDialog.Get());
            elog(TAG + "DeleteDialogOnCancel---: " + mDeleteDialog);
//         mDeleteDialog->Cancel();
            mDeleteDialog.Cancel();
//     }
//     return NOERROR;
        return;
// }
    }

// ECode CPhotoActivity::OpenDeleteDialog()
// {
    _apt.OpenDeleteDialog = function() {
//     Logger::D(TAG, "OpenDeleteDialog()--- : prev : %p", mDeleteDialog.Get());
        elog(TAG + "OpenDeleteDialog()---: prev : " + mDeleteDialog);
//     assert(mDeleteDialog.Get() == 0);
//     CDialog::New(this, R::style::MyDialog, (IDialog**)&mDeleteDialog);
        mDeleteDialog = Droid_New("CDialog", R.style.MyDialog);
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
// {
    _apt.ShowDetails = function () {
//     mShowDetails = TRUE;
        mShowDetails = true;
//     if (mDetailsHelper == NULL) {
        if (!mDetailsHelper) {
//         mDetailsHelper = new DetailsHelper(this, mDetailsArray);
            mDetailsHelper = new DetailsHelper();
//         AutoPtr<ICloseListener> closeListener = (ICloseListener*)mListener.Get();
//         mDetailsHelper->SetCloseListener(closeListener);
            mDetailsHelper.SetCloseListener(mListener);
//     }
        }
//     mDetailsHelper->Show();
        mDetailsHelper.Show();
// }
    }

// void CPhotoActivity::HideDetails()
// {
    _apt.HideDetails = function () {
//     mShowDetails = FALSE;
        mShowDetails = false;
//     mDetailsHelper->Hide();
        mDetailsHelper.Hide();
// }
    }

// void CPhotoActivity::SetCurrentPhoto()
// {
    _apt.SetCurrentPhoto = function () {
//     Logger::D(TAG, "SetCurrentPhoto()---");
        elog(TAG + "SetCurrentPhoto()---");
//     if (mCurrentIndex > 0) {
        if (mCurrentIndex) {
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
        if (mCurrentIndex > mTotalSize) {
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
//     /* [in] */ Boolean isNext)
// {
    _apt.ChangeCurrentPhoto = function () {
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
//     /* [in] */ IDrawable* imageDrawable,
//     /* [in] */ IImageView* imageView,
//     /* [in] */ const String& path,
//     /* [in] */ Boolean isHigh)
// {
    _apt.MyImageLoaded = function (imageDrawable,imageView,path,isHigh) {
//     Logger::D(TAG, "MyImageLoaded()-----path:%s,isHigh:%d", path.string(), isHigh);
        elog(TAG + "MyImageLoaded()-----path:"+path+",isHigh:"+isHigh);
//     if (!mCurrentImagePath.Equals(path)) {
        if (mCurrentImagePath == path) {
//         Logger::W(TAG, "MyImageLoaded()---not current path, drop!");
            elog(TAG + "MyImageLoaded()---not current path, drop!");
//         return;
            return;
//     }
        }

//     SwitchDisplayView(FALSE);
        SwitchDisplayView(FALSE);
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
                elog(TAG + "MyImageLoaded()---GetOrigionWidthAndHeight--failed!");
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
        mDetailsArray[2] = w+'';
//     (*mDetailsArray)[3] = StringUtils::Int32ToString(h);
        mDetailsArray[3] = h+'';
//     imageView->SetImageDrawable(imageDrawable);
        imageView.SetImageDrawable(imageDrawable);
// }
    }

// void CPhotoActivity::SetDetailInfo(
//     /* [in] */ const String& filePath)
// {
    _apt.SetDetailInfo = function(filePath) {
//     (*mDetailsArray)[0] = filePath.Substring(filePath.LastIndexOf(DataSourceHelper::PATH_SPLITE) + 1,
//         filePath.LastIndexOf("."));
        mDetailsArray[0] = filePath.substring(filePath.LastIndexOf(DataSourceHelper.PATH_SPLITE) + 1,
            filePath.LastIndexOf("."));
//     (*mDetailsArray)[5] = filePath;
        mDetailsArray[5] = filePath;
//     Logger::D(TAG, "SetDetailInfo()---filePath:%s", filePath.string());
        elog([
            TAG,
            "SetDetailInfo()---",
            "filePath:" + filePath,
        ].join(""));
//     AutoPtr<IFile> file;
        var file;
//     CFile::New(filePath, (IFile**)&file);
        file = Droid_New("CFile", filePath);
//     if (file != NULL) {
        if (file) {
//         Int64 last = 0;
            var last = 0;
//         file->LastModified(&last);
            last = file.LastModified();
//         AutoPtr<IDate> date;
            var data;
//         CDate::New(last, (IDate**)&date);
            date = Droid_New("CDate",last);
//         AutoPtr<ISimpleDateFormat> sdf;
            var sdf;
//         CSimpleDateFormat::New(String("yyyy-MM-dd HH:mm"), (ISimpleDateFormat**)&sdf);
            sdf = Droid_New("CSimpleDateFormat","yyyy-MM-dd HH:mm");
//         String str;
            var str;
//         sdf->FormatDate(date, &str);
            str = sdf.FormatDate(date);
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
                Int32 i = len / (1024 * 1024);
//             if (i > 0) {
                if (i > 0) {
//                 Float j = (((Float)(len % (1024 * 1024))) / (1024 * 1024)) * 10;
                    var j = (len % (1024 * 1024) / (1024 * 1024)) * 10;
//                 Int32 m = Elastos::Core::Math::Round(j);
                    var m = Round(j);
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
                    Float j = ((len % 1024) / 1024) * 10;
//                 Int32 m = Elastos::Core::Math::Round(j);
                    Int32 m = Round(j);
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
//     /* [in] */ const String& path,
//     /* [in] */ Boolean isHighQuality)
// {
    _apt.LoadCurrentImage = function(path,isHighQuality) {
//     Logger::D(TAG, "LoadCurrentImage()---isHighQuality:%d", isHighQuality);
        elog([
            TAG,
            "LoadCurrentImage()---",
            JSON.stringify{
                isHighQuality:isHighQuality,
            }),
        ].join(""));
//     SwitchDisplayView(FALSE);
        SwitchDisplayView(false);
//     if (isHighQuality) {
        if (isHighQuality) {
//         AutoPtr<IWeakReference> weakHost;
//         GetWeakReference((IWeakReference**)&weakHost);
//         assert(weakHost != NULL);
            var weakHost = mHost;
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
                    mDetailsArray[2] = w + '';
//                 (*mDetailsArray)[3] = StringUtils::Int32ToString(h);
                    mDetailsArray[3] = h + '';
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
//         GetWeakReference((IWeakReference**)&weakHost);
//         assert(weakHost != NULL);
            var weakHost = mhost;
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
//     /* [in] */ Boolean showLoading)
// {
    _apt.SwitchDisplayView = function(showLoading) {
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
//     /* [in] */ Boolean needReload)
// {
    _apt.UpdateLoadingStatus = function(needReload) {
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
                var firstIndex = oldText.IndexOf(".");
//             Int32 lastIndex = oldText.LastIndexOf(".");
                var LastIndexOf = oldText.LastIndexOf(".");
//             Int32 num = lastIndex - firstIndex;
                var num = LastIndex - firstIndex;
//             Logger::D(TAG, "UpdateLoadingStatus()---oldText:%s,firstIndex:%d,num:%d", oldText.string(), firstIndex, num);
                elog([
                    TAG,
                    "UpdateLoadingStatus()---",
                    oldText:oldText,
                    firstIndex:firstIndex,
                    num:num
                ].join(""));
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
                mPhotoLoadingText.SetText();
//             TriggerLoadingCallback(FALSE);
                TriggerLoadingCallback(false);
//         }
            }
//         else {
            else {
//             Logger::D(TAG, "UpdateLoadingStatus()---stop:%d", ret);
                elog([
                    TAT,
                    "UpdateLoadingStatus()---",
                    stop+":"+ret,
                ].join(""));
//         }
            }
//     }
        }
// }
    }

// void CPhotoActivity::TriggerLoadingCallback(
//     /* [in] */ Boolean isCancel)
// {
    _apt.TriggerLoadingCallback = function(isCancel) {
//     Logger::D(TAG, "TriggerLoadingCallback()---isCancel:%d", isCancel);
        elog(TAG + "TriggerLoadingCallback()---isCancel:"+isCancel);
//     if (isCancel) {
        if (isCancel) {
//         mHandler->RemoveMessages(MSG_UPDATE_LOADING_STATUS);
            mHandler.RemoveMessages(MSG_UPDATE_LOADING_STATUS);
//     }
        }
//     else {
        else {
//         Boolean needReload = FALSE;
            needReload = false;
//         AutoPtr<IMessage> msg;
//         mMyHandler->ObtainMessageEx2(MSG_UPDATE_LOADING_STATUS,
//             needReload ? 1 : 0, 0, (IMessage**)&msg);
            msg = mMyHandler.ObtainMessageEx2(MSG_UPDATE_LOADING_STATUS,
                needReload ? 1 : 0, 0);
//         Boolean result;
//         mMyHandler->SendMessageDelayed(msg, 1000, &result);
            var result = mMyHandler.SendMessageDelayed(msg, 1000);
//     }
        }
// }
    }

// ECode CPhotoActivity::OnNewIntent(
//     /* [in] */ IIntent *intent)
// {
    _apt.OnNewIntent = function(intent) {
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
//     /* [in] */ IIntent* intent)
// {
    _apt.LoadPhotoEntryList = function(intent) {
//     if (intent != NULL) {
        if (intent) {
//         String action;
//         String filePath;
            var filePath;
//         intent->GetAction(&action);
            var action = intent.GetAction();
//         Logger::D(TAG, "action:%s", action.string());
            elog(TAG + "action:" + action);
//         AutoPtr<IUri> uri;
//         intent->GetData((IUri**)&uri);
            var uri = intent.GetData();
//         if (uri != NULL) {
            if (uri) {
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
                        entry.desc = filePath.split(DataSourceHelper.PATH_SPLITE).pop();
//                     Logger::D(TAG, "single photo, path:%s, desc:%s", entry->sourcePath.string(), entry->desc.string());
                        elog(TAG + "single photo, path:"+entry.sourcePath+", desc:"+entry.desc);
//                     mPhotoEntryList.PushBack(entry);
                        mPhotoEntryList.push(entry);
//                     mCurrentIndex = 0;
                        mCurrentIndex = 0;
//                     mTotalSize = mPhotoEntryList.GetSize();
                        mTotalSize = mPhotoEntryList.GetSize();
//                     mViewSingleItem = TRUE;
                        mViewSingleItem = true;
//                 }
                    }
//                 else {
                    else {
//                     Logger::W(TAG, "image type is not support! filePath:%s", filePath.string());
                        elog(TAG + "image type is not support! filePath:"+filePath);
//                     Utils::MakeToast(this, String("不支持显示该类型的文件！"), IToast::LENGTH_LONG);
                        var utils = Droid_New("Utils");
                        utils.MakeToast(oActivity,"不支持显示该类型的文件！");
//                     Finish();
                        Finish();
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
                filePath = intent.GetStringExtra(DataSourceHelper.SOURCE_PATH);
//             if (!filePath.IsNullOrEmpty()) {
                if (filePath.length) {
//                 intent->GetInt32Extra(DataSourceHelper::SOURCE_INDEX, 0, &mCurrentIndex);
                    mCurrentIndex = intent.GetInt32Extra(DataSourceHelper.SOURCE_INDEX, 0);
//                 mFolderPath = filePath.Substring(0, filePath.LastIndexOf(DataSourceHelper::PATH_SPLITE));
                    mFolderPath = filePath.slice(-filePath.LastIndexOf(DataSourceHelper.PATH_SPLITE));
//                 AutoPtr<List<String> > imageItems = DataSourceHelper::GetItemList(mFolderPath);
                    var imageItems = DataSourceHelper.GetItemList(mFolderPath);
//                 if (imageItems != NULL) {
                    if (imageItems) {
//                     AutoPtr<PhotoEntry> entry;
                        var entry;
//                     List<String>::Iterator it = imageItems->Begin();
//                     for (Int32 i = 0; it != imageItems->End(); ++it, ++i) {
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
        if (!mPhotoSlideshow.length) {
//         Logger::W(TAG, "mPhotoEntryList size is zero!");
            elgo(TAG + "mPhotoSlideshow size is zero!");
//         Utils::MakeToast(this, String("没有图片文件！"), IToast::LENGTH_LONG);
            var utils = Droid_New("Utils");
            utils.MakeToast(oActivity, "没有图片文件！", IToast__LENGTH_LONG);
//         Finish();
            Finish();
//         return FALSE;
            return false;
//     }

//     return TRUE;
        return true;
// }
    }

    //--------Activity end----------------

    //--------ActivityListener begin----------------

    //return {
    function CActivityListener (host) {
        this.host = host;
    };
    var _pt = CActivityListener.prototype;

// ECode CPhotoActivity::OnCreate(
//     /* [in] */ IBundle* savedInstanceState)
// {
    _pt.OnCreate = function(aoContext){
        var _this = aoContext;
//     Logger::D(TAG, "OnCreate()---");
        elog(TAG + "OnCreate()---");
//     Activity::OnCreate(savedInstanceState);
//     SetContentView(R::layout::activity_photo);
        _this.SetContentView(R.layout.activity_photo);

//     AutoPtr<IWeakReference> weakHost;
//     GetWeakReference((IWeakReference**)&weakHost);
//     mMyHandler = new MyHandler(weakHost);
        mMyHandler = new MyHandler(_this);

//     mListener = new MyListener(this);
        mListener = new MyListener(_this);
//     mDialogListener = new DialogListener(this);
        mDialogListener = new DialogListener(_this);
//     mDetailsArray = ArrayOf<String>::Alloc(6);
        mDetailsArray = [];

//     AutoPtr<IIntent> intent;
//     GetIntent((IIntent**)&intent);
        var intent = _this.GetIntent();
//     if (!LoadPhotoEntryList(intent))
//         return NOERROR;
        try {LoadPhotoEntryList(intent) catch (e) {
            return;
        }

//     AutoPtr<IView> view = FindViewById(R::id::photo_up);
//     mUpButton = IImageView::Probe(view);
//     assert(mUpButton != NULL);
        mUpButton = _this.FindViewById(R.id.photo_up);

//     view = FindViewById(R::id::photo_name);
//     mPhotoName = ITextView::Probe(view);
//     assert(mPhotoName != NULL);
        mPhotoName = _this.FindViewById(R,id,photo_name);

//     view = FindViewById(R::id::photo_slideshow);
//     mPhotoSlideshow = IImageView::Probe(view);
//     assert(mPhotoSlideshow != NULL);
        mPhotoSlideshow = _this.FindViewById(R.id.photo_slideshow);

//     view = FindViewById(R::id::photo_more);
//     mPhotoMore = IImageView::Probe(view);
//     assert(mPhotoMore != NULL);
        mPhotoMore = _this.FindViewById(R.id.photo_more);

//     view = FindViewById(R::id::photo_detail);
//     mPhotoView = IImageView::Probe(view);
//     assert(mPhotoView != NULL);
        mPhotoView = _this.FindViewById(R.id.photo_detail);

//     view = FindViewById(R::id::photo_loading);
//     mPhotoLoadingText = ITextView::Probe(view);
//     assert(mPhotoLoadingText != NULL);
        mPhotoLoadingText = _this.FindViewById(R.id.photo_loading);

//     view = FindViewById(R::id::img_btn_left);
//     mLeftButton = IImageButton::Probe(view);
//     assert(mLeftButton != NULL);
        mLeftButton = _this.FindViewById(R.id.img_btn_left);

//     view = FindViewById(R::id::img_btn_right);
//     mRightButton = IImageButton::Probe(view);
//     assert(mRightButton != NULL);
        mRightButton = _this.FindViewById(R.id.img_btn_right);

//     view = FindViewById(R::id::img_btn_zoom);
//     mZoomButton = IImageButton::Probe(view);
//     assert(mZoomButton != NULL);
        mZoomButton = _this.FindViewById(R.id.img_btn_zoom);

//     view = FindViewById(R::id::img_btn_rotate_left);
//     mRotateLeftButton = IImageButton::Probe(view);
//     assert(mRotateLeftButton != NULL);
        mRotateLeftButton = _this.FindViewById(R.id.img_btn_rotate_left);

//     view = FindViewById(R::id::img_btn_rotate_right);
//     mRotateRightButton = IImageButton::Probe(view);
//     assert(mRotateRightButton != NULL);
        mRotateRightButton = _this.FindViewById(R.id.img_btn_rotate_right);

//     view = FindViewById(R::id::img_btn_share);
//     mShareButton = IImageButton::Probe(view);
//     assert(mShareButton != NULL);
        mShareButton = _this.FindViewById(R.id.img_btn_share);

//     view = FindViewById(R::id::img_btn_edit);
//     mEditButton = IImageButton::Probe(view);
//     assert(mEditButton != NULL);
        mEditButton = _this.FindViewById(R.id.img_btn_edit);

//     view = FindViewById(R::id::img_btn_delete);
//     mDeleteButton = IImageButton::Probe(view);
//     assert(mDeleteButton != NULL);
        mDeleteButton = _this.FindViewById(R.id.img_btn_delete);

//     AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mListener.Get();
//     mUpButton->SetOnClickListener(clickListener);
        mUpButton.SetOnClickListener(mListener);
//     mPhotoSlideshow->SetOnClickListener(clickListener);
        mUpButton.SetOnClickListener(mListener);
//     mPhotoSlideshow->SetOnClickListener(clickListener);
        mUpButton.SetOnClickListener(mListener);
//     mLeftButton->SetOnClickListener(clickListener);
        mLeftButton.SetOnClickListener(mListener);
//     mRightButton->SetOnClickListener(clickListener);
        mRightButton.SetOnClickListener(mListener);
//     mZoomButton->SetOnClickListener(clickListener);
        mZoomButton.SetOnClickListener(mListener);
//     mRotateLeftButton->SetOnClickListener(clickListener);
        mRotateLeftButton.SetOnClickListener(mListener);
//     mRotateRightButton->SetOnClickListener(clickListener);
        mRotateRightButton.SetOnClickListener(mListener);
//     mShareButton->SetOnClickListener(clickListener);
        mShareButton.SetOnClickListener(mListener);
//     mEditButton->SetOnClickListener(clickListener);
        mEditButton.SetOnClickListener(mListener);
//     mDeleteButton->SetOnClickListener(clickListener);
        mDeleteButton.SetOnClickListener(mListener);

//     if (mViewSingleItem) {
        if (mViewSingleItem) {
//         mUpButton->SetVisibility(IView::INVISIBLE);
            mUpButton.SetVisibility(IView__INVISIBLE);
//         mPhotoSlideshow->SetVisibility(IView::INVISIBLE);
            mPhotoSlideshow.SetVisibility(IView__INVISIBLE);
//     }
        }

//     // for test
//     AutoPtr<IDisplayMetrics> dm;
//     AutoPtr<IDisplay> dis;
//     CDisplayMetrics::New((IDisplayMetrics**)&dm);
        var dm = Droid_New("CDisplayMetrics");
//     GetWindowManager()->GetDefaultDisplay((IDisplay**)&dis);
        var dis = _this.GetWindowManager().GetDefaultDisplay();
//     dis->GetMetrics(dm);
        dis.GetMetrics(dm);
//     Int32 widthPixels = 0, heightPixels = 0;
//     dm->GetWidthPixels(&widthPixels);
        var widthPixels = dm.GetWidthPixels();
//     dm->GetHeightPixels(&heightPixels);
        var heightPixels = dm.GetHeightPixels();
//     mDisplayWidth = widthPixels - 128;
        mDisplayWidth = widthPixels - 128;
//     mDisplayHeight = heightPixels - 128;
        mDisplayHeight = heightPixels - 128;
//     Logger::D(TAG, "OnCreate()---widthPixels:%d,heightPixels:%d,mDisplayWidth:%d,mDisplayHeight:%d",
//         widthPixels, heightPixels, mDisplayWidth, mDisplayHeight);
        elog([
            TAG + "OnCreate()---["
            "widthPixels:" + widthPixels,
            "heightPixels" + heightPixels,
            "mDisplayWidth:" + mDisplayWidth,
            "mDisplayWidth:" + mDisplayWidth,
            "]"
        ].join("]["));

//     SetCurrentPhoto();
        SetCurrentPhoto();
//     return NOERROR;
        return;
// }
    }  //OnCreate
    _pt.OnStart = function(aoContext){
        elog('====jso_activity_cb====OnStart.begin====');
    }
    _pt.OnResume = function(aoContext){
            elog('====jso_activity_cb====OnResume.begin====');
    }
    _pt.OnPause = function(aoContext){
            elog('====jso_activity_cb====OnPause.begin====');
    }
    _pt.OnStop = function(aoContext){
            elog('====jso_activity_cb====OnStop.begin====');
    }
    _pt.OnDestroy = function(aoContext){
            elog('====jso_activity_cb====OnDestroy.begin====');
    }
    _pt.OnActivityResult = function(aoContext, aiRequestCode, aiResultCode, aoIntentData){
            elog('====jso_activity_cb====OnActivityResult.begin====');
        },
    _pt.OnCreateDialog = function(aoContext, aiId, out_aoDialog){
            elog('====jso_activity_cb====OnCreateDialog.begin====');
    }   //OnCreateDialog
    _pt.OnCreateContextMenu = function(aoContext, aoMenu, aoV, aoMenuInfo){
            elog('====jso_activity_cb====OnCreateContextMenu.begin====');
    }
    _pt.CreateNavigationBar = function(aoContext){
            elog('====jso_activity_cb====CreateNavigationBar.begin====');
    }
        //OnExchangeData : function(inObject_0, inObject_1, inObject_2, inObject_3, inObject_4, outObject_0, outObject_1, outObject_2, outObject_3, outObject_4) {
    _pt.OnExchangeData = function(aiTimes ,aoInObject, aoOutObject) {
            elog('====jso_activity_cb====OnExchangeData.begin====');
    }
    _pt.OnHandleMessage = function(aoContext, aoMessage){
            elog('====jso_activity_cb====OnHandleMessage.begin====');
    }
    //}   //return
    //--------ActivityListener end----------------

    var _js_activity = new CPhotoActivity();

    return new CActivityListener(_js_activity);
};  //module.exports