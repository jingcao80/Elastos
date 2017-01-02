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

#include "elastos/droid/internal/app/MediaRouteChooserDialogFragment.h"
#include "elastos/droid/internal/app/CMediaRouteChooserDialog.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Os::CBundle;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(MediaRouteChooserDialogFragment, DialogFragment, IMediaRouteChooserDialogFragment)

MediaRouteChooserDialogFragment::MediaRouteChooserDialogFragment()
    : ARGUMENT_ROUTE_TYPES("routeTypes")
{
}

ECode MediaRouteChooserDialogFragment::constructor()
{
    DialogFragment::constructor();
    SetCancelable(TRUE);
    SetStyle(STYLE_NORMAL, R::style::Theme_DeviceDefault_Dialog);
    return NOERROR;
}

ECode MediaRouteChooserDialogFragment::GetRouteTypes(
    /* [out] */ Int32* types)
{
    VALIDATE_NOT_NULL(types)
    *types = 0;
    AutoPtr<IBundle> args;
    GetArguments((IBundle**)&args);
    if (args != NULL)
        args->GetInt32(ARGUMENT_ROUTE_TYPES, types);
    return NOERROR;
}

ECode MediaRouteChooserDialogFragment::SetRouteTypes(
    /* [in] */ Int32 types)
{
    Int32 oldTypes;
    GetRouteTypes(&oldTypes);
    if (types != oldTypes) {
        AutoPtr<IBundle> args;
        GetArguments((IBundle**)&args);
        if (args == NULL) {
            CBundle::New((IBundle**)&args);
        }
        args->PutInt32(ARGUMENT_ROUTE_TYPES, types);
        SetArguments(args);

        AutoPtr<IDialog> _dialog;
        GetDialog((IDialog**)&_dialog);
        AutoPtr<IMediaRouteChooserDialog> dialog = IMediaRouteChooserDialog::Probe(_dialog);
        if (dialog != NULL) {
            dialog->SetRouteTypes(types);
        }
    }
    return NOERROR;
}

ECode MediaRouteChooserDialogFragment::SetExtendedSettingsClickListener(
    /* [in] */ IViewOnClickListener* listener)
{
    if (listener != mExtendedSettingsClickListener) {
        mExtendedSettingsClickListener = listener;

        AutoPtr<IDialog> _dialog;
        GetDialog((IDialog**)&_dialog);
        AutoPtr<IMediaRouteChooserDialog> dialog = IMediaRouteChooserDialog::Probe(_dialog);
        if (dialog != NULL) {
            dialog->SetExtendedSettingsClickListener(listener);
        }
    }
    return NOERROR;
}

ECode MediaRouteChooserDialogFragment::OnCreateChooserDialog(
    /* [in] */ IContext* context,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IMediaRouteChooserDialog** dialog)
{
    Int32 theme;
    GetTheme(&theme);
    return CMediaRouteChooserDialog::New(context, theme, dialog);
}

ECode MediaRouteChooserDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IMediaRouteChooserDialog> mrcDialog;
    OnCreateChooserDialog(IContext::Probe(activity), savedInstanceState,
        (IMediaRouteChooserDialog**)&mrcDialog);
    Int32 types;
    GetRouteTypes(&types);
    mrcDialog->SetRouteTypes(types);
    mrcDialog->SetExtendedSettingsClickListener(mExtendedSettingsClickListener);
    *dialog = IDialog::Probe(mrcDialog);
    REFCOUNT_ADD(*dialog)
    return NOERROR;
}

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos
