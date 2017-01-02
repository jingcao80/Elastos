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

#include "elastos/droid/teleservice/phone/settings/CPhoneAccountSettingsActivity.h"
#include "elastos/droid/teleservice/phone/settings/PhoneAccountSettingsFragment.h"
#include <Elastos.Droid.App.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::App::IFragmentManager;

namespace Elastos {
namespace Droid {
namespace TeleService {
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
    actionBar->SetTitle(Elastos::Droid::TeleService::R::string::phone_accounts);

    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    AutoPtr<IFragmentTransaction> transaction;
    manager->BeginTransaction((IFragmentTransaction**)&transaction);
    AutoPtr<IFragment> fragment = new PhoneAccountSettingsFragment();
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
} // namespace TeleService
} // namespace Droid
} // namespace Elastos
