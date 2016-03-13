
#ifndef __ELASTOS_DROID_TELEPHONY_CCELLIDENTITYLTE_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLIDENTITYLTE_H__

#include "_Elastos_Droid_Telephony_CCellIdentityLte.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellIdentityLte)
    , public Object
    , public ICellIdentityLte
    , public IParcelable
{
public:
    CCellIdentityLte();

    virtual ~CCellIdentityLte();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 mcc,
        /* [in] */ Int32 mnc,
        /* [in] */ Int32 ci,
        /* [in] */ Int32 pci,
        /* [in] */ Int32 tac);

    CARAPI constructor(
        /* [in] */ ICellIdentityLte* cid);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI Copy(
        /* [out] */ ICellIdentityLte** cid);

    CARAPI GetMcc(
        /* [out] */ Int32* mcc);

    CARAPI GetMnc(
        /* [out] */ Int32* mnc);

    CARAPI GetCi(
        /* [out] */ Int32* ci);

    CARAPI GetPci(
        /* [out] */ Int32* pci);

    CARAPI GetTac(
        /* [out] */ Int32* tac);

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
    // 28-bit cell identity
    Int32 mCi;
    // physical cell id 0..503
    Int32 mPci;
    // 16-bit tracking area code
    Int32 mTac;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCELLIDENTITYLTE_H__
