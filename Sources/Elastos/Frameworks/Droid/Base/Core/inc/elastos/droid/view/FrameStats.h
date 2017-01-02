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

#ifndef __ELASTOS_DROID_VIEW_FRAMESTATE_H__
#define __ELASTOS_DROID_VIEW_FRAMESTATE_H__

#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace View {

class FrameStats
    : public Object
    , public IFrameStats
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Gets the refresh period of the display hosting the window(s) for
     * which these statistics apply.
     *
     * @return The refresh period in nanoseconds.
     */
    CARAPI GetRefreshPeriodNano(
        /* [out] */ Int64* nano);

    /**
     * Gets the number of frames for which there is data.
     *
     * @return The number of frames.
     */
    CARAPI GetFrameCount(
        /* [out] */ Int32* count);

    /**
     * Gets the start time of the interval for which these statistics
     * apply. The start interval is the time when the first frame was
     * presented.
     *
     * @return The start time in nanoseconds or {@link #UNDEFINED_TIME_NANO}
     *         if there is no frame data.
     */
    CARAPI GetStartTimeNano(
        /* [out] */ Int64* nano);

    /**
     * Gets the end time of the interval for which these statistics
     * apply. The end interval is the time when the last frame was
     * presented.
     *
     * @return The end time in nanoseconds or {@link #UNDEFINED_TIME_NANO}
     *         if there is no frame data.
     */
    CARAPI GetEndTimeNano(
        /* [out] */ Int64* nano);

    /**
     * Get the time a frame at a given index was presented.
     *
     * @param index The frame index.
     * @return The presented time in nanoseconds or {@link #UNDEFINED_TIME_NANO}
     *         if the frame is not presented yet.
     */
    CARAPI GetFramePresentedTimeNano(
        /* [in] */ Int32 index,
        /* [out] */ Int64* nano);

protected:
    /** @hide */
    Int64 mRefreshPeriodNano;

    /** @hide */
    AutoPtr<ArrayOf<Int64> > mFramesPresentedTimeNano;
};

} // namespace View
} // namespace Droid
} // namespace Elastos
#endif