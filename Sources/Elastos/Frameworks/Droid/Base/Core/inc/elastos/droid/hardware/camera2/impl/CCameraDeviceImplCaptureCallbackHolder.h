
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CCAMERADEVICEIMPLCAPTURECALLBACKHOLDER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CCAMERADEVICEIMPLCAPTURECALLBACKHOLDER_H__

#include "_Elastos_Droid_Hardware_Camera2_Impl_CCameraDeviceImplCaptureCallbackHolder.h"
#include "elastos/droid/hardware/camera2/impl/CameraDeviceImpl.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

CarClass(CCameraDeviceImplCaptureCallbackHolder)
    , public CameraDeviceImpl::CaptureCallbackHolder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CCAMERADEVICEIMPLCAPTURECALLBACKHOLDER_H__
