
#include "elastos/droid/hardware/camera2/legacy/CCameraDeviceUserShimHelper.h"
#include "elastos/droid/hardware/camera2/legacy/CameraDeviceUserShim.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(CCameraDeviceUserShimHelper, Singleton, ICameraDeviceUserShimHelper)

CAR_SINGLETON_IMPL(CCameraDeviceUserShimHelper)

ECode CCameraDeviceUserShimHelper::ConnectBinderShim(
    /* [in] */ IICameraDeviceCallbacks* callbacks,
    /* [in] */ Int32 cameraId,
    /* [out] */ ICameraDeviceUserShim** shim)
{
    VALIDATE_NOT_NULL(shim);

    return CameraDeviceUserShim::ConnectBinderShim(callbacks, cameraId, shim);
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
