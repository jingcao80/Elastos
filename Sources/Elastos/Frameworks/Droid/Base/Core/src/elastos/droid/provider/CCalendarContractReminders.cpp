#include "elastos/droid/provider/CalendarContractReminders.h"
#include "elastos/droid/provider/CCalendarContractReminders.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CCalendarContractReminders)

CAR_INTERFACE_IMPL_4(CCalendarContractReminders, Singleton
    , ICalendarContractReminders
    , IBaseColumns
    , ICalendarContractRemindersColumns
    , ICalendarContractEventsColumns)

ECode CCalendarContractReminders::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return CalendarContractReminders::GetCONTENT_URI(uri);
}

ECode CCalendarContractReminders::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int64 eventId,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);

    return CalendarContractReminders::Query(cr, eventId, projection, cursor);
}

} //Provider
} //Droid
} //Elastos