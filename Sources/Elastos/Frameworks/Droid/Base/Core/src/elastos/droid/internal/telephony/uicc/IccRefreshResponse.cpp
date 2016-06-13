
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccRefreshResponse.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                          IccRefreshResponse
//=====================================================================
CAR_INTERFACE_IMPL(IccRefreshResponse, Object, IIccRefreshResponse);

ECode IccRefreshResponse::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "{" + refreshResult + ", " + aid +", " + efId + "}";
    assert(0);
    return NOERROR;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
