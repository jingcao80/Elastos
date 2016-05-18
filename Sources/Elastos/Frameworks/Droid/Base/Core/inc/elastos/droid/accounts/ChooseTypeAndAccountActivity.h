#ifndef __ELASTOS_DROID_ACCOUNTS_CHOOSETYPEANDACCOUNTACTIVITY_H__
#define __ELASTOS_DROID_ACCOUNTS_CHOOSETYPEANDACCOUNTACTIVITY_H__

#include "_Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Utility::ISet;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Accounts {

class ChooseTypeAndAccountActivity
    : public Activity
    , public IChooseTypeAndAccountActivity
    , public IAccountManagerCallback
{
private:
    class AdapterViewOnItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        AdapterViewOnItemClickListener(
            /* [in] */ ChooseTypeAndAccountActivity* host);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);
    private:
        ChooseTypeAndAccountActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ChooseTypeAndAccountActivity();

    ~ChooseTypeAndAccountActivity();

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnResume();

    CARAPI OnDestroy();

    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    CARAPI OnCancelButtonClicked(
        /* [in] */ IView* view);

    CARAPI OnOkButtonClicked(
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
        /* [in] */ IArrayList* accounts,
        /* [in] */ const String& selectedAccountName,
        /* [in] */ Boolean selectedAddNewAccount);

    CARAPI_(AutoPtr< ArrayOf<String> >) GetListOfDisplayableOptions(
        /* [in] */ IArrayList* accounts);

    /**
     * Create a list of Account objects for each account that is acceptable. Filter out
     * accounts that don't match the allowable types, if provided, or that don't match the
     * allowable accounts, if provided.
     */
    CARAPI_(AutoPtr<IArrayList>) GetAcceptableAccountChoices(
        /* [in] */ IAccountManager* accountManager);

    /**
     * Return a set of account types speficied by the intent as well as supported by the
     * AccountManager.
     */
    CARAPI_(AutoPtr<ISet>) GetReleventAccountTypes(
        /* [in] */ IIntent* intent);

    /**
     * Returns a set of whitelisted accounts given by the intent or null if none specified by the
     * intent.
     */
    CARAPI_(AutoPtr<ISet>) GetAllowableAccountSet(
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

private:
    static const String TAG;

    static const String KEY_INSTANCE_STATE_PENDING_REQUEST;
    static const String KEY_INSTANCE_STATE_EXISTING_ACCOUNTS;
    static const String KEY_INSTANCE_STATE_SELECTED_ACCOUNT_NAME;
    static const String KEY_INSTANCE_STATE_SELECTED_ADD_ACCOUNT;
    static const String KEY_INSTANCE_STATE_ACCOUNT_LIST;

    static const Int32 SELECTED_ITEM_NONE;

    AutoPtr<ISet> mSetOfAllowableAccounts;
    AutoPtr<ISet> mSetOfRelevantAccountTypes;
    String mSelectedAccountName;
    Boolean mSelectedAddNewAccount;
    Boolean mAlwaysPromptForAccount;
    String mDescriptionOverride;

    AutoPtr<IArrayList> mAccounts;
    Int32 mPendingRequest;
    AutoPtr<ArrayOf<IParcelable*> > mExistingAccounts;
    Int32 mSelectedItemIndex;
    AutoPtr<IButton> mOkButton;
    Int32 mCallingUid;
    String mCallingPackage;
    Boolean mDisallowAddAccounts;
    Boolean mDontShowPicker;

    friend class AdapterViewOnItemClickListener;
};

} // Accounts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CHOOSETYPEANDACCOUNTACTIVITY_H__
