#include "elastos/droid/server/ServiceThread.h"
#include <elastos/droid/os/Process.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Os.h>

using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Os::CStrictMode;
using Elastos::Droid::Os::Process;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String ServiceThread::TAG("ServiceThread");

ServiceThread::ServiceThread(
    /* [in] */ const String& name,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean allowIo)
    : HandlerThread(name, priority)
    , mAllowIo(allowIo)
{
}

ECode ServiceThread::Run()
{
    Process::SetCanSelfBackground(FALSE);

    // For debug builds, log event loop stalls to dropbox for analysis.
    AutoPtr<IStrictMode> strictMode;
    CStrictMode::AcquireSingleton((IStrictMode**)&strictMode);
    Boolean bval;
    if (!mAllowIo && (strictMode->ConditionallyEnableDebugLogging(&bval), bval)) {
        String name;
        GetName(&name);
        Slogger::I(TAG, "Enabled StrictMode logging for %s looper.", name.string());
    }

    return HandlerThread::Run();
}

} // namespace Server
} // namepsace Droid
} // namespace Elastos
