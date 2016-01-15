#include "elastos/droid/systemui/statusbar/tablet/CNotificationIconAreaIconLayout.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

IVIEW_METHODS_IMPL(CNotificationIconAreaIconLayout, NotificationIconAreaIconLayout)
IVIEWGROUP_METHODS_IMPL(CNotificationIconAreaIconLayout, NotificationIconAreaIconLayout)
IVIEWPARENT_METHODS_IMPL(CNotificationIconAreaIconLayout, NotificationIconAreaIconLayout)
IVIEWMANAGER_METHODS_IMPL(CNotificationIconAreaIconLayout, NotificationIconAreaIconLayout)
IDRAWABLECALLBACK_METHODS_IMPL(CNotificationIconAreaIconLayout, NotificationIconAreaIconLayout)
IKEYEVENTCALLBACK_METHODS_IMPL(CNotificationIconAreaIconLayout, NotificationIconAreaIconLayout)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CNotificationIconAreaIconLayout, NotificationIconAreaIconLayout)
ILINEARLAYOUT_METHODS_IMPL(CNotificationIconAreaIconLayout, NotificationIconAreaIconLayout)


PInterface CNotificationIconAreaIconLayout::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CNotificationIconAreaIconLayout::Probe(riid);
}

ECode CNotificationIconAreaIconLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return NotificationIconAreaIconLayout::Init(context, attrs);
}

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

