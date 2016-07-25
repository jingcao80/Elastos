
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CTICKERVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CTICKERVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CTickerView.h"
#include <elastos/droid/widget/TextSwitcher.h>

using Elastos::Droid::Widget::TextSwitcher;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CTickerView)
    , public TextSwitcher
    , public ITickerView
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetTicker(
        /* [in] */ ITicker* t);

protected:
    // @Override
    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

private:
    AutoPtr<ITicker> mTicker;
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CTICKERVIEW_H__
