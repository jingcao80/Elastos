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