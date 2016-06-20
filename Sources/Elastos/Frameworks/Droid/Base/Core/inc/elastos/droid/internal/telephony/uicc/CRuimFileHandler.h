#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CRUIMFILEHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CRUIMFILEHANDLER_H__

#include "_Elastos_Droid_Internal_Telephony_Uicc_CRuimFileHandler.h"
#include "elastos/droid/internal/telephony/uicc/RuimFileHandler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CRuimFileHandler)
    , public RuimFileHandler
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CRUIMFILEHANDLER_H__
