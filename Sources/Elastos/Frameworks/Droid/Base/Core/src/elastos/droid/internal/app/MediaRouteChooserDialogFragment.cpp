
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
