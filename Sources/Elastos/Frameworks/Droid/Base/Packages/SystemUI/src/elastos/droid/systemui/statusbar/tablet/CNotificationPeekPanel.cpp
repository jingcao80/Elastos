#include "elastos/droid/systemui/statusbar/tablet/CNotificationPeekPanel.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


IVIEW_METHODS_IMPL(CNotificationPeekPanel, NotificationPeekPanel)
IVIEWGROUP_METHODS_IMPL(CNotificationPeekPanel, NotificationPeekPanel)
IVIEWPARENT_METHODS_IMPL(CNotificationPeekPanel, NotificationPeekPanel)
IVIEWMANAGER_METHODS_IMPL(CNotificationPeekPanel, NotificationPeekPanel)
IDRAWABLECALLBACK_METHODS_IMPL(CNotificationPeekPanel, NotificationPeekPanel)
IKEYEVENTCALLBACK_METHODS_IMPL(CNotificationPeekPanel, NotificationPeekPanel)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CNotificationPeekPanel, NotificationPeekPanel)
IRELATIVELAYOUT_METHODS_IMPL(CNotificationPeekPanel, NotificationPeekPanel)


PInterface CNotificationPeekPanel::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CNotificationPeekPanel::Probe(riid);
}

ECode CNotificationPeekPanel::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return NotificationPeekPanel::Init(context, attrs);
}

ECode CNotificationPeekPanel::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return NotificationPeekPanel::Init(context, attrs);
}

ECode CNotificationPeekPanel::IsInContentArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NotificationPeekPanel::IsInContentArea(x, y);
    return NOERROR;
}

ECode CNotificationPeekPanel::SetBar(
    /* [in] */ ITabletStatusBar* bar)
{
    return NotificationPeekPanel::SetBar(bar);
}

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
