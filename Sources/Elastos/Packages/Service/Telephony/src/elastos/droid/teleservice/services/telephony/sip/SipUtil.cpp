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

#include "elastos/droid/teleservice/services/telephony/sip/SipUtil.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipProfileDb.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipSharedPreferences.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/AutoLock.h>
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telecom::CPhoneAccountHandle;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

const String SipUtil::TAG("SIP");
const String SipUtil::EXTRA_INCOMING_CALL_INTENT("com.android.services.telephony.sip.incoming_call_intent");
const String SipUtil::EXTRA_PHONE_ACCOUNT("com.android.services.telephony.sip.phone_account");

Boolean SipUtil::IsVoipSupported(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Boolean res1, res2;
    resources->GetBoolean(Elastos::Droid::R::bool_::config_built_in_sip_phone, &res1);
    resources->GetBoolean(Elastos::Droid::R::bool_::config_voice_capable, &res2);
    assert(0);
    //return SipManager.isVoipSupported(context) && res1 && res2;
    return FALSE;
}

AutoPtr<IPendingIntent> SipUtil::CreateIncomingCallPendingIntent(
    /* [in] */ IContext* context,
    /* [in] */ const String& sipUri)
{
    AutoPtr<IIntent> intent;
    CIntent::New(context, ECLSID_CSipBroadcastReceiver, (IIntent**)&intent);
    assert(0);
    //intent->SetAction(ISipManager::ACTION_SIP_INCOMING_CALL);
    AutoPtr<IPhoneAccountHandle> handle = CreateAccountHandle(context, sipUri);
    intent->PutExtra(EXTRA_PHONE_ACCOUNT, IParcelable::Probe(handle));
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> pendingIntent;
    helper->GetBroadcast(context, 0, intent, IPendingIntent::FLAG_UPDATE_CURRENT,
            (IPendingIntent**)&pendingIntent);
    return pendingIntent;
}

Boolean SipUtil::IsPhoneIdle(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelecomManager> manager = ITelecomManager::Probe(obj);
    if (manager != NULL) {
        Boolean res;
        manager->IsInCall(&res);
        return !res;
    }
    return TRUE;
}

AutoPtr<IPhoneAccountHandle> SipUtil::CreateAccountHandle(
    /* [in] */ IContext* context,
    /* [in] */ const String& sipUri)
{
    AutoPtr<IComponentName> name;
    assert(0);
    //CComponentName::New(context, ECLSID_CSipConnectionService, (IComponentName**)&name);
    AutoPtr<IPhoneAccountHandle> handle;
    CPhoneAccountHandle::New(name, sipUri, (IPhoneAccountHandle**)&handle);
    return handle;
}

String SipUtil::GetSipUriFromPhoneAccount(
    /* [in] */ IPhoneAccountHandle* phoneAccountHandle)
{
    if (phoneAccountHandle == NULL) {
        return String(NULL);
    }

    String sipUri;
    phoneAccountHandle->GetId(&sipUri);
    if (TextUtils::IsEmpty(sipUri)) {
        return String(NULL);
    }
    return sipUri;
}

// static CARAPI_(AutoPtr<IPhoneAccount>) CreatePhoneAccount(
//     /* [in] */ IContext* context,
//     /* [in] */ SipProfile profile);


void SipUtil::UseSipToReceiveIncomingCalls(
    /* [in] */ IContext* context,
    /* [in] */ Boolean isEnabled)
{
    AutoPtr<SipProfileDb> profileDb = new SipProfileDb(context);

    // Mark all profiles as auto-register if we are now receiving calls.
    AutoPtr<IList> sipProfileList;
    profileDb->RetrieveSipProfileList((IList**)&sipProfileList);
    assert(0);
    // for (SipProfile p : sipProfileList) {
    //     updateAutoRegistrationFlag(p, profileDb, isEnabled);
    // }
}

Boolean SipUtil::UseSipForPstnCalls(
    /* [in] */ IContext* context)
{
    AutoPtr<SipSharedPreferences> sipSharedPreferences = new SipSharedPreferences(context);
    String option;
    sipSharedPreferences->GetSipCallOption(&option);
    return option.Equals(ISettingsSystem::SIP_ALWAYS);
}

// static CARAPI_(void) UpdateAutoRegistrationFlag(
//     /* [in] */ SipProfile p,
//     /* [in] */ SipProfileDb* db,
//     /* [in] */ Boolean isEnabled);

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos