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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/preference/CheckBoxPreference.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(CheckBoxPreference, TwoStatePreference, ICheckBoxPreference)

CheckBoxPreference::CheckBoxPreference()
{
}

ECode CheckBoxPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CheckBoxPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(TwoStatePreference::constructor(context, attrs, defStyleAttr, 0));

    AutoPtr< ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::CheckBoxPreference);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    String summaryOn;
    a->GetString(R::styleable::CheckBoxPreference_summaryOn, &summaryOn);
    AutoPtr<ICharSequence> cs;
    CString::New(summaryOn, (ICharSequence**)&cs);
    SetSummaryOn(cs);

    String summaryOff;
    a->GetString(R::styleable::CheckBoxPreference_summaryOff, &summaryOff);
    cs = NULL;
    CString::New(summaryOff, (ICharSequence**)&cs);
    SetSummaryOff(cs);

    Boolean disableDependentsState;
    a->GetBoolean(R::styleable::CheckBoxPreference_disableDependentsState, FALSE, &disableDependentsState);

    SetDisableDependentsState(disableDependentsState);
    a->Recycle();

    return NOERROR;
}

ECode CheckBoxPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::checkBoxPreferenceStyle);
}

ECode CheckBoxPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CheckBoxPreference::OnBindView(
    /* [in] */ IView* view)
{
    TwoStatePreference::OnBindView(view);

    AutoPtr<IView> checkboxView;
    view->FindViewById(R::id::checkbox, (IView**)&checkboxView);
    if (checkboxView != NULL && ICheckable::Probe(checkboxView) != NULL) {
        ICheckable::Probe(checkboxView)->SetChecked(mChecked);
    }

    SyncSummaryView(view);
    return NOERROR;
}

}
}
}