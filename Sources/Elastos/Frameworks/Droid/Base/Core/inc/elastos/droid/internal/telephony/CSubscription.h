
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTION_H__

#include "_Elastos_Droid_Internal_Telephony_CSubscription.h"
#include "elastos/droid/internal/telephony/Subscription.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSubscription), public Subscription
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTION_H__
