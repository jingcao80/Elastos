#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CSPNOVERRIDE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CSPNOVERRIDE_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/SpnOverride.h"
#include "_Elastos_Droid_Internal_Telephony_Uicc_CSpnOverride.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CSpnOverride)
    , public SpnOverride
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CSPNOVERRIDE_H__
