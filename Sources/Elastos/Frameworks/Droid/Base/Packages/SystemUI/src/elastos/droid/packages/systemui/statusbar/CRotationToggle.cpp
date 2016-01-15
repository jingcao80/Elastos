#include "elastos/droid/systemui/statusbar/CRotationToggle.h"

using Elastos::Droid::View::EIID_View;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {


IVIEW_METHODS_IMPL(CRotationToggle, RotationToggle)
IDRAWABLECALLBACK_METHODS_IMPL(CRotationToggle, RotationToggle)
IKEYEVENTCALLBACK_METHODS_IMPL(CRotationToggle, RotationToggle)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CRotationToggle, RotationToggle)
ITEXTVIEW_METHODS_IMPL(CRotationToggle, RotationToggle)
ICHECKABLE_METHODS_IMPL(CRotationToggle, RotationToggle)
ICOMPOUNDBUTTON_METHODS_IMPL(CRotationToggle, RotationToggle)


PInterface CRotationToggle::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    return _CRotationToggle::Probe(riid);
}

ECode CRotationToggle::constructor(
    /* [in] */ IContext* context)
{
    return RotationToggle::Init(context);
}

ECode CRotationToggle::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return RotationToggle::Init(context, attrs);
}

ECode CRotationToggle::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return RotationToggle::Init(context, attrs, defStyle);
}

ECode CRotationToggle::SetRotationLockControlVisibility(
    /* [in] */ Boolean show)
{
    return RotationToggle::SetRotationLockControlVisibility(show);
}

ECode CRotationToggle::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = RotationToggle::OnPreDraw();
    return NOERROR;
}

ECode CRotationToggle::GetButtonDrawable(
    /* [out] */ IDrawable** d)
{
    VALIDATE_NOT_NULL(d);
    AutoPtr<IDrawable> dTemp = RotationToggle::GetButtonDrawable();
    *d = dTemp;
    REFCOUNT_ADD(*d);
    return NOERROR;
}

}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
