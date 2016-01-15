#include "elastos/droid/systemui/CUniverseBackground.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {


IVIEW_METHODS_IMPL(CUniverseBackground, UniverseBackground)
IVIEWGROUP_METHODS_IMPL(CUniverseBackground, UniverseBackground)
IVIEWPARENT_METHODS_IMPL(CUniverseBackground, UniverseBackground)
IVIEWMANAGER_METHODS_IMPL(CUniverseBackground, UniverseBackground)
IDRAWABLECALLBACK_METHODS_IMPL(CUniverseBackground, UniverseBackground)
IKEYEVENTCALLBACK_METHODS_IMPL(CUniverseBackground, UniverseBackground)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CUniverseBackground, UniverseBackground)
IFRAMELAYOUT_METHODS_IMPL(CUniverseBackground, UniverseBackground)

PInterface CUniverseBackground::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CUniverseBackground::Probe(riid);
}

ECode CUniverseBackground::constructor(
    /* [in] */ IContext* context)
{
    return UniverseBackground::Init(context);
}

ECode CUniverseBackground::GetWindowManagerLayoutParams(
    /* [out] */ IWindowManagerLayoutParams** lp)
{
    VALIDATE_NOT_NULL(lp);
    AutoPtr<IWindowManagerLayoutParams> temp = UniverseBackground::GetWindowManagerLayoutParams();
    *lp = temp;
    REFCOUNT_ADD(*lp);
    return NOERROR;
}

ECode CUniverseBackground::AnimateCollapse()
{
    return UniverseBackground::AnimateCollapse();
}

ECode CUniverseBackground::AnimateCollapse(
    /* [in] */ Float velocityMultiplier)
{
    return UniverseBackground::AnimateCollapse(velocityMultiplier);
}

ECode CUniverseBackground::ConsumeEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    return UniverseBackground::ConsumeEvent(event, result);
}

}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
