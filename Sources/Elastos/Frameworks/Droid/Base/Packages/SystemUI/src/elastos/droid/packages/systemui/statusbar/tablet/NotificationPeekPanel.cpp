#include "elastos/droid/systemui/statusbar/tablet/NotificationPeekPanel.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


NotificationPeekPanel::NotificationPeekPanel()
{}

NotificationPeekPanel::NotificationPeekPanel(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : RelativeLayout(context, attrs)
{}

NotificationPeekPanel::NotificationPeekPanel(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : RelativeLayout(context, attrs, defStyle)
{}

Boolean NotificationPeekPanel::IsInContentArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 l = GetPaddingLeft();
    Int32 r = GetWidth() - GetPaddingRight();
    Int32 t = GetPaddingTop();
    Int32 b = GetHeight() - GetPaddingBottom();
    return x >= l && x < r && y >= t && y < b;
}

ECode NotificationPeekPanel::SetBar(
    /* [in] */ ITabletStatusBar* bar)
{
    mBar = bar;
    return NOERROR;
}

Boolean NotificationPeekPanel::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    mBar->ResetNotificationPeekFadeTimer();
    return FALSE;
}

Boolean NotificationPeekPanel::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    // Ignore hover events outside of this panel bounds since such events
    // generate spurious accessibility events with the panel content when
    // tapping outside of it, thus confusing the user.
    Float fx, fy;
    event->GetX(&fx);
    event->GetX(&fy);
    Int32 x = fx;
    Int32 y = fy;
    if (x >= 0 && x < GetWidth() && y >= 0 && y < GetHeight()) {
        return RelativeLayout::DispatchHoverEvent(event);
    }
    return TRUE;
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
