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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERAINFO_H__
#define __ELASTOS_DROID_HARDWARE_CAMERAINFO_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/hardware/HardwareCamera.h"
#include <elastos/core/Object.h>
#include <camera/Camera.h>

using Elastos::Droid::Hardware::IHardwareCameraInfo;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Hardware {

class CameraInfo
    : public Object
    , public ICameraInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CameraInfo();

    virtual ~CameraInfo() {}

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);


    // public static final Parcelable.Creator<CameraInfo> CREATOR =
    //         new Parcelable.Creator<CameraInfo>() {
    //     @Override
    //     public CameraInfo createFromParcel(Parcel in) {
    //         CameraInfo info = new CameraInfo();
    //         info.readFromParcel(in);

    //         return info;
    //     }

    //     @Override
    //     public CameraInfo[] newArray(int size) {
    //         return new CameraInfo[size];
    //     }
    // };

    CARAPI GetInfo(
       /* [out] */ IHardwareCameraInfo** info);

    CARAPI SetInfo(
       /* [in] */ IHardwareCameraInfo* info);

    CARAPI ToString(
        /* [out] */ String* str);
public:
// Can't parcel nested classes, so make this a top level class that composes
    // CameraInfo.
    //AutoPtr<Camera::CameraInfo> info;
    AutoPtr<IHardwareCameraInfo> mInfo;
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CAMERAINFO_H__
