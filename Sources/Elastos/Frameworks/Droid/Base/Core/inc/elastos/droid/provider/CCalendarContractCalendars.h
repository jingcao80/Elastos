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

#ifndef __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTCALENDARS_H__
#define __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTCALENDARS_H__

#include "_Elastos_Droid_Provider_CCalendarContractCalendars.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

/**
 * Constants and helpers for the Calendars table, which contains details for
 * individual calendars. <h3>Operations</h3> All operations can be done
 * either as an app or as a sync adapter. To perform an operation as a sync
 * adapter {@link #CALLER_IS_SYNCADAPTER} should be set to true and
 * {@link #ACCOUNT_NAME} and {@link #ACCOUNT_TYPE} must be set in the Uri
 * parameters. See
 * {@link Uri.Builder#appendQueryParameter(java.lang.String, java.lang.String)}
 * for details on adding parameters. Sync adapters have write access to more
 * columns but are restricted to a single account at a time. Calendars are
 * designed to be primarily managed by a sync adapter and inserting new
 * calendars should be done as a sync adapter. For the most part, apps
 * should only update calendars (such as changing the color or display
 * name). If a local calendar is required an app can do so by inserting as a
 * sync adapter and using an {@link #ACCOUNT_TYPE} of
 * {@link #ACCOUNT_TYPE_LOCAL} .
 * <dl>
 * <dt><b>Insert</b></dt>
 * <dd>When inserting a new calendar the following fields must be included:
 * <ul>
 * <li>{@link #ACCOUNT_NAME}</li>
 * <li>{@link #ACCOUNT_TYPE}</li>
 * <li>{@link #NAME}</li>
 * <li>{@link #CALENDAR_DISPLAY_NAME}</li>
 * <li>{@link #CALENDAR_COLOR}</li>
 * <li>{@link #CALENDAR_ACCESS_LEVEL}</li>
 * <li>{@link #OWNER_ACCOUNT}</li>
 * </ul>
 * The following fields are not required when inserting a Calendar but are
 * generally a good idea to include:
 * <ul>
 * <li>{@link #SYNC_EVENTS} set to 1</li>
 * <li>{@link #CALENDAR_TIME_ZONE}</li>
 * <li>{@link #ALLOWED_REMINDERS}</li>
 * <li>{@link #ALLOWED_AVAILABILITY}</li>
 * <li>{@link #ALLOWED_ATTENDEE_TYPES}</li>
 * </ul>
 * <dt><b>Update</b></dt>
 * <dd>To perform an update on a calendar the {@link #_ID} of the calendar
 * should be provided either as an appended id to the Uri (
 * {@link ContentUris#withAppendedId}) or as the first selection item--the
 * selection should start with "_id=?" and the first selectionArg should be
 * the _id of the calendar. Calendars may also be updated using a selection
 * without the id. In general, the {@link #ACCOUNT_NAME} and
 * {@link #ACCOUNT_TYPE} should not be changed after a calendar is created
 * as this can cause issues for sync adapters.
 * <dt><b>Delete</b></dt>
 * <dd>Calendars can be deleted either by the {@link #_ID} as an appended id
 * on the Uri or using any standard selection. Deleting a calendar should
 * generally be handled by a sync adapter as it will remove the calendar
 * from the database and all associated data (aka events).</dd>
 * <dt><b>Query</b></dt>
 * <dd>Querying the Calendars table will get you all information about a set
 * of calendars. There will be one row returned for each calendar that
 * matches the query selection, or at most a single row if the {@link #_ID}
 * is appended to the Uri.</dd>
 * </dl>
 * <h3>Calendar Columns</h3> The following Calendar columns are writable by
 * both an app and a sync adapter.
 * <ul>
 * <li>{@link #NAME}</li>
 * <li>{@link #CALENDAR_DISPLAY_NAME}</li>
 * <li>{@link #VISIBLE}</li>
 * <li>{@link #SYNC_EVENTS}</li>
 * </ul>
 * The following Calendars columns are writable only by a sync adapter
 * <ul>
 * <li>{@link #ACCOUNT_NAME}</li>
 * <li>{@link #ACCOUNT_TYPE}</li>
 * <li>{@link #CALENDAR_COLOR}</li>
 * <li>{@link #_SYNC_ID}</li>
 * <li>{@link #DIRTY}</li>
 * <li>{@link #OWNER_ACCOUNT}</li>
 * <li>{@link #MAX_REMINDERS}</li>
 * <li>{@link #ALLOWED_REMINDERS}</li>
 * <li>{@link #ALLOWED_AVAILABILITY}</li>
 * <li>{@link #ALLOWED_ATTENDEE_TYPES}</li>
 * <li>{@link #CAN_MODIFY_TIME_ZONE}</li>
 * <li>{@link #CAN_ORGANIZER_RESPOND}</li>
 * <li>{@link #CAN_PARTIALLY_UPDATE}</li>
 * <li>{@link #CALENDAR_LOCATION}</li>
 * <li>{@link #CALENDAR_TIME_ZONE}</li>
 * <li>{@link #CALENDAR_ACCESS_LEVEL}</li>
 * <li>{@link #DELETED}</li>
 * <li>{@link #CAL_SYNC1}</li>
 * <li>{@link #CAL_SYNC2}</li>
 * <li>{@link #CAL_SYNC3}</li>
 * <li>{@link #CAL_SYNC4}</li>
 * <li>{@link #CAL_SYNC5}</li>
 * <li>{@link #CAL_SYNC6}</li>
 * <li>{@link #CAL_SYNC7}</li>
 * <li>{@link #CAL_SYNC8}</li>
 * <li>{@link #CAL_SYNC9}</li>
 * <li>{@link #CAL_SYNC10}</li>
 * </ul>
 */
CarClass(CCalendarContractCalendars)
    , public Singleton
    , public ICalendarContractCalendars
    , public IBaseColumns
    , public ICalendarContractSyncColumns
    , public ICalendarContractCalendarColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URL for accessing Calendars
     */
    //@SuppressWarnings("hiding")
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * These fields are only writable by a sync adapter. To modify them the
     * caller must include {@link #CALLER_IS_SYNCADAPTER},
     * {@link #ACCOUNT_NAME}, and {@link #ACCOUNT_TYPE} in the Uri's query
     * parameters. TODO move to provider
     *
     * @hide
     */
    CARAPI GetSYNCWRITABLECOLUMNS(
        /* [out, callee] */ ArrayOf<String>** columns);

public:
    static AutoPtr<ArrayOf<String> > SYNC_WRITABLE_COLUMNS;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTCALENDARS_H__
