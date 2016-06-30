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
        elog(HideDetails);
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

//     if (mShowDetails) {
//         HideDetails();
//     }
//     else if (mShowMore) {
//         CloseMorePopupWindow();
//     }
//     else if (mShowZoom) {
//         CloseZoomPopupWindow();
//     }
//     else {
//         Activity::OnBackPressed();
//     }

//     return NOERROR;
// }
    }

// ECode CPhotoActivity::OpenZoomPopupWindow(
//     /* [in] */ IView* v)
// {
    _apt.OpenZoomPopupWindow = function(v) {
//     Logger::D(TAG, "OpenZoomPopupWindow()---");

//     AutoPtr<ILayoutInflater> inflater;
//     GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);

//     AutoPtr<IView> layout;
//     inflater->Inflate(R::layout::zoom_popup_dialog, NULL, (IView**)&layout);
//     AutoPtr<IView> view;
//     layout->FindViewById(R::id::zoom_seeker, (IView**)&view);
//     AutoPtr<ISeekBar> seekBar = ISeekBar::Probe(view);
//     assert(seekBar != NULL);
//     seekBar->SetOnSeekBarChangeListener((ISeekBarOnSeekBarChangeListener*)(mListener.Get()));

//     assert(mZoomPopupWindow.Get() == NULL);
//     CPopupWindow::New(layout, 400, 50, TRUE, (IPopupWindow**)&mZoomPopupWindow);
//     mZoomPopupWindow->SetTouchable(TRUE);
//     mZoomPopupWindow->SetOutsideTouchable(TRUE);

//     AutoPtr<IResources> res;
//     GetResources((IResources**)&res);
//     AutoPtr<IBitmapDrawable> bitmapDrawable;
//     CBitmapDrawable::New(res, (IBitmap*)NULL, (IBitmapDrawable**)&bitmapDrawable);
//     mZoomPopupWindow->SetBackgroundDrawable(IDrawable::Probe(bitmapDrawable));
//     mZoomPopupWindow->SetOnDismissListener((IPopupWindowOnDismissListener*)mListener.Get());
//     mZoomPopupWindow->ShowAtLocation(v, IGravity::NO_GRAVITY, 310, 561);
//     mShowZoom = TRUE;

//     return NOERROR;
// }
    }

// ECode CPhotoActivity::CloseZoomPopupWindow()
// {
    _apt.CloseZoomPopupWindow = function() {
//     Boolean isShow = FALSE;
//     if (mZoomPopupWindow != NULL && (mZoomPopupWindow->IsShowing(&isShow), isShow)) {
//         Logger::D(TAG, "CloseZoomPopupWindow()---");
//         mZoomPopupWindow->Dismiss();
//         mShowZoom = FALSE;
//     }
//     return NOERROR;
// }
    }

// ECode CPhotoActivity::OpenMorePopupWindow(
//     /* [in] */ IView* v)
// {
    _apt.OpenMorePopupWindow = function(v) {
//     Logger::D(TAG, "OpenMorePopupWindow()---");
//     AutoPtr<ILayoutInflater> inflater;
//     GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);

//     AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mListener.Get();
//     AutoPtr<IView> layout;
//     inflater->Inflate(R::layout::more_popup_dialog, NULL, (IView**)&layout);
//     AutoPtr<IView> view;
//     layout->FindViewById(R::id::more_pop_wallpaper, (IView**)&view);
//     AutoPtr<ITextView> popupWallpaper = ITextView::Probe(view);
//     assert(popupWallpaper != NULL);
//     popupWallpaper->SetOnClickListener(clickListener);

//     view = NULL;
//     layout->FindViewById(R::id::more_pop_info, (IView**)&view);
//     AutoPtr<ITextView> popupInfo = ITextView::Probe(view);
//     assert(popupInfo != NULL);
//     popupInfo->SetOnClickListener(clickListener);

//     assert(mMorePopupWindow.Get() == NULL);
//     CPopupWindow::New(layout, 80, 63, TRUE, (IPopupWindow**)&mMorePopupWindow);
//     mMorePopupWindow->SetTouchable(TRUE);
//     mMorePopupWindow->SetOutsideTouchable(TRUE);

//     AutoPtr<IResources> res;
//     GetResources((IResources**)&res);
//     AutoPtr<IBitmapDrawable> bitmapDrawable;
//     CBitmapDrawable::New(res, (IBitmap*)NULL, (IBitmapDrawable**)&bitmapDrawable);
//     mMorePopupWindow->SetBackgroundDrawable(IDrawable::Probe(bitmapDrawable));
//     mMorePopupWindow->SetOnDismissListener((IPopupWindowOnDismissListener*)mListener.Get());
//     mMorePopupWindow->ShowAtLocation(v, IGravity::NO_GRAVITY, 1180, 56);
//     mShowMore = TRUE;

//     return NOERROR;
// }
    }

// ECode CPhotoActivity::CloseMorePopupWindow()
// {
    _apt.CloseMorePopupWindow = function() {
//     Boolean isShow = FALSE;
//     if (mMorePopupWindow != NULL && (mMorePopupWindow->IsShowing(&isShow), isShow)) {
//         Logger::D(TAG, "CloseMorePopupWindow()---");
//         mMorePopupWindow->Dismiss();
//         mShowMore = FALSE;
//     }
//     return NOERROR;
// }
    }

// ECode CPhotoActivity::OpenShareDialog()
// {
    _apt.OpenShareDialog = function() {
//     Logger::D(TAG, "OpenShareDialog()--- : prev : %p", mShareDialog.Get());
//     assert(mShareDialog.Get() == 0);
//     CDialog::New(this, R::style::MyDialog, (IDialog**)&mShareDialog);
//     mShareDialog->SetOnDismissListener((IDialogInterfaceOnDismissListener*)mDialogListener.Get());
//     Logger::D(TAG, "OpenShareDialog()--- : new : %p", mShareDialog.Get());
//     mShareDialog->SetContentView(R::layout::share_dialog);

//     AutoPtr<IView> view;
//     mShareDialog->FindViewById(R::id::share_dialog_btn_cancel, (IView**)&view);
//     AutoPtr<IButton> cancelButton = IButton::Probe(view);
//     AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mListener.Get();
//     cancelButton->SetOnClickListener(clickListener);

//     mShareDialog->Show();
//     return NOERROR;
// }
    }

// ECode CPhotoActivity::ShareDialogOnOK()
// {
    _apt.ShareDialogOnOK = function() {
//     ShareDialogOnCancel(); //TODO
//     return NOERROR;
// }
    }

// ECode CPhotoActivity::ShareDialogOnCancel()
// {
    _apt.ShareDialogOnCancel = function() {
//     if (mShareDialog) {
//         Logger::D(TAG, "ShareDialogOnCancel---: %p", mShareDialog.Get());
//         mShareDialog->Cancel();
//     }
//     return NOERROR;
// }
    }

// ECode CPhotoActivity::DeleteDialogOnOK()
// {
    _apt.DeleteDialogOnOK = function () {
//     DeleteDialogOnCancel(); //TODO
//     return NOERROR;
// }
    }

// ECode CPhotoActivity::DeleteDialogOnCancel()
// {
    _apt.DeleteDialogOnCancel = function() {
//     if (mDeleteDialog) {
//         Logger::D(TAG, "DeleteDialogOnCancel---: %p", mDeleteDialog.Get());
//         mDeleteDialog->Cancel();
//     }
//     return NOERROR;
// }
    }

// ECode CPhotoActivity::OpenDeleteDialog()
// {
    _apt.OpenDeleteDialog = function() {
//     Logger::D(TAG, "OpenDeleteDialog()--- : prev : %p", mDeleteDialog.Get());
//     assert(mDeleteDialog.Get() == 0);
//     CDialog::New(this, R::style::MyDialog, (IDialog**)&mDeleteDialog);
//     mDeleteDialog->SetOnDismissListener((IDialogInterfaceOnDismissListener*)mDialogListener.Get());
//     Logger::D(TAG, "OpenShareDialog()--- : new : %p", mDeleteDialog.Get());
//     mDeleteDialog->SetContentView(R::layout::delete_dialog);

//     AutoPtr<IView> view;
//     mDeleteDialog->FindViewById(R::id::delete_dialog_btn_ok, (IView**)&view);
//     AutoPtr<IButton> okButton = IButton::Probe(view);
//     AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mListener.Get();
//     okButton->SetOnClickListener(clickListener);

//     view = NULL;
//     mDeleteDialog->FindViewById(R::id::delete_dialog_btn_cancel, (IView**)&view);
//     AutoPtr<IButton> cancelButton = IButton::Probe(view);
//     cancelButton->SetOnClickListener(clickListener);

//     mDeleteDialog->Show();
//     return NOERROR;
// }
    }

