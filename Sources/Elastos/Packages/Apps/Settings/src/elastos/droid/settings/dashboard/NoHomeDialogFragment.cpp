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

#include "elastos/droid/settings/dashboard/NoHomeDialogFragment.h"
#include "elastos/droid/R.h"
#include "../R.h"

using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

NoHomeDialogFragment::NoHomeDialogFragment()
{}

NoHomeDialogFragment::~NoHomeDialogFragment()
{}

void NoHomeDialogFragment::Show(
    /* [in] */ IActivity* parent)
{
    AutoPtr<NoHomeDialogFragment> dialog = new NoHomeDialogFragment();
    AutoPtr<IFragmentManager> manager;
    parent->GetFragmentManager((IFragmentManager**)&manager);
    dialog->DialogFragment::Show(manager, String(NULL));
}

ECode NoHomeDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
    builder->SetMessage(R::string::only_one_home_message);
    builder->SetPositiveButton(Elastos::Droid::R::string::ok, NULL);
    AutoPtr<IAlertDialog> alertDialog;
    builder->Create((IAlertDialog**)&alertDialog);
    *dialog = IDialog::Probe(alertDialog);
    REFCOUNT_ADD(*dialog);
    return NOERROR;
}

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos
