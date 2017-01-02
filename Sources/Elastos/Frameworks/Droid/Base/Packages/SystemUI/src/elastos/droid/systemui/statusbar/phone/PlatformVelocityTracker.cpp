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

#include "elastos/droid/systemui/statusbar/phone/PlatformVelocityTracker.h"

using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::IVelocityTrackerHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

AutoPtr<Pools::SynchronizedPool<IVelocityTrackerInterface> > PlatformVelocityTracker::sPool
    = new Pools::SynchronizedPool<IVelocityTrackerInterface>(2);
CAR_INTERFACE_IMPL(PlatformVelocityTracker, Object, IVelocityTrackerInterface)
AutoPtr<PlatformVelocityTracker> PlatformVelocityTracker::Obtain()
{
    AutoPtr<PlatformVelocityTracker> tracker = (PlatformVelocityTracker*)sPool->AcquireItem().Get();
    if (tracker == NULL) {
        tracker = new PlatformVelocityTracker();
    }
    AutoPtr<IVelocityTrackerHelper> helper;
    CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
    AutoPtr<IVelocityTracker> vt;
    helper->Obtain((IVelocityTracker**)&vt);
    tracker->SetTracker(vt);
    return tracker;
}

ECode PlatformVelocityTracker::SetTracker(
    /* [in] */ IVelocityTracker* tracker)
{
    mTracker = tracker;
    return NOERROR;
}

ECode PlatformVelocityTracker::AddMovement(
    /* [in] */ IMotionEvent* event)
{
    mTracker->AddMovement(event);
    return NOERROR;
}

ECode PlatformVelocityTracker::ComputeCurrentVelocity(
    /* [in] */ Int32 units)
{
    mTracker->ComputeCurrentVelocity(units);
    return NOERROR;
}

ECode PlatformVelocityTracker::GetXVelocity(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    return mTracker->GetXVelocity(result);
}

ECode PlatformVelocityTracker::GetYVelocity(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    return mTracker->GetYVelocity(result);
}

ECode PlatformVelocityTracker::Recycle()
{
    mTracker->Recycle();
    sPool->ReleaseItem(this);
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
