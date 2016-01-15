
#ifndef __ELASTOS_DROID_TELEPHONY_CCELLSIGNALSTRENGTHCDMA_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLSIGNALSTRENGTHCDMA_H__

#include "_Elastos_Droid_Telephony_CCellSignalStrengthCdma.h"
#include "CellSignalStrength.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellSignalStrengthCdma), CellSignalStrength
{
public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 cdmaDbm,
        /* [in] */ Int32 cdmaEcio,
        /* [in] */ Int32 evdoDbm,
        /* [in] */ Int32 evdoEcio,
        /* [in] */ Int32 evdoSnr);

    CARAPI constructor(
        /* [in] */ ICellSignalStrengthCdma* css);

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI SetDefaultValues();

    CARAPI GetLevel(
        /* [out] */ Int32* level);

    CARAPI GetAsuLevel(
        /* [out] */ Int32* asuLevel);

    CARAPI GetDbm(
        /* [out] */ Int32* dbm);

    CARAPI Copy(
        /* [out] */ ICellSignalStrength** css);

    CARAPI Copy(
        /* [out] */ ICellSignalStrengthCdma** css);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* res);

    CARAPI Initialize(
        /* [in] */ Int32 cdmaDbm,
        /* [in] */ Int32 cdmaEcio,
        /* [in] */ Int32 evdoDbm,
        /* [in] */ Int32 evdoEcio,
        /* [in] */ Int32 evdoSnr);

    CARAPI GetCdmaLevel(
        /* [out] */ Int32* cdmaLevel);

    CARAPI GetEvdoLevel(
        /* [out] */ Int32* evdoLevel);

    CARAPI GetCdmaDbm(
        /* [out] */ Int32* cdmaDbm);

    CARAPI SetCdmaDbm(
        /* [in] */ Int32 cdmaDbm);

    CARAPI GetCdmaEcio(
        /* [out] */ Int32* cdmaEcio);

    CARAPI SetCdmaEcio(
        /* [in] */ Int32 cdmaEcio);

    CARAPI GetEvdoDbm(
        /* [out] */ Int32* evdoDbm);

    CARAPI SetEvdoDbm(
        /* [in] */ Int32 evdoDbm);

    CARAPI GetEvdoEcio(
        /* [out] */ Int32* evdoEcio);

    CARAPI SetEvdoEcio(
        /* [in] */ Int32 evdoEcio);

    CARAPI GetEvdoSnr(
        /* [out] */ Int32* evdoSnr);

    CARAPI SetEvdoSnr(
        /* [in] */ Int32 evdoSnr);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    CARAPI CopyFrom(
        /* [in] */ ICellSignalStrengthCdma* css);

private:

    static CARAPI Log(
        /* [in] */ const String& s);

    static const String LOG_TAG/* = "CellSignalStrengthCdma"*/;
    static const Boolean DBG/* = false*/;

    Int32 mCdmaDbm;   // This value is the RSSI value
    Int32 mCdmaEcio;  // This value is the Ec/Io
    Int32 mEvdoDbm;   // This value is the EVDO RSSI value
    Int32 mEvdoEcio;  // This value is the EVDO Ec/Io
    Int32 mEvdoSnr;   // Valid values are 0-8.  8 is the highest signal to noise ratio
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCELLSIGNALSTRENGTHCDMA_H__
