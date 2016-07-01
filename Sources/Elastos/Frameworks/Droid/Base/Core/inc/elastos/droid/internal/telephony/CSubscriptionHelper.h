
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CSubscriptionHelper.h"
#include "elastos/droid/internal/telephony/SubscriptionHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSubscriptionHelper)
    , public SubscriptionHelper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONHELPER_H__
