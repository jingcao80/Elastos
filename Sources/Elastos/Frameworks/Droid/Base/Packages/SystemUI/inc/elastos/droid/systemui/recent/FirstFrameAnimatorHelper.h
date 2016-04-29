
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_FIRSTFRAMEANIMATORHELPER_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_FIRSTFRAMEANIMATORHELPER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IOnDrawListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewPropertyAnimator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

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
    class MyViewTreeObserverOnDrawListener
        : public Object
        , public IOnDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyViewTreeObserverOnDrawListener();

        // @Override
        CARAPI OnDraw();

    private:
        FirstFrameAnimatorHelper* mHost;
        Int64 mTime;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ IValueAnimator* animation,
            /* [in] */ FirstFrameAnimatorHelper* host);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<IValueAnimator> mAnimation;
        FirstFrameAnimatorHelper* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    FirstFrameAnimatorHelper(
        /* [in] */ IValueAnimator* animator,
        /* [in] */ IView* target);

    FirstFrameAnimatorHelper(
        /* [in] */ IViewPropertyAnimator* vpa,
        /* [in] */ IView* target);

    // only used for ViewPropertyAnimators
    CARAPI OnAnimationStart(
        /* [in] */ IAnimator* animation);

    static CARAPI_(void) InitializeDrawListener(
        /* [in] */ IView* view);

    CARAPI OnAnimationUpdate(
        /* [in] */ IValueAnimator* animation);

    CARAPI Print(
        /* [in] */ IValueAnimator* animation);

private:
    static const Boolean DEBUG;
    static const Int32 MAX_DELAY = 1000;
    static const Int32 IDEAL_FRAME_DURATION = 16;
    AutoPtr<IView> mTarget;
    Int64 mStartFrame;
    Int64 mStartTime;
    Boolean mHandlingOnAnimationUpdate;
    Boolean mAdjustedSecondFrameTime;

    static AutoPtr<IOnDrawListener> sGlobalDrawListener;
    static Int64 sGlobalFrameCounter;
};

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_FIRSTFRAMEANIMATORHELPER_H__