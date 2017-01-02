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

#include "CMonkeyKeyEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkeyKeyEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyKeyEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyKeyEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyKeyEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyKeyEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyKeyEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyKeyEvent::GetKeyCode(
    /* [out] */ Int32 *keyCode)
{
    VALIDATE_NOT_NULL(keyCode)
    *keyCode = MonkeyKeyEvent::GetKeyCode();
    return NOERROR;
}

ECode CMonkeyKeyEvent::GetAction(
    /* [out] */ Int32 *action)
{
    VALIDATE_NOT_NULL(action)
    *action = MonkeyKeyEvent::GetAction();
    return NOERROR;
}

ECode CMonkeyKeyEvent::GetDownTime(
    /* [out] */ Int64 *downTime)
{
    VALIDATE_NOT_NULL(downTime)
    *downTime = MonkeyKeyEvent::GetDownTime();
    return NOERROR;
}

ECode CMonkeyKeyEvent::GetEventTime(
    /* [out] */ Int64 *eventTime)
{
    VALIDATE_NOT_NULL(eventTime)
    *eventTime = MonkeyKeyEvent::GetEventTime();
    return NOERROR;
}

ECode CMonkeyKeyEvent::SetDownTime(
    /* [in] */ Int64 downTime)
{
    MonkeyKeyEvent::SetDownTime(downTime);
    return NOERROR;
}

ECode CMonkeyKeyEvent::SetEventTime(
    /* [in] */ Int64 eventTime)
{
    MonkeyKeyEvent::SetEventTime(eventTime);
    return NOERROR;
}

ECode CMonkeyKeyEvent::constructor(
    /* [in] */ Int32 action,
    /* [in] */ Int32 keyCode)
{
    MonkeyKeyEvent::Init(action, keyCode);
    return NOERROR;
}

ECode CMonkeyKeyEvent::constructor(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 repeatCount,
    /* [in] */ Int32 metaState,
    /* [in] */ Int32 device,
    /* [in] */ Int32 scanCode)
{
    MonkeyKeyEvent::Init(downTime, eventTime, action,
        keyCode, repeatCount, metaState, device, scanCode);
    return NOERROR;
}

ECode CMonkeyKeyEvent::constructor(
    /* [in] */IKeyEvent *keyEvent)
{
    MonkeyKeyEvent::Init(keyEvent);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos