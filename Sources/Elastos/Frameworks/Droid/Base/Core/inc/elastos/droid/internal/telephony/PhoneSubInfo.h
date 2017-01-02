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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONESUBINFO_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONESUBINFO_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

using Elastos::Core::Object;
using Elastos::Core::IThrowable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class PhoneSubInfo
    : public Object
    , public IPhoneSubInfo
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IPhone* phone);

    CARAPI Dispose();

    //@Override
    CARAPI Finalize();

    /**
     * Retrieves the unique device ID, e.g., IMEI for GSM phones and MEID for CDMA phones.
     */
    CARAPI GetDeviceId(
        /* [out] */ String* result);

    /**
     * Retrieves the IMEI.
     */
    CARAPI GetImei(
        /* [out] */ String* result);

    /**
     * Retrieves the software version number for the device, e.g., IMEI/SV
     * for GSM phones.
     */
    CARAPI GetDeviceSvn(
        /* [out] */ String* result);

    /**
     * Retrieves the unique subscriber ID, e.g., IMSI for GSM phones.
     */
    CARAPI GetSubscriberId(
        /* [out] */ String* result);

    /**
     * Retrieves the Group Identifier Level1 for GSM phones.
     */
    CARAPI GetGroupIdLevel1(
        /* [out] */ String* result);

    /**
     * Retrieves the serial number of the ICC, if applicable.
     */
    CARAPI GetIccSerialNumber(
        /* [out] */ String* result);

    /**
     * Retrieves the phone number string for line 1.
     */
    CARAPI GetLine1Number(
        /* [out] */ String* result);

    /**
     * Retrieves the alpha identifier for line 1.
     */
    CARAPI GetLine1AlphaTag(
        /* [out] */ String* result);

    /**
     * Retrieves the MSISDN string.
     */
    CARAPI GetMsisdn(
        /* [out] */ String* result);

    /**
     * Retrieves the voice mail number.
     */
    CARAPI GetVoiceMailNumber(
        /* [out] */ String* result);

    /**
     * Retrieves the complete voice mail number.
     *
     * @hide
     */
    CARAPI GetCompleteVoiceMailNumber(
        /* [out] */ String* result);

    /**
     * Retrieves the alpha identifier associated with the voice mail number.
     */
    CARAPI GetVoiceMailAlphaTag(
        /* [out] */ String* result);

    /**
     * Returns the IMS private user Identity (IMPI) that was loaded from the ISIM.
     * @return the IMPI, or NULL if not present or not loaded
     */
    CARAPI GetIsimImpi(
        /* [out] */ String* result);

    /**
     * Returns the IMS home network domain name that was loaded from the ISIM.
     * @return the IMS domain name, or NULL if not present or not loaded
     */
    CARAPI GetIsimDomain(
        /* [out] */ String* result);

    /**
     * Returns the IMS public user Identities (IMPU) that were loaded from the ISIM.
     * @return an array of IMPU strings, with one IMPU per string, or NULL if
     *      not present or not loaded
     */
    CARAPI GetIsimImpu(
        /* [out] */ ArrayOf<String>** result);

    /**
     * Returns the IMS Service Table (IST) that was loaded from the ISIM.
     * @return IMS Service Table or NULL if not present or not loaded
     */
    CARAPI GetIsimIst(
        /* [out] */ String* result);

    /**
     * Returns the IMS Proxy Call Session Control Function(PCSCF) that were loaded from the ISIM.
     * @return an array of  PCSCF strings with one PCSCF per string, or NULL if
     *      not present or not loaded
     */
    CARAPI GetIsimPcscf(
        /* [out] */ ArrayOf<String>** result);

    /**
     * Returns the response of ISIM Authetification through RIL.
     * Returns NULL if the Authentification hasn't been successed or isn't present iphonesubinfo.
     * @return the response of ISIM Authetification, or NULL if not available
     */
    CARAPI GetIsimChallengeResponse(
        /* [in] */ const String& nonce,
        /* [out] */ String* result);

    /**
     * Returns the response of the SIM application on the UICC to authentication
     * challenge/response algorithm. The data string and challenge response are
     * Base64 encoded Strings.
     * Can support EAP-SIM, EAP-AKA with results encoded per 3GPP TS 31.102.
     *
     * @param appType ICC application Family (@see com.android.internal.telephony.PhoneConstants#APPTYPE_xxx)
     * @param data authentication challenge data
     * @return challenge response
     */
    CARAPI GetIccSimChallengeResponse(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 appType,
        /* [in] */ const String& data,
        /* [out] */ String* result);

    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(void) Log(
        /* [in] */ String s);

    CARAPI_(void) Loge(
        /* [in] */ String s,
        /* [in] */ IThrowable* e);

public:
    static const String LOGTAG;

private:
    static const Boolean DBG;
    static const Boolean VDBG; // STOPSHIP if TRUE

    AutoPtr<IPhone> mPhone;
    AutoPtr<IContext> mContext;
    static const String READ_PHONE_STATE;
    // TODO: change GetCompleteVoiceMailNumber() to require READ_PRIVILEGED_PHONE_STATE
    static const String CALL_PRIVILEGED;
    static const String READ_PRIVILEGED_PHONE_STATE;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONESUBINFO_H__
