
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/RadioButton.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(RadioButton, CompoundButton, IRadioButton);

RadioButton::RadioButton()
{}

RadioButton::~RadioButton()
{}

ECode RadioButton::constructor(
    /* [in] */ IContext * context)
{
    return constructor(context, NULL);
}

ECode RadioButton::constructor(
    /* [in] */ IContext * context,
    /* [in] */ IAttributeSet * attrs)
{
    return constructor(context, attrs, R::attr::radioButtonStyle);
}

ECode RadioButton::constructor(
    /* [in] */ IContext * context,
    /* [in] */ IAttributeSet * attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode RadioButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return CompoundButton::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode RadioButton::Toggle()
{
    // we override to prevent toggle when the radio is already
    // checked (as opposed to check boxes widgets)
    Boolean res;
    if (IsChecked(&res), !res) {
        return CompoundButton::Toggle();
    }

    return NOERROR;
}

ECode RadioButton::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    CompoundButton::OnInitializeAccessibilityEvent(event);
    IAccessibilityRecord::Probe(event)->SetClassName(CoreUtils::Convert("CRadioButton"));
    return NOERROR;
}

ECode RadioButton::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    CompoundButton::OnInitializeAccessibilityNodeInfo(info);
    info->SetClassName(CoreUtils::Convert("CRadioButton"));
    return NOERROR;
}

} // namespace Widget
} // namepsace Droid
} // namespace Elastos
