
#ifndef __ELASTOS_DROID_TRANSITION_TRANSITIONVALUESMAPS_H__
#define __ELASTOS_DROID_TRANSITION_TRANSITIONVALUESMAPS_H__

#include "elastos/droid/animation/AnimatorListenerAdapter.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::View::IView;
using Elastos::Droid::Animation::ITimeInterpolator;


namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * This class is used by Slide and Explode to create an animator that goes from the start
 * position to the end position. It takes into account the canceled position so that it
 * will not blink out or shift suddenly when the transition is interrupted.
 */
class TranslationAnimationCreator
    : public Object
{
private:
    class TransitionPositionListener
        : public AnimatorListenerAdapter
    {
    public:
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animator);

        CARAPI OnAnimationPause(
            /* [in] */ IAnimator* animator);

        CARAPI OnAnimationResume(
            /* [in] */ IAnimator* animator);

    public:
        TransitionPositionListener(
            /* [in] */ IView* movingView,
            /* [in] */ IView* viewInHierarchy,
            /* [in] */ Int32 startX,
            /* [in] */ Int32 startY,
            /* [in] */ Float terminalX,
            /* [in] */ Float terminalY);

    private:
        AutoPtr<IView> mViewInHierarchy;
        AutoPtr<IView> mMovingView;
        Int32 mStartX;
        Int32 mStartY;
        AutoPtr<ArrayOf<Int32> > mTransitionPosition;
        Float mPausedX;
        Float mPausedY;
        Float mTerminalX;
        Float mTerminalY;
    };

public:
    static CARAPI_(AutoPtr<IAnimator>) CreateAnimation(
        /* [in] */ IView* view,
        /* [in] */ ITransitionValues* values,
        /* [in] */ Int32 viewPosX,
        /* [in] */ Int32 viewPosY,
        /* [in] */ Float startX,
        /* [in] */ Float startY,
        /* [in] */ Float endX,
        /* [in] */ Float endY,
        /* [in] */ ITimeInterpolator* interpolator);
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_TRANSITIONVALUESMAPS_H__
