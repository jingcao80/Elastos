
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CTELEPHONYDEVCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CTELEPHONYDEVCONTROLLER_H__

#include "_Elastos_Droid_Internal_Telephony_CTelephonyDevController.h"
#include "elastos/droid/internal/telephony/TelephonyDevController.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CTelephonyDevController)
    , public TelephonyDevController
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CTELEPHONYDEVCONTROLLER_H__
