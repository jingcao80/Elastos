
#ifndef __ELASTOS_DROID_ANIMATION_CANIMATORSET_H__
#define __ELASTOS_DROID_ANIMATION_CANIMATORSET_H__

#include "_Elastos_Droid_Animation_CAnimatorSet.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/animation/Animator.h"
#include "elastos/droid/animation/AnimatorSet.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CarClass(CAnimatorSet) , public AnimatorSet
{
public:
    CAR_OBJECT_DECL();
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_CANIMATORSET_H__
