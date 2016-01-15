#ifndef __ELASTOS_DROID_ACCOUNTS_CHOOSETYPEANDACCOUNTACTIVITY_H__
#define __ELASTOS_DROID_ACCOUNTS_CHOOSETYPEANDACCOUNTACTIVITY_H__

#include "Elastos.Droid.Core_server.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Etl::List;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Content::IIntent;


namespace Elastos {
namespace Droid {
namespace Accounts {

class ChooseTypeAndAccountActivity
    : public Activity
    , public IAccountManagerCallback
{
private:
    class AdapterViewOnItemClickListener
        : public ElRefBase
        , public IAdapterViewOnItemClickListener
    {
    public:
        AdapterViewOnItemClickListener(
            /* [in] */ ChooseTypeAndAccountActivity* host);

        CAR_INTERFACE_DECL();

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);
    private:
        AutoPtr<ChooseTypeAndAccountActivity> mHost;
    };

public:
    ChooseTypeAndAccountActivity();

    ~ChooseTypeAndAccountActivity();

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnResume();

    CARAPI OnDestroy();

    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    CARAPI_(void) OnCancelButtonClicked(
        /* [in] */ IView* view);

    CARAPI_(void) OnOkButtonClicked(
        /* [in] */ IView* view);

    // Called when the choose account type activity (for adding an account) returns.
    // If it was a success read the account and set it in the result. In all cases
    // return the result and finish this activity.
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI Run(
        /* [in] */ IAccountManagerFuture* accountManagerFuture);

protected:
    CARAPI_(void) RunAddAccountForAuthenticator(
        /* [in] */ const String& type);

private:
    CARAPI_(void) OnAccountSelected(
        /* [in] */ IAccount* account);

    CARAPI_(void) SetResultAndFinish(
        /* [in] */ const String& accountName,
        /* [in] */ const String& accountType);

    CARAPI_(void) StartChooseAccountTypeActivity();

    /**
     * @return a value between 0 (inclusive) and accounts.size() (inclusive) or SELECTED_ITEM_NONE.
     *      An index value of accounts.size() indicates 'Add account' option.
     */
    CARAPI_(Int32) GetItemIndexToSelect(
        /* [in] */ List<AutoPtr<IAccount> >* accounts,
        /* [in] */ const String& selectedAccountName,
        /* [in] */ Boolean selectedAddNewAccount);

    CARAPI_(AutoPtr< ArrayOf<String> >) GetListOfDisplayableOptions(
        /* [in] */ List<AutoPtr<IAccount> >* accounts);

    /**
     * Create a list of Account objects for each account that is acceptable. Filter out
     * accounts that don't match the allowable types, if provided, or that don't match the
     * allowable accounts, if provided.
     */
    CARAPI_(AutoPtr< List<AutoPtr<IAccount> > >) GetAcceptableAccountChoices(
        /* [in] */ IAccountManager* accountManager);

    /**
     * Return a set of account types speficied by the intent as well as supported by the
     * AccountManager.
     */
    CARAPI_(AutoPtr< HashSet<String> >) GetReleventAccountTypes(
        /* [in] */ IIntent* intent);

    /**
     * Returns a set of whitelisted accounts given by the intent or null if none specified by the
     * intent.
     */
    CARAPI_(AutoPtr< HashSet<AutoPtr<IAccount> > >) GetAllowableAccountSet(
        /* [in] */ IIntent* intent);

    /**
     * Overrides the description text view for the picker activity if specified by the intent.
     * If not specified then makes the description invisible.
     */
    CARAPI_(void) OverrideDescriptionIfSupplied(
        /* [in] */ const String& descriptionOverride);

    /**
     * Populates the UI ListView with the given list of items and selects an item
     * based on {@code mSelectedItemIndex} member variable.
     */
    CARAPI_(void) PopulateUIAccountList(
        /* [in] */ ArrayOf<String>* listItems);

public:
    /**
     * A Parcelable ArrayList of Account objects that limits the choosable accounts to those
     * in this list, if this parameter is supplied.
     */
    static const String EXTRA_ALLOWABLE_ACCOUNTS_ARRAYLIST;

    /**
     * A Parcelable ArrayList of String objects that limits the accounts to choose to those
     * that match the types in this list, if this parameter is supplied. This list is also
     * used to filter the allowable account types if add account is selected.
     */
    static const String EXTRA_ALLOWABLE_ACCOUNT_TYPES_STRING_ARRAY;

    /**
     * This is passed as the addAccountOptions parameter in AccountManager.addAccount()
     * if it is called.
     */
    static const String EXTRA_ADD_ACCOUNT_OPTIONS_BUNDLE;

    /**
     * This is passed as the requiredFeatures parameter in AccountManager.addAccount()
     * if it is called.
     */
    static const String EXTRA_ADD_ACCOUNT_REQUIRED_FEATURES_STRING_ARRAY;

    /**
     * This is passed as the authTokenType string in AccountManager.addAccount()
     * if it is called.
     */
    static const String EXTRA_ADD_ACCOUNT_AUTH_TOKEN_TYPE_STRING;

    /**
     * If set then the specified account is already "selected".
     */
    static const String EXTRA_SELECTED_ACCOUNT;

    /**
     * If true then display the account selection list even if there is just
     * one account to choose from. boolean.
     */
    static const String EXTRA_ALWAYS_PROMPT_FOR_ACCOUNT;

    /**
     * If set then this string willb e used as the description rather than
     * the default.
     */
    static const String EXTRA_DESCRIPTION_TEXT_OVERRIDE;

    static const Int32 REQUEST_NULL;
    static const Int32 REQUEST_CHOOSE_TYPE;
    static const Int32 REQUEST_ADD_ACCOUNT;

private:
    static const String TAG;

    static const String KEY_INSTANCE_STATE_PENDING_REQUEST;
    static const String KEY_INSTANCE_STATE_EXISTING_ACCOUNTS;
    static const String KEY_INSTANCE_STATE_SELECTED_ACCOUNT_NAME;
    static const String KEY_INSTANCE_STATE_SELECTED_ADD_ACCOUNT;

    static const Int32 SELECTED_ITEM_NONE;

    AutoPtr< HashSet<AutoPtr<IAccount> > > mSetOfAllowableAccounts;
    AutoPtr <HashSet<String> > mSetOfRelevantAccountTypes;
    String mSelectedAccountName;
    Boolean mSelectedAddNewAccount;
    Boolean mAlwaysPromptForAccount;
    String mDescriptionOverride;

    AutoPtr< List<AutoPtr<IAccount> > > mAccounts;
    Int32 mPendingRequest;
    AutoPtr< ArrayOf<IParcelable*> > mExistingAccounts;
    Int32 mSelectedItemIndex;
    AutoPtr<IButton> mOkButton;

    friend class AdapterViewOnItemClickListener;
};

} // Accounts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CHOOSETYPEANDACCOUNTACTIVITY_H__
