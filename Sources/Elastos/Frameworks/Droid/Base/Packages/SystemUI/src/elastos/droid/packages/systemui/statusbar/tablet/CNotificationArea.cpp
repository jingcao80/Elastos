#include "elastos/droid/systemui/statusbar/tablet/CNotificationArea.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


IVIEW_METHODS_IMPL(CNotificationArea, NotificationArea)
IVIEWGROUP_METHODS_IMPL(CNotificationArea, NotificationArea)
IVIEWPARENT_METHODS_IMPL(CNotificationArea, NotificationArea)
IVIEWMANAGER_METHODS_IMPL(CNotificationArea, NotificationArea)
IDRAWABLECALLBACK_METHODS_IMPL(CNotificationArea, NotificationArea)
IKEYEVENTCALLBACK_METHODS_IMPL(CNotificationArea, NotificationArea)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CNotificationArea, NotificationArea)
ILINEARLAYOUT_METHODS_IMPL(CNotificationArea, NotificationArea)


PInterface CNotificationArea::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CNotificationArea::Probe(riid);
}

ECode CNotificationArea::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return NotificationArea::Init(context, attrs);
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
