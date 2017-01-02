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

#ifndef __ELASTOS_DROID_VIEW_CWINDOWANIMATIONFRAMESTATS_H__
#define __ELASTOS_DROID_VIEW_CWINDOWANIMATIONFRAMESTATS_H__

#include "_Elastos_Droid_View_CWindowAnimationFrameStats.h"
#include "elastos/droid/view/FrameStats.h"

namespace Elastos {
namespace Droid {
namespace View {

/**
 * This class contains window animation frame statistics. For example, a window
 * animation is usually performed when the application is transitioning from one
 * activity to another. The frame statistics are a snapshot for the time interval
 * from {@link #getStartTimeNano()} to {@link #getEndTimeNano()}.
 * <p>
 * The key idea is that in order to provide a smooth user experience the system should
 * run window animations at a specific time interval obtained by calling {@link
 * #getRefreshPeriodNano()}. If the system does not render a frame every refresh
 * period the user will see irregular window transitions. The time when the frame was
 * actually presented on the display by calling {@link #getFramePresentedTimeNano(int)}.
 */
CarClass(CWindowAnimationFrameStats)
    , public FrameStats
    , public IWindowAnimationFrameStats
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * @hide
     */
    CWindowAnimationFrameStats();

    CARAPI constructor();

    /**
     * Initializes this isntance.
     *
     * @param refreshPeriodNano The display refresh period.
     * @param framesPresentedTimeNano The presented frame times.
     *
     * @hide
     */
    CARAPI_(void) Init(
        /* [in] */ Int64 refreshPeriodNano,
        /* [in] */ ArrayOf<Int64>* framesPresentedTimeNano);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_CWINDOWANIMATIONFRAMESTATS_H__
