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

#ifndef __ELASTOS_DROID_ACCOUNTS_CACCOUNTMANAGER_H__
#define __ELASTOS_DROID_ACCOUNTS_CACCOUNTMANAGER_H__

#include "_Elastos_Droid_Accounts_CAccountManager.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/utility/concurrent/FutureTask.h>
#include <elastos/core/Object.h>
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::Core::IInteger64;
using Elastos::Utility::IHashMap;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::FutureTask;

namespace Elastos {
namespace Droid {
namespace Accounts {

class CAccountManagerAmsResponse;
class CAccountManagerFutureResponse;
class CChooseResponse;

CarClass(CAccountManager)
    , public Object
    , public IAccountManager
{
private:
    class AmsTask
        : public FutureTask
        , public IAccountManagerFuture
    {
    public:
        CAR_INTERFACE_DECL()

        AmsTask(
            /* [in] */ IActivity* activity,
            /* [in] */ IHandler* handler,
            /* [in] */ IAccountManagerCallback* cb,
            /* [in] */ CAccountManager* host);

        CARAPI Start(
            /* [out] */ IAccountManagerFuture** accountManagerFuture);

        virtual CARAPI DoWork() = 0;

        CARAPI GetResult(
            /* [out] */ IInterface** result);

        CARAPI GetResult(
            /* [in] */ Int64 timeout,
            /* [in] */ ITimeUnit* unit,
            /* [out] */ IInterface** result);

        CARAPI Cancel(
            /* [in] */ Boolean mayInterruptIfRunning,
            /* [out] */ Boolean* result);

        CARAPI IsCancelled(
            /* [out] */ Boolean* result);

        CARAPI IsDone(
            /* [out] */ Boolean* result);

    protected:
        CARAPI_(void) Set(
            /* [in] */ IInterface* bundle);

        CARAPI_(void) Done();

    private:
        CARAPI InternalGetResult(
            /* [in] */ IInteger64* timeout,
            /* [in] */ ITimeUnit* unit,
            /* [out] */ IBundle** result);

    public:
        AutoPtr<IAccountManagerResponse> mResponse;
        AutoPtr<IHandler> mHandler;
        AutoPtr<IAccountManagerCallback> mCallback;
        AutoPtr<IActivity> mActivity;

    protected:
        CAccountManager* mHost;
        friend class CAccountManagerAmsResponse;
        friend class CChooseResponse;
    };

    class AmsTask_GetAuthToken
        : public AmsTask
    {
    public:
        AmsTask_GetAuthToken(
            /* [in] */ IActivity* activity,
            /* [in] */ IHandler* handler,
            /* [in] */ IAccountManagerCallback* cb,
            /* [in] */ CAccountManager* host,
            /* [in] */ IAccount* account,
            /* [in] */ const String& authTokenType,
            /* [in] */ Boolean notifyAuthFailure,
            /* [in] */ Boolean expectActivityLaunch,
            /* [in] */ IBundle* options);

        CARAPI DoWork();

    private:
        AutoPtr<IAccount> mAccount;
        String mAuthTokenType;
        Boolean mNotifyAuthFailure;
        Boolean mExpectActivityLaunch;
        AutoPtr<IBundle> mOptions;
    };

    class AmsTask_AddAccount
        : public AmsTask
    {
    public:
        AmsTask_AddAccount(
            /* [in] */ IActivity* activity,
            /* [in] */ IHandler* handler,
            /* [in] */ IAccountManagerCallback* cb,
            /* [in] */ CAccountManager* host,
            /* [in] */ const String& accountType,
            /* [in] */ const String& authTokenType,
            /* [in] */ ArrayOf<String>* requiredFeatures,
            /* [in] */ IBundle* options);

        CARAPI DoWork();

    private:
        String mAccountType;
        String mAuthTokenType;
        AutoPtr< ArrayOf<String> > mRequiredFeatures;
        AutoPtr<IBundle> mOptions;
    };

    class AmsTask_AddAccountAsUser
        : public AmsTask
    {
    public:
        AmsTask_AddAccountAsUser(
            /* [in] */ IActivity* activity,
            /* [in] */ IHandler* handler,
            /* [in] */ IAccountManagerCallback* cb,
            /* [in] */ CAccountManager* host,
            /* [in] */ const String& accountType,
            /* [in] */ const String& authTokenType,
            /* [in] */ ArrayOf<String>* requiredFeatures,
            /* [in] */ IBundle* optionsIn,
            /* [in] */ Int32 id);

        CARAPI DoWork();

    private:
        String mAccountType;
        String mAuthTokenType;
        AutoPtr<ArrayOf<String> > mRequiredFeatures;
        AutoPtr<IBundle> mOptionsIn;
        Int32 mId;
    };

    class AmsTask_ConfirmCredentialsAsUser
        : public AmsTask
    {
    public:
        AmsTask_ConfirmCredentialsAsUser(
            /* [in] */ IActivity* activity,
            /* [in] */ IHandler* handler,
            /* [in] */ IAccountManagerCallback* accountManagerCb,
            /* [in] */ CAccountManager* host,
            /* [in] */ IAccount* account,
            /* [in] */ IBundle* options,
            /* [in] */ Int32 userId);

        CARAPI DoWork();

    private:
        AutoPtr<IAccount> mAccount;
        AutoPtr<IBundle> mOptions;
        Int32 mUserId;
    };

    class AmsTask_UpdateCredentials
        : public AmsTask
    {
    public:
        AmsTask_UpdateCredentials(
            /* [in] */ IActivity* activity,
            /* [in] */ IHandler* handler,
            /* [in] */ IAccountManagerCallback* cb,
            /* [in] */ CAccountManager* host,
            /* [in] */ IAccount* account,
            /* [in] */ const String& authTokenType,
            /* [in] */ IBundle* options);

        CARAPI DoWork();

    private:
        AutoPtr<IAccount> mAccount;
        String mAuthTokenType;
        AutoPtr<IBundle> mOptions;
    };

    class AmsTask_EditProperties
        : public AmsTask
    {
    public:
        AmsTask_EditProperties(
            /* [in] */ IActivity* activity,
            /* [in] */ IHandler* handler,
            /* [in] */ IAccountManagerCallback* cb,
            /* [in] */ CAccountManager* host,
            /* [in] */ const String& authTokenType);

        CARAPI DoWork();

    private:
        String mAuthTokenType;
    };

    class BaseFutureTask
        : public FutureTask
    {
    public:
        BaseFutureTask(
            /* [in] */ IHandler* handler,
            /* [in] */ CAccountManager* host);

        virtual CARAPI DoWork() = 0;

        virtual CARAPI BundleToResult(
            /* [in] */ IBundle* bundle,
            /* [out] */ IInterface** result) = 0;

    protected:
        CARAPI_(void) PostRunnableToHandler(
            /* [in] */ IRunnable* runnable);

        CARAPI StartTask();

    public:
        AutoPtr<IAccountManagerResponse> mResponse;
        AutoPtr<IHandler> mHandler;
        CAccountManager* mHost;
    };

    class Future2Task
        : public BaseFutureTask
        , public IAccountManagerFuture
    {
    private:
        class CallbackRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL()

            CallbackRunnable(
                /* [in] */ IAccountManagerCallback* cb,
                /* [in] */ Future2Task* host);

            CARAPI Run();

        private:
            AutoPtr<IAccountManagerCallback> mCallback;
            Future2Task* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        Future2Task(
            /* [in] */ IHandler* handler,
            /* [in] */ IAccountManagerCallback* cb,
            /* [in] */ CAccountManager* host);

        virtual ~Future2Task() {}

        CARAPI Start(
            /* [out] */ IAccountManagerFuture** accountManagerFuture);

        CARAPI GetResult(
            /* [out] */ IInterface** result);

        CARAPI GetResult(
            /* [in] */ Int64 timeout,
            /* [in] */ ITimeUnit* unit,
            /* [out] */ IInterface** result);

        CARAPI Cancel(
            /* [in] */ Boolean mayInterruptIfRunning,
            /* [out] */ Boolean* result);

        CARAPI IsCancelled(
            /* [out] */ Boolean* result);

        CARAPI IsDone(
            /* [out] */ Boolean* result);

    protected:
        CARAPI_(void) Done();

    private:
        CARAPI InternalGetResult(
            /* [in] */ IInteger64* timeout,
            /* [in] */ ITimeUnit* unit,
            /* [out] */ IInterface** result);

    private:
        AutoPtr<IAccountManagerCallback> mCallback;
    };

    class Future2Task_GetAuthTokenLabel
        : public Future2Task
    {
    public:
        Future2Task_GetAuthTokenLabel(
            /* [in] */ IHandler* handler,
            /* [in] */ IAccountManagerCallback* cb,
            /* [in] */ CAccountManager* host,
            /* [in] */ const String& accountType,
            /* [in] */ const String& authTokenType);

        CARAPI DoWork();

        CARAPI BundleToResult(
            /* [in] */ IBundle* bundle,
            /* [out] */ IInterface** result);

    private:
        String mAccountType;
        String mAuthTokenType;
    };

    class Future2Task_HasFeatures
        : public Future2Task
    {
    public:
        Future2Task_HasFeatures(
            /* [in] */ IHandler* handler,
            /* [in] */ IAccountManagerCallback* cb,
            /* [in] */ CAccountManager* host,
            /* [in] */ IAccount* account,
            /* [in] */ ArrayOf<String>* features);

        CARAPI DoWork();

        CARAPI BundleToResult(
            /* [in] */ IBundle* bundle,
            /* [out] */ IInterface** result);

    private:
        AutoPtr<IAccount> mAccount;
        AutoPtr< ArrayOf<String> > mFeatures;
    };

    class Future2Task_GetAccountsByTypeAndFeatures
        : public Future2Task
    {
    public:
        Future2Task_GetAccountsByTypeAndFeatures(
            /* [in] */ IHandler* handler,
            /* [in] */ IAccountManagerCallback* cb,
            /* [in] */ CAccountManager* host,
            /* [in] */ const String& type,
            /* [in] */ ArrayOf<String>* features);

        CARAPI DoWork();

        CARAPI BundleToResult(
            /* [in] */ IBundle* bundle,
            /* [out] */ IInterface** result);

    private:
        String mType;
        AutoPtr< ArrayOf<String> > mFeatures;
    };

    class Future2Task_RenameAccount
        : public Future2Task
    {
    public:
        Future2Task_RenameAccount(
            /* [in] */ IHandler* handler,
            /* [in] */ IAccountManagerCallback* cb,
            /* [in] */ CAccountManager* host,
            /* [in] */ IAccount* account,
            /* [in] */ const String& newName);

        CARAPI DoWork();

        CARAPI BundleToResult(
            /* [in] */ IBundle* bundle,
            /* [out] */ IInterface** result);

    private:
        AutoPtr<IAccount> mAccount;
        String mNewName;
    };

    class Future2Task_RemoveAccount
        : public Future2Task
    {
    public:
        Future2Task_RemoveAccount(
            /* [in] */ IHandler* handler,
            /* [in] */ IAccountManagerCallback* cb,
            /* [in] */ CAccountManager* host,
            /* [in] */ IAccount* account);

        CARAPI DoWork();

        CARAPI BundleToResult(
            /* [in] */ IBundle* bundle,
            /* [out] */ IInterface** result);

    private:
        AutoPtr<IAccount> mAccount;
    };

    class Future2Task_RemoveAccountAsUser
        : public Future2Task
    {
    public:
        Future2Task_RemoveAccountAsUser(
            /* [in] */ IHandler* handler,
            /* [in] */ IAccountManagerCallback* cb,
            /* [in] */ CAccountManager* host,
            /* [in] */ IAccount* account,
            /* [in] */ IUserHandle* userHandle);

        CARAPI DoWork();

        CARAPI BundleToResult(
            /* [in] */ IBundle* bundle,
            /* [out] */ IInterface** result);

    public:
        AutoPtr<IAccount> mAccount;
        AutoPtr<IUserHandle> mUserHandle;
    };

    class GetAuthTokenByTypeAndFeaturesTask
        : public AmsTask
        , public IAccountManagerCallback
    {
    private:
        class GetAccountsCallback
            : public Object
            , public IAccountManagerCallback
        {
        public:
            CAR_INTERFACE_DECL()

            GetAccountsCallback(
                /* [in] */ GetAuthTokenByTypeAndFeaturesTask* host);

            CARAPI Run(
                /* [in] */ IAccountManagerFuture* future);

        private:
            GetAuthTokenByTypeAndFeaturesTask* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        GetAuthTokenByTypeAndFeaturesTask(
            /* [in] */ const String& accountType,
            /* [in] */ const String& authTokenType,
            /* [in] */ ArrayOf<String>* features,
            /* [in] */ IActivity* activityForPrompting,
            /* [in] */ IBundle* addAccountOptions,
            /* [in] */ IBundle* loginOptions,
            /* [in] */ IAccountManagerCallback* cb,
            /* [in] */ IHandler* handler,
            /* [in] */ CAccountManager* host);

        CARAPI DoWork();

        using AmsTask::Run;

        CARAPI Run(
            /* [in] */ IAccountManagerFuture* future);

    public:
        AutoPtr<IAccountManagerFuture> mFuture;
        String mAccountType;
        String mAuthTokenType;
        AutoPtr< ArrayOf<String> > mFeatures;
        AutoPtr<IBundle> mAddAccountOptions;
        AutoPtr<IBundle> mLoginOptions;
        AutoPtr<IAccountManagerCallback> mMyCallback;

    private:
        Int32 mNumAccounts;//todo: if the value is equal to 2, this mean the numberf of Accounts is above 2;
    };

    class AccountsChangedBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        AccountsChangedBroadcastReceiver(
            /* [in] */ CAccountManager* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("AccountsChangedBroadcastReceiver: ")

    private:
        CAccountManager* mHost;
    };

    class CallbackAction
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        CallbackAction(
            /* [in] */ IAccountManagerCallback* accountManagerCb,
            /* [in] */ IAccountManagerFuture* future);

        CARAPI Run();

    private:
        AutoPtr<IAccountManagerCallback> mCallback;
        AutoPtr<IAccountManagerFuture> mFuture;
    };

    class AccountUpdateAction
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        AccountUpdateAction(
            /* [in] */ IOnAccountsUpdateListener* listener,
            /* [in] */ ArrayOf<IAccount*>* accounts);

        CARAPI Run();

    private:
        AutoPtr<IOnAccountsUpdateListener> mListener;
        AutoPtr< ArrayOf<IAccount*> > mAccounts;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CAccountManager();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIAccountManager* service);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIAccountManager* service,
        /* [in] */ IHandler* handler);

