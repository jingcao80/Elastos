#include "elastos/droid/systemui/statusbar/tablet/CNotificationLinearLayout.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


IVIEW_METHODS_IMPL(CNotificationLinearLayout, NotificationLinearLayout)
IVIEWGROUP_METHODS_IMPL(CNotificationLinearLayout, NotificationLinearLayout)
IVIEWPARENT_METHODS_IMPL(CNotificationLinearLayout, NotificationLinearLayout)
IVIEWMANAGER_METHODS_IMPL(CNotificationLinearLayout, NotificationLinearLayout)
IDRAWABLECALLBACK_METHODS_IMPL(CNotificationLinearLayout, NotificationLinearLayout)
IKEYEVENTCALLBACK_METHODS_IMPL(CNotificationLinearLayout, NotificationLinearLayout)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CNotificationLinearLayout, NotificationLinearLayout)
ILINEARLAYOUT_METHODS_IMPL(CNotificationLinearLayout, NotificationLinearLayout)


PInterface CNotificationLinearLayout::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CNotificationLinearLayout::Probe(riid);
}

ECode CNotificationLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return NotificationLinearLayout::Init(context, attrs);
}

ECode CNotificationLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle)
{
    return NotificationLinearLayout::Init(context, attrs, defStyle);
}

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
