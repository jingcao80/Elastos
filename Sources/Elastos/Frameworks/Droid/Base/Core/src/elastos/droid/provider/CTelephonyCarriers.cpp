#include "elastos/droid/provider/CTelephonyCarriers.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_2(CTelephonyCarriers, Singleton, ITelephonyCarriers, IBaseColumns);
CAR_SINGLETON_IMPL(CTelephonyCarriers);

ECode CTelephonyCarriers::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Carriers::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
