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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/accounts/CAccountManagerHelper.h"
#include "elastos/droid/accounts/CAccountManager.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

CAR_SINGLETON_IMPL(CAccountManagerHelper)

CAR_INTERFACE_IMPL(CAccountManagerHelper, Singleton, IAccountManagerHelper)

ECode CAccountManagerHelper::Get(
    /* [in] */ IContext* context,
    /* [out] */ IAccountManager** accountManager)
{
    VALIDATE_NOT_NULL(accountManager);
    return CAccountManager::Get(context, accountManager);
}

ECode CAccountManagerHelper::NewChooseAccountIntent(
    /* [in] */ IAccount* selectedAccount,
    /* [in] */ ArrayOf<IAccount*>* allowableAccounts,
    /* [in] */ ArrayOf<String>* allowableAccountTypes,
    /* [in] */ Boolean alwaysPromptForAccount,
    /* [in] */ const String& descriptionOverrideText,
    /* [in] */ const String& addAccountAuthTokenType,
    /* [in] */ ArrayOf<String>* addAccountRequiredFeatures,
    /* [in] */ IBundle* addAccountOptions,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    return CAccountManager::NewChooseAccountIntent(selectedAccount,
            allowableAccounts, allowableAccountTypes, alwaysPromptForAccount,
            descriptionOverrideText, addAccountAuthTokenType,
            addAccountRequiredFeatures, addAccountOptions, intent);
}

}   //namespace Accounts
}   //namespace Droid
}   //namespace Elastos
