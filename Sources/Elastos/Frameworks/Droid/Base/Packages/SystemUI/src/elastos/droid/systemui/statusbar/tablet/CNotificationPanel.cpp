#include "elastos/droid/systemui/statusbar/tablet/CNotificationPanel.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


IVIEW_METHODS_IMPL(CNotificationPanel, NotificationPanel)
IVIEWGROUP_METHODS_IMPL(CNotificationPanel, NotificationPanel)
IVIEWPARENT_METHODS_IMPL(CNotificationPanel, NotificationPanel)
IVIEWMANAGER_METHODS_IMPL(CNotificationPanel, NotificationPanel)
IDRAWABLECALLBACK_METHODS_IMPL(CNotificationPanel, NotificationPanel)
IKEYEVENTCALLBACK_METHODS_IMPL(CNotificationPanel, NotificationPanel)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CNotificationPanel, NotificationPanel)
IRELATIVELAYOUT_METHODS_IMPL(CNotificationPanel, NotificationPanel)


PInterface CNotificationPanel::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CNotificationPanel::Probe(riid);
}

ECode CNotificationPanel::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return NotificationPanel::Init(context, attrs);
}

ECode CNotificationPanel::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return NotificationPanel::Init(context, attrs, defStyle);
}

ECode CNotificationPanel::SetBar(
    /* [in] */ ITabletStatusBar* b)
{
    return NotificationPanel::SetBar(b);
}

ECode CNotificationPanel::GetClearButton(
    /* [out] */ IView** button)
{
    VALIDATE_NOT_NULL(button);
    AutoPtr<IView> view = NotificationPanel::GetClearButton();
    *button = view;
    REFCOUNT_ADD(*button);
    return NOERROR;
}

ECode CNotificationPanel::Show(
    /* [in] */ Boolean show,
    /* [in] */ Boolean animate)
{
    return NotificationPanel::Show(show, animate);
}

ECode CNotificationPanel::IsShowing(
    /* [out] */ Boolean* isShowing)
{
    VALIDATE_NOT_NULL(isShowing);
    *isShowing = NotificationPanel::IsShowing();
    return NOERROR;
}

ECode CNotificationPanel::SetNotificationCount(
    /* [in] */ Int32 n)
{
    return NotificationPanel::SetNotificationCount(n);
}

ECode CNotificationPanel::SetContentFrameVisible(
    /* [in] */ Boolean showing,
    /* [in] */ Boolean animate)
{
    return NotificationPanel::SetContentFrameVisible(showing, animate);
}

ECode CNotificationPanel::SwapPanels()
{
    return NotificationPanel::SwapPanels();
}

ECode CNotificationPanel::UpdateClearButton()
{
    return NotificationPanel::UpdateClearButton();
}

ECode CNotificationPanel::SetClearable(
    /* [in] */ Boolean clearable)
{
    return NotificationPanel::SetClearable(clearable);
}

ECode CNotificationPanel::UpdatePanelModeButtons()
{
    return NotificationPanel::UpdatePanelModeButtons();
}

ECode CNotificationPanel::OnClick(
    /* [in] */ IView* v)
{
    return NotificationPanel::OnClick(v);
}

ECode CNotificationPanel::IsInContentArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NotificationPanel::IsInContentArea(x, y);
    return NOERROR;
}

ECode CNotificationPanel::SetSettingsEnabled(
    /* [in] */ Boolean settingsEnabled)
{
    return NotificationPanel::SetSettingsEnabled(settingsEnabled);
}

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
