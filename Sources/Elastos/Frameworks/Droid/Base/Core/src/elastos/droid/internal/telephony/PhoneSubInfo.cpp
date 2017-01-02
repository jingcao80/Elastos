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

#include "elastos/droid/internal/telephony/PhoneSubInfo.h"
#include "elastos/droid/telephony/CPhoneNumberUtils.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/Manifest.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String PhoneSubInfo::LOGTAG("PhoneSubInfo");
const Boolean PhoneSubInfo::DBG = TRUE;
const Boolean PhoneSubInfo::VDBG = FALSE; // STOPSHIP if TRUE

const String PhoneSubInfo::READ_PHONE_STATE =
    Manifest::permission::READ_PHONE_STATE;

const String PhoneSubInfo::CALL_PRIVILEGED =
    Manifest::permission::CALL_PRIVILEGED;
const String PhoneSubInfo::READ_PRIVILEGED_PHONE_STATE =
    Manifest::permission::READ_PRIVILEGED_PHONE_STATE;

CAR_INTERFACE_IMPL(PhoneSubInfo, Object, IPhoneSubInfo)

ECode PhoneSubInfo::constructor(
    /* [in] */ IPhone* phone)
{
    mPhone = phone;
    phone->GetContext((IContext**)&mContext);
    return NOERROR;
}

ECode PhoneSubInfo::Dispose()
{
    return NOERROR;
}

ECode PhoneSubInfo::Finalize()
{
    assert(0 && "TODO");
    // super->Finalize();

    if (DBG) Log(String("PhoneSubInfo finalized"));
    return NOERROR;
}

ECode PhoneSubInfo::GetDeviceId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, String("Requires READ_PHONE_STATE"));
    return mPhone->GetDeviceId(result);
}

ECode PhoneSubInfo::GetImei(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, String("Requires READ_PHONE_STATE"));
    return mPhone->GetImei(result);
}

ECode PhoneSubInfo::GetDeviceSvn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, String("Requires READ_PHONE_STATE"));
    return mPhone->GetDeviceSvn(result);
}

ECode PhoneSubInfo::GetSubscriberId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, String("Requires READ_PHONE_STATE"));
    return mPhone->GetSubscriberId(result);
}

ECode PhoneSubInfo::GetGroupIdLevel1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, String("Requires READ_PHONE_STATE"));
    return mPhone->GetGroupIdLevel1(result);
}

ECode PhoneSubInfo::GetIccSerialNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, String("Requires READ_PHONE_STATE"));
    return mPhone->GetIccSerialNumber(result);
}

ECode PhoneSubInfo::GetLine1Number(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, String("Requires READ_PHONE_STATE"));
    return mPhone->GetLine1Number(result);
}

ECode PhoneSubInfo::GetLine1AlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, String("Requires READ_PHONE_STATE"));
    return mPhone->GetLine1AlphaTag(result);
}

ECode PhoneSubInfo::GetMsisdn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, String("Requires READ_PHONE_STATE"));
    return mPhone->GetMsisdn(result);
}

ECode PhoneSubInfo::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, String("Requires READ_PHONE_STATE"));
    String mailNum;
    mPhone->GetVoiceMailNumber(&mailNum);
    AutoPtr<IPhoneNumberUtils> pnu;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&pnu);
    String number;
    pnu->ExtractNetworkPortion(mailNum, &number);
    if (VDBG) {
        Logger::D("VM: PhoneSubInfo.getVoiceMailNUmber: %d", number);
    }
    *result = number;
    return NOERROR;
}

ECode PhoneSubInfo::GetCompleteVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(CALL_PRIVILEGED,
            String("Requires CALL_PRIVILEGED"));
    String number;
    mPhone->GetVoiceMailNumber(&number);
    if (VDBG) {
        Logger::D("VM: PhoneSubInfo.getCompleteVoiceMailNUmber: %d", number);
    }
    *result = number;
    return NOERROR;
}

ECode PhoneSubInfo::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, String("Requires READ_PHONE_STATE"));
    return mPhone->GetVoiceMailAlphaTag(result);
}

ECode PhoneSubInfo::GetIsimImpi(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE,
            String("Requires READ_PRIVILEGED_PHONE_STATE"));
    AutoPtr<IIsimRecords> isim;
    mPhone->GetIsimRecords((IIsimRecords**)&isim);
    if (isim != NULL) {
        return isim->GetIsimImpi(result);
    }
    else {
        *result = NULL;
        return NOERROR;
    }
}

ECode PhoneSubInfo::GetIsimDomain(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE,
            String("Requires READ_PRIVILEGED_PHONE_STATE"));
    AutoPtr<IIsimRecords> isim;
    mPhone->GetIsimRecords((IIsimRecords**)&isim);
    if (isim != NULL) {
        return isim->GetIsimDomain(result);
    }
    else {
        *result = NULL;
        return NOERROR;
    }
}

