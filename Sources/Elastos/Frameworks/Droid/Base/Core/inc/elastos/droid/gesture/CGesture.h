#ifndef __ELASTOS_DROID_GESTURE_CGESTURE_H__
#define __ELASTOS_DROID_GESTURE_CGESTURE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/gesture/Gesture.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(CGesture)
    , public Gesture
{
public:
    CAR_OBJECT_DECL();

    CGesture();

    virtual ~CGesture();
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_CGESTURE_H__
