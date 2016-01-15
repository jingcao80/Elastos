
#ifndef __ELASTOS_DROID_TRANSITION_CCIRCULARPROPAGATION_H__
#define __ELASTOS_DROID_TRANSITION_CCIRCULARPROPAGATION_H__

#include "_Elastos_Droid_Transition_CCircularPropagation.h"
#include "elastos/droid/transition/VisibilityPropagation.h"

using Elastos::Droid::View::IViewGroup;

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass(CCircularPropagation)
    , public VisibilityPropagation
    , public ICircularPropagation
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI SetPropagationSpeed(
        /* [in] */ Float propagationSpeed);

    CARAPI GetStartDelay(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransition* transition,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ Int64* result);

private:
    static CARAPI_(Float) Distance(
        /* [in] */ Float x1,
        /* [in] */ Float y1,
        /* [in] */ Float x2,
        /* [in] */ Float y2);

private:
    static String TAG;

    Float mPropagationSpeed;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CCIRCULARPROPAGATION_H__
