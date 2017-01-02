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

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/dialer/calllog/CCallLogNotificationsService.h"
#include "elastos/droid/dialer/calllog/DefaultVoicemailNotifier.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Dialer::CallLog::EIID_ICallLogNotificationsService;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

const String CCallLogNotificationsService::TAG("CallLogNotificationsService");

CAR_INTERFACE_IMPL(CCallLogNotificationsService, IntentService, ICallLogNotificationsService)

CAR_OBJECT_IMPL(CCallLogNotificationsService)

ECode CCallLogNotificationsService::constructor()
{
    return IntentService::constructor(TAG);
}

ECode CCallLogNotificationsService::OnCreate()
{
    IntentService::OnCreate();

    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    mCallLogQueryHandler = new CallLogQueryHandler();
    mCallLogQueryHandler->constructor(resolver, NULL /*listener*/);
    return NOERROR;
}

ECode CCallLogNotificationsService::OnHandleIntent(
    /* [in] */ IIntent* intent)
{
    if (intent == NULL) {
        Logger::D(TAG, "onHandleIntent: could not handle null intent");
        return NOERROR;
    }

    String action;
    intent->GetAction(&action);
    if (ICallLogNotificationsService::ACTION_MARK_NEW_VOICEMAILS_AS_OLD.Equals(action)) {
        mCallLogQueryHandler->MarkNewVoicemailsAsOld();
    }
    else if (ICallLogNotificationsService::ACTION_UPDATE_NOTIFICATIONS.Equals(action)) {
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(
                ICallLogNotificationsService::EXTRA_NEW_VOICEMAIL_URI, (IParcelable**)&parcelable);
        AutoPtr<IUri> voicemailUri = IUri::Probe(parcelable);
        DefaultVoicemailNotifier::GetInstance(this)->UpdateNotification(voicemailUri);
    }
    else {
        Logger::D(TAG, "onHandleIntent: could not handle: %s", TO_CSTR(intent));
    }

    return NOERROR;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos

