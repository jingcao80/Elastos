#ifndef __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTREMENDERS_H__
#define __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTREMENDERS_H__

#include "_Elastos_Droid_Provider_CCalendarContractReminders.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver ;
using Elastos::Droid::Database::ICursor ;
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
CarClass(CCalendarContractReminders)
    , public Singleton
    , public ICalendarContractReminders
    , public IBaseColumns
    , public ICalendarContractRemindersColumns
    , public ICalendarContractEventsColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URL for the top-level calendar authority
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI Query(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 eventId,
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTREMENDERS_H__
