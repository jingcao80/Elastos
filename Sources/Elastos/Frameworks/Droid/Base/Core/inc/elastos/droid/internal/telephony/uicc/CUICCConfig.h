#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCONFIG_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCONFIG_H__

#include "_Elastos_Droid_Internal_Telephony_Uicc_CUICCConfig.h"
#include "elastos/droid/internal/telephony/uicc/UICCConfig.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CUICCConfig)
    , public UICCConfig
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCONFIG_H__
