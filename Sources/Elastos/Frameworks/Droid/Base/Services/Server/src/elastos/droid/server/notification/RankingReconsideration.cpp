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

#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/server/notification/RankingReconsideration.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
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
        {    AutoLock syncLock(this);
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
