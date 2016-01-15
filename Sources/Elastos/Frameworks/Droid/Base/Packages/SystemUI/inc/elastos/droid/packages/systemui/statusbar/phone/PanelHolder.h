#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELHOLDER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELHOLDER_H__

#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {


class PanelHolder : public FrameLayout
{
public:
    PanelHolder();

    PanelHolder(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(Int32) GetPanelIndex(
        /* [in] */ IPanelView* panelView);

    CARAPI SetSelectedPanel(
        /* [in] */ IPanelView* panelView);

    CARAPI SetBar(
        /* [in] */ IPanelBar* panelBar);

    //@Override
    virtual Boolean OnTouchEvent(
        /* [in] */ IMotionEvent* event);

protected:
    virtual CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    virtual Int32 GetChildDrawingOrder(
        /* [in] */ Int32 childCount,
        /* [in] */ Int32 i);

private:
    Int32 mSelectedPanelIndex;// = -1;
    AutoPtr<IPanelBar> mBar;
};


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PANELHOLDER_H__
