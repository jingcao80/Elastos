
#ifndef __ELASTOS_DROID_TRANSITION_CSLIDEPROPAGATION_H__
#define __ELASTOS_DROID_TRANSITION_CSLIDEPROPAGATION_H__

#include "_Elastos_Droid_Transition_CSidePropagation.h"
#include "elastos/droid/transition/VisibilityPropagation.h"

using Elastos::Droid::View::IViewGroup;

namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * A <code>TransitionPropagation</code> that propagates based on the distance to the side
 * and, orthogonally, the distance to epicenter. If the transitioning View is visible in
 * the start of the transition, then it will transition sooner when closer to the side and
 * later when farther. If the view is not visible in the start of the transition, then
 * it will transition later when closer to the side and sooner when farther from the edge.
 * This is the default TransitionPropagation used with {@link android.transition.Slide}.
 */
CarClass(CSidePropagation)
    , public VisibilityPropagation
    , public ISidePropagation
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI SetSide(
        /* [in] */ Int32 side);

    CARAPI SetPropagationSpeed(
        /* [in] */ Float propagationSpeed);

    CARAPI GetStartDelay(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransition* transition,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ Int64* result);

private:
    CARAPI_(Int32) Distance(
        /* [in] */ Int32 viewX,
        /* [in] */ Int32 viewY,
        /* [in] */ Int32 epicenterX,
        /* [in] */ Int32 epicenterY,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI_(Int32) GetMaxDistance(
        /* [in] */ IViewGroup* sceneRoot);

private:
    static const String TAG;

    Float mPropagationSpeed;
    Int32 mSide;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CSLIDEPROPAGATION_H__
