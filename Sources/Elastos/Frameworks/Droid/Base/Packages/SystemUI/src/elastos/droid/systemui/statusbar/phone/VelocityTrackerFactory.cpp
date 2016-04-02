
#include "elastos/droid/systemui/statusbar/phone/VelocityTrackerFactory.h"
#include "elastos/droid/systemui/statusbar/phone/NoisyVelocityTracker.h"
#include "elastos/droid/systemui/statusbar/phone/PlatformVelocityTracker.h"
#include "../../R.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const String VelocityTrackerFactory::PLATFORM_IMPL("platform");
const String VelocityTrackerFactory::NOISY_IMPL("noisy");
AutoPtr<IVelocityTrackerInterface> VelocityTrackerFactory::Obtain(
    /* [in] */ IContext* ctx)
{
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    String tracker;
    res->GetString(R::string::velocity_tracker_impl, &tracker);
    if(tracker.Equals(NOISY_IMPL)) {
        return NoisyVelocityTracker::Obtain();
    }
    else if (tracker.Equals(PLATFORM_IMPL)) {
        return PlatformVelocityTracker::Obtain();
    }
    assert(0 && "Invalid tracker: ");
    // throw new IllegalStateException("Invalid tracker: " + tracker);
    return NULL;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
