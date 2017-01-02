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

#include "elastos/droid/server/telecom/TelephonyUtil.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Telecom.h>
#include <Elastos.Droid.Telephony.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Telecom::CPhoneAccountBuilder;
using Elastos::Droid::Telecom::CPhoneAccountHandle;
using Elastos::Droid::Telecom::IPhoneAccountBuilder;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

const String TAG("TelephonyUtil");
const String TelephonyUtil::TELEPHONY_PACKAGE_NAME("Elastos.Droid.TeleService");
const String TelephonyUtil::PSTN_CALL_SERVICE_CLASS_NAME("Elastos.Droid.TeleService.Services.Telephony.CTelephonyConnectionService");
AutoPtr<IPhoneAccountHandle> TelephonyUtil::DEFAULT_EMERGENCY_PHONE_ACCOUNT_HANDLE = InitDEFAULT_EMERGENCY_PHONE_ACCOUNT_HANDLE();

TelephonyUtil::TelephonyUtil()
{}

ECode TelephonyUtil::GetDefaultEmergencyPhoneAccount(
    /* [out] */ IPhoneAccount** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhoneAccountBuilder> builder;
    CPhoneAccountBuilder::New(DEFAULT_EMERGENCY_PHONE_ACCOUNT_HANDLE, StringUtils::ParseCharSequence(String("E")), (IPhoneAccountBuilder**)&builder);
    builder->SetCapabilities(IPhoneAccount::CAPABILITY_SIM_SUBSCRIPTION |
            IPhoneAccount::CAPABILITY_CALL_PROVIDER |
            IPhoneAccount::CAPABILITY_PLACE_EMERGENCY_CALLS);
    builder->Build(result);
    return NOERROR;
}

ECode TelephonyUtil::IsPstnComponentName(
    /* [in] */ IComponentName* componentName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IComponentName> pstnComponentName;
    CComponentName::New(
            TELEPHONY_PACKAGE_NAME, PSTN_CALL_SERVICE_CLASS_NAME, (IComponentName**)&pstnComponentName);
    return IObject::Probe(pstnComponentName)->Equals(componentName, result);
}

ECode TelephonyUtil::ShouldProcessAsEmergency(
    /* [in] */ IContext* context,
    /* [in] */ IUri* handle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
    String schemeSpecificPart;
    handle->GetSchemeSpecificPart(&schemeSpecificPart);
    Boolean isPotentialLocalEmergencyNumber;
    phoneNumberUtilsHelper->IsPotentialLocalEmergencyNumber(context, schemeSpecificPart, &isPotentialLocalEmergencyNumber);
    *result = handle != NULL && isPotentialLocalEmergencyNumber;
    return NOERROR;
}

AutoPtr<IPhoneAccountHandle> TelephonyUtil::InitDEFAULT_EMERGENCY_PHONE_ACCOUNT_HANDLE()
{
    AutoPtr<IPhoneAccountHandle> rev;
    AutoPtr<IComponentName> componentName;
    CComponentName::New(TELEPHONY_PACKAGE_NAME, PSTN_CALL_SERVICE_CLASS_NAME, (IComponentName**)&componentName);
    CPhoneAccountHandle::New(componentName , String("E"), (IPhoneAccountHandle**)&rev);
    return rev;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
