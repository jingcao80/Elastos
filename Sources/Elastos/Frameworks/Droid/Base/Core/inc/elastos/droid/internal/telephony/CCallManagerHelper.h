#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLMANAGERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLMANAGERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CCallManagerHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CCallManagerHelper)
    , public Singleton
    , public ICallManagerHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ ICallManager** result);

    CARAPI IsSamePhone(
        /* [in] */ IPhone* p1,
        /* [in] */ IPhone* p2,
        /* [out] */ Boolean* result);
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLMANAGERHELPER_H__
