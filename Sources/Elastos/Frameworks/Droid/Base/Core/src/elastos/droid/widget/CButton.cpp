
#include "elastos/droid/widget/CButton.h"

using Elastos::Droid::View::EIID_View;
namespace Elastos {
namespace Droid {
namespace Widget {

IVIEW_METHODS_IMPL(CButton, Button)
IDRAWABLECALLBACK_METHODS_IMPL(CButton, Button)
IKEYEVENTCALLBACK_METHODS_IMPL(CButton, Button)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CButton, Button)
ITEXTVIEW_METHODS_IMPL(CButton, Button)

PInterface CButton::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_TextView) {
        return reinterpret_cast<PInterface>((TextView*)this);
    }
    return _CButton::Probe(riid);
}

ECode CButton::constructor(
    /* [in] */ IContext * pCtx)
{
    return Button::Init(pCtx);
}

ECode CButton::constructor(
    /* [in] */ IContext * pCtx,
    /* [in] */ IAttributeSet * pAttrs)
{
    return Button::Init(pCtx, pAttrs);
}

ECode CButton::constructor(
    /* [in] */ IContext * pContext,
    /* [in] */ IAttributeSet * pAttrs,
    /* [in] */ Int32 defStyle)
{
    return Button::Init(pContext, pAttrs, defStyle);
}

ECode CButton::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Button::OnPreDraw();

    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
