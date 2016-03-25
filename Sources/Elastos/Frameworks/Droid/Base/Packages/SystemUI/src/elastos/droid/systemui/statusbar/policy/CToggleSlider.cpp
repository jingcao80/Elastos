#include "elastos/droid/systemui/statusbar/policy/CToggleSlider.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


IVIEW_METHODS_IMPL(CToggleSlider, ToggleSlider)
IVIEWGROUP_METHODS_IMPL(CToggleSlider, ToggleSlider)
IVIEWPARENT_METHODS_IMPL(CToggleSlider, ToggleSlider)
IVIEWMANAGER_METHODS_IMPL(CToggleSlider, ToggleSlider)
IDRAWABLECALLBACK_METHODS_IMPL(CToggleSlider, ToggleSlider)
IKEYEVENTCALLBACK_METHODS_IMPL(CToggleSlider, ToggleSlider)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CToggleSlider, ToggleSlider)
IRELATIVELAYOUT_METHODS_IMPL(CToggleSlider, ToggleSlider)

PInterface CToggleSlider::Probe(
    /*  [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>(this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CToggleSlider::Probe(riid);
}

ECode CToggleSlider::constructor(
    /* [in] */ IContext* context)
{
    return ToggleSlider::Init(context);
}

ECode CToggleSlider::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ToggleSlider::Init(context, attrs);
}

ECode CToggleSlider::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return ToggleSlider::Init(context, attrs, defStyle);
}

ECode CToggleSlider::SetOnChangedListener(
    /* [in] */ IToggleSliderListener* listener)
{
    return ToggleSlider::SetOnChangedListener(listener);
}

ECode CToggleSlider::SetChecked(
    /* [in] */ Boolean clearable)
{
    return ToggleSlider::SetChecked(clearable);
}

ECode CToggleSlider::IsChecked(
    /* [out] */ Boolean* settingsEnabled)
{
    VALIDATE_NOT_NULL(settingsEnabled);
    *settingsEnabled = ToggleSlider::IsChecked();
    return NOERROR;
}

ECode CToggleSlider::SetMax(
    /* [in] */ Int32 maxValue)
{
    return ToggleSlider::SetMax(maxValue);
}

ECode CToggleSlider::SetValue(
    /* [in] */ Int32 value)
{
    return ToggleSlider::SetValue(value);
}

ECode CToggleSlider::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    return ToggleSlider::OnCheckedChanged(buttonView, isChecked);
}

ECode CToggleSlider::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    return ToggleSlider::OnProgressChanged(seekBar, progress, fromUser);
}

ECode CToggleSlider::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    return ToggleSlider::OnStartTrackingTouch(seekBar);
}

ECode CToggleSlider::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    return ToggleSlider::OnStopTrackingTouch(seekBar);
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
