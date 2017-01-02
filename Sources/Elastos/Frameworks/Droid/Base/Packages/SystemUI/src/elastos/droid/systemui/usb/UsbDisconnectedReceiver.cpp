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

#include "elastos/droid/systemui/usb/UsbDisconnectedReceiver.h"

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Hardware::Usb::IUsbManager;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

UsbDisconnectedReceiver::UsbDisconnectedReceiver(
    /* [in] */ IActivity* activity,
    /* [in] */ IUsbDevice* device)
    : mActivity(activity)
    , mDevice(device)
{
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IUsbManager::ACTION_USB_DEVICE_DETACHED, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    IContext::Probe(activity)->RegisterReceiver(this, filter, (IIntent**)&intent);
}

UsbDisconnectedReceiver::UsbDisconnectedReceiver(
    /* [in] */ IActivity* activity,
    /* [in] */ IUsbAccessory* accessory)
    : mActivity(activity)
    , mAccessory(accessory)
{
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IUsbManager::ACTION_USB_ACCESSORY_DETACHED, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    IContext::Probe(activity)->RegisterReceiver(this, filter, (IIntent**)&intent);
}

ECode UsbDisconnectedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IUsbManager::ACTION_USB_DEVICE_DETACHED.Equals(action)) {
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(IUsbManager::EXTRA_DEVICE, (IParcelable**)&parcelable);
        AutoPtr<IUsbDevice> device = IUsbDevice::Probe(parcelable);
        Boolean b;
        IObject::Probe(device)->Equals(mDevice, &b);
        if (device != NULL && b) {
            mActivity->Finish();
        }
    }
    else if (IUsbManager::ACTION_USB_ACCESSORY_DETACHED.Equals(action)) {
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(IUsbManager::EXTRA_ACCESSORY, (IParcelable**)&parcelable);
        AutoPtr<IUsbAccessory> accessory = IUsbAccessory::Probe(parcelable);
        Boolean b;
        IObject::Probe(accessory)->Equals(mAccessory, &b);
        if (accessory != NULL && b) {
            mActivity->Finish();
        }
    }
    return NOERROR;
}


} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
