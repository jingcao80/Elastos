#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCARRIERPRIVILEGERULES_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCARRIERPRIVILEGERULES_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Internal_Telephony_Uicc_CUiccCarrierPrivilegeRules.h"
#include "elastos/droid/internal/telephony/uicc/UiccCarrierPrivilegeRules.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CUiccCarrierPrivilegeRules)
    , public UiccCarrierPrivilegeRules
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCARRIERPRIVILEGERULES_H__
