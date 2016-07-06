
#include "elastos/droid/teleservice/phone/Profiler.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewParent;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

const String Profiler::TAG = IPhoneGlobals::TAG;

const Boolean Profiler::PROFILE = FALSE;

Int64 Profiler::sTimeCallScreenRequested;
Int64 Profiler::sTimeCallScreenOnCreate;
Int64 Profiler::sTimeCallScreenCreated;

Int64 Profiler::sTimeIncomingCallPanelRequested;
Int64 Profiler::sTimeIncomingCallPanelOnCreate;
Int64 Profiler::sTimeIncomingCallPanelCreated;

ECode Profiler::ProfileViewCreate(
    /* [in] */ IWindow* win,
    /* [in] */ const String& tag)
{
    if (FALSE) {
        AutoPtr<IView> view;
        win->GetDecorView((IView**)&view);
        AutoPtr<IViewParent> p = IViewParent::Probe(view);
        while (IView::Probe(p) != NULL) {
            IView::Probe(p)->GetParent((IViewParent**)&p);
        }
        // IViewRoot::Probe(p)->Profile();
        // IViewRoot::Probe(p)->SetProfileTag(tag);
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
        DumpIncomingCallPanelStat();
    }
    return NOERROR;
}

void Profiler::Log(
    /* [in] */ const String& msg)
{
    StringBuilder sb;
    sb += "[Profiler] ";
    sb += msg;
    Logger::D(TAG, sb.ToString());
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos