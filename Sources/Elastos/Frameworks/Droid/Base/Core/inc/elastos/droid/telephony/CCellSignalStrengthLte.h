
#ifndef __ELASTOS_DROID_TELEPHONY_CCELLSIGNALSTRENGTHLTE_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLSIGNALSTRENGTHLTE_H__

#include "_Elastos_Droid_Telephony_CCellSignalStrengthLte.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/telephony/CellSignalStrength.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellSignalStrengthLte)
    , public CellSignalStrength
    , public ICellSignalStrengthLte
    , public IParcelable
{
public:
    CCellSignalStrengthLte();

    virtual ~CCellSignalStrengthLte();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 signalStrength,
        /* [in] */ Int32 rsrp,
        /* [in] */ Int32 rsrq,
        /* [in] */ Int32 rssnr,
        /* [in] */ Int32 cqi,
        /* [in] */ Int32 timingAdvance);

    CARAPI constructor(
        /* [in] */ ICellSignalStrengthLte* css);

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
        /* [out] */ ICellSignalStrength** cssReturn);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* res);

    CARAPI Initialize(
        /* [in] */ Int32 lteSignalStrength,
        /* [in] */ Int32 rsrp,
        /* [in] */ Int32 rsrq,
        /* [in] */ Int32 rssnr,
        /* [in] */ Int32 cqi,
        /* [in] */ Int32 timingAdvance);

    CARAPI Initialize(
        /* [in] */ ISignalStrength* ss,
        /* [in] */ Int32 timingAdvance);

    CARAPI Copy(
        /* [out] */ ICellSignalStrengthLte** css);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetSignalStrength(
        /* [out] */ Int32* signalStrength);

    CARAPI SetSignalStrength(
        /* [in] */ Int32 signalStrength);

    CARAPI GetRsrp(
        /* [out] */ Int32* rsrp);

    CARAPI SetRsrp(
        /* [in] */ Int32 rsrp);

    CARAPI GetRsrq(
        /* [out] */ Int32* rsrq);

    CARAPI SetRsrq(
        /* [in] */ Int32 rsrq);

    CARAPI GetRssnr(
        /* [out] */ Int32* rssnr);

    CARAPI SetRssnr(
        /* [in] */ Int32 rssnr);

    CARAPI GetCqi(
        /* [out] */ Int32* cqi);

    CARAPI SetCqi(
        /* [in] */ Int32 cqi);

    CARAPI GetTimingAdvance(
        /* [out] */ Int32* timingAdvance);

    CARAPI SetTimingAdvance(
        /* [in] */ Int32 timingAdvance);

protected:

    CARAPI CopyFrom(
        /* [in] */ ICellSignalStrengthLte* css);

private:

    static CARAPI Log(
        /* [in] */ const String& s);

    static const String TAG;
    static const Boolean DBG;

    Int32 mSignalStrength;
    Int32 mRsrp;
    Int32 mRsrq;
    Int32 mRssnr;
    Int32 mCqi;
    Int32 mTimingAdvance;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCELLSIGNALSTRENGTHLTE_H__
