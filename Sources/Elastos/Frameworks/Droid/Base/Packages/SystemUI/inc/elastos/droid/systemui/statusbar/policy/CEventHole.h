
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CEVENTHOLE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CEVENTHOLE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CEventHole.h"

#include "elastos/droid/systemui/statusbar/policy/EventHole.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CEventHole), public EventHole
{
public:
    IVIEW_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* pCtx,
        /* [in] */ IAttributeSet* pAttrs);

    CARAPI constructor(
        /* [in] */ IContext* pCtx,
        /* [in] */ IAttributeSet* pAttrs,
        /* [in] */ Int32 defStyle);

    CARAPI OnComputeInternalInsets(
        /* [in] */ IInternalInsetsInfo* inoutInfo);
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CEVENTHOLE_H__
