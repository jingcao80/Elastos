#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CCalendarContractSyncState.h"
#include "elastos/droid/provider/CalendarContract.h"

using Elastos::Droid::Net::Uri;
using Elastos::Droid::Provider::ISyncStateContractConstants;

namespace Elastos {
namespace Droid {
namespace Provider {

const String CCalendarContractSyncState::CONTENT_DIRECTORY = ISyncStateContractConstants::CONTENT_DIRECTORY;

CAR_SINGLETON_IMPL(CCalendarContractSyncState)

CAR_INTERFACE_IMPL(CCalendarContractSyncState, Singleton, ICalendarContractSyncState)

ECode CCalendarContractSyncState::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> _uri;
    FAIL_RETURN(CalendarContract::GetCONTENT_URI((IUri**)&_uri))
    return Uri::WithAppendedPath(_uri, CONTENT_DIRECTORY, uri);
}

} //Provider
} //Droid
} //Elastos