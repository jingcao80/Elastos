
#ifndef __ELASTOS_DROID_TELEPHONY_CCELLINFOCDMA_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLINFOCDMA_H__

#include "_Elastos_Droid_Telephony_CCellInfoCdma.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/telephony/CellInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellInfoCdma)
    , public CellInfo
    , public ICellInfoCdma
{
public:
    CCellInfoCdma();

    virtual ~CCellInfoCdma();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICellInfoCdma* ci);

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
    static const String TAG;
    static const Boolean DBG;

    AutoPtr<ICellIdentityCdma> mCellIdentityCdma;
    AutoPtr<ICellSignalStrengthCdma> mCellSignalStrengthCdma;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCELLINFOCDMA_H__
