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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_NOISYVELOCITYTRACKER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_NOISYVELOCITYTRACKER_H__

#include "_Elastos.Droid.SystemUI.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/droid/utility/Pools.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::Pools;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Core::Object;
using Elastos::Utility::IArrayDeque;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * A very simple low-pass velocity filter for motion events for noisy touch screens.
 */
class NoisyVelocityTracker
    : public Object
    , public INoisyVelocityTracker
    , public IVelocityTrackerInterface
{
private:
    class MotionEventCopy: public Object
    {
    public:
        MotionEventCopy(
            /* [in] */ Float x2,
            /* [in] */ Float y2,
            /* [in] */ Int64 eventTime);

    public:
        Float mX;
        Float mY;
        Int64 mT;
    };

public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<NoisyVelocityTracker>) Obtain();

    CARAPI AddMovement(
        /* [in] */ IMotionEvent* event);

    CARAPI ComputeCurrentVelocity(
        /* [in] */ Int32 units);

    CARAPI GetXVelocity(
        /* [out] */ Float* result);

    CARAPI GetYVelocity(
        /* [out] */ Float* result);

    CARAPI Recycle();

private:
    NoisyVelocityTracker();

private:
    static AutoPtr<Pools::SynchronizedPool<INoisyVelocityTracker> > sNoisyPool;

    static const Float DECAY;
    static const Boolean DEBUG;

    const Int32 MAX_EVENTS;
    AutoPtr<IArrayDeque> mEventBuf;  /*<MotionEventCopy*/
    Float mVX;
    Float mVY;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_NOISYVELOCITYTRACKER_H__
