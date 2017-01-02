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

#include "MonkeyEventQueue.h"
#include "CMonkeyThrottleEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode MonkeyEventQueue::IsEmpty(
    /* [out] */ Boolean *isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty)
    *isEmpty = mList.IsEmpty();
    return NOERROR;
}

ECode MonkeyEventQueue::RemoveFirst()
{
    mList.PopFront();
    return NOERROR;
}

ECode MonkeyEventQueue::GetFirst(
    /* [out] */ IMonkeyEvent **monkeyEvent)
{
    VALIDATE_NOT_NULL(monkeyEvent)
    AutoPtr<IMonkeyEvent> temp = mList.GetFront();
    *monkeyEvent = temp;
    REFCOUNT_ADD(*monkeyEvent)
    return NOERROR;
}

ECode MonkeyEventQueue::AddLast(
    /* [in] */ IMonkeyEvent *event)
{
    mList.PushBack(event);
    Boolean isThrottlable = FALSE;
    event->IsThrottlable(&isThrottlable);
    if (isThrottlable) {
        Int64 throttle = mThrottle;
        if (mRandomizeThrottle && mThrottle > 0) {
            mRandom->NextInt64(&throttle);
            if (throttle < 0) {
                throttle = -throttle;
            }
            throttle %= mThrottle;
            ++throttle;
        }
        AutoPtr<IMonkeyThrottleEvent> e;
        CMonkeyThrottleEvent::New(throttle, (IMonkeyThrottleEvent**)&e);
        mList.PushBack(e);
    }
    return NOERROR;
}

ECode MonkeyEventQueue::Init(
    /* [in] */ IRandom *random,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle)
{
    mRandom = random;
    mThrottle = throttle;
    mRandomizeThrottle = randomizeThrottle;
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

