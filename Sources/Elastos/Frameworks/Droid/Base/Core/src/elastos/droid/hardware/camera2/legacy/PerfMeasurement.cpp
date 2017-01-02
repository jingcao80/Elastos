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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/camera2/legacy/PerfMeasurement.h"
#include "elastos/droid/hardware/camera2/legacy/PerfMeasurement.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <system/window.h>
#include <ui/GraphicBuffer.h>
#include <GLES2/gl2ext.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Core::StringBuilder;
using Elastos::Core::IInteger64;
using Elastos::Core::CoreUtils;
using Elastos::IO::IWriter;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileWriter;
using Elastos::IO::CFileWriter;
using Elastos::IO::IBufferedWriter;
using Elastos::IO::CBufferedWriter;
using Elastos::IO::IFileWriter;
using Elastos::IO::CFileWriter;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(PerfMeasurement, Object, IPerfMeasurement)

const String PerfMeasurement::TAG("PerfMeasurement");
const Int64 PerfMeasurement::NO_DURATION_YET = -1l;
const Int64 PerfMeasurement::FAILED_TIMING = -2l;

PerfMeasurement::PerfMeasurement()
    : mNativeContext(0)
    , mCompletedQueryCount(0)
    , mStartTimeNs(0)
{
    CArrayList::New((IArrayList**)&mCollectedGpuDurations);
    CArrayList::New((IArrayList**)&mCollectedCpuDurations);
    CArrayList::New((IArrayList**)&mCollectedTimestamps);

    CLinkedList::New((IQueue**)&mTimestampQueue);
    CLinkedList::New((IQueue**)&mCpuDurationsQueue);
}

PerfMeasurement::~PerfMeasurement()
{
    NativeDeleteContext(mNativeContext);
}


ECode PerfMeasurement::constructor()
{
    mNativeContext = NativeCreateContext(DEFAULT_MAX_QUERIES);
    return NOERROR;
}

ECode PerfMeasurement::constructor(
    /* [in] */ Int32 maxQueries)
{
    if (maxQueries < 1) {
        //throw new IllegalArgumentException("maxQueries is less than 1");
        Logger::E(TAG, "maxQueries is less than 1");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mNativeContext = NativeCreateContext(maxQueries);
    return NOERROR;
}

ECode PerfMeasurement::IsGlTimingSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeQuerySupport();
    return NOERROR;
}

ECode PerfMeasurement::DumpPerformanceData(
    /* [in] */ const String& path)
{
    //try {
    AutoPtr<IFileWriter> fWriter;
    CFileWriter::New(path, (IFileWriter**)&fWriter);
    AutoPtr<IBufferedWriter> dump;
    ECode ec = CBufferedWriter::New(IWriter::Probe(fWriter), (IBufferedWriter**)&dump);
    if(FAILED(ec)) {
        Logger::E(TAG, "Error writing data dump to %s : %d", path.string(), ec);
        ICloseable::Probe(dump)->Close();
        return NOERROR;
    }
    ec = IWriter::Probe(dump)->Write(String("timestamp gpu_duration cpu_duration\n"));
    if(FAILED(ec)) {
        Logger::E(TAG, "Error writing data dump to %s : %d", path.string(), ec);
        ICloseable::Probe(dump)->Close();
        return NOERROR;
    }
    Int32 size;
    mCollectedGpuDurations->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        StringBuilder sb;
        AutoPtr<IInterface> obj;
        mCollectedTimestamps->Get(i, (IInterface**)&obj);
        AutoPtr<IInteger64> numObj = IInteger64::Probe(obj);
        Int64 num;
        numObj->GetValue(&num);
        sb += num;
        sb += " ";
        obj = NULL;
        numObj = NULL;
        mCollectedGpuDurations->Get(i, (IInterface**)&obj);
        numObj = IInteger64::Probe(obj);
        numObj->GetValue(&num);
        sb += num;
        sb += " ";
        obj = NULL;
        numObj = NULL;
        mCollectedCpuDurations->Get(i, (IInterface**)&obj);
        numObj = IInteger64::Probe(obj);
        numObj->GetValue(&num);
        sb += num;
        sb += "\n";
        ec = IWriter::Probe(dump)->Write(sb.ToString());
        if(FAILED(ec)) {
            Logger::E(TAG, "Error writing data dump to %s : %d", path.string(), ec);
            ICloseable::Probe(dump)->Close();
            return NOERROR;
        }
    }
    mCollectedTimestamps->Clear();
    mCollectedGpuDurations->Clear();
    mCollectedCpuDurations->Clear();
    //} catch (IOException e) {
    if(FAILED(ec)) {
        Logger::E(TAG, "Error writing data dump to %s : %d", path.string(), ec);
        ICloseable::Probe(dump)->Close();
    }
    //}
    return NOERROR;
}

ECode PerfMeasurement::StartTimer()
{
    NativeStartGlTimer(mNativeContext);
    mStartTimeNs = SystemClock::GetElapsedRealtimeNanos();
    return NOERROR;
}

ECode PerfMeasurement::StopTimer()
{
    // Complete CPU timing
    Int64 endTimeNs = SystemClock::GetElapsedRealtimeNanos();
    AutoPtr<IInteger64> obj = CoreUtils::Convert(endTimeNs - mStartTimeNs);
    mCpuDurationsQueue->Add(TO_IINTERFACE(obj));
    // Complete GL timing
    NativeStopGlTimer(mNativeContext);

    // Poll to see if GL timing results have arrived; if so
    // store the results for a frame
    Int64 duration = GetNextGlDuration();
    if (duration > 0) {
        AutoPtr<IInteger64> obj = CoreUtils::Convert(duration);
        mCollectedGpuDurations->Add(TO_IINTERFACE(obj));

        Boolean result;
        mTimestampQueue->IsEmpty(&result);
        if (result) {
            AutoPtr<IInteger64> obj2 = CoreUtils::Convert(NO_DURATION_YET);
            mCollectedTimestamps->Add(TO_IINTERFACE(obj2));
        }
        else {
            AutoPtr<IInterface> obj3;
            mTimestampQueue->Poll((IInterface**)&obj3);
            mCollectedTimestamps->Add(obj3);
        }

        mCpuDurationsQueue->IsEmpty(&result);
        if (result) {
            AutoPtr<IInteger64> obj4 = CoreUtils::Convert(NO_DURATION_YET);
            mCollectedCpuDurations->Add(TO_IINTERFACE(obj4));
        }
        else {
            AutoPtr<IInterface> obj5;
            mCpuDurationsQueue->Poll((IInterface**)&obj5);
            mCollectedCpuDurations->Add(obj5);
        }
    }
    if (duration == FAILED_TIMING) {
        // Discard timestamp and CPU measurement since GPU measurement failed
        Boolean result;
        mTimestampQueue->IsEmpty(&result);
        if (!result) {
            AutoPtr<IInterface> obj;
            mTimestampQueue->Poll((IInterface**)&obj);
        }

        mCpuDurationsQueue->IsEmpty(&result);
        if (!result) {
            AutoPtr<IInterface> obj;
            mCpuDurationsQueue->Poll((IInterface**)&obj);
        }
    }
    return NOERROR;
}

ECode PerfMeasurement::AddTimestamp(
    /* [in] */ Int64 timestamp)
{
    AutoPtr<IInteger64> obj = CoreUtils::Convert(timestamp);
    return mTimestampQueue->Add(TO_IINTERFACE(obj));
}

Int64 PerfMeasurement::GetNextGlDuration()
{
    Int64 duration = NativeGetNextGlDuration(mNativeContext);
    if (duration > 0) {
        mCompletedQueryCount++;
    }
    return duration;
}

ECode PerfMeasurement::GetCompletedQueryCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    *count = mCompletedQueryCount;
    return NOERROR;
}

static Boolean extensionEqual(
    /* [in] */ const GLubyte* pExtensions,
    /* [in] */ const GLubyte* pExtension)
{
    while (TRUE) {
        char a = *pExtensions++;
        char b = *pExtension++;
        Boolean aEnd = a == '\0' || a == ' ';
        Boolean bEnd = b == '\0';
        if (aEnd || bEnd) {
            return aEnd == bEnd;
        }
        if (a != b) {
            return FALSE;
        }
    }
}

static const GLubyte* nextExtension(
    /* [in] */ const GLubyte* pExtensions)
{
    while (TRUE) {
        char a = *pExtensions++;
        if (a == '\0') {
            return pExtensions-1;
        }
        else if ( a == ' ') {
            return pExtensions;
        }
    }
}

static Boolean checkForExtension(
    /* [in] */ const GLubyte* pExtensions,
    /* [in] */ const GLubyte* pExtension)
{
    for (; *pExtensions != '\0'; pExtensions = nextExtension(pExtensions)) {
        if (extensionEqual(pExtensions, pExtension)) {
            return TRUE;
        }
    }
    return FALSE;
}

const nsecs_t PerfMeasurement::PerfMeasurementContext::NO_DURATION_YET = -1L;
const nsecs_t PerfMeasurement::PerfMeasurementContext::FAILED_MEASUREMENT = -2L;

PerfMeasurement::PerfMeasurementContext::PerfMeasurementContext(
    /* [in] */ size_t maxQueryCount)
    : mTimingStartIndex(0)
    , mTimingEndIndex(0)
    , mTimingQueryIndex(0)
    , mFreeQueries(maxQueryCount)
    , mInitDone(FALSE)
{
    mTimingQueries.resize(maxQueryCount);
}

Int32 PerfMeasurement::PerfMeasurementContext::GetMaxQueryCount()
{
    return mTimingQueries.size();
}

Int32 PerfMeasurement::PerfMeasurementContext::StartGlTimer()
{
    // Lazy init of queries to avoid needing GL context during construction
    if (!mInitDone) {
        glGenQueriesEXT(mTimingQueries.size(), mTimingQueries.editArray());
        mInitDone = TRUE;
    }

    if (mTimingEndIndex != mTimingStartIndex) {
        return android::INVALID_OPERATION;
    }

    if (mFreeQueries == 0) {
        return android::BAD_VALUE;
    }

    glBeginQueryEXT(GL_TIME_ELAPSED_EXT, mTimingQueries[mTimingStartIndex]);

    mTimingStartIndex = (mTimingStartIndex + 1) % mTimingQueries.size();
    mFreeQueries--;

    return android::OK;
}

Int32 PerfMeasurement::PerfMeasurementContext::StopGlTimer()
{
    size_t nextEndIndex = (mTimingEndIndex + 1) % mTimingQueries.size();
    if (nextEndIndex != mTimingStartIndex) {
        return android::INVALID_OPERATION;
    }
    glEndQueryEXT(GL_TIME_ELAPSED_EXT);

    mTimingEndIndex = nextEndIndex;

    return android::OK;
}

nsecs_t PerfMeasurement::PerfMeasurementContext::GetNextGlDuration()
{
    if (!mInitDone) {
        // No start/stop called yet
        return NO_DURATION_YET;
    }

    GLint available;
    glGetQueryObjectivEXT(mTimingQueries[mTimingQueryIndex],
            GL_QUERY_RESULT_AVAILABLE_EXT, &available);
    if (!available) {
        return NO_DURATION_YET;
    }

    GLint64 duration = FAILED_MEASUREMENT;
    GLint disjointOccurred;
    glGetIntegerv(GL_GPU_DISJOINT_EXT, &disjointOccurred);

    if (!disjointOccurred) {
        glGetQueryObjecti64vEXT(mTimingQueries[mTimingQueryIndex],
                GL_QUERY_RESULT_EXT,
                &duration);
    }

    mTimingQueryIndex = (mTimingQueryIndex + 1) % mTimingQueries.size();
    mFreeQueries++;

    return static_cast<nsecs_t>(duration);
}

Boolean PerfMeasurement::PerfMeasurementContext::IsMeasurementSupported()
{
    const GLubyte* extensions = glGetString(GL_EXTENSIONS);
    return checkForExtension(extensions,
            reinterpret_cast<const GLubyte*>("GL_EXT_disjoint_timer_query"));
}

PerfMeasurement::PerfMeasurementContext* PerfMeasurement::GetContext(
    /* [in] */ Int64 context) {
    return reinterpret_cast<PerfMeasurement::PerfMeasurementContext*>(context);
}

Int64 PerfMeasurement::NativeCreateContext(
    /* [in] */ Int32 maxQueryCount)
{
    PerfMeasurementContext *context = new PerfMeasurementContext(maxQueryCount);
    return reinterpret_cast<Int64>(context);
}

void PerfMeasurement::NativeDeleteContext(
    /* [in] */ Int64 contextHandle)
{
    PerfMeasurementContext *context = GetContext(contextHandle);
    delete(context);
    return;
}

Boolean PerfMeasurement::NativeQuerySupport()
{
    return  PerfMeasurementContext::IsMeasurementSupported();
}

ECode PerfMeasurement::NativeStartGlTimer(
    /* [in] */ Int64 contextHandle)
{
    PerfMeasurementContext *context = GetContext(contextHandle);
    android::status_t err = context->StartGlTimer();
    if (err != android::OK) {
        switch (err) {
            case android::INVALID_OPERATION:
                // jniThrowExceptionFmt(env, "java/lang/IllegalStateException",
                //         "Mismatched start/end GL timing calls");
                Logger::E(TAG, "java/lang/IllegalStateException"
                        "Mismatched start/end GL timing calls");
                return E_ILLEGAL_STATE_EXCEPTION;
            case android::BAD_VALUE:
                // jniThrowExceptionFmt(env, "java/lang/IllegalStateException",
                //         "Too many timing queries in progress, max %d",
                //         context->getMaxQueryCount());

                Logger::E(TAG, "java/lang/IllegalStateException"
                        "Too many timing queries in progress, max %d", context->GetMaxQueryCount());
                return E_ILLEGAL_STATE_EXCEPTION;
            default:
                // jniThrowExceptionFmt(env, "java/lang/IllegalStateException",
                //         "Unknown error starting GL timing");
                Logger::E(TAG, "java/lang/IllegalStateException"
                        "Unknown error starting GL timing");
                return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    //checkGlError(env);
    return NOERROR;
}

ECode PerfMeasurement::NativeStopGlTimer(
    /* [in] */ Int64 contextHandle)
{
    PerfMeasurementContext *context = GetContext(contextHandle);
    android::status_t err = context->StopGlTimer();
    if (err != android::OK) {
        switch (err) {
            case android::INVALID_OPERATION:
                // jniThrowExceptionFmt(env, "java/lang/IllegalStateException",
                //         "Mismatched start/end GL timing calls");
                Logger::E(TAG, "java/lang/IllegalStateException"
                        "Mismatched start/end GL timing calls");
                return E_ILLEGAL_STATE_EXCEPTION;
            default:
                // jniThrowExceptionFmt(env, "java/lang/IllegalStateException",
                //         "Unknown error ending GL timing");
                Logger::E(TAG, "java/lang/IllegalStateException"
                        "Unknown error ending GL timing");
                return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    //checkGlError(env);
    return NOERROR;
}

Int64 PerfMeasurement::NativeGetNextGlDuration(
    /* [in] */ Int64 contextHandle)
{
    PerfMeasurementContext *context = GetContext(contextHandle);
    nsecs_t duration = context->GetNextGlDuration();

    //checkGlError(env);
    return static_cast<Int64>(duration);
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
