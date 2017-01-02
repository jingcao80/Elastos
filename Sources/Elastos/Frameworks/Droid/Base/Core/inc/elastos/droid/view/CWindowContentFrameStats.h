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

#ifndef __ELASTOS_DROID_VIEW_CWINDOWCONTENTFRAMESTATS_H__
#define __ELASTOS_DROID_VIEW_CWINDOWCONTENTFRAMESTATS_H__

#include "_Elastos_Droid_View_CWindowContentFrameStats.h"
#include "elastos/droid/view/FrameStats.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * This class contains window content frame statistics. For example, a window content
 * is rendred in frames when a view is scrolled. The frame statistics are a snapshot
 * for the time interval from {@link #getStartTimeNano()} to {@link #getEndTimeNano()}.
 * <p>
 * The key idea is that in order to provide a smooth user experience an application
 * has to draw a frame at a specific time interval obtained by calling {@link
 * #getRefreshPeriodNano()}. If the application does not render a frame every refresh
 * period the user will see irregular UI transitions.
 * </p>
 * <p>
 * An application posts a frame for presentation by synchronously rendering its contents
 * in a buffer which is then posted or posting a buffer to which the application is
 * asychronously rendering the content via GL. After the frame is posted and rendered
 * (potentially asynchronosly) it is presented to the user. The time a frame was posted
 * can be obtained via {@link #getFramePostedTimeNano(Int32)}, the time a frame content
 * was rendered and ready for dsiplay (GL case) via {@link #getFrameReadyTimeNano(Int32)},
 * and the time a frame was presented on the screen via {@link #getFramePresentedTimeNano(Int32)}.
 * </p>
 */
CarClass(CWindowContentFrameStats)
    , public FrameStats
    , public IWindowContentFrameStats
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * @hide
     */
    CWindowContentFrameStats();

    CARAPI constructor();

    /**
     * Initializes this isntance.
     *
     * @param refreshPeriodNano The display refresh period.
     * @param framesPostedTimeNano The times in milliseconds for when the frame contents were posted.
     * @param framesPresentedTimeNano The times in milliseconds for when the frame contents were presented.
     * @param framesReadyTimeNano The times in milliseconds for when the frame contents were ready to be presented.
     *
     * @hide
     */
    CARAPI_(void) Init(
        /* [in] */ Int64 refreshPeriodNano,
        /* [in] */ ArrayOf<Int64>* framesPostedTimeNano,
        /* [in] */ ArrayOf<Int64>* framesPresentedTimeNano,
        /* [in] */ ArrayOf<Int64>* framesReadyTimeNano);

    /**
     * Get the time a frame at a given index was posted by the producer (e.g. the application).
     * It is either explicitly set or defaulted to the time when the render buffer was posted.
     * <p>
     * <strong>Note:</strong> A frame can be posted and still it contents being rendered
     * asynchronously in GL. To get the time the frame content was completely rendered and
     * ready to display call {@link #getFrameReadyTimeNano(Int32)}.
     * </p>
     *
     * @param index The frame index.
     * @return The posted time in nanoseconds.
     */
    CARAPI GetFramePostedTimeNano(
        /* [in] */ Int32 index,
        /* [out] */ Int64* value);

    /**
     * Get the time a frame at a given index was ready for presentation.
     * <p>
     * <strong>Note:</strong> A frame can be posted and still it contents being rendered
     * asynchronously in GL. In such a case this is the time when the frame contents were
     * completely rendered.
     * </p>
     *
     * @param index The frame index.
     * @return The ready time in nanoseconds or {@link #UNDEFINED_TIME_NANO}
     *         if the frame is not ready yet.
     */
    CARAPI GetFrameReadyTimeNano(
        /* [in] */ Int32 index,
        /* [out] */ Int64* value);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<ArrayOf<Int64> > mFramesPostedTimeNano;
    AutoPtr<ArrayOf<Int64> > mFramesReadyTimeNano;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_VIEW_CWINDOWCONTENTFRAMESTATS_H__
