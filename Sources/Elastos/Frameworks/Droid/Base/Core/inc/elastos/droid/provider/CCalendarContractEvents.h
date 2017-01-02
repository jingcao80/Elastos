//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTEVENTS_H__
#define __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTEVENTS_H__

#include "_Elastos_Droid_Provider_CCalendarContractEvents.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

/**
 * Constants and helpers for the Events table, which contains details for
 * individual events. <h3>Operations</h3> All operations can be done either
 * as an app or as a sync adapter. To perform an operation as a sync adapter
 * {@link #CALLER_IS_SYNCADAPTER} should be set to true and
 * {@link #ACCOUNT_NAME} and {@link #ACCOUNT_TYPE} must be set in the Uri
 * parameters. See
 * {@link Uri.Builder#appendQueryParameter(java.lang.String, java.lang.String)}
 * for details on adding parameters. Sync adapters have write access to more
 * columns but are restricted to a single account at a time.
 * <dl>
 * <dt><b>Insert</b></dt>
 * <dd>When inserting a new event the following fields must be included:
 * <ul>
 * <li>dtstart</li>
 * <li>dtend if the event is non-recurring</li>
 * <li>duration if the event is recurring</li>
 * <li>rrule or rdate if the event is recurring</li>
 * <li>eventTimezone</li>
 * <li>a calendar_id</li>
 * </ul>
 * There are also further requirements when inserting or updating an event.
 * See the section on Writing to Events.</dd>
 * <dt><b>Update</b></dt>
 * <dd>To perform an update of an Event the {@link Events#_ID} of the event
 * should be provided either as an appended id to the Uri (
 * {@link ContentUris#withAppendedId}) or as the first selection item--the
 * selection should start with "_id=?" and the first selectionArg should be
 * the _id of the event. Updates may also be done using a selection and no
 * id. Updating an event must respect the same rules as inserting and is
 * further restricted in the fields that can be written. See the section on
 * Writing to Events.</dd>
 * <dt><b>Delete</b></dt>
 * <dd>Events can be deleted either by the {@link Events#_ID} as an appended
 * id on the Uri or using any standard selection. If an appended id is used
 * a selection is not allowed. There are two versions of delete: as an app
 * and as a sync adapter. An app delete will set the deleted column on an
 * event and remove all instances of that event. A sync adapter delete will
 * remove the event from the database and all associated data.</dd>
 * <dt><b>Query</b></dt>
 * <dd>Querying the Events table will get you all information about a set of
 * events except their reminders, attendees, and extended properties. There
 * will be one row returned for each event that matches the query selection,
 * or at most a single row if the {@link Events#_ID} is appended to the Uri.
 * Recurring events will only return a single row regardless of the number
 * of times that event repeats.</dd>
 * </dl>
 * <h3>Writing to Events</h3> There are further restrictions on all Updates
 * and Inserts in the Events table:
 * <ul>
 * <li>If allDay is set to 1 eventTimezone must be {@link Time#TIMEZONE_UTC}
 * and the time must correspond to a midnight boundary.</li>
 * <li>Exceptions are not allowed to recur. If rrule or rdate is not empty,
 * original_id and original_sync_id must be empty.</li>
 * <li>In general a calendar_id should not be modified after insertion. This
 * is not explicitly forbidden but many sync adapters will not behave in an
 * expected way if the calendar_id is modified.</li>
 * </ul>
 * The following Events columns are writable by both an app and a sync
 * adapter.
 * <ul>
 * <li>{@link #CALENDAR_ID}</li>
 * <li>{@link #ORGANIZER}</li>
 * <li>{@link #TITLE}</li>
 * <li>{@link #EVENT_LOCATION}</li>
 * <li>{@link #DESCRIPTION}</li>
 * <li>{@link #EVENT_COLOR}</li>
 * <li>{@link #DTSTART}</li>
 * <li>{@link #DTEND}</li>
 * <li>{@link #EVENT_TIMEZONE}</li>
 * <li>{@link #EVENT_END_TIMEZONE}</li>
 * <li>{@link #DURATION}</li>
 * <li>{@link #ALL_DAY}</li>
 * <li>{@link #RRULE}</li>
 * <li>{@link #RDATE}</li>
 * <li>{@link #EXRULE}</li>
 * <li>{@link #EXDATE}</li>
 * <li>{@link #ORIGINAL_ID}</li>
 * <li>{@link #ORIGINAL_SYNC_ID}</li>
 * <li>{@link #ORIGINAL_INSTANCE_TIME}</li>
 * <li>{@link #ORIGINAL_ALL_DAY}</li>
 * <li>{@link #ACCESS_LEVEL}</li>
 * <li>{@link #AVAILABILITY}</li>
 * <li>{@link #GUESTS_CAN_MODIFY}</li>
 * <li>{@link #GUESTS_CAN_INVITE_OTHERS}</li>
 * <li>{@link #GUESTS_CAN_SEE_GUESTS}</li>
 * <li>{@link #CUSTOM_APP_PACKAGE}</li>
 * <li>{@link #CUSTOM_APP_URI}</li>
 * <li>{@link #UID_2445}</li>
 * </ul>
 * The following Events columns are writable only by a sync adapter
 * <ul>
 * <li>{@link #DIRTY}</li>
 * <li>{@link #_SYNC_ID}</li>
 * <li>{@link #SYNC_DATA1}</li>
 * <li>{@link #SYNC_DATA2}</li>
 * <li>{@link #SYNC_DATA3}</li>
 * <li>{@link #SYNC_DATA4}</li>
 * <li>{@link #SYNC_DATA5}</li>
 * <li>{@link #SYNC_DATA6}</li>
 * <li>{@link #SYNC_DATA7}</li>
 * <li>{@link #SYNC_DATA8}</li>
 * <li>{@link #SYNC_DATA9}</li>
 * <li>{@link #SYNC_DATA10}</li>
 * </ul>
 * The remaining columns are either updated by the provider only or are
 * views into other tables and cannot be changed through the Events table.
 */
