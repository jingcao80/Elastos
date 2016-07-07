
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CRETRYMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CRETRYMANAGER_H__

#include "_Elastos_Droid_Internal_Telephony_CRetryManager.h"
#include "elastos/droid/internal/telephony/RetryManager.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CRetryManager)
    , public RetryManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CRETRYMANAGER_H__
