
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