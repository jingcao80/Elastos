
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_LIFETOACTIVITELISTENER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_LIFETOACTIVITELISTENER_H__

#include "_Elastos.Droid.SystemUI.h"

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