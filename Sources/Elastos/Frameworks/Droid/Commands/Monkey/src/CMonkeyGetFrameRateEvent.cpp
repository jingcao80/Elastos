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

#include "CMonkeyGetFrameRateEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CAR_OBJECT_IMPL(CMonkeyGetFrameRateEvent)

ECode CMonkeyGetFrameRateEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyGetFrameRateEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyGetFrameRateEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyGetFrameRateEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyGetFrameRateEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyGetFrameRateEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyGetFrameRateEvent::constructor(
    /* [in] */ const String& status,
    /* [in] */ const String& testCaseName)
{
    MonkeyGetFrameRateEvent::Init(status, testCaseName);
    return NOERROR;
}

ECode CMonkeyGetFrameRateEvent::constructor(
    /* [in] */ const String& status)
{
    MonkeyGetFrameRateEvent::Init(status);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
