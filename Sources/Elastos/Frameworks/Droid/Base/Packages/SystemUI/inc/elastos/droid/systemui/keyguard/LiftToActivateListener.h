
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