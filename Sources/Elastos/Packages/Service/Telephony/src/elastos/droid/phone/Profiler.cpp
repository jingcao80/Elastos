
#include "elastos/droid/phone/Profiler.h"

namespace Elastos {
namespace Droid {
namespace Phone {

static const String Profiler::LOG_TAG = PhoneGlobals::LOG_TAG;

static const Boolean Profiler::PROFILE = FALSE;

static Int64 Profiler::sTimeCallScreenRequested;
static Int64 Profiler::sTimeCallScreenOnCreate;
static Int64 Profiler::sTimeCallScreenCreated;

static Int64 Profiler::sTimeIncomingCallPanelRequested;
static Int64 Profiler::sTimeIncomingCallPanelOnCreate;
static Int64 Profiler::sTimeIncomingCallPanelCreated;

ECode Profiler::ProfileViewCreate(
    /* [in] */ IWindow* win,
    /* [in] */ const String& tag)
{
    if (FALSE) {
        assert(0);
        // ViewParent p = (ViewParent) win.getDecorView();
        // while (p instanceof View) {
        //     p = ((View) p).getParent();
        // }
        //((ViewRoot)p).profile();
        //((ViewRoot)p).setProfileTag(tag);
    }
    return NOERROR;
}

ECode Profiler::CallScreenRequested()
{
    if (PROFILE) {
        sTimeCallScreenRequested = SystemClock::GetUptimeMillis();
    }
    return NOERROR;
}

ECode Profiler::CallScreenOnCreate()
{
    if (PROFILE) {
        sTimeCallScreenOnCreate = SystemClock::GetUptimeMillis();
    }
    return NOERROR;
}

ECode Profiler::CallScreenCreated()
{
    if (PROFILE) {
        sTimeCallScreenCreated = SystemClock::GetUptimeMillis();
        DumpCallScreenStat();
    }
    return NOERROR;
}

void Profiler::DumpCallScreenStat()
{
    if (PROFILE) {
        Log(String(">>> call screen perf stats <<<"));

        StringBuilder sb;
        sb += ">>> request -> onCreate = ";
        sb += sTimeCallScreenOnCreate - sTimeCallScreenRequested;
        Log(sb.ToString());

        StringBuilder sb2;
        sb2 += ">>> onCreate -> created = ";
        sb2 += sTimeCallScreenCreated - sTimeCallScreenOnCreate;
        Log(sb2.ToString());
    }
}

ECode Profiler::IncomingCallPanelRequested()
{
    if (PROFILE) {
        sTimeIncomingCallPanelRequested = SystemClock::GetUptimeMillis();
    }
    return NOERROR;
}

ECode Profiler::IncomingCallPanelOnCreate()
{
    if (PROFILE) {
        sTimeIncomingCallPanelOnCreate = SystemClock::GetUptimeMillis();
    }
    return NOERROR;
}

ECode Profiler::IncomingCallPanelCreated()
{
    if (PROFILE) {
        sTimeIncomingCallPanelCreated = SystemClock::GetUptimeMillis();
        dumpIncomingCallPanelStat();
    }
    return NOERROR;
}

void Profiler::Log(
    /* [in] */ const String& msg)
{
    StringBuilder sb;
    sb += "[Profiler] ";
    sb += msg
    Logger::D(LOG_TAG, sb.ToString());
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos