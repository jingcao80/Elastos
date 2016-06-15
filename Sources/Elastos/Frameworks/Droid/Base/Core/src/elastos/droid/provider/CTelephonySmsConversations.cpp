#include "elastos/droid/provider/CTelephonySmsConversations.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_3(CTelephonySmsConversations, Singleton, ITelephonySmsConversations, IBaseColumns, ITelephonyTextBasedSmsColumns);
CAR_SINGLETON_IMPL(CTelephonySmsConversations);

ECode CTelephonySmsConversations::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Sms::Conversations::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
