#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CCalendarContract.h"

using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CCalendarContract)

CAR_INTERFACE_IMPL(CCalendarContract, Singleton, ICalendarContract)

ECode CCalendarContract::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return CalendarContract::GetCONTENT_URI(uri);
}

} //Provider
} //Droid
} //Elastos