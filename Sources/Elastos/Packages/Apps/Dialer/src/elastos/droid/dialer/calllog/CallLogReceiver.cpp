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

#include "_Elastos.Apps.Dialer.h"
#include "elastos/droid/dialer/calllog/CallLogReceiver.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::IVoicemailContract;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

const String CallLogReceiver::TAG("CallLogReceiver");

ECode CallLogReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IVoicemailContract::ACTION_NEW_VOICEMAIL.Equals(action)) {
        AutoPtr<IIntent> serviceIntent;
        CIntent::New(context,
                ECLSID_CCallLogNotificationsService, (IIntent**)&serviceIntent);
        serviceIntent->SetAction(ICallLogNotificationsService::ACTION_UPDATE_NOTIFICATIONS);

        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        serviceIntent->PutExtra(
                ICallLogNotificationsService::EXTRA_NEW_VOICEMAIL_URI, IParcelable::Probe(data));
        AutoPtr<IComponentName> name;
        context->StartService(serviceIntent, (IComponentName**)&name);
    }
    else if (IIntent::ACTION_BOOT_COMPLETED.Equals(action)) {
        AutoPtr<IIntent> serviceIntent;
        CIntent::New(context,
                ECLSID_CCallLogNotificationsService, (IIntent**)&serviceIntent);
        serviceIntent->SetAction(ICallLogNotificationsService::ACTION_UPDATE_NOTIFICATIONS);
        AutoPtr<IComponentName> name;
        context->StartService(serviceIntent, (IComponentName**)&name);
    }
    else {
        String str;
        IObject::Probe(intent)->ToString(&str);
        Logger::W(TAG, "onReceive: could not handle: %s", str.string());
    }

    return NOERROR;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
