#ifndef __ELASTOS_DROID_OS_SYSTEMCLOCK_H__
#define __ELASTOS_DROID_OS_SYSTEMCLOCK_H__

#include <elatypes.h>

namespace Elastos {
namespace Droid {
namespace Os {

class ECO_PUBLIC SystemClock
{
public:
    /**
     * Waits a given number of milliseconds (of uptimeMillis) before returning.
     * Similar to {@link java.lang.Thread#sleep(long)}, but does not throw
     * {@link InterruptedException}; {@link Thread#interrupt()} events are
     * deferred until the next interruptible operation.  Does not return until
     * at least the specified number of milliseconds has elapsed.
     *
     * @param ms to sleep before returning, in milliseconds of uptime.
     */
    static CARAPI_(void) Sleep(
        /* [in] */ Int64 ms);

    /**
     * Sets the current wall time, in milliseconds.  Requires the calling
     * process to have appropriate permissions.
     *
     * @return if the clock was successfully set to the specified time.
     */
    static CARAPI_(Boolean) SetCurrentTimeMillis(
        /* [in] */ Int64 millis);

    /**
     * Returns milliseconds since boot, not counting time spent in deep sleep.
     * <b>Note:</b> This value may get reset occasionally (before it would
     * otherwise wrap around).
     *
     * @return milliseconds of non-sleep uptime since boot.
     */
    static CARAPI_(Int64) GetUptimeMillis();

    /**
     * Returns milliseconds since boot, including time spent in sleep.
     *
     * @return elapsed milliseconds since boot.
     */
    static CARAPI_(Int64) GetElapsedRealtime();

    /**
     * Returns nanoseconds since boot, including time spent in sleep.
     *
     * @return elapsed nanoseconds since boot.
     */
    static CARAPI_(Int64) GetElapsedRealtimeNanos();

    /**
     * Returns milliseconds running in the current thread.
     *
     * @return elapsed milliseconds in the thread
     */
    static CARAPI_(Int64) GetCurrentThreadTimeMillis();

    /**
     * Returns microseconds running in the current thread.
     *
     * @return elapsed microseconds in the thread
     *
     * @hide
     */
    static CARAPI_(Int64) GetCurrentThreadTimeMicro();

    /**
     * Returns current wall time in  microseconds.
     *
     * @return elapsed microseconds in wall time
     *
     * @hide
     */
    static CARAPI_(Int64) GetCurrentTimeMicro();

private:
    ECO_LOCAL SystemClock();
    ECO_LOCAL SystemClock(const SystemClock&);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_SYSTEMCLOCK_H__
