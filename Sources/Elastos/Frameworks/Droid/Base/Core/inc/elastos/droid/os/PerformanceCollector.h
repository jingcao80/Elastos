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

#ifndef __ELASTOS_DROID_OS_PERFORMANCE_COLLECTOR_H__
#define __ELASTOS_DROID_OS_PERFORMANCE_COLLECTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IThrowable;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Collects performance data between two function calls in Bundle objects and
 * outputs the results using writer of type {@link PerformanceResultsWriter}.
 * <p>
 * {@link #beginSnapshot(String)} and {@link #endSnapshot()} functions collect
 * memory usage information and measure runtime between calls to begin and end.
 * These functions logically wrap around an entire test, and should be called
 * with name of test as the label, e.g. EmailPerformanceTest.
 * <p>
 * {@link #startTiming(String)} and {@link #stopTiming(String)} functions
 * measure runtime between calls to start and stop. These functions logically
 * wrap around a single test case or a small block of code, and should be called
 * with the name of test case as the label, e.g. testSimpleSendMailSequence.
 * <p>
 * {@link #addIteration(String)} inserts intermediate measurement point which
 * can be labeled with a String, e.g. Launch email app, compose, send, etc.
 * <p>
 * Snapshot and timing functions do not interfere with each other, and thus can
 * be called in any order. The intended structure is to wrap begin/endSnapshot
 * around calls to start/stopTiming, for example:
 * <p>
 * <code>beginSnapshot("EmailPerformanceTest");
 * startTiming("testSimpleSendSequence");
 * addIteration("Launch email app");
 * addIteration("Compose");
 * stopTiming("Send");
 * startTiming("testComplexSendSequence");
 * stopTiming("");
 * startTiming("testAddLabel");
 * stopTiming("");
 * endSnapshot();</code>
 * <p>
 * Structure of results output is up to implementor of
 * {@link PerformanceResultsWriter }.
 *
 * {@hide} Pending approval for public API.
 */
class PerformanceCollector
    : public Object
{
public:
    /**
     * Interface for reporting performance data.
     */
    class PerformanceResultsWriter
        : public Object
    {
    public:
        /**
         * Callback invoked as first action in
         * PerformanceCollector#beginSnapshot(String) for reporting the start of
         * a performance snapshot.
         *
         * @param label description of code block between beginSnapshot and
         *              PerformanceCollector#endSnapshot()
         * @see PerformanceCollector#beginSnapshot(String)
         */
        virtual CARAPI_(void) WriteBeginSnapshot(
            /* [in] */ const String& label) = 0;

        /**
         * Callback invoked as last action in PerformanceCollector#endSnapshot()
         * for reporting performance data collected in the snapshot.
         *
         * @param results memory and runtime metrics stored as key/value pairs,
         *        in the same structure as returned by
         *        PerformanceCollector#endSnapshot()
         * @see PerformanceCollector#endSnapshot()
         */
        virtual CARAPI_(void) WriteEndSnapshot(
            /* [in] */ IBundle* results) = 0;

        /**
         * Callback invoked as first action in
         * PerformanceCollector#startTiming(String) for reporting the start of
         * a timing measurement.
         *
         * @param label description of code block between startTiming and
         *              PerformanceCollector#stopTiming(String)
         * @see PerformanceCollector#startTiming(String)
         */
        virtual CARAPI_(void) WriteStartTiming(
            /* [in] */ const String& label) = 0;

        /**
         * Callback invoked as last action in
         * {@link PerformanceCollector#stopTiming(String)} for reporting the
         * sequence of timings measured.
         *
         * @param results runtime metrics of code block between calls to
         *                startTiming and stopTiming, in the same structure as
         *                returned by PerformanceCollector#stopTiming(String)
         * @see PerformanceCollector#stopTiming(String)
         */
        virtual CARAPI_(void) WriteStopTiming(
            /* [in] */ IBundle* results) = 0;

        /**
         * Callback invoked as last action in
         * {@link PerformanceCollector#addMeasurement(String, Int64)} for
         * reporting an integer type measurement.
         *
         * @param label short description of the metric that was measured
         * @param value Int64 value of the measurement
         */
        virtual CARAPI_(void) WriteMeasurement(
            /* [in] */ const String& label,
            /* [in] */ Int64 value) = 0;

        /**
         * Callback invoked as last action in
         * {@link PerformanceCollector#addMeasurement(String, float)} for
         * reporting a float type measurement.
         *
         * @param label short description of the metric that was measured
         * @param value float value of the measurement
         */
        virtual CARAPI_(void) WriteMeasurement(
            /* [in] */ const String& label,
            /* [in] */ Float value) = 0;

        /**
         * Callback invoked as last action in
         * {@link PerformanceCollector#addMeasurement(String, String)} for
         * reporting a string field.
         *
         * @param label short description of the metric that was measured
         * @param value string summary of the measurement
         */
        virtual CARAPI_(void) WriteMeasurement(
            /* [in] */ const String& label,
            /* [in] */ const String& value) = 0;
    };

public:
    PerformanceCollector();

    PerformanceCollector(
        /* [in] */ PerformanceResultsWriter* writer);

    virtual ~PerformanceCollector();

    CARAPI_(void) SetPerformanceResultsWriter(
        /* [in] */ PerformanceResultsWriter* writer);

    /**
     * Begin collection of memory usage information.
     *
     * @param label description of code block between beginSnapshot and
     *              endSnapshot, used to label output
     */
    CARAPI_(void) BeginSnapshot(
        /* [in] */ const String& label);

    /**
     * End collection of memory usage information. Returns collected data in a
     * Bundle object.
     *
     * @return Memory and runtime metrics stored as key/value pairs. Values are
     *         of type long, and keys include:
     *         <ul>
     *         <li>{@link #METRIC_KEY_CPU_TIME cpu_time}
     *         <li>{@link #METRIC_KEY_EXECUTION_TIME execution_time}
     *         <li>{@link #METRIC_KEY_PRE_RECEIVED_TRANSACTIONS
     *         pre_received_transactions}
     *         <li>{@link #METRIC_KEY_PRE_SENT_TRANSACTIONS
     *         pre_sent_transactions}
     *         <li>{@link #METRIC_KEY_RECEIVED_TRANSACTIONS
     *         received_transactions}
     *         <li>{@link #METRIC_KEY_SENT_TRANSACTIONS sent_transactions}
     *         <li>{@link #METRIC_KEY_GC_INVOCATION_COUNT gc_invocation_count}
     *         <li>{@link #METRIC_KEY_JAVA_ALLOCATED java_allocated}
     *         <li>{@link #METRIC_KEY_JAVA_FREE java_free}
     *         <li>{@link #METRIC_KEY_JAVA_PRIVATE_DIRTY java_private_dirty}
     *         <li>{@link #METRIC_KEY_JAVA_PSS java_pss}
     *         <li>{@link #METRIC_KEY_JAVA_SHARED_DIRTY java_shared_dirty}
     *         <li>{@link #METRIC_KEY_JAVA_SIZE java_size}
     *         <li>{@link #METRIC_KEY_NATIVE_ALLOCATED native_allocated}
     *         <li>{@link #METRIC_KEY_NATIVE_FREE native_free}
     *         <li>{@link #METRIC_KEY_NATIVE_PRIVATE_DIRTY native_private_dirty}
     *         <li>{@link #METRIC_KEY_NATIVE_PSS native_pss}
     *         <li>{@link #METRIC_KEY_NATIVE_SHARED_DIRTY native_shared_dirty}
     *         <li>{@link #METRIC_KEY_NATIVE_SIZE native_size}
     *         <li>{@link #METRIC_KEY_GLOBAL_ALLOC_COUNT global_alloc_count}
     *         <li>{@link #METRIC_KEY_GLOBAL_ALLOC_SIZE global_alloc_size}
     *         <li>{@link #METRIC_KEY_GLOBAL_FREED_COUNT global_freed_count}
     *         <li>{@link #METRIC_KEY_GLOBAL_FREED_SIZE global_freed_size}
     *         <li>{@link #METRIC_KEY_OTHER_PRIVATE_DIRTY other_private_dirty}
     *         <li>{@link #METRIC_KEY_OTHER_PSS other_pss}
     *         <li>{@link #METRIC_KEY_OTHER_SHARED_DIRTY other_shared_dirty}
     *         </ul>
     */
    CARAPI_(AutoPtr<IBundle>) EndSnapshot();

    /**
     * Start measurement of user and cpu time.
     *
     * @param label description of code block between startTiming and
     *        stopTiming, used to label output
     */
    CARAPI_(void) StartTiming(
        /* [in] */ const String& label);

    /**
     * Add a measured segment, and start measuring the next segment. Returns
     * collected data in a Bundle object.
     *
     * @param label description of code block between startTiming and
     *              addIteration, and between two calls to addIteration, used
     *              to label output
     * @return Runtime metrics stored as key/value pairs. Values are of type
     *         long, and keys include:
     *         <ul>
     *         <li>{@link #METRIC_KEY_LABEL label}
     *         <li>{@link #METRIC_KEY_CPU_TIME cpu_time}
     *         <li>{@link #METRIC_KEY_EXECUTION_TIME execution_time}
     *         </ul>
     */
    CARAPI_(AutoPtr<IBundle>) AddIteration(
        /* [in] */ const String& label);

    /**
     * Stop measurement of user and cpu time.
     *
     * @param label description of code block between addIteration or
     *              startTiming and stopTiming, used to label output
     * @return Runtime metrics stored in a bundle, including all iterations
     *         between calls to startTiming and stopTiming. List of iterations
     *         is keyed by {@link #METRIC_KEY_ITERATIONS iterations}.
     */
    CARAPI_(AutoPtr<IBundle>) StopTiming(
        /* [in] */ const String& label);

    /**
     * Add a float type measurement to the collector.
     *
     * @param label short description of the metric that was measured
     * @param value float value of the measurement
     */
    CARAPI_(void) AddMeasurement(
        /* [in] */ const String& label,
        /* [in] */ Int64 value);

    /**
     * Add a float type measurement to the collector.
     *
     * @param label short description of the metric that was measured
     * @param value float value of the measurement
     */
    CARAPI_(void) AddMeasurement(
        /* [in] */ const String& label,
        /* [in] */ Float value);

    /**
     * Add a string field to the collector.
     *
     * @param label short description of the metric that was measured
     * @param value string summary of the measurement
     */
    CARAPI_(void) AddMeasurement(
        /* [in] */ const String& label,
        /* [in] */ const String& value);

private:
    /*
     * Starts tracking memory usage, binder transactions, and real & cpu timing.
     */
    CARAPI_(void) StartPerformanceSnapshot();

    /*
     * Stops tracking memory usage, binder transactions, and real & cpu timing.
     * Stores collected data as type long into Bundle object for reporting.
     */
    CARAPI_(void) EndPerformanceSnapshot();

    /*
     * Starts allocation counting. This triggers a gc and resets the counts.
     */
    static CARAPI_(void) StartAllocCounting();

    /*
     * Stops allocation counting.
     */
    static CARAPI_(void) StopAllocCounting();

    /*
     * Returns a bundle with the current results from the allocation counting.
     */
    static CARAPI_(AutoPtr<IBundle>) GetAllocCounts();

    /*
     * Returns a bundle with the counts for various binder counts for this
     * process. Currently the only two that are reported are the number of send
     * and the number of received transactions.
     */
    static CARAPI_(AutoPtr<IBundle>) GetBinderCounts();

public:

    /**
     * In a results Bundle, this key references a List of iteration Bundles.
     */
    static const String METRIC_KEY_ITERATIONS;// = "iterations";
    /**
     * In an iteration Bundle, this key describes the iteration.
     */
    static const String METRIC_KEY_LABEL;// = "label";
    /**
     * In a results Bundle, this key reports the cpu time of the code block
     * under measurement.
     */
    static const String METRIC_KEY_CPU_TIME;// = "cpu_time";
    /**
     * In a results Bundle, this key reports the execution time of the code
     * block under measurement.
     */
    static const String METRIC_KEY_EXECUTION_TIME;// = "execution_time";
    /**
     * In a snapshot Bundle, this key reports the number of received
     * transactions from the binder driver before collection started.
     */
    static const String METRIC_KEY_PRE_RECEIVED_TRANSACTIONS;// = "pre_received_transactions";
    /**
     * In a snapshot Bundle, this key reports the number of transactions sent by
     * the running program before collection started.
     */
    static const String METRIC_KEY_PRE_SENT_TRANSACTIONS;// = "pre_sent_transactions";
    /**
     * In a snapshot Bundle, this key reports the number of received
     * transactions from the binder driver.
     */
    static const String METRIC_KEY_RECEIVED_TRANSACTIONS;// = "received_transactions";
    /**
     * In a snapshot Bundle, this key reports the number of transactions sent by
     * the running program.
     */
    static const String METRIC_KEY_SENT_TRANSACTIONS;// = "sent_transactions";
    /**
     * In a snapshot Bundle, this key reports the number of garbage collection
     * invocations.
     */
    static const String METRIC_KEY_GC_INVOCATION_COUNT;// = "gc_invocation_count";
    /**
     * In a snapshot Bundle, this key reports the amount of allocated memory
     * used by the running program.
     */
    static const String METRIC_KEY_JAVA_ALLOCATED;// = "java_allocated";
    /**
     * In a snapshot Bundle, this key reports the amount of free memory
     * available to the running program.
     */
    static const String METRIC_KEY_JAVA_FREE;// = "java_free";
    /**
     * In a snapshot Bundle, this key reports the number of private dirty pages
     * used by dalvik.
     */
    static const String METRIC_KEY_JAVA_PRIVATE_DIRTY;// = "java_private_dirty";
    /**
     * In a snapshot Bundle, this key reports the proportional set size for
     * dalvik.
     */
    static const String METRIC_KEY_JAVA_PSS;// = "java_pss";
    /**
     * In a snapshot Bundle, this key reports the number of shared dirty pages
     * used by dalvik.
     */
    static const String METRIC_KEY_JAVA_SHARED_DIRTY;// = "java_shared_dirty";
    /**
     * In a snapshot Bundle, this key reports the total amount of memory
     * available to the running program.
     */
    static const String METRIC_KEY_JAVA_SIZE;// = "java_size";
    /**
     * In a snapshot Bundle, this key reports the amount of allocated memory in
     * the native heap.
     */
    static const String METRIC_KEY_NATIVE_ALLOCATED;// = "native_allocated";
    /**
     * In a snapshot Bundle, this key reports the amount of free memory in the
     * native heap.
     */
    static const String METRIC_KEY_NATIVE_FREE;// = "native_free";
    /**
     * In a snapshot Bundle, this key reports the number of private dirty pages
     * used by the native heap.
     */
    static const String METRIC_KEY_NATIVE_PRIVATE_DIRTY;// = "native_private_dirty";
    /**
     * In a snapshot Bundle, this key reports the proportional set size for the
     * native heap.
     */
    static const String METRIC_KEY_NATIVE_PSS;// = "native_pss";
    /**
     * In a snapshot Bundle, this key reports the number of shared dirty pages
     * used by the native heap.
     */
    static const String METRIC_KEY_NATIVE_SHARED_DIRTY;// = "native_shared_dirty";
    /**
     * In a snapshot Bundle, this key reports the size of the native heap.
     */
    static const String METRIC_KEY_NATIVE_SIZE;// = "native_size";
    /**
     * In a snapshot Bundle, this key reports the number of objects allocated
     * globally.
     */
    static const String METRIC_KEY_GLOBAL_ALLOC_COUNT;// = "global_alloc_count";
    /**
     * In a snapshot Bundle, this key reports the size of all objects allocated
     * globally.
     */
    static const String METRIC_KEY_GLOBAL_ALLOC_SIZE;// = "global_alloc_size";
    /**
     * In a snapshot Bundle, this key reports the number of objects freed
     * globally.
     */
    static const String METRIC_KEY_GLOBAL_FREED_COUNT;// = "global_freed_count";
    /**
     * In a snapshot Bundle, this key reports the size of all objects freed
     * globally.
     */
    static const String METRIC_KEY_GLOBAL_FREED_SIZE;// = "global_freed_size";
    /**
     * In a snapshot Bundle, this key reports the number of private dirty pages
     * used by everything else.
     */
    static const String METRIC_KEY_OTHER_PRIVATE_DIRTY;// = "other_private_dirty";
    /**
     * In a snapshot Bundle, this key reports the proportional set size for
     * everything else.
     */
    static const String METRIC_KEY_OTHER_PSS;// = "other_pss";
    /**
     * In a snapshot Bundle, this key reports the number of shared dirty pages
     * used by everything else.
     */
    static const String METRIC_KEY_OTHER_SHARED_DIRTY;// = "other_shared_dirty";

private:
    AutoPtr<PerformanceResultsWriter> mPerfWriter;
    AutoPtr<IBundle> mPerfSnapshot;
    AutoPtr<IBundle> mPerfMeasurement;
    Int64 mSnapshotCpuTime;
    Int64 mSnapshotExecTime;
    Int64 mCpuTime;
    Int64 mExecTime;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_PERFORMANCE_COLLECTOR_H__
