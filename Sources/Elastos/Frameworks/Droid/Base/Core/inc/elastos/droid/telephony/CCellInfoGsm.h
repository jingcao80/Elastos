
#ifndef __ELASTOS_DROID_TELEPHONY_CCELLINFOGSM_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLINFOGSM_H__

#include "_Elastos_Droid_Telephony_CCellInfoGsm.h"
#include "telephony/CellInfo.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellInfoGsm), public CellInfo
{
public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICellInfoGsm* ci);

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI IsRegistered(
        /* [out] */ Boolean* registered);

    CARAPI SetRegisterd(
        /* [in] */ Boolean registered);

    CARAPI GetTimeStamp(
        /* [out] */ Int64* timeStamp);

    CARAPI SetTimeStamp(
        /* [in] */ Int64 timeStamp);

    CARAPI GetTimeStampType(
        /* [out] */ Int32* timeStampType);

    CARAPI SetTimeStampType(
        /* [in] */ Int32 timeStampType);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetCellIdentity(
        /* [out] */ ICellIdentityGsm** cid);

    CARAPI SetCellIdentity(
        /* [in] */ ICellIdentityGsm* cid);

    CARAPI GetCellSignalStrength(
        /* [out] */ ICellSignalStrengthGsm** css);

    CARAPI SetCellSignalStrength(
        /* [in] */ ICellSignalStrengthGsm* css);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:

    static const String LOG_TAG;
    static const Boolean DBG;

    AutoPtr<ICellIdentityGsm> mCellIdentityGsm;
    AutoPtr<ICellSignalStrengthGsm> mCellSignalStrengthGsm;
};


} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCELLINFOGSM_H__
