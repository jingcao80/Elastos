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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_CAMERADEVICE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_CAMERADEVICE_H__

#include "Elastos.Droid.Hardware.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

class ECO_PUBLIC CameraDevice
    : public Object
    , public ICameraDevice
    , public ICloseable
{
public:
    class StateCallback
        : public Object
        , public ICameraDeviceStateCallback
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI OnClosed(
            /* [in] */ ICameraDevice* camera);
    };

    class StateListener
        : public StateCallback
        , public ICameraDeviceStateListener
    {
    public:
        CAR_INTERFACE_DECL()
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * To be inherited by android.hardware.camera2.* code only.
     * @hide
     */
    CameraDevice() {}

    virtual ~CameraDevice() {}
};

} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_CAMERADEVICE_H__
