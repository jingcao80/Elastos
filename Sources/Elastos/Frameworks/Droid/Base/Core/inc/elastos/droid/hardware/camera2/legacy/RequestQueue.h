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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTQUEUE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTQUEUE_H__

#include "Elastos.Droid.Hardware.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Camera2::Utils::ILongParcelable;
using Elastos::Droid::Hardware::Camera2::Legacy::IBurstHolder;
using Elastos::Droid::Utility::IPair;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayDeque;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class RequestQueue
    : public Object
    , public IRequestQueue
{
public:
    CAR_INTERFACE_DECL()

    RequestQueue();

    virtual ~RequestQueue() {}

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IList* jpegSurfaceIds);

    /**
     * Return and remove the next burst on the queue.
     *
     * <p>If a repeating burst is returned, it will not be removed.</p>
     *
     * @return a pair containing the next burst and the current frame number, or null if none exist.
     */
    CARAPI GetNext(
        /* [out] */ IPair** outpair);

    /**
     * Cancel a repeating request.
     *
     * @param requestId the id of the repeating request to cancel.
     * @return the last frame to be returned from the HAL for the given repeating request, or
     *          {@code INVALID_FRAME} if none exists.
     */
    CARAPI StopRepeating(
        /* [in] */ Int32 requestId,
        /* [out] */ Int64* value);

    /**
     * Cancel a repeating request.
     *
     * @return the last frame to be returned from the HAL for the given repeating request, or
     *          {@code INVALID_FRAME} if none exists.
     */
    CARAPI StopRepeating(
        /* [out] */ Int64* value);

    /**
     * Add a the given burst to the queue.
     *
     * <p>If the burst is repeating, replace the current repeating burst.</p>
     *
     * @param requests the burst of requests to add to the queue.
     * @param repeating true if the burst is repeating.
     * @param frameNumber an output argument that contains either the frame number of the last frame
     *                    that will be returned for this request, or the frame number of the last
     *                    frame that will be returned for the current repeating request if this
     *                    burst is set to be repeating.
     * @return the request id.
     */
    CARAPI Submit(
        /* [in] */ IList* requests,
        /* [in] */ Boolean repeating,
        /* [in] */ ILongParcelable* frameNumber,
        /* [out] */ Int32* value);

private:
    CARAPI CalculateLastFrame(
        /* [in] */ Int32 requestId,
        /* [out] */ Int64* result);

private:
    static const String TAG;

    static const Int64 INVALID_FRAME;

    AutoPtr<IBurstHolder> mRepeatingRequest;
    AutoPtr<IArrayDeque> mRequestQueue;

    Int64 mCurrentFrameNumber;
    Int64 mCurrentRepeatingFrameNumber;
    Int32 mCurrentRequestId;
    AutoPtr<IList> mJpegSurfaceIds;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTQUEUE_H__
