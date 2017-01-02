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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONESUBINFOCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONESUBINFOCONTROLLER_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneSubInfoProxy;

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class PhoneSubInfoController
    : public Object
    , public IIPhoneSubInfo
    , public IPhoneSubInfoController
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ ArrayOf<IPhone*>* phone);

    // Use first phoneId to return unique value always
    CARAPI GetDeviceId(
        /* [out] */ String* result);

    CARAPI GetDeviceIdForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    CARAPI GetImeiForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    CARAPI GetDeviceSvn(
        /* [out] */ String* result);

    CARAPI GetSubscriberId(
        /* [out] */ String* result);

    CARAPI GetSubscriberIdForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    /**
     * Retrieves the serial number of the ICC, if applicable.
     */
    CARAPI GetIccSerialNumber(
        /* [out] */ String* result);

    CARAPI GetIccSerialNumberForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    CARAPI GetLine1Number(
        /* [out] */ String* result);

    CARAPI GetLine1NumberForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    CARAPI GetLine1AlphaTag(
        /* [out] */ String* result);

    CARAPI GetLine1AlphaTagForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    CARAPI GetMsisdn(
        /* [out] */ String* result);

    CARAPI GetMsisdnForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    CARAPI GetVoiceMailNumber(
        /* [out] */ String* result);

    CARAPI GetVoiceMailNumberForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    CARAPI GetCompleteVoiceMailNumber(
        /* [out] */ String* result);

    CARAPI GetCompleteVoiceMailNumberForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    CARAPI GetVoiceMailAlphaTag(
        /* [out] */ String* result);

    CARAPI GetVoiceMailAlphaTagForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    CARAPI GetIsimImpi(
        /* [out] */ String* result);

    CARAPI GetIsimDomain(
        /* [out] */ String* result);

    CARAPI GetIsimImpu(
        /* [out] */ ArrayOf<String>** result);

    CARAPI GetIsimIst(
        /* [out] */ String* result);

    CARAPI GetIsimPcscf(
        /* [out] */ ArrayOf<String>** result);

    CARAPI GetIsimChallengeResponse(
        /* [in] */ const String& nonce,
        /* [out] */ String* result);

    CARAPI GetIccSimChallengeResponse(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 appType,
        /* [in] */ const String& data,
        /* [out] */ String* result);

    CARAPI GetGroupIdLevel1(
        /* [out] */ String* result);

    CARAPI GetGroupIdLevel1ForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

private:
    /**
     * get Phone sub info proxy object based on subId.
     **/
    CARAPI_(AutoPtr<IPhoneSubInfoProxy>) GetPhoneSubInfoProxy(
        /* [in] */ Int64 subId);

    CARAPI_(Int64) GetDefaultSubId();

    CARAPI_(Int64) GetDefaultVoiceSubId();

    CARAPI_(Int64) GetFirstPhoneSubId();

private:
    static const String TAG;
    AutoPtr<ArrayOf<IPhone*> > mPhone;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONESUBINFOCONTROLLER_H__
