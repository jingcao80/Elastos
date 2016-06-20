
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGGROUPBUILDER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGGROUPBUILDER_H__

#include "_Elastos_Apps_Dialer_CallLog_CCallLogGroupBuilder.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Text.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Text::Format::ITime;

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

CarClass(CCallLogGroupBuilder)
    , public Object
    , public ICallLogGroupBuilder
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CARAPI constructor(
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
    CARAPI AddGroups(
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

private:
    /** Instance of the time object used for time calculations. */
    static AutoPtr<ITime> TIME;

    /** The object on which the groups are created. */
    AutoPtr<ICallLogGroupBuilderGroupCreator> mGroupCreator;
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGGROUPBUILDER_H__

