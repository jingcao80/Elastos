//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
