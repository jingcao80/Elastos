
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CTABLETSTATUSBAR_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CTABLETSTATUSBAR_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Tablet_CTabletStatusBar.h"
#include "elastos/droid/systemui/statusbar/tablet/TabletStatusBar.h"


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

CarClass(CTabletStatusBar), public TabletStatusBar
{
public:
    IBASESTATUSBAR_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI GetStatusBarHeight(
        /* [out] */ Int32* height);

    CARAPI OnBarHeightChanged(
        /* [in] */ Int32 height);

    CARAPI ShowClock(
        /* [in] */ Boolean show);

    // called by TabletTicker when it's done with all queued ticks
    CARAPI DoneTicking();

    CARAPI SetLightsOn(
        /* [in] */ Boolean on);

    CARAPI OnClickRecentButton();

    CARAPI OnClickInputMethodSwitchButton();

    CARAPI OnClickCompatModeButton();

    CARAPI ResetNotificationPeekFadeTimer();

    CARAPI AnimateCollapsePanels();

    CARAPI ClearAll();

    CARAPI AddIcon(
        /* [in] */ const String& slot,
        /* [in] */ Int32 index,
        /* [in] */ Int32 viewIndex,
        /* [in] */ IStatusBarIcon* icon);

    CARAPI UpdateIcon(
        /* [in] */ const String& slot,
        /* [in] */ Int32 index,
        /* [in] */ Int32 viewIndex,
        /* [in] */ IStatusBarIcon* old,
        /* [in] */ IStatusBarIcon* icon);

    CARAPI RemoveIcon(
        /* [in] */ const String& slot,
        /* [in] */ Int32 index,
        /* [in] */ Int32 viewIndex);

    CARAPI AddNotification(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* notification);

    CARAPI RemoveNotification(
        /* [in] */ IBinder* key);

    CARAPI Disable(
        /* [in] */ Int32 state);

    CARAPI AnimateExpandNotificationsPanel();

    CARAPI AnimateCollapsePanels(
        /* [in] */ Int32 flags);

    CARAPI AnimateExpandSettingsPanel();

    CARAPI SetNavigationIconHints(
        /* [in] */ Int32 hints);

    CARAPI SetSystemUiVisibility(
        /* [in] */ Int32 vis,
        /* [in] */ Int32 mask);

    CARAPI TopAppWindowChanged(
        /* [in] */ Boolean showMenu);

    CARAPI SetImeWindowStatus(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 vis,
        /* [in] */ Int32 backDisposition);

    CARAPI SetHardKeyboardStatus(
        /* [in] */ Boolean available,
        /* [in] */ Boolean enabled);

    CARAPI ToggleRecentApps();

    CARAPI PreloadRecentApps();

    CARAPI ShowSearchPanel();

    CARAPI HideSearchPanel();

    CARAPI CancelPreloadRecentApps();

    CARAPI OnHardKeyboardEnabledChange(
        /* [in] */ Boolean enabled);
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_CTABLETSTATUSBAR_H__
