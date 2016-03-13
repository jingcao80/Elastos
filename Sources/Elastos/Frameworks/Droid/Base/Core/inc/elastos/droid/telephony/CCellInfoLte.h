
#ifndef __ELASTOS_DROID_TELEPHONY_CCELLINFOLTE_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLINFOLTE_H__

#include "_Elastos_Droid_Telephony_CCellInfoLte.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/telephony/CellInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellInfoLte)
    , public CellInfo
    , public ICellInfoLte
{
public:
    CCellInfoLte();

    virtual ~CCellInfoLte();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICellInfoLte* ci);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetCellIdentity(
        /* [out] */ ICellIdentityLte** cid);

    CARAPI SetCellIdentity(
        /* [in] */ ICellIdentityLte* cid);

    CARAPI GetCellSignalStrength(
        /* [out] */ ICellSignalStrengthLte** css);

    CARAPI SetCellSignalStrength(
        /* [in] */ ICellSignalStrengthLte* css);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:

    static const String TAG;
    static const Boolean DBG;

    AutoPtr<ICellIdentityLte> mCellIdentityLte;
    AutoPtr<ICellSignalStrengthLte> mCellSignalStrengthLte;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCELLINFOLTE_H__
