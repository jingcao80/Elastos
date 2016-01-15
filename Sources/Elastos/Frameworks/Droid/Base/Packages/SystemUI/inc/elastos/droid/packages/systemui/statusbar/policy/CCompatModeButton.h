#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CCOMPATMODEBUTTON_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CCOMPATMODEBUTTON_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CCompatModeButton.h"


#include "elastos/droid/systemui/statusbar/policy/CompatModeButton.h"

using Elastos::Droid::Widget::ImageViewScaleType;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CCompatModeButton), public CompatModeButton
{
public:
    IVIEW_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    IIMAGEVIEW_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI Refresh();

    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CCOMPATMODEBUTTON_H__
