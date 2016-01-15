
#ifndef __ELASTOS_DROID_SERVER_DISPLAY_RAMPANIMATOR_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_RAMPANIMATOR_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/os/Runnable.h>

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Utility::IInt32Property;
using Elastos::Droid::View::IChoreographer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

/**
 * A custom animator that progressively updates a property value at
 * a given variable rate until it reaches a particular target value.
 */
class RampAnimator
    : public Object
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ RampAnimator* host);

        // @Override // Choreographer callback
        CARAPI Run();
    private:
        RampAnimator* mHost;
    };

public:
    RampAnimator(
        /* [in] */ IInterface* object,
        /* [in] */ IInt32Property* property);

    /**
     * Starts animating towards the specified value.
     *
     * If this is the first time the property is being set or if the rate is 0,
     * the value jumps directly to the target.
     *
     * @param target The target value.
     * @param rate The convergence rate in units per second, or 0 to set the value immediately.
     * @return True if the target differs from the previous target.
     */
    Boolean AnimateTo(
        /* [in] */ Int32 target,
        /* [in] */ Int32 rate);

    /**
     * Returns TRUE if the animation is running.
     */
    Boolean IsAnimating();

    /**
     * Sets a listener to watch for animation events.
     */
    void SetListener(
        /* [in] */ IRampAnimatorListener* listener);

private:
    void PostAnimationCallback();

    void CancelAnimationCallback();

private:
    AutoPtr<IRunnable> mAnimationCallback;

    AutoPtr<IInterface> mObject;
    AutoPtr<IInt32Property> mProperty;
    AutoPtr<IChoreographer> mChoreographer;

    Int32 mCurrentValue;
    Int32 mTargetValue;
    Int32 mRate;

    Boolean mAnimating;
    Float mAnimatedValue; // higher precision copy of mCurrentValue
    Int64 mLastFrameTimeNanos;

    Boolean mFirstTime;

    AutoPtr<IRampAnimatorListener> mListener;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_RAMPANIMATOR_H__
