#include "elastos/droid/internal/telephony/cdma/CCdmaConnectionHelper.h"
#include "elastos/droid/internal/telephony/cdma/CdmaConnection.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CAR_INTERFACE_IMPL(CCdmaConnectionHelper, Singleton, ICdmaConnectionHelper)
CAR_SINGLETON_IMPL(CCdmaConnectionHelper)


ECode CCdmaConnectionHelper::FormatDialString(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CdmaConnection::FormatDialString(phoneNumber);
    return NOERROR;
}


} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