    /**
     * @hide for internal use only
     */
    static CARAPI_(AutoPtr<IBundle>) SanitizeResult(
        /* [in] */ IBundle* result);

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
    static CARAPI Get(
        /* [in] */ IContext* context,
        /* [out] */ IAccountManager** accountManager);

    /**
     * Gets the saved password associated with the account.
     * This is intended for authenticators and related code; applications
     * should get an auth token instead.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#AUTHENTICATE_ACCOUNTS}
     * and to have the same UID as the account's authenticator.
     *
     * @param account The account to query for a password
     * @return The account's password, null if none or if the account doesn't exist
     */
    CARAPI GetPassword(
        /* [in] */ IAccount* account,
        /* [out] */ String* password);

    /**
     * Gets the user data named by "key" associated with the account.
     * This is intended for authenticators and related code to store
     * arbitrary metadata along with accounts.  The meaning of the keys
     * and values is up to the authenticator for the account.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#AUTHENTICATE_ACCOUNTS}
     * and to have the same UID as the account's authenticator.
     *
     * @param account The account to query for user data
     * @return The user data, null if the account or key doesn't exist
     */
    CARAPI GetUserData(
        /* [in] */ IAccount* account,
        /* [in] */ const String& key,
        /* [out] */ String* userData);

    /**
     * Lists the currently registered authenticators.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>No permission is required to call this method.
     *
     * @return An array of {@link AuthenticatorDescription} for every
     *     authenticator known to the AccountManager service.  Empty (never
     *     null) if no authenticators are known.
     */
    CARAPI GetAuthenticatorTypes(
        /* [out, callee] */ ArrayOf<IAuthenticatorDescription*>** authenticators);

