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

#include "elastos/droid/systemui/recents/misc/DebugTrigger.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Misc {

DebugTrigger::DebugTrigger(
    /* [in] */ IRunnable* triggeredRunnable)
    : mTriggeredRunnable(triggeredRunnable)
    , mLastKeyCode(0)
    , mLastKeyCodeTime(0)
{
    CHandler::New((IHandler**)&mHandler);
}

void DebugTrigger::Reset()
{
    mLastKeyCode = 0;
    mLastKeyCodeTime = 0;
}

void DebugTrigger::OnKeyEvent(
    /* [in] */ Int32 keyCode)
{
    //if (!Constants::DebugFlags::App::EnableDebugMode) return;//TODO

    if (mLastKeyCode == 0) {
        if (keyCode == IKeyEvent::KEYCODE_VOLUME_UP) {
            mLastKeyCode = keyCode;
            mLastKeyCodeTime = SystemClock::GetUptimeMillis();
            return;
        }
    } else {
        if (mLastKeyCode == IKeyEvent::KEYCODE_VOLUME_UP &&
                keyCode == IKeyEvent::KEYCODE_VOLUME_DOWN) {
            if ((SystemClock::GetUptimeMillis() - mLastKeyCodeTime) < 750) {
                mTriggeredRunnable->Run();
            }
        }
    }
    Reset();
}

} // namespace Misc
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos