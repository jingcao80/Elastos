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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PLATFORMVELOCITYTRACKER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PLATFORMVELOCITYTRACKER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/utility/Pools.h>
#include <elastos/core/Object.h>

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::Utility::Pools;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * An implementation of {@link VelocityTrackerInterface} using the platform-standard
 * {@link VelocityTracker}.
 */
class PlatformVelocityTracker
    : public Object
    , public IVelocityTrackerInterface
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<PlatformVelocityTracker>) Obtain();

    CARAPI SetTracker(
        /* [in] */ IVelocityTracker* tracker);

    // @Override
    CARAPI AddMovement(
        /* [in] */ IMotionEvent* event);

    // @Override
    CARAPI ComputeCurrentVelocity(
        /* [in] */ Int32 units);

    // @Override
    CARAPI GetXVelocity(
        /* [out] */ Float* result);

    // @Override
    CARAPI GetYVelocity(
        /* [out] */ Float* result);

    // @Override
    CARAPI Recycle();

private:
    static AutoPtr<Pools::SynchronizedPool<IVelocityTrackerInterface> > sPool;

    AutoPtr<IVelocityTracker> mTracker;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PLATFORMVELOCITYTRACKER_H__
