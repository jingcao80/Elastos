#include "elastos/droid/provider/CTelephonySmsDraft.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_3(CTelephonySmsDraft, Singleton, ITelephonySmsDraft, IBaseColumns, ITelephonyTextBasedSmsColumns);
CAR_SINGLETON_IMPL(CTelephonySmsDraft);

ECode CTelephonySmsDraft::AddMessage(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [out] */ IUri** uri)
{
    return Telephony::Sms::Draft::AddMessage(resolver, address, body, subject, date, uri);
}

ECode CTelephonySmsDraft::AddMessage(
    /* [in] */ Int64 subId,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& address,
    /* [in] */ const String& body,
    /* [in] */ const String& subject,
    /* [in] */ Int64 date,
    /* [out] */ IUri** uri)
{
    return Telephony::Sms::Draft::AddMessage(subId, resolver, address, body, subject, date, uri);
}

ECode CTelephonySmsDraft::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Sms::Draft::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
