
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONDATA_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONDATA_H__

#include "_Elastos_Droid_Internal_Telephony_CSubscriptionData.h"
#include "elastos/droid/internal/telephony/SubscriptionData.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSubscriptionData), public SubscriptionData
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBSCRIPTIONDATA_H__
