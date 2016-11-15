
#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_CACCOUNTMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_ACCOUNTS_CACCOUNTMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_Accounts_CAccountManagerService.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAccountAndUser;
using Elastos::Droid::Accounts::IAccountAuthenticatorResponse;
using Elastos::Droid::Accounts::IAccountManagerResponse;
using Elastos::Droid::Accounts::IAuthenticatorDescription;
using Elastos::Droid::Accounts::IIAccountAuthenticator;
using Elastos::Droid::Accounts::IIAccountAuthenticatorResponse;
using Elastos::Droid::Accounts::IIAccountManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IRegisteredServicesCacheListener;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::ISQLiteOpenHelper;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Server::Accounts::IIAccountAuthenticatorCache;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::IAtomicReference;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

CarClass(CAccountManagerService)
    , public Object
    , public IBinder
    , public IIAccountManager
    , public IRegisteredServicesCacheListener
{
    friend class UserAccounts;
    friend class DatabaseHelper;
    friend class Session;
    friend class RemoveAccountSession;
    friend class GetAuthTokenSession;
    friend class GetAccountsByTypeAndFeatureSession;

public:
    class DatabaseHelper;
    class UserAccounts
        : public Object
    {
    public:
        UserAccounts(
            /* [in] */ IContext* context,
            /* [in] */ Int32 userId);

    private:
        Int32 mUserId;
        AutoPtr<DatabaseHelper> mOpenHelper;
        AutoPtr<IHashMap> mCredentialsPermissionNotificationIds;
        AutoPtr<IHashMap> mSigninRequiredNotificationIds;
        Object mCacheLock;
        /** protected by the {@link #cacheLock} */
        AutoPtr<IHashMap> mAccountCache;
        /** protected by the {@link #cacheLock} */
        AutoPtr<IHashMap> mUserDataCache;
        /** protected by the {@link #cacheLock} */
        AutoPtr<IHashMap> mAuthTokenCache;

        /**
         * protected by the {@link #cacheLock}
         *
         * Caches the previous names associated with an account. Previous names
         * should be cached because we expect that when an Account is renamed,
         * many clients will receive a LOGIN_ACCOUNTS_CHANGED broadcast and
         * want to know if the accounts they care about have been renamed.
         *
         * The previous names are wrapped in an {@link AtomicReference} so that
         * we can distinguish between those accounts with no previous names and
         * those whose previous names haven't been cached (yet).
         */
        AutoPtr<IHashMap> mPreviousNameCache;

        friend class Session;
        friend class CAccountManagerService;
        friend class GetAuthTokenSession;
        friend class GetAccountsByTypeAndFeatureSession;
    };

    class MessageHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CAccountManagerService::MessageHandler")

        MessageHandler(
            /* [in] */ CAccountManagerService* service);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CAccountManagerService* mHost;
    };

    class DatabaseHelper
        : public SQLiteOpenHelper
    {
    public:
        DatabaseHelper(
            /* [in] */ IContext* context,
            /* [in] */ Int32 userId,
            /* [in] */ const String& name);

        /**
         * This call needs to be made while the mCacheLock is held. The way to
         * ensure this is to get the lock any time a method is called ont the DatabaseHelper
         * @param db The database.
         */
        // @Override
        CARAPI OnCreate(
            /* [in] */ ISQLiteDatabase* database);

        CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 newVersion);

        CARAPI OnOpen(
            /* [in] */ ISQLiteDatabase* db);

    private:
        CARAPI_(void) CreateSharedAccountsTable(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) AddOldAccountNameColumn(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) CreateAccountsDeletionTrigger(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) CreateGrantsTable(
            /* [in] */ ISQLiteDatabase* db);

    private:
        static const String TAG;
    };

    class Session
        : public Object
        , public IIAccountAuthenticatorResponse
        , public IBinder
        , public IProxyDeathRecipient
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        Session();

        CARAPI_(AutoPtr<IAccountManagerResponse>) GetResponseAndClose();

        // @Override
        CARAPI ProxyDied();

        CARAPI Bind();

        CARAPI_(void) ScheduleTimeout();

        CARAPI_(void) CancelTimeout();

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

        virtual CARAPI Run() = 0;

        CARAPI OnTimedOut();

        // @Override
        CARAPI OnResult(
            /* [in] */ IBundle* result);

        // @Override
        CARAPI OnRequestContinued();

        // @Override
        CARAPI OnError(
            /* [in] */ Int32 errorCode,
            /* [in] */ const String& errorMessage);

        virtual CARAPI_(String) ToString();

        CARAPI constructor(
            /* [in] */ UserAccounts* accounts,
            /* [in] */ IAccountManagerResponse* response,
            /* [in] */ const String& accountType,
            /* [in] */ Boolean expectActivityLaunch,
            /* [in] */ Boolean stripAuthTokenFromResult,
            /* [in] */ CAccountManagerService* host);

        CARAPI ToString(
            /* [in] */ String* result);

    protected:
        CARAPI_(String) ToDebugString();

        CARAPI_(String) ToDebugString(
            /* [in] */ Int64 now);

    private:
        CARAPI_(void) Close();

        CARAPI_(void) Unbind();

        /**
         * find the component name for the authenticator and initiate a bind
         * if no authenticator or the bind fails then return false, otherwise return true
         */
        CARAPI_(Boolean) BindToAuthenticator(
            /* [in] */ const String& authenticatorType);

    public:
        AutoPtr<IAccountManagerResponse> mResponse;
        String mAccountType;
        Boolean mExpectActivityLaunch;
        Int64 mCreationTime;

        Int32 mNumResults;

        AutoPtr<IIAccountAuthenticator> mAuthenticator;

    protected:
        AutoPtr<UserAccounts> mAccounts;
        CAccountManagerService* mHost;

    private:
        static const String TAG;
        Int32 mNumRequestContinued;
        Int32 mNumErrors;

        Boolean mStripAuthTokenFromResult;
    };

    class TestFeaturesSession
        : public Session
    {
    public:
        // @Override
        CARAPI Run();

        // @Override
        CARAPI OnResult(
            /* [in] */ IBundle* result);

        CARAPI constructor(
            /* [in] */ IInterface* accounts,
            /* [in] */ IAccountManagerResponse* response,
            /* [in] */ IAccount* account,
            /* [in] */ const ArrayOf<String>& features,
            /* [in] */ IIAccountManager* host);

    protected:
        // @Override
        CARAPI_(String) ToDebugString(
            /* [in] */ Int64 now);

    private:
        AutoPtr<ArrayOf<String> > mFeatures;
        AutoPtr<IAccount> mAccount;
        static const String TAG;
    };

    class RemoveAccountSession
        : public Session
    {
    public:
        CARAPI constructor(
            /* [in] */ IInterface* accounts,
            /* [in] */ IAccountManagerResponse* response,
            /* [in] */ IAccount* account,
            /* [in] */ IIAccountManager* host);

        // @Override
        CARAPI Run();

        // @Override
        CARAPI OnResult(
            /* [in] */ IBundle* result);

    protected:
        // @Override
        CARAPI_(String) ToDebugString(
            /* [in] */ Int64 now);

    public:
        AutoPtr<IAccount> mAccount;
        static const String TAG;
    };

    class GetAuthTokenLabelSession
        : public Session
    {
    public:
        // @Override
        CARAPI Run();

        // @Override
        CARAPI OnResult(
            /* [in] */ IBundle* result);

        CARAPI constructor(
            /* [in] */ IInterface* accounts,
            /* [in] */ IAccountManagerResponse* response,
            /* [in] */ const String& accountType,
            /* [in] */ const String& authTokenType,
            /* [in] */ IIAccountManager* host);

    protected:
        // @Override
        CARAPI_(String) ToDebugString(
            /* [in] */ Int64 now);

    private:
        String mAuthTokenType;
    };

    class GetAuthTokenSession
        : public Session
    {
    public:
        GetAuthTokenSession();

        // @Override
        CARAPI Run();

        // @Override
        CARAPI OnResult(
            /* [in] */ IBundle* result);

        CARAPI constructor(
            /* [in] */ IInterface* accounts,
            /* [in] */ IAccountManagerResponse* response,
            /* [in] */ IAccount* account,
            /* [in] */ const String& authTokenType,
            /* [in] */ Boolean expectActivityLaunch,
            /* [in] */ IBundle* loginOptions,
            /* [in] */ Boolean notifyOnAuthFailure,
            /* [in] */ Boolean permissionGranted,
            /* [in] */ Boolean customTokens,
            /* [in] */ IIAccountManager* host);

    protected:
        // @Override
        CARAPI_(String) ToDebugString(
            /* [in] */ Int64 now);

    private:
        AutoPtr<IAccount> mAccount;
        String mAuthTokenType;
        AutoPtr<IBundle> mLoginOptions;
        Boolean mNotifyOnAuthFailure;
        Boolean mPermissionGranted;
        Boolean mCustomTokens;
    };

    class AddAcountSession
        : public Session
    {
    public:
        // @Override
        CARAPI Run();

        CARAPI constructor(
            /* [in] */ IInterface* accounts,
            /* [in] */ IAccountManagerResponse* response,
            /* [in] */ const String& accountType,
            /* [in] */ Boolean expectActivityLaunch,
            /* [in] */ const String& authTokenType,
            /* [in] */ const ArrayOf<String>& requiredFeatures,
            /* [in] */ IBundle* options,
            /* [in] */ IIAccountManager* host);

    protected:
        // @Override
        CARAPI_(String) ToDebugString(
            /* [in] */ Int64 now);

    private:
        String mAuthTokenType;
        AutoPtr<ArrayOf<String> > mRequiredFeatures;
        AutoPtr<IBundle> mOptions;
    };

    class ConfirmCredentialsAsUserSession
        : public Session
    {
    public:
        // @Override
        CARAPI Run();

        CARAPI constructor(
            /* [in] */ IInterface* accounts,
            /* [in] */ IAccountManagerResponse* response,
            /* [in] */ Boolean expectActivityLaunch,
            /* [in] */ IAccount* account,
            /* [in] */ IBundle* options,
            /* [in] */ IIAccountManager* host);

    protected:
        // @Override
        CARAPI_(String) ToDebugString(
            /* [in] */ Int64 now);

    private:
        AutoPtr<IAccount> mAccount;
        AutoPtr<IBundle> mOptions;
    };

    class UpdateCredentialsSession
        : public Session
    {
    public:
        // @Override
        CARAPI Run();

        CARAPI constructor(
            /* [in] */ IInterface* accounts,
            /* [in] */ IAccountManagerResponse* response,
            /* [in] */ Boolean expectActivityLaunch,
            /* [in] */ IAccount* account,
            /* [in] */ const String& authTokenType,
            /* [in] */ IBundle* loginOptions,
            /* [in] */ IIAccountManager* host);

    protected:
        // @Override
        CARAPI_(String) ToDebugString(
            /* [in] */ Int64 now);

    private:
        AutoPtr<IAccount> mAccount;
        String mAuthTokenType;
        AutoPtr<IBundle> mLoginOptions;
    };

    class EditPropertiesSession
        : public Session
    {
    public:
        // @Override
        CARAPI Run();

        CARAPI constructor(
            /* [in] */ IInterface* accounts,
            /* [in] */ IAccountManagerResponse* response,
            /* [in] */ const String accountType,
            /* [in] */ Boolean expectActivityLaunch,
            /* [in] */ IIAccountManager* host);

    protected:
        CARAPI_(String) ToDebugString(
            /* [in] */ Int64 now);
    };

    class GetAccountsByTypeAndFeatureSession
        : public Session
    {
    public:
        GetAccountsByTypeAndFeatureSession();

        // @Override
        CARAPI Run();

        CARAPI CheckAccount();

        // @Override
        CARAPI OnResult(
            /* [in] */ IBundle* result);

        CARAPI SendResult();

        CARAPI constructor(
            /* [in] */ IInterface* accounts,
            /* [in] */ IAccountManagerResponse* response,
            /* [in] */ const String& type,
            /* [in] */ const ArrayOf<String>& features,
            /* [in] */ Int32 callingUid,
            /* [in] */ IIAccountManager* host);

    protected:
        // @Override
        CARAPI_(String) ToDebugString(
            /* [in] */ Int64 now);

    private:
        AutoPtr<ArrayOf<String> > mFeatures;
        AutoPtr<ArrayOf<IAccount*> > mAccountsOfType;
        AutoPtr<IList> mAccountsWithFeatures;
        Int32 mCurrentAccount;
        Int32 mCallingUid;
        static const String TAG;
    };

    class CopyAccountToUserSession
        : public Session
    {
    public:
        CARAPI constructor(
            /* [in] */ IInterface* accounts,
            /* [in] */ IAccountManagerResponse* response,
            /* [in] */ IAccount* account,
            /* [in] */ Boolean expectActivityLaunch,
            /* [in] */ Boolean stripAuthTokenFromResult,
            /* [in] */ IIAccountManager* host,
            /* [in] */ IInterface* toAccounts);

        // @Override
        CARAPI Run();

        // @Override
        CARAPI OnResult(
            /* [in] */ IBundle* result);

    protected:
         // @Override
        CARAPI_(String) ToDebugString(
            /* [in] */ Int64 now);

    private:
        AutoPtr<IAccount> mAccount;
        AutoPtr<UserAccounts> mToAccounts;
    };

    class CompleteCloningAccountSession
        : public Session
    {
    public:
        CARAPI constructor(
            /* [in] */ IInterface* accounts,
            /* [in] */ IAccountManagerResponse* response,
            /* [in] */ IAccount* account,
            /* [in] */ Boolean expectActivityLaunch,
            /* [in] */ Boolean stripAuthTokenFromResult,
            /* [in] */ IIAccountManager* host,
            /* [in] */ IBundle* result);

        // @Override
        CARAPI Run();

        // @Override
        CARAPI OnResult(
            /* [in] */ IBundle* result);

        // @Override
        CARAPI OnError(
            /* [in] */ Int32 errorCode,
            /* [in] */ const String& errorMessage);

    protected:
        // @Override
        CARAPI_(String) ToDebugString(
            /* [in] */ Int64 now);

    private:
        AutoPtr<IAccount> mAccount;
        AutoPtr<IBundle> mResult;
    };

    class AddAccountAsUserSession
        : public Session
    {
    public:
        CARAPI constructor(
            /* [in] */ IInterface* accounts,
            /* [in] */ IAccountManagerResponse* response,
            /* [in] */ const String& accountType,
            /* [in] */ Boolean expectActivityLaunch,
            /* [in] */ Boolean stripAuthTokenFromResult,
            /* [in] */ IIAccountManager* host,
            /* [in] */ const String& authTokenType,
            /* [in] */ ArrayOf<String>* requiredFeatures,
            /* [in] */ IBundle* options);

        // @Override
        CARAPI Run();

    protected:
        // @Override
        CARAPI_(String) ToDebugString(
            /* [in] */ Int64 now);

    private:
        String mAuthTokenType;
        AutoPtr<ArrayOf<String> > mRequiredFeatures;
        AutoPtr<IBundle> mOptions;
    };

