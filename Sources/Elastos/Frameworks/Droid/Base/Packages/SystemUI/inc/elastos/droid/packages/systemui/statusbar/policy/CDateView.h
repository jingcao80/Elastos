#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDATEVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDATEVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CDateView.h"


#include "elastos/droid/systemui/statusbar/policy/DateView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CDateView), public DateView
{
public:
    IVIEW_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    ITEXTVIEW_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI OnPreDraw(
        /* [out] */ Boolean* result);
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDATEVIEW_H__
