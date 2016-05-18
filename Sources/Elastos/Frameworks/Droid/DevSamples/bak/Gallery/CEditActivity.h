
#ifndef __CEDITACTIVITY_H__
#define __CEDITACTIVITY_H__

#include "_CEditActivity.h"
#include "elastos/droid/app/Activity.h"
#include "src/data/AsyncImageLoader.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;

namespace Gallery {

class CEditActivity
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
    {
    public:
        MyListener(
            /* [in] */ CEditActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CEditActivity* mHost;
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

friend class MyListener;
friend class MyLoadImageCallback;

public:
    CEditActivity();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    CARAPI_(void) MyImageLoaded(
        /* [in] */ IDrawable* imageDrawable,
        /* [in] */ IImageView* imageView);

private:
    static const String TAG;
    static const Int32 MSG_IMAGE_LOADED;
private:
    AutoPtr<IImageView> mBackButton;
    AutoPtr<IImageView> mEditView;
    String mFilePath;
    AutoPtr<IHandler> mMyHandler;
};

} // namespace Gallery

#endif // __CEDITACTIVITY_H__
