#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CalendarContractReminders.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Net::Uri;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

const String CalendarContractReminders::REMINDERS_WHERE = ICalendarContractRemindersColumns::EVENT_ID + String("=?");

ECode CalendarContractReminders::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    StringBuilder builder;
    builder += "content://";
    builder += ICalendarContract::AUTHORITY;
    builder += "/reminders";
    String str = builder.ToString();
    return Uri::Parse(str, uri);
}

ECode CalendarContractReminders::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int64 eventId,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);

    AutoPtr<ArrayOf<String> > remArgs = ArrayOf<String>::Alloc(1);
    (*remArgs)[0] = StringUtils::ToString(eventId);
    AutoPtr<IUri> uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&uri))
    return cr->Query(uri, projection, REMINDERS_WHERE, remArgs /*selection args*/, String(NULL) /* sort order */, cursor);
}

} //Provider
} //Droid
} //Elastos