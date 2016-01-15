#include "elastos/droid/systemui/statusbar/policy/CDateView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::Widget::EIID_TextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


IVIEW_METHODS_IMPL(CDateView, DateView)
IDRAWABLECALLBACK_METHODS_IMPL(CDateView, DateView)
IKEYEVENTCALLBACK_METHODS_IMPL(CDateView, DateView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CDateView, DateView)
ITEXTVIEW_METHODS_IMPL(CDateView, DateView)

PInterface CDateView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_TextView) {
        return reinterpret_cast<PInterface>((TextView*)this);
    }
    return _CDateView::Probe(riid);
}

ECode CDateView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return DateView::Init(context, attrs);
}

ECode CDateView::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = DateView::OnPreDraw();
    return NOERROR;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
