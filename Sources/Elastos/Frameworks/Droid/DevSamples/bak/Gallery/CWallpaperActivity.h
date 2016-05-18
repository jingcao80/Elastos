
#ifndef __CWALLPAPERACTIVITY_H__
#define __CWALLPAPERACTIVITY_H__

#include "_CWallpaperActivity.h"
#include "elastos/droid/app/Activity.h"
#include "src/data/AsyncImageLoader.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IImageView;

namespace Gallery {

class CWallpaperActivity
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
            /* [in] */ CWallpaperActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CWallpaperActivity* mHost;
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
    CWallpaperActivity();

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

    CARAPI_(void) SetWallpaper();

private:
    static const String TAG;
    static const Int32 MSG_IMAGE_LOADED;

private:
    String mFilePath;
    AutoPtr<IImageView> mBackButton;
    AutoPtr<IImageView> mWallpaperView;
    AutoPtr<MyListener> mListener;
    AutoPtr<IHandler> mMyHandler;
};

} // namespace Gallery

#endif // __CWALLPAPERACTIVITY_H__
