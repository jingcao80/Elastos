#include "elastos/droid/provider/CCalendarContractColors.h"
#include "elastos/droid/net/Uri.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Net::Uri;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CCalendarContractColors)

CAR_INTERFACE_IMPL(CCalendarContractColors, Singleton, ICalendarContractColors)

ECode CCalendarContractColors::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    StringBuilder builder;
    builder += "content://";
    builder += ICalendarContract::AUTHORITY;
    builder += "/colors";
    String str = builder.ToString();
    return Uri::Parse(str, uri);
}

} //Provider
} //Droid
} //Elastos