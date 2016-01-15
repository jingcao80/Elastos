#include "elastos/droid/systemui/statusbar/tablet/NotificationIconAreaIconLayout.h"

using Elastos::Droid::View::EIID_IView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

NotificationIconAreaIconLayout::NotificationIconAreaIconLayout()
{
}

NotificationIconAreaIconLayout::NotificationIconAreaIconLayout(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : LinearLayout(context, attrs)
{}

ECode NotificationIconAreaIconLayout::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::Init(context, attrs);
}

Boolean NotificationIconAreaIconLayout::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    return TRUE;
}

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
