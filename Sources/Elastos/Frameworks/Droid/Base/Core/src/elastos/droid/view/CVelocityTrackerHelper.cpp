
#include "elastos/droid/view/CVelocityTrackerHelper.h"
#include "elastos/droid/view/VelocityTracker.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_SINGLETON_IMPL(CVelocityTrackerHelper);

CAR_INTERFACE_IMPL(CVelocityTrackerHelper, Singleton, IVelocityTrackerHelper);

ECode CVelocityTrackerHelper::Obtain(
    /* [out] */ IVelocityTracker** velocity)
{
    VALIDATE_NOT_NULL(velocity);

    AutoPtr<VelocityTracker> tracker = VelocityTracker::Obtain();
    *velocity = (IVelocityTracker*)tracker.Get();
    REFCOUNT_ADD(*velocity);
    return NOERROR;
}

ECode CVelocityTrackerHelper::Obtain(
    /* [in] */ const String& strategy,
    /* [out] */ IVelocityTracker** velocity)
{
    VALIDATE_NOT_NULL(velocity);

    AutoPtr<VelocityTracker> tracker = VelocityTracker::Obtain(strategy);
    *velocity = (IVelocityTracker*)tracker.Get();
    REFCOUNT_ADD(*velocity);
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
