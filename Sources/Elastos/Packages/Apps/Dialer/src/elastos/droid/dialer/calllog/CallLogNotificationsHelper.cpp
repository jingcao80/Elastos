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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Telecom.h"
#include "elastos/droid/dialer/calllog/CallLogNotificationsHelper.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Dialer::CallLog::ECLSID_CCallLogNotificationsService;
using Elastos::Droid::Telecom::ITelecomManager;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

void CallLogNotificationsHelper::RemoveMissedCallNotifications(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&service);
    AutoPtr<ITelecomManager> telecomManager = ITelecomManager::Probe(service);
    telecomManager->CancelMissedCallsNotification();
}

void CallLogNotificationsHelper::UpdateVoicemailNotifications(
        /* [in] */ IContext* context)
{
    AutoPtr<IIntent> serviceIntent;
    CIntent::New(context,
            ECLSID_CCallLogNotificationsService, (IIntent**)&serviceIntent);
    serviceIntent->SetAction(ICallLogNotificationsService::ACTION_UPDATE_NOTIFICATIONS);
    AutoPtr<IComponentName> name;
    context->StartService(serviceIntent, (IComponentName**)&name);
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
