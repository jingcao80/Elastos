#include "elastos/droid/widget/internal/CDigitalClock2.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

IVIEW_METHODS_IMPL(CDigitalClock2, DigitalClock)

IVIEWGROUP_METHODS_IMPL(CDigitalClock2, DigitalClock)

IVIEWPARENT_METHODS_IMPL(CDigitalClock2, DigitalClock)

IVIEWMANAGER_METHODS_IMPL(CDigitalClock2, DigitalClock)

IDRAWABLECALLBACK_METHODS_IMPL(CDigitalClock2, DigitalClock)

IKEYEVENTCALLBACK_METHODS_IMPL(CDigitalClock2, DigitalClock)

IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CDigitalClock2, DigitalClock)

PInterface CDigitalClock2::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>(this);
    }
    if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CDigitalClock2::Probe(riid);
}

ECode CDigitalClock2::constructor(
    /* [in] */ IContext* ctx)
{
    return Init(ctx);
}

ECode CDigitalClock2::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return Init(ctx, attrs);
}

ECode CDigitalClock2::UpdateTime()
{
    return DigitalClock::UpdateTime();
}

ECode CDigitalClock2::SetIgnoreGravity(
    /* [in] */ Int32 viewId)
{
    return DigitalClock::SetIgnoreGravity(viewId);
}

ECode CDigitalClock2::GetGravity(
    /* [out] */ Int32* gravity)
{
    *gravity = DigitalClock::GetGravity();
    return NOERROR;
}

ECode CDigitalClock2::SetGravity(
    /* [in] */ Int32 gravity)
{
    return DigitalClock::SetGravity(gravity);
}

ECode CDigitalClock2::SetHorizontalGravity(
    /* [in] */ Int32 horizontalGravity)
{
    return DigitalClock::SetHorizontalGravity(horizontalGravity);
}

ECode CDigitalClock2::SetVerticalGravity(
    /* [in] */ Int32 verticalGravity)
{
    return DigitalClock::SetVerticalGravity(verticalGravity);
}

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
