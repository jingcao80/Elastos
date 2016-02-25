#include "elastos/droid/systemui/statusbar/policy/CEventHole.h"

using Elastos::Droid::View::EIID_View;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


IVIEW_METHODS_IMPL(CEventHole, EventHole)
IDRAWABLECALLBACK_METHODS_IMPL(CEventHole, EventHole)
IKEYEVENTCALLBACK_METHODS_IMPL(CEventHole, EventHole)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CEventHole, EventHole)

PInterface CEventHole::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    return _CEventHole::Probe(riid);
}

ECode CEventHole::constructor(
    /* [in] */ IContext* pCtx,
    /* [in] */ IAttributeSet* pAttrs)
{
    return EventHole::Init(pCtx, pAttrs);
}

ECode CEventHole::constructor(
    /* [in] */ IContext* pCtx,
    /* [in] */ IAttributeSet* pAttrs,
    /* [in] */ Int32 defStyle)
{
    return EventHole::Init(pCtx, pAttrs, defStyle);
}

ECode CEventHole::OnComputeInternalInsets(
    /* [in] */ IInternalInsetsInfo* inoutInfo)
{
    return EventHole::OnComputeInternalInsets(inoutInfo);
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
