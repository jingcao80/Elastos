
#ifndef __CPHOTOACTIVITY_H__
#define __CPHOTOACTIVITY_H__

#include "_CPhotoActivity.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/os/HandlerBase.h"
#include "src/ui/DetailsHelper.h"
#include "src/data/AsyncImageLoader.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::IPopupWindowOnDismissListener;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;

namespace Gallery {

class CPhotoActivity
    : public Activity
{
private:
    class MyHandler : public HandlerBase
    {
    public:
        MyHandler(
            /* [in] */ IWeakReference* host)
            : mWeakHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class MyListener
        : public ICloseListener
        , public IViewOnClickListener
        , public IPopupWindowOnDismissListener
        , public ISeekBarOnSeekBarChangeListener
    {
    public:
        MyListener(
            /* [in] */ CPhotoActivity* host);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnDismiss();

        CARAPI OnProgressChanged(
            /* [in] */ ISeekBar* seekBar,
            /* [in] */ Int32 progress,
            /* [in] */ Boolean fromUser);

        CARAPI OnStartTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        CARAPI OnStopTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        CARAPI OnClose();

    private:
        CPhotoActivity* mHost;
    };

    class DialogListener
        : public ElRefBase
        , public IDialogInterfaceOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL();

        DialogListener(
            /* [in] */ CPhotoActivity* host);

        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

    private:
        CPhotoActivity* mHost;
    };

    class MyLoadImageCallback
        : public ILoadImageCallback
    {
    public:
        MyLoadImageCallback(
            /* [in] */ IWeakReference* host,
            /* [in] */ const String& path,
            /* [in] */ Boolean isHigh);

        CARAPI ImageLoaded(
            /* [in] */ IDrawable* imageDrawable,
            /* [in] */ IImageView* imageView);

    private:
        AutoPtr<IWeakReference> mWeakHost;
        String mPath;
        Boolean mIsHigh;
    };

    class PhotoEntry
        : public ElRefBase
    {
    public:
        String sourcePath;
        String desc;
    };

friend class MyListener;
friend class MyLoadImageCallback;

public:
    CPhotoActivity();
    ~CPhotoActivity();

protected:

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

    CARAPI OnBackPressed();

    CARAPI OnNewIntent(
        /* [in] */ IIntent *intent);

private:
    CARAPI OpenZoomPopupWindow(
        /* [in] */ IView* v);

    CARAPI CloseZoomPopupWindow();

    CARAPI OpenMorePopupWindow(
        /* [in] */ IView* v);

    CARAPI CloseMorePopupWindow();

    CARAPI OpenShareDialog();

    CARAPI ShareDialogOnOK();

    CARAPI ShareDialogOnCancel();

    CARAPI OpenDeleteDialog();

    CARAPI DeleteDialogOnOK();

    CARAPI DeleteDialogOnCancel();

    CARAPI_(void) ShowDetails();

    CARAPI_(void) HideDetails();

    CARAPI_(void) SetCurrentPhoto();

    CARAPI_(void) ChangeCurrentPhoto(
        /* [in] */ Boolean isNext);

    CARAPI_(void) MyImageLoaded(
        /* [in] */ IDrawable* imageDrawable,
        /* [in] */ IImageView* imageView,
        /* [in] */ const String& path,
        /* [in] */ Boolean isHigh);

    CARAPI_(void) SetDetailInfo(
        /* [in] */ const String& filePath);

    CARAPI_(void) LoadCurrentImage(
        /* [in] */ const String& path,
        /* [in] */ Boolean isHighQuality);

    CARAPI_(void) SwitchDisplayView(
        /* [in] */ Boolean showLoading);

    CARAPI_(void) UpdateLoadingStatus(
        /* [in] */ Boolean reload);

    CARAPI_(void) TriggerLoadingCallback(
        /* [in] */ Boolean isCancel);

    CARAPI_(Boolean) LoadPhotoEntryList(
        /* [in] */ IIntent* intent);

private:
    static const String TAG;
    static const Int32 MSG_IMAGE_LOADED;
    static const Int32 MSG_UPDATE_LOADING_STATUS;

private:
    Int32 mIndex;
    Int32 mCurrentIndex;
    Int32 mTotalSize;
    Int32 mDisplayWidth;
    Int32 mDisplayHeight;

    Boolean mShowMore;
    Boolean mShowZoom;
    Boolean mShowDetails;
    Boolean mViewSingleItem;

    String mFolderPath;
    String mCurrentImagePath;

    AutoPtr<IImageView> mUpButton;
    AutoPtr<ITextView> mPhotoName;
    AutoPtr<IImageView> mPhotoSlideshow;
    AutoPtr<IImageView> mPhotoMore;
    AutoPtr<IImageView> mPhotoView;
    AutoPtr<ITextView> mPhotoLoadingText;
    AutoPtr<IImageButton> mLeftButton;
    AutoPtr<IImageButton> mRightButton;
    AutoPtr<IImageButton> mZoomButton;
    AutoPtr<IImageButton> mRotateLeftButton;
    AutoPtr<IImageButton> mRotateRightButton;
    AutoPtr<IImageButton> mShareButton;
    AutoPtr<IImageButton> mEditButton;
    AutoPtr<IImageButton> mDeleteButton;
    AutoPtr<IView> mTopView;
    AutoPtr<IResources> mRes;
    AutoPtr<IPopupWindow> mZoomPopupWindow;
    AutoPtr<IPopupWindow> mMorePopupWindow;
    AutoPtr<MyListener> mListener;
    AutoPtr<DialogListener> mDialogListener;
    AutoPtr<IDialog> mShareDialog;
    AutoPtr<IDialog> mDeleteDialog;
    AutoPtr<DetailsHelper> mDetailsHelper;
    AutoPtr< ArrayOf<String> > mDetailsArray;
    List< AutoPtr<PhotoEntry> > mPhotoEntryList;
    AutoPtr<IHandler> mMyHandler;
};

} // namespace Gallery

#endif // __CPHOTOACTIVITY_H__
