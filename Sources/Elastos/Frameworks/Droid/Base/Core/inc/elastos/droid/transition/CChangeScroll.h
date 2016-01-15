
#ifndef __ELASTOS_DROID_TRANSITION_CCHANGESCROLL_H__
#define __ELASTOS_DROID_TRANSITION_CCHANGESCROLL_H__

#include "_Elastos_Droid_Transition_CChangeScroll.h"
#include "elastos/droid/transition/Transition.h"

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass(CChangeScroll)
    , public Transition
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI CaptureStartValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CaptureEndValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CaptureValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CreateAnimator(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ IAnimator** result);

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI CloneImpl(
        /* [in] */ ITransition* obj);

private:
    static const String PROPNAME_SCROLL_X;
    static const String PROPNAME_SCROLL_Y;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CCHANGESCROLL_H__
