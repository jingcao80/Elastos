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

#include "MonkeyKeyEvent.h"
#include "elastos/droid/os/SystemClock.h"
#include "CMonkeySourceRandomHelper.h"

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::IInputManagerHelper;
using Elastos::Droid::Hardware::Input::CInputManagerHelper;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyKeyEvent::MonkeyKeyEvent()
    : mDeviceId(0)
    , mEventTime(0)
    , mDownTime(0)
    , mAction(0)
    , mKeyCode(0)
    , mScanCode(0)
    , mMetaState(0)
    , mRepeatCount(0)
{}

MonkeyKeyEvent::MonkeyKeyEvent(
    /* [in] */ Int32 action,
    /* [in] */ Int32 keyCode)
{
    Init(-1, -1, action, keyCode, 0, 0, IKeyCharacterMap::VIRTUAL_KEYBOARD, 0);
}

MonkeyKeyEvent::MonkeyKeyEvent(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 repeatCount,
    /* [in] */ Int32 metaState,
    /* [in] */ Int32 device,
    /* [in] */ Int32 scanCode)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_KEY)
    , mDeviceId(device)
    , mEventTime(eventTime)
    , mDownTime(downTime)
    , mAction(action)
    , mKeyCode(keyCode)
    , mScanCode(scanCode)
    , mMetaState(metaState)
    , mRepeatCount(repeatCount)
{}

MonkeyKeyEvent::MonkeyKeyEvent(
    /* [in] */ IKeyEvent *pKeyEvent)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_KEY)
    , mKeyEvent(pKeyEvent)
{}

ECode MonkeyKeyEvent::Init(
    /* [in] */ Int32 action,
    /* [in] */ Int32 keyCode)
{
    Init(-1, -1, action, keyCode, 0, 0, IKeyCharacterMap::VIRTUAL_KEYBOARD, 0);
    return NOERROR;
}

ECode MonkeyKeyEvent::Init(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 repeatCount,
    /* [in] */ Int32 metaState,
    /* [in] */ Int32 device,
    /* [in] */ Int32 scanCode)
{
    mDownTime = downTime;
    mEventTime = eventTime;
    mAction = action;
    mKeyCode = keyCode;
    mRepeatCount = repeatCount;
    mMetaState = metaState;
    mDeviceId = device;
    mScanCode = scanCode;
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_KEY);
    return NOERROR;
}

ECode MonkeyKeyEvent::Init(
    /* [in] */ IKeyEvent *pKeyEvent)
{
    mKeyEvent = pKeyEvent;
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_KEY);
    return NOERROR;
}

Int32 MonkeyKeyEvent::GetKeyCode()
{
    Int32 keyCode;
    if(mKeyEvent != NULL)
    {
        mKeyEvent->GetKeyCode(&keyCode);
        return keyCode;
    }
    return mKeyCode;
}

Int32 MonkeyKeyEvent::GetAction()
{
    Int32 action;
    if(mKeyEvent != NULL)
    {
        mKeyEvent->GetAction(&action);
        return action;
    }
    return mAction;
}

Int64 MonkeyKeyEvent::GetDownTime()
{
    Int64 downtime;
    if(mKeyEvent != NULL)
    {
        mKeyEvent->GetDownTime(&downtime);
        return downtime;
    }
    return mDownTime;
}

Int64 MonkeyKeyEvent::GetEventTime()
{
    Int64 eventtime;
    if(mKeyEvent != NULL)
    {
        mKeyEvent->GetEventTime(&eventtime);
        return eventtime;
    }
    return mEventTime;
}

ECode MonkeyKeyEvent::SetDownTime(
    /* [in] */ Int64 downTime)
{
    mDownTime = downTime;
    return NOERROR;
}

ECode MonkeyKeyEvent::SetEventTime(
    /* [in] */ Int64 eventTime)
{
    mEventTime = eventTime;
    return NOERROR;
}

Boolean MonkeyKeyEvent::IsThrottlable()
{
    return (GetAction() == IKeyEvent::ACTION_UP);
}

Int32 MonkeyKeyEvent::InjectEvent(
    /* [in] */ IIWindowManager* iwm,
    /* [in] */ IIActivityManager* iam,
    /* [in] */ Int32 verbose)
{
    if (verbose > 1) {
        String note;
        if (mAction == IKeyEvent::ACTION_UP) {
            note = String("ACTION_UP");
        }
        else {
            note = String("ACTION_DOWN");
        }

        AutoPtr<IMonkeySourceRandomHelper> helper;
        CMonkeySourceRandomHelper::AcquireSingleton((IMonkeySourceRandomHelper**)&helper);
        String keyName;
        ASSERT_SUCCEEDED(helper->GetKeyName(mKeyCode, &keyName))
        PFL_EX(":Sending Key ( %s ): %d     // %s", note.string(), mKeyCode, keyName.string());

        AutoPtr<IKeyEvent> keyEvent = mKeyEvent;
        if(keyEvent.Get() == NULL){
            Int64 eventTime = mEventTime;
            if(eventTime <= 0) {
                eventTime = SystemClock::GetUptimeMillis();
            }
            Int64 downTime = mDownTime;
            if(downTime <= 0) {
                downTime = eventTime;
            }
            CKeyEvent::New(downTime, eventTime, mAction, mKeyCode
                , mRepeatCount, mMetaState, mDeviceId, mScanCode
                , IKeyEvent::FLAG_FROM_SYSTEM, IInputDevice::SOURCE_KEYBOARD, (IKeyEvent**)&keyEvent);
        }

        Boolean ret = FALSE;
        AutoPtr<IInputManagerHelper> imHelper;
        CInputManagerHelper::AcquireSingleton((IInputManagerHelper**)&imHelper);
        AutoPtr<IInputManager> inputManager;
        imHelper->GetInstance((IInputManager**)&inputManager);
        inputManager->InjectInputEvent(keyEvent, IInputManager::INJECT_INPUT_EVENT_MODE_WAIT_FOR_RESULT, &ret);
        if (!ret)
            return IMonkeyEvent::INJECT_FAIL;
    }
    return IMonkeyEvent::INJECT_SUCCESS;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

