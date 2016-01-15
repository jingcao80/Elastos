#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDEADZONE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDEADZONE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CDeadZone.h"

#include "elastos/droid/systemui/statusbar/policy/DeadZone.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CDeadZone), public DeadZone
{
public:
    IVIEW_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetFlashOnTouchCapture(
        /* [in] */ Boolean dbg);

    CARAPI Poke(
        /* [in] */ IMotionEvent* event);

    CARAPI SetFlash(
        /* [in] */ Float f);

    CARAPI GetFlash(
        /* [out] */ Float* f);
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDEADZONE_H__
