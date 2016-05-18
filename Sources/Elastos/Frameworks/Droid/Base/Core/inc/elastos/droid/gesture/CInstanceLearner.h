#ifndef __ELASTOS_DROID_GESTURE_CINSTANCELEARNER_H__
#define __ELASTOS_DROID_GESTURE_CINSTANCELEARNER_H__

#include "_Elastos_Droid_Gesture_CInstanceLearner.h"
#include "elastos/droid/gesture/InstanceLearner.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(CInstanceLearner)
    , public InstanceLearner
{
public:
    CAR_OBJECT_DECL()

    CInstanceLearner();

    virtual ~CInstanceLearner();
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GESTURE_CINSTANCELEARNER_H__
