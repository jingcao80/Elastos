#include "elastos/droid/provider/CTelephonyMmsSent.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_2(CTelephonyMmsSent, Singleton, ITelephonyMmsSent, ITelephonyBaseMmsColumns);
CAR_SINGLETON_IMPL(CTelephonyMmsSent);

ECode CTelephonyMmsSent::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Mms::Sent::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