    /**
     * @hide
     * Lists the currently registered authenticators for a given user id.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>The caller has to be in the same user or have the permission
     * {@link android.Manifest.permission#INTERACT_ACROSS_USERS_FULL}.
     *
     * @return An array of {@link AuthenticatorDescription} for every
     *     authenticator known to the AccountManager service.  Empty (never
     *     null) if no authenticators are known.
     */
    CARAPI GetAuthenticatorTypesAsUser(
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<IAuthenticatorDescription*>** authenticators);

    /**
     * Lists all accounts of any type registered on the device.
     * Equivalent to getAccountsByType(null).
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#GET_ACCOUNTS}.
     *
     * @return An array of {@link Account}, one for each account.  Empty
     *     (never null) if no accounts have been added.
     */
    CARAPI GetAccounts(
        /* [out, callee] */ ArrayOf<IAccount*>** accounts);

    /**
     * @hide
     * Lists all accounts of any type registered on the device for a given
     * user id. Equivalent to getAccountsByType(null).
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#GET_ACCOUNTS}.
     *
     * @return An array of {@link Account}, one for each account.  Empty
     *     (never null) if no accounts have been added.
     */
    CARAPI GetAccountsAsUser(
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<IAccount*>** accounts);

    /**
     * @hide
     * For use by internal activities. Returns the list of accounts that the calling package
     * is authorized to use, particularly for shared accounts.
     * @param packageName package name of the calling app.
     * @param uid the uid of the calling app.
     * @return the accounts that are available to this package and user.
     */
    CARAPI GetAccountsForPackage(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [out, callee] */ ArrayOf<IAccount*>** accounts);

    /**
     * Returns the accounts visible to the specified package, in an environment where some apps
     * are not authorized to view all accounts. This method can only be called by system apps.
     * @param type The type of accounts to return, null to retrieve all accounts
     * @param packageName The package name of the app for which the accounts are to be returned
     * @return An array of {@link Account}, one per matching account.  Empty
     *     (never null) if no accounts of the specified type have been added.
     */
    CARAPI GetAccountsByTypeForPackage(
        /* [in] */ const String& type,
        /* [in] */ const String& packageName,
        /* [out, callee] */ ArrayOf<IAccount*>** accounts);

    /**
     * Lists all accounts of a particular type.  The account type is a
     * string token corresponding to the authenticator and useful domain
     * of the account.  For example, there are types corresponding to Google
     * and Facebook.  The exact string token to use will be published somewhere
     * associated with the authenticator in question.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#GET_ACCOUNTS}.
     *
     * @param type The type of accounts to return, null to retrieve all accounts
     * @return An array of {@link Account}, one per matching account.  Empty
     *     (never null) if no accounts of the specified type have been added.
     */
    CARAPI GetAccountsByType(
        /* [in] */ const String& type,
        /* [out, callee] */ ArrayOf<IAccount*>** accounts);

    /** @hide Same as {@link #getAccountsByType(String)} but for a specific user. */
    CARAPI GetAccountsByTypeAsUser(
        /* [in] */ const String& type,
        /* [in] */ IUserHandle* userHandle,
        /* [out, callee] */ ArrayOf<IAccount*>** accounts);

    /**
     * Change whether or not an app (identified by its uid) is allowed to retrieve an authToken
     * for an account.
     * <p>
     * This is only meant to be used by system activities and is not in the SDK.
     * @param account The account whose permissions are being modified
     * @param authTokenType The type of token whose permissions are being modified
     * @param uid The uid that identifies the app which is being granted or revoked permission.
     * @param value true is permission is being granted, false for revoked
     * @hide
     */
    CARAPI UpdateAppPermission(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean value);

    /**
     * Get the user-friendly label associated with an authenticator's auth token.
     * @param accountType the type of the authenticator. must not be null.
     * @param authTokenType the token type. must not be null.
     * @param callback callback to invoke when the result is available. may be null.
     * @param handler the handler on which to invoke the callback, or null for the main thread
     * @return a future containing the label string
     * @hide
     */
    CARAPI GetAuthTokenLabel(
        /* [in] */ const String& accountType,
        /* [in] */ const String& authTokenType,
        /* [in] */ IAccountManagerCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ IAccountManagerFuture** future);

    /**
     * Finds out whether a particular account has all the specified features.
     * Account features are authenticator-specific string tokens identifying
     * boolean account properties.  For example, features are used to tell
     * whether Google accounts have a particular service (such as Google
     * Calendar or Google Talk) enabled.  The feature names and their meanings
     * are published somewhere associated with the authenticator in question.
     *
     * <p>This method may be called from any thread, but the returned
     * {@link AccountManagerFuture} must not be used on the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#GET_ACCOUNTS}.
     *
     * @param account The {@link Account} to test
     * @param features An array of the account features to check
     * @param callback Callback to invoke when the request completes,
     *     null for no callback
     * @param handler {@link Handler} identifying the callback thread,
     *     null for the main thread
     * @return An {@link AccountManagerFuture} which resolves to a Boolean,
     * true if the account exists and has all of the specified features.
     */
    CARAPI HasFeatures(
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<String>* features,
        /* [in] */ IAccountManagerCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ IAccountManagerFuture** accountManagerFuture);

