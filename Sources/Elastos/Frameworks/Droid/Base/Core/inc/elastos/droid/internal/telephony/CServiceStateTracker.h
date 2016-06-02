
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSERVICESTATETRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSERVICESTATETRACKER_H__

#include "_Elastos_Droid_Internal_Telephony_CServiceStateTracker.h"
#include "elastos/droid/internal/telephony/ServiceStateTracker.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CServiceStateTracker), public ServiceStateTracker
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSERVICESTATETRACKER_H__
