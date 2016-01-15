#ifndef __ELASTOS_DROID_GESTURE_CGESTUREPOINT_H__
#define __ELASTOS_DROID_GESTURE_CGESTUREPOINT_H__

#include "elastos/droid/gesture/GesturePoint.h"
#include "_Elastos_Droid_Gesture_CGesturePoint.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(CGesturePoint)
    , public GesturePoint
{
public:
    CAR_OBJECT_DECL();

    CGesturePoint();

    virtual ~CGesturePoint();
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_CGESTUREPOINT_H__
