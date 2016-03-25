
#include "CPhotoActivity.h"
#include "src/data/DataSourceHelper.h"
#include "src/data/AsyncImageLoader.h"
#include "src/util/Utils.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include "elastos/droid/os/SomeArgs.h"

using Elastos::Droid::Os::SomeArgs;
using Elastos::Core::CStringWrapper;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Droid::App::CDialog;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawable;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::Widget::CPopupWindow;
using Elastos::Droid::Widget::EIID_IPopupWindowOnDismissListener;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IGridView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::App::EIID_IActivity;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::CDate;
using Elastos::Utility::IDate;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String CPhotoActivity::TAG("CPhotoActivity");
const Int32 CPhotoActivity::MSG_IMAGE_LOADED = 0;
const Int32 CPhotoActivity::MSG_UPDATE_LOADING_STATUS = 1;

ECode CPhotoActivity::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IActivity> strongObj;
    mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
    if (strongObj == NULL) {
        return NOERROR;
    }

    CPhotoActivity* mHost = (CPhotoActivity*)strongObj.Get();

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch(what) {
        case CPhotoActivity::MSG_IMAGE_LOADED: {
            SomeArgs* args = (SomeArgs*)obj.Get();
            IDrawable* drawable = IDrawable::Probe(args->mArg1);
            IImageView* imageView = IImageView::Probe(args->mArg2);
            ICharSequence* seq = ICharSequence::Probe(args->mArg3);
            String path;
            seq->ToString(&path);
            Boolean isHigh = args->mArgi1 != 0;
            mHost->MyImageLoaded(drawable, imageView, path, isHigh);
            args->Recycle();
            break;
        }
        case CPhotoActivity::MSG_UPDATE_LOADING_STATUS: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->UpdateLoadingStatus(arg1 == 1);
            break;
        }
    }

    return NOERROR;
}

CPhotoActivity::MyListener::MyListener(
    /* [in] */ CPhotoActivity* host)
    : mHost(host)
{}

PInterface CPhotoActivity::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return this;
    }
    else if (riid == EIID_IPopupWindowOnDismissListener) {
        return this;
    }
    else if (riid == EIID_ISeekBarOnSeekBarChangeListener) {
        return this;
    }
    else if (riid == EIID_ICloseListener) {
        return (PInterface)this;
    }

    return NULL;
}

UInt32 CPhotoActivity::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CPhotoActivity::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CPhotoActivity::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else if (pObject == (IInterface*)this) {
        *pIID = EIID_IPopupWindowOnDismissListener;
    }
    else if (pObject == (IInterface*)this) {
        *pIID = EIID_ISeekBarOnSeekBarChangeListener;
    }
    else if (pObject == (IInterface*)this) {
        *pIID = EIID_ICloseListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CPhotoActivity::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 viewId = 0;
    v->GetId(&viewId);

    switch(viewId) {
        case R::id::photo_up: {
            Logger::D(TAG, "OnClick()---photo_up");
            mHost->OnBackPressed();
            break;
        }
        case R::id::photo_slideshow: {
            Logger::D(TAG, "OnClick()---photo_slideshow");
            break;
        }
        case R::id::photo_more: {
            Logger::D(TAG, "OnClick()---photo_more");
            if (mHost->mShowMore) {
                mHost->CloseMorePopupWindow();
            }
            else {
                mHost->OpenMorePopupWindow(v);
            }
            break;
        }
        case R::id::photo_detail: {
            Logger::D(TAG, "OnClick()---photo_detail");
            break;
        }
        case R::id::img_btn_left: {
            Logger::D(TAG, "OnClick()---img_btn_left--mCurrentIndex:%d",mHost->mCurrentIndex);
            mHost->ChangeCurrentPhoto(FALSE);
            break;
        }
        case R::id::img_btn_right: {
            Logger::D(TAG, "OnClick()---img_btn_right--mCurrentIndex:%d",mHost->mCurrentIndex);
            mHost->ChangeCurrentPhoto(TRUE);
            break;
        }
        case R::id::img_btn_zoom: {
            Logger::D(TAG, "OnClick()---img_btn_zoom");
            if (mHost->mShowZoom) {
                mHost->CloseZoomPopupWindow();
            }
            else {
                mHost->OpenZoomPopupWindow(v);
            }
            break;
        }
        case R::id::img_btn_rotate_left: {
            Logger::D(TAG, "OnClick()---img_btn_rotate_left");
            break;
        }
        case R::id::img_btn_rotate_right: {
            Logger::D(TAG, "OnClick()---img_btn_rotate_right");
            break;
        }
        case R::id::img_btn_share: {
            Logger::D(TAG, "OnClick()---img_btn_share");
            mHost->OpenShareDialog();
            break;
        }
        case R::id::img_btn_edit: {
            Logger::D(TAG, "OnClick()---img_btn_edit");
            AutoPtr<PhotoEntry> entry = mHost->mPhotoEntryList[mHost->mCurrentIndex];
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetClassName(String("Gallery"), String("Gallery.CEditActivity"));
            intent->PutExtra(DataSourceHelper::SOURCE_PATH, entry->sourcePath);
            if (FAILED(mHost->StartActivity(intent))) {
                Logger::E(TAG, "OnClick()---StartActivity CEditActivity failed!");
            }
            break;
        }
        case R::id::img_btn_delete: {
            Logger::D(TAG, "OnClick()---img_btn_delete");
            mHost->OpenDeleteDialog();
            break;
        }
        case R::id::more_pop_wallpaper: {
            Logger::D(TAG, "OnClick()---more_pop_wallpaper");
            mHost->CloseMorePopupWindow();
            AutoPtr<PhotoEntry> entry = mHost->mPhotoEntryList[mHost->mCurrentIndex];
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetClassName(String("Gallery"), String("Gallery.CWallpaperActivity"));
            intent->PutExtra(DataSourceHelper::SOURCE_PATH, entry->sourcePath);
            if (FAILED(mHost->StartActivity(intent))) {
                Logger::E(TAG, "OnClick()---StartActivity CWallpaperActivity failed!");
            }
            break;
        }
        case R::id::more_pop_info: {
            Logger::D(TAG, "OnClick()---more_pop_info");
            mHost->CloseMorePopupWindow();
            if (mHost->mShowDetails) {
                mHost->HideDetails();
            }
            else {
                mHost->ShowDetails();
            }
            break;
        }
        case R::id::share_dialog_btn_cancel: {
            Logger::D(TAG, "OnClick()---share_dialog_btn_cancel");
            mHost->ShareDialogOnCancel();
            break;
        }
        case R::id::delete_dialog_btn_ok: {
            Logger::D(TAG, "OnClick()---delete_dialog_btn_ok");
            mHost->DeleteDialogOnOK();
            break;
        }
        case R::id::delete_dialog_btn_cancel: {
            Logger::D(TAG, "OnClick()---delete_dialog_btn_cancel");
            mHost->DeleteDialogOnCancel();
            break;
        }
        default:
            break;
    }

    return NOERROR;
}

ECode CPhotoActivity::MyListener::OnDismiss()
{
    Logger::D(TAG, "OnDismiss()---: TODO");
    if (mHost->mZoomPopupWindow) {
        mHost->mZoomPopupWindow = NULL;
        mHost->mShowZoom = FALSE;
    }

    if (mHost->mMorePopupWindow) {
        mHost->mMorePopupWindow = NULL;
        mHost->mShowMore = FALSE;
    }
    return NOERROR;
}

ECode CPhotoActivity::MyListener::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    Logger::D(TAG, "OnProgressChanged()---progress:%d", progress);
    return NOERROR;
}

ECode CPhotoActivity::MyListener::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    Logger::D(TAG, "OnStartTrackingTouch()---");
    return NOERROR;
}

ECode CPhotoActivity::MyListener::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    Logger::D(TAG, "OnStopTrackingTouch()---");
    return NOERROR;
}

ECode CPhotoActivity::MyListener::OnClose()
{
    Logger::D(TAG, "OnClose()---");
    mHost->HideDetails();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CPhotoActivity::DialogListener, IDialogInterfaceOnDismissListener)

CPhotoActivity::DialogListener::DialogListener(
    /* [in] */ CPhotoActivity* host)
    : mHost(host)
{
}

ECode CPhotoActivity::DialogListener::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    if (IDialogInterface::Probe(mHost->mShareDialog) == dialog) {
        Logger::D(TAG, "OnDismissShareDialog---: %p", mHost->mShareDialog.Get());
        mHost->mShareDialog = NULL;
    }
    else if (IDialogInterface::Probe(mHost->mDeleteDialog) == dialog) {
        Logger::D(TAG, "OnDismissDeleteDialog---: %p", mHost->mDeleteDialog.Get());
        mHost->mDeleteDialog = NULL;
    }

    return NOERROR;
}

CPhotoActivity::MyLoadImageCallback::MyLoadImageCallback(
    /* [in] */ IWeakReference* host,
    /* [in] */ const String& path,
    /* [in] */ Boolean isHigh)
    : mWeakHost(host)
    , mPath(path)
    , mIsHigh(isHigh)
{}

ECode CPhotoActivity::MyLoadImageCallback::ImageLoaded(
    /* [in] */ IDrawable* imageDrawable,
    /* [in] */ IImageView* imageView)
{
    Logger::D(TAG, "ImageLoaded()-----");
    AutoPtr<IActivity> strongObj;
    mWeakHost->Resolve(EIID_IActivity, (IInterface**)&strongObj);
    if (strongObj == NULL) {
        return NOERROR;
    }

    CPhotoActivity* mHost = (CPhotoActivity*)strongObj.Get();
    if (mHost->mMyHandler == NULL) {
        return NOERROR;
    }

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = imageDrawable;
    args->mArg2 = imageView;
    AutoPtr<ICharSequence> seq;
    CStringWrapper::New(mPath, (ICharSequence**)&seq);
    args->mArg3 = seq;
    args->mArgi1 = mIsHigh ? 1 : 0;

    AutoPtr<IMessage> msg;
    mHost->mMyHandler->ObtainMessage(CPhotoActivity::MSG_IMAGE_LOADED, args, (IMessage**)&msg);
    Boolean result;
    return mHost->mMyHandler->SendMessage(msg, &result);
}

CPhotoActivity::CPhotoActivity()
    : mIndex(0)
    , mCurrentIndex(0)
    , mTotalSize(0)
    , mDisplayWidth(0)
    , mDisplayHeight(0)
    , mShowMore(FALSE)
    , mShowZoom(FALSE)
    , mShowDetails(FALSE)
    , mViewSingleItem(FALSE)
{
    Logger::D(TAG, "CPhotoActivity(): %p", this);
}

CPhotoActivity::~CPhotoActivity()
{
    Logger::D(TAG, "~CPhotoActivity(): %p", this);
}

ECode CPhotoActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::D(TAG, "OnCreate()---");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_photo);

    AutoPtr<IWeakReference> weakHost;
    GetWeakReference((IWeakReference**)&weakHost);
    mMyHandler = new MyHandler(weakHost);

    mListener = new MyListener(this);
    mDialogListener = new DialogListener(this);
    mDetailsArray = ArrayOf<String>::Alloc(6);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    if (!LoadPhotoEntryList(intent))
        return NOERROR;

    AutoPtr<IView> view = FindViewById(R::id::photo_up);
    mUpButton = IImageView::Probe(view);
    assert(mUpButton != NULL);

    view = FindViewById(R::id::photo_name);
    mPhotoName = ITextView::Probe(view);
    assert(mPhotoName != NULL);

    view = FindViewById(R::id::photo_slideshow);
    mPhotoSlideshow = IImageView::Probe(view);
    assert(mPhotoSlideshow != NULL);

    view = FindViewById(R::id::photo_more);
    mPhotoMore = IImageView::Probe(view);
    assert(mPhotoMore != NULL);

    view = FindViewById(R::id::photo_detail);
    mPhotoView = IImageView::Probe(view);
    assert(mPhotoView != NULL);

    view = FindViewById(R::id::photo_loading);
    mPhotoLoadingText = ITextView::Probe(view);
    assert(mPhotoLoadingText != NULL);

    view = FindViewById(R::id::img_btn_left);
    mLeftButton = IImageButton::Probe(view);
    assert(mLeftButton != NULL);

    view = FindViewById(R::id::img_btn_right);
    mRightButton = IImageButton::Probe(view);
    assert(mRightButton != NULL);

    view = FindViewById(R::id::img_btn_zoom);
    mZoomButton = IImageButton::Probe(view);
    assert(mZoomButton != NULL);

    view = FindViewById(R::id::img_btn_rotate_left);
    mRotateLeftButton = IImageButton::Probe(view);
    assert(mRotateLeftButton != NULL);

    view = FindViewById(R::id::img_btn_rotate_right);
    mRotateRightButton = IImageButton::Probe(view);
    assert(mRotateRightButton != NULL);

    view = FindViewById(R::id::img_btn_share);
    mShareButton = IImageButton::Probe(view);
    assert(mShareButton != NULL);

    view = FindViewById(R::id::img_btn_edit);
    mEditButton = IImageButton::Probe(view);
    assert(mEditButton != NULL);

    view = FindViewById(R::id::img_btn_delete);
    mDeleteButton = IImageButton::Probe(view);
    assert(mDeleteButton != NULL);

    AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mListener.Get();
    mUpButton->SetOnClickListener(clickListener);
    mPhotoSlideshow->SetOnClickListener(clickListener);
    mPhotoMore->SetOnClickListener(clickListener);
    mLeftButton->SetOnClickListener(clickListener);
    mRightButton->SetOnClickListener(clickListener);
    mZoomButton->SetOnClickListener(clickListener);
    mRotateLeftButton->SetOnClickListener(clickListener);
    mRotateRightButton->SetOnClickListener(clickListener);
    mShareButton->SetOnClickListener(clickListener);
    mEditButton->SetOnClickListener(clickListener);
    mDeleteButton->SetOnClickListener(clickListener);

    if (mViewSingleItem) {
        mUpButton->SetVisibility(IView::INVISIBLE);
        mPhotoSlideshow->SetVisibility(IView::INVISIBLE);
    }

    // for test
    AutoPtr<IDisplayMetrics> dm;
    AutoPtr<IDisplay> dis;
    CDisplayMetrics::New((IDisplayMetrics**)&dm);
    GetWindowManager()->GetDefaultDisplay((IDisplay**)&dis);
    dis->GetMetrics(dm);
    Int32 widthPixels = 0, heightPixels = 0;
    dm->GetWidthPixels(&widthPixels);
    dm->GetHeightPixels(&heightPixels);
    mDisplayWidth = widthPixels - 128;
    mDisplayHeight = heightPixels - 128;
    Logger::D(TAG, "OnCreate()---widthPixels:%d,heightPixels:%d,mDisplayWidth:%d,mDisplayHeight:%d",
        widthPixels, heightPixels, mDisplayWidth, mDisplayHeight);

    SetCurrentPhoto();
    return NOERROR;
}

ECode CPhotoActivity::OnStart()
{
    Logger::D(TAG, "OnStart()---");
    Activity::OnStart();
    return NOERROR;
}

ECode CPhotoActivity::OnResume()
{
    Logger::D(TAG, "OnResume()---");
    Activity::OnResume();
    return NOERROR;
}

ECode CPhotoActivity::OnPause()
{
    Logger::D(TAG, "OnPause()---");
    Activity::OnPause();
    return NOERROR;
}

ECode CPhotoActivity::OnStop()
{
    Logger::D(TAG, "OnStop()---");
    Activity::OnStop();
    return NOERROR;
}

ECode CPhotoActivity::OnDestroy()
{
    Logger::D(TAG, "OnDestroy()---");
    Activity::OnDestroy();
    return NOERROR;
}

ECode CPhotoActivity::OnBackPressed()
{
    Logger::D(TAG, "OnBackPressed()---");

    if (mShowDetails) {
        HideDetails();
    }
    else if (mShowMore) {
        CloseMorePopupWindow();
    }
    else if (mShowZoom) {
        CloseZoomPopupWindow();
    }
    else {
        Activity::OnBackPressed();
    }

    return NOERROR;
}

ECode CPhotoActivity::OpenZoomPopupWindow(
    /* [in] */ IView* v)
{
    Logger::D(TAG, "OpenZoomPopupWindow()---");

    AutoPtr<ILayoutInflater> inflater;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);

    AutoPtr<IView> layout;
    inflater->Inflate(R::layout::zoom_popup_dialog, NULL, (IView**)&layout);
    AutoPtr<IView> view;
    layout->FindViewById(R::id::zoom_seeker, (IView**)&view);
    AutoPtr<ISeekBar> seekBar = ISeekBar::Probe(view);
    assert(seekBar != NULL);
    seekBar->SetOnSeekBarChangeListener((ISeekBarOnSeekBarChangeListener*)(mListener.Get()));

    assert(mZoomPopupWindow.Get() == NULL);
    CPopupWindow::New(layout, 400, 50, TRUE, (IPopupWindow**)&mZoomPopupWindow);
    mZoomPopupWindow->SetTouchable(TRUE);
    mZoomPopupWindow->SetOutsideTouchable(TRUE);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IBitmapDrawable> bitmapDrawable;
    CBitmapDrawable::New(res, (IBitmap*)NULL, (IBitmapDrawable**)&bitmapDrawable);
    mZoomPopupWindow->SetBackgroundDrawable(IDrawable::Probe(bitmapDrawable));
    mZoomPopupWindow->SetOnDismissListener((IPopupWindowOnDismissListener*)mListener.Get());
    mZoomPopupWindow->ShowAtLocation(v, IGravity::NO_GRAVITY, 310, 561);
    mShowZoom = TRUE;

    return NOERROR;
}

ECode CPhotoActivity::CloseZoomPopupWindow()
{
    Boolean isShow = FALSE;
    if (mZoomPopupWindow != NULL && (mZoomPopupWindow->IsShowing(&isShow), isShow)) {
        Logger::D(TAG, "CloseZoomPopupWindow()---");
        mZoomPopupWindow->Dismiss();
        mShowZoom = FALSE;
    }
    return NOERROR;
}

ECode CPhotoActivity::OpenMorePopupWindow(
    /* [in] */ IView* v)
{
    Logger::D(TAG, "OpenMorePopupWindow()---");
    AutoPtr<ILayoutInflater> inflater;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);

    AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mListener.Get();
    AutoPtr<IView> layout;
    inflater->Inflate(R::layout::more_popup_dialog, NULL, (IView**)&layout);
    AutoPtr<IView> view;
    layout->FindViewById(R::id::more_pop_wallpaper, (IView**)&view);
    AutoPtr<ITextView> popupWallpaper = ITextView::Probe(view);
    assert(popupWallpaper != NULL);
    popupWallpaper->SetOnClickListener(clickListener);

    view = NULL;
    layout->FindViewById(R::id::more_pop_info, (IView**)&view);
    AutoPtr<ITextView> popupInfo = ITextView::Probe(view);
    assert(popupInfo != NULL);
    popupInfo->SetOnClickListener(clickListener);

    assert(mMorePopupWindow.Get() == NULL);
    CPopupWindow::New(layout, 80, 63, TRUE, (IPopupWindow**)&mMorePopupWindow);
    mMorePopupWindow->SetTouchable(TRUE);
    mMorePopupWindow->SetOutsideTouchable(TRUE);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IBitmapDrawable> bitmapDrawable;
    CBitmapDrawable::New(res, (IBitmap*)NULL, (IBitmapDrawable**)&bitmapDrawable);
    mMorePopupWindow->SetBackgroundDrawable(IDrawable::Probe(bitmapDrawable));
    mMorePopupWindow->SetOnDismissListener((IPopupWindowOnDismissListener*)mListener.Get());
    mMorePopupWindow->ShowAtLocation(v, IGravity::NO_GRAVITY, 1180, 56);
    mShowMore = TRUE;

    return NOERROR;
}

