#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_LATESTITEMVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_LATESTITEMVIEW_H__

#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::Widget::FrameLayout;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {


class LatestItemView : public FrameLayout
{
public:
    LatestItemView();

    LatestItemView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    virtual CARAPI SetOnClickListener(
        /* [in] */ IViewOnClickListener* l);

    virtual Boolean OnRequestSendAccessibilityEvent(
        /* [in] */ IView* child,
        /* [in] */ IAccessibilityEvent* event);
};


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_LATESTITEMVIEW_H__
