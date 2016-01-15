#ifndef __ELASTOS_DROID_GESTURE_CGESTURESTORE_H__
#define __ELASTOS_DROID_GESTURE_CGESTURESTORE_H__

#include "elastos/droid/gesture/GestureStore.h"
#include "_Elastos_Droid_Gesture_CGestureStore.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(CGestureStore)
    , public GestureStore
{
public:
    CAR_OBJECT_DECL();

    CGestureStore();

    virtual ~CGestureStore();
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_CGESTURESTORE_H__
