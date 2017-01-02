//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

