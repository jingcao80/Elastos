
#include "elastos/droid/dialer/calllog/CCallLogGroupBuilder.h"

#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Provider::ICalls;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

AutoPtr<ITime> createTime()
{
    AutoPtr<ITime> obj;
    CTime::New((ITime**)&obj);
    return obj;
}

AutoPtr<ITime> CCallLogGroupBuilder::TIME = createTime();

CAR_INTERFACE_IMPL(CCallLogGroupBuilder, Object, ICallLogGroupBuilder);

CAR_OBJECT_IMPL(CCallLogGroupBuilder);

ECode CCallLogGroupBuilder::constructor(
    /* [in] */ ICallLogGroupBuilderGroupCreator* groupCreator)
{
    mGroupCreator = groupCreator;
    return NOERROR;
}

ECode CCallLogGroupBuilder::AddGroups(
    /* [in] */ ICursor* cursor)
{
    Int32 count;
    cursor->GetCount(&count);
    if (count == 0) {
        return NOERROR;
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
    cursor->GetString(ICallLogQuery::NUMBER, &firstNumber);
    // This is the type of the first call in the group.
    Int32 firstCallType;
    cursor->GetInt32(ICallLogQuery::CALL_TYPE, &firstCallType);

    // The account information of the first entry in the group.
    String firstAccountComponentName;
    cursor->GetString(ICallLogQuery::ACCOUNT_COMPONENT_NAME, &firstAccountComponentName);
    String firstAccountId;
    cursor->GetString(ICallLogQuery::ACCOUNT_ID, &firstAccountId);

    // Determine the day group for the first call in the cursor.
    Int64 firstDate;
    cursor->GetInt64(ICallLogQuery::DATE, &firstDate);
    Int64 firstRowId;
    cursor->GetInt64(ICallLogQuery::ID, &firstRowId);
    Int32 currentGroupDayGroup = GetDayGroup(firstDate, currentTime);
    mGroupCreator->SetDayGroup(firstRowId, currentGroupDayGroup);

    while (cursor->MoveToNext(&succeeded), succeeded) {
        // The number of the current row in the cursor.
        String currentNumber;
        cursor->GetString(ICallLogQuery::NUMBER, &currentNumber);
        Int32 callType;
        cursor->GetInt32(ICallLogQuery::CALL_TYPE, &callType);
        String currentAccountComponentName;
        cursor->GetString(
                ICallLogQuery::ACCOUNT_COMPONENT_NAME, &currentAccountComponentName);
        String currentAccountId;
        cursor->GetString(ICallLogQuery::ACCOUNT_ID, &currentAccountId);

        Boolean sameNumber = EqualNumbers(firstNumber, currentNumber);
        Boolean sameAccountComponentName = firstAccountComponentName.IsNull()
                ? currentAccountComponentName.IsNull()
                : firstAccountComponentName.Equals(currentAccountComponentName);
        Boolean sameAccountId = firstAccountId.IsNull()
                ? currentAccountId.IsNull() : firstAccountId.Equals(currentAccountId);
        Boolean sameAccount = sameAccountComponentName && sameAccountId;

        Boolean shouldGroup;
        Int64 currentCallId;
        cursor->GetInt64(ICallLogQuery::ID, &currentCallId);
        Int64 date;
        cursor->GetInt64(ICallLogQuery::DATE, &date);

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

    return NOERROR;
}

void CCallLogGroupBuilder::AddGroup(
    /* [in] */ Int32 cursorPosition,
    /* [in] */ Int32 size)
{
    mGroupCreator->AddGroup(cursorPosition, size, FALSE);
}

Boolean CCallLogGroupBuilder::EqualNumbers(
    /* [in] */ const String& number1,
    /* [in] */ const String& number2)
{
    assert(0 && "TODO");
    // if (PhoneNumberHelper::IsUriNumber(number1) || PhoneNumberHelper::IsUriNumber(number2)) {
    //     return CompareSipAddresses(number1, number2);
    // }
    // else {
    //     return PhoneNumberUtils::Compare(number1, number2);
    // }
    return FALSE;
}

Boolean CCallLogGroupBuilder::CompareSipAddresses(
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

Int32 CCallLogGroupBuilder::GetDayGroup(
    /* [in] */ Int64 date,
    /* [in] */ Int64 now)
{
    Int32 days;
    assert(0 && "TODO");
    // days = DateUtils::GetDayDifference(TIME, date, now);

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