// void CPhotoActivity::ShowDetails()
// {
    _apt.ShowDetails = function () {
//     mShowDetails = TRUE;
//     if (mDetailsHelper == NULL) {
//         mDetailsHelper = new DetailsHelper(this, mDetailsArray);
//         AutoPtr<ICloseListener> closeListener = (ICloseListener*)mListener.Get();
//         mDetailsHelper->SetCloseListener(closeListener);
//     }
//     mDetailsHelper->Show();
// }
    }

// void CPhotoActivity::HideDetails()
// {
    _apt.HideDetails = function () {
//     mShowDetails = FALSE;
//     mDetailsHelper->Hide();
// }
    }

// void CPhotoActivity::SetCurrentPhoto()
// {
    _apt.SetCurrentPhoto = function () {
//     Logger::D(TAG, "SetCurrentPhoto()---");
//     if (mCurrentIndex > 0) {
//         mLeftButton->SetImageResource(R::drawable::photo_arrow_left);
//     }
//     else {
//         mLeftButton->SetImageResource(R::drawable::photo_arrow_left_disable);
//     }
//     if (mCurrentIndex >= (mTotalSize - 1)) {
//         mRightButton->SetImageResource(R::drawable::photo_arrow_right_disable);
//     }
//     else {
//         mRightButton->SetImageResource(R::drawable::photo_arrow_right);
//     }

//     mLeftButton->SetVisibility(mViewSingleItem ? IView::INVISIBLE : IView::VISIBLE);
//     mRightButton->SetVisibility(mViewSingleItem ? IView::INVISIBLE : IView::VISIBLE);

//     AutoPtr<PhotoEntry> entry = mPhotoEntryList[mCurrentIndex];
//     AutoPtr<ICharSequence> cs;
//     CStringWrapper::New(entry->desc, (ICharSequence**)&cs);
//     mPhotoName->SetText(cs);
//     mCurrentImagePath = entry->sourcePath;

//     Boolean isNeed = AsyncImageLoader::NeedLoadHighDrawable(mCurrentImagePath);
//     if (isNeed) {
//         LoadCurrentImage(mCurrentImagePath, TRUE);
//     }
//     else {
//         LoadCurrentImage(mCurrentImagePath, FALSE);
//     }

//     SetDetailInfo(mCurrentImagePath);
// }
    }

// void CPhotoActivity::ChangeCurrentPhoto(
//     /* [in] */ Boolean isNext)
// {
    _apt.ChangeCurrentPhoto = function () {
//     if (isNext) {
//         Int32 tmp = mTotalSize - 1;
//         if (mCurrentIndex >= tmp) return;
//         mCurrentIndex++;
//         mCurrentIndex = tmp > mCurrentIndex ? mCurrentIndex : tmp;
//     }
//     else {
//         if (mCurrentIndex <= 0) return;
//         mCurrentIndex--;
//         mCurrentIndex = mCurrentIndex > 0 ? mCurrentIndex : 0;
//     }
//     SetCurrentPhoto();
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
//     if (!mCurrentImagePath.Equals(path)) {
//         Logger::W(TAG, "MyImageLoaded()---not current path, drop!");
//         return;
//     }

//     SwitchDisplayView(FALSE);
//     Int32 w = 0;
//     Int32 h = 0;

//     if (isHigh) {
//         Boolean ret = AsyncImageLoader::GetOrigionWidthAndHeight(path, &w, &h);
//         if (!ret) {
//             Logger::W(TAG, "MyImageLoaded()---GetOrigionWidthAndHeight---failed!");
//             imageDrawable->GetIntrinsicWidth(&w);
//             imageDrawable->GetIntrinsicHeight(&h);
//         }
//     }
//     else {
//         imageDrawable->GetIntrinsicWidth(&w);
//         imageDrawable->GetIntrinsicHeight(&h);
//     }
//     Logger::D(TAG, "MyImageLoaded()---w:%d,h:%d", w, h);
//     (*mDetailsArray)[2] = StringUtils::Int32ToString(w);
//     (*mDetailsArray)[3] = StringUtils::Int32ToString(h);
//     imageView->SetImageDrawable(imageDrawable);
// }
    }

// void CPhotoActivity::SetDetailInfo(
//     /* [in] */ const String& filePath)
// {
    _apt.SetDetailInfo = function(filePath) {
//     (*mDetailsArray)[0] = filePath.Substring(filePath.LastIndexOf(DataSourceHelper::PATH_SPLITE) + 1,
//         filePath.LastIndexOf("."));
//     (*mDetailsArray)[5] = filePath;
//     Logger::D(TAG, "SetDetailInfo()---filePath:%s", filePath.string());
//     AutoPtr<IFile> file;
//     CFile::New(filePath, (IFile**)&file);
//     if (file != NULL) {
//         Int64 last = 0;
//         file->LastModified(&last);
//         AutoPtr<IDate> date;
//         CDate::New(last, (IDate**)&date);
//         AutoPtr<ISimpleDateFormat> sdf;
//         CSimpleDateFormat::New(String("yyyy-MM-dd HH:mm"), (ISimpleDateFormat**)&sdf);
//         String str;
//         sdf->FormatDate(date, &str);
//         (*mDetailsArray)[1] = str;
//         Int64 len = 0;
//         file->GetLength(&len);
//         StringBuilder sizeStr;
//         Int32 n = len / 1024;
//         if (n > 0) {
//             Int32 i = len / (1024 * 1024);
//             if (i > 0) {
//                 Float j = (((Float)(len % (1024 * 1024))) / (1024 * 1024)) * 10;
//                 Int32 m = Elastos::Core::Math::Round(j);
//                 sizeStr += StringUtils::Int32ToString(i);
//                 sizeStr += ".";
//                 sizeStr += StringUtils::Int32ToString(m);
//                 sizeStr += "MB";
//             }
//             else {
//                 Float j = (((Float)(len % 1024)) / 1024) * 10;
//                 Int32 m = Elastos::Core::Math::Round(j);
//                 sizeStr += StringUtils::Int32ToString(n);
//                 sizeStr += ".";
//                 sizeStr += StringUtils::Int32ToString(m);
//                 sizeStr += "KB";
//             }
//         }
//         else {
//             sizeStr += StringUtils::Int64ToString(len);
//             sizeStr += "B";
//         }
//         (*mDetailsArray)[4] = sizeStr.ToString();
//     }
// }
    }

// void CPhotoActivity::LoadCurrentImage(
//     /* [in] */ const String& path,
//     /* [in] */ Boolean isHighQuality)
// {
    _apt.LoadCurrentImage = function(path,isHighQuality) {
//     Logger::D(TAG, "LoadCurrentImage()---isHighQuality:%d", isHighQuality);
//     SwitchDisplayView(FALSE);
//     if (isHighQuality) {
//         AutoPtr<IWeakReference> weakHost;
//         GetWeakReference((IWeakReference**)&weakHost);
//         assert(weakHost != NULL);
//         AutoPtr<MyLoadImageCallback> myLoadImage = new MyLoadImageCallback(weakHost, path, TRUE);
//         AutoPtr<IBitmapDrawable> drawable = AsyncImageLoader::LoadDrawable(path, TRUE, mPhotoView, myLoadImage);
//         if (drawable != NULL) {
//             mPhotoView->SetImageDrawable(IDrawable::Probe(drawable));
//             Int32 w = 0;
//             Int32 h = 0;
//             Boolean ret = AsyncImageLoader::GetOrigionWidthAndHeight(path, &w, &h);
//             if (ret) {
//                 Logger::D(TAG, "LoadCurrentImage()---HighQuality---w:%d,h:%d", w, h);
//                 (*mDetailsArray)[2] = StringUtils::Int32ToString(w);
//                 (*mDetailsArray)[3] = StringUtils::Int32ToString(h);
//             }
//             else {
//                 Logger::W(TAG, "LoadCurrentImage()---GetOrigionWidthAndHeight--failed!");
//             }
//         }
//         else {
//             SwitchDisplayView(TRUE);
//         }
//     }
//     else {
//         AutoPtr<IWeakReference> weakHost;
//         GetWeakReference((IWeakReference**)&weakHost);
//         assert(weakHost != NULL);
//         AutoPtr<MyLoadImageCallback> myLoadImage = new MyLoadImageCallback(weakHost, path, FALSE);
//         AutoPtr<IBitmapDrawable> drawable = AsyncImageLoader::LoadDrawable(path, FALSE, mPhotoView, myLoadImage);
//         if (drawable != NULL) {
//             mPhotoView->SetImageDrawable(IDrawable::Probe(drawable));
//             Int32 w = 0;
//             Int32 h = 0;
//             drawable->GetIntrinsicWidth(&w);
//             drawable->GetIntrinsicHeight(&h);
//             Logger::D(TAG, "LoadCurrentImage()---w:%d,h:%d", w, h);
//             (*mDetailsArray)[2] = StringUtils::Int32ToString(w);
//             (*mDetailsArray)[3] = StringUtils::Int32ToString(h);
//         }
//         else {
//             SwitchDisplayView(TRUE);
//         }
//     }
// }
    }

