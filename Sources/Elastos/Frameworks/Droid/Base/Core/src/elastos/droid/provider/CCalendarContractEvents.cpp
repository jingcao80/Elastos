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

#include "elastos/droid/provider/CCalendarContractEvents.h"
#include "elastos/droid/net/Uri.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Net::Uri;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Provider {

static AutoPtr<ArrayOf<String> > initPROVIDERWRITABLECOLUMNS()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(24);

    (*args)[0] = ICalendarContractSyncColumns::ACCOUNT_NAME;
    (*args)[1] = ICalendarContractSyncColumns::ACCOUNT_TYPE;
    (*args)[2] = ICalendarContractCalendarSyncColumns::CAL_SYNC1;
    (*args)[3] = ICalendarContractCalendarSyncColumns::CAL_SYNC2;
    (*args)[4] = ICalendarContractCalendarSyncColumns::CAL_SYNC3;
    (*args)[5] = ICalendarContractCalendarSyncColumns::CAL_SYNC4;
    (*args)[6] = ICalendarContractCalendarSyncColumns::CAL_SYNC5;
    (*args)[7] = ICalendarContractCalendarSyncColumns::CAL_SYNC6;
    (*args)[8] = ICalendarContractCalendarSyncColumns::CAL_SYNC7;
    (*args)[9] = ICalendarContractCalendarSyncColumns::CAL_SYNC8;
    (*args)[10] = ICalendarContractCalendarSyncColumns::CAL_SYNC9;
    (*args)[11] = ICalendarContractCalendarSyncColumns::CAL_SYNC10;
    (*args)[12] = ICalendarContractCalendarColumns::ALLOWED_REMINDERS;
    (*args)[13] = ICalendarContractCalendarColumns::ALLOWED_ATTENDEE_TYPES;
    (*args)[14] = ICalendarContractCalendarColumns::ALLOWED_AVAILABILITY;
    (*args)[15] = ICalendarContractCalendarColumns::CALENDAR_ACCESS_LEVEL;
    (*args)[16] = ICalendarContractCalendarColumns::CALENDAR_COLOR;
    (*args)[17] = ICalendarContractCalendarColumns::CALENDAR_TIME_ZONE;
    (*args)[18] = ICalendarContractCalendarColumns::CAN_MODIFY_TIME_ZONE;
    (*args)[19] = ICalendarContractCalendarColumns::CAN_ORGANIZER_RESPOND;
    (*args)[20] = ICalendarContractCalendarColumns::CALENDAR_DISPLAY_NAME;
    (*args)[21] = ICalendarContractSyncColumns::CAN_PARTIALLY_UPDATE;
    (*args)[22] = ICalendarContractCalendarColumns::SYNC_EVENTS;
    (*args)[23] = ICalendarContractCalendarColumns::VISIBLE;

    return args;
}

static AutoPtr<ArrayOf<String> > initSYNCWRITABLECOLUMNS()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(24);

    (*args)[0] = ICalendarContractSyncColumns::_SYNC_ID;
    (*args)[1] = ICalendarContractSyncColumns::DIRTY;
    (*args)[2] = ICalendarContractEventsColumns::SYNC_DATA1;
    (*args)[3] = ICalendarContractEventsColumns::SYNC_DATA2;
    (*args)[4] = ICalendarContractEventsColumns::SYNC_DATA3;
    (*args)[5] = ICalendarContractEventsColumns::SYNC_DATA4;
    (*args)[6] = ICalendarContractEventsColumns::SYNC_DATA5;
    (*args)[7] = ICalendarContractEventsColumns::SYNC_DATA6;
    (*args)[8] = ICalendarContractEventsColumns::SYNC_DATA7;
    (*args)[9] = ICalendarContractEventsColumns::SYNC_DATA8;
    (*args)[10] = ICalendarContractEventsColumns::SYNC_DATA9;
    (*args)[11] = ICalendarContractEventsColumns::SYNC_DATA10;

    return args;
}

AutoPtr<ArrayOf<String> > CCalendarContractEvents::PROVIDER_WRITABLE_COLUMNS = initPROVIDERWRITABLECOLUMNS();
AutoPtr<ArrayOf<String> > CCalendarContractEvents::SYNC_WRITABLE_COLUMNS = initSYNCWRITABLECOLUMNS();
const String CCalendarContractEvents::DEFAULT_SORT_ORDER = String("");

CAR_SINGLETON_IMPL(CCalendarContractEvents)

CAR_INTERFACE_IMPL_6(CCalendarContractEvents, Singleton
    , ICalendarContractEvents
    , IBaseColumns
    , ICalendarContractSyncColumns
    , ICalendarContractEventsColumns
    , ICalendarContractCalendarColumns
    , ICalendarContract)

ECode CCalendarContractEvents::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    StringBuilder builder;
    builder += "content://";
    builder += ICalendarContract::AUTHORITY;
    builder += "/events";
    String str = builder.ToString();
    return Uri::Parse(str, uri);
}

ECode CCalendarContractEvents::GetCONTENTEXCEPTIONURI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    StringBuilder builder;
    builder += "content://";
    builder += ICalendarContract::AUTHORITY;
    builder += "/exception";
    String str = builder.ToString();
    return Uri::Parse(str, uri);
}

ECode CCalendarContractEvents::GetPROVIDERWRITABLECOLUMNS(
    /* [out, callee] */ ArrayOf<String>** columns)
{
    VALIDATE_NOT_NULL(columns);

    *columns = PROVIDER_WRITABLE_COLUMNS;
    REFCOUNT_ADD(*columns);
    return NOERROR;
}

ECode CCalendarContractEvents::GetSYNCWRITABLECOLUMNS(
    /* [out, callee] */ ArrayOf<String>** columns)
{
    VALIDATE_NOT_NULL(columns);

    *columns = SYNC_WRITABLE_COLUMNS;
    REFCOUNT_ADD(*columns);
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos