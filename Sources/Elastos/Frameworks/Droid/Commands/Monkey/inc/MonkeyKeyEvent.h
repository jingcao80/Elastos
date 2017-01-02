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

#ifndef __MONKEYKEYEVENT_H__
#define __MONKEYKEYEVENT_H__

#include "MonkeyEvent.h"

using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::IInputDevice;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyKeyEvent : public MonkeyEvent
{
protected:
    MonkeyKeyEvent();

    MonkeyKeyEvent(
        /* [in] */ Int32 action,
        /* [in] */ Int32 keyCode);

    MonkeyKeyEvent(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 repeatCount,
        /* [in] */ Int32 metaState,
        /* [in] */ Int32 device,
        /* [in] */ Int32 scanCode);

    MonkeyKeyEvent(
        /* [in] */ IKeyEvent *keyEvent);

    virtual CARAPI_(Int32) InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose);

    CARAPI Init(
        /* [in] */ Int32 action,
        /* [in] */ Int32 keyCode);

    CARAPI Init(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 repeatCount,
        /* [in] */ Int32 metaState,
        /* [in] */ Int32 device,
        /* [in] */ Int32 scanCode);

    CARAPI Init(
        /* [in] */ IKeyEvent *keyEvent);

    CARAPI_(Int32) GetKeyCode();

    CARAPI_(Int32) GetAction();

    CARAPI_(Int64) GetDownTime();

    CARAPI_(Int64) GetEventTime();

    CARAPI SetDownTime(
        /* [in] */ Int64 downTime);

    CARAPI SetEventTime(
        /* [in] */ Int64 eventTime);

    CARAPI_(Boolean) IsThrottlable();

private:
    Int32 mDeviceId;
    Int64 mEventTime;
    Int64 mDownTime;
    Int32 mAction;
    Int32 mKeyCode;
    Int32 mScanCode;
    Int32 mMetaState;
    Int32 mRepeatCount;
    AutoPtr<IKeyEvent> mKeyEvent;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif //__MONKEYKEYEVENT_H__