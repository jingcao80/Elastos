#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONPEEKPANEL_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONPEEKPANEL_H__

#include "elastos/droid/widget/RelativeLayout.h"

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::RelativeLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


class NotificationPeekPanel : public RelativeLayout
{
public:
    NotificationPeekPanel();

    NotificationPeekPanel(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    NotificationPeekPanel(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    Boolean IsInContentArea(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI SetBar(
        /* [in] */ ITabletStatusBar* bar);

    // We don't really want to intercept the touch event, but we *do* want to reset the fade timer
    // in case the user is interacting with some custom controls or something.
    //@Override
    virtual Boolean OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev);

    virtual Boolean DispatchHoverEvent(
        /* [in] */ IMotionEvent* event);

private:
    AutoPtr<ITabletStatusBar> mBar;
};


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONPEEKPANEL_H__
