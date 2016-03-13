#ifndef  __ELASTOS_DROID_LAUNCHER2_FIRSTFRAMEANIMATORHELPER_H__
#define  __ELASTOS_DROID_LAUNCHER2_FIRSTFRAMEANIMATORHELPER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/*
 *  This is a helper class that listens to updates from the corresponding animation.
 *  For the first two frames, it adjusts the current play time of the animation to
 *  prevent jank at the beginning of the animation
 */

class FirstFrameAnimatorHelper
    : public AnimatorListenerAdapter
    , public IValueAnimatorAnimatorUpdateListener
{
private:
    class MyOnDrawListener
        : public Object
        , public IOnDrawListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyOnDrawListener(
            /* [in] */ FirstFrameAnimatorHelper* host);

        CARAPI OnDraw();

    private:
        Int64 mTime;
        FirstFrameAnimatorHelper* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ IValueAnimator* animation,
            /* [in] */ FirstFrameAnimatorHelper* host);


        CARAPI Run();

    private:
        IValueAnimator* mAnimation;
        FirstFrameAnimatorHelper* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    FirstFrameAnimatorHelper(
        /* [in] */ IValueAnimator* animator,
        /* [in] */ IView* target);

    FirstFrameAnimatorHelper(
        /* [in] */ IViewPropertyAnimator* vpa,
        /* [in] */ IView* target);

    // only used for ViewPropertyAnimators
    CARAPI OnAnimationStart(
        /* [in] */ IAnimator* animation);

    CARAPI static SetIsVisible(
        /* [in] */ boolean visible);

    CARAPI static InitializeDrawListener(
        /* [in] */ IView* view);

    CARAPI OnAnimationUpdate(
        /* [in] */ IValueAnimator* animation);

    CARAPI Print(
        /* [in] */ IValueAnimator* animation);

private:
    static const Boolean DEBUG;
    static const Int32 MAX_DELAY;
    static const Int32 IDEAL_FRAME_DURATION;
    AutoPtr<IView> mTarget;
    Int64 mStartFrame;
    Int64 mStartTime;
    Boolean mHandlingOnAnimationUpdate;
    Boolean mAdjustedSecondFrameTime;

    static AutoPtr<IViewTreeObserverOnDrawListener> sGlobalDrawListener;
    static Int64 sGlobalFrameCounter;
    static Boolean sVisible;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_FIRSTFRAMEANIMATORHELPER_H__