#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CRIL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CRIL_H__

#include "_Elastos_Droid_Internal_Telephony_CRIL.h"
#include "elastos/droid/internal/telephony/RIL.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CRIL)
    , public RIL
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CRIL_H__
