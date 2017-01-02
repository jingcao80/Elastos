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

#include "MonkeyWaitEvent.h"
#include <unistd.h>
#define msleep(X) usleep(X * 1000)

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyWaitEvent::MonkeyWaitEvent()
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_THROTTLE)
{
}

MonkeyWaitEvent::MonkeyWaitEvent(
    /* [in] */ Int64 waitTime)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_THROTTLE)
    , mWaitTime(waitTime)
{
}

ECode MonkeyWaitEvent::Init(
    /* [in] */ Int64 waitTime)
{
    mWaitTime = waitTime;
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_THROTTLE);
    return NOERROR;
}

Int32 MonkeyWaitEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose)
{
    if (verbose > 1) {
        PFL_EX("Wait Event For %lld  milliseconds", mWaitTime)
    }

    msleep(mWaitTime);
    return IMonkeyEvent::INJECT_SUCCESS;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

