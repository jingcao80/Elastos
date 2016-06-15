#include "elastos/droid/provider/CTelephonyMmsOutbox.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_2(CTelephonyMmsOutbox, Singleton, ITelephonyMmsOutbox, ITelephonyBaseMmsColumns);
CAR_SINGLETON_IMPL(CTelephonyMmsOutbox);

ECode CTelephonyMmsOutbox::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Mms::Outbox::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
