
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/provider/CalendarContract.h"
#include "elastos/droid/net/Uri.h"

using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

const String CalendarContract::TAG("Calendar");

ECode CalendarContract::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://") + ICalendarContract::AUTHORITY, uri);
}

} //Provider
} //Droid
} //Elastos