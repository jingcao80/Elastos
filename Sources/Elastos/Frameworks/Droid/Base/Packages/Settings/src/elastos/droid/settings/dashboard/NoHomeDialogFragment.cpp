
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
