
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELHOLDER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELHOLDER_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CPanelHolder.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CPanelHolder)
    , public FrameLayout
    , public IPanelHolder
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CPanelHolder();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI GetPanelIndex(
        /* [in] */ IPanelView* pv,
        /* [out] */ Int32* result);

    CARAPI SetSelectedPanel(
        /* [in] */ IPanelView* pv);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI SetBar(
        /* [in] */ IPanelBar* panelBar);

protected:
    // @Override
    CARAPI_(Int32) GetChildDrawingOrder(
        /* [in] */ Int32 childCount,
        /* [in] */ Int32 i);

    // @Override
    CARAPI OnFinishInflate();

public:
    static const Boolean DEBUG_GESTURES;

private:
    Int32 mSelectedPanelIndex;
    AutoPtr<IPanelBar> mBar;
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELHOLDER_H__
