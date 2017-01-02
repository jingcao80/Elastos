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

#include "CFireMissilesDialogFragment.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DialogFragmentDemo {

static const String TAG("DialogFragmentDemo::CFireMissilesDialogFragment");

CAR_OBJECT_IMPL(CFireMissilesDialogFragment)

ECode CFireMissilesDialogFragment::constructor()
{
    return DialogFragment::constructor();
}

ECode CFireMissilesDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    *dialog = NULL;

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);

    // Add action buttons
    AutoPtr<ICharSequence> csq = CoreUtils::Convert("Fire missiles");
    builder->SetMessage(csq);

    // AutoPtr<IDialogInterfaceOnClickListener> listener = new DialogInterfaceOnClickListener(this);
    csq = CoreUtils::Convert("Fire");
    builder->SetPositiveButton(csq, NULL/*listener*/);
    csq = CoreUtils::Convert("Cancel");
    builder->SetNegativeButton(csq, NULL/*listener*/);

    AutoPtr<IAlertDialog> ad;
    ECode ec = builder->Create((IAlertDialog**)&ad);
    assert(ad != NULL);
    if (FAILED(ec)) return ec;

    *dialog = IDialog::Probe(ad);
    REFCOUNT_ADD(*dialog);
    return NOERROR;
}

} // namespace DialogFragmentDemo
} // namespace DevSamples
} // namespace Elastos