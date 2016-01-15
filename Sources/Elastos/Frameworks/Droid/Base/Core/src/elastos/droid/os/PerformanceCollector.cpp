
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/PerformanceCollector.h"

namespace Elastos {
namespace Droid {
namespace Os {

const String PerformanceCollector::METRIC_KEY_ITERATIONS("iterations");
const String PerformanceCollector::METRIC_KEY_LABEL("label");
const String PerformanceCollector::METRIC_KEY_CPU_TIME("cpu_time");
const String PerformanceCollector::METRIC_KEY_EXECUTION_TIME("execution_time");
const String PerformanceCollector::METRIC_KEY_PRE_RECEIVED_TRANSACTIONS("pre_received_transactions");
const String PerformanceCollector::METRIC_KEY_PRE_SENT_TRANSACTIONS("pre_sent_transactions");
const String PerformanceCollector::METRIC_KEY_RECEIVED_TRANSACTIONS("received_transactions");
const String PerformanceCollector::METRIC_KEY_SENT_TRANSACTIONS("sent_transactions");
const String PerformanceCollector::METRIC_KEY_GC_INVOCATION_COUNT("gc_invocation_count");
const String PerformanceCollector::METRIC_KEY_JAVA_ALLOCATED("java_allocated");
const String PerformanceCollector::METRIC_KEY_JAVA_FREE("java_free");
const String PerformanceCollector::METRIC_KEY_JAVA_PRIVATE_DIRTY("java_private_dirty");
const String PerformanceCollector::METRIC_KEY_JAVA_PSS("java_pss");
const String PerformanceCollector::METRIC_KEY_JAVA_SHARED_DIRTY("java_shared_dirty");
const String PerformanceCollector::METRIC_KEY_JAVA_SIZE("java_size");
const String PerformanceCollector::METRIC_KEY_NATIVE_ALLOCATED("native_allocated");
const String PerformanceCollector::METRIC_KEY_NATIVE_FREE("native_free");
const String PerformanceCollector::METRIC_KEY_NATIVE_PRIVATE_DIRTY("native_private_dirty");
const String PerformanceCollector::METRIC_KEY_NATIVE_PSS("native_pss");
const String PerformanceCollector::METRIC_KEY_NATIVE_SHARED_DIRTY("native_shared_dirty");
const String PerformanceCollector::METRIC_KEY_NATIVE_SIZE("native_size");
const String PerformanceCollector::METRIC_KEY_GLOBAL_ALLOC_COUNT("global_alloc_count");
const String PerformanceCollector::METRIC_KEY_GLOBAL_ALLOC_SIZE("global_alloc_size");
const String PerformanceCollector::METRIC_KEY_GLOBAL_FREED_COUNT("global_freed_count");
const String PerformanceCollector::METRIC_KEY_GLOBAL_FREED_SIZE("global_freed_size");
const String PerformanceCollector::METRIC_KEY_OTHER_PRIVATE_DIRTY("other_private_dirty");
const String PerformanceCollector::METRIC_KEY_OTHER_PSS("other_pss");
const String PerformanceCollector::METRIC_KEY_OTHER_SHARED_DIRTY("other_shared_dirty");

PerformanceCollector::PerformanceCollector()
    : mSnapshotCpuTime(0)
    , mSnapshotExecTime(0)
    , mCpuTime(0)
    , mExecTime(0)
{
}

PerformanceCollector::PerformanceCollector(
    /* [in] */ PerformanceCollector::PerformanceResultsWriter* writer)
{
    mPerfWriter = writer;
}

PerformanceCollector::~PerformanceCollector()
{
}

void PerformanceCollector::SetPerformanceResultsWriter(
    /* [in] */ PerformanceCollector::PerformanceResultsWriter* writer)
{
    mPerfWriter = writer;
}

void PerformanceCollector::BeginSnapshot(
    /* [in] */ const String& label)
{
    if (mPerfWriter != NULL)
        mPerfWriter->WriteBeginSnapshot(label);
    StartPerformanceSnapshot();
}

AutoPtr<IBundle> PerformanceCollector::EndSnapshot()
{
    EndPerformanceSnapshot();
    if (mPerfWriter != NULL)
        mPerfWriter->WriteEndSnapshot(mPerfSnapshot);
    return mPerfSnapshot;
}

void PerformanceCollector::StartTiming(
    /* [in] */ const String& label)
{
    assert(0 && "TODO");
    // if (mPerfWriter != NULL)
    //     mPerfWriter->WriteStartTiming(label);
    // mPerfMeasurement = new Bundle();
    // mPerfMeasurement.putParcelableArrayList(
    //         METRIC_KEY_ITERATIONS, new ArrayList<Parcelable>());
    // mExecTime = SystemClock.uptimeMillis();
    // mCpuTime = Process.getElapsedCpuTime();
}

AutoPtr<IBundle> PerformanceCollector::AddIteration(
    /* [in] */ const String& label)
{
    assert(0 && "TODO");
    return NULL;
    // mCpuTime = Process.getElapsedCpuTime() - mCpuTime;
    // mExecTime = SystemClock.uptimeMillis() - mExecTime;

    // Bundle iteration = new Bundle();
    // iteration.putString(METRIC_KEY_LABEL, label);
    // iteration.putLong(METRIC_KEY_EXECUTION_TIME, mExecTime);
    // iteration.putLong(METRIC_KEY_CPU_TIME, mCpuTime);
    // mPerfMeasurement.getParcelableArrayList(METRIC_KEY_ITERATIONS).add(iteration);

    // mExecTime = SystemClock.uptimeMillis();
    // mCpuTime = Process.getElapsedCpuTime();
    // return iteration;
}

AutoPtr<IBundle> PerformanceCollector::StopTiming(
    /* [in] */ const String& label)
{
    AddIteration(label);
    if (mPerfWriter != NULL)
        mPerfWriter->WriteStopTiming(mPerfMeasurement);
    return mPerfMeasurement;
}

void PerformanceCollector::AddMeasurement(
    /* [in] */ const String& label,
    /* [in] */ Int64 value)
{
    if (mPerfWriter != NULL)
        mPerfWriter->WriteMeasurement(label, value);
}

void PerformanceCollector::AddMeasurement(
    /* [in] */ const String& label,
    /* [in] */ Float value)
{
    if (mPerfWriter != NULL)
        mPerfWriter->WriteMeasurement(label, value);
}

void PerformanceCollector::AddMeasurement(
    /* [in] */ const String& label,
    /* [in] */ const String& value)
{
    if (mPerfWriter != NULL)
        mPerfWriter->WriteMeasurement(label, value);
}

void PerformanceCollector::StartPerformanceSnapshot()
{
    assert(0 && "TODO");
    // // Create new snapshot
    // mPerfSnapshot = new Bundle();

    // // Add initial binder counts
    // Bundle binderCounts = getBinderCounts();
    // for (String key : binderCounts.keySet()) {
    //     mPerfSnapshot.putLong("pre_" + key, binderCounts.getLong(key));
    // }

    // // Force a GC and zero out the performance counters. Do this
    // // before reading initial CPU/wall-clock times so we don't include
    // // the cost of this setup in our final metrics.
    // startAllocCounting();

    // // Record CPU time up to this point, and start timing. Note: this
    // // must happen at the end of this method, otherwise the timing will
    // // include noise.
    // mSnapshotExecTime = SystemClock.uptimeMillis();
    // mSnapshotCpuTime = Process.getElapsedCpuTime();
}

void PerformanceCollector::EndPerformanceSnapshot()
{
    assert(0 && "TODO");
    // // Stop the timing. This must be done first before any other counting is
    // // stopped.
    // mSnapshotCpuTime = Process.getElapsedCpuTime() - mSnapshotCpuTime;
    // mSnapshotExecTime = SystemClock.uptimeMillis() - mSnapshotExecTime;

    // stopAllocCounting();

    // long nativeMax = Debug.getNativeHeapSize() / 1024;
    // long nativeAllocated = Debug.getNativeHeapAllocatedSize() / 1024;
    // long nativeFree = Debug.getNativeHeapFreeSize() / 1024;

    // Debug.MemoryInfo memInfo = new Debug.MemoryInfo();
    // Debug.getMemoryInfo(memInfo);

    // Runtime runtime = Runtime.getRuntime();

    // long dalvikMax = runtime.totalMemory() / 1024;
    // long dalvikFree = runtime.freeMemory() / 1024;
    // long dalvikAllocated = dalvikMax - dalvikFree;

    // // Add final binder counts
    // Bundle binderCounts = getBinderCounts();
    // for (String key : binderCounts.keySet()) {
    //     mPerfSnapshot.putLong(key, binderCounts.getLong(key));
    // }

    // // Add alloc counts
    // Bundle allocCounts = getAllocCounts();
    // for (String key : allocCounts.keySet()) {
    //     mPerfSnapshot.putLong(key, allocCounts.getLong(key));
    // }

    // mPerfSnapshot.putLong(METRIC_KEY_EXECUTION_TIME, mSnapshotExecTime);
    // mPerfSnapshot.putLong(METRIC_KEY_CPU_TIME, mSnapshotCpuTime);

    // mPerfSnapshot.putLong(METRIC_KEY_NATIVE_SIZE, nativeMax);
    // mPerfSnapshot.putLong(METRIC_KEY_NATIVE_ALLOCATED, nativeAllocated);
    // mPerfSnapshot.putLong(METRIC_KEY_NATIVE_FREE, nativeFree);
    // mPerfSnapshot.putLong(METRIC_KEY_NATIVE_PSS, memInfo.nativePss);
    // mPerfSnapshot.putLong(METRIC_KEY_NATIVE_PRIVATE_DIRTY, memInfo.nativePrivateDirty);
    // mPerfSnapshot.putLong(METRIC_KEY_NATIVE_SHARED_DIRTY, memInfo.nativeSharedDirty);

    // mPerfSnapshot.putLong(METRIC_KEY_JAVA_SIZE, dalvikMax);
    // mPerfSnapshot.putLong(METRIC_KEY_JAVA_ALLOCATED, dalvikAllocated);
    // mPerfSnapshot.putLong(METRIC_KEY_JAVA_FREE, dalvikFree);
    // mPerfSnapshot.putLong(METRIC_KEY_JAVA_PSS, memInfo.dalvikPss);
    // mPerfSnapshot.putLong(METRIC_KEY_JAVA_PRIVATE_DIRTY, memInfo.dalvikPrivateDirty);
    // mPerfSnapshot.putLong(METRIC_KEY_JAVA_SHARED_DIRTY, memInfo.dalvikSharedDirty);

    // mPerfSnapshot.putLong(METRIC_KEY_OTHER_PSS, memInfo.otherPss);
    // mPerfSnapshot.putLong(METRIC_KEY_OTHER_PRIVATE_DIRTY, memInfo.otherPrivateDirty);
    // mPerfSnapshot.putLong(METRIC_KEY_OTHER_SHARED_DIRTY, memInfo.oth
}

void PerformanceCollector::StartAllocCounting()
{
    assert(0 && "TODO");
    // // Before we start trigger a GC and reset the debug counts. Run the
    // // finalizers and another GC before starting and stopping the alloc
    // // counts. This will free up any objects that were just sitting around
    // // waiting for their finalizers to be run.
    // Runtime.getRuntime().gc();
    // Runtime.getRuntime().runFinalization();
    // Runtime.getRuntime().gc();

    // Debug.resetAllCounts();

    // // start the counts
    // Debug.startAllocCounting();
}

void PerformanceCollector::StopAllocCounting()
{
    assert(0 && "TODO");
    // Runtime.getRuntime().gc();
    // Runtime.getRuntime().runFinalization();
    // Runtime.getRuntime().gc();
    // Debug.stopAllocCounting();
}

AutoPtr<IBundle> PerformanceCollector::GetAllocCounts()
{
    assert(0 && "TODO");
    // Bundle results = new Bundle();
    // results.putLong(METRIC_KEY_GLOBAL_ALLOC_COUNT, Debug.getGlobalAllocCount());
    // results.putLong(METRIC_KEY_GLOBAL_ALLOC_SIZE, Debug.getGlobalAllocSize());
    // results.putLong(METRIC_KEY_GLOBAL_FREED_COUNT, Debug.getGlobalFreedCount());
    // results.putLong(METRIC_KEY_GLOBAL_FREED_SIZE, Debug.getGlobalFreedSize());
    // results.putLong(METRIC_KEY_GC_INVOCATION_COUNT, Debug.getGlobalGcInvocationCount());
    // return results;
}

AutoPtr<IBundle> PerformanceCollector::GetBinderCounts()
{
    assert(0 && "TODO");
    // Bundle results = new Bundle();
    // results.putLong(METRIC_KEY_SENT_TRANSACTIONS, Debug.getBinderSentTransactions());
    // results.putLong(METRIC_KEY_RECEIVED_TRANSACTIONS, Debug.getBinderReceivedTransactions());
    // return results;
    return NULL;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
