
#include "elastos/droid/hardware/CameraInfo.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CAR_INTERFACE_IMPL_2(CameraInfo, Object, ICameraInfo, IParcelable)

CameraInfo::CameraInfo()
{
    mInfo = new HardwareCamera::CameraInfo();
}

ECode CameraInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Int32 face;
    mInfo->GetFacing(&face);
    FAIL_RETURN(dest->WriteInt32(face))
    Int32 orientation;
    mInfo->GetOrientation(&orientation);
    FAIL_RETURN(dest->WriteInt32(orientation))
    return NOERROR;
}

ECode CameraInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 face;
    FAIL_RETURN(source->ReadInt32(&face))
    mInfo->SetFacing(face);
    Int32 orientation;
    FAIL_RETURN(source->ReadInt32(&orientation))
    mInfo->SetOrientation(orientation);
    return NOERROR;
}

ECode CameraInfo::GetInfo(
   /* [out] */ IHardwareCameraInfo** info)
{
    VALIDATE_NOT_NULL(info);

    *info = mInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CameraInfo::SetInfo(
   /* [in] */ IHardwareCameraInfo* info)
{
    mInfo = info;
    return NOERROR;
}

ECode CameraInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = TO_STR(mInfo);
    return NOERROR;
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

