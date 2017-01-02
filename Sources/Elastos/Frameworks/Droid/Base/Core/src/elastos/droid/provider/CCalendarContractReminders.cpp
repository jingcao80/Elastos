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