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

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/settings/location/CRadioButtonPreference.h"
#include "../R.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

CAR_INTERFACE_IMPL(CRadioButtonPreference, CheckBoxPreference, IRadioButtonPreference)

CAR_OBJECT_IMPL(CRadioButtonPreference)

CRadioButtonPreference::CRadioButtonPreference()
{}

CRadioButtonPreference::~CRadioButtonPreference()
{}

ECode CRadioButtonPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    CheckBoxPreference::constructor(context, attrs, defStyle);
    return SetWidgetLayoutResource(R::layout::preference_widget_radiobutton);
}

ECode CRadioButtonPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, Elastos::Droid::R::attr::checkBoxPreferenceStyle);
}

ECode CRadioButtonPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CRadioButtonPreference::SetOnClickListener(
    /* [in] */ IRadioButtonPreferenceOnClickListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode CRadioButtonPreference::OnClick()
{
    if (mListener != NULL) {
        mListener->OnRadioButtonClicked(this);
    }
    return NOERROR;
}

ECode CRadioButtonPreference::OnBindView(
    /* [in] */ IView* view)
{
    CheckBoxPreference::OnBindView(view);

    AutoPtr<IView> tmp;
    view->FindViewById(Elastos::Droid::R::id::title, (IView**)&tmp);
    ITextView* title = ITextView::Probe(tmp);
    if (title != NULL) {
        title->SetSingleLine(FALSE);
        title->SetMaxLines(3);
    }
    return NOERROR;
}

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos