
#ifndef __ELASTOS_DROID_TELEPHONY_CCELLINFOWCDMA_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLINFOWCDMA_H__

#include "_Elastos_Droid_Telephony_CCellInfoWcdma.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/telephony/CellInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellInfoWcdma)
    , public CellInfo
    , public ICellInfoWcdma
{
public:
    CCellInfoWcdma();

    virtual ~CCellInfoWcdma();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICellInfoWcdma* ci);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetCellIdentity(
        /* [out] */ ICellIdentityWcdma** cid);

    CARAPI SetCellIdentity(
        /* [in] */ ICellIdentityWcdma* cid);

    CARAPI GetCellSignalStrength(
        /* [out] */ ICellSignalStrengthWcdma** css);

    CARAPI SetCellSignalStrength(
        /* [in] */ ICellSignalStrengthWcdma* css);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:

    static const String TAG;
    static const Boolean DBG;

    AutoPtr<ICellIdentityWcdma> mCellIdentityWcdma;
    AutoPtr<ICellSignalStrengthWcdma> mCellSignalStrengthWcdma;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCELLINFOWCDMA_H__
