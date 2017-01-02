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

#include "elastos/droid/internal/telephony/CTelephonyCapabilities.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  CTelephonyCapabilities::
//==============================================================
const String CTelephonyCapabilities::LOGTAG("CTelephonyCapabilities");

CAR_SINGLETON_IMPL(CTelephonyCapabilities)

CAR_INTERFACE_IMPL(CTelephonyCapabilities, Singleton, ITelephonyCapabilities)

CTelephonyCapabilities::CTelephonyCapabilities()
{
}

ECode CTelephonyCapabilities::SupportsEcm(
    /* [in] */ IPhone* phone,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 type = 0;
    phone->GetPhoneType(&type);
    AutoPtr<IPhone> ph;
    phone->GetImsPhone((IPhone**)&ph);
    String str("supportsEcm: Phone type = ");
    //str += type;
    str += " Ims Phone = ";
    assert(0 && "TODO");
    // str += ph;
    // Rlog::D(LOGTAG, str);
    *result = (type == IPhoneConstants::PHONE_TYPE_CDMA ||
            ph != NULL);
    return NOERROR;
}

ECode CTelephonyCapabilities::SupportsOtasp(
    /* [in] */ IPhone* phone,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 type = 0;
    phone->GetPhoneType(&type);
    *result = type == IPhoneConstants::PHONE_TYPE_CDMA;
    return NOERROR;
}

ECode CTelephonyCapabilities::SupportsVoiceMessageCount(
    /* [in] */ IPhone* phone,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 count = 0;
    phone->GetVoiceMessageCount(&count);
    *result = count != -1;
    return NOERROR;
}

ECode CTelephonyCapabilities::SupportsNetworkSelection(
    /* [in] */ IPhone* phone,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 type = 0;
    phone->GetPhoneType(&type);
    *result = (type == IPhoneConstants::PHONE_TYPE_GSM);
    return NOERROR;
}

ECode CTelephonyCapabilities::GetDeviceIdLabel(
    /* [in] */ IPhone* phone,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 type = 0;
    phone->GetPhoneType(&type);
    if (type == IPhoneConstants::PHONE_TYPE_GSM) {
        *result = R::string::imei;
        return NOERROR;
    }
    else if (type == IPhoneConstants::PHONE_TYPE_CDMA) {
        *result = R::string::meid;
        return NOERROR;
    }
    else {
        String name;
        phone->GetPhoneName(&name);
        String str("getDeviceIdLabel: no known label for phone ");
        str += name;
        // Rlog::W(LOGTAG, str);
        *result = 0;
        return NOERROR;
    }
}

ECode CTelephonyCapabilities::SupportsConferenceCallManagement(
    /* [in] */ IPhone* phone,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 type = 0;
    phone->GetPhoneType(&type);
    *result = ((type == IPhoneConstants::PHONE_TYPE_GSM)
            || (type == IPhoneConstants::PHONE_TYPE_SIP));
    return NOERROR;
}

ECode CTelephonyCapabilities::SupportsHoldAndUnhold(
    /* [in] */ IPhone* phone,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 type = 0;
    phone->GetPhoneType(&type);
    *result = ((type == IPhoneConstants::PHONE_TYPE_GSM)
            || (type == IPhoneConstants::PHONE_TYPE_SIP)
            || (type == IPhoneConstants::PHONE_TYPE_IMS));
    return NOERROR;
}

ECode CTelephonyCapabilities::SupportsAnswerAndHold(
    /* [in] */ IPhone* phone,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 type = 0;
    phone->GetPhoneType(&type);
    *result = ((type == IPhoneConstants::PHONE_TYPE_GSM)
            || (type == IPhoneConstants::PHONE_TYPE_SIP));
    return NOERROR;
}

ECode CTelephonyCapabilities::SupportsAdn(
    /* [in] */ Int32 phoneType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ((phoneType == IPhoneConstants::PHONE_TYPE_GSM)
            || (phoneType == IPhoneConstants::PHONE_TYPE_CDMA));
    return NOERROR;
}

ECode CTelephonyCapabilities::CanDistinguishDialingAndConnected(
    /* [in] */ Int32 phoneType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = phoneType == IPhoneConstants::PHONE_TYPE_GSM;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
