
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CUICCSMSCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CUICCSMSCONTROLLER_H__

#include "_Elastos_Droid_Internal_Telephony_CUiccSmsController.h"
#include "elastos/droid/internal/telephony/UiccSmsController.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CUiccSmsController)
    , public UiccSmsController
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CUICCSMSCONTROLLER_H__
