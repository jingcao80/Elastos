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

#include "elastos/droid/teleservice/services/telephony/sip/CSipBroadcastReceiver.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipUtil.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipAccountRegistry.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecom.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecom::ITelecomManagerHelper;
using Elastos::Droid::Telecom::CTelecomManagerHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

const String CSipBroadcastReceiver::PREFIX("[SipBroadcastReceiver] ");
const Boolean CSipBroadcastReceiver::VERBOSE = FALSE; /* STOP SHIP if true */

CAR_OBJECT_IMPL(CSipBroadcastReceiver)

ECode CSipBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (!SipUtil::IsVoipSupported(context)) {
        if (VERBOSE) Log(String("SIP VOIP not supported: ") + action);
        return NOERROR;
    }

    AutoPtr<SipAccountRegistry> sipAccountRegistry = SipAccountRegistry::GetInstance();
    assert(0);
    // if (action.Equals(ISipManager::ACTION_SIP_INCOMING_CALL)) {
    //     TakeCall(context, intent);
    // }
    // else if (action.Equals(ISipManager::ACTION_SIP_SERVICE_UP) ||
    //         action.Equals(ISipManager::ACTION_SIP_CALL_OPTION_CHANGED)) {
    //     sipAccountRegistry->Setup(context);
    // }
    // else if (action.Equals(ISipManager::ACTION_SIP_REMOVE_PHONE)) {
    //     if (VERBOSE) Log("SIP_REMOVE_PHONE " +
    //                     intent.getStringExtra(ISipManager::EXTRA_LOCAL_URI));
    //     sipAccountRegistry->RemoveSipProfile(intent.getStringExtra(ISipManager::EXTRA_LOCAL_URI));
    // }
    // else {
    //     if (VERBOSE) Log(String("onReceive, action not processed: ") + action);
    // }
    return NOERROR;
}

void CSipBroadcastReceiver::TakeCall(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (VERBOSE) Log(String("takeCall, intent: ") + TO_CSTR(intent));

    AutoPtr<IParcelable> p;
    intent->GetParcelableExtra(SipUtil::EXTRA_PHONE_ACCOUNT, (IParcelable**)&p);
    AutoPtr<IPhoneAccountHandle> accountHandle = IPhoneAccountHandle::Probe(p);
    if (accountHandle != NULL) {
        AutoPtr<IBundle> extras;
        CBundle::New((IBundle**)&extras);
        extras->PutParcelable(SipUtil::EXTRA_INCOMING_CALL_INTENT, IParcelable::Probe(intent));

        AutoPtr<ITelecomManagerHelper> helper;
        CTelecomManagerHelper::AcquireSingleton((ITelecomManagerHelper**)&helper);
        AutoPtr<ITelecomManager> manager;
        helper->From(context, (ITelecomManager**)&manager);
        manager->AddNewIncomingCall(accountHandle, extras);
    }
}

void CSipBroadcastReceiver::Log(
    /* [in] */ const String& msg)
{
    Logger::D(SipUtil::TAG, PREFIX + msg);
}

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos