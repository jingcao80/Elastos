
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CalendarContractAttendees.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Net::Uri;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

String CalendarContractAttendees::ATTENDEES_WHERE = String(ICalendarContractAttendeesColumns::EVENT_ID) + String("=?");

ECode CalendarContractAttendees::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    StringBuilder builder;
    builder += "content://";
    builder += ICalendarContract::AUTHORITY;
    builder += "/attendees";
    String str = builder.ToString();
    return Uri::Parse(str, uri);
}

ECode CalendarContractAttendees::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int64 eventId,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** cursor)
{
    AutoPtr<ArrayOf<String> > attArgs = ArrayOf<String>::Alloc(1);
    (*attArgs)[0] = StringUtils::ToString(eventId);
    AutoPtr<IUri> _uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&_uri))
    return cr->Query(_uri, projection, ATTENDEES_WHERE, attArgs /* selection args */, String(NULL) /* sort order */, cursor);
}

} //Provider
} //Droid
} //Elastos