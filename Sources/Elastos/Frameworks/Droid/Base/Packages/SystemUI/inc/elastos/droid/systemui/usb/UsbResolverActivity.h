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

#ifndef __ELASTOS_DROID_SYSTEMUI_USB_USBRESOLVERACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_USBRESOLVERACTIVITY_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/app/ResolverActivity.h"
#include "elastos/droid/systemui/usb/UsbDisconnectedReceiver.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Hardware::Usb::IUsbAccessory;
using Elastos::Droid::Hardware::Usb::IUsbDevice;
using Elastos::Droid::Internal::App::ResolverActivity;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

/* Activity for choosing an application for a USB device or accessory */
class UsbResolverActivity
    : public ResolverActivity
{
public:
    CARAPI constructor();

protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnDestroy();

    // @Override
    CARAPI OnIntentSelected(
        /* [in] */ IResolveInfo* ri,
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean alwaysCheck);

public:
    const static String TAG;
    const static String EXTRA_RESOLVE_INFOS;

private:
    AutoPtr<IUsbDevice> mDevice;
    AutoPtr<IUsbAccessory> mAccessory;
    AutoPtr<UsbDisconnectedReceiver> mDisconnectedReceiver;
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_USB_USBRESOLVERACTIVITY_H__