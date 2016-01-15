
#ifndef __ELASTOS_DROID_TRANSITION_CTRANSITIONSET_H__
#define __ELASTOS_DROID_TRANSITION_CTRANSITIONSET_H__

#include "_Elastos_Droid_Transition_CTransitionSet.h"
#include "elastos/droid/transition/TransitionSet.h"

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass(CTransitionSet)
    , public TransitionSet
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CTRANSITIONSET_H__
