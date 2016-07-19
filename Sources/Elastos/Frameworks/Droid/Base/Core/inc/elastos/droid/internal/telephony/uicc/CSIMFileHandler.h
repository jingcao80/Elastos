#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CSIMFILEHANDLER_HH__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CSIMFILEHANDLER_HH__

#include "_Elastos_Droid_Internal_Telephony_Uicc_CSIMFileHandler.h"
#include "elastos/droid/internal/telephony/uicc/SIMFileHandler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CSIMFileHandler)
    , public SIMFileHandler
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CSIMFILEHANDLER_HH__
