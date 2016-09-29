
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/dialer/calllog/CallLogGroupBuilder.h"
#include "elastos/droid/dialer/calllog/CallLogQuery.h"
#include "elastos/droid/contacts/common/util/PhoneNumberHelper.h"
#include "elastos/droid/contacts/common/util/DateUtils.h"
#include "elastos/utility/Objects.h"

using Elastos::Droid::Contacts::Common::Util::PhoneNumberHelper;
using Elastos::Droid::Contacts::Common::Util::DateUtils;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

const Int32 CallLogGroupBuilder::DAY_GROUP_NONE;
const Int32 CallLogGroupBuilder::DAY_GROUP_TODAY;
const Int32 CallLogGroupBuilder::DAY_GROUP_YESTERDAY;
const Int32 CallLogGroupBuilder::DAY_GROUP_OTHER;

static AutoPtr<ITime> createTime()
{
    AutoPtr<ITime> obj;
    CTime::New((ITime**)&obj);
    return obj;
}
AutoPtr<ITime> CallLogGroupBuilder::TIME = createTime();

CallLogGroupBuilder::CallLogGroupBuilder(
    /* [in] */ ICallLogGroupBuilderGroupCreator* groupCreator)
{
    mGroupCreator = groupCreator;
}

void CallLogGroupBuilder::AddGroups(
    /* [in] */ ICursor* cursor)
{
    Int32 count;
    cursor->GetCount(&count);
    if (count == 0) {
        return;
    }

    // Clear any previous day grouping information.
    mGroupCreator->ClearDayGroups();

    // Get current system time, used for calculating which day group calls belong to.
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 currentTime;
    sys->GetCurrentTimeMillis(&currentTime);

    Int32 currentGroupSize = 1;
    Boolean succeeded;
    cursor->MoveToFirst(&succeeded);
    // The number of the first entry in the group.
    String firstNumber;
    cursor->GetString(CallLogQuery::NUMBER, &firstNumber);
    // This is the type of the first call in the group.
    Int32 firstCallType;
    cursor->GetInt32(CallLogQuery::CALL_TYPE, &firstCallType);

    // The account information of the first entry in the group.
    String firstAccountComponentName;
    cursor->GetString(CallLogQuery::ACCOUNT_COMPONENT_NAME, &firstAccountComponentName);
    String firstAccountId;
    cursor->GetString(CallLogQuery::ACCOUNT_ID, &firstAccountId);

    // Determine the day group for the first call in the cursor.
    Int64 firstDate;
    cursor->GetInt64(CallLogQuery::DATE, &firstDate);
    Int64 firstRowId;
    cursor->GetInt64(CallLogQuery::ID, &firstRowId);
    Int32 currentGroupDayGroup = GetDayGroup(firstDate, currentTime);
    mGroupCreator->SetDayGroup(firstRowId, currentGroupDayGroup);

    while (cursor->MoveToNext(&succeeded), succeeded) {
        // The number of the current row in the cursor.
        String currentNumber;
        cursor->GetString(CallLogQuery::NUMBER, &currentNumber);
        Int32 callType;
        cursor->GetInt32(CallLogQuery::CALL_TYPE, &callType);
        String currentAccountComponentName;
        cursor->GetString(
                CallLogQuery::ACCOUNT_COMPONENT_NAME, &currentAccountComponentName);
        String currentAccountId;
        cursor->GetString(CallLogQuery::ACCOUNT_ID, &currentAccountId);

        Boolean sameNumber = EqualNumbers(firstNumber, currentNumber);
        Boolean sameAccountComponentName = firstAccountComponentName.IsNull()
                ? currentAccountComponentName.IsNull()
                : firstAccountComponentName.Equals(currentAccountComponentName);
        Boolean sameAccountId = Objects::Equals(&firstAccountId, &currentAccountId);
        Boolean sameAccount = sameAccountComponentName && sameAccountId;

        Boolean shouldGroup;
        Int64 currentCallId;
        cursor->GetInt64(CallLogQuery::ID, &currentCallId);
        Int64 date;
        cursor->GetInt64(CallLogQuery::DATE, &date);

        if (!sameNumber || !sameAccount) {
            // Should only group with calls from the same number.
            shouldGroup = FALSE;
        }
        else if (firstCallType == ICalls::VOICEMAIL_TYPE) {
            // never group voicemail.
            shouldGroup = FALSE;
        }
        else {
            // Incoming, outgoing, and missed calls group together.
            shouldGroup = callType != ICalls::VOICEMAIL_TYPE;
        }

        if (shouldGroup) {
            // Increment the size of the group to include the current call, but do not create
            // the group until we find a call that does not match.
            currentGroupSize++;
        }
        else {
            // The call group has changed, so determine the day group for the new call group.
            // This ensures all calls grouped together in the call log are assigned the same
            // day group.
            currentGroupDayGroup = GetDayGroup(date, currentTime);

            // Create a group for the previous set of calls, excluding the current one, but do
            // not create a group for a single call.
            if (currentGroupSize > 1) {
                Int32 position;
                cursor->GetPosition(&position);
                AddGroup(position - currentGroupSize, currentGroupSize);
            }
            // Start a new group; it will include at least the current call.
            currentGroupSize = 1;
            // The current entry is now the first in the group.
            firstNumber = currentNumber;
            firstCallType = callType;
            firstAccountComponentName = currentAccountComponentName;
            firstAccountId = currentAccountId;
        }

        // Save the day group associated with the current call.
        mGroupCreator->SetDayGroup(currentCallId, currentGroupDayGroup);
    }
    // If the last set of calls at the end of the call log was itself a group, create it now.
    if (currentGroupSize > 1) {
        AddGroup(count - currentGroupSize, currentGroupSize);
    }
}

