#ifndef  __ELASTOS_DROID_PHONE_PROFILER_H__
#define  __ELASTOS_DROID_PHONE_PROFILER_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::View::IWindow;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Profiling utilities for the Phone app.
 */
class Profiler
{
public:
    static CARAPI ProfileViewCreate(
        /* [in] */ IWindow* win,
        /* [in] */ const String& tag);

    static CARAPI CallScreenRequested();

    static CARAPI CallScreenOnCreate();

    static CARAPI CallScreenCreated();

    static CARAPI IncomingCallPanelRequested();

    static CARAPI IncomingCallPanelOnCreate();

    static CARAPI IncomingCallPanelCreated();

private:
    /** This class is never instantiated. */
    Profiler() {}

    static CARAPI_(void) DumpCallScreenStat();

    static CARAPI_(void) DumpIncomingCallPanelStat();

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;

    // Let the compiler optimize all this code out unless we're actively
    // doing profiling runs.
    // TODO: Instead of doing all these "if (PROFILE)" checks here, every
    // place that *calls* any of these methods should check the value of
    // Profiler.PROFILE first, so the method calls will get optimized out
    // too.
    static const Boolean PROFILE;

    static Int64 sTimeCallScreenRequested;
    static Int64 sTimeCallScreenOnCreate;
    static Int64 sTimeCallScreenCreated;

    // TODO: Clean up any usage of these times.  (There's no "incoming call
    // panel" in the Phone UI any more; incoming calls just go straight to the
    // regular in-call UI.)
    static Int64 sTimeIncomingCallPanelRequested;
    static Int64 sTimeIncomingCallPanelOnCreate;
    static Int64 sTimeIncomingCallPanelCreated;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_PROFILER_H__