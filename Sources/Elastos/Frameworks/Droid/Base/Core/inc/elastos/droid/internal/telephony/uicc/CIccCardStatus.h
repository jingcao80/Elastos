#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CICCCARDSTATUS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CICCCARDSTATUS_H__

#include "_Elastos_Droid_Internal_Telephony_Uicc_CIccCardStatus.h"
#include "elastos/droid/internal/telephony/uicc/IccCardStatus.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CIccCardStatus)
    , public IccCardStatus
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CICCCARDSTATUS_H__
