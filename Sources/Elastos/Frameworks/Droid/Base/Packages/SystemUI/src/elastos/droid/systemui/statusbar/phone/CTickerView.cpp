
#include "elastos/droid/systemui/statusbar/phone/CTickerView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_OBJECT_IMPL(CTickerView)
CAR_INTERFACE_IMPL(CTickerView, TextSwitcher, ITickerView)
ECode CTickerView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TextSwitcher::constructor(context, attrs);
}

void CTickerView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    TextSwitcher::OnSizeChanged(w, h, oldw, oldh);
    if (mTicker != NULL) mTicker->ReflowText();
}

ECode CTickerView::SetTicker(
    /* [in] */ ITicker* t)
{
    mTicker = t;
    return NOERROR;
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
