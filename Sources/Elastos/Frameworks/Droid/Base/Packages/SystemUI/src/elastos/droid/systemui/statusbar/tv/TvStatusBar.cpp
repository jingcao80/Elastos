
#include "elastos/droid/systemui/statusbar/tv/TvStatusBar.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace TV {

CAR_INTERFACE_IMPL(TvStatusBar, BaseStatusBar, ITvStatusBar);
ECode TvStatusBar::AddIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex,
    /* [in] */ IStatusBarIcon* icon)
{
    return NOERROR;
}

ECode TvStatusBar::UpdateIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex,
    /* [in] */ IStatusBarIcon* old,
    /* [in] */ IStatusBarIcon* icon)
{
    return NOERROR;
}

ECode TvStatusBar::RemoveIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex)
{
    return NOERROR;
}

ECode TvStatusBar::AddNotification(
    /* [in] */ IStatusBarNotification* notification,
    /* [in] */ INotificationListenerServiceRankingMap* ranking)
{
    return NOERROR;
}

void TvStatusBar::UpdateNotificationRanking(
    /* [in] */ INotificationListenerServiceRankingMap* ranking)
{
}

ECode TvStatusBar::RemoveNotification(
    /* [in] */ const String& key,
    /* [in] */ INotificationListenerServiceRankingMap* ranking)
{
    return NOERROR;
}

ECode TvStatusBar::Disable(
    /* [in] */ Int32 state,
    /* [in] */ Boolean animate)
{
    return NOERROR;
}

ECode TvStatusBar::AnimateExpandNotificationsPanel()
{
    return NOERROR;
}

ECode TvStatusBar::AnimateCollapsePanels(
    /* [in] */ Int32 flags)
{
    return NOERROR;
}

ECode TvStatusBar::SetSystemUiVisibility(
    /* [in] */ Int32 vis,
    /* [in] */ Int32 mask)
{
    return NOERROR;
}

ECode TvStatusBar::TopAppWindowChanged(
    /* [in] */ Boolean visible)
{
    return NOERROR;
}

ECode TvStatusBar::SetImeWindowStatus(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 vis,
    /* [in] */ Int32 backDisposition,
    /* [in] */ Boolean showImeSwitcher)
{
    return NOERROR;
}

ECode TvStatusBar::ToggleRecentApps()
{
    return NOERROR;
}

ECode TvStatusBar::SetWindowState(
    /* [in] */ Int32 window,
    /* [in] */ Int32 state)
{
    return NOERROR;
}

ECode TvStatusBar::BuzzBeepBlinked()
{
    return NOERROR;
}

ECode TvStatusBar::NotificationLightOff()
{
    return NOERROR;
}

ECode TvStatusBar::NotificationLightPulse(
    /* [in] */ Int32 argb,
    /* [in] */ Int32 onMillis,
    /* [in] */ Int32 offMillis)
{
    return NOERROR;
}

AutoPtr<IWindowManagerLayoutParams> TvStatusBar::GetSearchLayoutParams(
    /* [in] */ IViewGroupLayoutParams* layoutParams)
{
    return NULL;
}

void TvStatusBar::HaltTicker()
{
}

void TvStatusBar::SetAreThereNotifications()
{
}

void TvStatusBar::UpdateNotifications()
{
}

void TvStatusBar::Tick(
    /* [in] */ IStatusBarNotification* n,
    /* [in] */ Boolean firstTime)
{
}

void TvStatusBar::UpdateExpandedViewPos(
    /* [in] */ Int32 expandedPosition)
{
}

Boolean TvStatusBar::ShouldDisableNavbarGestures()
{
    return TRUE;
}

ECode TvStatusBar::GetStatusBarView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;
    return NOERROR;
}

ECode TvStatusBar::ResetHeadsUpDecayTimer()
{
    return NOERROR;
}

ECode TvStatusBar::ScheduleHeadsUpOpen()
{
    return NOERROR;
}

ECode TvStatusBar::ScheduleHeadsUpEscalation()
{
    return NOERROR;
}

ECode TvStatusBar::ScheduleHeadsUpClose()
{
    return NOERROR;
}

Int32 TvStatusBar::GetMaxKeyguardNotifications()
{
    return 0;
}

ECode TvStatusBar::AnimateExpandSettingsPanel()
{
    return NOERROR;
}

void TvStatusBar::CreateAndAddWindows()
{
}

void TvStatusBar::RefreshLayout(
    /* [in] */ Int32 layoutDirection)
{
}

ECode TvStatusBar::OnActivated(
    /* [in] */ IActivatableNotificationView* view)
{
    return NOERROR;
}

ECode TvStatusBar::OnActivationReset(
    /* [in] */ IActivatableNotificationView* view)
{
    return NOERROR;
}

} // namespace TV
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
