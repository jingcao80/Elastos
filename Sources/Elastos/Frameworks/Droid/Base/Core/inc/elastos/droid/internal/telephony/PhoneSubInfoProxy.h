
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONESUBINFOPROXY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONESUBINFOPROXY_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class PhoneSubInfoProxy
    : public Object
    , public IIPhoneSubInfo
    , public IPhoneSubInfoProxy
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IPhoneSubInfo* phoneSubInfo);

    CARAPI SetmPhoneSubInfo(
        /* [in] */ IPhoneSubInfo* phoneSubInfo);

    //@Override
    CARAPI GetDeviceId(
        /* [out] */ String* result);

    CARAPI GetImei(
        /* [out] */ String* result);

    //@Override
    CARAPI GetDeviceSvn(
        /* [out] */ String* result);

    /**
     * Retrieves the unique subscriber ID, e.g., IMSI for GSM phones.
     */
    //@Override
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
    //@Override
    CARAPI GetIccSerialNumber(
        /* [out] */ String* result);

    /**
     * Retrieves the phone number string for line 1.
     */
    //@Override
    CARAPI GetLine1Number(
        /* [out] */ String* result);

    /**
     * Retrieves the alpha identifier for line 1.
     */
    //@Override
    CARAPI GetLine1AlphaTag(
        /* [out] */ String* result);

    /**
     * Retrieves the MSISDN Number.
     */
    //@Override
    CARAPI GetMsisdn(
        /* [out] */ String* result);

    /**
     * Retrieves the voice mail number.
     */
    //@Override
    CARAPI GetVoiceMailNumber(
        /* [out] */ String* result);

    /**
     * Retrieves the complete voice mail number.
     */
    //@Override
    CARAPI GetCompleteVoiceMailNumber(
        /* [out] */ String* result);

    /**
     * Retrieves the alpha identifier associated with the voice mail number.
     */
    //@Override
    CARAPI GetVoiceMailAlphaTag(
        /* [out] */ String* result);

    /**
     * Returns the IMS private user Identity (IMPI) that was loaded from the ISIM.
     * @return the IMPI, or NULL if not present or not loaded
     */
    //@Override
    CARAPI GetIsimImpi(
        /* [out] */ String* result);

    /**
     * Returns the IMS home network domain name that was loaded from the ISIM.
     * @return the IMS domain name, or NULL if not present or not loaded
     */
    //@Override
    CARAPI GetIsimDomain(
        /* [out] */ String* result);

    /**
     * Returns the IMS public user Identities (IMPU) that were loaded from the ISIM.
     * @return an array of IMPU strings, with one IMPU per string, or NULL if
     *      not present or not loaded
     */
    //@Override
    CARAPI GetIsimImpu(
        /* [out] */ ArrayOf<String>** result);

    //@Override
    CARAPI GetDeviceIdForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    //@Override
    CARAPI GetImeiForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    //@Override
    CARAPI GetSubscriberIdForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    //@Override
    CARAPI GetGroupIdLevel1ForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    //@Override
    CARAPI GetIccSerialNumberForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    //@Override
    CARAPI GetLine1NumberForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    //@Override
    CARAPI GetLine1AlphaTagForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    //@Override
    CARAPI GetMsisdnForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    //@Override
    CARAPI GetVoiceMailNumberForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    //@Override
    CARAPI GetCompleteVoiceMailNumberForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    //@Override
    CARAPI GetVoiceMailAlphaTagForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    /**
     * Returns the IMS Service Table (IST) that was loaded from the ISIM.
     * @return IMS Service Table or NULL if not present or not loaded
     */
    //@Override
    CARAPI GetIsimIst(
        /* [out] */ String* result);

    /**
     * Returns the IMS Proxy Call Session Control Function(PCSCF) that were loaded from the ISIM.
     * @return an array of  PCSCF strings with one PCSCF per string, or NULL if
     *      not present or not loaded
     */
    //@Override
    CARAPI GetIsimPcscf(
        /* [out] */ ArrayOf<String>** result);

    /**
     * Returns the response of ISIM Authetification through RIL.
     * Returns NULL if the Authentification hasn't been successed or isn't present iphonesubinfo.
     * @return the response of ISIM Authetification, or NULL if not available
     * @deprecated
     * @see getIccSimChallengeResponse
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
     * @param appType ICC application Type (@see com.android.internal.telephony.PhoneConstants#APPTYPE_xxx)
     * @param data authentication challenge data
     * @return challenge response
     */
    CARAPI GetIccSimChallengeResponse(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 appType,
        /* [in] */ const String& data,
        /* [out] */ String* result);

    //@Override
    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    AutoPtr<IPhoneSubInfo> mPhoneSubInfo;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONESUBINFOPROXY_H__
