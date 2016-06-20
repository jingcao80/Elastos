#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CICCREFRESHRESPONSE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CICCREFRESHRESPONSE_H__

#include "_Elastos_Droid_Internal_Telephony_Uicc_CIccRefreshResponse.h"
#include "elastos/droid/internal/telephony/uicc/IccRefreshResponse.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CIccRefreshResponse)
    , public IccRefreshResponse
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CICCREFRESHRESPONSE_H__
