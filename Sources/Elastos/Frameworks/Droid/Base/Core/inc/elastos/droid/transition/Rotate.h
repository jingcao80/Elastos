
#ifndef __ELASTOS_DROID_TRANSITION_ROTATE_H__
#define __ELASTOS_DROID_TRANSITION_ROTATE_H__

#include "elastos/droid/transition/Transition.h"

namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * This transition captures the rotation property of targets before and after
 * the scene change and animates any changes.
 *
 * @hide
 */
class Rotate
    : public Transition
{
public:
    CARAPI CaptureStartValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CaptureEndValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CreateAnimator(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ IAnimator** result);

private:
    static const String PROPNAME_ROTATION;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_ROTATE_H__
