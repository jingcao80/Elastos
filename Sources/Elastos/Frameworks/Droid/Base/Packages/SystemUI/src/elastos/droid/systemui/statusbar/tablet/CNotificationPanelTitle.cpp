#include "elastos/droid/systemui/statusbar/tablet/CNotificationPanelTitle.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


IVIEW_METHODS_IMPL(CNotificationPanelTitle, NotificationPanelTitle)
IVIEWGROUP_METHODS_IMPL(CNotificationPanelTitle, NotificationPanelTitle)
IVIEWPARENT_METHODS_IMPL(CNotificationPanelTitle, NotificationPanelTitle)
IVIEWMANAGER_METHODS_IMPL(CNotificationPanelTitle, NotificationPanelTitle)
IDRAWABLECALLBACK_METHODS_IMPL(CNotificationPanelTitle, NotificationPanelTitle)
IKEYEVENTCALLBACK_METHODS_IMPL(CNotificationPanelTitle, NotificationPanelTitle)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CNotificationPanelTitle, NotificationPanelTitle)
IRELATIVELAYOUT_METHODS_IMPL(CNotificationPanelTitle, NotificationPanelTitle)


PInterface CNotificationPanelTitle::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CNotificationPanelTitle::Probe(riid);
}

ECode CNotificationPanelTitle::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return NotificationPanelTitle::Init(context, attrs);
}

ECode CNotificationPanelTitle::SetPanel(
    /* [in] */ INotificationPanel* b)
{
    return NotificationPanelTitle::SetPanel(b);
}

ECode CNotificationPanelTitle::OnClick(
    /* [in] */ IView* v)
{
    return NotificationPanelTitle::OnClick(v);
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
