#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/CSwitch.h"

using Elastos::Droid::View::EIID_View;

namespace Elastos {
namespace Droid {
namespace Widget {

IVIEW_METHODS_IMPL(CSwitch, Switch)
IDRAWABLECALLBACK_METHODS_IMPL(CSwitch, Switch)
IKEYEVENTCALLBACK_METHODS_IMPL(CSwitch, Switch)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CSwitch, Switch)
ITEXTVIEW_METHODS_IMPL(CSwitch, Switch)
ICOMPOUNDBUTTON_METHODS_IMPL(CSwitch, Switch)
ICHECKABLE_METHODS_IMPL(CSwitch, Switch)


PInterface CSwitch::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    return _CSwitch::Probe(riid);
}

ECode CSwitch::constructor(
    /* [in] */ IContext* context)
{
    return Switch::Init(context);
}

ECode CSwitch::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Switch::Init(context, attrs);
}

ECode CSwitch::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return Switch::Init(context, attrs, defStyle);
}

ECode CSwitch::SetSwitchTextAppearance(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resid)
{
    return Switch::SetSwitchTextAppearance(context, resid);
}

ECode CSwitch::SetSwitchTypeface(
    /* [in] */ ITypeface* tf,
    /* [in] */ Int32 style)
{
    return Switch::SetSwitchTypeface(tf, style);
}

ECode CSwitch::SetSwitchTypeface(
    /* [in] */ ITypeface* tf)
{
    return Switch::SetSwitchTypeface(tf);
}

ECode CSwitch::SetSwitchPadding(
    /* [in] */ Int32 pixels)
{
    return Switch::SetSwitchPadding(pixels);
}

ECode CSwitch::GetSwitchPadding(
    /* [out] */ Int32* padding)
{
    VALIDATE_NOT_NULL(padding);
    *padding = Switch::GetSwitchPadding();
    return NOERROR;
}

ECode CSwitch::SetSwitchMinWidth(
    /* [in] */ Int32 pixels)
{
    return Switch::SetSwitchMinWidth(pixels);
}

ECode CSwitch::GetSwitchMinWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = Switch::GetSwitchMinWidth();
    return NOERROR;
}

ECode CSwitch::SetThumbTextPadding(
    /* [in] */ Int32 pixels)
{
    return Switch::SetThumbTextPadding(pixels);
}

ECode CSwitch::GetThumbTextPadding(
    /* [out] */ Int32* padding)
{
    VALIDATE_NOT_NULL(padding);
    *padding = Switch::GetThumbTextPadding();
    return NOERROR;
}

ECode CSwitch::SetTrackDrawable(
    /* [in] */ IDrawable* track)
{
    return Switch::SetTrackDrawable(track);
}

ECode CSwitch::SetTrackResource(
    /* [in] */ Int32 resId)
{
    return Switch::SetTrackResource(resId);
}

ECode CSwitch::GetTrackDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    AutoPtr<IDrawable> d = Switch::GetTrackDrawable();
    *drawable = d;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode CSwitch::SetThumbDrawable(
    /* [in] */ IDrawable* thumb)
{
    return Switch::SetThumbDrawable(thumb);
}

ECode CSwitch::SetThumbResource(
    /* [in] */ Int32 resId)
{
    return Switch::SetThumbResource(resId);
}

ECode CSwitch::GetThumbDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    AutoPtr<IDrawable> d = Switch::GetThumbDrawable();
    *drawable = d;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode CSwitch::GetTextOn(
    /* [out] */ ICharSequence** c)
{
    VALIDATE_NOT_NULL(c);
    AutoPtr<ICharSequence> chars = Switch::GetTextOn();
    *c = chars;
    REFCOUNT_ADD(*c);
    return NOERROR;
}

ECode CSwitch::SetTextOn(
    /* [in] */ ICharSequence* textOn)
{
    return Switch::SetTextOn(textOn);
}

ECode CSwitch::GetTextOff(
    /* [out] */ ICharSequence** c)
{
    VALIDATE_NOT_NULL(c);
    AutoPtr<ICharSequence> chars = Switch::GetTextOff();
    *c = chars;
    REFCOUNT_ADD(*c);
    return NOERROR;
}

ECode CSwitch::SetTextOff(
    /* [in] */ ICharSequence* textOff)
{
    return Switch::SetTextOff(textOff);
}

ECode CSwitch::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Switch::OnPreDraw();

    return NOERROR;
}

ECode CSwitch::GetButtonDrawable(
    /* [out] */ IDrawable** d)
{
    AutoPtr<IDrawable> dTemp = Switch::GetButtonDrawable();
    *d = dTemp;
    REFCOUNT_ADD(*d);
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
