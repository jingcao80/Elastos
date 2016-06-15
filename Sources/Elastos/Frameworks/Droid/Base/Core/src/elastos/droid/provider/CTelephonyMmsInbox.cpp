#include "elastos/droid/provider/CTelephonyMmsInbox.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_2(CTelephonyMmsInbox, Singleton, ITelephonyMmsInbox, ITelephonyBaseMmsColumns);
CAR_SINGLETON_IMPL(CTelephonyMmsInbox);

ECode CTelephonyMmsInbox::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Mms::Inbox::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
