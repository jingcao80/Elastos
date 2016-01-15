
#ifndef __ELASTOS_DROID_TELEPHONY_CCELLINFOCDMA_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLINFOCDMA_H__

#include "_Elastos_Droid_Telephony_CCellInfoCdma.h"
#include "telephony/CellInfo.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellInfoCdma), public CellInfo
{
public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICellInfoCdma* ci);

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

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
        /* [out] */ ICellIdentityCdma** cid);

    CARAPI SetCellIdentity(
        /* [in] */ ICellIdentityCdma* cid);

    CARAPI GetCellSignalStrength(
        /* [out] */ ICellSignalStrengthCdma** css);

    CARAPI SetCellSignalStrength(
        /* [in] */ ICellSignalStrengthCdma* css);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    static const String LOG_TAG;
    static const Boolean DBG;

    AutoPtr<ICellIdentityCdma> mCellIdentityCdma;
    AutoPtr<ICellSignalStrengthCdma> mCellSignalStrengthCdma;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCELLINFOCDMA_H__
