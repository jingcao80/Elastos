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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CAPTURECOLLECTOR_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CAPTURECOLLECTOR_H__

#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IPair;
using Elastos::Core::Object;
using Elastos::Core::IInteger64;
using Elastos::Core::IComparable;
using Elastos::Utility::ITreeSet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrayDeque;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::Locks::ILock;
using Elastos::Utility::Concurrent::Locks::ICondition;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class CaptureCollector
    : public Object
    , public ICaptureCollector
{
private:
    class CaptureHolder
        : public Object
        , public ICaptureCollectorCaptureHolder
        , public IComparable
    {
    public:
        CAR_INTERFACE_DECL()

        CaptureHolder(
            /* [in] */ IRequestHolder* request,
            /* [in] */ ILegacyRequest* legacyHolder,
            /* [in] */ CaptureCollector* host);

        CARAPI_(Boolean) IsPreviewCompleted();

        CARAPI_(Boolean) IsJpegCompleted();

        CARAPI_(Boolean) IsCompleted();

        CARAPI_(void) TryComplete();

        CARAPI SetJpegTimestamp(
            /* in] */ Int64 timestamp);

        CARAPI SetJpegProduced();

        CARAPI_(void) SetJpegFailed();

        CARAPI SetPreviewTimestamp(
            /* [in] */ Int64 timestamp);

        CARAPI SetPreviewProduced();

        CARAPI_(void) SetPreviewFailed();

        // Comparison and equals based on frame number.
        //@Override
        CARAPI CompareTo(
            /* [in] */ IInterface* another,
            /* [out] */ Int32* result);

        // Comparison and equals based on frame number.
        //@Override
        CARAPI Equals(
            /* [in] */ IInterface* other,
            /* [out] */ Boolean* result);

    private:
        friend class CaptureCollector;

        AutoPtr<IRequestHolder> mRequest;
        AutoPtr<ILegacyRequest> mLegacy;
        Boolean mNeedsJpeg;
        Boolean mNeedsPreview;

        Int64 mTimestamp;
        Int32 mReceivedFlags;
        Boolean mHasStarted;
        Boolean mFailedJpeg;
        Boolean mFailedPreview;
        Boolean mCompleted;
        Boolean mPreviewCompleted;

        CaptureCollector* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CaptureCollector();

    virtual ~CaptureCollector() {}

    CARAPI constructor();

    /**
     * Create a new {@link CaptureCollector} that can modify the given {@link CameraDeviceState}.
     *
     * @param maxInFlight max allowed in-flight requests.
     * @param deviceState the {@link CameraDeviceState} to update as requests are processed.
     */
    CARAPI constructor(
        /* [in] */ Int32 maxInFlight,
        /* [in] */ ICameraDeviceState* deviceState);

    /**
     * Queue a new request.
     *
     * <p>
     * For requests that use the Camera1 API preview output stream, this will block if there are
     * already {@code maxInFlight} requests in progress (until at least one prior request has
     * completed). For requests that use the Camera1 API jpeg callbacks, this will block until
     * all prior requests have been completed to avoid stopping preview for
     * {@link android.hardware.Camera#takePicture} before prior preview requests have been
     * completed.
     * </p>
     * @param holder the {@link RequestHolder} for this request.
     * @param legacy the {@link LegacyRequest} for this request; this will not be mutated.
     * @param timeout a timeout to use for this call.
     * @param unit the units to use for the timeout.
     * @return {@code false} if this method timed out.
     * @throws InterruptedException if this thread is interrupted.
     */
    CARAPI QueueRequest(
        /* [in] */ IRequestHolder* holder,
        /* [in] */ ILegacyRequest* legacy,
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ Boolean* value);

    /**
     * Wait all queued requests to complete.
     *
     * @param timeout a timeout to use for this call.
     * @param unit the units to use for the timeout.
     * @return {@code false} if this method timed out.
     * @throws InterruptedException if this thread is interrupted.
     */
    CARAPI WaitForEmpty(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ Boolean* value);

    /**
     * Wait all queued requests that use the Camera1 API preview output to complete.
     *
     * @param timeout a timeout to use for this call.
     * @param unit the units to use for the timeout.
     * @return {@code false} if this method timed out.
     * @throws InterruptedException if this thread is interrupted.
     */
    CARAPI WaitForPreviewsEmpty(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ Boolean* value);

    /**
     * Wait for the specified request to be completed (all buffers available).
     *
     * <p>May not wait for the same request more than once, since a successful wait
     * will erase the history of that request.</p>
     *
     * @param holder the {@link RequestHolder} for this request.
     * @param timeout a timeout to use for this call.
     * @param unit the units to use for the timeout.
     * @param timestamp the timestamp of the request will be written out to here, in ns
     *
     * @return {@code false} if this method timed out.
     *
     * @throws InterruptedException if this thread is interrupted.
     */
    CARAPI WaitForRequestCompleted(
        /* [in] */IRequestHolder* holder,
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [in] */ IInteger64* timestamp,
        /* [out] */ Boolean* value);

    /**
     * Called to alert the {@link CaptureCollector} that the jpeg capture has begun.
     *
     * @param timestamp the time of the jpeg capture.
     * @return the {@link RequestHolder} for the request associated with this capture.
     */
    CARAPI JpegCaptured(
        /* [in] */ Int64 timestamp,
        /* [out] */ IRequestHolder** outrh);

    /**
     * Called to alert the {@link CaptureCollector} that the jpeg capture has completed.
     *
     * @return a pair containing the {@link RequestHolder} and the timestamp of the capture.
     */
    CARAPI JpegProduced(
        /* [out] */ IPair** outpair);

    /**
     * Check if there are any pending capture requests that use the Camera1 API preview output.
     *
     * @return {@code true} if there are pending preview requests.
     */
    CARAPI HasPendingPreviewCaptures(
        /* [out] */ Boolean* value);

    /**
     * Called to alert the {@link CaptureCollector} that the preview capture has begun.
     *
     * @param timestamp the time of the preview capture.
     * @return a pair containing the {@link RequestHolder} and the timestamp of the capture.
     */
    CARAPI PreviewCaptured(
        /* [in] */ Int64 timestamp,
        /* [out] */ IPair** outpair);

    /**
     * Called to alert the {@link CaptureCollector} that the preview capture has completed.
     *
     * @return the {@link RequestHolder} for the request associated with this capture.
     */
    CARAPI PreviewProduced(
        /* [out] */ IRequestHolder** outrh);

    /**
     * Called to alert the {@link CaptureCollector} that the next pending preview capture has failed.
     */
    CARAPI FailNextPreview();

    /**
     * Called to alert the {@link CaptureCollector} that the next pending jpeg capture has failed.
     */
    CARAPI FailNextJpeg();

    /**
     * Called to alert the {@link CaptureCollector} all pending captures have failed.
     */
    CARAPI FailAll();

private:
    CARAPI_(Boolean) RemoveRequestIfCompleted(
        /* [in] */ IRequestHolder* holder,
        /* [in] */ IInteger64* timestamp);

    CARAPI OnPreviewCompleted();

    CARAPI OnRequestCompleted(
        /* [in] */ CaptureHolder* capture);

private:
    static const String TAG;

    static const Boolean DEBUG;

    static const Int32 FLAG_RECEIVED_JPEG;
    static const Int32 FLAG_RECEIVED_JPEG_TS;
    static const Int32 FLAG_RECEIVED_PREVIEW;
    static const Int32 FLAG_RECEIVED_PREVIEW_TS;
    static const Int32 FLAG_RECEIVED_ALL_JPEG;
    static const Int32 FLAG_RECEIVED_ALL_PREVIEW;

    static const Int32 MAX_JPEGS_IN_FLIGHT ;

    AutoPtr<ITreeSet> mActiveRequests;
    AutoPtr<IArrayDeque> mJpegCaptureQueue;
    AutoPtr<IArrayDeque> mJpegProduceQueue;
    AutoPtr<IArrayDeque> mPreviewCaptureQueue;
    AutoPtr<IArrayDeque> mPreviewProduceQueue;
    AutoPtr<IArrayList> mCompletedRequests;

    AutoPtr<ILock> mLock;
    AutoPtr<ICondition> mIsEmpty;
    AutoPtr<ICondition> mPreviewsEmpty;
    AutoPtr<ICondition> mNotFull;
    AutoPtr<ICameraDeviceState> mDeviceState;
    Int32 mInFlight;
    Int32 mInFlightPreviews;
    Int32 mMaxInFlight;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CAPTURECOLLECTOR_H__
