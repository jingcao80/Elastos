
#ifndef __ELASTOS_DROID_TELEPHONY_CSERVICESTATE_H__
#define __ELASTOS_DROID_TELEPHONY_CSERVICESTATE_H__

#include "_Elastos_Droid_Telephony_CServiceState.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CServiceState)
{
public:

    static CARAPI NewFromBundle(
        /* [in] */ IBundle* m,
        /* [out] */ IServiceState** res);

    static CARAPI_(String) RilRadioTechnologyToString(
        /* [in] */ Int32 rt);

    static CARAPI_(Boolean) IsGsm(
        /* [in] */ Int32 radioTechnology);

    static CARAPI_(Boolean) IsCdma(
        /* [in] */ Int32  radioTechnology);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IServiceState* s);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetState(
        /* [out] */ Int32* state);

    CARAPI GetRoaming(
        /* [out] */ Boolean* roaming);

    CARAPI IsEmergencyOnly(
        /* [out] */ Boolean* res);

    CARAPI GetCdmaRoamingIndicator(
        /* [out] */ Int32* cdmaRoamingIndicator);

    CARAPI GetCdmaDefaultRoamingIndicator(
        /* [out] */ Int32* cdmaDefaultRoamingIndicator);

    CARAPI GetCdmaEriIconIndex(
        /* [out] */ Int32* cdmaEriIconIndex);

    CARAPI GetCdmaEriIconMode(
        /* [out] */ Int32* cdmaEriIconMode);

    CARAPI GetOperatorAlphaLong(
        /* [out] */ String* operatorAlphaLong);

    CARAPI GetOperatorAlphaShort(
        /* [out] */ String* operatorAlphaShort);

    CARAPI GetOperatorNumeric(
        /* [out] */ String* operatorNumeric);

    CARAPI GetIsManualSelection(
        /* [out] */ Boolean* isManualSelection);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetStateOutOfService();

    CARAPI SetStateOff();

    CARAPI SetState(
        /* [in] */ Int32 state);

    CARAPI SetRoaming(
        /* [in] */ Boolean roaming);

    CARAPI SetEmergencyOnly(
        /* [in] */ Boolean emergencyOnly);

    CARAPI SetCdmaRoamingIndicator(
        /* [in] */ Int32 roaming);

    CARAPI SetCdmaDefaultRoamingIndicator(
        /* [in] */ Int32 roaming);

    CARAPI SetCdmaEriIconIndex(
        /* [in] */ Int32 index);

    CARAPI SetCdmaEriIconMode(
        /* [in] */ Int32 mode);

    CARAPI SetOperatorName(
        /* [in] */ const String& longName,
        /* [in] */ const String& shortName,
        /* [in] */ const String& numeric);

    CARAPI SetOperatorAlphaLong(
        /* [in] */ const String& longName);

    CARAPI SetIsManualSelection(
        /* [in] */ Boolean isManual);

    CARAPI FillInNotifierBundle(
        /* [in] */ IBundle* m);

    CARAPI SetRadioTechnology(
        /* [in] */ Int32 state);

    CARAPI SetCssIndicator(
        /* [in] */ Int32 css);

    CARAPI SetSystemAndNetworkId(
        /* [in] */ Int32 systemId,
        /* [in] */ Int32 networkId);

    CARAPI GetRilRadioTechnology(
        /* [out] */ Int32* rilRadioTechnology);

    CARAPI GetRadioTechnology(
        /* [out] */ Int32* radioTechnology);

    CARAPI GetNetworkType(
        /* [out] */ Int32* networkType);

    CARAPI GetCssIndicator(
        /* [out] */ Int32* cssIndicator);

    CARAPI GetNetworkId(
        /* [out] */ Int32* networkId);

    CARAPI GetSystemId(
        /* [out] */ Int32* systemId);

protected:
    CARAPI CopyFrom(
        /* [in] */ IServiceState* s);

private:

    static CARAPI_(Boolean) EqualsHandlesNulls(
        /* [in] */ const String& a,
        /* [in] */ const String& b);

    CARAPI SetNullState(
        /* [in] */ Int32 state);

    CARAPI SetFromNotifierBundle(
        /* [in] */ IBundle* m);

    static const String LOG_TAG/* = "PHONE"*/;
    Int32 mState/*STATE_OUT_OF_SERVICE*/;
    Boolean mRoaming;
    String mOperatorAlphaLong;
    String mOperatorAlphaShort;
    String mOperatorNumeric;
    Boolean mIsManualNetworkSelection;

    Boolean mIsEmergencyOnly;

    //***** CDMA
    Int32 mRadioTechnology;
    Boolean mCssIndicator;
    Int32 mNetworkId;
    Int32 mSystemId;
    Int32 mCdmaRoamingIndicator;
    Int32 mCdmaDefaultRoamingIndicator;
    Int32 mCdmaEriIconIndex;
    Int32 mCdmaEriIconMode;
};

}
}
}

#endif // __ELASTOS_DROID_TELEPHONY_CSERVICESTATE_H__
