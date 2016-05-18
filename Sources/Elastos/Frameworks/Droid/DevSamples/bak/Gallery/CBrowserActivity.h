
#ifndef __CBROWSERACTIVITY_H__
#define __CBROWSERACTIVITY_H__

#include "_CBrowserActivity.h"
#include "elastos/droid/app/Activity.h"
#include "src/data/AsyncImageLoader.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::IPopupWindowOnDismissListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IGridView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::ISimpleAdapter;
using Elastos::Droid::Widget::ISimpleAdapterViewBinder;
using Elastos::Droid::Widget::ILinearLayout;

namespace Gallery {

class CBrowserActivity
    : public Activity
{
public:
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
        : public ElRefBase
        , public IViewOnClickListener
        , public IPopupWindowOnDismissListener
        , public IViewOnTouchListener
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        MyListener(
            /* [in] */ CBrowserActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnDismiss();

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        CBrowserActivity* mHost;
    };

    class MyViewBinder
        : public ElRefBase
        , public ISimpleAdapterViewBinder
    {
    public:
        MyViewBinder(
            /* [in] */ CBrowserActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI SetViewValue(
            /* [in] */ IView* view,
            /* [in] */ IInterface* data,
            /* [in] */ const String& textRepresentation,
            /* [out] */ Boolean* result);

    private:
            CBrowserActivity* mHost;
    };

    class MyLoadImageCallback
        : public ILoadImageCallback
    {
    public:
        MyLoadImageCallback(
            /* [in] */ IWeakReference* host);

        CARAPI ImageLoaded(
            /* [in] */ IDrawable* imageDrawable,
            /* [in] */ IImageView* imageView);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class PictureEntry
        : public ElRefBase
    {
    public:
        String sourcePath;
        String desc;
    };

friend class MyListener;
friend class MyViewBinder;
friend class MyLoadImageCallback;

public:
    CBrowserActivity();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

    CARAPI OnBackPressed();

private:
    CARAPI OpenPicPopupWindow(
        /* [in] */ IView* v);

    CARAPI ClosePicPopupWindow();

    CARAPI_(AutoPtr<ISimpleAdapter>) GetSimpleAdapter();

    CARAPI_(void) MyImageLoaded(
        /* [in] */ IDrawable* imageDrawable,
        /* [in] */ IImageView* imageView);

private:
    static const String TAG;
    static const Int32 MSG_IMAGE_LOADED;

private:
    Boolean mShowPicPop;

    AutoPtr<MyListener> mMyListener;
    AutoPtr<IImageView> mBackButton;
    AutoPtr<IGridView> mGridView;
    AutoPtr<ILinearLayout> mWallpaperLayout;
    // AutoPtr<IButton> mDetailButton;
    AutoPtr<ILinearLayout> mDetailLayout;
    AutoPtr<ILinearLayout> mShareLayout;
    AutoPtr<IButton> mPopButton;
    AutoPtr<IPopupWindow> mPicPopupWindow;
    AutoPtr<ISimpleAdapter> mSimpleAdapter;
    List< AutoPtr<PictureEntry> > mPictureEntryList;
    AutoPtr<IHandler> mMyHandler;
};

} // namespace Gallery

#endif // __CBROWSERACTIVITY_H__
