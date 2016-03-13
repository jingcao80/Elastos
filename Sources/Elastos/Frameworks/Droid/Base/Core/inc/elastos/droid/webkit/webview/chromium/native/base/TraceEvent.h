
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_TRACEEVENT_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_TRACEEVENT_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Os::IIdleHandler;
// import android.os.SystemClock;
// import android.util.Log;
using Elastos::Droid::Utility::IPrinter;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * Java mirror of Chrome trace event API. See base/debug/trace_event.h.  Unlike the native version,
 * Java does not have stack objects, so a TRACE_EVENT() which does both TRACE_EVENT_BEGIN() and
 * TRACE_EVENT_END() in ctor/dtor is not possible.
 * It is OK to use tracing before the native library has loaded, but such traces will
 * be ignored. (Perhaps we could devise to buffer them up in future?).
 */
//@JNINamespace("base::android")
class TraceEvent
    : public Object
{
public:
    class BasicLooperMonitor
        : public Object
        , public IPrinter
    {
    public:
        CAR_INTERFACE_DECL();

        //@Override
        CARAPI Println(
            /* [in] */ const String& line);

        virtual CARAPI_(void) BeginHandling(
            /* [in] */ const String& line);

        virtual CARAPI_(void) EndHandling(
            /* [in] */ const String& line);
    };

    friend AutoPtr<BasicLooperMonitor> BasicLooperMonitor_Create();

private:
    /**
     * A class that records, traces and logs statistics about the UI thead's Looper.
     * The output of this class can be used in a number of interesting ways:
     * <p>
     * <ol><li>
     * When using chrometrace, there will be a near-continuous line of
     * measurements showing both event dispatches as well as idles;
     * </li><li>
     * Logging messages are output for events that run too long on the
     * event dispatcher, making it easy to identify problematic areas;
     * </li><li>
     * Statistics are output whenever there is an idle after a non-trivial
     * amount of activity, allowing information to be gathered about task
     * density and execution cadence on the Looper;
     * </li></ol>
     * <p>
     * The class attaches itself as an idle handler to the main Looper, and
     * monitors the execution of events and idle notifications. Task counters
     * accumulate between idle notifications and get reset when a new idle
     * notification is received.
     */
    class IdleTracingLooperMonitor
        : public BasicLooperMonitor
        , public IIdleHandler
    {
    public:
        IdleTracingLooperMonitor();

        CAR_INTERFACE_DECL();

        //@Override
        CARAPI QueueIdle(
            /* [out] */ Boolean* result);

        //@Override
        CARAPI_(void) BeginHandling(
            /* [in] */ const String& line);

        //@Override
        CARAPI_(void) EndHandling(
            /* [in] */ const String& line);

    private:
        // Called from within the begin/end methods only.
        // This method can only execute on the looper thread, because that is
        // the only thread that is permitted to call Looper.myqueue().
        CARAPI_(void) SyncIdleMonitoring();

        static CARAPI_(void) TraceAndLog(
            /* [in] */ Int32 level,
            /* [in] */ const String& message);

    private:
        // Tags for dumping to logcat or TraceEvent
        static const String TAG;
        static const String IDLE_EVENT_NAME;

        // Calculation constants
        static const Int64 FRAME_DURATION_MILLIS = 1000L / 60L; // 60 FPS
        // A reasonable threshold for defining a Looper event as "long running"
        static const Int64 MIN_INTERESTING_DURATION_MILLIS =
                FRAME_DURATION_MILLIS;
        // A reasonable threshold for a "burst" of tasks on the Looper
        static const Int64 MIN_INTERESTING_BURST_DURATION_MILLIS =
                MIN_INTERESTING_DURATION_MILLIS * 3;

        // Stats tracking
        Int64 mLastIdleStartedAt;
        Int64 mLastWorkStartedAt;
        Int32 mNumTasksSeen;
        Int32 mNumIdlesSeen;
        Int32 mNumTasksSinceLastIdle;

        // State
        Boolean mIdleMonitorAttached;
    };

    // Holder for monitor avoids unnecessary construction on non-debug runs
    class LooperMonitorHolder
    {
    public:
        static const AutoPtr<BasicLooperMonitor> sInstance;
    };

public:
    /**
     * Register an enabled observer, such that java traces are always enabled with native.
     */
    static CARAPI_(void) RegisterNativeEnabledObserver();

    /**
     * Notification from native that tracing is enabled/disabled.
     */
    //@CalledByNative
    static CARAPI_(void) SetEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Enables or disabled Android systrace path of Chrome tracing. If enabled, all Chrome
     * traces will be also output to Android systrace. Because of the overhead of Android
     * systrace, this is for WebView only.
     */
    static CARAPI_(void) SetATraceEnabled(
        /* [in] */ Boolean enabled);

    /**
     * @return True if tracing is enabled, false otherwise.
     * It is safe to call trace methods without checking if TraceEvent
     * is enabled.
     */
    static CARAPI_(Boolean) Enabled();

    /**
     * Triggers the 'instant' native trace event with no arguments.
     * @param name The name of the event.
     */
    static CARAPI_(void) Instant(
        /* [in] */ const String& name);

    /**
     * Triggers the 'instant' native trace event.
     * @param name The name of the event.
     * @param arg  The arguments of the event.
     */
    static CARAPI_(void) Instant(
        /* [in] */ const String& name,
        /* [in] */ const String& arg);

    /**
     * Convenience wrapper around the versions of startAsync() that take string parameters.
     * @param id The id of the asynchronous event.
     * @see #begin()
     */
    static CARAPI_(void) StartAsync(
        /* [in] */ Int64 id);

    /**
     * Triggers the 'start' native trace event with no arguments.
     * @param name The name of the event.
     * @param id   The id of the asynchronous event.
     */
    static CARAPI_(void) StartAsync(
        /* [in] */ const String& name,
        /* [in] */ Int64 id);

    /**
     * Triggers the 'start' native trace event.
     * @param name The name of the event.
     * @param id   The id of the asynchronous event.
     * @param arg  The arguments of the event.
     */
    static CARAPI_(void) StartAsync(
        /* [in] */ const String& name,
        /* [in] */ Int64 id,
        /* [in] */ const String& arg);

    /**
     * Convenience wrapper around the versions of finishAsync() that take string parameters.
     * @param id The id of the asynchronous event.
     * @see #begin()
     */
    static CARAPI_(void) FinishAsync(
        /* [in] */ Int64 id);

    /**
     * Triggers the 'finish' native trace event with no arguments.
     * @param name The name of the event.
     * @param id   The id of the asynchronous event.
     */
    static CARAPI_(void) FinishAsync(
        /* [in] */ const String& name,
        /* [in] */ Int64 id);

    /**
     * Triggers the 'finish' native trace event.
     * @param name The name of the event.
     * @param id   The id of the asynchronous event.
     * @param arg  The arguments of the event.
     */
    static CARAPI_(void) FinishAsync(
        /* [in] */ const String& name,
        /* [in] */ Int64 id,
        /* [in] */ const String& arg);

    /**
     * Convenience wrapper around the versions of begin() that take string parameters.
     * The name of the event will be derived from the class and function name that call this.
     * IMPORTANT: if using this version, ensure end() (no parameters) is always called from the
     * same calling context.
     *
     * Note that the overhead is at ms or sub-ms order. Don't use this when millisecond accuracy
     * is desired.
     */
    static CARAPI_(void) Begin();

    /**
     * Triggers the 'begin' native trace event with no arguments.
     * @param name The name of the event.
     */
    static CARAPI_(void) Begin(
        /* [in] */ const String& name);

    /**
     * Triggers the 'begin' native trace event.
     * @param name The name of the event.
     * @param arg  The arguments of the event.
     */
    static CARAPI_(void) Begin(
        /* [in] */ const String& name,
        /* [in] */ const String& arg);

    /**
     * Convenience wrapper around the versions of end() that take string parameters.
     * @see #begin()
     */
    static CARAPI_(void) End();

    /**
     * Triggers the 'end' native trace event with no arguments.
     * @param name The name of the event.
     */
    static CARAPI_(void) End(
        /* [in] */ const String& name);

    /**
     * Triggers the 'end' native trace event.
     * @param name The name of the event.
     * @param arg  The arguments of the event.
     */
    static CARAPI_(void) End(
        /* [in] */ const String& name,
        /* [in] */ const String& arg);

    static CARAPI_(void*) ElaTraceEventCallback_Init();

private:
    static CARAPI_(String) GetCallerName();

    static CARAPI_(void) NativeRegisterEnabledObserver();

    static CARAPI_(void) NativeStartATrace();

    static CARAPI_(void) NativeStopATrace();

    static CARAPI_(void) NativeInstant(
        /* [in] */ const String& name,
        /* [in] */ const String& arg);

    static CARAPI_(void) NativeBegin(
        /* [in] */ const String& name,
        /* [in] */ const String& arg);

    static CARAPI_(void) NativeEnd(
        /* [in] */ const String& name,
        /* [in] */ const String& arg);

    static CARAPI_(void) NativeBeginToplevel();

    static CARAPI_(void) NativeEndToplevel();

    static CARAPI_(void) NativeStartAsync(
        /* [in] */ const String& name,
        /* [in] */ Int64 id,
        /* [in] */ const String& arg);

    static CARAPI_(void) NativeFinishAsync(
        /* [in] */ const String& name,
        /* [in] */ Int64 id,
        /* [in] */ const String& arg);

private:
    static volatile Boolean sEnabled;
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_TRACEEVENT_H__
