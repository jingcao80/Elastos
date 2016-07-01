
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONCONTROLLER_H__

#include "_Elastos_Droid_Internal_Telephony_CSubscriptionController.h"
#include "elastos/droid/internal/telephony/SubscriptionController.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSubscriptionController)
    , public SubscriptionController
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONCONTROLLER_H__
