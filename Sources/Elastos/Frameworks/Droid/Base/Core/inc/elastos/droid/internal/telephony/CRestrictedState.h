
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CRESTRICTEDSTATE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CRESTRICTEDSTATE_H__

#include "_Elastos_Droid_Internal_Telephony_CRestrictedState.h"
#include "elastos/droid/internal/telephony/RestrictedState.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CRestrictedState)
    , public RestrictedState
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CRESTRICTEDSTATE_H__
