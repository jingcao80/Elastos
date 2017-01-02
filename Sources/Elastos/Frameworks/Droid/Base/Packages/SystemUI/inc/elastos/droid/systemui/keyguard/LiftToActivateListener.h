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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_LIFETOACTIVITELISTENER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_LIFETOACTIVITELISTENER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnHoverListener;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Hover listener that implements lift-to-activate interaction for
 * accessibility. May be added to multiple views.
 */
class LiftToActivateListener
    : public Object
    , public IViewOnHoverListener
{
public:
    TO_STRING_IMPL("LiftToActivateListener")

    CAR_INTERFACE_DECL()

    LiftToActivateListener();

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnHover(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

private:
    /** Manager used to query accessibility enabled state. */
    AutoPtr<IAccessibilityManager> mAccessibilityManager;

    Boolean mCachedClickableState;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_LIFETOACTIVITELISTENER_H__