ECode PhoneSubInfo::GetIsimImpu(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE,
            String("Requires READ_PRIVILEGED_PHONE_STATE"));
    AutoPtr<IIsimRecords> isim;
    mPhone->GetIsimRecords((IIsimRecords**)&isim);
    if (isim != NULL) {
        return isim->GetIsimImpu(result);
    }
    else {
        *result = NULL;
        return NOERROR;
    }
}

ECode PhoneSubInfo::GetIsimIst(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE,
            String("Requires READ_PRIVILEGED_PHONE_STATE"));
    AutoPtr<IIsimRecords> isim;
    mPhone->GetIsimRecords((IIsimRecords**)&isim);
    if (isim != NULL) {
        return isim->GetIsimIst(result);
    }
    else {
        *result = NULL;
        return NOERROR;
    }
}

ECode PhoneSubInfo::GetIsimPcscf(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE,
            String("Requires READ_PRIVILEGED_PHONE_STATE"));
    AutoPtr<IIsimRecords> isim;
    mPhone->GetIsimRecords((IIsimRecords**)&isim);
    if (isim != NULL) {
        return isim->GetIsimPcscf(result);
    }
    else {
        *result = NULL;
        return NOERROR;
    }
}

ECode PhoneSubInfo::GetIsimChallengeResponse(
    /* [in] */ const String& nonce,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE,
            String("Requires READ_PRIVILEGED_PHONE_STATE"));
    AutoPtr<IIsimRecords> isim;
    mPhone->GetIsimRecords((IIsimRecords**)&isim);
    if (isim != NULL) {
        return isim->GetIsimChallengeResponse(nonce, result);
    }
    else {
        *result = String(NULL);
        return NOERROR;
    }
}

ECode PhoneSubInfo::GetIccSimChallengeResponse(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 appType,
    /* [in] */ const String& data,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // FIXME: use subId!!
    mContext->EnforceCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE,
            String("Requires READ_PRIVILEGED_PHONE_STATE"));

    AutoPtr<IUiccCard> uiccCard;
    mPhone->GetUiccCard((IUiccCard**)&uiccCard);
    if (uiccCard == NULL) {
        // Rlog::E(LOG_TAG, "GetIccSimChallengeResponse() UiccCard is NULL");
        *result = String(NULL);
        return NOERROR;
    }

    AutoPtr<IUiccCardApplication> uiccApp;
    uiccCard->GetApplicationByType(appType, (IUiccCardApplication**)&uiccApp);
    if (uiccApp == NULL) {
        // Rlog::E(LOG_TAG, "GetIccSimChallengeResponse() no app with specified type -- %d",
        //         appType);
        *result = String(NULL);
        return NOERROR;
    }
    else {
        String aid;
        uiccApp->GetAid(&aid);
        // Rlog::E(LOG_TAG, "GetIccSimChallengeResponse() found app %s specified type -- %d",
        //     aid, appType);
    }

    Int32 authContext = 0;
    uiccApp->GetAuthContext(&authContext);

    if (data.GetLength() < 32) {
        /* must use EAP_SIM context */
        // Rlog::E(LOG_TAG, "data is too small to use EAP_AKA, using EAP_SIM instead");
        authContext = IUiccCardApplication::AUTH_CONTEXT_EAP_SIM;
    }

    if(authContext == IUiccCardApplication::AUTH_CONTEXT_UNDEFINED) {
        // Rlog::E(LOG_TAG, "GetIccSimChallengeResponse() authContext undefined for app type %d",
        //         appType);
        *result = String(NULL);
        return NOERROR;
    }

    AutoPtr<IIccRecords> records;
    uiccApp->GetIccRecords((IIccRecords**)&records);
    return records->GetIccSimChallengeResponse(authContext, data, result);
}

void PhoneSubInfo::Log(
    /* [in] */ String s)
{
    // Rlog::D(LOG_TAG, s);
}

void PhoneSubInfo::Loge(
    /* [in] */ String s,
    /* [in] */ IThrowable* e)
{
    // Rlog::E(LOG_TAG, s, e);
}

void PhoneSubInfo::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Int32 permission = 0;
    mContext->CheckCallingOrSelfPermission(Manifest::permission::DUMP, &permission);
    String str;
    if (permission != IPackageManager::PERMISSION_GRANTED) {
        // str = "Permission Denial: can't dump PhoneSubInfo from from pid=";
        // str += Binder::GetCallingPid();
        // str += ", uid=";
        // str += Binder::GetCallingUid();
        // pw->Println(str);
        return;
    }

    pw->Println(String("Phone Subscriber Info:"));
    String name;
    mPhone->GetPhoneName(&name);
    str = "  Phone Type = ";
    str += name;
    pw->Println(str);
    String id;
    mPhone->GetDeviceId(&id);
    str = "  Device ID = ";
    str += id;
    pw->Println(str);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