CarClass(CCalendarContractEvents)
    , public Singleton
    , public ICalendarContractEvents
    , public IBaseColumns
    , public ICalendarContractSyncColumns
    , public ICalendarContractEventsColumns
    , public ICalendarContractCalendarColumns
    , public ICalendarContract
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URL for interacting with events. Appending an
     * event id using {@link ContentUris#withAppendedId(Uri, long)} will
     * specify a single event.
     */
    //@SuppressWarnings("hiding")
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URI for recurring event exceptions.  Insertions require an
     * appended event ID.  Deletion of exceptions requires both the original event ID and
     * the exception event ID (see {@link Uri.Builder#appendPath}).
     */
    CARAPI GetCONTENTEXCEPTIONURI(
        /* [out] */ IUri** uri);

    /**
     * These are columns that should only ever be updated by the provider,
     * either because they are views mapped to another table or because they
     * are used for provider only functionality. TODO move to provider
     *
     * @hide
     */
    CARAPI GetPROVIDERWRITABLECOLUMNS(
        /* [out, callee] */ ArrayOf<String>** columns);

    /**
     * These fields are only writable by a sync adapter. To modify them the
     * caller must include CALLER_IS_SYNCADAPTER, _SYNC_ACCOUNT, and
     * _SYNC_ACCOUNT_TYPE in the query parameters. TODO move to provider.
     *
     * @hide
     */
    CARAPI GetSYNCWRITABLECOLUMNS(
        /* [out, callee] */ ArrayOf<String>** columns);

public:
    /**
     * These are columns that should only ever be updated by the provider,
     * either because they are views mapped to another table or because they
     * are used for provider only functionality. TODO move to provider
     *
     * @hide
     */
    static AutoPtr<ArrayOf<String> > PROVIDER_WRITABLE_COLUMNS;

    /**
     * These fields are only writable by a sync adapter. To modify them the
     * caller must include CALLER_IS_SYNCADAPTER, _SYNC_ACCOUNT, and
     * _SYNC_ACCOUNT_TYPE in the query parameters. TODO move to provider.
     *
     * @hide
     */
    static AutoPtr<ArrayOf<String> > SYNC_WRITABLE_COLUMNS;

private:
    /**
     * The default sort order for this table
     */
     static const String DEFAULT_SORT_ORDER;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTEVENTS_H__
