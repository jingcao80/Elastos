
#ifndef __ELASTOS_DROID_SERVICE_GESTURE_CEDGE_GESTURE_MANAGER_H__
#define __ELASTOS_DROID_SERVICE_GESTURE_CEDGE_GESTURE_MANAGER_H__

#include "_Elastos_Droid_Service_Gesture_CEdgeGestureManager.h"
#include "elastos/droid/service/gesture/EdgeGestureManager.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Gesture {

 CarClass(CEdgeGestureManager)
    , public EdgeGestureManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Gesture
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_GESTURE_CEDGE_GESTURE_MANAGER_H__
