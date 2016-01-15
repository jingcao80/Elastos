
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/webkit/native/base/PerfTraceEvent.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Base {

//===============================================================
//                  PerfTraceEvent::EventType
//===============================================================

const String PerfTraceEvent::EventType::START("S");
const String PerfTraceEvent::EventType::FINISH("F");
const String PerfTraceEvent::EventType::INSTANT("I");

PerfTraceEvent::EventType::EventType(
    /* [in] */ const String& typeStr)
    : mTypeStr(typeStr)
{
}

//@Override
String PerfTraceEvent::EventType::ToString()
{
    return mTypeStr;
}

//===============================================================
//                        PerfTraceEvent
//===============================================================

const Int32 PerfTraceEvent::MAX_NAME_LENGTH;
const String PerfTraceEvent::MEMORY_TRACE_NAME_SUFFIX("_BZR_PSS");
AutoPtr<IFile> PerfTraceEvent::sOutputFile;

Boolean PerfTraceEvent::sEnabled = FALSE;
Boolean PerfTraceEvent::sTrackTiming = TRUE;
Boolean PerfTraceEvent::sTrackMemory = FALSE;

//AutoPtr<IJSONArray> PerfTraceEvent::sPerfTraceStrings;
//List<String> PerfTraceEvent::sFilter;
Int64 PerfTraceEvent::sBeginNanoTime;

/**
 * Specifies what event names will be tracked.
 *
 * @param strings Event names we will record.
 */
//synchronized
// void PerfTraceEvent::SetFilter(
//     /* [in] */ List<String>* strings)
// {
//     AutoLock lock(this);
//     sFilter = new LinkedList<String>(strings);
// }

/**
 * Enable or disable perf tracing.
 * Disabling of perf tracing will dump trace data to the system log.
 */
//synchronized
void PerfTraceEvent::SetEnabled(
    /* [in] */ Boolean enabled)
{
    assert(0);
#if 0
    AutoLock lock(this);
    if (sEnabled == enabled) {
        return;
    }

    if (enabled) {
        sBeginNanoTime = System::NanoTime();
        CJSONArray::New((IJSONArray**)&sPerfTraceStrings);
    }
    else {
        DumpPerf();
        sPerfTraceStrings = NULL;
        sFilter = null;
    }
    sEnabled = enabled;
#endif
}

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
void PerfTraceEvent::SetMemoryTrackingEnabled(
    /* [in] */ Boolean enabled)
{
//    AutoLock lock(this);
    sTrackMemory = enabled;
}

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
void PerfTraceEvent::SetTimingTrackingEnabled(
    /* [in] */ Boolean enabled)
{
//    AutoLock lock(this);
    sTrackTiming = enabled;
}

/**
 * @return True if tracing is enabled, false otherwise.
 * It is safe to call trace methods without checking if PerfTraceEvent
 * is enabled.
 */
//synchronized
Boolean PerfTraceEvent::Enabled()
{
//    AutoLock lock(this);
    return sEnabled;
}

/**
 * Record an "instant" perf trace event.  E.g. "screen update happened".
 */
//synchronized
void PerfTraceEvent::Instant(
    /* [in] */ const String& name)
{
    assert(0);
#if 0
    AutoLock lock(this);
    // Instant doesn't really need/take an event id, but this should be okay.
    Int64 eventId = name.HashCode();
    TraceEvent::Instant(name);
    if (sEnabled && MatchesFilter(name)) {
        SavePerfString(name, eventId, EventType::INSTANT, FALSE);
    }
#endif
}


/**
 * Record an "begin" perf trace event.
 * Begin trace events should have a matching end event.
 */
//synchronized
void PerfTraceEvent::Begin(
    /* [in] */ const String& name)
{
    assert(0);
#if 0
    AutoLock lock(this);
    const Int64 eventId = name.HashCode();
    TraceEvent::StartAsync(name, eventId);
    if (sEnabled && MatchesFilter(name)) {
        // Done before calculating the starting perf data to ensure calculating the memory usage
        // does not influence the timing data.
        if (sTrackMemory) {
            SavePerfString(MakeMemoryTraceNameFromTimingName(name), eventId, EventType::START,
                    TRUE);
        }
        if (sTrackTiming) {
            SavePerfString(name, eventId, EventType::START, FALSE);
        }
    }
#endif
}

/**
 * Record an "end" perf trace event, to match a begin event.  The
 * time delta between begin and end is usually interesting to
 * graph code.
 */
//synchronized
void PerfTraceEvent::End(
    /* [in] */ const String& name)
{
    assert(0);
#if 0
    AutoLock lock(this);
    const Int64 eventId = name.HashCode();
    TraceEvent::FinishAsync(name, eventId);
    if (sEnabled && MatchesFilter(name)) {
        if (sTrackTiming) {
            SavePerfString(name, eventId, EventType.FINISH, FALSE);
        }
        // Done after calculating the ending perf data to ensure calculating the memory usage
        // does not influence the timing data.
        if (sTrackMemory) {
            SavePerfString(MakeMemoryTraceNameFromTimingName(name), eventId, EventType::FINISH,
                    TRUE);
        }
    }
#endif
}

/**
 * Record an "begin" memory trace event.
 * Begin trace events should have a matching end event.
 */
#if 0
//synchronized
void PerfTraceEvent::Begin(
    /* [in] */ const String& name,
    /* [in] */ IMemoryInfo* memoryInfo)
{
    AutoLock lock(this);
    const Int64 eventId = name.HashCode();
    TraceEvent::StartAsync(name, eventId);
    if (sEnabled && MatchesFilter(name)) {
        // Done before calculating the starting perf data to ensure calculating the memory usage
        // does not influence the timing data.
        Int64 timestampUs = (System::NanoTime() - sBeginNanoTime) / 1000;
        SavePerfString(MakeMemoryTraceNameFromTimingName(name), eventId, EventType::START,
                timestampUs, memoryInfo);
        if (sTrackTiming) {
            SavePerfString(name, eventId, EventType::START, FALSE);
        }
    }
}
#endif

/**
 * Record an "end" memory trace event, to match a begin event.  The
 * memory usage delta between begin and end is usually interesting to
 * graph code.
 */
#if 0
//synchronized
void PerfTraceEvent::End(
    /* [in] */ const String& name,
    /* [in] */ IMemoryInfo* memoryInfo)
{
    AutoLock lock(this);
    Int64 eventId = name.HashCode();
    TraceEvent::FinishAsync(name, eventId);
    if (sEnabled && MatchesFilter(name)) {
        if (sTrackTiming) {
            SavePerfString(name, eventId, EventType::FINISH, FALSE);
        }
        // Done after calculating the instant perf data to ensure calculating the memory usage
        // does not influence the timing data.
        Int64 timestampUs = (System::NanoTime() - sBeginNanoTime) / 1000;
        SavePerfString(MakeMemoryTraceNameFromTimingName(name), eventId, EventType::FINISH,
                timestampUs, memoryInfo);
    }
}
#endif

/**
 * Determine if we are interested in this trace event.
 * @return True if the name matches the allowed filter; else false.
 */
Boolean PerfTraceEvent::MatchesFilter(
    /* [in] */ const String& name)
{
    assert(0);
//    return sFilter != null ? sFilter.contains(name) : false;
    return FALSE;
}

/**
 * Save a perf trace event as a JSON dict.  The format mirrors a TraceEvent dict.
 *
 * @param name The trace data
 * @param id The id of the event
 * @param type the type of trace event (I, S, F)
 * @param includeMemory Whether to include current browser process memory usage in the trace.
 */
void PerfTraceEvent::SavePerfString(
    /* [in] */ const String& name,
    /* [in] */ Int64 id,
    /* [in] */ const String& type,
    /* [in] */ Boolean includeMemory)
{
    assert(0);
#if 0
    Int64 timestampUs = (System::NanoTime() - sBeginNanoTime) / 1000;
    AutoPtr<IMemoryInfo> memInfo;
    if (includeMemory) {
        CMemoryInfo::New((IMemoryInfo**)&memInfo);
        Debug::GetMemoryInfo(memInfo);
    }
    SavePerfString(name, id, type, timestampUs, memInfo);
#endif
}

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
#if 0
void PerfTraceEvent::SavePerfString(
    /* [in] */ const String& name,
    /* [in] */ Int64 id,
    /* [in] */ const String& type,
    /* [in] */ Int64 timestampUs,
    /* [in] */ IMemoryInfo* memoryInfo)
{
    //try {
        AutoPtr<IJSONObject> traceObj;
        CJSONObject((IJSONObject**)&traceObj);
        traceObj->Put(String("cat"), String("Java"));
        traceObj->Put(String("ts"), timestampUs);
        traceObj->Put(String("ph"), type);
        traceObj->Put(String("name"), name);
        traceObj->Put(String("id"), id);
        if (memoryInfo != NULL) {
            Int32 nativePss, dalvikPss, otherPss;
            memoryInfo->GetNativePss(&nativePss);
            memoryInfo->GetDalvikPss(&dalvikPss);
            memoryInfo->GetOtherPss(&otherPss);
            Int32 pss = nativePss + dalvikPss + otherPss;
            traceObj->Put(String("mem"), pss);
        }
        sPerfTraceStrings->Put(traceObj);
    //} catch (JSONException e) {
    //    throw new RuntimeException(e);
    //}
}
#endif

/**
 * Generating a trace name for tracking memory based on the timing name passed in.
 *
 * @param name The timing name to use as a base for the memory perf name.
 * @return The memory perf name to use.
 */
String PerfTraceEvent::MakeMemoryTraceNameFromTimingName(
    /* [in] */ const String& name)
{
    return MakeSafeTraceName(name, MEMORY_TRACE_NAME_SUFFIX);
}

/**
 * Builds a name to be used in the perf trace framework.  The framework has length requirements
 * for names, so this ensures the generated name does not exceed the maximum (trimming the
 * base name if necessary).
 *
 * @param baseName The base name to use when generating the name.
 * @param suffix The required suffix to be appended to the name.
 * @return A name that is safe for the perf trace framework.
 */
String PerfTraceEvent::MakeSafeTraceName(
    /* [in] */ const String& baseName,
    /* [in] */ const String& suffix)
{
    Int32 suffixLength = suffix.GetLength();
    String _baseName;

    if (baseName.GetLength() + suffixLength > MAX_NAME_LENGTH) {
        _baseName = baseName.Substring(0, MAX_NAME_LENGTH - suffixLength);
    }

    return _baseName + suffix;
}

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
void PerfTraceEvent::SetOutputFile(
    /* [in] */ IFile* file)
{
//    AutoLock lock(this);
    sOutputFile = file;
}

/**
 * Dump all performance data we have saved up to the log.
 * Output as JSON for parsing convenience.
 */
void PerfTraceEvent::DumpPerf()
{
    assert(0);
#if 0
    String json = sPerfTraceStrings.toString();

    if (sOutputFile == null) {
        System.out.println(json);
    }
    else {
        try {
            PrintStream stream = new PrintStream(new FileOutputStream(sOutputFile, true));
            try {
                stream.print(json);
            } finally {
                try {
                    stream.close();
                } catch (Exception ex) {
                    Log.e("PerfTraceEvent", "Unable to close perf trace output file.");
                }
            }
        } catch (FileNotFoundException ex) {
            Log.e("PerfTraceEvent", "Unable to dump perf trace data to output file.");
        }
    }
#endif
}

} // namespace Base
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
