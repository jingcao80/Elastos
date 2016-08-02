
#ifndef __ELASTOS_DROID_TELEPHONY_CSIGNALSTRENGTH_H__
#define __ELASTOS_DROID_TELEPHONY_CSIGNALSTRENGTH_H__

#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telephony_CSignalStrength.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Telephony::ISignalStrength;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CSignalStrength)
    , public Object
    , public ISignalStrength
    , public IParcelable
{
public:
    CSignalStrength();

    virtual ~CSignalStrength();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    static CARAPI NewFromBundle(
        /* [in] */ IBundle* m,
        /* [out] */ ISignalStrength** ss);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Boolean gsmFlag);

    CARAPI constructor(
        /* [in] */ Int32 gsmSignalStrength,
        /* [in] */ Int32 gsmBitErrorRate,
        /* [in] */ Int32 cdmaDbm,
        /* [in] */ Int32 cdmaEcio,
        /* [in] */ Int32 evdoDbm,
        /* [in] */ Int32 evdoEcio,
        /* [in] */ Int32 evdoSnr,
        /* [in] */ Int32 lteSignalStrength,
        /* [in] */ Int32 lteRsrp,
        /* [in] */ Int32 lteRsrq,
        /* [in] */ Int32 lteRssnr,
        /* [in] */ Int32 lteCqi,
        /* [in] */ Boolean gsmFlag);

    CARAPI constructor(
        /* [in] */ Int32 gsmSignalStrength,
        /* [in] */ Int32 gsmBitErrorRate,
        /* [in] */ Int32 cdmaDbm,
        /* [in] */ Int32 cdmaEcio,
        /* [in] */ Int32 evdoDbm,
        /* [in] */ Int32 evdoEcio,
        /* [in] */ Int32 evdoSnr,
        /* [in] */ Boolean gsmFlag);

    CARAPI constructor(
        /* [in] */ ISignalStrength* s);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI Initialize(
        /* [in] */ Int32 gsmSignalStrength,
        /* [in] */ Int32 gsmBitErrorRate,
        /* [in] */ Int32 cdmaDbm,
        /* [in] */ Int32 cdmaEcio,
        /* [in] */ Int32 evdoDbm,
        /* [in] */ Int32 evdoEcio,
        /* [in] */ Int32 evdoSnr,
        /* [in] */ Boolean gsm);

    CARAPI Initialize(
        /* [in] */ Int32 gsmSignalStrength,
        /* [in] */ Int32 gsmBitErrorRate,
        /* [in] */ Int32 cdmaDbm,
        /* [in] */ Int32 cdmaEcio,
        /* [in] */ Int32 evdoDbm,
        /* [in] */ Int32 evdoEcio,
        /* [in] */ Int32 evdoSnr,
        /* [in] */ Int32 lteSignalStrength,
        /* [in] */ Int32 lteRsrp,
        /* [in] */ Int32 lteRsrq,
        /* [in] */ Int32 lteRssnr,
        /* [in] */ Int32 lteCqi,
        /* [in] */ Boolean gsm);

    CARAPI ValidateInput();

    CARAPI SetGsm(
        /* [in] */ Boolean gsmFlag);

    CARAPI GetGsmSignalStrength(
        /* [out] */ Int32* gsmSignalStrength);

    CARAPI GetGsmBitErrorRate(
        /* [out] */ Int32* gsmBitErrorRate);

    CARAPI GetCdmaDbm(
        /* [out] */ Int32* cdmaDbm);

    CARAPI GetCdmaEcio(
        /* [out] */ Int32* cdmaEcio);

    CARAPI GetEvdoDbm(
        /* [out] */ Int32* evdoDbm);

    CARAPI GetEvdoEcio(
        /* [out] */ Int32* evdoEcio);

    CARAPI GetEvdoSnr(
        /* [out] */ Int32* evdoSnr);

    CARAPI GetLteSignalStrength(
        /* [out] */ Int32* lteSignalStrength);

    CARAPI GetLteRsrp(
        /* [out] */ Int32* lteRsrp);

    CARAPI GetLteRsrq(
        /* [out] */ Int32* lteRsrq);

    CARAPI GetLteRssnr(
        /* [out] */ Int32* lteRssnr);

    CARAPI GetLteCqi(
        /* [out] */ Int32* lteCqi);

    CARAPI GetLevel(
        /* [out] */ Int32* level);

    CARAPI GetAsuLevel(
        /* [out] */ Int32* asuLevel);

    CARAPI GetDbm(
        /* [out] */ Int32* dbm);

    CARAPI GetGsmDbm(
        /* [out] */ Int32* gsmDbm);

    CARAPI GetGsmLevel(
        /* [out] */ Int32* gsmLevel);

    CARAPI GetGsmAsuLevel(
        /* [out] */ Int32* gsmAsuLevel);

    CARAPI GetCdmaLevel(
        /* [out] */ Int32* cdmaLevel);

    CARAPI GetCdmaAsuLevel(
        /* [out] */ Int32* cdmaAsuLevel);

    CARAPI GetEvdoLevel(
        /* [out] */ Int32* evdoLevel);

    CARAPI GetEvdoAsuLevel(
        /* [out] */ Int32* evdoAsuLevel);

    CARAPI GetLteDbm(
        /* [out] */ Int32* lteDbm);

    CARAPI GetLteLevel(
        /* [out] */ Int32* lteLevel);

    CARAPI GetLteAsuLevel(
        /* [out] */ Int32* lteAsuLevel);

    CARAPI IsGsm(
        /* [out] */ Boolean* res);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI FillInNotifierBundle(
        /* [in] */ Elastos::Droid::Os::IBundle* m);

    static const AutoPtr< ArrayOf<String> > SIGNAL_STRENGTH_NAMES;

    /**
     * Make a SignalStrength object from the given parcel as passed up by
     * the ril which does not have isGsm. isGsm will be changed by ServiceStateTracker
     * so the default is a don't care.
     *
     * @hide
     */
    static CARAPI_(AutoPtr<ISignalStrength>) MakeSignalStrengthFromRilParcel(
        /* [in] */ IParcel* in);

protected:
    CARAPI CopyFrom(
        /* [in] */ ISignalStrength* s);

private:
    CARAPI SetFromNotifierBundle(
        /* [in] */ IBundle* m);

    static const String TAG;
    static const Boolean DBG/* = FALSE*/;

    Int32 mGsmSignalStrength; // Valid values are (0-31, 99) as defined in TS 27.007 8.5
    Int32 mGsmBitErrorRate;   // bit error rate (0-7, 99) as defined in TS 27.007 8.5
    Int32 mCdmaDbm;   // This value is the RSSI value
    Int32 mCdmaEcio;  // This value is the Ec/Io
    Int32 mEvdoDbm;   // This value is the EVDO RSSI value
    Int32 mEvdoEcio;  // This value is the EVDO Ec/Io
    Int32 mEvdoSnr;   // Valid values are 0-8.  8 is the highest signal to noise ratio
    Int32 mLteSignalStrength;
    Int32 mLteRsrp;
    Int32 mLteRsrq;
    Int32 mLteRssnr;
    Int32 mLteCqi;

    Boolean mIsGsm; // This value is set by the ServiceStateTracker onSignalStrengthResult
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CSIGNALSTRENGTH_H__
