#ifndef __ELASTOS_DROID_PROVIDER_CALENDARCONTRACTATTENDEES_H__
#define __ELASTOS_DROID_PROVIDER_CALENDARCONTRACTATTENDEES_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

/**
 * Fields and helpers for interacting with Attendees. Each row of this table
 * represents a single attendee or guest of an event. Calling
 * {@link #query(ContentResolver, long, String[])} will return a list of attendees for
 * the event with the given eventId. Both apps and sync adapters may write
 * to this table. There are six writable fields and all of them except
 * {@link #ATTENDEE_NAME} must be included when inserting a new attendee.
 * They are:
 * <ul>
 * <li>{@link #EVENT_ID}</li>
 * <li>{@link #ATTENDEE_NAME}</li>
 * <li>{@link #ATTENDEE_EMAIL}</li>
 * <li>{@link #ATTENDEE_RELATIONSHIP}</li>
 * <li>{@link #ATTENDEE_TYPE}</li>
 * <li>{@link #ATTENDEE_STATUS}</li>
 * <li>{@link #ATTENDEE_IDENTITY}</li>
 * <li>{@link #ATTENDEE_ID_NAMESPACE}</li>
 * </ul>
 */
class CalendarContractAttendees
{
public:
    /**
     * The content:// style URL for accessing Attendees data
     */
    //@SuppressWarnings("hiding")
    static CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Queries all attendees associated with the given event. This is a
     * blocking call and should not be done on the UI thread.
     *
     * @param cr The content resolver to use for the query
     * @param eventId The id of the event to retrieve attendees for
     * @param projection the columns to return in the cursor
     * @return A Cursor containing all attendees for the event
     */
    static CARAPI Query(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 eventId,
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);

private:
    static String ATTENDEES_WHERE;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CALENDARCONTRACTATTENDEES_H__
