#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CCalendarContractCalendarCache.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Net::Uri;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CCalendarContractCalendarCache)

CAR_INTERFACE_IMPL(CCalendarContractCalendarCache, Singleton, ICalendarContractCalendarCache)

ECode CCalendarContractCalendarCache::GetURI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    StringBuilder builder;
    builder += "content://";
    builder += ICalendarContract::AUTHORITY;
    builder += "/properties";
    String str = builder.ToString();
    return Uri::Parse(str, uri);
}

} //Provider
} //Droid
} //Elastos