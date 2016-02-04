
#include "elastos/droid/webkit/native/base/TraceEvent.h"
#include "elastos/droid/webkit/native/base/api/TraceEvent_dec.h"
#include "elastos/droid/webkit/native/base/CommandLine.h"
#include "elastos/droid/webkit/native/base/BaseSwitches.h"
#include "elastos/droid/webkit/native/base/ThreadUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/CLooperHelper.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::EIID_IIdleHandler;
using Elastos::Droid::Utility::EIID_IPrinter;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Base {

//===============================================================
//                 TraceEvent::BasicLooperMonitor
//===============================================================
CAR_INTERFACE_IMPL(TraceEvent::BasicLooperMonitor, Object, IPrinter);

//@Override
ECode TraceEvent::BasicLooperMonitor::Println(
    /* [in] */ const String& line)
{
    if (line.StartWith(">")) {
        BeginHandling(line);
    }
    else {
        assert(line.StartWith("<"));
        EndHandling(line);
    }

    return NOERROR;
}

void TraceEvent::BasicLooperMonitor::BeginHandling(
    /* [in] */ const String& line)
{
    if (sEnabled) NativeBeginToplevel();
}

void TraceEvent::BasicLooperMonitor::EndHandling(
    /* [in] */ const String& line)
{
    if (sEnabled) NativeEndToplevel();
}

//===============================================================
//              TraceEvent::IdleTracingLooperMonitor
//===============================================================
CAR_INTERFACE_IMPL(TraceEvent::IdleTracingLooperMonitor, BasicLooperMonitor, IIdleHandler);

const String TraceEvent::IdleTracingLooperMonitor::TAG("TraceEvent.LooperMonitor");
const String TraceEvent::IdleTracingLooperMonitor::IDLE_EVENT_NAME("Looper.queueIdle");

const Int64 TraceEvent::IdleTracingLooperMonitor::FRAME_DURATION_MILLIS;
const Int64 TraceEvent::IdleTracingLooperMonitor::MIN_INTERESTING_DURATION_MILLIS;
const Int64 TraceEvent::IdleTracingLooperMonitor::MIN_INTERESTING_BURST_DURATION_MILLIS;

// Stats tracking

TraceEvent::IdleTracingLooperMonitor::IdleTracingLooperMonitor()
    : mLastIdleStartedAt(0L)
    , mLastWorkStartedAt(0L)
    , mNumTasksSeen(0)
    , mNumIdlesSeen(0)
    , mNumTasksSinceLastIdle(0)
    , mIdleMonitorAttached(FALSE)
{
}

// Called from within the begin/end methods only.
// This method can only execute on the looper thread, because that is
// the only thread that is permitted to call Looper.myqueue().
void TraceEvent::IdleTracingLooperMonitor::SyncIdleMonitoring()
{
    if (sEnabled && !mIdleMonitorAttached) {
        // approximate start time for computational purposes
        mLastIdleStartedAt = SystemClock::GetElapsedRealtime();
        AutoPtr<ILooperHelper> looperHelper;
        CLooperHelper::AcquireSingleton((ILooperHelper**)&looperHelper);
        AutoPtr<IMessageQueue> queue;
        looperHelper->GetMyQueue((IMessageQueue**)&queue);
        queue->AddIdleHandler(this);
        mIdleMonitorAttached = TRUE;
//        Log.v(TAG, "attached idle handler");
    }
    else if (mIdleMonitorAttached && !sEnabled) {
        AutoPtr<ILooperHelper> looperHelper;
        CLooperHelper::AcquireSingleton((ILooperHelper**)&looperHelper);
        AutoPtr<IMessageQueue> queue;
        looperHelper->GetMyQueue((IMessageQueue**)&queue);
        queue->RemoveIdleHandler(this);
        mIdleMonitorAttached = FALSE;
//        Log.v(TAG, "detached idle handler");
    }
}

//@Override
void TraceEvent::IdleTracingLooperMonitor::BeginHandling(
    /* [in] */ const String& line)
{
    // Close-out any prior 'idle' period before starting new task.
    if (mNumTasksSinceLastIdle == 0) {
        TraceEvent::End(IDLE_EVENT_NAME);
    }
    mLastWorkStartedAt = SystemClock::GetElapsedRealtime();
    SyncIdleMonitoring();
    BasicLooperMonitor::BeginHandling(line);
}

//@Override
void TraceEvent::IdleTracingLooperMonitor::EndHandling(
    /* [in] */ const String& line)
{
    const Int64 elapsed = SystemClock::GetElapsedRealtime()
            - mLastWorkStartedAt;
    if (elapsed > MIN_INTERESTING_DURATION_MILLIS) {
//        TraceAndLog(Log.WARN, "observed a task that took "
//                + elapsed + "ms: " + line);
    }
    BasicLooperMonitor::EndHandling(line);
    SyncIdleMonitoring();
    mNumTasksSeen++;
    mNumTasksSinceLastIdle++;
}

void TraceEvent::IdleTracingLooperMonitor::TraceAndLog(
    /* [in] */ Int32 level,
    /* [in] */ const String& message)
{
    TraceEvent::Instant(String("TraceEvent.LooperMonitor:IdleStats"), message);
//    Log.println(level, TAG, message);
}

//@Override
ECode TraceEvent::IdleTracingLooperMonitor::QueueIdle(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    const Int64 now =  SystemClock::GetElapsedRealtime();
    if (mLastIdleStartedAt == 0) mLastIdleStartedAt = now;
    const Int64 elapsed = now - mLastIdleStartedAt;
    mNumIdlesSeen++;
    assert(0);
    String strNumTasksSinceLastIdle = StringUtils::ToString(mNumTasksSinceLastIdle);
    String str(" tasks since last idle.");
    TraceEvent::Begin(IDLE_EVENT_NAME + strNumTasksSinceLastIdle + str);
    if (elapsed > MIN_INTERESTING_BURST_DURATION_MILLIS) {
#if 0
        // Dump stats
        String statsString = mNumTasksSeen + " tasks and "
                + mNumIdlesSeen + " idles processed so far, "
                + mNumTasksSinceLastIdle + " tasks bursted and "
                + elapsed + "ms elapsed since last idle";
        TraceAndLog(Log.DEBUG, statsString);
#endif
    }
    mLastIdleStartedAt = now;
    mNumTasksSinceLastIdle = 0;
    *result = TRUE; // stay installed

    return NOERROR;
}


//===============================================================
//                TraceEvent::LooperMonitorHolder
//===============================================================

AutoPtr<TraceEvent::BasicLooperMonitor> BasicLooperMonitor_Create()
{
    AutoPtr<TraceEvent::BasicLooperMonitor> instant;
    if (CommandLine::GetInstance()->HasSwitch(BaseSwitches::ENABLE_IDLE_TRACING)) {
        instant = new TraceEvent::IdleTracingLooperMonitor();
    }
    else {
        instant = new TraceEvent::BasicLooperMonitor();
    }
    return instant;
}

const AutoPtr<TraceEvent::BasicLooperMonitor> TraceEvent::LooperMonitorHolder::sInstance;// = BasicLooperMonitor_Create();

//===============================================================
//                         TraceEvent
//===============================================================

volatile Boolean TraceEvent::sEnabled = FALSE;

/**
 * Register an enabled observer, such that java traces are always enabled with native.
 */
void TraceEvent::RegisterNativeEnabledObserver()
{
    NativeRegisterEnabledObserver();
}

/**
 * Notification from native that tracing is enabled/disabled.
 */
//@CalledByNative
void TraceEvent::SetEnabled(
    /* [in] */ Boolean enabled)
{
   sEnabled = enabled;
   ThreadUtils::GetUiThreadLooper()->SetMessageLogging(
       enabled ? LooperMonitorHolder::sInstance : NULL);
}

/**
 * Enables or disabled Android systrace path of Chrome tracing. If enabled, all Chrome
 * traces will be also output to Android systrace. Because of the overhead of Android
 * systrace, this is for WebView only.
 */
void TraceEvent::SetATraceEnabled(
    /* [in] */ Boolean enabled)
{
    if (sEnabled == enabled) return;
    if (enabled) {
        NativeStartATrace();
    }
    else {
        NativeStopATrace();
    }
}

/**
 * @return True if tracing is enabled, false otherwise.
 * It is safe to call trace methods without checking if TraceEvent
 * is enabled.
 */
Boolean TraceEvent::Enabled()
{
    return sEnabled;
}

/**
 * Triggers the 'instant' native trace event with no arguments.
 * @param name The name of the event.
 */
void TraceEvent::Instant(
    /* [in] */ const String& name)
{
    if (sEnabled) NativeInstant(name, String(NULL));
}

/**
 * Triggers the 'instant' native trace event.
 * @param name The name of the event.
 * @param arg  The arguments of the event.
 */
void TraceEvent::Instant(
    /* [in] */ const String& name,
    /* [in] */ const String& arg)
{
    if (sEnabled) NativeInstant(name, arg);
}

/**
 * Convenience wrapper around the versions of startAsync() that take string parameters.
 * @param id The id of the asynchronous event.
 * @see #begin()
 */
void TraceEvent::StartAsync(
    /* [in] */ Int64 id)
{
    if (sEnabled) NativeStartAsync(GetCallerName(), id, String(NULL));
}

/**
 * Triggers the 'start' native trace event with no arguments.
 * @param name The name of the event.
 * @param id   The id of the asynchronous event.
 */
void TraceEvent::StartAsync(
    /* [in] */ const String& name,
    /* [in] */ Int64 id)
{
    if (sEnabled) NativeStartAsync(name, id, String(NULL));
}

/**
 * Triggers the 'start' native trace event.
 * @param name The name of the event.
 * @param id   The id of the asynchronous event.
 * @param arg  The arguments of the event.
 */
void TraceEvent::StartAsync(
    /* [in] */ const String& name,
    /* [in] */ Int64 id,
    /* [in] */ const String& arg)
{
    if (sEnabled) NativeStartAsync(name, id, arg);
}

/**
 * Convenience wrapper around the versions of finishAsync() that take string parameters.
 * @param id The id of the asynchronous event.
 * @see #begin()
 */
void TraceEvent::FinishAsync(
    /* [in] */ Int64 id)
{
    if (sEnabled) NativeFinishAsync(GetCallerName(), id, String(NULL));
}

/**
 * Triggers the 'finish' native trace event with no arguments.
 * @param name The name of the event.
 * @param id   The id of the asynchronous event.
 */
void TraceEvent::FinishAsync(
    /* [in] */ const String& name,
    /* [in] */ Int64 id)
{
    if (sEnabled) NativeFinishAsync(name, id, String(NULL));
}

/**
 * Triggers the 'finish' native trace event.
 * @param name The name of the event.
 * @param id   The id of the asynchronous event.
 * @param arg  The arguments of the event.
 */
void TraceEvent::FinishAsync(
    /* [in] */ const String& name,
    /* [in] */ Int64 id,
    /* [in] */ const String& arg)
{
    if (sEnabled) NativeFinishAsync(name, id, arg);
}

/**
 * Convenience wrapper around the versions of begin() that take string parameters.
 * The name of the event will be derived from the class and function name that call this.
 * IMPORTANT: if using this version, ensure end() (no parameters) is always called from the
 * same calling context.
 *
 * Note that the overhead is at ms or sub-ms order. Don't use this when millisecond accuracy
 * is desired.
 */
void TraceEvent::Begin() {
    if (sEnabled) NativeBegin(GetCallerName(), String(NULL));
}

/**
 * Triggers the 'begin' native trace event with no arguments.
 * @param name The name of the event.
 */
void TraceEvent::Begin(
    /* [in] */ const String& name)
{
    if (sEnabled) NativeBegin(name, String(NULL));
}

/**
 * Triggers the 'begin' native trace event.
 * @param name The name of the event.
 * @param arg  The arguments of the event.
 */
void TraceEvent::Begin(
    /* [in] */ const String& name,
    /* [in] */ const String& arg)
{
    if (sEnabled) NativeBegin(name, arg);
}

/**
 * Convenience wrapper around the versions of end() that take string parameters.
 * @see #begin()
 */
void TraceEvent::End() {
    if (sEnabled) NativeEnd(GetCallerName(), String(NULL));
}

/**
 * Triggers the 'end' native trace event with no arguments.
 * @param name The name of the event.
 */
void TraceEvent::End(
    /* [in] */ const String& name)
{
    if (sEnabled) NativeEnd(name, String(NULL));
}

/**
 * Triggers the 'end' native trace event.
 * @param name The name of the event.
 * @param arg  The arguments of the event.
 */
void TraceEvent::End(
    /* [in] */ const String& name,
    /* [in] */ const String& arg)
{
    if (sEnabled) NativeEnd(name, arg);
}

String TraceEvent::GetCallerName()
{
    assert(0);
#if 0
    // This was measured to take about 1ms on Trygon device.
    StackTraceElement[] stack = java.lang.Thread.currentThread().getStackTrace();

    // Commented out to avoid excess call overhead, but these lines can be useful to debug
    // exactly where the TraceEvent's client is on the callstack.
    //  int index = 0;
    //  while (!stack[index].getClassName().equals(TraceEvent.class.getName())) ++index;
    //  while (stack[index].getClassName().equals(TraceEvent.class.getName())) ++index;
    //  System.logW("TraceEvent caller is at stack index " + index);

    // '4' Was derived using the above commented out code snippet.
    return stack[4].getClassName() + "." + stack[4].getMethodName();
#endif
    return String(NULL);
}

void TraceEvent::NativeRegisterEnabledObserver()
{
    Elastos_TraceEvent_nativeRegisterEnabledObserver();
}

void TraceEvent::NativeStartATrace()
{
    Elastos_TraceEvent_nativeStartATrace();
}

void TraceEvent::NativeStopATrace()
{
    Elastos_TraceEvent_nativeStopATrace();
}

void TraceEvent::NativeInstant(
    /* [in] */ const String& name,
    /* [in] */ const String& arg)
{
    Elastos_TraceEvent_nativeInstant(name, arg);
}

void TraceEvent::NativeBegin(
    /* [in] */ const String& name,
    /* [in] */ const String& arg)
{
    Elastos_TraceEvent_nativeBegin(name, arg);
}

void TraceEvent::NativeEnd(
    /* [in] */ const String& name,
    /* [in] */ const String& arg)
{
    Elastos_TraceEvent_nativeEnd(name, arg);
}

void TraceEvent::NativeBeginToplevel()
{
    Elastos_TraceEvent_nativeBeginToplevel();
}

void TraceEvent::NativeEndToplevel()
{
    Elastos_TraceEvent_nativeEndToplevel();
}

void TraceEvent::NativeStartAsync(
    /* [in] */ const String& name,
    /* [in] */ Int64 id,
    /* [in] */ const String& arg)
{
    Elastos_TraceEvent_nativeStartAsync(name, id, arg);
}

void TraceEvent::NativeFinishAsync(
    /* [in] */ const String& name,
    /* [in] */ Int64 id,
    /* [in] */ const String& arg)
{
    Elastos_TraceEvent_nativeFinishAsync(name, id, arg);
}

} // namespace Base
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
