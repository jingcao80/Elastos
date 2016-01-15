
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
