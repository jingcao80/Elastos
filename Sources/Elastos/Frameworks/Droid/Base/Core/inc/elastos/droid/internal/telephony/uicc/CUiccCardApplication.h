#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCARDAPPLICATION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCARDAPPLICATION_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Internal_Telephony_Uicc_CUiccCardApplication.h"
#include "elastos/droid/internal/telephony/uicc/UiccCardApplication.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CUiccCardApplication)
    , public UiccCardApplication
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCARDAPPLICATION_H__
