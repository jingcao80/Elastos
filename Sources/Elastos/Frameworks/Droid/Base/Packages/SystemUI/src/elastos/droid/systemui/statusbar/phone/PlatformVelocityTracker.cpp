
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
CAR_INTERFACE_IMPL(PlatformVelocityTracker, Object, IVelocityTrackerInterface);
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
