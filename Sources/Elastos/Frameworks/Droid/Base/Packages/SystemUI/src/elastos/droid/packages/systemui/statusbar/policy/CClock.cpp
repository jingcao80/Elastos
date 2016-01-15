#include "elastos/droid/systemui/statusbar/policy/CClock.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::Widget::EIID_TextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

IVIEW_METHODS_IMPL(CClock, Clock);
IDRAWABLECALLBACK_METHODS_IMPL(CClock, Clock);
IKEYEVENTCALLBACK_METHODS_IMPL(CClock, Clock);
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CClock, Clock);
ITEXTVIEW_METHODS_IMPL(CClock, Clock);

PInterface CClock::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_TextView) {
        return reinterpret_cast<PInterface>((TextView*)this);
    }
    return _CClock::Probe(riid);
}

ECode CClock::constructor(
    /* [in] */ IContext * pCtx)
{
    return Clock::Init(pCtx);
}

ECode CClock::constructor(
    /* [in] */ IContext * pCtx,
    /* [in] */ IAttributeSet * pAttrs)
{
    return Clock::Init(pCtx, pAttrs);
}

ECode CClock::constructor(
    /* [in] */ IContext * pContext,
    /* [in] */ IAttributeSet * pAttrs,
    /* [in] */ Int32 defStyle)
{
    return Clock::Init(pContext, pAttrs, defStyle);
}

ECode CClock::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Clock::OnPreDraw();
    return NOERROR;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
