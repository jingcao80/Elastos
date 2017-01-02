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

#include "elastos/droid/hardware/usb/CUsbAccessory.h"
#include "elastos/droid/hardware/usb/CUsbDevice.h"
#include "elastos/droid/hardware/usb/CUsbDeviceConnection.h"
#include "elastos/droid/hardware/usb/CUsbEndpoint.h"
#include "elastos/droid/hardware/usb/CUsbInterface.h"
#include "elastos/droid/hardware/usb/CUsbManager.h"
#include "elastos/droid/hardware/usb/CUsbRequest.h"
#include "elastos/droid/hardware/usb/CUsbConfiguration.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CAR_OBJECT_IMPL(CUsbAccessory)

CAR_OBJECT_IMPL(CUsbDevice)

CAR_OBJECT_IMPL(CUsbDeviceConnection)

CAR_OBJECT_IMPL(CUsbEndpoint)

CAR_OBJECT_IMPL(CUsbInterface)

CAR_OBJECT_IMPL(CUsbManager)

CAR_OBJECT_IMPL(CUsbRequest)

CAR_OBJECT_IMPL(CUsbConfiguration)

} //Usb
} //Hardware
} //Droid
} //Elastos
