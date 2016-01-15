#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/systemui/statusbar/policy/CDeadZone.h"

using Elastos::Droid::View::EIID_View;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


IVIEW_METHODS_IMPL(CDeadZone, DeadZone)
IDRAWABLECALLBACK_METHODS_IMPL(CDeadZone, DeadZone)
IKEYEVENTCALLBACK_METHODS_IMPL(CDeadZone, DeadZone)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CDeadZone, DeadZone)

PInterface CDeadZone::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    return _CDeadZone::Probe(riid);
}

ECode CDeadZone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return DeadZone::Init(context, attrs);
}

ECode CDeadZone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return DeadZone::Init(context, attrs, defStyle);
}

ECode CDeadZone::SetFlashOnTouchCapture(
    /* [in] */ Boolean dbg)
{
    return DeadZone::SetFlashOnTouchCapture(dbg);
}

ECode CDeadZone::Poke(
    /* [in] */ IMotionEvent* event)
{
    return DeadZone::Poke(event);
}

ECode CDeadZone::SetFlash(
    /* [in] */ Float f)
{
    return DeadZone::SetFlash(f);
}

ECode CDeadZone::GetFlash(
    /* [out] */ Float* f)
{
    VALIDATE_NOT_NULL(f);
    *f = DeadZone::GetFlash();
    return NOERROR;
}


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
