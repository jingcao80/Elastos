
#include "elastos/droid/hardware/CHardwareCameraHelper.h"
#include "elastos/droid/hardware/HardwareCamera.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CAR_INTERFACE_IMPL(CHardwareCameraHelper, Singleton, IHardwareCameraHelper)

CAR_SINGLETON_IMPL(CHardwareCameraHelper)

ECode CHardwareCameraHelper::GetCameraInfo(
    /* [in] */ Int32 cameraId,
    /* [in] */ IHardwareCameraInfo* cameraInfo)
{
    return HardwareCamera::GetCameraInfo(cameraId, cameraInfo);
}

ECode CHardwareCameraHelper::Open(
    /* [in] */ Int32 cameraId,
    /* [out] */ IHardwareCamera** camera)
{
    return HardwareCamera::Open(cameraId, camera);
}

ECode CHardwareCameraHelper::Open(
    /* [out] */ IHardwareCamera** camera)
{
    return HardwareCamera::Open(camera);
}

ECode CHardwareCameraHelper::GetEmptyParameters(
    /* [out] */ IParameters** para)
{
    return HardwareCamera::GetEmptyParameters(para);
}

ECode CHardwareCameraHelper::GetNumberOfCameras(
    /* [out] */ Int32* num)
{
    return HardwareCamera::GetNumberOfCameras(num);
}

ECode CHardwareCameraHelper::CheckInitErrors(
    /* [in] */ Int32 err,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = HardwareCamera::CheckInitErrors(err);
    return NOERROR;
}

ECode CHardwareCameraHelper::OpenUninitialized(
    /* [out] */ IHardwareCamera** camera)
{
    VALIDATE_NOT_NULL(camera);
    AutoPtr<IHardwareCamera> temp = HardwareCamera::OpenUninitialized();
    *camera = temp;
    REFCOUNT_ADD(*camera);
    return NOERROR;
}

ECode CHardwareCameraHelper::GetParametersCopy(
    /* [in] */ IParameters* parameters,
    /* [out] */ IParameters** para)
{
    return HardwareCamera::GetParametersCopy(parameters, para);
}

ECode CHardwareCameraHelper::OpenLegacy(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 halVersion,
    /* [out] */ IHardwareCamera** camera)
{
    return HardwareCamera::OpenLegacy(cameraId, halVersion, camera);
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos
