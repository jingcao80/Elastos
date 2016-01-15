
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CalendarContractExtendedProperties.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Net::Uri;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Provider {

ECode CalendarContractExtendedProperties::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    StringBuilder builder;
    builder += "content://";
    builder += ICalendarContract::AUTHORITY;
    builder += "/extendedproperties";
    String str = builder.ToString();
    return Uri::Parse(str, uri);
}

} //Provider
} //Droid
} //Elastos