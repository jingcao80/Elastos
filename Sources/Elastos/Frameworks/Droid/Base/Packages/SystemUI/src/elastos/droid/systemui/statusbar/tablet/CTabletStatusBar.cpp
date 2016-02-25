#include "elastos/droid/systemui/statusbar/tablet/CTabletStatusBar.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

IBASESTATUSBAR_METHODS_IMPL(CTabletStatusBar, TabletStatusBar)

PInterface CTabletStatusBar::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_TabletStatusBar) {
        return reinterpret_cast<IInterface*>((TabletStatusBar*)this);
    }
    return _CTabletStatusBar::Probe(riid);
}

ECode CTabletStatusBar::GetStatusBarHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = TabletStatusBar::GetStatusBarHeight();
    return NOERROR;
}

ECode CTabletStatusBar::OnBarHeightChanged(
    /* [in] */ Int32 height)
{
    return TabletStatusBar::OnBarHeightChanged(height);
}

ECode CTabletStatusBar::ShowClock(
    /* [in] */ Boolean show)
{
    return TabletStatusBar::ShowClock(show);
}

// called by TabletTicker when it's done with all queued ticks
ECode CTabletStatusBar::DoneTicking()
{
    return TabletStatusBar::DoneTicking();
}

ECode CTabletStatusBar::SetLightsOn(
    /* [in] */ Boolean on)
{
    return TabletStatusBar::SetLightsOn(on);
}

ECode CTabletStatusBar::OnClickRecentButton()
{
    return TabletStatusBar::OnClickRecentButton();
}

ECode CTabletStatusBar::OnClickInputMethodSwitchButton()
{
    return TabletStatusBar::OnClickInputMethodSwitchButton();
}

ECode CTabletStatusBar::OnClickCompatModeButton()
{
    return TabletStatusBar::OnClickCompatModeButton();
}

ECode CTabletStatusBar::ResetNotificationPeekFadeTimer()
{
    return TabletStatusBar::ResetNotificationPeekFadeTimer();
}

ECode CTabletStatusBar::ClearAll()
{
    return TabletStatusBar::ClearAll();
}

ECode CTabletStatusBar::AnimateCollapsePanels()
{
    return TabletStatusBar::AnimateCollapsePanels();
}

ECode CTabletStatusBar::AddIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex,
    /* [in] */ IStatusBarIcon* icon)
{
    return TabletStatusBar::AddIcon(slot, index, viewIndex, icon);
}

ECode CTabletStatusBar::UpdateIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex,
    /* [in] */ IStatusBarIcon* old,
    /* [in] */ IStatusBarIcon* icon)
{
    return TabletStatusBar::UpdateIcon(slot, index, viewIndex, old, icon);
}

ECode CTabletStatusBar::RemoveIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex)
{
    return TabletStatusBar::RemoveIcon(slot, index, viewIndex);
}

ECode CTabletStatusBar::AddNotification(
    /* [in] */ IBinder* key,
    /* [in] */ IStatusBarNotification* notification)
{
    return TabletStatusBar::AddNotification(key, notification);
}

ECode CTabletStatusBar::RemoveNotification(
    /* [in] */ IBinder* key)
{
    return TabletStatusBar::RemoveNotification(key);
}

ECode CTabletStatusBar::Disable(
    /* [in] */ Int32 state)
{
    return TabletStatusBar::Disable(state);
}

ECode CTabletStatusBar::AnimateExpandNotificationsPanel()
{
    return TabletStatusBar::AnimateExpandNotificationsPanel();
}

ECode CTabletStatusBar::AnimateCollapsePanels(
    /* [in] */ Int32 flags)
{
    return TabletStatusBar::AnimateCollapsePanels(flags);
}

ECode CTabletStatusBar::AnimateExpandSettingsPanel()
{
    return TabletStatusBar::AnimateExpandSettingsPanel();
}

ECode CTabletStatusBar::SetNavigationIconHints(
    /* [in] */ Int32 hints)
{
    return TabletStatusBar::SetNavigationIconHints(hints);
}

ECode CTabletStatusBar::SetSystemUiVisibility(
    /* [in] */ Int32 vis,
    /* [in] */ Int32 mask)
{
    return TabletStatusBar::SetSystemUiVisibility(vis, mask);
}

ECode CTabletStatusBar::TopAppWindowChanged(
    /* [in] */ Boolean showMenu)
{
    return TabletStatusBar::TopAppWindowChanged(showMenu);
}

ECode CTabletStatusBar::SetImeWindowStatus(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 vis,
    /* [in] */ Int32 backDisposition)
{
    return TabletStatusBar::SetImeWindowStatus(token, vis, backDisposition);
}

ECode CTabletStatusBar::SetHardKeyboardStatus(
    /* [in] */ Boolean available,
    /* [in] */ Boolean enabled)
{
    return TabletStatusBar::SetHardKeyboardStatus(available, enabled);
}

ECode CTabletStatusBar::ToggleRecentApps()
{
    return TabletStatusBar::ToggleRecentApps();
}

ECode CTabletStatusBar::PreloadRecentApps()
{
    return TabletStatusBar::PreloadRecentApps();
}

ECode CTabletStatusBar::ShowSearchPanel()
{
    return TabletStatusBar::ShowSearchPanel();
}

ECode CTabletStatusBar::HideSearchPanel()
{
    return TabletStatusBar::HideSearchPanel();
}

ECode CTabletStatusBar::CancelPreloadRecentApps()
{
    return TabletStatusBar::CancelPreloadRecentApps();
}

ECode CTabletStatusBar::OnHardKeyboardEnabledChange(
    /* [in] */ Boolean enabled)
{
    return TabletStatusBar::OnHardKeyboardEnabledChange(enabled);
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
