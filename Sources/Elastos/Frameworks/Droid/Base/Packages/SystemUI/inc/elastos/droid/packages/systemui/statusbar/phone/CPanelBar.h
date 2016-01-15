#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELBAR_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELBAR_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CPanelBar.h"



#include "elastos/droid/systemui/statusbar/phone/PanelBar.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CPanelBar), public PanelBar
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

    CARAPI Go(
        /* [in] */ Int32 state);

    CARAPI AddPanel(
        /* [in] */ IPanelView* pv);

    CARAPI SetPanelHolder(
        /* [in] */ IPanelHolder* ph);

    CARAPI GetBarHeight(
        /* [out] */ Float* height);

    CARAPI SelectPanelForTouch(
        /* [in] */ IMotionEvent* touch,
        /* [out] */ IPanelView** view);

    CARAPI PanelsEnabled(
        /* [out] */ Boolean* enabled);

    // called from PanelView when self-expanding, too
    CARAPI StartOpeningPanel(
        /* [in] */ IPanelView* pv);

    CARAPI PanelExpansionChanged(
        /* [in] */ IPanelView* pv,
        /* [in] */ Float frac);

    CARAPI CollapseAllPanels(
        /* [in] */ Boolean animate);

    CARAPI OnPanelPeeked();

    CARAPI OnAllPanelsCollapsed();

    CARAPI OnPanelFullyOpened(
        /* [in] */ IPanelView* openPanel);

    CARAPI OnTrackingStarted(
        /* [in] */ IPanelView* panel);

    CARAPI OnTrackingStopped(
        /* [in] */ IPanelView* panel);
};


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELBAR_H__
