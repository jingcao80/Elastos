#include "elastos/droid/provider/CTelephonyMmsSms.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_2(CTelephonyMmsSms, Singleton, ITelephonyMmsSms, IBaseColumns);
CAR_SINGLETON_IMPL(CTelephonyMmsSms);

ECode CTelephonyMmsSms::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::MmsSms::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CTelephonyMmsSms::GetCONTENT_CONVERSATIONS_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::MmsSms::CONTENT_CONVERSATIONS_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CTelephonyMmsSms::GetCONTENT_FILTER_BYPHONE_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::MmsSms::CONTENT_FILTER_BYPHONE_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CTelephonyMmsSms::GetCONTENT_UNDELIVERED_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::MmsSms::CONTENT_UNDELIVERED_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CTelephonyMmsSms::GetCONTENT_DRAFT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::MmsSms::CONTENT_DRAFT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CTelephonyMmsSms::GetCONTENT_LOCKED_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::MmsSms::CONTENT_LOCKED_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CTelephonyMmsSms::GetSEARCH_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::MmsSms::SEARCH_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
