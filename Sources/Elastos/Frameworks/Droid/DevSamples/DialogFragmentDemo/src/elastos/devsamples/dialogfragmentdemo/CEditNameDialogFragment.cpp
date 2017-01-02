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

#include "CEditNameDialogFragment.h"
#include "R.h"
#include <Elastos.Droid.View.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::IWindow;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DialogFragmentDemo {

static const String TAG("DialogFragmentDemo::CEditNameDialogFragment");

CAR_OBJECT_IMPL(CEditNameDialogFragment)

ECode CEditNameDialogFragment::constructor()
{
    return DialogFragment::constructor();
}

ECode CEditNameDialogFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    Boolean isLargeLayout = FALSE;
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    // res->GetBoolean(R::bool_::large_layout, &isLargeLayout);
    Logger::I(TAG, " >> isLargeLayout : %d", isLargeLayout);

    if (isLargeLayout) {
        AutoPtr<IDialog> dialog;
        GetDialog((IDialog**)&dialog);
        Boolean bval;
        dialog->RequestWindowFeature(IWindow::FEATURE_NO_TITLE, &bval);
    }

    return inflater->Inflate(R::layout::fragment_edit_name, container, FALSE, view);
}

} // namespace DialogFragmentDemo
} // namespace DevSamples
} // namespace Elastos