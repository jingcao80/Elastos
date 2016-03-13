
#ifndef __ELASTOS_DROID_TELEPHONY_CCELLINFOGSM_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLINFOGSM_H__

#include "_Elastos_Droid_Telephony_CCellInfoGsm.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/telephony/CellInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellInfoGsm)
    , public CellInfo
    , public ICellInfoGsm
{
public:
    CCellInfoGsm();

    virtual ~CCellInfoGsm();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICellInfoGsm* ci);

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

    static const String TAG;
    static const Boolean DBG;

    AutoPtr<ICellIdentityGsm> mCellIdentityGsm;
    AutoPtr<ICellSignalStrengthGsm> mCellSignalStrengthGsm;
};


} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCELLINFOGSM_H__
