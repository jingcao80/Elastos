
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_CCAPTURERESULTKEY_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_CCAPTURERESULTKEY_H__

#include "_Elastos_Droid_Hardware_Camera2_CCaptureResultKey.h"
#include "elastos/droid/hardware/camera2/CaptureResult.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CarClass(CCaptureResultKey)
    , public CaptureResult::Key
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_CCAPTURERESULTKEY_H__
