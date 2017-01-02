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

#include "elastos/droid/settings/CBugreportPreference.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;

namespace Elastos {
namespace Droid {
namespace Settings {

CAR_OBJECT_IMPL(CBugreportPreference)

CBugreportPreference::CBugreportPreference()
{}

CBugreportPreference::~CBugreportPreference()
{}

ECode CBugreportPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return DialogPreference::constructor(context, attrs);
}

ECode CBugreportPreference::OnPrepareDialogBuilder(
    /* [in] */ IAlertDialogBuilder* builder)
{
    DialogPreference::OnPrepareDialogBuilder(builder);
    builder->SetPositiveButton(Elastos::Droid::R::string::report, this);
    builder->SetMessage(Elastos::Droid::R::string::bugreport_message);
    return NOERROR;
}

ECode CBugreportPreference::ShowDialog(
    /* [in] */ IBundle* state)
{
    return DialogPreference::ShowDialog(state);
}

ECode CBugreportPreference::OnBindDialogView(
    /* [in] */ IView* view)
{
    return DialogPreference::OnBindDialogView(view);
}

ECode CBugreportPreference::OnDialogClosed(
    /* [in] */ Boolean positiveResult)
{
    return DialogPreference::OnDialogClosed(positiveResult);
}

ECode CBugreportPreference::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (which == IDialogInterface::BUTTON_POSITIVE) {
        AutoPtr<ISystemProperties> sp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
        sp->Set(String("ctl.start"), String("bugreport"));
    }
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos