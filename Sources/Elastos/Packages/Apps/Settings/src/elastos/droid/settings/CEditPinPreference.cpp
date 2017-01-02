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

#include "elastos/droid/settings/CEditPinPreference.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Settings {

CAR_INTERFACE_IMPL(CEditPinPreference, EditTextPreference, IEditPinPreference)

CAR_OBJECT_IMPL(CEditPinPreference)

CEditPinPreference::CEditPinPreference()
{}

CEditPinPreference::~CEditPinPreference()
{}

ECode CEditPinPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return EditTextPreference::constructor(context, attrs);
}

ECode CEditPinPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return EditTextPreference::constructor(context, attrs, defStyleAttr);
}

ECode CEditPinPreference::SetOnPinEnteredListener(
    /* [in] */ IEditPinPreferenceOnPinEnteredListener* listener)
{
    mPinListener = listener;
    return NOERROR;
}

ECode CEditPinPreference::OnBindDialogView(
    /* [in] */ IView* view)
{
    EditTextPreference::OnBindDialogView(view);

    AutoPtr<IEditText> editText;
    GetEditText((IEditText**)&editText);

    if (editText != NULL) {
        ITextView::Probe(editText)->SetInputType(IInputType::TYPE_CLASS_NUMBER | IInputType::TYPE_NUMBER_VARIATION_PASSWORD);
    }
    return NOERROR;
}

Boolean CEditPinPreference::IsDialogOpen()
{
    AutoPtr<IDialog> dialog;
    GetDialog((IDialog**)&dialog);
    Boolean res;
    return dialog != NULL && (dialog->IsShowing(&res), res);
}

ECode CEditPinPreference::OnDialogClosed(
    /* [in] */ Boolean positiveResult)
{
    EditTextPreference::OnDialogClosed(positiveResult);
    if (mPinListener != NULL) {
        mPinListener->OnPinEntered(this, positiveResult);
    }
    return NOERROR;
}

ECode CEditPinPreference::ShowPinDialog()
{
    AutoPtr<IDialog> dialog;
    GetDialog((IDialog**)&dialog);
    Boolean res;
    if (dialog == NULL || (dialog->IsShowing(&res), !res)) {
        ShowDialog(NULL);
    }
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos