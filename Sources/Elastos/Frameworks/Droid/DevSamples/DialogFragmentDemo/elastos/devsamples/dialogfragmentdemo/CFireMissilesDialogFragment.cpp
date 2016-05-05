
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