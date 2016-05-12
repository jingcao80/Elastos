
#ifndef __ELASTOS_DROID_SERVICE_GESTURE_CEDGE_GESTURE_MANAGER_HELPER_H__
#define __ELASTOS_DROID_SERVICE_GESTURE_CEDGE_GESTURE_MANAGER_HELPER_H__

#include "_Elastos_Droid_Service_Gesture_CEdgeGestureManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Gesture {

 CarClass(CEdgeGestureManagerHelper)
    , public Singleton
    , public IEdgeGestureManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ IEdgeGestureManager** result);
};

} // namespace Gesture
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_GESTURE_CEDGE_GESTURE_MANAGER_HELPER_H__
