
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