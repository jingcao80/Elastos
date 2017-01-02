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

#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.Os.h>
//TODO #include "elastos/droid/os/CDebugMemoryInfo.h"
#include "elastos/droid/webkit/webview/chromium/native/base/PerfTraceEvent.h"
#include "elastos/droid/webkit/webview/chromium/native/base/TraceEvent.h"
#include "elastos/core/AutoLock.h"
//TODO #include "org/json/CJSONArray.h"
//TODO #include "org/json/CJSONObject.h"

//TODO using Elastos::Droid::Os::CDebug;
//TODO using Elastos::Droid::Os::CDebugMemoryInfo;
using Elastos::Core::AutoLock;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IPrintStream;
using Elastos::IO::CPrintStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IOutputStream;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::ICollection;
//TODO using Org::Json::CJSONArray;
using Org::Json::IJSONObject;
//TODO using Org::Json::CJSONObject;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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

AutoPtr</*TODO IJSONArray*/IInterface> PerfTraceEvent::sPerfTraceStrings;
AutoPtr<IList> PerfTraceEvent::sFilter;
Int64 PerfTraceEvent::sBeginNanoTime;
Object PerfTraceEvent::sLock;

/**
 * Specifies what event names will be tracked.
 *
 * @param strings Event names we will record.
 */
//synchronized
void PerfTraceEvent::SetFilter(
     /* [in] */ IList* strings)
{
     AutoLock lock(sLock);
     //sFilter = new LinkedList<String>(strings);
     CLinkedList::New(ICollection::Probe(strings), (IList**)&sFilter);
}

/**
 * Enable or disable perf tracing.
 * Disabling of perf tracing will dump trace data to the system log.
 */
//synchronized
void PerfTraceEvent::SetEnabled(
    /* [in] */ Boolean enabled)
{
    AutoLock lock(sLock);
    if (sEnabled == enabled) {
        return;
    }

    if (enabled) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetNanoTime(&sBeginNanoTime);
        //TODO CJSONArray::New((IJSONArray**)&sPerfTraceStrings);
    }
    else {
        DumpPerf();
        sPerfTraceStrings = NULL;
        sFilter = NULL;
    }
    sEnabled = enabled;
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
    AutoLock lock(sLock);
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
    AutoLock lock(sLock);
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
    AutoLock lock(sLock);
    return sEnabled;
}

/**
 * Record an "instant" perf trace event.  E.g. "screen update happened".
 */
//synchronized
void PerfTraceEvent::Instant(
    /* [in] */ const String& name)
{
    AutoLock lock(sLock);
    // Instant doesn't really need/take an event id, but this should be okay.
    Int64 eventId = name.GetHashCode();
    TraceEvent::Instant(name);
    if (sEnabled && MatchesFilter(name)) {
        SavePerfString(name, eventId, EventType::INSTANT, FALSE);
    }
}


/**
 * Record an "begin" perf trace event.
 * Begin trace events should have a matching end event.
 */
//synchronized
void PerfTraceEvent::Begin(
    /* [in] */ const String& name)
{
    AutoLock lock(sLock);
    const Int64 eventId = name.GetHashCode();
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
    AutoLock lock(sLock);
    const Int64 eventId = name.GetHashCode();
    TraceEvent::FinishAsync(name, eventId);
    if (sEnabled && MatchesFilter(name)) {
        if (sTrackTiming) {
            SavePerfString(name, eventId, EventType::FINISH, FALSE);
        }
        // Done after calculating the ending perf data to ensure calculating the memory usage
        // does not influence the timing data.
        if (sTrackMemory) {
            SavePerfString(MakeMemoryTraceNameFromTimingName(name), eventId, EventType::FINISH,
                    TRUE);
        }
    }
}

/**
 * Record an "begin" memory trace event.
 * Begin trace events should have a matching end event.
 */
//synchronized
void PerfTraceEvent::Begin(
    /* [in] */ const String& name,
    /* [in] */ IDebugMemoryInfo* memoryInfo)
{
    AutoLock lock(sLock);
    const Int64 eventId = name.GetHashCode();
    TraceEvent::StartAsync(name, eventId);
    if (sEnabled && MatchesFilter(name)) {
        // Done before calculating the starting perf data to ensure calculating the memory usage
        // does not influence the timing data.
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 nanoTime;
        system->GetNanoTime(&nanoTime);
        Int64 timestampUs = (nanoTime - sBeginNanoTime) / 1000;
        SavePerfString(MakeMemoryTraceNameFromTimingName(name), eventId, EventType::START,
                timestampUs, memoryInfo);
        if (sTrackTiming) {
            SavePerfString(name, eventId, EventType::START, FALSE);
        }
    }
}

/**
 * Record an "end" memory trace event, to match a begin event.  The
 * memory usage delta between begin and end is usually interesting to
 * graph code.
 */
//synchronized
void PerfTraceEvent::End(
    /* [in] */ const String& name,
    /* [in] */ IDebugMemoryInfo* memoryInfo)
{
    AutoLock lock(sLock);
    Int64 eventId = name.GetHashCode();
    TraceEvent::FinishAsync(name, eventId);
    if (sEnabled && MatchesFilter(name)) {
        if (sTrackTiming) {
            SavePerfString(name, eventId, EventType::FINISH, FALSE);
        }
        // Done after calculating the instant perf data to ensure calculating the memory usage
        // does not influence the timing data.
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 nanoTime;
        system->GetNanoTime(&nanoTime);
        Int64 timestampUs = (nanoTime - sBeginNanoTime) / 1000;
        SavePerfString(MakeMemoryTraceNameFromTimingName(name), eventId, EventType::FINISH,
                timestampUs, memoryInfo);
    }
}

/**
 * Determine if we are interested in this trace event.
 * @return True if the name matches the allowed filter; else false.
 */
Boolean PerfTraceEvent::MatchesFilter(
    /* [in] */ const String& name)
{
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    Boolean contains;
    return sFilter != NULL? (sFilter->Contains(TO_IINTERFACE(cs), &contains), contains) : FALSE;
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
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 nanoTime;
    system->GetNanoTime(&nanoTime);
    Int64 timestampUs = (nanoTime - sBeginNanoTime) / 1000;
    AutoPtr<IDebugMemoryInfo> memInfo;
    if (includeMemory) {
        //TODO CDebugMemoryInfo::New((IDebugMemoryInfo**)&memInfo);
        //TODO Debug::GetMemoryInfo(memInfo);
    }
    SavePerfString(name, id, type, timestampUs, memInfo);
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
void PerfTraceEvent::SavePerfString(
    /* [in] */ const String& name,
    /* [in] */ Int64 id,
    /* [in] */ const String& type,
    /* [in] */ Int64 timestampUs,
    /* [in] */ IDebugMemoryInfo* memoryInfo)
{
    //try {
        //TODO AutoPtr<IJSONObject> traceObj;
        //TODO CJSONObject::New((IJSONObject**)&traceObj);
        AutoPtr<ICharSequence> cs;
        CString::New(String("Java"), (ICharSequence**)&cs);
        //TODO traceObj->Put(String("cat"), TO_IINTERFACE(cs));
        //TODO traceObj->Put(String("ts"), timestampUs);
        AutoPtr<ICharSequence> csType;
        CString::New(type, (ICharSequence**)&csType);
        //TODO traceObj->Put(String("ph"), TO_IINTERFACE(csType));
        AutoPtr<ICharSequence> csName;
        CString::New(name, (ICharSequence**)&csName);
        //TODO traceObj->Put(String("name"), TO_IINTERFACE(csName));
        //TODO traceObj->Put(String("id"), id);
        if (memoryInfo != NULL) {
            Int32 nativePss, dalvikPss, otherPss;
            memoryInfo->GetNativePss(&nativePss);
            memoryInfo->GetDalvikPss(&dalvikPss);
            memoryInfo->GetOtherPss(&otherPss);
            //TODO Int32 pss = nativePss + dalvikPss + otherPss;
            //TODO traceObj->Put(String("mem"), pss);
        }
        //TODO sPerfTraceStrings->Put(TO_IINTERFACE(traceObj));
    //} catch (JSONException e) {
    //    throw new RuntimeException(e);
    //}
}

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
    AutoLock lock(sLock);
    sOutputFile = file;
}

/**
 * Dump all performance data we have saved up to the log.
 * Output as JSON for parsing convenience.
 */
void PerfTraceEvent::DumpPerf()
{
    String json;
    //TODO sPerfTraceStrings->ToString(4, &json);

    if (sOutputFile == NULL) {
        //System.out.println(json);
        printf("%s\n", json.string());
    }
    else {
        //try {
            AutoPtr<IFileOutputStream> fos;
            CFileOutputStream::New(sOutputFile, TRUE, (IFileOutputStream**)&fos);
            AutoPtr<IPrintStream> stream;
            CPrintStream::New(IOutputStream::Probe(fos), (IPrintStream**)&stream);
            //try {
                stream->Print(json);
            //} finally {
            //    try {
                ICloseable::Probe(stream)->Close();
            //    } catch (Exception ex) {
            //        Log.e("PerfTraceEvent", "Unable to close perf trace output file.");
            //    }
            //}
        //} catch (FileNotFoundException ex) {
        //    Log.e("PerfTraceEvent", "Unable to dump perf trace data to output file.");
        //}
    }
}

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
