#include "elastos/droid/provider/CTelephonySmsOutbox.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_3(CTelephonySmsOutbox, Singleton, ITelephonySmsOutbox, IBaseColumns, ITelephonyTextBasedSmsColumns);
CAR_SINGLETON_IMPL(CTelephonySmsOutbox);

ECode CTelephonySmsOutbox::AddMessage(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [in] */ Boolean deliveryReport,
    /* [in] */ Int64 threadId,
    /* [out] */ IUri** uri)
{
    return Telephony::Sms::Outbox::AddMessage(resolver, address, body, subject, date, deliveryReport, threadId, uri);
}

ECode CTelephonySmsOutbox::AddMessage(
    /* [in] */ Int64 subId,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [in] */ Boolean deliveryReport,
    /* [in] */ Int64 threadId,
    /* [out] */ IUri** uri)
{
    return Telephony::Sms::Outbox::AddMessage(subId, resolver, address, body, subject, date, deliveryReport, threadId, uri);
}

ECode CTelephonySmsOutbox::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Sms::Outbox::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
