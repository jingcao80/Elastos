#include "elastos/droid/internal/telephony/cdma/sms/CCdmaSmsAddressHelper.h"
#include "elastos/droid/internal/telephony/cdma/sms/CdmaSmsAddress.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

CAR_INTERFACE_IMPL(CCdmaSmsAddressHelper, Singleton, ICdmaSmsAddressHelper)
CAR_SINGLETON_IMPL(CCdmaSmsAddressHelper)

ECode CCdmaSmsAddressHelper::Parse(
    /* [in] */ const String& address,
    /* [out] */ ICdmaSmsAddress** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICdmaSmsAddress> csa = CdmaSmsAddress::Parse(address);
    *result = csa;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
