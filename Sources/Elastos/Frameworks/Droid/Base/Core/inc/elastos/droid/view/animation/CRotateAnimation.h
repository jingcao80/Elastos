
#ifndef __ELASTOS_DROID_VIEW_ANIMATION_CROTATEANIMATION_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_CROTATEANIMATION_H__

#include "_Elastos_Droid_View_Animation_CRotateAnimation.h"
#include "elastos/droid/view/animation/RotateAnimation.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CarClass(CRotateAnimation), public RotateAnimation
{
public:
    CAR_OBJECT_DECL();
};

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif // __ELASTOS_DROID_VIEW_ANIMATION_CROTATEANIMATION_H__
