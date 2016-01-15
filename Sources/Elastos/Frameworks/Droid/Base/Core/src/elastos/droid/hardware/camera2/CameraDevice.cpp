
#include "elastos/droid/hardware/camera2/CameraDevice.h"

using Elastos::IO::EIID_ICloseable;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CAR_INTERFACE_IMPL(CameraDevice::StateCallback, Object, ICameraDeviceStateCallback)

ECode CameraDevice::StateCallback::OnClosed(
    /* [in] */ ICameraDevice* camera)
{
    // Default empty implementation
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CameraDevice, Object, ICameraDevice, ICloseable)

} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos