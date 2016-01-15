
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CREQUESTTHREADMANAGERFPSCONUTER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CREQUESTTHREADMANAGERFPSCONUTER_H__

#include "_Elastos_Droid_Hardware_Camera2_Legacy_CRequestThreadManagerFpsCounter.h"
#include "elastos/droid/hardware/camera2/legacy/RequestThreadManager.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CarClass(CRequestThreadManagerFpsCounter)
    , public RequestThreadManager::RequestThreadManagerFpsCounter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CREQUESTTHREADMANAGERFPSCONUTER_H__
