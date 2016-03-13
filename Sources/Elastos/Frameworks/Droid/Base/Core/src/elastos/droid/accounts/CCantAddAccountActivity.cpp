
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/accounts/CCantAddAccountActivity.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Accounts {

CAR_OBJECT_IMPL(CCantAddAccountActivity)

CAR_INTERFACE_IMPL(CCantAddAccountActivity, Object, ICantAddAccountActivity)

ECode CCantAddAccountActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::app_not_authorized);
    return NOERROR;
}

ECode CCantAddAccountActivity::OnCancelButtonClicked(
    /* [in] */ IView* view)
{
    return OnBackPressed();
}

} // namespace Accounts
} // namespace Droid
} // namespace Elastos
