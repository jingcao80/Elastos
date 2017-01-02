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

#include "elastos/droid/internal/telephony/PhoneSubInfoProxy.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL_2(PhoneSubInfoProxy, Object, IIPhoneSubInfo, IPhoneSubInfoProxy)

ECode PhoneSubInfoProxy::constructor(
    /* [in] */ IPhoneSubInfo* phoneSubInfo)
{
    mPhoneSubInfo = phoneSubInfo;
    return NOERROR;
}

ECode PhoneSubInfoProxy::SetmPhoneSubInfo(
    /* [in] */ IPhoneSubInfo* phoneSubInfo)
{
    mPhoneSubInfo = phoneSubInfo;
    return NOERROR;
}

ECode PhoneSubInfoProxy::GetDeviceId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetDeviceId(result);
}

ECode PhoneSubInfoProxy::GetImei(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetImei(result);
}

ECode PhoneSubInfoProxy::GetDeviceSvn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetDeviceSvn(result);
}

ECode PhoneSubInfoProxy::GetSubscriberId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetSubscriberId(result);
}

ECode PhoneSubInfoProxy::GetGroupIdLevel1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetGroupIdLevel1(result);
}

ECode PhoneSubInfoProxy::GetIccSerialNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetIccSerialNumber(result);
}

ECode PhoneSubInfoProxy::GetLine1Number(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetLine1Number(result);
}

ECode PhoneSubInfoProxy::GetLine1AlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetLine1AlphaTag(result);
}

ECode PhoneSubInfoProxy::GetMsisdn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetMsisdn(result);
}

ECode PhoneSubInfoProxy::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetVoiceMailNumber(result);
}

ECode PhoneSubInfoProxy::GetCompleteVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetCompleteVoiceMailNumber(result);
}

ECode PhoneSubInfoProxy::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetVoiceMailAlphaTag(result);
}

ECode PhoneSubInfoProxy::GetIsimImpi(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetIsimImpi(result);
}

ECode PhoneSubInfoProxy::GetIsimDomain(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetIsimDomain(result);
}

ECode PhoneSubInfoProxy::GetIsimImpu(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetIsimImpu(result);
}

ECode PhoneSubInfoProxy::GetDeviceIdForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // FIXME: getDeviceIdForSubscriber
    *result = String(NULL);
    return NOERROR;
}

ECode PhoneSubInfoProxy::GetImeiForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // FIXME: getImeiForSubscriber
    *result = String(NULL);
    return NOERROR;
}

ECode PhoneSubInfoProxy::GetSubscriberIdForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // FIXME: getSubscriberIdForSubscriber
    *result = String(NULL);
    return NOERROR;
}

ECode PhoneSubInfoProxy::GetGroupIdLevel1ForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // FIXME: getGroupIdLevel1ForSubscriber
    *result = String(NULL);
    return NOERROR;
}

ECode PhoneSubInfoProxy::GetIccSerialNumberForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // FIXME: getIccSerialNumberForSubscriber
    *result = String(NULL);
    return NOERROR;
}

ECode PhoneSubInfoProxy::GetLine1NumberForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // FIXME: getLine1NumberForSubscriber
    *result = String(NULL);
    return NOERROR;
}

ECode PhoneSubInfoProxy::GetLine1AlphaTagForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // FIXME: getLine1AlphaTagForSubscriber
    *result = String(NULL);
    return NOERROR;
}

ECode PhoneSubInfoProxy::GetMsisdnForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // FIXME: getMsisdnForSubscriber
    *result = String(NULL);
    return NOERROR;
}

ECode PhoneSubInfoProxy::GetVoiceMailNumberForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // FIXME: getVoiceMailNumberForSubscriber
    *result = String(NULL);
    return NOERROR;
}

ECode PhoneSubInfoProxy::GetCompleteVoiceMailNumberForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // FIXME: getCompleteVoiceMailNumberForSubscriber
    *result = String(NULL);
    return NOERROR;
}

ECode PhoneSubInfoProxy::GetVoiceMailAlphaTagForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // FIXME: getVoiceMailAlphaTagForSubscriber
    *result = String(NULL);
    return NOERROR;
}

ECode PhoneSubInfoProxy::GetIsimIst(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetIsimIst(result);
}

ECode PhoneSubInfoProxy::GetIsimPcscf(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetIsimPcscf(result);
}

ECode PhoneSubInfoProxy::GetIsimChallengeResponse(
    /* [in] */ const String& nonce,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetIsimChallengeResponse(nonce, result);
}

ECode PhoneSubInfoProxy::GetIccSimChallengeResponse(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 appType,
    /* [in] */ const String& data,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSubInfo->GetIccSimChallengeResponse(subId, appType, data, result);
}

void PhoneSubInfoProxy::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    assert(0 && "TODO");
    // mPhoneSubInfo->Dump(fd, pw, args);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
