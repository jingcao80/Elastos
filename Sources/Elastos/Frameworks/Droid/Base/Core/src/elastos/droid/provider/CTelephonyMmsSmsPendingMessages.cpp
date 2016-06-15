#include "elastos/droid/provider/CTelephonyMmsSmsPendingMessages.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_2(CTelephonyMmsSmsPendingMessages, Singleton, ITelephonyMmsSmsPendingMessages, IBaseColumns);
CAR_SINGLETON_IMPL(CTelephonyMmsSmsPendingMessages);

ECode CTelephonyMmsSmsPendingMessages::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::MmsSms::PendingMessages::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
