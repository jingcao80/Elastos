
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CUICCPHONEBOOKCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CUICCPHONEBOOKCONTROLLER_H__

#include "_Elastos_Droid_Internal_Telephony_CUiccPhoneBookController.h"
#include "elastos/droid/internal/telephony/UiccPhoneBookController.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CUiccPhoneBookController)
    , public UiccPhoneBookController
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CUICCPHONEBOOKCONTROLLER_H__
