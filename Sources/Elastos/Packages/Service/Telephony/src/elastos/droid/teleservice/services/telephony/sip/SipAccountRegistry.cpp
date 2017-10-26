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

#include "elastos/droid/teleservice/services/telephony/sip/SipAccountRegistry.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipUtil.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipSharedPreferences.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.CoreLibrary.Core.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecom::ITelecomManagerHelper;
using Elastos::Droid::Telecom::CTelecomManagerHelper;
using Elastos::Core::IRunnable;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Concurrent::CCopyOnWriteArrayList;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

ECode SipAccountRegistry::MyRunnable::Run()
{
    mHost->StartSipProfiles(mContext, mSipUri);
    return NOERROR;
}

const String SipAccountRegistry::PREFIX("[SipAccountRegistry] ");
const Boolean SipAccountRegistry::VERBOSE = FALSE; /* STOP SHIP if true */
const AutoPtr<SipAccountRegistry> SipAccountRegistry::INSTANCE = new SipAccountRegistry();

SipAccountRegistry::SipAccountRegistry()
{
    CCopyOnWriteArrayList::New((IList**)&mAccounts);
}

AutoPtr<SipAccountRegistry> SipAccountRegistry::GetInstance()
{
    return INSTANCE;
}

ECode SipAccountRegistry::Setup(
    /* [in] */ IContext* context)
{
    StartSipProfilesAsync(context, String(NULL));
    return NOERROR;
}

ECode SipAccountRegistry::StartSipService(
    /* [in] */ IContext* context,
    /* [in] */ const String& sipUri)
{
    StartSipProfilesAsync(context, sipUri);
    return NOERROR;
}

ECode SipAccountRegistry::RemoveSipProfile(
    /* [in] */ const String& sipUri)
{
    AutoPtr<AccountEntry> accountEntry = GetAccountEntry(sipUri);

    if (accountEntry != NULL) {
        mAccounts->Remove(TO_IINTERFACE(accountEntry));
    }
    return NOERROR;
}

ECode SipAccountRegistry::StopSipService(
    /* [in] */ IContext* context,
    /* [in] */ const String& sipUri)
{
    // Stop the sip service for the profile.
    AutoPtr<AccountEntry> accountEntry = GetAccountEntry(sipUri);
    if (accountEntry != NULL ) {
        assert(0);
        // SipManager sipManager = SipManager.newInstance(context);
        // accountEntry->StopSipService(sipManager);
    }

    // Un-register its PhoneAccount.
    AutoPtr<IPhoneAccountHandle> handle = SipUtil::CreateAccountHandle(context, sipUri);
    AutoPtr<ITelecomManagerHelper> helper;
    CTelecomManagerHelper::AcquireSingleton((ITelecomManagerHelper**)&helper);
    AutoPtr<ITelecomManager> manager;
    helper->From(context, (ITelecomManager**)&manager);
    return manager->UnregisterPhoneAccount(handle);
}

ECode SipAccountRegistry::RestartSipService(
    /* [in] */ IContext* context)
{
    StartSipProfiles(context, String(NULL));
    return NOERROR;
}

void SipAccountRegistry::StartSipProfilesAsync(
    /* [in] */ IContext* context,
    /* [in] */ const String& sipUri)
{
    if (VERBOSE) Log(String("startSipProfiles, start auto registration"));

    AutoPtr<IRunnable> r = new MyRunnable(this, context, sipUri);
    AutoPtr<IThread> t;
    CThread::New(r, (IThread**)&t);
    t->Start();
}

void SipAccountRegistry::StartSipProfiles(
    /* [in] */ IContext* context,
    /* [in] */ const String& sipUri)
{
    AutoPtr<SipSharedPreferences> sipSharedPreferences = new SipSharedPreferences(context);
    Boolean isReceivingCalls;
    sipSharedPreferences->IsReceivingCallsEnabled(&isReceivingCalls);
    String primaryProfile;
    sipSharedPreferences->GetPrimaryAccount(&primaryProfile);

    AutoPtr<ITelecomManagerHelper> helper;
    CTelecomManagerHelper::AcquireSingleton((ITelecomManagerHelper**)&helper);
    AutoPtr<ITelecomManager> telecomManager;
    helper->From(context, (ITelecomManager**)&telecomManager);

    assert(0);
    //SipManager sipManager = SipManager.newInstance(context);
    AutoPtr<SipProfileDb> profileDb = new SipProfileDb(context);
    AutoPtr<IList> sipProfileList;
    profileDb->RetrieveSipProfileList((IList**)&sipProfileList);

    assert(0);
    // for (SipProfile profile : sipProfileList) {
    //     // Register a PhoneAccount for the profile and optionally enable the primary
    //     // profile.
    //     if (sipUri == null || Objects.equals(sipUri, profile.getUriString())) {
    //         PhoneAccount phoneAccount = SipUtil.createPhoneAccount(context, profile);
    //         telecomManager.registerPhoneAccount(phoneAccount);
    //     }

    //     if (sipUri == null || Objects.equals(sipUri, profile.getUriString())) {
    //         startSipServiceForProfile(profile, sipManager, context, isReceivingCalls);
    //     }
    // }

    if (!primaryProfile.IsNull()) {
        // Remove the primary account shared preference, ensuring the migration does not
        // occur again in the future.
        sipSharedPreferences->CleanupPrimaryAccountSetting();
    }
}

// void SipAccountRegistry::startSipServiceForProfile(
//     /* [in] */ SipProfile* profile,
//     /* [in] */ SipManager* sipManager,
//     /* [in] */ IContext* context,
//     /* [in] */ Boolean isReceivingCalls)
// {
//     RemoveSipProfile(profile.getUriString());

//     AutoPtr<AccountEntry> entry = new AccountEntry(profile);
//     Boolean res;
//     if (entry->StartSipService(sipManager, context, isReceivingCalls, &res), res) {
//         mAccounts->Add(TO_IINTERFACE(entry));
//     }
// }

AutoPtr<SipAccountRegistry::AccountEntry> SipAccountRegistry::GetAccountEntry(
    /* [in] */ const String& sipUri)
{
    Int32 size;
    mAccounts->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mAccounts->Get(i, (IInterface**)&obj);
        AutoPtr<SipAccountRegistry::AccountEntry> entry = (SipAccountRegistry::AccountEntry*)IObject::Probe(obj);

        assert(0);
        // if (Objects::Equals(sipUri, entry.getProfile().getUriString())) {
        //     return entry;
        // }

    }
    return NULL;
}

void SipAccountRegistry::Log(
    /* [in] */ const String& message)
{
    Logger::D(SipUtil::TAG, PREFIX + message);
}

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos