#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CPanelView.h"



#include "elastos/droid/systemui/statusbar/phone/PanelView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CPanelView), public PanelView
{
public:
    IVIEW_METHODS_DECL()
    IVIEWGROUP_METHODS_DECL()
    IVIEWPARENT_METHODS_DECL()
    IVIEWMANAGER_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    IFRAMELAYOUT_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetRubberbandingEnabled(
        /* [in] */ Boolean enabled);

    CARAPI Fling(
        /* [in] */ Float vel,
        /* [in] */ Boolean always);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetExpandedHeight(
        /* [in] */ Float height);

    CARAPI SetExpandedHeightInternal(
        /* [in] */ Float h);

    CARAPI GetHandle(
        /* [out] */ IView** height);

    CARAPI SetExpandedFraction(
        /* [in] */ Float frac);

    CARAPI GetExpandedHeight(
        /* [out] */ Float* height);

    CARAPI GetExpandedFraction(
        /* [out] */ Float* fraction);

    CARAPI IsFullyExpanded(
        /* [out] */ Boolean* isFullyExpanded);

    CARAPI IsFullyCollapsed(
        /* [out] */ Boolean* isFullyCollapsed);

    CARAPI IsCollapsing(
        /* [out] */ Boolean* isCollapsing);

    CARAPI SetBar(
        /* [in] */ IPanelBar* panelBar);

    CARAPI Collapse();

    CARAPI Expand();
};


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELVIEW_H__
