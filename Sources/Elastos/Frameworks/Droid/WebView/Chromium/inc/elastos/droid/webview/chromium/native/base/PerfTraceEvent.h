//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_PERFTRACEEVENT_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_PERFTRACEEVENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// import android.os.Debug;
// import android.os.Debug.MemoryInfo;
// import android.util.Log;

// import org.json.JSONArray;
// import org.json.JSONException;
// import org.json.JSONObject;
using Elastos::Droid::Os::IDebugMemoryInfo;
using Elastos::IO::IFile;
using Elastos::Utility::IList;
using Org::Json::IJSONArray;
// import java.io.FileNotFoundException;
// import java.io.FileOutputStream;
// import java.io.PrintStream;
// import java.util.LinkedList;
// import java.util.List;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * PerfTraceEvent can be used like TraceEvent, but is intended for
 * performance measurement.  By limiting the types of tracing we hope
 * to minimize impact on measurement.
 *
 * All PerfTraceEvent events funnel into TraceEvent. When not doing
 * performance measurements, they act the same.  However,
 * PerfTraceEvents can be enabled even when TraceEvent is not.
 *
 * Unlike TraceEvent, PerfTraceEvent data is sent to the system log,
 * not to a trace file.
 *
 * Performance events need to have very specific names so we find
 * the right ones.  For example, we specify the name exactly in
 * the @TracePerf annotation.  Thus, unlike TraceEvent, we do not
 * support an implicit trace name based on the callstack.
 */
class PerfTraceEvent : public Object
{
private:
    /** The event types understood by the perf trace scripts. */
    class EventType
        : public Object
    {
    public:
        EventType(
            /* [in] */ const String& typeStr);

        CARAPI_(String) ToString();

    public:
        static const String START;
        static const String FINISH;
        static const String INSTANT;

    private:
        // The string understood by the trace scripts.
        const String mTypeStr;
    };

public:
    /**
     * Specifies what event names will be tracked.
     *
     * @param strings Event names we will record.
     */
    //synchronized
    static CARAPI_(void) SetFilter(
        /* [in] */ IList* strings);// <String>

    /**
     * Enable or disable perf tracing.
     * Disabling of perf tracing will dump trace data to the system log.
     */
    //synchronized
    static CARAPI_(void) SetEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Enables memory tracking for all timing perf events tracked.
     *
     * <p>
     * Only works when called in combination with {@link #setEnabled(boolean)}.
     *
     * <p>
     * By enabling this feature, an additional perf event containing the memory usage will be
     * logged whenever {@link #instant(String)}, {@link #begin(String)}, or {@link #end(String)}
     * is called.
     *
     * @param enabled Whether to enable memory tracking for all perf events.
     */
    //synchronized
    static CARAPI_(void) SetMemoryTrackingEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Enables timing tracking for all perf events tracked.
     *
     * <p>
     * Only works when called in combination with {@link #setEnabled(boolean)}.
     *
     * <p>
     * If this feature is enabled, whenever {@link #instant(String)}, {@link #begin(String)},
     * or {@link #end(String)} is called the time since start of tracking will be logged.
     *
     * @param enabled Whether to enable timing tracking for all perf events.
     */
    //synchronized
    static CARAPI_(void) SetTimingTrackingEnabled(
        /* [in] */ Boolean enabled);

    /**
     * @return True if tracing is enabled, false otherwise.
     * It is safe to call trace methods without checking if PerfTraceEvent
     * is enabled.
     */
    //synchronized
    static CARAPI_(Boolean) Enabled();

    /**
     * Record an "instant" perf trace event.  E.g. "screen update happened".
     */
    //synchronized
    static CARAPI_(void) Instant(
        /* [in] */ const String& name);

    /**
     * Record an "begin" perf trace event.
     * Begin trace events should have a matching end event.
     */
    //synchronized
    static CARAPI_(void) Begin(
        /* [in] */ const String& name);

    /**
     * Record an "end" perf trace event, to match a begin event.  The
     * time delta between begin and end is usually interesting to
     * graph code.
     */
    //synchronized
    static CARAPI_(void) End(
        /* [in] */ const String& name);

    /**
     * Record an "begin" memory trace event.
     * Begin trace events should have a matching end event.
     */
    //synchronized
    static CARAPI_(void) Begin(
        /* [in] */ const String& name,
        /* [in] */ IDebugMemoryInfo* memoryInfo);

    /**
     * Record an "end" memory trace event, to match a begin event.  The
     * memory usage delta between begin and end is usually interesting to
     * graph code.
     */
    //synchronized
    static CARAPI_(void) End(
        /* [in] */ const String& name,
        /* [in] */ IDebugMemoryInfo* memoryInfo);

    /**
     * Generating a trace name for tracking memory based on the timing name passed in.
     *
     * @param name The timing name to use as a base for the memory perf name.
     * @return The memory perf name to use.
     */
    static CARAPI_(String) MakeMemoryTraceNameFromTimingName(
        /* [in] */ const String& name);

    /**
     * Builds a name to be used in the perf trace framework.  The framework has length requirements
     * for names, so this ensures the generated name does not exceed the maximum (trimming the
     * base name if necessary).
     *
     * @param baseName The base name to use when generating the name.
     * @param suffix The required suffix to be appended to the name.
     * @return A name that is safe for the perf trace framework.
     */
    static CARAPI_(String) MakeSafeTraceName(
        /* [in] */ const String& baseName,
        /* [in] */ const String& suffix);

    /**
     * Sets a file to dump the results to.  If {@code file} is {@code null}, it will be dumped
     * to STDOUT, otherwise the JSON performance data will be appended to {@code file}.  This should
     * be called before the performance run starts.  When {@link #setEnabled(boolean)} is called
     * with {@code false}, the perf data will be dumped.
     *
     * @param file Which file to append the performance data to.  If {@code null}, the performance
     *             data will be sent to STDOUT.
     */
    //synchronized
    static CARAPI_(void) SetOutputFile(
        /* [in] */ IFile* file);

private:
    /**
     * Determine if we are interested in this trace event.
     * @return True if the name matches the allowed filter; else false.
     */
    static CARAPI_(Boolean) MatchesFilter(
        /* [in] */ const String& name);

    /**
     * Save a perf trace event as a JSON dict.  The format mirrors a TraceEvent dict.
     *
     * @param name The trace data
     * @param id The id of the event
     * @param type the type of trace event (I, S, F)
     * @param includeMemory Whether to include current browser process memory usage in the trace.
     */
    static CARAPI_(void) SavePerfString(
        /* [in] */ const String& name,
        /* [in] */ Int64 id,
        /* [in] */ const String& type,
        /* [in] */ Boolean includeMemory);

    /**
     * Save a perf trace event as a JSON dict.  The format mirrors a TraceEvent dict.
     *
     * @param name The trace data
     * @param id The id of the event
     * @param type the type of trace event (I, S, F)
     * @param timestampUs The time stamp at which this event was recorded
     * @param memoryInfo Memory details to be included in this perf string, null if
     *                   no memory details are to be included.
     */
    static CARAPI_(void) SavePerfString(
        /* [in] */ const String& name,
        /* [in] */ Int64 id,
        /* [in] */ const String& type,
        /* [in] */ Int64 timestampUs,
        /* [in] */ IDebugMemoryInfo* memoryInfo);

    /**
     * Dump all performance data we have saved up to the log.
     * Output as JSON for parsing convenience.
     */
    static CARAPI_(void) DumpPerf();

private:
    static const Int32 MAX_NAME_LENGTH = 40;
    static const String MEMORY_TRACE_NAME_SUFFIX;
    static AutoPtr<IFile> sOutputFile;

    static Boolean sEnabled;
    static Boolean sTrackTiming;
    static Boolean sTrackMemory;

    // A list of performance trace event strings.
    // Events are stored as a JSON dict much like TraceEvent.
    // E.g. timestamp is in microseconds.
    static AutoPtr</*TODO IJSONArray*/IInterface> sPerfTraceStrings;

    // A filter for performance tracing.  Only events that match a
    // string in the list are saved.  Presence of a filter does not
    // necessarily mean perf tracing is enabled.
    static AutoPtr<IList> sFilter;//String

    // Nanosecond start time of performance tracing.
    static Int64 sBeginNanoTime;

    static Object sLock;
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_PERFTRACEEVENT_H__
