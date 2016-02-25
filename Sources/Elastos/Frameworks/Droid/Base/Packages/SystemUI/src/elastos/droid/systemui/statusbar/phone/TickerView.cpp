#include "elastos/droid/systemui/statusbar/phone/TickerView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

TickerView::TickerView()
{
}

TickerView::TickerView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : TextSwitcher(context, attrs)
{
}

void TickerView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    TextSwitcher::OnSizeChanged(w, h, oldw, oldh);
    mTicker->ReflowText();
}

ECode TickerView::SetTicker(
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
