
#include "elastos/droid/phone/settings/CPhoneAccountSettingsActivity.h"
#include <Elastos.Droid.App.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::App::IFragmentManager;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Settings {

CAR_OBJECT_IMPL(CPhoneAccountSettingsActivity)

ECode CPhoneAccountSettingsActivity::constructor()
{
    return PreferenceActivity::constructor();
}

ECode CPhoneAccountSettingsActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    PreferenceActivity::OnCreate(icicle);

    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    actionBar->SetTitle(Elastos::Droid::Server::Telephony::R::string::phone_account_settings);

    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    AutoPtr<IFragmentTransaction> transaction;
    manager->BeginTransaction((IFragmentTransaction**)&transaction);
    assert(0 && "need class PhoneAccountSettingsFragment");
    AutoPtr<IFragment> fragment;// = new PhoneAccountSettingsFragment();
    transaction->Replace(Elastos::Droid::R::id::content, fragment);
    Int32 tmp;
    return transaction->Commit(&tmp);
}

ECode CPhoneAccountSettingsActivity::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 itemId;
    item->GetItemId(&itemId);
    if (itemId == Elastos::Droid::R::id::home) {
        OnBackPressed();
        *result = TRUE;
        return NOERROR;
    }
    return PreferenceActivity::OnOptionsItemSelected(item, result);
}

} // namespace Settings
} // namespace Phone
} // namespace Droid
} // namespace Elastos