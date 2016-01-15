
#ifndef __ELASTOS_DROID_INTERNAL_APP_PLATLOGOACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_PLATLOGOACTIVITY_H__

#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOutlineProvider;
using Elastos::Droid::View::View;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class PlatLogoActivity
    : public Activity
    , public IPlatLogoActivity
{
private:
    class StickView : public Elastos::Droid::View::View
    {
    private:
        class ViewOutlineProvider
            : public Object
            , public IViewOutlineProvider
        {
        public:
            ViewOutlineProvider(
                /* [in] */ StickView* host);

            CAR_INTERFACE_DECL()

            // @Override
            CARAPI GetOutline(
                /* [in] */ IView* view,
                /* [in] */ IOutline* outline);

        private:
            StickView* mHost;
        };

    public:
        CARAPI constructor(
            /* [in] */ PlatLogoActivity* host,
            /* [in] */ Int32 size);

        // @Override
        CARAPI OnAttachedToWindow();

        // @Override
        CARAPI_(void) OnDraw(
            /* [in] */ ICanvas* c);

    public:
        AutoPtr<IPaint> mPaint;
        AutoPtr<IPath> mShadow;
        PlatLogoActivity* mHost;
        Int32 mSize;
    };

    class LocalRunnable : public Runnable
    {
    public:
        LocalRunnable(
            /* [in] */ PlatLogoActivity* host)
            : mHost(host)
        {
        }

        CARAPI Run();

    private:
        PlatLogoActivity* mHost;
    };

    class LocalListener
        : public Object
        , public IViewOnClickListener
        , public IViewOnLongClickListener
        , public IViewOnKeyListener
    {
    public:
        LocalListener(
            /* [in] */ PlatLogoActivity* host,
            /* [in] */ IImageView* im,
            /* [in] */ StickView* stick,
            /* [in] */ IDrawable* platlogo);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

        CARAPI OnKey(
            /* [in] */ IView* v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

    private:
        PlatLogoActivity* mHost;
        IImageView* mIm;
        AutoPtr<StickView> mStick;
        AutoPtr<IDrawable> mPlatlogo;
    };

public:
    PlatLogoActivity();

    CAR_INTERFACE_DECL()

    static CARAPI_(Int32) NewColorIndex();

    CARAPI_(AutoPtr<IDrawable>) MakeRipple();

    CARAPI OnAttachedToWindow();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI_(AutoPtr<IView>) MakeView();

public:
    static const Int32 FLAVORS_LENGTH = 12;
    static const Int32 FLAVORS[FLAVORS_LENGTH];
    AutoPtr<IFrameLayout> mLayout;
    Int32 mTapCount;
    Int32 mKeyCount;
    // AutoPtr<IPathInterpolator> mInterpolator;
    AutoPtr<ITimeInterpolator> mInterpolator;
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_PLATLOGOACTIVITY_H__
