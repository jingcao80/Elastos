#ifndef  __ELASTOS_DROID_LAUNCHER2_FIRSTFRAMEANIMATORHELPER_H__
#define  __ELASTOS_DROID_LAUNCHER2_FIRSTFRAMEANIMATORHELPER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/os/Runnable.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::View::IOnDrawListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Core::Object;
using Elastos::Droid::Os::Runnable;

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
    , public IAnimatorUpdateListener
{
private:
    class MyOnDrawListener
        : public Object
        , public IOnDrawListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyOnDrawListener();

        CARAPI OnDraw();

    private:
        Int64 mTime;
        AutoPtr<FirstFrameAnimatorHelper> mHost;
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
        AutoPtr<IValueAnimator> mAnimation;
        AutoPtr<FirstFrameAnimatorHelper> mHost;
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
        /* [in] */ Boolean visible);

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

    static AutoPtr<IOnDrawListener> sGlobalDrawListener;
    static Int64 sGlobalFrameCounter;
    static Boolean sVisible;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_FIRSTFRAMEANIMATORHELPER_H__