void CallLogGroupBuilder::AddGroup(
    /* [in] */ Int32 cursorPosition,
    /* [in] */ Int32 size)
{
    mGroupCreator->AddGroup(cursorPosition, size, FALSE);
}

Boolean CallLogGroupBuilder::EqualNumbers(
    /* [in] */ const String& number1,
    /* [in] */ const String& number2)
{
    if (PhoneNumberHelper::IsUriNumber(number1) || PhoneNumberHelper::IsUriNumber(number2)) {
        return CompareSipAddresses(number1, number2);
    }
    else {
        return PhoneNumberUtils::Compare(number1, number2);
    }
}

Boolean CallLogGroupBuilder::CompareSipAddresses(
    /* [in] */ const String& number1,
    /* [in] */ const String& number2)
{
    if (number1.IsNull() || number2.IsNull()) {
        return number1 == number2;
    }

    Int32 index1 = number1.IndexOf('@');
    String userinfo1;
    String rest1;
    if (index1 != -1) {
        userinfo1 = number1.Substring(0, index1);
        rest1 = number1.Substring(index1);
    }
    else {
        userinfo1 = number1;
        rest1 = "";
    }

    Int32 index2 = number2.IndexOf('@');
    String userinfo2;
    String rest2;
    if (index2 != -1) {
        userinfo2 = number2.Substring(0, index2);
        rest2 = number2.Substring(index2);
    }
    else {
        userinfo2 = number2;
        rest2 = "";
    }

    return userinfo1.Equals(userinfo2) && rest1.EqualsIgnoreCase(rest2);
}

Int32 CallLogGroupBuilder::GetDayGroup(
    /* [in] */ Int64 date,
    /* [in] */ Int64 now)
{
    Int32 days;
    days = DateUtils::GetDayDifference(TIME, date, now);

    if (days == 0) {
        return DAY_GROUP_TODAY;
    }
    else if (days == 1) {
        return DAY_GROUP_YESTERDAY;
    }
    else {
        return DAY_GROUP_OTHER;
    }
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
