
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/provider/CTelephonyBlacklist.h"
#include <elastos/droid/net/Uri.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Provider::ITelephonyBlacklist;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL(CTelephonyBlacklist, Singleton, ITelephonyBlacklist)

CAR_SINGLETON_IMPL(CTelephonyBlacklist)

AutoPtr<IUri> CTelephonyBlacklist::CONTENT_URI = InitUri(String("content://blacklist"));
AutoPtr<IUri> CTelephonyBlacklist::CONTENT_FILTER_BYNUMBER_URI = InitUri(String("content://blacklist/bynumber"));
AutoPtr<IUri> CTelephonyBlacklist::CONTENT_PHONE_URI = InitUri(String("content://blacklist/phone"));
AutoPtr<IUri> CTelephonyBlacklist::CONTENT_MESSAGE_URI = InitUri(String("content://blacklist/message"));

ECode CTelephonyBlacklist::GetCONTENT_URI(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)

    *result = CONTENT_URI;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTelephonyBlacklist::GetCONTENT_FILTER_BYNUMBER_URI(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)

    *result = CONTENT_FILTER_BYNUMBER_URI;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTelephonyBlacklist::GetCONTENT_PHONE_URI(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)

    *result = CONTENT_PHONE_URI;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTelephonyBlacklist::GetCONTENT_MESSAGE_URI(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)

    *result = CONTENT_MESSAGE_URI;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IUri> CTelephonyBlacklist::InitUri(
    /* [in] */ const String& uriStr)
{
    AutoPtr<IUri> rev;
    Uri::Parse(uriStr, (IUri**)&rev);
    return rev;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