    /**
     * Lists all accounts of a type which have certain features.  The account
     * type identifies the authenticator (see {@link #getAccountsByType}).
     * Account features are authenticator-specific string tokens identifying
     * boolean account properties (see {@link #hasFeatures}).
     *
     * <p>Unlike {@link #getAccountsByType}, this method calls the authenticator,
     * which may contact the server or do other work to check account features,
     * so the method returns an {@link AccountManagerFuture}.
     *
     * <p>This method may be called from any thread, but the returned
     * {@link AccountManagerFuture} must not be used on the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#GET_ACCOUNTS}.
     *
     * @param type The type of accounts to return, must not be null
     * @param features An array of the account features to require,
     *     may be null or empty
     * @param callback Callback to invoke when the request completes,
     *     null for no callback
     * @param handler {@link Handler} identifying the callback thread,
     *     null for the main thread
     * @return An {@link AccountManagerFuture} which resolves to an array of
     *     {@link Account}, one per account of the specified type which
     *     matches the requested features.
     */
    CARAPI GetAccountsByTypeAndFeatures(
        /* [in] */ const String& type,
        /* [in] */ ArrayOf<String>* features,
        /* [in] */ IAccountManagerCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ IAccountManagerFuture** accountManagerFuture);

    /**
     * Adds an account directly to the AccountManager.  Normally used by sign-up
     * wizards associated with authenticators, not directly by applications.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#AUTHENTICATE_ACCOUNTS}
     * and to have the same UID as the added account's authenticator.
     *
     * @param account The {@link Account} to add
     * @param password The password to associate with the account, null for none
     * @param userdata String values to use for the account's userdata, null for none
     * @return True if the account was successfully added, false if the account
     *     already exists, the account is null, or another error occurs.
     */
    CARAPI AddAccountExplicitly(
        /* [in] */ IAccount* account,
        /* [in] */ const String& password,
        /* [in] */ IBundle* userdata,
        /* [out] */ Boolean* result);

    /**
     * Rename the specified {@link Account}.  This is equivalent to removing
     * the existing account and adding a new renamed account with the old
     * account's user data.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#AUTHENTICATE_ACCOUNTS}
     * and have the same UID as the account's authenticator.
     *
     * @param account The {@link Account} to rename
     * @param newName String name to be associated with the account.
     * @param callback Callback to invoke when the request completes, null for
     *     no callback
     * @param handler {@link Handler} identifying the callback thread, null for
     *     the main thread
     * @return An {@link AccountManagerFuture} which resolves to the Account
     *     after the name change. If successful the account's name will be the
     *     specified new name.
     */
    CARAPI RenameAccount(
        /* [in] */ IAccount* account,
        /* [in] */ const String& newName,
        /* [in] */ IAccountManagerCallback* callback,
        /* [in] */ IHandler* handler,
        /* [out] */ IAccountManagerFuture** result);

    /**
     * Gets the previous name associated with the account or {@code null}, if
     * none. This is intended so that clients of {@link
     * #LOGIN_ACCOUNTS_CHANGED_ACTION} broadcasts can determine if an
     * authenticator has renamed an account.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * @param account The account to query for a previous name.
     * @return The account's previous name, null if the account has never been
     *         renamed.
     */
    CARAPI GetPreviousName(
        /* [in] */ IAccount* account,
        /* [out] */ String* result);

    /**
     * Removes an account from the AccountManager.  Does nothing if the account
     * does not exist.  Does not delete the account from the server.
     * The authenticator may have its own policies preventing account
     * deletion, in which case the account will not be deleted.
     *
     * <p>This method may be called from any thread, but the returned
     * {@link AccountManagerFuture} must not be used on the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#MANAGE_ACCOUNTS}.
     *
     * @param account The {@link Account} to remove
     * @param callback Callback to invoke when the request completes,
     *     null for no callback
     * @param handler {@link Handler} identifying the callback thread,
     *     null for the main thread
     * @return An {@link AccountManagerFuture} which resolves to a Boolean,
     *     true if the account has been successfully removed,
     *     false if the authenticator forbids deleting this account.
     */
    CARAPI RemoveAccount(
        /* [in] */ IAccount* account,
        /* [in] */ IAccountManagerCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ IAccountManagerFuture** accountManagerFuture);

    /**
     * @see #removeAccount(Account, AccountManagerCallback, Handler)
     * @hide
     */
    CARAPI RemoveAccountAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ IAccountManagerCallback* callback,
        /* [in] */ IHandler* handler,
        /* [in] */ IUserHandle* userHandle,
        /* [out] */ IAccountManagerFuture** accountManagerFuture);

    /**
     * Removes an auth token from the AccountManager's cache.  Does nothing if
     * the auth token is not currently in the cache.  Applications must call this
     * method when the auth token is found to have expired or otherwise become
     * invalid for authenticating requests.  The AccountManager does not validate
     * or expire cached auth tokens otherwise.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#MANAGE_ACCOUNTS} or
     * {@link android.Manifest.permission#USE_CREDENTIALS}
     *
     * @param accountType The account type of the auth token to invalidate, must not be null
     * @param authToken The auth token to invalidate, may be null
     */
    CARAPI InvalidateAuthToken(
        /* [in] */ const String& accountType,
        /* [in] */ const String& authToken);

    /**
     * Gets an auth token from the AccountManager's cache.  If no auth
     * token is cached for this account, null will be returned -- a new
     * auth token will not be generated, and the server will not be contacted.
     * Intended for use by the authenticator, not directly by applications.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#AUTHENTICATE_ACCOUNTS}
     * and to have the same UID as the account's authenticator.
     *
     * @param account The account to fetch an auth token for
     * @param authTokenType The type of auth token to fetch, see {#getAuthToken}
     * @return The cached auth token for this account and type, or null if
     *     no auth token is cached or the account does not exist.
     */
    CARAPI PeekAuthToken(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [out] */ String* token);

    /**
     * Sets or forgets a saved password.  This modifies the local copy of the
     * password used to automatically authenticate the user; it does
     * not change the user's account password on the server.  Intended for use
     * by the authenticator, not directly by applications.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#AUTHENTICATE_ACCOUNTS}
     * and have the same UID as the account's authenticator.
     *
     * @param account The account to set a password for
     * @param password The password to set, null to clear the password
     */
    CARAPI SetPassword(
        /* [in] */ IAccount* account,
        /* [in] */ const String& password);

    /**
     * Forgets a saved password.  This erases the local copy of the password;
     * it does not change the user's account password on the server.
     * Has the same effect as setPassword(account, null) but requires fewer
     * permissions, and may be used by applications or management interfaces
     * to "sign out" from an account.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#MANAGE_ACCOUNTS}
     *
     * @param account The account whose password to clear
     */
    CARAPI ClearPassword(
        /* [in] */ IAccount* account);

    /**
     * Sets one userdata key for an account.  Intended by use for the
     * authenticator to stash state for itself, not directly by applications.
     * The meaning of the keys and values is up to the authenticator.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#AUTHENTICATE_ACCOUNTS}
     * and to have the same UID as the account's authenticator.
     *
     * @param account The account to set the userdata for
     * @param key The userdata key to set.  Must not be null
     * @param value The value to set, null to clear this userdata key
     */
    CARAPI SetUserData(
        /* [in] */ IAccount* account,
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    /**
     * Adds an auth token to the AccountManager cache for an account.
     * If the account does not exist then this call has no effect.
     * Replaces any previous auth token for this account and auth token type.
     * Intended for use by the authenticator, not directly by applications.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#AUTHENTICATE_ACCOUNTS}
     * and to have the same UID as the account's authenticator.
     *
     * @param account The account to set an auth token for
     * @param authTokenType The type of the auth token, see {#getAuthToken}
     * @param authToken The auth token to add to the cache
     */
    CARAPI SetAuthToken(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ const String& authToken);

    /**
     * This convenience helper synchronously gets an auth token with
     * {@link #getAuthToken(Account, String, boolean, AccountManagerCallback, Handler)}.
     *
     * <p>This method may block while a network request completes, and must
     * never be made from the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#USE_CREDENTIALS}.
     *
     * @param account The account to fetch an auth token for
     * @param authTokenType The auth token type, see {#link getAuthToken}
     * @param notifyAuthFailure If true, display a notification and return null
     *     if authentication fails; if false, prompt and wait for the user to
     *     re-enter correct credentials before returning
     * @return An auth token of the specified type for this account, or null
     *     if authentication fails or none can be fetched.
     * @throws AuthenticatorException if the authenticator failed to respond
     * @throws OperationCanceledException if the request was canceled for any
     *     reason, including the user canceling a credential request
     * @throws java.io.IOException if the authenticator experienced an I/O problem
     *     creating a new auth token, usually because of network trouble
     */
    CARAPI BlockingGetAuthToken(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ Boolean notifyAuthFailure,
        /* [out] */ String* token);

    /**
     * Gets an auth token of the specified type for a particular account,
     * prompting the user for credentials if necessary.  This method is
     * intended for applications running in the foreground where it makes
     * sense to ask the user directly for a password.
     *
     * <p>If a previously generated auth token is cached for this account and
     * type, then it is returned.  Otherwise, if a saved password is
     * available, it is sent to the server to generate a new auth token.
     * Otherwise, the user is prompted to enter a password.
     *
     * <p>Some authenticators have auth token <em>types</em>, whose value
     * is authenticator-dependent.  Some services use different token types to
     * access different functionality -- for example, Google uses different auth
     * tokens to access Gmail and Google Calendar for the same account.
     *
     * <p>This method may be called from any thread, but the returned
     * {@link AccountManagerFuture} must not be used on the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#USE_CREDENTIALS}.
     *
     * @param account The account to fetch an auth token for
     * @param authTokenType The auth token type, an authenticator-dependent
     *     string token, must not be null
     * @param options Authenticator-specific options for the request,
     *     may be null or empty
     * @param activity The {@link Activity} context to use for launching a new
     *     authenticator-defined sub-Activity to prompt the user for a password
     *     if necessary; used only to call startActivity(); must not be null.
     * @param callback Callback to invoke when the request completes,
     *     null for no callback
     * @param handler {@link Handler} identifying the callback thread,
     *     null for the main thread
     * @return An {@link AccountManagerFuture} which resolves to a Bundle with
     *     at least the following fields:
     * <ul>
     * <li> {@link #KEY_ACCOUNT_NAME} - the name of the account you supplied
     * <li> {@link #KEY_ACCOUNT_TYPE} - the type of the account
     * <li> {@link #KEY_AUTHTOKEN} - the auth token you wanted
     * </ul>
     *
     * (Other authenticator-specific values may be returned.)  If an auth token
     * could not be fetched, {@link AccountManagerFuture#getResult()} throws:
     * <ul>
     * <li> {@link AuthenticatorException} if the authenticator failed to respond
     * <li> {@link OperationCanceledException} if the operation is canceled for
     *      any reason, incluidng the user canceling a credential request
     * <li> {@link IOException} if the authenticator experienced an I/O problem
     *      creating a new auth token, usually because of network trouble
     * </ul>
     * If the account is no longer present on the device, the return value is
     * authenticator-dependent.  The caller should verify the validity of the
     * account before requesting an auth token.
     */
    CARAPI GetAuthToken(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ IBundle* options,
        /* [in] */ IActivity* activity,
        /* [in] */ IAccountManagerCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ IAccountManagerFuture** accountManagerFuture);

    /**
     * Gets an auth token of the specified type for a particular account,
     * optionally raising a notification if the user must enter credentials.
     * This method is intended for background tasks and services where the
     * user should not be immediately interrupted with a password prompt.
     *
     * <p>If a previously generated auth token is cached for this account and
     * type, then it is returned.  Otherwise, if a saved password is
     * available, it is sent to the server to generate a new auth token.
     * Otherwise, an {@link Intent} is returned which, when started, will
     * prompt the user for a password.  If the notifyAuthFailure parameter is
     * set, a status bar notification is also created with the same Intent,
     * alerting the user that they need to enter a password at some point.
     *
     * <p>In that case, you may need to wait until the user responds, which
     * could take hours or days or forever.  When the user does respond and
     * supply a new password, the account manager will broadcast the
     * {@link #LOGIN_ACCOUNTS_CHANGED_ACTION} Intent, which applications can
     * use to try again.
     *
     * <p>If notifyAuthFailure is not set, it is the application's
     * responsibility to launch the returned Intent at some point.
     * Either way, the result from this call will not wait for user action.
     *
     * <p>Some authenticators have auth token <em>types</em>, whose value
     * is authenticator-dependent.  Some services use different token types to
     * access different functionality -- for example, Google uses different auth
     * tokens to access Gmail and Google Calendar for the same account.
     *
     * <p>This method may be called from any thread, but the returned
     * {@link AccountManagerFuture} must not be used on the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#USE_CREDENTIALS}.
     *
     * @param account The account to fetch an auth token for
     * @param authTokenType The auth token type, an authenticator-dependent
     *     string token, must not be null
     * @param notifyAuthFailure True to add a notification to prompt the
     *     user for a password if necessary, false to leave that to the caller
     * @param callback Callback to invoke when the request completes,
     *     null for no callback
     * @param handler {@link Handler} identifying the callback thread,
     *     null for the main thread
     * @return An {@link AccountManagerFuture} which resolves to a Bundle with
     *     at least the following fields on success:
     * <ul>
     * <li> {@link #KEY_ACCOUNT_NAME} - the name of the account you supplied
     * <li> {@link #KEY_ACCOUNT_TYPE} - the type of the account
     * <li> {@link #KEY_AUTHTOKEN} - the auth token you wanted
     * </ul>
     *
     * (Other authenticator-specific values may be returned.)  If the user
     * must enter credentials, the returned Bundle contains only
     * {@link #KEY_INTENT} with the {@link Intent} needed to launch a prompt.
     *
     * If an error occurred, {@link AccountManagerFuture#getResult()} throws:
     * <ul>
     * <li> {@link AuthenticatorException} if the authenticator failed to respond
     * <li> {@link OperationCanceledException} if the operation is canceled for
     *      any reason, incluidng the user canceling a credential request
     * <li> {@link IOException} if the authenticator experienced an I/O problem
     *      creating a new auth token, usually because of network trouble
     * </ul>
     * If the account is no longer present on the device, the return value is
     * authenticator-dependent.  The caller should verify the validity of the
     * account before requesting an auth token.
     */
    CARAPI GetAuthToken(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ Boolean notifyAuthFailure,
        /* [in] */ IAccountManagerCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ IAccountManagerFuture** accountManagerFuture);

    /**
     * Gets an auth token of the specified type for a particular account,
     * optionally raising a notification if the user must enter credentials.
     * This method is intended for background tasks and services where the
     * user should not be immediately interrupted with a password prompt.
     *
     * <p>If a previously generated auth token is cached for this account and
     * type, then it is returned.  Otherwise, if a saved password is
     * available, it is sent to the server to generate a new auth token.
     * Otherwise, an {@link Intent} is returned which, when started, will
     * prompt the user for a password.  If the notifyAuthFailure parameter is
     * set, a status bar notification is also created with the same Intent,
     * alerting the user that they need to enter a password at some point.
     *
     * <p>In that case, you may need to wait until the user responds, which
     * could take hours or days or forever.  When the user does respond and
     * supply a new password, the account manager will broadcast the
     * {@link #LOGIN_ACCOUNTS_CHANGED_ACTION} Intent, which applications can
     * use to try again.
     *
     * <p>If notifyAuthFailure is not set, it is the application's
     * responsibility to launch the returned Intent at some point.
     * Either way, the result from this call will not wait for user action.
     *
     * <p>Some authenticators have auth token <em>types</em>, whose value
     * is authenticator-dependent.  Some services use different token types to
     * access different functionality -- for example, Google uses different auth
     * tokens to access Gmail and Google Calendar for the same account.
     *
     * <p>This method may be called from any thread, but the returned
     * {@link AccountManagerFuture} must not be used on the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#USE_CREDENTIALS}.
     *
     * @param account The account to fetch an auth token for
     * @param authTokenType The auth token type, an authenticator-dependent
     *     string token, must not be null
     * @param options Authenticator-specific options for the request,
     *     may be null or empty
     * @param notifyAuthFailure True to add a notification to prompt the
     *     user for a password if necessary, false to leave that to the caller
     * @param callback Callback to invoke when the request completes,
     *     null for no callback
     * @param handler {@link Handler} identifying the callback thread,
     *     null for the main thread
     * @return An {@link AccountManagerFuture} which resolves to a Bundle with
     *     at least the following fields on success:
     * <ul>
     * <li> {@link #KEY_ACCOUNT_NAME} - the name of the account you supplied
     * <li> {@link #KEY_ACCOUNT_TYPE} - the type of the account
     * <li> {@link #KEY_AUTHTOKEN} - the auth token you wanted
     * </ul>
     *
     * (Other authenticator-specific values may be returned.)  If the user
     * must enter credentials, the returned Bundle contains only
     * {@link #KEY_INTENT} with the {@link Intent} needed to launch a prompt.
     *
     * If an error occurred, {@link AccountManagerFuture#getResult()} throws:
     * <ul>
     * <li> {@link AuthenticatorException} if the authenticator failed to respond
     * <li> {@link OperationCanceledException} if the operation is canceled for
     *      any reason, incluidng the user canceling a credential request
     * <li> {@link IOException} if the authenticator experienced an I/O problem
     *      creating a new auth token, usually because of network trouble
     * </ul>
     * If the account is no longer present on the device, the return value is
     * authenticator-dependent.  The caller should verify the validity of the
     * account before requesting an auth token.
     */
    CARAPI GetAuthToken(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ IBundle* options,
        /* [in] */ Boolean notifyAuthFailure,
        /* [in] */ IAccountManagerCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ IAccountManagerFuture** accountManagerFuture);

    /**
     * Asks the user to add an account of a specified type.  The authenticator
     * for this account type processes this request with the appropriate user
     * interface.  If the user does elect to create a new account, the account
     * name is returned.
     *
     * <p>This method may be called from any thread, but the returned
     * {@link AccountManagerFuture} must not be used on the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#MANAGE_ACCOUNTS}.
     *
     * @param accountType The type of account to add; must not be null
     * @param authTokenType The type of auth token (see {@link #getAuthToken})
     *     this account will need to be able to generate, null for none
     * @param requiredFeatures The features (see {@link #hasFeatures}) this
     *     account must have, null for none
     * @param addAccountOptions Authenticator-specific options for the request,
     *     may be null or empty
     * @param activity The {@link Activity} context to use for launching a new
     *     authenticator-defined sub-Activity to prompt the user to create an
     *     account; used only to call startActivity(); if null, the prompt
     *     will not be launched directly, but the necessary {@link Intent}
     *     will be returned to the caller instead
     * @param callback Callback to invoke when the request completes,
     *     null for no callback
     * @param handler {@link Handler} identifying the callback thread,
     *     null for the main thread
     * @return An {@link AccountManagerFuture} which resolves to a Bundle with
     *     these fields if activity was specified and an account was created:
     * <ul>
     * <li> {@link #KEY_ACCOUNT_NAME} - the name of the account created
     * <li> {@link #KEY_ACCOUNT_TYPE} - the type of the account
     * </ul>
     *
     * If no activity was specified, the returned Bundle contains only
     * {@link #KEY_INTENT} with the {@link Intent} needed to launch the
     * actual account creation process.  If an error occurred,
     * {@link AccountManagerFuture#getResult()} throws:
     * <ul>
     * <li> {@link AuthenticatorException} if no authenticator was registered for
     *      this account type or the authenticator failed to respond
     * <li> {@link OperationCanceledException} if the operation was canceled for
     *      any reason, including the user canceling the creation process
     * <li> {@link IOException} if the authenticator experienced an I/O problem
     *      creating a new account, usually because of network trouble
     * </ul>
     */
    CARAPI AddAccount(
        /* [in] */ const String& accountType,
        /* [in] */ const String& authTokenType,
        /* [in] */ ArrayOf<String>* requiredFeatures,
        /* [in] */ IBundle* addAccountOptions,
        /* [in] */ IActivity* activity,
        /* [in] */ IAccountManagerCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ IAccountManagerFuture** accountManagerFuture);

    /**
     * @see #addAccount(String, String, String[], Bundle, Activity, AccountManagerCallback, Handler)
     * @hide
     */
    CARAPI AddAccountAsUser(
        /* [in] */ const String& accountType,
        /* [in] */ const String& authTokenType,
        /* [in] */ ArrayOf<String>* requiredFeatures,
        /* [in] */ IBundle* addAccountOptions,
        /* [in] */ IActivity* activity,
        /* [in] */ IAccountManagerCallback* callback,
        /* [in] */ IHandler* handler,
        /* [in] */ IUserHandle* userHandle,
        /* [out] */ IAccountManagerFuture** result);

    /**
     * Adds a shared account from the primary user to a secondary user. Adding the shared account
     * doesn't take effect immediately. When the target user starts up, any pending shared accounts
     * are attempted to be copied to the target user from the primary via calls to the
     * authenticator.
     * @param account the account to share
     * @param user the target user
     * @return
     * @hide
     */
    CARAPI AddSharedAccount(
        /* [in] */ IAccount* account,
        /* [in] */ IUserHandle* user,
        /* [out] */ Boolean* result);

    /**
     * @hide
     * Removes the shared account.
     * @param account the account to remove
     * @param user the user to remove the account from
     * @return
     */
    CARAPI RemoveSharedAccount(
        /* [in] */ IAccount* account,
        /* [in] */ IUserHandle* user,
        /* [out] */ Boolean* result);

    /**
     * @hide
     * @param user
     * @return
     */
    CARAPI GetSharedAccounts(
        /* [in] */ IUserHandle* user,
        /* [out] */ ArrayOf<IAccount*>** result);

    /**
     * Confirms that the user knows the password for an account to make extra
     * sure they are the owner of the account.  The user-entered password can
     * be supplied directly, otherwise the authenticator for this account type
     * prompts the user with the appropriate interface.  This method is
     * intended for applications which want extra assurance; for example, the
     * phone lock screen uses this to let the user unlock the phone with an
     * account password if they forget the lock pattern.
     *
     * <p>If the user-entered password matches a saved password for this
     * account, the request is considered valid; otherwise the authenticator
     * verifies the password (usually by contacting the server).
     *
     * <p>This method may be called from any thread, but the returned
     * {@link AccountManagerFuture} must not be used on the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#MANAGE_ACCOUNTS}.
     *
     * @param account The account to confirm password knowledge for
     * @param options Authenticator-specific options for the request;
     *     if the {@link #KEY_PASSWORD} string field is present, the
     *     authenticator may use it directly rather than prompting the user;
     *     may be null or empty
     * @param activity The {@link Activity} context to use for launching a new
     *     authenticator-defined sub-Activity to prompt the user to enter a
     *     password; used only to call startActivity(); if null, the prompt
     *     will not be launched directly, but the necessary {@link Intent}
     *     will be returned to the caller instead
     * @param callback Callback to invoke when the request completes,
     *     null for no callback
     * @param handler {@link Handler} identifying the callback thread,
     *     null for the main thread
     * @return An {@link AccountManagerFuture} which resolves to a Bundle
     *     with these fields if activity or password was supplied and
     *     the account was successfully verified:
     * <ul>
     * <li> {@link #KEY_ACCOUNT_NAME} - the name of the account created
     * <li> {@link #KEY_ACCOUNT_TYPE} - the type of the account
     * <li> {@link #KEY_BOOLEAN_RESULT} - true to indicate success
     * </ul>
     *
     * If no activity or password was specified, the returned Bundle contains
     * only {@link #KEY_INTENT} with the {@link Intent} needed to launch the
     * password prompt.  If an error occurred,
     * {@link AccountManagerFuture#getResult()} throws:
     * <ul>
     * <li> {@link AuthenticatorException} if the authenticator failed to respond
     * <li> {@link OperationCanceledException} if the operation was canceled for
     *      any reason, including the user canceling the password prompt
     * <li> {@link IOException} if the authenticator experienced an I/O problem
     *      verifying the password, usually because of network trouble
     * </ul>
     */
    CARAPI ConfirmCredentials(
        /* [in] */ IAccount* account,
        /* [in] */ IBundle* options,
        /* [in] */ IActivity* activity,
        /* [in] */ IAccountManagerCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ IAccountManagerFuture** accountManagerFuture);

    /**
     * @hide
     * Same as {@link #confirmCredentials(Account, Bundle, Activity, AccountManagerCallback, Handler)}
     * but for the specified user.
     */
    CARAPI ConfirmCredentialsAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ IBundle* options,
        /* [in] */ IActivity* activity,
        /* [in] */ IAccountManagerCallback* cb,
        /* [in] */ IHandler* handler,
        /* [in] */ IUserHandle* userHandle,
        /* [out] */ IAccountManagerFuture** accountManagerFuture);

    /**
     * Asks the user to enter a new password for an account, updating the
     * saved credentials for the account.  Normally this happens automatically
     * when the server rejects credentials during an auth token fetch, but this
     * can be invoked directly to ensure we have the correct credentials stored.
     *
     * <p>This method may be called from any thread, but the returned
     * {@link AccountManagerFuture} must not be used on the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#MANAGE_ACCOUNTS}.
     *
     * @param account The account to update credentials for
     * @param authTokenType The credentials entered must allow an auth token
     *     of this type to be created (but no actual auth token is returned);
     *     may be null
     * @param options Authenticator-specific options for the request;
     *     may be null or empty
     * @param activity The {@link Activity} context to use for launching a new
     *     authenticator-defined sub-Activity to prompt the user to enter a
     *     password; used only to call startActivity(); if null, the prompt
     *     will not be launched directly, but the necessary {@link Intent}
     *     will be returned to the caller instead
     * @param callback Callback to invoke when the request completes,
     *     null for no callback
     * @param handler {@link Handler} identifying the callback thread,
     *     null for the main thread
     * @return An {@link AccountManagerFuture} which resolves to a Bundle
     *     with these fields if an activity was supplied and the account
     *     credentials were successfully updated:
     * <ul>
     * <li> {@link #KEY_ACCOUNT_NAME} - the name of the account created
     * <li> {@link #KEY_ACCOUNT_TYPE} - the type of the account
     * </ul>
     *
     * If no activity was specified, the returned Bundle contains only
     * {@link #KEY_INTENT} with the {@link Intent} needed to launch the
     * password prompt.  If an error occurred,
     * {@link AccountManagerFuture#getResult()} throws:
     * <ul>
     * <li> {@link AuthenticatorException} if the authenticator failed to respond
     * <li> {@link OperationCanceledException} if the operation was canceled for
     *      any reason, including the user canceling the password prompt
     * <li> {@link IOException} if the authenticator experienced an I/O problem
     *      verifying the password, usually because of network trouble
     * </ul>
     */
    CARAPI UpdateCredentials(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ IBundle* options,
        /* [in] */ IActivity* activity,
        /* [in] */ IAccountManagerCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ IAccountManagerFuture** accountManagerFuture);

    /**
     * Offers the user an opportunity to change an authenticator's settings.
     * These properties are for the authenticator in general, not a particular
     * account.  Not all authenticators support this method.
     *
     * <p>This method may be called from any thread, but the returned
     * {@link AccountManagerFuture} must not be used on the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#MANAGE_ACCOUNTS}.
     *
     * @param accountType The account type associated with the authenticator
     *     to adjust
     * @param activity The {@link Activity} context to use for launching a new
     *     authenticator-defined sub-Activity to adjust authenticator settings;
     *     used only to call startActivity(); if null, the settings dialog will
     *     not be launched directly, but the necessary {@link Intent} will be
     *     returned to the caller instead
     * @param callback Callback to invoke when the request completes,
     *     null for no callback
     * @param handler {@link Handler} identifying the callback thread,
     *     null for the main thread
     * @return An {@link AccountManagerFuture} which resolves to a Bundle
     *     which is empty if properties were edited successfully, or
     *     if no activity was specified, contains only {@link #KEY_INTENT}
     *     needed to launch the authenticator's settings dialog.
     *     If an error occurred, {@link AccountManagerFuture#getResult()}
     *     throws:
     * <ul>
     * <li> {@link AuthenticatorException} if no authenticator was registered for
     *      this account type or the authenticator failed to respond
     * <li> {@link OperationCanceledException} if the operation was canceled for
     *      any reason, including the user canceling the settings dialog
     * <li> {@link IOException} if the authenticator experienced an I/O problem
     *      updating settings, usually because of network trouble
     * </ul>
     */
    CARAPI EditProperties(
        /* [in] */ const String& accountType,
        /* [in] */ IActivity* activity,
        /* [in] */ IAccountManagerCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ IAccountManagerFuture** accountManagerFuture);

    /**
     * This convenience helper combines the functionality of
     * {@link #getAccountsByTypeAndFeatures}, {@link #getAuthToken}, and
     * {@link #addAccount}.
     *
     * <p>This method gets a list of the accounts matching the
     * specified type and feature set; if there is exactly one, it is
     * used; if there are more than one, the user is prompted to pick one;
     * if there are none, the user is prompted to add one.  Finally,
     * an auth token is acquired for the chosen account.
     *
     * <p>This method may be called from any thread, but the returned
     * {@link AccountManagerFuture} must not be used on the main thread.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#MANAGE_ACCOUNTS}.
     *
     * @param accountType The account type required
     *     (see {@link #getAccountsByType}), must not be null
     * @param authTokenType The desired auth token type
     *     (see {@link #getAuthToken}), must not be null
     * @param features Required features for the account
     *     (see {@link #getAccountsByTypeAndFeatures}), may be null or empty
     * @param activity The {@link Activity} context to use for launching new
     *     sub-Activities to prompt to add an account, select an account,
     *     and/or enter a password, as necessary; used only to call
     *     startActivity(); should not be null
     * @param addAccountOptions Authenticator-specific options to use for
     *     adding new accounts; may be null or empty
     * @param getAuthTokenOptions Authenticator-specific options to use for
     *     getting auth tokens; may be null or empty
     * @param callback Callback to invoke when the request completes,
     *     null for no callback
     * @param handler {@link Handler} identifying the callback thread,
     *     null for the main thread
     * @return An {@link AccountManagerFuture} which resolves to a Bundle with
     *     at least the following fields:
     * <ul>
     * <li> {@link #KEY_ACCOUNT_NAME} - the name of the account
     * <li> {@link #KEY_ACCOUNT_TYPE} - the type of the account
     * <li> {@link #KEY_AUTHTOKEN} - the auth token you wanted
     * </ul>
     *
     * If an error occurred, {@link AccountManagerFuture#getResult()} throws:
     * <ul>
     * <li> {@link AuthenticatorException} if no authenticator was registered for
     *      this account type or the authenticator failed to respond
     * <li> {@link OperationCanceledException} if the operation was canceled for
     *      any reason, including the user canceling any operation
     * <li> {@link IOException} if the authenticator experienced an I/O problem
     *      updating settings, usually because of network trouble
     * </ul>
     */
    CARAPI GetAuthTokenByFeatures(
        /* [in] */ const String& accountType,
        /* [in] */ const String& authTokenType,
        /* [in] */ ArrayOf<String>* features,
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* addAccountOptions,
        /* [in] */ IBundle* getAuthTokenOptions,
        /* [in] */ IAccountManagerCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ IAccountManagerFuture** accountManagerFuture);

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
    static CARAPI NewChooseAccountIntent(
        /* [in] */ IAccount* selectedAccount,
        /* [in] */ ArrayOf<IAccount*>* allowableAccounts,
        /* [in] */ ArrayOf<String>* allowableAccountTypes,
        /* [in] */ Boolean alwaysPromptForAccount,
        /* [in] */ const String& descriptionOverrideText,
        /* [in] */ const String& addAccountAuthTokenType,
        /* [in] */ ArrayOf<String>*  addAccountRequiredFeatures,
        /* [in] */ IBundle* addAccountOptions,
        /* [out] */ IIntent** intent);

    /**
     * Adds an {@link OnAccountsUpdateListener} to this instance of the
     * {@link AccountManager}.  This listener will be notified whenever the
     * list of accounts on the device changes.
     *
     * <p>As long as this listener is present, the AccountManager instance
     * will not be garbage-collected, and neither will the {@link Context}
     * used to retrieve it, which may be a large Activity instance.  To avoid
     * memory leaks, you must remove this listener before then.  Normally
     * listeners are added in an Activity or Service's {@link Activity#onCreate}
     * and removed in {@link Activity#onDestroy}.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>No permission is required to call this method.
     *
     * @param listener The listener to send notifications to
     * @param handler {@link Handler} identifying the thread to use
     *     for notifications, null for the main thread
     * @param updateImmediately If true, the listener will be invoked
     *     (on the handler thread) right away with the current account list
     * @throws IllegalArgumentException if listener is null
     * @throws IllegalStateException if listener was already added
     */
    CARAPI AddOnAccountsUpdatedListener(
        /* [in] */ IOnAccountsUpdateListener* listener,
        /* [in] */ IHandler* handler,
        /* [in] */ Boolean updateImmediately);

    /**
     * Removes an {@link OnAccountsUpdateListener} previously registered with
     * {@link #addOnAccountsUpdatedListener}.  The listener will no longer
     * receive notifications of account changes.
     *
     * <p>It is safe to call this method from the main thread.
     *
     * <p>No permission is required to call this method.
     *
     * @param listener The previously added listener to remove
     * @throws IllegalArgumentException if listener is null
     * @throws IllegalStateException if listener was not already added
     */
    CARAPI RemoveOnAccountsUpdatedListener(
        /* [in] */ IOnAccountsUpdateListener* listener);

private:
    CARAPI EnsureNotOnMainThread();

    CARAPI_(void) PostToHandler(
        /* [in] */ IHandler* handler,
        /* [in] */ IAccountManagerCallback* cb,
        /* [in] */ IAccountManagerFuture* future);

    CARAPI_(void) PostToHandler(
        /* [in] */ IHandler* handler,
        /* [in] */ IOnAccountsUpdateListener* listener,
        /* [in] */ ArrayOf<IAccount*>* accounts);

    CARAPI ConvertErrorToException(
        /* [in] */ Int32 code,
        /* [in] */ const String& message);

private:
    static const String TAG;

    IContext* mContext;
    AutoPtr<IIAccountManager> mService;
    AutoPtr<IHandler> mMainHandler;

    AutoPtr<IHashMap> mAccountsUpdatedListeners;
    Object mAccountsUpdatedListenersLock;

    /**
     * BroadcastReceiver that listens for the LOGIN_ACCOUNTS_CHANGED_ACTION intent
     * so that it can read the updated list of accounts and send them to the listener
     * in mAccountsUpdatedListeners.
     */
    AutoPtr<AccountsChangedBroadcastReceiver> mAccountsChangedBroadcastReceiver;

    friend class CChooseResponse;
    friend class CAccountManagerAmsResponse;
    friend class CAccountManagerFutureResponse;
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ACCOUNTS_CACCOUNTMANAGER_H__
