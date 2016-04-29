
#ifndef __ELASTOS_DROID_SYSTEMUI_SCREENSHOT_GLOBALSCREENSHOT_H__
#define __ELASTOS_DROID_SYSTEMUI_SCREENSHOT_GLOBALSCREENSHOT_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IPointF;
using Elastos::Droid::Media::IMediaActionSound;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::Widget::IImageView;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Screenshot {

/**
 * TODO:
 *   - Performance when over gl surfaces? Ie. Gallery
 *   - what do we say in the Toast? Which icon do we get if the user uses another
 *     type of gallery?
 */
class GlobalScreenshot
    : public Object
{
private:
    class MyViewOnTouchListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyViewOnTouchListener();

        //Override
        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter(
            /* [in] */ IRunnable* finisher,
            /* [in] */ GlobalScreenshot* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IRunnable> mFinisher;
        GlobalScreenshot* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ GlobalScreenshot* host);

        // @Override
        CARAPI Run();

    private:
        GlobalScreenshot* mHost;
    };

    class MyInterpolator1
        : public Object
        , public ITimeInterpolator
        , public IInterpolator
    {
    public:
        CAR_INTERFACE_DECL()

        MyInterpolator1(
            /* [in] */ Float flashDurationPct);

        //@Override
        CARAPI GetInterpolation(
            /* [in] */ Float x,
            /* [out] */ Float* interpolation);

        //@Override
        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        Float mFlashDurationPct;
    };

    class MyInterpolator2
        : public Object
        , public ITimeInterpolator
        , public IInterpolator
    {
    public:
        CAR_INTERFACE_DECL()

        MyInterpolator2(
            /* [in] */ Float flashPeakDurationPct,
            /* [in] */ Float flashDurationPct);

        //@Override
        CARAPI GetInterpolation(
            /* [in] */ Float x,
            /* [out] */ Float* interpolation);

        //@Override
        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        Float mFlashPeakDurationPct;
        Float mFlashDurationPct;
    };

    class MyAnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter2(
            /* [in] */ GlobalScreenshot* host);

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        GlobalScreenshot* mHost;
    };

    class MyAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyAnimatorUpdateListener(
            /* [in] */ IInterpolator* scaleInterpolator,
            /* [in] */ IInterpolator* flashAlphaInterpolator,
            /* [in] */ GlobalScreenshot* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<IInterpolator> mScaleInterpolator;
        AutoPtr<IInterpolator> mFlashAlphaInterpolator;
        GlobalScreenshot* mHost;
    };

    class MyAnimatorListenerAdapter3
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter3(
            /* [in] */ GlobalScreenshot* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        GlobalScreenshot* mHost;
    };

    class MyAnimatorUpdateListener2
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyAnimatorUpdateListener2(
            /* [in] */ GlobalScreenshot* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        GlobalScreenshot* mHost;
    };

    class MyInterpolator3
        : public Object
        , public ITimeInterpolator
        , public IInterpolator
    {
    public:
        CAR_INTERFACE_DECL()

        MyInterpolator3(
            /* [in] */ Float scaleDurationPct);

        //@Override
        CARAPI GetInterpolation(
            /* [in] */ Float x,
            /* [out] */ Float* interpolation);

        //@Override
        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        Float mScaleDurationPct;
    };

   class MyAnimatorUpdateListener3
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyAnimatorUpdateListener3(
            /* [in] */ IInterpolator* scaleInterpolator,
            /* [in] */ IPointF* finalPos,
            /* [in] */ GlobalScreenshot* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<IInterpolator> mScaleInterpolator;
        AutoPtr<IPointF> mFinalPos;
        GlobalScreenshot* mHost;
    };

public:
    /**
     * @param context everything needs a context :(
     */
    GlobalScreenshot(
        /* [in] */ IContext* context);
    /**
     * Takes a screenshot of the current display and shows an animation.
     */
    CARAPI_(void) TakeScreenshot(
        /* [in] */ IRunnable* finisher,
        /* [in] */ Boolean statusBarVisible,
        /* [in] */ Boolean navBarVisible);

    static CARAPI_(void) NotifyScreenshotError(
        /* [in] */ IContext* context,
        /* [in] */ INotificationManager* nManager);

private:
    /**
     * Creates a new worker thread and saves the screenshot to the media store.
     */
    CARAPI_(void) SaveScreenshotInWorkerThread(
        /* [in] */ IRunnable* finisher);

    /**
     * @return the current display rotation in degrees
     */
    CARAPI_(Float) GetDegreesForRotation(
        /* [in] */ Int32 value);

    /**
     * Starts the animation after taking the screenshot
     */
    CARAPI_(void) StartAnimation(
        /* [in] */ IRunnable* finisher,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Boolean statusBarVisible,
        /* [in] */ Boolean navBarVisible);

    CARAPI_(AutoPtr<IValueAnimator>) CreateScreenshotDropInAnimation();

    CARAPI_(AutoPtr<IValueAnimator>) CreateScreenshotDropOutAnimation(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Boolean statusBarVisible,
        /* [in] */ Boolean navBarVisible);

private:
    const static String TAG;

    const static Int32 SCREENSHOT_NOTIFICATION_ID = 789;
    const static Int32 SCREENSHOT_FLASH_TO_PEAK_DURATION = 130;
    const static Int32 SCREENSHOT_DROP_IN_DURATION = 430;
    const static Int32 SCREENSHOT_DROP_OUT_DELAY = 500;
    const static Int32 SCREENSHOT_DROP_OUT_DURATION = 430;
    const static Int32 SCREENSHOT_DROP_OUT_SCALE_DURATION = 370;
    const static Int32 SCREENSHOT_FAST_DROP_OUT_DURATION = 320;
    const static Float BACKGROUND_ALPHA;
    const static Float SCREENSHOT_SCALE;
    const static Float SCREENSHOT_DROP_IN_MIN_SCALE;
    const static Float SCREENSHOT_DROP_OUT_MIN_SCALE;
    const static Float SCREENSHOT_FAST_DROP_OUT_MIN_SCALE;
    const static Float SCREENSHOT_DROP_OUT_MIN_SCALE_OFFSET;
    Int32 mPreviewWidth;
    Int32 mPreviewHeight;

    AutoPtr<IContext> mContext;
    AutoPtr<IWindowManager> mWindowManager;
    AutoPtr<IWindowManagerLayoutParams> mWindowLayoutParams;
    AutoPtr<INotificationManager> mNotificationManager;
    AutoPtr<IDisplay> mDisplay;
    AutoPtr<IDisplayMetrics> mDisplayMetrics;
    AutoPtr<IMatrix> mDisplayMatrix;

    AutoPtr<IBitmap> mScreenBitmap;
    AutoPtr<IView> mScreenshotLayout;
    AutoPtr<IImageView> mBackgroundView;
    AutoPtr<IImageView> mScreenshotView;
    AutoPtr<IImageView> mScreenshotFlash;

    AutoPtr<IAnimatorSet> mScreenshotAnimation;

    Int32 mNotificationIconSize;
    Float mBgPadding;
    Float mBgPaddingScale;

    AutoPtr<AsyncTask> mSaveInBgTask;
    AutoPtr<IMediaActionSound> mCameraSound;
};

} // namespace Screenshot
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SCREENSHOT_GLOBALSCREENSHOT_H__