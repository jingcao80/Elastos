#include "elastos/droid/provider/CTelephonySmsSent.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_3(CTelephonySmsSent, Singleton, ITelephonySmsSent, IBaseColumns, ITelephonyTextBasedSmsColumns);
CAR_SINGLETON_IMPL(CTelephonySmsSent);

ECode CTelephonySmsSent::AddMessage(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [out] */ IUri** uri)
{
    return Telephony::Sms::Sent::AddMessage(resolver, address, body, subject, date, uri);
}

ECode CTelephonySmsSent::AddMessage(
    /* [in] */ Int64 subId,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [out] */ IUri** uri)
{
    return Telephony::Sms::Sent::AddMessage(subId, resolver, address, body, subject, date, uri);
}

ECode CTelephonySmsSent::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Sms::Sent::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
