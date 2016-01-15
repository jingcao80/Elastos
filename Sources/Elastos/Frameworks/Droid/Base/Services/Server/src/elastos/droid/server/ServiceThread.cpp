#include "elastos/droid/server/ServiceThread.h"
#include <elastos/droid/os/Process.h>
// #include <elastos/droid/os/StrictMode.h>

// using Elastos::Droid::Os::StrictMode;
using Elastos::Droid::Os::Process;

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

    assert(0 && "TODO");
    // For debug builds, log event loop stalls to dropbox for analysis.
    // if (!mAllowIo && StrictMode::ConditionallyEnableDebugLogging()) {
    //     Slogger::I(TAG, "Enabled StrictMode logging for %s looper.");
    // }

    return HandlerThread::Run();
}

} // namespace Server
} // namepsace Droid
} // namespace Elastos
