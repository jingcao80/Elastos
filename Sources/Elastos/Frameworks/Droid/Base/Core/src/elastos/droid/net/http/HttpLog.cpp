
#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::ILog;

using Elastos::Core::ISystem;
using Elastos::Core::IThread;
using Elastos::Core::Thread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

const String HttpLog::LOGTAG("http");
const Boolean HttpLog::DEBUG = FALSE;
const Boolean HttpLog::LOGV = FALSE;

ECode HttpLog::V(
    /* [in] */ const char* fmt, ...)
{
    AutoPtr<IThread> thread = Thread::GetCurrentThread();
    String name;
    thread->GetName(&name);
    String sFormat("%d %s ");
    va_list ap;
    va_start(ap, fmt);
    sFormat.Append(fmt);
    Logger::V(LOGTAG, sFormat.string(), SystemClock::GetUptimeMillis(), name.string(), ap);
    va_end(ap);
    return NOERROR;
}

ECode HttpLog::E(
    /* [in] */ const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    Logger::E(LOGTAG, fmt, ap);
    va_end(ap);
    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
