
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
