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

CAR_INTERFACE_IMPL(CameraDevice::StateListener, StateCallback, ICameraDeviceStateListener)

CAR_INTERFACE_IMPL_2(CameraDevice, Object, ICameraDevice, ICloseable)

} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos