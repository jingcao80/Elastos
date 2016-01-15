#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONICONAREAICONLAYOUT_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONICONAREAICONLAYOUT_H__

#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

class NotificationIconAreaIconLayout : public LinearLayout
{
public:
    NotificationIconAreaIconLayout();

    NotificationIconAreaIconLayout(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    virtual CARAPI_(Boolean) OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev);
protected:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONICONAREAICONLAYOUT_H__
