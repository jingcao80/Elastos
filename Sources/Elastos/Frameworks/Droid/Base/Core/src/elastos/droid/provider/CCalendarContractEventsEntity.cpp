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

#include "Elastos.Droid.Database.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/CEntity.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CalendarContractAttendees.h"
#include "elastos/droid/provider/CalendarContractExtendedProperties.h"
#include "elastos/droid/provider/CalendarContractReminders.h"
#include "elastos/droid/provider/CCalendarContractEventsEntity.h"
#include "elastos/droid/provider/ContactsContract.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CEntity;
using Elastos::Droid::Content::EIID_ICursorEntityIterator;
using Elastos::Droid::Content::EIID_IEntityIterator;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::ICursorEntityIterator;
using Elastos::Droid::Content::IEntity;
using Elastos::Droid::Content::IEntityIterator;
using Elastos::Droid::Database::DatabaseUtils;
using Elastos::Droid::Net::Uri;
using Elastos::Utility::EIID_IIterator;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Provider {

static AutoPtr<ArrayOf<String> > initREMINDERSPROJECTION()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);

    (*args)[0] = ICalendarContractRemindersColumns::MINUTES;
    (*args)[1] = ICalendarContractRemindersColumns::METHOD;

    return args;
}

static AutoPtr<ArrayOf<String> > initATTENDEESPROJECTION()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(7);

    (*args)[0] = ICalendarContractAttendeesColumns::ATTENDEE_NAME;
    (*args)[1] = ICalendarContractAttendeesColumns::ATTENDEE_EMAIL;
    (*args)[2] = ICalendarContractAttendeesColumns::ATTENDEE_RELATIONSHIP;
    (*args)[3] = ICalendarContractAttendeesColumns::ATTENDEE_TYPE;
    (*args)[4] = ICalendarContractAttendeesColumns::ATTENDEE_STATUS;
    (*args)[5] = ICalendarContractAttendeesColumns::ATTENDEE_IDENTITY;
    (*args)[6] = ICalendarContractAttendeesColumns::ATTENDEE_ID_NAMESPACE;

    return args;
}

static AutoPtr<ArrayOf<String> > initEXTENDEDPROJECTION()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(3);

    (*args)[0] = IBaseColumns::ID;
    (*args)[1] = ICalendarContractExtendedPropertiesColumns::NAME;
    (*args)[2] = ICalendarContractExtendedPropertiesColumns::VALUE;

    return args;
}

AutoPtr<ArrayOf<String> > CCalendarContractEventsEntity::EntityIteratorImpl::REMINDERS_PROJECTION = initREMINDERSPROJECTION();
AutoPtr<ArrayOf<String> > CCalendarContractEventsEntity::EntityIteratorImpl::ATTENDEES_PROJECTION = initATTENDEESPROJECTION();
AutoPtr<ArrayOf<String> > CCalendarContractEventsEntity::EntityIteratorImpl::EXTENDED_PROJECTION = initEXTENDEDPROJECTION();

Int32 CCalendarContractEventsEntity::EntityIteratorImpl::COLUMN_MINUTES = 0;
Int32 CCalendarContractEventsEntity::EntityIteratorImpl::COLUMN_METHOD = 1;

Int32 CCalendarContractEventsEntity::EntityIteratorImpl::COLUMN_ATTENDEE_NAME = 0;
Int32 CCalendarContractEventsEntity::EntityIteratorImpl::COLUMN_ATTENDEE_EMAIL = 1;
Int32 CCalendarContractEventsEntity::EntityIteratorImpl::COLUMN_ATTENDEE_RELATIONSHIP = 2;
Int32 CCalendarContractEventsEntity::EntityIteratorImpl::COLUMN_ATTENDEE_TYPE = 3;
Int32 CCalendarContractEventsEntity::EntityIteratorImpl::COLUMN_ATTENDEE_STATUS = 4;
Int32 CCalendarContractEventsEntity::EntityIteratorImpl::COLUMN_ATTENDEE_IDENTITY = 5;
Int32 CCalendarContractEventsEntity::EntityIteratorImpl::COLUMN_ATTENDEE_ID_NAMESPACE = 6;

Int32 CCalendarContractEventsEntity::EntityIteratorImpl::COLUMN_ID = 0;
Int32 CCalendarContractEventsEntity::EntityIteratorImpl::COLUMN_NAME = 1;
Int32 CCalendarContractEventsEntity::EntityIteratorImpl::COLUMN_VALUE = 2;

String CCalendarContractEventsEntity::EntityIteratorImpl::WHERE_EVENT_ID = String("event_id=?");

CAR_INTERFACE_IMPL_3(CCalendarContractEventsEntity::EntityIteratorImpl, Object, ICursorEntityIterator, IEntityIterator, IIterator)

CCalendarContractEventsEntity::EntityIteratorImpl::EntityIteratorImpl()
{}

CCalendarContractEventsEntity::EntityIteratorImpl::~EntityIteratorImpl()
{}

ECode CCalendarContractEventsEntity::EntityIteratorImpl::constructor(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentResolver* resolver)
{
    mResolver = resolver;
    return CursorEntityIterator::constructor(cursor);
}

ECode CCalendarContractEventsEntity::EntityIteratorImpl::constructor(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentProviderClient* provider)
{
    mProvider = provider;
    return CursorEntityIterator::constructor(cursor);
}

ECode CCalendarContractEventsEntity::EntityIteratorImpl::GetEntityAndIncrementCursor(
    /* [in] */ ICursor* cursor,
    /* [out] */ IEntity** entity)
{
    // we expect the cursor is already at the row we need to read from
    Int32 columnIndex;
    FAIL_RETURN(cursor->GetColumnIndexOrThrow(IBaseColumns::ID, &columnIndex));
    Int64 eventId;
    cursor->GetInt64(columnIndex, &eventId);

    AutoPtr<IContentValues> cv;
    CContentValues::New((IContentValues**)&cv);
    cv->Put(IBaseColumns::ID, eventId);

    DatabaseUtils::CursorInt32ToContentValuesIfPresent(cursor, cv, CALENDAR_ID);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, TITLE);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, DESCRIPTION);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, EVENT_LOCATION);
    DatabaseUtils::CursorInt32ToContentValuesIfPresent(cursor, cv, STATUS);
    DatabaseUtils::CursorInt32ToContentValuesIfPresent(cursor, cv, SELF_ATTENDEE_STATUS);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, DTSTART);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, DTEND);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, DURATION);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, EVENT_TIMEZONE);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, EVENT_END_TIMEZONE);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ALL_DAY);
    DatabaseUtils::CursorInt32ToContentValuesIfPresent(cursor, cv, ACCESS_LEVEL);
    DatabaseUtils::CursorInt32ToContentValuesIfPresent(cursor, cv, AVAILABILITY);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, HAS_ALARM);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, HAS_EXTENDED_PROPERTIES);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, RRULE);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, RDATE);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, EXRULE);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, EXDATE);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ORIGINAL_SYNC_ID);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ORIGINAL_ID);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, ORIGINAL_INSTANCE_TIME);
    DatabaseUtils::CursorInt32ToContentValuesIfPresent(cursor, cv, ORIGINAL_ALL_DAY);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, LAST_DATE);
    DatabaseUtils::CursorInt32ToContentValuesIfPresent(cursor, cv, HAS_ATTENDEE_DATA);
    DatabaseUtils::CursorInt32ToContentValuesIfPresent(cursor, cv, GUESTS_CAN_INVITE_OTHERS);
    DatabaseUtils::CursorInt32ToContentValuesIfPresent(cursor, cv, GUESTS_CAN_MODIFY);
    DatabaseUtils::CursorInt32ToContentValuesIfPresent(cursor, cv, GUESTS_CAN_SEE_GUESTS);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, CUSTOM_APP_PACKAGE);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, CUSTOM_APP_URI);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, UID_2445);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ORGANIZER);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, IS_ORGANIZER);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, _SYNC_ID);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, DIRTY);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, LAST_SYNCED);
    DatabaseUtils::CursorInt32ToContentValuesIfPresent(cursor, cv, DELETED);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractEventsColumns::SYNC_DATA1);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractEventsColumns::SYNC_DATA2);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractEventsColumns::SYNC_DATA3);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractEventsColumns::SYNC_DATA4);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractEventsColumns::SYNC_DATA5);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractEventsColumns::SYNC_DATA6);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractEventsColumns::SYNC_DATA7);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractEventsColumns::SYNC_DATA8);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractEventsColumns::SYNC_DATA9);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractEventsColumns::SYNC_DATA10);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractCalendarSyncColumns::CAL_SYNC1);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractCalendarSyncColumns::CAL_SYNC2);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractCalendarSyncColumns::CAL_SYNC3);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractCalendarSyncColumns::CAL_SYNC4);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractCalendarSyncColumns::CAL_SYNC5);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractCalendarSyncColumns::CAL_SYNC6);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractCalendarSyncColumns::CAL_SYNC7);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractCalendarSyncColumns::CAL_SYNC8);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractCalendarSyncColumns::CAL_SYNC9);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ICalendarContractCalendarSyncColumns::CAL_SYNC10);

    // Create the Entity from the ContentValue
    AutoPtr<IEntity> _entity;
    CEntity::New(cv, (IEntity**)&_entity);

    AutoPtr<ICursor> subCursor;
    AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
    (*selectionArgs)[0] = StringUtils::ToString(eventId);
    AutoPtr<IUri> remindUri;
    CalendarContractReminders::GetCONTENT_URI((IUri**)&remindUri);
    if (mResolver != NULL) {
        FAIL_RETURN(mResolver->Query(remindUri, REMINDERS_PROJECTION,
            WHERE_EVENT_ID, selectionArgs, String(NULL), (ICursor**)&subCursor))
    }
    else {
        FAIL_RETURN(mProvider->Query(remindUri, REMINDERS_PROJECTION,
            WHERE_EVENT_ID, selectionArgs, String(NULL), (ICursor**)&subCursor))
    }
    //try {
    Boolean result;
    while ((subCursor->MoveToNext(&result), result)) {
        AutoPtr<IContentValues> reminderValues;
        CContentValues::New((IContentValues**)&reminderValues);
        Int32 minutus;
        subCursor->GetInt32(COLUMN_MINUTES, &minutus);
        reminderValues->Put(ICalendarContractRemindersColumns::MINUTES, minutus);
        Int32 method;
        subCursor->GetInt32(COLUMN_MINUTES, &method);
        reminderValues->Put(ICalendarContractRemindersColumns::METHOD, method);
        _entity->AddSubValue(remindUri, reminderValues);
    }
    //} finally {
    ICloseable::Probe(cursor)->Close();
    //}

    subCursor = NULL;
    AutoPtr<IUri> attendeesUri;
    CalendarContractAttendees::GetCONTENT_URI((IUri**)&attendeesUri);
    if (mResolver != NULL) {
        FAIL_RETURN(mResolver->Query(attendeesUri, ATTENDEES_PROJECTION,
            WHERE_EVENT_ID, selectionArgs, String(NULL), (ICursor**)&subCursor))
    }
    else {
        FAIL_RETURN(mProvider->Query(attendeesUri, ATTENDEES_PROJECTION,
            WHERE_EVENT_ID, selectionArgs, String(NULL), (ICursor**)&subCursor))
    }

    //try {
    while ((subCursor->MoveToNext(&result), result)) {
        AutoPtr<IContentValues> attendeeValues;
        CContentValues::New((IContentValues**)&attendeeValues);
        String name;
        subCursor->GetString(COLUMN_ATTENDEE_NAME, &name);
        attendeeValues->Put(ICalendarContractAttendeesColumns::ATTENDEE_NAME, name);

        String email;
        subCursor->GetString(COLUMN_ATTENDEE_EMAIL, &email);
        attendeeValues->Put(ICalendarContractAttendeesColumns::ATTENDEE_EMAIL, email);

        Int32 relationship;
        subCursor->GetInt32(COLUMN_ATTENDEE_RELATIONSHIP, &relationship);
        attendeeValues->Put(ICalendarContractAttendeesColumns::ATTENDEE_RELATIONSHIP, relationship);

        Int32 type;
        subCursor->GetInt32(COLUMN_ATTENDEE_TYPE, &type);
        attendeeValues->Put(ICalendarContractAttendeesColumns::ATTENDEE_TYPE, type);

        Int32 status;
        subCursor->GetInt32(COLUMN_ATTENDEE_STATUS, &status);
        attendeeValues->Put(ICalendarContractAttendeesColumns::ATTENDEE_STATUS, status);

        String identity;
        subCursor->GetString(COLUMN_ATTENDEE_IDENTITY, &identity);
        attendeeValues->Put(ICalendarContractAttendeesColumns::ATTENDEE_IDENTITY, identity);

        String nameSpace;
        subCursor->GetString(COLUMN_ATTENDEE_ID_NAMESPACE, &nameSpace);
        attendeeValues->Put(ICalendarContractAttendeesColumns::ATTENDEE_ID_NAMESPACE, nameSpace);

        _entity->AddSubValue(attendeesUri, attendeeValues);
    }
    //} finally {
    ICloseable::Probe(subCursor)->Close();
    //}

    subCursor = NULL;
    AutoPtr<IUri> propertiesUri;
    CalendarContractExtendedProperties::GetCONTENT_URI((IUri**)&propertiesUri);
    if (mResolver != NULL) {
        FAIL_RETURN(mResolver->Query(propertiesUri, EXTENDED_PROJECTION,
            WHERE_EVENT_ID, selectionArgs, String(NULL), (ICursor**)&subCursor))
    } else {
        FAIL_RETURN(mProvider->Query(propertiesUri, EXTENDED_PROJECTION,
            WHERE_EVENT_ID, selectionArgs, String(NULL), (ICursor**)&subCursor))
    }
    //try {
    while ((subCursor->MoveToNext(&result), result)) {
        AutoPtr<IContentValues> extendedValues;
        CContentValues::New((IContentValues**)&extendedValues);
        String id;
        subCursor->GetString(COLUMN_ID, &id);
        extendedValues->Put(IBaseColumns::ID, id);
        String name;
        subCursor->GetString(COLUMN_NAME, &name);
        extendedValues->Put(ICalendarContractExtendedPropertiesColumns::NAME, name);
        String value;
        subCursor->GetString(COLUMN_VALUE, &value);
        extendedValues->Put(ICalendarContractExtendedPropertiesColumns::VALUE, value);
        _entity->AddSubValue(propertiesUri, extendedValues);
    }
    //} finally {
    ICloseable::Probe(subCursor)->Close();
    //}

    cursor->MoveToNext(&result);
    // Return the created Entity
    *entity = _entity;
    REFCOUNT_ADD(*entity);
    return NOERROR;
}

ECode CCalendarContractEventsEntity::EntityIteratorImpl::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return CursorEntityIterator::HasNext(result);
}

ECode CCalendarContractEventsEntity::EntityIteratorImpl::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<IEntity> entity;
    CursorEntityIterator::GetNext((IEntity**)&entity);
    *object = entity.Get();
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CCalendarContractEventsEntity::EntityIteratorImpl::Remove()
{
    return CursorEntityIterator::Remove();
}

ECode CCalendarContractEventsEntity::EntityIteratorImpl::Reset()
{
    return CursorEntityIterator::Reset();
}

ECode CCalendarContractEventsEntity::EntityIteratorImpl::Close()
{
    return CursorEntityIterator::Close();
}

CAR_SINGLETON_IMPL(CCalendarContractEventsEntity)

CAR_INTERFACE_IMPL_4(CCalendarContractEventsEntity, Singleton
    , ICalendarContractEventsEntity
    , IBaseColumns
    , ICalendarContractSyncColumns
    , ICalendarContractEventsColumns)

ECode CCalendarContractEventsEntity::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    StringBuilder builder;
    builder += "content://";
    builder += ICalendarContract::AUTHORITY;
    builder += "/event_entities";
    String str = builder.ToString();
    return Uri::Parse(str, uri);
}


ECode CCalendarContractEventsEntity::NewEntityIterator(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentResolver* resolver,
    /* [out] */ IEntityIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator);

    AutoPtr<EntityIteratorImpl> impl = new EntityIteratorImpl();
    impl->constructor(cursor, resolver);
    *iterator = (IEntityIterator*)impl.Get();
    REFCOUNT_ADD(*iterator);
    return NOERROR;
}

ECode CCalendarContractEventsEntity::NewEntityIterator(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentProviderClient* provider,
    /* [out] */ IEntityIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator);

    AutoPtr<EntityIteratorImpl> impl = new EntityIteratorImpl();
    impl->constructor(cursor, provider);
    *iterator = (IEntityIterator*)impl.Get();
    REFCOUNT_ADD(*iterator);
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos