
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/server/notification/RankingReconsideration.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

const Int64 RankingReconsideration::IMMEDIATE = 0;

const Int32 RankingReconsideration::START = 0;
const Int32 RankingReconsideration::RUNNING = 1;
const Int32 RankingReconsideration::DONE = 2;
const Int32 RankingReconsideration::CANCELLED = 3;

CAR_INTERFACE_IMPL(RankingReconsideration, Runnable, IRankingReconsideration);

RankingReconsideration::RankingReconsideration(
    /* [in] */ const String& key)
    : mState(0)
    , mDelay(0)
{
    /* RankingReconsideration(key, IMMEDIATE);*/
    mDelay = IMMEDIATE;
    mKey = key;
    mState = START;
}

RankingReconsideration::RankingReconsideration(
    /* [in] */ const String& key,
    /* [in] */ Int64 delay)
    : mState(0)
    , mDelay(0)
{
    mDelay = delay;
    mKey = key;
    mState = START;
}

RankingReconsideration::~RankingReconsideration()
{}

String RankingReconsideration::GetKey()
{
    return mKey;
}

ECode RankingReconsideration::Run()
{
    if (mState == START) {
        mState = RUNNING;

        Work();

        mState = DONE;
        synchronized(this) {
            NotifyAll();
        }
    }
    return NOERROR;
}

Int64 RankingReconsideration::GetDelay(
    /* [in] */ ITimeUnit* unit)
{
    AutoPtr<ITimeUnitHelper> helper;
    CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&helper);
    AutoPtr<ITimeUnit> timeUnit;
    helper->GetMILLISECONDS((ITimeUnit**)&timeUnit);
    Int64 result;
    unit->Convert(mDelay, timeUnit, &result);
    return result;
}

Boolean RankingReconsideration::Cancel(
    /* [in] */ Boolean mayInterruptIfRunning)
{
    if (mState == START) {  // can't cancel if running or done
        mState = CANCELLED;
        return TRUE;
    }
    return FALSE;
}

Boolean RankingReconsideration::IsCancelled()
{
    return mState == CANCELLED;
}

Boolean RankingReconsideration::IsDone()
{
    return mState == DONE;
}

} // Notification
} // Server
} // Droid
} // Elastos