ECode CPhotoActivity::CloseMorePopupWindow()
{
    Boolean isShow = FALSE;
    if (mMorePopupWindow != NULL && (mMorePopupWindow->IsShowing(&isShow), isShow)) {
        Logger::D(TAG, "CloseMorePopupWindow()---");
        mMorePopupWindow->Dismiss();
        mShowMore = FALSE;
    }
    return NOERROR;
}

ECode CPhotoActivity::OpenShareDialog()
{
    Logger::D(TAG, "OpenShareDialog()--- : prev : %p", mShareDialog.Get());
    assert(mShareDialog.Get() == 0);
    CDialog::New(this, R::style::MyDialog, (IDialog**)&mShareDialog);
    mShareDialog->SetOnDismissListener((IDialogInterfaceOnDismissListener*)mDialogListener.Get());
    Logger::D(TAG, "OpenShareDialog()--- : new : %p", mShareDialog.Get());
    mShareDialog->SetContentView(R::layout::share_dialog);

    AutoPtr<IView> view;
    mShareDialog->FindViewById(R::id::share_dialog_btn_cancel, (IView**)&view);
    AutoPtr<IButton> cancelButton = IButton::Probe(view);
    AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mListener.Get();
    cancelButton->SetOnClickListener(clickListener);

    mShareDialog->Show();
    return NOERROR;
}

ECode CPhotoActivity::ShareDialogOnOK()
{
    ShareDialogOnCancel(); //TODO
    return NOERROR;
}

ECode CPhotoActivity::ShareDialogOnCancel()
{
    if (mShareDialog) {
        Logger::D(TAG, "ShareDialogOnCancel---: %p", mShareDialog.Get());
        mShareDialog->Cancel();
    }
    return NOERROR;
}

ECode CPhotoActivity::DeleteDialogOnOK()
{
    DeleteDialogOnCancel(); //TODO
    return NOERROR;
}

ECode CPhotoActivity::DeleteDialogOnCancel()
{
    if (mDeleteDialog) {
        Logger::D(TAG, "DeleteDialogOnCancel---: %p", mDeleteDialog.Get());
        mDeleteDialog->Cancel();
    }
    return NOERROR;
}

ECode CPhotoActivity::OpenDeleteDialog()
{
    Logger::D(TAG, "OpenDeleteDialog()--- : prev : %p", mDeleteDialog.Get());
    assert(mDeleteDialog.Get() == 0);
    CDialog::New(this, R::style::MyDialog, (IDialog**)&mDeleteDialog);
    mDeleteDialog->SetOnDismissListener((IDialogInterfaceOnDismissListener*)mDialogListener.Get());
    Logger::D(TAG, "OpenShareDialog()--- : new : %p", mDeleteDialog.Get());
    mDeleteDialog->SetContentView(R::layout::delete_dialog);

    AutoPtr<IView> view;
    mDeleteDialog->FindViewById(R::id::delete_dialog_btn_ok, (IView**)&view);
    AutoPtr<IButton> okButton = IButton::Probe(view);
    AutoPtr<IViewOnClickListener> clickListener = (IViewOnClickListener*)mListener.Get();
    okButton->SetOnClickListener(clickListener);

    view = NULL;
    mDeleteDialog->FindViewById(R::id::delete_dialog_btn_cancel, (IView**)&view);
    AutoPtr<IButton> cancelButton = IButton::Probe(view);
    cancelButton->SetOnClickListener(clickListener);

    mDeleteDialog->Show();
    return NOERROR;
}

void CPhotoActivity::ShowDetails()
{
    mShowDetails = TRUE;
    if (mDetailsHelper == NULL) {
        mDetailsHelper = new DetailsHelper(this, mDetailsArray);
        AutoPtr<ICloseListener> closeListener = (ICloseListener*)mListener.Get();
        mDetailsHelper->SetCloseListener(closeListener);
    }
    mDetailsHelper->Show();
}

void CPhotoActivity::HideDetails()
{
    mShowDetails = FALSE;
    mDetailsHelper->Hide();
}

