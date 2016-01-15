#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONPANELTITLE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONPANELTITLE_H__

#include "elastos/droid/widget/RelativeLayout.h"


using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::RelativeLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


class NotificationPanelTitle : public RelativeLayout
{
public:
    NotificationPanelTitle();

    NotificationPanelTitle(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetPanel(
        /* [in] */ INotificationPanel* b);

    virtual CARAPI OnFinishInflate();

    virtual ECode SetPressed(
        /* [in] */ Boolean pressed);

    virtual Boolean OnTouchEvent(
        /* [in] */ IMotionEvent* e);

    ECode OnClick(
        /* [in] */ IView* v);

    virtual Boolean OnRequestSendAccessibilityEvent(
        /* [in] */ IView* child,
        /* [in] */ IAccessibilityEvent* event);

protected:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI InitImpl(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

private:
    AutoPtr<INotificationPanel> mPanel;
    List<AutoPtr<IView> > mButtons;
    AutoPtr<IView> mSettingsButton;
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONPANELTITLE_H__
