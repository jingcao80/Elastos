
#ifndef __ELASTOS_DROID_TELEPHONY_CCELLIDENTITYGSM_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLIDENTITYGSM_H__

#include "_Elastos_Droid_Telephony_CCellIdentityGsm.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellIdentityGsm)
    , public Object
    , public ICellIdentityGsm
    , public IParcelable
{
public:
    CCellIdentityGsm();

    virtual ~CCellIdentityGsm();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 mcc,
        /* [in] */ Int32 mnc,
        /* [in] */ Int32 lac,
        /* [in] */ Int32 cid);

    CARAPI constructor(
        /* [in] */ ICellIdentityGsm* cid);

    CARAPI Copy(
        /* [out] */ ICellIdentityGsm** cid);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetMcc(
        /* [out] */ Int32* mcc);

    CARAPI GetMnc(
        /* [out] */ Int32* mnc);

    CARAPI GetLac(
        /* [out] */ Int32* lac);

    CARAPI GetCid(
        /* [out] */ Int32* cid);

    CARAPI GetPsc(
        /* [out] */ Int32* psc);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* str);

private:

    static const String TAG;
    static const Boolean DBG = FALSE;

    // 3-digit Mobile Country Code, 0..999
    Int32 mMcc;
    // 2 or 3-digit Mobile Network Code, 0..999
    Int32 mMnc;
    // 16-bit Location Area Code, 0..65535
    Int32 mLac;
    // 16-bit GSM Cell Identity described in TS 27.007, 0..65535
    // 28-bit UMTS Cell Identity described in TS 25.331, 0..268435455
    Int32 mCid;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCELLIDENTITYGSM_H__