void CPhotoActivity::SetCurrentPhoto()
{
    Logger::D(TAG, "SetCurrentPhoto()---");
    if (mCurrentIndex > 0) {
        mLeftButton->SetImageResource(R::drawable::photo_arrow_left);
    }
    else {
        mLeftButton->SetImageResource(R::drawable::photo_arrow_left_disable);
    }
    if (mCurrentIndex >= (mTotalSize - 1)) {
        mRightButton->SetImageResource(R::drawable::photo_arrow_right_disable);
    }
    else {
        mRightButton->SetImageResource(R::drawable::photo_arrow_right);
    }

    mLeftButton->SetVisibility(mViewSingleItem ? IView::INVISIBLE : IView::VISIBLE);
    mRightButton->SetVisibility(mViewSingleItem ? IView::INVISIBLE : IView::VISIBLE);

    AutoPtr<PhotoEntry> entry = mPhotoEntryList[mCurrentIndex];
    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(entry->desc, (ICharSequence**)&cs);
    mPhotoName->SetText(cs);
    mCurrentImagePath = entry->sourcePath;

    Boolean isNeed = AsyncImageLoader::NeedLoadHighDrawable(mCurrentImagePath);
    if (isNeed) {
        LoadCurrentImage(mCurrentImagePath, TRUE);
    }
    else {
        LoadCurrentImage(mCurrentImagePath, FALSE);
    }

    SetDetailInfo(mCurrentImagePath);
}

void CPhotoActivity::ChangeCurrentPhoto(
    /* [in] */ Boolean isNext)
{
    if (isNext) {
        Int32 tmp = mTotalSize - 1;
        if (mCurrentIndex >= tmp) return;
        mCurrentIndex++;
        mCurrentIndex = tmp > mCurrentIndex ? mCurrentIndex : tmp;
    }
    else {
        if (mCurrentIndex <= 0) return;
        mCurrentIndex--;
        mCurrentIndex = mCurrentIndex > 0 ? mCurrentIndex : 0;
    }
    SetCurrentPhoto();
}

void CPhotoActivity::MyImageLoaded(
    /* [in] */ IDrawable* imageDrawable,
    /* [in] */ IImageView* imageView,
    /* [in] */ const String& path,
    /* [in] */ Boolean isHigh)
{
    Logger::D(TAG, "MyImageLoaded()-----path:%s,isHigh:%d", path.string(), isHigh);
    if (!mCurrentImagePath.Equals(path)) {
        Logger::W(TAG, "MyImageLoaded()---not current path, drop!");
        return;
    }

    SwitchDisplayView(FALSE);
    Int32 w = 0;
    Int32 h = 0;

    if (isHigh) {
        Boolean ret = AsyncImageLoader::GetOrigionWidthAndHeight(path, &w, &h);
        if (!ret) {
            Logger::W(TAG, "MyImageLoaded()---GetOrigionWidthAndHeight---failed!");
            imageDrawable->GetIntrinsicWidth(&w);
            imageDrawable->GetIntrinsicHeight(&h);
        }
    }
    else {
        imageDrawable->GetIntrinsicWidth(&w);
        imageDrawable->GetIntrinsicHeight(&h);
    }
    Logger::D(TAG, "MyImageLoaded()---w:%d,h:%d", w, h);
    (*mDetailsArray)[2] = StringUtils::ToString(w);
    (*mDetailsArray)[3] = StringUtils::ToString(h);
    imageView->SetImageDrawable(imageDrawable);
}

void CPhotoActivity::SetDetailInfo(
    /* [in] */ const String& filePath)
{
    (*mDetailsArray)[0] = filePath.Substring(filePath.LastIndexOf(DataSourceHelper::PATH_SPLITE) + 1,
        filePath.LastIndexOf("."));
    (*mDetailsArray)[5] = filePath;
    Logger::D(TAG, "SetDetailInfo()---filePath:%s", filePath.string());
    AutoPtr<IFile> file;
    CFile::New(filePath, (IFile**)&file);
    if (file != NULL) {
        Int64 last = 0;
        file->LastModified(&last);
        AutoPtr<IDate> date;
        CDate::New(last, (IDate**)&date);
        AutoPtr<ISimpleDateFormat> sdf;
        CSimpleDateFormat::New(String("yyyy-MM-dd HH:mm"), (ISimpleDateFormat**)&sdf);
        String str;
        sdf->FormatDate(date, &str);
        (*mDetailsArray)[1] = str;
        Int64 len = 0;
        file->GetLength(&len);
        StringBuilder sizeStr;
        Int32 n = len / 1024;
        if (n > 0) {
            Int32 i = len / (1024 * 1024);
            if (i > 0) {
                Float j = (((Float)(len % (1024 * 1024))) / (1024 * 1024)) * 10;
                Int32 m = Elastos::Core::Math::Round(j);
                sizeStr += StringUtils::ToString(i);
                sizeStr += ".";
                sizeStr += StringUtils::ToString(m);
                sizeStr += "MB";
            }
            else {
                Float j = (((Float)(len % 1024)) / 1024) * 10;
                Int32 m = Elastos::Core::Math::Round(j);
                sizeStr += StringUtils::ToString(n);
                sizeStr += ".";
                sizeStr += StringUtils::ToString(m);
                sizeStr += "KB";
            }
        }
        else {
            sizeStr += StringUtils::ToString(len);
            sizeStr += "B";
        }
        (*mDetailsArray)[4] = sizeStr.ToString();
    }
}

void CPhotoActivity::LoadCurrentImage(
    /* [in] */ const String& path,
    /* [in] */ Boolean isHighQuality)
{
    Logger::D(TAG, "LoadCurrentImage()---isHighQuality:%d", isHighQuality);
    SwitchDisplayView(FALSE);
    if (isHighQuality) {
        AutoPtr<IWeakReference> weakHost;
        GetWeakReference((IWeakReference**)&weakHost);
        assert(weakHost != NULL);
        AutoPtr<MyLoadImageCallback> myLoadImage = new MyLoadImageCallback(weakHost, path, TRUE);
        AutoPtr<IBitmapDrawable> drawable = AsyncImageLoader::LoadDrawable(path, TRUE, mPhotoView, myLoadImage);
        if (drawable != NULL) {
            mPhotoView->SetImageDrawable(IDrawable::Probe(drawable));
            Int32 w = 0;
            Int32 h = 0;
            Boolean ret = AsyncImageLoader::GetOrigionWidthAndHeight(path, &w, &h);
            if (ret) {
                Logger::D(TAG, "LoadCurrentImage()---HighQuality---w:%d,h:%d", w, h);
                (*mDetailsArray)[2] = StringUtils::ToString(w);
                (*mDetailsArray)[3] = StringUtils::ToString(h);
            }
            else {
                Logger::W(TAG, "LoadCurrentImage()---GetOrigionWidthAndHeight--failed!");
            }
        }
        else {
            SwitchDisplayView(TRUE);
        }
    }
    else {
        AutoPtr<IWeakReference> weakHost;
        GetWeakReference((IWeakReference**)&weakHost);
        assert(weakHost != NULL);
        AutoPtr<MyLoadImageCallback> myLoadImage = new MyLoadImageCallback(weakHost, path, FALSE);
        AutoPtr<IBitmapDrawable> drawable = AsyncImageLoader::LoadDrawable(path, FALSE, mPhotoView, myLoadImage);
        if (drawable != NULL) {
            mPhotoView->SetImageDrawable(IDrawable::Probe(drawable));
            Int32 w = 0;
            Int32 h = 0;
            drawable->GetIntrinsicWidth(&w);
            drawable->GetIntrinsicHeight(&h);
            Logger::D(TAG, "LoadCurrentImage()---w:%d,h:%d", w, h);
            (*mDetailsArray)[2] = StringUtils::ToString(w);
            (*mDetailsArray)[3] = StringUtils::ToString(h);
        }
        else {
            SwitchDisplayView(TRUE);
        }
    }
}

void CPhotoActivity::SwitchDisplayView(
    /* [in] */ Boolean showLoading)
{
    if (showLoading) {
        mPhotoView->SetVisibility(IView::INVISIBLE);
        mPhotoLoadingText->SetVisibility(IView::VISIBLE);
        TriggerLoadingCallback(FALSE);
    }
    else {
        mPhotoLoadingText->SetVisibility(IView::INVISIBLE);
        mPhotoView->SetVisibility(IView::VISIBLE);
        UpdateLoadingStatus(TRUE);
    }
}

void CPhotoActivity::UpdateLoadingStatus(
    /* [in] */ Boolean needReload)
{
    AutoPtr<ICharSequence> cs;
    StringBuilder newText("图片加载中 ");
    if (needReload) {
        TriggerLoadingCallback(TRUE);
        newText += "...";
        cs = newText.ToCharSequence();
        mPhotoLoadingText->SetText(cs);
    }
    else {
        Int32 ret = IView::GONE;
        mPhotoLoadingText->GetVisibility(&ret);
        if (ret == IView::VISIBLE) {
            mPhotoLoadingText->GetText((ICharSequence**)&cs);
            String oldText;
            cs->ToString(&oldText);
            Int32 firstIndex = oldText.IndexOf(".");
            Int32 lastIndex = oldText.LastIndexOf(".");
            Int32 num = lastIndex - firstIndex;
            Logger::D(TAG, "UpdateLoadingStatus()---oldText:%s,firstIndex:%d,num:%d", oldText.string(), firstIndex, num);
            if (num == 0) {
                if (firstIndex > 0) {
                    newText += ".. ";
                }
                else {
                    newText += ".  ";
                }
            }
            else if (num == 1) {
                newText += "...";
            }
            else if (num == 2) {
                newText += "   ";
            }
            cs = newText.ToCharSequence();
            mPhotoLoadingText->SetText(cs);
            TriggerLoadingCallback(FALSE);
        }
        else {
            Logger::D(TAG, "UpdateLoadingStatus()---stop:%d", ret);
        }
    }
}

void CPhotoActivity::TriggerLoadingCallback(
    /* [in] */ Boolean isCancel)
{
    Logger::D(TAG, "TriggerLoadingCallback()---isCancel:%d", isCancel);
    if (isCancel) {
        mHandler->RemoveMessages(MSG_UPDATE_LOADING_STATUS);
    }
    else {
        Boolean needReload = FALSE;
        AutoPtr<IMessage> msg;
        mMyHandler->ObtainMessage(MSG_UPDATE_LOADING_STATUS,
            needReload ? 1 : 0, 0, (IMessage**)&msg);
        Boolean result;
        mMyHandler->SendMessageDelayed(msg, 1000, &result);
    }
}

ECode CPhotoActivity::OnNewIntent(
    /* [in] */ IIntent *intent)
{
    mPhotoEntryList.Clear();
    if (!LoadPhotoEntryList(intent))
        return NOERROR;

    mUpButton->SetVisibility(mViewSingleItem ? IView::INVISIBLE : IView::VISIBLE);
    mPhotoSlideshow->SetVisibility(mViewSingleItem ? IView::INVISIBLE : IView::VISIBLE);

    SetCurrentPhoto();
    return NOERROR;
}

Boolean CPhotoActivity::LoadPhotoEntryList(
    /* [in] */ IIntent* intent)
{
    if (intent != NULL) {
        String action;
        String filePath;
        intent->GetAction(&action);
        Logger::D(TAG, "action:%s", action.string());
        AutoPtr<IUri> uri;
        intent->GetData((IUri**)&uri);
        if (uri != NULL) {
            String scheme;
            uri->GetScheme(&scheme);
            if (scheme.Equals("file")) {
                uri->GetPath(&filePath);
                Boolean isImage = DataSourceHelper::IsImageFile(filePath);
                if (isImage) {
                    AutoPtr<PhotoEntry> entry = new PhotoEntry();
                    entry->sourcePath = filePath;
                    entry->desc = filePath.Substring(filePath.LastIndexOf(DataSourceHelper::PATH_SPLITE) + 1,
                        filePath.GetLength());
                    Logger::D(TAG, "single photo, path:%s, desc:%s", entry->sourcePath.string(), entry->desc.string());
                    mPhotoEntryList.PushBack(entry);
                    mCurrentIndex = 0;
                    mTotalSize = mPhotoEntryList.GetSize();
                    mViewSingleItem = TRUE;
                }
                else {
                    Logger::W(TAG, "image type is not support! filePath:%s", filePath.string());
                    Utils::MakeToast(this, String("不支持显示该类型的文件！"), IToast::LENGTH_LONG);
                    Finish();
                    return FALSE;
                }
            }
            else {
                Logger::W(TAG, "scheme is not a file! scheme:%s", scheme.string());
            }
        }
        else {
            intent->GetStringExtra(DataSourceHelper::SOURCE_PATH, &filePath);
            if (!filePath.IsNullOrEmpty()) {
                intent->GetInt32Extra(DataSourceHelper::SOURCE_INDEX, 0, &mCurrentIndex);
                mFolderPath = filePath.Substring(0, filePath.LastIndexOf(DataSourceHelper::PATH_SPLITE));
                AutoPtr<List<String> > imageItems = DataSourceHelper::GetItemList(mFolderPath);
                if (imageItems != NULL) {
                    AutoPtr<PhotoEntry> entry;
                    List<String>::Iterator it = imageItems->Begin();
                    for (Int32 i = 0; it != imageItems->End(); ++it, ++i) {
                        entry = new PhotoEntry();
                        entry->sourcePath = mFolderPath;
                        entry->sourcePath += DataSourceHelper::PATH_SPLITE;
                        entry->sourcePath += *it;
                        entry->desc = (*it).Substring(0, (*it).GetLength());
                        Logger::D(TAG, " > %d, path:%s, desc:%s", i, entry->sourcePath.string(), entry->desc.string());
                        mPhotoEntryList.PushBack(entry);
                    }
                    mTotalSize = mPhotoEntryList.GetSize();
                    mViewSingleItem = FALSE;
                }
                else {
                    Logger::W(TAG, "imageItems is null!");
                }
            }
        }
    }

    if (mPhotoEntryList.IsEmpty()) {
        Logger::W(TAG, "mPhotoEntryList size is zero!");
        Utils::MakeToast(this, String("没有图片文件！"), IToast::LENGTH_LONG);
        Finish();
        return FALSE;
    }

    return TRUE;
}

} // namespace Gallery
