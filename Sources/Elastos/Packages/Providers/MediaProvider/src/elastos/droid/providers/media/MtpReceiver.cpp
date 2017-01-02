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

#include "elastos/droid/providers/media/MtpReceiver.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::IBundle;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

const String MtpReceiver::TAG("UsbReceiver");

CAR_INTERFACE_IMPL(MtpReceiver, BroadcastReceiver, IMtpReceiver)

ECode MtpReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Logger::I(TAG, " >> OnReceive: %s", action.string());
    if (IIntent::ACTION_BOOT_COMPLETED.Equals(action)) {
        AutoPtr<IIntent> usbState;
        AutoPtr<IIntentFilter> intentFilter;
        CIntentFilter::New(IUsbManager::ACTION_USB_STATE, (IIntentFilter**)&intentFilter);
        context->RegisterReceiver(NULL, intentFilter.Get(), (IIntent**)&usbState);
        if (usbState != NULL) {
            HandleUsbState(context, usbState);
        }
    }
    else if (IUsbManager::ACTION_USB_STATE.Equals(action)) {
        HandleUsbState(context, intent);
    }
    return NOERROR;
}

ECode MtpReceiver::HandleUsbState(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IBundle> extras;
    intent->GetExtras((IBundle**)&extras);
    Boolean connected = FALSE;
    extras->GetBoolean(IUsbManager::USB_CONFIGURED, &connected);
    Boolean mtpEnabled = FALSE;
    extras->GetBoolean(IUsbManager::USB_FUNCTION_MTP, &mtpEnabled);
    Boolean ptpEnabled = FALSE;
    extras->GetBoolean(IUsbManager::USB_FUNCTION_PTP, &ptpEnabled);
    // Start MTP service if USB is connected and either the MTP or PTP function is enabled
    AutoPtr<IUri> uri;
    AutoPtr<IContentResolver> resolver;
    AutoPtr<IUriHelper> uh;
    CUriHelper::AcquireSingleton((IUriHelper**)&uh);
    if (connected && (mtpEnabled || ptpEnabled)) {
        AutoPtr<IIntent> temp;
        CIntent::New(context, ECLSID_CMtpService, (IIntent**)&temp);
        if (ptpEnabled) {
            temp->PutBooleanExtra(IUsbManager::USB_FUNCTION_PTP, TRUE);
        }
        AutoPtr<IComponentName> cn;
        context->StartService(temp, (IComponentName**)&cn);
        // tell MediaProvider MTP is connected so it can bind to the service

        uh->Parse(String("content://media/none/mtp_connected"), (IUri**)&uri);
        context->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<IUri> oUri;
        resolver->Insert(uri.Get(), NULL, (IUri**)&oUri);
    }
    else {
        AutoPtr<IIntent> temp;
        CIntent::New(context, ECLSID_CMtpService, (IIntent**)&temp);
        Boolean flag = FALSE;
        context->StopService(temp, &flag);
        // tell MediaProvider MTP is disconnected so it can unbind from the service
        uh->Parse(String("content://media/none/mtp_connected"), (IUri**)&uri);
        context->GetContentResolver((IContentResolver**)&resolver);
        Int32 vol;
        resolver->Delete(uri.Get(), String(NULL), NULL, &vol);
    }
    return NOERROR;
}

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos