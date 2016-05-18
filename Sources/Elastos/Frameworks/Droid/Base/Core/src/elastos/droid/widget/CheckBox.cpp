
#include "elastos/droid/widget/CheckBox.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(CheckBox, CompoundButton, ICheckBox)

CheckBox::CheckBox()
{}

ECode CheckBox::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CheckBox::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::checkboxStyle);
}

ECode CheckBox::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CheckBox::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return CompoundButton::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode CheckBox::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FAIL_RETURN(CompoundButton::OnInitializeAccessibilityEvent(event));
    AutoPtr<ICharSequence> txt = CoreUtils::Convert(String("CheckBox"));
    return IAccessibilityRecord::Probe(event)->SetClassName(txt);
}

ECode CheckBox::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    FAIL_RETURN(CompoundButton::OnInitializeAccessibilityNodeInfo(info));
    AutoPtr<ICharSequence> txt = CoreUtils::Convert(String("CheckBox"));
    return info->SetClassName(txt);
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
