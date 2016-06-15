#include "elastos/droid/provider/CTelephonySmsInbox.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_3(CTelephonySmsInbox, Singleton, ITelephonySmsInbox, IBaseColumns, ITelephonyTextBasedSmsColumns);
CAR_SINGLETON_IMPL(CTelephonySmsInbox);

ECode CTelephonySmsInbox::AddMessage(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [in] */ Boolean read,
    /* [out] */ IUri** uri)
{
    return Telephony::Sms::Inbox::AddMessage(resolver, address, body, subject, date, read, uri);
}

ECode CTelephonySmsInbox::AddMessage(
    /* [in] */ Int64 subId,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [in] */ Boolean read,
    /* [out] */ IUri** uri)
{
    return Telephony::Sms::Inbox::AddMessage(subId, resolver, address, body, subject, date, read, uri);
}

ECode CTelephonySmsInbox::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Sms::Inbox::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
