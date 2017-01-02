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

#include "elastos/droid/systemui/statusbar/phone/VelocityTrackerFactory.h"
#include "elastos/droid/systemui/statusbar/phone/NoisyVelocityTracker.h"
#include "elastos/droid/systemui/statusbar/phone/PlatformVelocityTracker.h"
#include "../R.h"
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
