#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCARD_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCARD_H__

#include "_Elastos_Droid_Internal_Telephony_Uicc_CUiccCard.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/internal/telephony/uicc/UiccCard.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CUiccCard)
    , public UiccCard
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCARD_H__
