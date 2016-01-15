
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/CVelocityTrackerHelper.h"
#include "elastos/droid/view/VelocityTracker.h"

namespace Elastos {
namespace Droid {
namespace View {

ECode CVelocityTrackerHelper::Obtain(
    /* [out] */ IVelocityTracker** velocity)
{
    VALIDATE_NOT_NULL(velocity);

    AutoPtr<VelocityTracker> tracker = VelocityTracker::Obtain();
    *velocity = (IVelocityTracker*)tracker->Probe(EIID_IVelocityTracker);
    REFCOUNT_ADD(*velocity);
    return NOERROR;
}

ECode CVelocityTrackerHelper::Obtain(
    /* [in] */ const String& strategy,
    /* [out] */ IVelocityTracker** velocity)
{
    VALIDATE_NOT_NULL(velocity);

    AutoPtr<VelocityTracker> tracker = VelocityTracker::Obtain(strategy);
    *velocity = (IVelocityTracker*)tracker->Probe(EIID_IVelocityTracker);
    REFCOUNT_ADD(*velocity);
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
