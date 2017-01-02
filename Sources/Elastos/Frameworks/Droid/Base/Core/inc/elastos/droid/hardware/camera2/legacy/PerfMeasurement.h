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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_PERFMEASUREMENT_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_PERFMEASUREMENT_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <GLES2/gl2.h>
#include <utils/Vector.h>
#include <utils/Timers.h>

using Elastos::Core::Object;
using Elastos::Utility::IQueue;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class PerfMeasurement
    : public Object
    , public IPerfMeasurement
{
private:
    /**
     * Asynchronous low-overhead GL performance measurement using
     * http://www.khronos.org/registry/gles/extensions/EXT/EXT_disjoint_timer_query.txt
     *
     * Measures the duration of GPU processing for a set of GL commands, delivering
     * the measurement asynchronously once processing completes.
     *
     * All calls must come from a single thread with a valid GL context active.
     **/
    class PerfMeasurementContext
        : public Object
    {
    public:
        /**
         * maxQueryCount should be a conservative estimate of how many query objects
         * will be active at once, which is a function of the GPU's level of
         * pipelining and the frequency of queries.
         */
        PerfMeasurementContext(
            /* [in] */ size_t maxQueryCount);

        CARAPI_(Int32) GetMaxQueryCount();

        /**
         * Start a measurement period using the next available query object.
         * Returns INVALID_OPERATION if called multiple times in a row,
         * and BAD_VALUE if no more query objects are available.
         */
        CARAPI_(Int32) StartGlTimer();

        /**
         * Finish the current measurement period
         * Returns INVALID_OPERATION if called before any startGLTimer calls
         * or if called multiple times in a row.
         */
        CARAPI_(Int32) StopGlTimer();

        /**
         * Get the next available duration measurement.
         *
         * Returns NO_DURATION_YET if no new measurement is available,
         * and FAILED_MEASUREMENT if an error occurred during the next
         * measurement period.
         *
         * Otherwise returns a positive number of nanoseconds measuring the
         * duration of the oldest completed query.
         */
        CARAPI_(nsecs_t) GetNextGlDuration();

        static CARAPI_(Boolean) IsMeasurementSupported();

    private:
        static const nsecs_t NO_DURATION_YET;
        static const nsecs_t FAILED_MEASUREMENT;

        android::Vector<GLuint> mTimingQueries;
        size_t mTimingStartIndex;
        size_t mTimingEndIndex;
        size_t mTimingQueryIndex;
        size_t mFreeQueries;

        Boolean mInitDone;
    };

public:
    CAR_INTERFACE_DECL()

    PerfMeasurement();

    virtual ~PerfMeasurement();

    /**
     * Create a performance measurement object with a maximum of {@value #DEFAULT_MAX_QUERIES}
     * in-progess queries.
     */
    CARAPI constructor();

    /**
     * Create a performance measurement object with maxQueries as the maximum number of
     * in-progress queries.
     *
     * @param maxQueries maximum in-progress queries, must be larger than 0.
     * @throws IllegalArgumentException if maxQueries is less than 1.
     */
    CARAPI constructor(
        /* [in] */ Int32 maxQueries);

    /**
     * Returns true if the Gl timing methods will work, false otherwise.
     *
     * <p>Must be called within a valid GL context.</p>
     */
    static CARAPI IsGlTimingSupported(
        /* [out] */ Boolean* result);

    /**
     * Dump collected data to file, and clear the stored data.
     *
     * <p>
     * Format is a simple csv-like text file with a header,
     * followed by a 3-column list of values in nanoseconds:
     * <pre>
     *   timestamp gpu_duration cpu_duration
     *   <long> <long> <long>
     *   <long> <long> <long>
     *   <long> <long> <long>
     *   ....
     * </pre>
     * </p>
     */
    CARAPI DumpPerformanceData(
        /* [in] */ const String& path);

    /**
     * Start a GPU/CPU timing measurement.
     *
     * <p>Call before starting a rendering pass. Only one timing measurement can be active at once,
     * so {@link #stopTimer} must be called before the next call to this method.</p>
     *
     * @throws IllegalStateException if the maximum number of queries are in progress already,
     *                               or the method is called multiple times in a row, or there is
     *                               a GPU error.
     */
    CARAPI StartTimer();

    /**
     * Finish a GPU/CPU timing measurement.
     *
     * <p>Call after finishing all the drawing for a rendering pass. Only one timing measurement can
     * be active at once, so {@link #startTimer} must be called before the next call to this
     * method.</p>
     *
     * @throws IllegalStateException if no GL timer is currently started, or there is a GPU
     *                               error.
     */
    CARAPI StopTimer();

    /**
     * Add a timestamp to a timing measurement. These are queued up and matched to completed
     * workload measurements as they become available.
     */
    CARAPI AddTimestamp(
        /* [in] */ Int64 timestamp);

    /**
     * Returns the number of measurements so far that returned a valid duration
     * measurement.
     */
    CARAPI GetCompletedQueryCount(
        /* [out] */ Int32* count);

protected:
    /**
     * Start a GL timing section.
     *
     * <p>All GL commands between this method and the next {@link #nativeEndGlTimer} will be
     * included in the timing.</p>
     *
     * <p>Must be called from the same thread as calls to {@link #nativeEndGlTimer} and
     * {@link #nativeGetNextGlDuration}.</p>
     *
     * @throws IllegalStateException if a GL error occurs or start is called repeatedly.
     */
    static CARAPI NativeStartGlTimer(
        /* [in] */ Int64 contextHandle);

    /**
     * Finish a GL timing section.
     *
     * <p>Some time after this call returns, the time the GPU took to
     * execute all work submitted between the latest {@link #nativeStartGlTimer} and
     * this call, will become available from calling {@link #nativeGetNextGlDuration}.</p>
     *
     * <p>Must be called from the same thread as calls to {@link #nativeStartGlTimer} and
     * {@link #nativeGetNextGlDuration}.</p>
     *
     * @throws IllegalStateException if a GL error occurs or stop is called before start
     */
    static CARAPI NativeStopGlTimer(
        /* [in] */ Int64 contextHandle);

    /**
     * Get the next available GL duration measurement, in nanoseconds.
     *
     * <p>Must be called from the same thread as calls to {@link #nativeStartGlTimer} and
     * {@link #nativeEndGlTimer}.</p>
     *
     * @return the next GL duration measurement, or {@link #NO_DURATION_YET} if
     *         no new measurement is available, or {@link #FAILED_TIMING} if timing
     *         failed for the next duration measurement.
     * @throws IllegalStateException if a GL error occurs
     */
    static CARAPI_(Int64) NativeGetNextGlDuration(
        /* [in] */ Int64 contextHandle);

private:
    static CARAPI_(PerfMeasurementContext*) GetContext(
        /* [in] */ Int64 context);

    /**
     * Get the next available GPU timing measurement.
     *
     * <p>Since the GPU works asynchronously, the results of a single start/stopGlTimer measurement
     * will only be available some time after the {@link #stopTimer} call is made. Poll this method
     * until the result becomes available. If multiple start/endTimer measurements are made in a
     * row, the results will be available in FIFO order.</p>
     *
     * @return The measured duration of the GPU workload for the next pending query, or
     *         {@link #NO_DURATION_YET} if no queries are pending or the next pending query has not
     *         yet finished, or {@link #FAILED_TIMING} if the GPU was unable to complete the
     *         measurement.
     *
     * @throws IllegalStateException If there is a GPU error.
     *
     */
    CARAPI_(Int64) GetNextGlDuration();

    /**
     * Create a native performance measurement context.
     *
     * @param maxQueryCount maximum in-progress queries; must be >= 1.
     */
    static CARAPI_(Int64) NativeCreateContext(
        /* [in] */ Int32 maxQueryCount);

    /**
     * Delete the native context.
     *
     * <p>Not safe to call more than once.</p>
     */
    static CARAPI_(void) NativeDeleteContext(
        /* [in] */ Int64 contextHandle);

    /**
     * Query whether the relevant Gl extensions are available for Gl timing
     */
    static CARAPI_(Boolean) NativeQuerySupport();

private:
    static const String TAG;

    Int64 mNativeContext;

    Int32 mCompletedQueryCount;

    /**
     * Values for completed measurements
     */
    AutoPtr<IArrayList> mCollectedGpuDurations;
    AutoPtr<IArrayList> mCollectedCpuDurations;
    AutoPtr<IArrayList> mCollectedTimestamps;

    /**
     * Values for in-progress measurements (waiting for async GPU results)
     */
    AutoPtr<IQueue> mTimestampQueue;
    AutoPtr<IQueue> mCpuDurationsQueue;

    Int64 mStartTimeNs;

    /**
     * The value returned by {@link #nativeGetNextGlDuration} if no new timing
     * measurement is available since the last call.
     */
    static const Int64 NO_DURATION_YET;

    /**
     * The value returned by {@link #nativeGetNextGlDuration} if timing failed for
     * the next timing interval
     */
    static const Int64 FAILED_TIMING;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_BURSTHOLDER_H__
