
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CCALLBACKPROXIESDEVICECAPTURECALLBACKPROXY_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CCALLBACKPROXIESDEVICECAPTURECALLBACKPROXY_H__

#include "_Elastos_Droid_Hardware_Camera2_Impl_CCallbackProxiesDeviceCaptureCallbackProxy.h"
#include "elastos/droid/hardware/camera2/impl/CallbackProxies.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

CarClass(CCallbackProxiesDeviceCaptureCallbackProxy)
    , public CallbackProxies::DeviceCaptureCallbackProxy
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CCALLBACKPROXIESDEVICECAPTURECALLBACKPROXY_H__
