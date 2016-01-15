#ifndef __ELASTOS_DROID_PROVIDER_CALENDARCONTRACTREMENDERS_H__
#define __ELASTOS_DROID_PROVIDER_CALENDARCONTRACTREMENDERS_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

/**
 * Fields and helpers for accessing reminders for an event. Each row of this
 * table represents a single reminder for an event. Calling
 * {@link #query(ContentResolver, long, String[])} will return a list of reminders for
 * the event with the given eventId. Both apps and sync adapters may write
 * to this table. There are three writable fields and all of them must be
 * included when inserting a new reminder. They are:
 * <ul>
 * <li>{@link #EVENT_ID}</li>
 * <li>{@link #MINUTES}</li>
 * <li>{@link #METHOD}</li>
 * </ul>
 */
class CalendarContractReminders
{
public:
    /**
     * The content:// style URL for the top-level calendar authority
     */
    static CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    static CARAPI Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int64 eventId,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** cursor);

private:
    static const String REMINDERS_WHERE;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CALENDARCONTRACTREMENDERS_H__
