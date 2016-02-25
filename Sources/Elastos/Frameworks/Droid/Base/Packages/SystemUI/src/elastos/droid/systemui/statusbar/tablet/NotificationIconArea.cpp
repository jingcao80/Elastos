#include "elastos/droid/systemui/statusbar/tablet/NotificationIconArea.h"
#include "elastos/droid/systemui/SystemUIR.h"

using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


NotificationIconArea::NotificationIconArea()
{
}

NotificationIconArea::NotificationIconArea(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : RelativeLayout(context, attrs)
{
    InitImpl(context, attrs);
}

ECode NotificationIconArea::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    RelativeLayout::Init(context, attrs);
    return InitImpl(context, attrs);
}

ECode NotificationIconArea::InitImpl(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<IView> tmpView = FindViewById(SystemUIR::id::icons);
    mIconLayout = INotificationIconAreaIconLayout::Probe(tmpView.Get());
    return NOERROR;
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
