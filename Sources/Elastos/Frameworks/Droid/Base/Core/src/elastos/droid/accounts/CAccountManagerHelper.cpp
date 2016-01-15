
#include "elastos/droid/accounts/CAccountManagerHelper.h"
#include "elastos/droid/accounts/CAccountManager.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

ECode CAccountManagerHelper::Get(
    /* [in] */ IContext* context,
    /* [out] */ IAccountManager** accountManager)
{
    VALIDATE_NOT_NULL(accountManager);
    return CAccountManager::Get(context, accountManager);
}

ECode CAccountManagerHelper::NewChooseAccountIntent(
    /* [in] */ IAccount* selectedAccount,
    /* [in] */ const ArrayOf<IAccount*>& allowableAccounts,
    /* [in] */ const ArrayOf<String>& allowableAccountTypes,
    /* [in] */ Boolean alwaysPromptForAccount,
    /* [in] */ const String& descriptionOverrideText,
    /* [in] */ const String& addAccountAuthTokenType,
    /* [in] */ const ArrayOf<String>& addAccountRequiredFeatures,
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
