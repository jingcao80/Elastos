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

#include "elastos/droid/internal/app/MediaRouteControllerDialogFragment.h"
#include "elastos/droid/internal/app/CMediaRouteControllerDialog.h"
#include "elastos/droid/R.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(MediaRouteControllerDialogFragment, DialogFragment, IMediaRouteControllerDialogFragment)

ECode MediaRouteControllerDialogFragment::constructor()
{
    DialogFragment::constructor();
    SetCancelable(TRUE);
    SetStyle(STYLE_NORMAL, R::style::Theme_DeviceDefault_Dialog);
    return NOERROR;
}

ECode MediaRouteControllerDialogFragment::OnCreateControllerDialog(
    /* [in] */ IContext* context,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IMediaRouteControllerDialog** dialog)
{
    Int32 theme;
    GetTheme(&theme);
    return CMediaRouteControllerDialog::New(context, theme, dialog);
}

ECode MediaRouteControllerDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IMediaRouteControllerDialog> mrcDialog;
    OnCreateControllerDialog(IContext::Probe(activity), savedInstanceState,
        (IMediaRouteControllerDialog**)&mrcDialog);
    *dialog = IDialog::Probe(mrcDialog);
    REFCOUNT_ADD(*dialog)
    return NOERROR;
}

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos
