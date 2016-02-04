
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/Thread.h>
#include <utils/SystemClock.h>

using Elastos::Core::Thread;

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

Boolean SystemClock::SetCurrentTimeMillis(Int64 millis)
{
    assert(0);
    // TODO
    // IBinder b = ServiceManager.getService(Context.ALARM_SERVICE);
    // IAlarmManager mgr = IAlarmManager.Stub.asInterface(b);
    // if (mgr == null) {
    //     return false;
    // }

    // try {
    //     return mgr.setTime(millis);
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Unable to set RTC", e);
    // } catch (SecurityException e) {
    //     Slog.e(TAG, "Unable to set RTC", e);
    // }
    // return false;
    return FALSE;
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
