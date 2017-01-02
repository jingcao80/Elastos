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

#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGGROUPBUILDER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGGROUPBUILDER_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Text.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Text::Format::ITime;

namespace Elastos{
namespace Droid{
namespace Dialer {
namespace CallLog {

class CallLogGroupBuilder : public Object
{
public:
    CallLogGroupBuilder(
        /* [in] */ ICallLogGroupBuilderGroupCreator* groupCreator);

    /**
     * Finds all groups of adjacent entries in the call log which should be grouped together and
     * calls {@link GroupCreator#addGroup(int, int, boolean)} on {@link #mGroupCreator} for each of
     * them.
     * <p>
     * For entries that are not grouped with others, we do not need to create a group of size one.
     * <p>
     * It assumes that the cursor will not change during its execution.
     *
     * @see GroupingListAdapter#addGroups(Cursor)
     */
    CARAPI_(void) AddGroups(
        /* [in] */ ICursor* cursor);

    // @VisibleForTesting
    CARAPI_(Boolean) EqualNumbers(
        /* [in] */ const String& number1,
        /* [in] */ const String& number2);

    CARAPI_(Boolean) CompareSipAddresses(
        /* [in] */ const String& number1,
        /* [in] */ const String& number2);
private:
    /**
     * Creates a group of items in the cursor.
     * <p>
     * The group is always unexpanded.
     *
     * @see CallLogAdapter#addGroup(int, int, boolean)
     */
    CARAPI_(void) AddGroup(
        /* [in] */ Int32 cursorPosition,
        /* [in] */ Int32 size);

    /**
     * Given a call date and the current date, determine which date group the call belongs in.
     *
     * @param date The call date.
     * @param now The current date.
     * @return The date group the call belongs in.
     */
    CARAPI_(Int32) GetDayGroup(
        /* [in] */ Int64 date,
        /* [in] */ Int64 now);

public:
    /**
     * Day grouping for call log entries used to represent no associated day group.  Used primarily
     * when retrieving the previous day group, but there is no previous day group (i.e. we are at
     * the start of the list).
     */
    static const Int32 DAY_GROUP_NONE = -1;

    /** Day grouping for calls which occurred today. */
    static const Int32 DAY_GROUP_TODAY = 0;

    /** Day grouping for calls which occurred yesterday. */
    static const Int32 DAY_GROUP_YESTERDAY = 1;

    /** Day grouping for calls which occurred before last week. */
    static const Int32 DAY_GROUP_OTHER = 2;

private:
    /** Instance of the time object used for time calculations. */
    static AutoPtr<ITime> TIME;

    /** The object on which the groups are created. */
    ICallLogGroupBuilderGroupCreator* mGroupCreator;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CALLLOGGROUPBUILDER_H__

