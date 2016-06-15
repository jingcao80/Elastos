#include "elastos/droid/provider/CTelephonyBlacklist.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_2(CTelephonyBlacklist, Singleton, ITelephonyBlacklist, IBaseColumns);
CAR_SINGLETON_IMPL(CTelephonyBlacklist);

ECode CTelephonyBlacklist::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Blacklist::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CTelephonyBlacklist::GetCONTENT_FILTER_BYNUMBER_URI(
     /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = Telephony::Blacklist::CONTENT_FILTER_BYNUMBER_URI;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTelephonyBlacklist::GetCONTENT_PHONE_URI(
     /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = Telephony::Blacklist::CONTENT_PHONE_URI;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTelephonyBlacklist::GetCONTENT_MESSAGE_URI(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = Telephony::Blacklist::CONTENT_MESSAGE_URI;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
