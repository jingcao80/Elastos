#include "elastos/droid/systemui/statusbar/tablet/CNotificationIconArea.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


IVIEW_METHODS_IMPL(CNotificationIconArea, NotificationIconArea)
IVIEWGROUP_METHODS_IMPL(CNotificationIconArea, NotificationIconArea)
IVIEWPARENT_METHODS_IMPL(CNotificationIconArea, NotificationIconArea)
IVIEWMANAGER_METHODS_IMPL(CNotificationIconArea, NotificationIconArea)
IDRAWABLECALLBACK_METHODS_IMPL(CNotificationIconArea, NotificationIconArea)
IKEYEVENTCALLBACK_METHODS_IMPL(CNotificationIconArea, NotificationIconArea)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CNotificationIconArea, NotificationIconArea)
IRELATIVELAYOUT_METHODS_IMPL(CNotificationIconArea, NotificationIconArea)


PInterface CNotificationIconArea::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CNotificationIconArea::Probe(riid);
}

ECode CNotificationIconArea::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return NotificationIconArea::Init(context, attrs);
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
