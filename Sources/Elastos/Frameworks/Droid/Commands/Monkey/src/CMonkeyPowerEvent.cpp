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

#include "CMonkeyPowerEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkeyPowerEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyPowerEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyPowerEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyPowerEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyPowerEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyPowerEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyPowerEvent::constructor(
    /* [in] */ const String& powerLogTag,
    /* [in] */ const String& powerTestResult)
{
    MonkeyPowerEvent::Init(powerLogTag, powerTestResult);
    return NOERROR;
}

ECode CMonkeyPowerEvent::constructor(
    /* [in] */ const String& powerLogTag)
{
    MonkeyPowerEvent::Init(powerLogTag);
    return NOERROR;
}

ECode CMonkeyPowerEvent::constructor()
{
    MonkeyPowerEvent::Init();
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
