
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccRefreshResponse.h"

#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

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
    *result = String("{") + StringUtils::ToString(mRefreshResult)
            + String(", ") + mAid + String(", ")
            + StringUtils::ToString(mEfId) + String("}");
    return NOERROR;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
