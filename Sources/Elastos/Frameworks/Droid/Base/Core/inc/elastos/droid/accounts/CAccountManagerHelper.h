
#ifndef __ELASTOS_DROID_ACCOUNTS_CACCOUNTMANAGERHELPER_H__
#define __ELASTOS_DROID_ACCOUNTS_CACCOUNTMANAGERHELPER_H__

#include "_Elastos_Droid_Accounts_CAccountManagerHelper.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Accounts {

CarClass(CAccountManagerHelper)
{
public:
    /**
     * Gets an AccountManager instance associated with a Context.
     * The {@link Context} will be used as long as the AccountManager is
     * active, so make sure to use a {@link Context} whose lifetime is
     * commensurate with any listeners registered to
     * {@link #addOnAccountsUpdatedListener} or similar methods.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>No permission is required to call this method.
     *
     * @param context The {@link Context} to use when necessary
     * @return An {@link AccountManager} instance
     */
    CARAPI Get(
        /* [in] */ IContext* context,
        /* [out] */ IAccountManager** accountManager);

    /**
     * Returns an intent to an {@link Activity} that prompts the user to choose from a list of
     * accounts.
     * The caller will then typically start the activity by calling
     * <code>startActivityWithResult(intent, ...);</code>.
     * <p>
     * On success the activity returns a Bundle with the account name and type specified using
     * keys {@link #KEY_ACCOUNT_NAME} and {@link #KEY_ACCOUNT_TYPE}.
     * <p>
     * The most common case is to call this with one account type, e.g.:
     * <p>
     * <pre>  newChooseAccountsIntent(null, null, new String[]{"com.google"}, false, null,
     * null, null, null);</pre>
     * @param selectedAccount if specified, indicates that the {@link Account} is the currently
     * selected one, according to the caller's definition of selected.
     * @param allowableAccounts an optional {@link ArrayList} of accounts that are allowed to be
     * shown. If not specified then this field will not limit the displayed accounts.
     * @param allowableAccountTypes an optional string array of account types. These are used
     * both to filter the shown accounts and to filter the list of account types that are shown
     * when adding an account.
     * @param alwaysPromptForAccount if set the account chooser screen is always shown, otherwise
     * it is only shown when there is more than one account from which to choose
     * @param descriptionOverrideText if non-null this string is used as the description in the
     * accounts chooser screen rather than the default
     * @param addAccountAuthTokenType this string is passed as the {@link #addAccount}
     * authTokenType parameter
     * @param addAccountRequiredFeatures this string array is passed as the {@link #addAccount}
     * requiredFeatures parameter
     * @param addAccountOptions This {@link Bundle} is passed as the {@link #addAccount} options
     * parameter
     * @return an {@link Intent} that can be used to launch the ChooseAccount activity flow.
     */
    CARAPI NewChooseAccountIntent(
        /* [in] */ IAccount* selectedAccount,
        /* [in] */ const ArrayOf<IAccount*>& allowableAccounts,
        /* [in] */ const ArrayOf<String>& allowableAccountTypes,
        /* [in] */ Boolean alwaysPromptForAccount,
        /* [in] */ const String& descriptionOverrideText,
        /* [in] */ const String& addAccountAuthTokenType,
        /* [in] */ const ArrayOf<String>&  addAccountRequiredFeatures,
        /* [in] */ IBundle* addAccountOptions,
        /* [out] */ IIntent** intent);
};

}   //namespace Accounts
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CACCOUNTMANAGERHELPER_H__
