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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_DEBUGTRIGGER_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_DEBUGTRIGGER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Misc {

/**
 * A trigger for catching a debug chord.
 * We currently use volume up then volume down to trigger this mode.
 */
class DebugTrigger
    : public Object
{
public:
    DebugTrigger(
        /* [in] */ IRunnable* triggeredRunnable);

    /** Resets the debug trigger */
    CARAPI_(void) Reset();

    /**
     * Processes a key event and tests if it is a part of the trigger. If the chord is complete,
     * then we just call the callback.
     */
    CARAPI_(void) OnKeyEvent(
        /* [in] */ Int32 keyCode);

public:
    AutoPtr<IHandler> mHandler;
    AutoPtr<IRunnable> mTriggeredRunnable;

    Int32 mLastKeyCode;
    Int64 mLastKeyCodeTime;

};

} // namespace Misc
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_DEBUGTRIGGER_H__