// void CPhotoActivity::SwitchDisplayView(
//     /* [in] */ Boolean showLoading)
// {
    _apt.SwitchDisplayView = function(showLoading) {
//     if (showLoading) {
//         mPhotoView->SetVisibility(IView::INVISIBLE);
//         mPhotoLoadingText->SetVisibility(IView::VISIBLE);
//         TriggerLoadingCallback(FALSE);
//     }
//     else {
//         mPhotoLoadingText->SetVisibility(IView::INVISIBLE);
//         mPhotoView->SetVisibility(IView::VISIBLE);
//         UpdateLoadingStatus(TRUE);
//     }
// }
    }

// void CPhotoActivity::UpdateLoadingStatus(
//     /* [in] */ Boolean needReload)
// {
    _apt.UpdateLoadingStatus = function(needReload) {
//     AutoPtr<ICharSequence> cs;
//     StringBuilder newText("图片加载中 ");
//     if (needReload) {
//         TriggerLoadingCallback(TRUE);
//         newText += "...";
//         cs = newText.ToCharSequence();
//         mPhotoLoadingText->SetText(cs);
//     }
//     else {
//         Int32 ret = IView::GONE;
//         mPhotoLoadingText->GetVisibility(&ret);
//         if (ret == IView::VISIBLE) {
//             mPhotoLoadingText->GetText((ICharSequence**)&cs);
//             String oldText;
//             cs->ToString(&oldText);
//             Int32 firstIndex = oldText.IndexOf(".");
//             Int32 lastIndex = oldText.LastIndexOf(".");
//             Int32 num = lastIndex - firstIndex;
//             Logger::D(TAG, "UpdateLoadingStatus()---oldText:%s,firstIndex:%d,num:%d", oldText.string(), firstIndex, num);
//             if (num == 0) {
//                 if (firstIndex > 0) {
//                     newText += ".. ";
//                 }
//                 else {
//                     newText += ".  ";
//                 }
//             }
//             else if (num == 1) {
//                 newText += "...";
//             }
//             else if (num == 2) {
//                 newText += "   ";
//             }
//             cs = newText.ToCharSequence();
//             mPhotoLoadingText->SetText(cs);
//             TriggerLoadingCallback(FALSE);
//         }
//         else {
//             Logger::D(TAG, "UpdateLoadingStatus()---stop:%d", ret);
//         }
//     }
// }
    }

// void CPhotoActivity::TriggerLoadingCallback(
//     /* [in] */ Boolean isCancel)
// {
    _apt.TriggerLoadingCallback = function(isCancel) {
//     Logger::D(TAG, "TriggerLoadingCallback()---isCancel:%d", isCancel);
//     if (isCancel) {
//         mHandler->RemoveMessages(MSG_UPDATE_LOADING_STATUS);
//     }
//     else {
//         Boolean needReload = FALSE;
//         AutoPtr<IMessage> msg;
//         mMyHandler->ObtainMessageEx2(MSG_UPDATE_LOADING_STATUS,
//             needReload ? 1 : 0, 0, (IMessage**)&msg);
//         Boolean result;
//         mMyHandler->SendMessageDelayed(msg, 1000, &result);
//     }
// }
    }

// ECode CPhotoActivity::OnNewIntent(
//     /* [in] */ IIntent *intent)
// {
    _apt.OnNewIntent = function(intent) {
//     mPhotoEntryList.Clear();
//     if (!LoadPhotoEntryList(intent))
//         return NOERROR;

//     mUpButton->SetVisibility(mViewSingleItem ? IView::INVISIBLE : IView::VISIBLE);
//     mPhotoSlideshow->SetVisibility(mViewSingleItem ? IView::INVISIBLE : IView::VISIBLE);

//     SetCurrentPhoto();
//     return NOERROR;
// }
    }

// Boolean CPhotoActivity::LoadPhotoEntryList(
//     /* [in] */ IIntent* intent)
// {
    _apt.LoadPhotoEntryList = function(intent) {
//     if (intent != NULL) {
//         String action;
//         String filePath;
//         intent->GetAction(&action);
//         Logger::D(TAG, "action:%s", action.string());
//         AutoPtr<IUri> uri;
//         intent->GetData((IUri**)&uri);
//         if (uri != NULL) {
//             String scheme;
//             uri->GetScheme(&scheme);
//             if (scheme.Equals("file")) {
//                 uri->GetPath(&filePath);
//                 Boolean isImage = DataSourceHelper::IsImageFile(filePath);
//                 if (isImage) {
//                     AutoPtr<PhotoEntry> entry = new PhotoEntry();
//                     entry->sourcePath = filePath;
//                     entry->desc = filePath.Substring(filePath.LastIndexOf(DataSourceHelper::PATH_SPLITE) + 1,
//                         filePath.GetLength());
//                     Logger::D(TAG, "single photo, path:%s, desc:%s", entry->sourcePath.string(), entry->desc.string());
//                     mPhotoEntryList.PushBack(entry);
//                     mCurrentIndex = 0;
//                     mTotalSize = mPhotoEntryList.GetSize();
//                     mViewSingleItem = TRUE;
//                 }
//                 else {
//                     Logger::W(TAG, "image type is not support! filePath:%s", filePath.string());
//                     Utils::MakeToast(this, String("不支持显示该类型的文件！"), IToast::LENGTH_LONG);
//                     Finish();
//                     return FALSE;
//                 }
//             }
//             else {
//                 Logger::W(TAG, "scheme is not a file! scheme:%s", scheme.string());
//             }
//         }
//         else {
//             intent->GetStringExtra(DataSourceHelper::SOURCE_PATH, &filePath);
//             if (!filePath.IsNullOrEmpty()) {
//                 intent->GetInt32Extra(DataSourceHelper::SOURCE_INDEX, 0, &mCurrentIndex);
//                 mFolderPath = filePath.Substring(0, filePath.LastIndexOf(DataSourceHelper::PATH_SPLITE));
//                 AutoPtr<List<String> > imageItems = DataSourceHelper::GetItemList(mFolderPath);
//                 if (imageItems != NULL) {
//                     AutoPtr<PhotoEntry> entry;
//                     List<String>::Iterator it = imageItems->Begin();
//                     for (Int32 i = 0; it != imageItems->End(); ++it, ++i) {
//                         entry = new PhotoEntry();
//                         entry->sourcePath = mFolderPath;
//                         entry->sourcePath += DataSourceHelper::PATH_SPLITE;
//                         entry->sourcePath += *it;
//                         entry->desc = (*it).Substring(0, (*it).GetLength());
//                         Logger::D(TAG, " > %d, path:%s, desc:%s", i, entry->sourcePath.string(), entry->desc.string());
//                         mPhotoEntryList.PushBack(entry);
//                     }
//                     mTotalSize = mPhotoEntryList.GetSize();
//                     mViewSingleItem = FALSE;
//                 }
//                 else {
//                     Logger::W(TAG, "imageItems is null!");
//                 }
//             }
//         }
//     }

//     if (mPhotoEntryList.IsEmpty()) {
//         Logger::W(TAG, "mPhotoEntryList size is zero!");
//         Utils::MakeToast(this, String("没有图片文件！"), IToast::LENGTH_LONG);
//         Finish();
//         return FALSE;
//     }

//     return TRUE;
// }
    }

    //--------Activity end----------------

    //--------ActivityListener begin----------------

    //return {
    function CActivityListener (ao_host) {
        this.host = ao_host;
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