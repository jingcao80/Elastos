
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Slogger.h>
#include <utils/SystemClock.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::App::IIAlarmManager;
using Elastos::Core::Thread;
using Elastos::Utility::Logging::Slogger;


namespace Elastos {
namespace Droid {
namespace Os {

void SystemClock::Sleep(
    /* [in] */ Int64 ms)
{
    Int64 start = GetUptimeMillis();
    Int64 duration = ms;
    Boolean interrupted = FALSE;
    do {
        // try {
        ECode ec = Thread::Sleep(duration);
        if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
            interrupted = TRUE;
        }
        // }
        // catch (InterruptedException e) {
        //     interrupted = true;
        // }
        duration = start + ms - GetUptimeMillis();
    } while (duration > 0);

    if (interrupted) {
        // Important: we don't want to quietly eat an interrupt() event,
        // so we make sure to re-interrupt the thread so that the next
        // call to Thread.sleep() or Object.wait() will be interrupted.
        Thread::GetCurrentThread()->Interrupt();
    }
}

Boolean SystemClock::SetCurrentTimeMillis(
    /* [in] */ Int64 millis)
{
    AutoPtr<IInterface> service = ServiceManager::GetService(IContext::ALARM_SERVICE);
    AutoPtr<IIAlarmManager> mgr = IIAlarmManager::Probe(service);
    if (mgr == NULL) {
        return FALSE;
    }

    Boolean bval = FALSE;
    ECode ec = mgr->SetTime(millis, &bval);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::E("SystemClock", "Unable to set RTC, E_REMOTE_EXCEPTION");
    }
    else if (ec == (ECode)E_SECURITY_EXCEPTION) {
        Slogger::E("SystemClock", "Unable to set RTC, E_SECURITY_EXCEPTION");
    }
    return bval;
}

Int64 SystemClock::GetUptimeMillis()
{
    return (Int64)android::uptimeMillis();
}

Int64 SystemClock::GetElapsedRealtime()
{
    return (Int64)android::elapsedRealtime();
}

Int64 SystemClock::GetElapsedRealtimeNanos()
{
    return (Int64)android::elapsedRealtimeNano();
}

Int64 SystemClock::GetCurrentThreadTimeMillis()
{
#if defined(HAVE_POSIX_CLOCKS)
    struct timespec tm;

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tm);

    return tm.tv_sec * 1000LL + tm.tv_nsec / 1000000;
#else
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000LL + tv.tv_usec / 1000;
#endif
}

Int64 SystemClock::GetCurrentThreadTimeMicro()
{
#if defined(HAVE_POSIX_CLOCKS)
    struct timespec tm;

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tm);

    return tm.tv_sec * 1000000LL + tm.tv_nsec / 1000;
#else
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000LL + tv.tv_nsec / 1000;
#endif
}

Int64 SystemClock::GetCurrentTimeMicro()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000LL + tv.tv_usec;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
