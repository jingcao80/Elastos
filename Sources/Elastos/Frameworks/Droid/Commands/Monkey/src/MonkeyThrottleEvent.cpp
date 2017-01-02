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

#include "MonkeyThrottleEvent.h"
#include <unistd.h>

#define msleep(X) usleep(X * 1000)

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyThrottleEvent::MonkeyThrottleEvent()
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_THROTTLE)
    , mThrottle(0)
{}

MonkeyThrottleEvent::MonkeyThrottleEvent(
    /* [in] */ Int64 throttle)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_THROTTLE)
    , mThrottle(throttle)
{}

CARAPI MonkeyThrottleEvent::Init(
    /* [in] */ Int64 throttle)
{
    mThrottle = throttle;
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_THROTTLE);
    return NOERROR;
}

Int32 MonkeyThrottleEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose)
{
    if (verbose > 0) {
        PFL_EX("Sleeping for %lld milliseconds", mThrottle)
    }

    msleep(mThrottle);
    return IMonkeyEvent::INJECT_SUCCESS;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

