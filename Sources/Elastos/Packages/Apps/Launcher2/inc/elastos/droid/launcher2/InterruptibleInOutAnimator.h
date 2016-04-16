#ifndef  __ELASTOS_DROID_LAUNCHER2_INTERRUPTIBLEINOUTANIMATOR_H__
#define  __ELASTOS_DROID_LAUNCHER2_INTERRUPTIBLEINOUTANIMATOR_H__

#include "_Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::View::IView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class InterruptibleInOutAnimator
    : public Object
{
private:
    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter(
            /* [in] */ InterruptibleInOutAnimator* host);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<InterruptibleInOutAnimator> mHost;
    };

public:
    InterruptibleInOutAnimator(
        /* [in] */ IView* view,
        /* [in] */ Int64 duration,
        /* [in] */ Float fromValue,
        /* [in] */ Float toValue);

    CARAPI Cancel();

    CARAPI End();

    /**
     * Return true when the animation is not running and it hasn't even been started.
     */
    CARAPI_(Boolean) IsStopped();

    /**
     * This is the equivalent of calling Animator.start(), except that it can be called when
     * the animation is running in the opposite direction, in which case we reverse
     * direction and animate for a correspondingly shorter duration.
     */
    CARAPI AnimateIn();

    /**
     * This is the roughly the equivalent of calling Animator.reverse(), except that it uses the
     * same interpolation curve as animateIn(), rather than mirroring it. Also, like animateIn(),
     * if the animation is currently running in the opposite direction, we reverse
     * direction and animate for a correspondingly shorter duration.
     */
    CARAPI AnimateOut();

    CARAPI SetTag(
        /* [in] */ IInterface* tag);

    CARAPI_(AutoPtr<IInterface>) GetTag();

    CARAPI_(AutoPtr<IValueAnimator>) GetAnimator();

private:
    CARAPI Animate(
        /* [in] */ Int32 direction);

private:
    Int64 mOriginalDuration;
    Float mOriginalFromValue;
    Float mOriginalToValue;
    AutoPtr<IValueAnimator> mAnimator;

    Boolean mFirstRun;

    AutoPtr<IInterface> mTag;

    static const Int32 STOPPED;
    static const Int32 IN;
    static const Int32 OUT;

    // TODO: This isn't really necessary, but is here to help diagnose a bug in the drag viz
    Int32 mDirection;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Launcher2::InterruptibleInOutAnimator, IInterface)

#endif // __ELASTOS_DROID_LAUNCHER2_INTERRUPTIBLEINOUTANIMATOR_H__