private:
    class PurgeOldGrantsAllBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        PurgeOldGrantsAllBroadcastReceiver(
            /* [in] */ CAccountManagerService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CAccountManagerService::PurgeOldGrantsAllBroadcastReceiver: ")
    private:
        CAccountManagerService* mHost;
    };

    class OnUserRemovedBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        OnUserRemovedBroadcastReceiver(
            /* [in] */ CAccountManagerService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CAccountManagerService::OnUserRemovedBroadcastReceiver: ")
    private:
        CAccountManagerService* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CAccountManagerService();

    /**
     * This should only be called by system code. One should only call this after the service
     * has started.
     * @return a reference to the AccountManagerService instance
     * @hide
     */
    static CARAPI_(AutoPtr<CAccountManagerService>) GetSingleton();

    CARAPI_(void) SystemReady();

    /**
     * Validate internal set of accounts against installed authenticators for
     * given user. Clears cached authenticators before validating.
     */
    CARAPI_(void) ValidateAccounts(
        /* [in] */ Int32 userId);

    CARAPI OnServiceChanged(
        /* [in] */ IInterface* type,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean removed);

    // @Override
    CARAPI GetPassword(
        /* [in] */ IAccount* account,
        /* [out] */ String* password);

    // @Override
    CARAPI GetPreviousName(
        /* [in] */ IAccount* account,
        /* [out] */ String* result);

    // @Override
    CARAPI GetUserData(
        /* [in] */ IAccount* account,
        /* [in] */ const String& key,
        /* [out] */ String* userData);

    CARAPI GetAuthenticatorTypes(
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<IAuthenticatorDescription*>** types);

    CARAPI AddAccountExplicitly(
        /* [in] */ IAccount* account,
        /* [in] */ const String& password,
        /* [in] */ IBundle* extras,
        /* [out] */ Boolean* result);

    CARAPI CompleteCloningAccount(
        /* [in] */ IBundle* result,
        /* [in] */ IAccount* account,
        /* [in] */ UserAccounts* targetUser);

    // @Override
    CARAPI HasFeatures(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<String>* features);

    // @Override
    CARAPI RenameAccount(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ IAccount* accountToRename,
        /* [in] */ const String& newName);

    // @Override
    CARAPI RemoveAccount(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ IAccount* account);

    // @Override
    CARAPI RemoveAccountAsUser(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userId);

    // @Override
    CARAPI InvalidateAuthToken(
        /* [in] */ const String& accountType,
        /* [in] */ const String& authToken);

    // @Override
    CARAPI PeekAuthToken(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [out] */ String* token);

    // @Override
    CARAPI SetAuthToken(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ const String& authToken);

    // @Override
    CARAPI SetPassword(
        /* [in] */ IAccount* account,
        /* [in] */ const String& password);

    // @Override
    CARAPI ClearPassword(
        /* [in] */ IAccount* account);

    // @Override
    CARAPI SetUserData(
        /* [in] */ IAccount* account,
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    // @Override
    CARAPI GetAuthTokenLabel(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ const String& accountType,
        /* [in] */ const String& authTokenType);

    // @Override
    CARAPI GetAuthToken(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ Boolean notifyOnAuthFailure,
        /* [in] */ Boolean expectActivityLaunch,
        /* [in] */ IBundle* options);

    // @Override
    CARAPI AddAccount(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ const String& accountType,
        /* [in] */ const String& authTokenType,
        /* [in] */ ArrayOf<String>* requiredFeatures,
        /* [in] */ Boolean expectActivityLaunch,
        /* [in] */ IBundle* options);

    // @Override
    CARAPI AddAccountAsUser(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ const String& accountType,
        /* [in] */ const String& authTokenType,
        /* [in] */ ArrayOf<String>* requiredFeatures,
        /* [in] */ Boolean expectActivityLaunch,
        /* [in] */ IBundle* optionsIn,
        /* [in] */ Int32 userId);

    CARAPI ConfirmCredentialsAsUser(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ IBundle* options,
        /* [in] */ Boolean expectActivityLaunch,
        /* [in] */ Int32 userId);

    // @Override
    CARAPI UpdateCredentials(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ Boolean expectActivityLaunch,
        /* [in] */ IBundle* options);

    // @Override
    CARAPI EditProperties(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ const String& accountType,
        /* [in] */ Boolean expectActivityLaunch);

    /**
     * Returns the accounts for a specific user
     * @hide
     */
    CARAPI GetAccounts(
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<IAccount*>** accounts);

    /**
     * Returns accounts for all running users.
     *
     * @hide
     */
    CARAPI GetRunningAccounts(
        /* [out] */ ArrayOf<IAccountAndUser*>** accounts);

    CARAPI GetAllAccounts(
        /* [out, callee] */ ArrayOf<IAccountAndUser*>** accounts);

    CARAPI GetAccountsAsUser(
        /* [in] */ const String& accountType,
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<IAccount*>** accounts);

    // @Override
    CARAPI AddSharedAccountAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI RenameSharedAccountAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& newName,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI RemoveSharedAccountAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetSharedAccountsAsUser(
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<IAccount*>** result);

    // @Override
    CARAPI GetAccounts(
        /* [in] */ const String& accountType,
        /* [out, callee] */ ArrayOf<IAccount*>** accounts);

    // @Override
    CARAPI GetAccountsForPackage(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [out, callee] */ ArrayOf<IAccount*>** accounts);

    // @Override
    CARAPI GetAccountsByTypeForPackage(
        /* [in] */ const String& type,
        /* [in] */ const String& packageName,
        /* [out, callee] */ ArrayOf<IAccount*>** accounts);

    // @Override
    CARAPI GetAccountsByFeatures(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ const String& accountType,
        /* [in] */ ArrayOf<String>* features);

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    // @Override
    CARAPI UpdateAppPermission(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean value);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IPackageManager* packageManager,
        /* [in] */ IIAccountAuthenticatorCache* authenticatorCache);

    CARAPI ToString(
        /* [out] */ String* result);

protected:
    CARAPI_(AutoPtr<UserAccounts>) GetUserAccounts(
        /* [in] */ Int32 userId);

    /* For testing */
    CARAPI RemoveAccountInternal(
        /* [in] */ IAccount* account);

    // void dump(FileDescriptor fd, PrintWriter fout, String[] args);

    CARAPI_(void) InstallNotification(
        /* [in] */ Int32 notificationId,
        /* [in] */ INotification* n,
        /* [in] */ IUserHandle* user);

    CARAPI CancelNotification(
        /* [in] */ Int32 id,
        /* [in] */ IUserHandle* user);

    CARAPI GetAccountsFromCacheLocked(
        /* [in] */ UserAccounts* userAccounts,
        /* [in] */ const String& accountType,
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& callingPackage,
        /* [out, callee] */ ArrayOf<IAccount*>** result);

    CARAPI_(void) WriteUserDataIntoCacheLocked(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ IAccount* account,
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    CARAPI_(void) WriteAuthTokenIntoCacheLocked(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ IAccount* account,
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    CARAPI_(String) ReadAuthTokenInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType);

    CARAPI ReadUserDataInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [in] */ const String& key,
        /* [out] */ String* result);

    CARAPI ReadUserDataForAccountFromDatabaseLocked(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ IAccount* account,
        /* [out] */ IHashMap** result);

    CARAPI ReadAuthTokensForAccountFromDatabaseLocked(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ IAccount* account,
        /* [out] */ IHashMap** result);

private:
    CARAPI_(AutoPtr<IUserManager>) GetUserManager();

    /* Caller should lock mUsers */
    CARAPI_(AutoPtr<UserAccounts>) InitUserLocked(
        /* [in] */ Int32 userId);

    CARAPI_(void) PurgeOldGrantsAll();

    CARAPI PurgeOldGrants(
        /* [in] */ UserAccounts* accounts);

    /**
     * Validate internal set of accounts against installed authenticators for
     * given user. Clear cached authenticators before validating when requested.
     */
    CARAPI ValidateAccountsInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ Boolean invalidateAuthenticatorCache);

    CARAPI_(AutoPtr<UserAccounts>) GetUserAccountsForCaller();

    CARAPI_(void) OnUserRemoved(
        /* [in] */ IIntent* intent);

    CARAPI_(void) OnUserStarted(
        /* [in] */ IIntent* intent);

    CARAPI ReadPasswordInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [out] */ String* result);

    CARAPI ReadPreviousNameInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [out] */ String* result);

    CARAPI EnforceCrossUserPermission(
        /* [in] */ Int32 userId,
        /* [in] */ const String& errorMessage);

    CARAPI CopyAccountToUser(
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userFrom,
        /* [in] */ Int32 userTo,
        /* [out] */ Boolean* result);

    CARAPI AddAccountInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [in] */ const String& password,
        /* [in] */ IBundle* extras,
        /* [in] */ Boolean restricted,
        /* [out] */ Boolean* result);

    /**
     * Adds the account to all limited users as shared accounts. If the user is currently
     * running, then clone the account too.
     * @param account the account to share with limited users
     */
    CARAPI AddAccountToLimitedUsers(
        /* [in] */ IAccount* account);

    CARAPI_(Int64) InsertExtraLocked(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int64 accountId,
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    CARAPI RenameAccountInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* accountToRename,
        /* [in] */ const String& newName,
        /* [out] */ IAccount** result);

    CARAPI RemoveAccountInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account);

    CARAPI InvalidateAuthTokenLocked(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& accountType,
        /* [in] */ const String& authToken);

    CARAPI SaveAuthTokenToDatabase(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [in] */ const String& type,
        /* [in] */ const String& authToken,
        /* [out] */ Boolean* result);

    CARAPI SetPasswordInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [in] */ const String& password);

    CARAPI_(void) SendAccountsChangedBroadcast(
        /* [in] */ Int32 userId);

    CARAPI SetUserdataInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    CARAPI OnResult(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ IBundle* result);

    CARAPI CreateNoCredentialsPermissionNotification(
        /* [in] */ IAccount* account,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<IIntent>) NewGrantCredentialsPermissionIntent(
        /* [in] */ IAccount* account,
        /* [in] */ Int32 uid,
        /* [in] */ IAccountAuthenticatorResponse* response,
        /* [in] */ const String& authTokenType,
        /* [in] */ const String& authTokenLabel);

    CARAPI_(AutoPtr<IInteger32>) GetCredentialPermissionNotificationId(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ Int32 uid);

    CARAPI_(Int32) GetSigninRequiredNotificationId(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account);

    CARAPI ShowCantAddAccount(
        /* [in] */ Int32 errorCode,
        /* [in] */ Int32 userId);

    CARAPI GetAccounts(
        /* [in] */ const ArrayOf<Int32>& userIds,
        /* [out, callee] */ ArrayOf<IAccountAndUser*>** accounts);

    CARAPI GetAccountsAsUser(
        /* [in] */ const String& type,
        /* [in] */ Int32 userId,
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 packageUid,
        /* [out, callee] */ ArrayOf<IAccount*>** result);

    CARAPI GetAccountIdLocked(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ IAccount* account,
        /* [out] */ Int64* result);

    CARAPI GetExtrasIdLocked(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int64 accountId,
        /* [in] */ const String& key,
        /* [out] */ Int64* result);

    static CARAPI GetDatabaseName(
        /* [in] */ Int32 userId,
        /* [out] */ String* name);

    /**
     * Searches array of arguments for the specified string
     * @param args array of argument strings
     * @param value value to search for
     * @return true if the value is contained in the array
     */
    static CARAPI_(Boolean) ScanArgs(
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ const String& value);

    // void dumpUser(UserAccounts userAccounts, FileDescriptor fd, PrintWriter fout,
    //         String[] args, boolean isCheckinRequest);

    CARAPI DoNotification(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [in] */ ICharSequence* message,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 userId);

    /** Succeeds if any of the specified permissions are granted. */
    CARAPI CheckBinderPermission(
        /* [in] */ ArrayOf<String>* permissions);

    CARAPI HandleIncomingUser(
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI IsPrivileged(
        /* [in] */ Int32 callingUid,
        /* [out] */ Boolean* result);

    CARAPI_(Boolean) PermissionIsGranted(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ Int32 callerUid);

    CARAPI_(Boolean) HasAuthenticatorUid(
        /* [in] */ const String& accountType,
        /* [in] */ Int32 callingUid);

    CARAPI_(Boolean) HasExplicitlyGrantedPermission(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ Int32 callerUid);

    CARAPI CheckCallingUidAgainstAuthenticator(
        /* [in] */ IAccount* account);

    CARAPI CheckAuthenticateAccountsPermission(
        /* [in] */ IAccount* account);

    CARAPI CheckReadAccountsPermission();

    CARAPI CheckManageAccountsPermission();

    CARAPI CheckManageAccountsOrUseCredentialsPermissions();

    CARAPI_(Boolean) CanUserModifyAccounts(
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) CanUserModifyAccountsForType(
        /* [in] */ Int32 userId,
        /* [in] */ const String& accountType);

    /**
     * Allow callers with the given uid permission to get credentials for account/authTokenType.
     * <p>
     * Although this is public it can only be accessed via the AccountManagerService object
     * which is in the system. This means we don't need to protect it with permissions.
     * @hide
     */
    CARAPI GrantAppPermission(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ Int32 uid);

    /**
     * Don't allow callers with the given uid permission to get credentials for
     * account/authTokenType.
     * <p>
     * Although this is public it can only be accessed via the AccountManagerService object
     * which is in the system. This means we don't need to protect it with permissions.
     * @hide
     */
    CARAPI RevokeAppPermission(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ Int32 uid);

    static CARAPI_(String) StringArrayToString(
        /* [in] */ ArrayOf<String>* value);

    CARAPI_(void) RemoveAccountFromCacheLocked(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account);

    /**
     * This assumes that the caller has already checked that the account is not already present.
     */
    CARAPI_(void) InsertAccountIntoCacheLocked(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account);

    CARAPI FilterSharedAccounts(
        /* [in] */ UserAccounts* userAccounts,
        /* [in] */ ArrayOf<IAccount*>* unfiltered,
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& callingPackage,
        /* [out, callee] */ ArrayOf<IAccount*>** result);

    CARAPI GetContextForUser(
        /* [in] */ IUserHandle* user,
        /* [out] */ IContext** result);

    static CARAPI_(AutoPtr<IIntent>) InitAccountsChangedIntent();

    static CARAPI_(AutoPtr<IAtomicReference>) InitAtomicRef();

private:
    // Messages that can be sent on mHandler
    static const Int32 MESSAGE_TIMED_OUT;
    static const Int32 MESSAGE_COPY_SHARED_ACCOUNT;
    static const Int32 TIMEOUT_DELAY_MS;

    static const String TAG;

    static const String DATABASE_NAME;
    static const Int32 DATABASE_VERSION;

    static const String TABLE_ACCOUNTS;
    static const String ACCOUNTS_ID;
    static const String ACCOUNTS_NAME;
    static const String ACCOUNTS_TYPE;
    static const String ACCOUNTS_TYPE_COUNT;
    static const String ACCOUNTS_PASSWORD;
    static const String ACCOUNTS_PREVIOUS_NAME;

    static const String TABLE_AUTHTOKENS;
    static const String AUTHTOKENS_ID;
    static const String AUTHTOKENS_ACCOUNTS_ID;
    static const String AUTHTOKENS_TYPE;
    static const String AUTHTOKENS_AUTHTOKEN;

    static const String TABLE_GRANTS;
    static const String GRANTS_ACCOUNTS_ID;
    static const String GRANTS_AUTH_TOKEN_TYPE;
    static const String GRANTS_GRANTEE_UID;

    static const String TABLE_EXTRAS;
    static const String EXTRAS_ID;
    static const String EXTRAS_ACCOUNTS_ID;
    static const String EXTRAS_KEY;
    static const String EXTRAS_VALUE;

    static const String TABLE_META;
    static const String META_KEY;
    static const String META_VALUE;

    static const String TABLE_SHARED_ACCOUNTS;

    static const String ACCOUNT_NAME_TYPE_PROJECTION[];
    static const String ACCOUNT_TYPE_COUNT_PROJECTION[];
    static const AutoPtr<IIntent> ACCOUNTS_CHANGED_INTENT;

    static const String COUNT_OF_MATCHING_GRANTS;

    static const String SELECTION_AUTHTOKENS_BY_ACCOUNT;
    static const String COLUMNS_AUTHTOKENS_TYPE_AND_AUTHTOKEN[];

    static const String SELECTION_USERDATA_BY_ACCOUNT;
    static const String COLUMNS_EXTRAS_KEY_AND_VALUE[];

    static AutoPtr<ArrayOf<IAccount*> > EMPTY_ACCOUNT_ARRAY;

public:
    AutoPtr<IHandler> mMessageHandler;

private:
    AutoPtr<IContext> mContext;

    AutoPtr<IPackageManager> mPackageManager;
    AutoPtr<IUserManager> mUserManager;

    AutoPtr<IHandlerThread> mMessageThread;
    AutoPtr<IIAccountAuthenticatorCache> mAuthenticatorCache;
    AutoPtr<DatabaseHelper> mOpenHelper;

    AutoPtr<IHashMap> mSessions;
    AutoPtr<IAtomicInteger32> mNotificationIds;

    AutoPtr<ISparseArray> mUsers;

    AutoPtr<IHashMap> mSigninRequiredNotificationIds;
    static AutoPtr<IAtomicReference> sThis;
};

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_ACCOUNTS_CACCOUNTMANAGERSERVICE_H__
