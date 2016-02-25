#include "elastos/droid/systemui/statusbar/phone/CTickerView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {


IVIEW_METHODS_IMPL(CTickerView, TickerView)
IVIEWGROUP_METHODS_IMPL(CTickerView, TickerView)
IVIEWPARENT_METHODS_IMPL(CTickerView, TickerView)
IVIEWMANAGER_METHODS_IMPL(CTickerView, TickerView)
IDRAWABLECALLBACK_METHODS_IMPL(CTickerView, TickerView)
IKEYEVENTCALLBACK_METHODS_IMPL(CTickerView, TickerView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CTickerView, TickerView)
IFRAMELAYOUT_METHODS_IMPL(CTickerView, TickerView)


PInterface CTickerView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CTickerView::Probe(riid);
}

ECode CTickerView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TickerView::Init(context, attrs);
}

ECode CTickerView::SetTicker(
        /* [in] */ ITicker* t)
{
    return TickerView::SetTicker(t);
}


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
