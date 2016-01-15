#ifndef __ELASTOS_DROID_GESTURE_CINSTANCE_H__
#define __ELASTOS_DROID_GESTURE_CINSTANCE_H__

#include "_Elastos_Droid_Gesture_CInstance.h"
#include "elastos/droid/gesture/Instance.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(CInstance)
    , public Instance
{
public:
    CAR_OBJECT_DECL();

    CInstance();

    virtual ~CInstance();
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GESTURE_CINSTANCE_H__
