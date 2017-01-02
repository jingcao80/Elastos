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

#ifndef __ELASTOS_DROID_SYSTEMUI_USB_USBDISCONNECTEDRECEIVER_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_USBDISCONNECTEDRECEIVER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Hardware::Usb::IUsbAccessory;
using Elastos::Droid::Hardware::Usb::IUsbDevice;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

// This class is used to close UsbPermissionsActivity and UsbResolverActivity
// if their device/accessory is disconnected while the dialog is still open
class UsbDisconnectedReceiver
    : public BroadcastReceiver
{
public:
    TO_STRING_IMPL("UsbDisconnectedReceiver")

    UsbDisconnectedReceiver(
        /* [in] */ IActivity* activity,
        /* [in] */ IUsbDevice* device);

    UsbDisconnectedReceiver(
        /* [in] */ IActivity* activity,
        /* [in] */ IUsbAccessory* accessory);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    AutoPtr<IActivity> mActivity;
    AutoPtr<IUsbDevice> mDevice;
    AutoPtr<IUsbAccessory> mAccessory;
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_USB_USBDISCONNECTEDRECEIVER_H__