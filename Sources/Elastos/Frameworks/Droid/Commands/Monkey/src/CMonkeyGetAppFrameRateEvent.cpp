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

#include "CMonkeyGetAppFrameRateEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CAR_OBJECT_IMPL(CMonkeyGetAppFrameRateEvent)

ECode CMonkeyGetAppFrameRateEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyGetAppFrameRateEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyGetAppFrameRateEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyGetAppFrameRateEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyGetAppFrameRateEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyGetAppFrameRateEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyGetAppFrameRateEvent::constructor(
    /* [in] */ const String& status,
    /* [in] */ const String& activityName,
    /* [in] */ const String& testCaseName)
{
    MonkeyGetAppFrameRateEvent::Init(status, activityName, testCaseName);
    return NOERROR;
}

ECode CMonkeyGetAppFrameRateEvent::constructor(
    /* [in] */ const String& status,
    /* [in] */ const String& activityName)
{
    MonkeyGetAppFrameRateEvent::Init(status, activityName);
    return NOERROR;
}

ECode CMonkeyGetAppFrameRateEvent::constructor(
    /* [in] */ const String& status)
{
    MonkeyGetAppFrameRateEvent::Init(status);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
