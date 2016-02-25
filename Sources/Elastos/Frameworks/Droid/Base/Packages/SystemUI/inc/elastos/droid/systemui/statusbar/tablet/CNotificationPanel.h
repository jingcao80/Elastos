#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CNOTIFICATIONPANEL_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CNOTIFICATIONPANEL_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Tablet_CNotificationPanel.h"



#include "elastos/droid/systemui/statusbar/tablet/NotificationPanel.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

CarClass(CNotificationPanel), public NotificationPanel
{
public:
    IVIEW_METHODS_DECL()
    IVIEWGROUP_METHODS_DECL()
    IVIEWPARENT_METHODS_DECL()
    IVIEWMANAGER_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    IRELATIVELAYOUT_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /*  [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetBar(
        /* [in] */ ITabletStatusBar* b);

    CARAPI GetClearButton(
        /* [out] */ IView** button);

    CARAPI Show(
        /* [in] */ Boolean show,
        /* [in] */ Boolean animate);

    CARAPI IsShowing(
        /* [out] */ Boolean* isShowing);

    CARAPI SetNotificationCount(
        /* [in] */ Int32 n);

    CARAPI SetContentFrameVisible(
        /* [in] */ Boolean showing,
        /* [in] */ Boolean animate);

    CARAPI SwapPanels();

    CARAPI UpdateClearButton();

    CARAPI SetClearable(
        /* [in] */ Boolean clearable);

    CARAPI UpdatePanelModeButtons();

    CARAPI SetSettingsEnabled(
        /* [in] */ Boolean settingsEnabled);

    CARAPI OnClick(
        /* [in] */ IView* v);

    CARAPI IsInContentArea(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Boolean* result);
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CNOTIFICATIONPANEL_H__
