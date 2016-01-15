
#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_CACCOUNTMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_ACCOUNTS_CACCOUNTMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_Accounts_CAccountManagerService.h"
#include "accounts/IAccountAuthenticatorCache.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"
#include "elastos/droid/os/HandlerBase.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/Pair.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::Pair;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::App::INotification;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::ISQLiteOpenHelper;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAuthenticatorDescription;
using Elastos::Droid::Accounts::IAccountManagerResponse;
using Elastos::Droid::Accounts::IAccountAuthenticatorCache;
using Elastos::Droid::Accounts::IAccountAuthenticatorResponse;
using Elastos::Droid::Accounts::IAccountAndUser;


_ETL_NAMESPACE_BEGIN
template<> struct Hash<Pair<Pair<AutoPtr<IAccount>, String>*, AutoPtr<IInteger32> >*>
{
    size_t operator()(const Pair<Pair<AutoPtr<IAccount>, String>*, AutoPtr<IInteger32> >* s) const { return (size_t)s; }
};
_ETL_NAMESPACE_END


namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

class AccountManagerServiceSession;
class CRemoveAccountSession;
class CGetAuthTokenSession;
class CAccountManagerService;
class CGetAccountsByTypeAndFeatureSession;
class UserAccounts;

CarClass(CAccountManagerService)
{
public:
    class MessageHandler
        : public HandlerBase
    {
    public:
        MessageHandler(
            /* [in] */ CAccountManagerService* service)
            : mHost(service)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CAccountManagerService* mHost;
    };

    class DatabaseHelper
        : public ElRefBase
        , public ISQLiteOpenHelper
        , public SQLiteOpenHelper
    {
    public:
        DatabaseHelper(
            /* [in] */ IContext* context,
            /* [in] */ Int32 userId,
            /* [in] */ const String& name);

        CAR_INTERFACE_DECL();

        CARAPI GetDatabaseName(
            /* [out] */ String* name);

        CARAPI SetWriteAheadLoggingEnabled(
            /* [in] */ Boolean enabled);

        CARAPI GetWritableDatabase(
            /* [out] */ ISQLiteDatabase** database);

        CARAPI GetReadableDatabase(
            /* [out] */ ISQLiteDatabase** database);

        CARAPI Close();

        CARAPI OnConfigure(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI OnCreate(
            /* [in] */ ISQLiteDatabase* database);

        CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 newVersion);

        CARAPI OnDowngrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 newVersion);

        CARAPI OnOpen(
            /* [in] */ ISQLiteDatabase* db);

    private:
        CARAPI_(void) CreateAccountsDeletionTrigger(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) CreateGrantsTable(
            /* [in] */ ISQLiteDatabase* db);
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

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CAccountManagerService::PurgeOldGrantsAllBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        AutoPtr<CAccountManagerService> mHost;
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

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CAccountManagerService::OnUserRemovedBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        AutoPtr<CAccountManagerService> mHost;
    };

public:
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

    CARAPI GetPassword(
        /* [in] */ IAccount* account,
        /* [out] */ String* password);

    CARAPI GetUserData(
        /* [in] */ IAccount* account,
        /* [in] */ const String& key,
        /* [out] */ String* userData);

    CARAPI GetAuthenticatorTypes(
        /* [out, callee] */ ArrayOf<IAuthenticatorDescription*>** types);

    CARAPI AddAccount(
        /* [in] */ IAccount* account,
        /* [in] */ const String& password,
        /* [in] */ IBundle* extras,
        /* [out] */ Boolean* result);

    CARAPI HasFeatures(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ const ArrayOf<String>& features);

    CARAPI RemoveAccount(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ IAccount* account);

    CARAPI InvalidateAuthToken(
        /* [in] */ const String& accountType,
        /* [in] */ const String& authToken);

    CARAPI PeekAuthToken(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [out] */ String* token);

    CARAPI SetAuthToken(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ const String& authToken);

    CARAPI SetPassword(
        /* [in] */ IAccount* account,
        /* [in] */ const String& password);

    CARAPI ClearPassword(
        /* [in] */ IAccount* account);

    CARAPI SetUserData(
        /* [in] */ IAccount* account,
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    CARAPI GetAuthTokenLabel(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ const String& accountType,
        /* [in] */ const String& authTokenType);

    CARAPI GetAuthToken(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ Boolean notifyOnAuthFailure,
        /* [in] */ Boolean expectActivityLaunch,
        /* [in] */ IBundle* options);

    CARAPI AddAcount(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ const String& accountType,
        /* [in] */ const String& authTokenType,
        /* [in] */ const ArrayOf<String>& requiredFeatures,
        /* [in] */ Boolean expectActivityLaunch,
        /* [in] */ IBundle* options);

    CARAPI ConfirmCredentialsAsUser(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ IBundle* options,
        /* [in] */ Boolean expectActivityLaunch,
        /* [in] */ Int32 userId);

    CARAPI UpdateCredentials(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ Boolean expectActivityLaunch,
        /* [in] */ IBundle* options);

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

    CARAPI GetAccounts(
        /* [in] */ const String& accountType,
        /* [out, callee] */ ArrayOf<IAccount*>** accounts);

    CARAPI GetAccountsByFeatures(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ const String& accountType,
        /* [in] */ ArrayOf<String>* features);

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

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
        /* [in] */ Handle32 authenticatorCache);

protected:
    CARAPI_(AutoPtr<UserAccounts>) GetUserAccounts(
        /* [in] */ Int32 userId);

    /* For testing */
    CARAPI_(void) RemoveAccountInternal(
        /* [in] */ IAccount* account);

    // void dump(FileDescriptor fd, PrintWriter fout, String[] args);

    CARAPI_(void) InstallNotification(
        /* [in] */ Int32 notificationId,
        /* [in] */ INotification* n,
        /* [in] */ IUserHandle* user);

    CARAPI_(void) CancelNotification(
        /* [in] */ Int32 id,
        /* [in] */ IUserHandle* user);

    CARAPI_(AutoPtr< ArrayOf<IAccount*> >) GetAccountsFromCacheLocked(
        /* [in] */ UserAccounts* userAccounts,
        /* [in] */ const String& accountType);

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

    CARAPI_(String) ReadUserDataInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [in] */ const String& key);

    AutoPtr<HashMap<String, String> > ReadUserDataForAccountFromDatabaseLocked(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ IAccount* account);

    AutoPtr<HashMap<String, String> > ReadAuthTokensForAccountFromDatabaseLocked(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ IAccount* account);

private:
    CARAPI_(AutoPtr<IUserManager>) GetUserManager();

    CARAPI_(AutoPtr<UserAccounts>) InitUser(
        /* [in] */ Int32 userId);

    CARAPI_(void) PurgeOldGrantsAll();

    CARAPI_(void) PurgeOldGrants(
        /* [in] */ UserAccounts* accounts);

    /**
     * Validate internal set of accounts against installed authenticators for
     * given user. Clear cached authenticators before validating when requested.
     */
    CARAPI_(void) ValidateAccountsInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ Boolean invalidateAuthenticatorCache);

    CARAPI_(AutoPtr<UserAccounts>) GetUserAccountsForCaller();

    CARAPI_(void) OnUserRemoved(
        /* [in] */ IIntent* intent);

    CARAPI_(String) ReadPasswordInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account);

    CARAPI_(Boolean) AddAccountInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [in] */ const String& password,
        /* [in] */ IBundle* extras);

    CARAPI_(Int64) InsertExtraLocked(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int64 accountId,
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    CARAPI_(void) RemoveAccountInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account);

    CARAPI_(void) InvalidateAuthTokenLocked(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& accountType,
        /* [in] */ const String& authToken);

    CARAPI_(Boolean) SaveAuthTokenToDatabase(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [in] */ const String& type,
        /* [in] */ const String& authToken);

    CARAPI_(void) SetPasswordInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [in] */ const String& password);

    CARAPI_(void) SendAccountsChangedBroadcast(
        /* [in] */ Int32 userId);

    CARAPI_(void) SetUserdataInternal(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    CARAPI_(void) OnResult(
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ IBundle* result);

    CARAPI_(void) CreateNoCredentialsPermissionNotification(
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

    CARAPI GetAccounts(
        /* [in] */ const ArrayOf<Int32>& userIds,
        /* [out, callee] */ ArrayOf<IAccountAndUser*>** accounts);

    CARAPI_(Int64) GetAccountIdLocked(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ IAccount* account);

    CARAPI_(Int64) GetExtrasIdLocked(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int64 accountId,
        /* [in] */ const String& key);

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

    CARAPI_(void) DoNotification(
        /* [in] */ UserAccounts* accounts,
        /* [in] */ IAccount* account,
        /* [in] */ ICharSequence* message,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 userId);

    /** Succeeds if any of the specified permissions are granted. */
    CARAPI CheckBinderPermission(
        /* [in] */ ArrayOf<String>* permissions);

    CARAPI_(Boolean) InSystemImage(
        /* [in] */ Int32   callingUid);

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

    /**
     * Allow callers with the given uid permission to get credentials for account/authTokenType.
     * <p>
     * Although this is public it can only be accessed via the AccountManagerService object
     * which is in the system. This means we don't need to protect it with permissions.
     * @hide
     */
    CARAPI_(void) GrantAppPermission(
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
    CARAPI_(void) RevokeAppPermission(
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

public:
    // Messages that can be sent on mHandler
    static const Int32 MESSAGE_TIMED_OUT;
    static const Int32 TIMEOUT_DELAY_MS;

private:
    static const String TAG;

    static const String DATABASE_NAME;
    static const Int32 DATABASE_VERSION;

    static const String TABLE_ACCOUNTS;
    static const String ACCOUNTS_ID;
    static const String ACCOUNTS_NAME;
    static const String ACCOUNTS_TYPE;
    static const String ACCOUNTS_TYPE_COUNT;
    static const String ACCOUNTS_PASSWORD;

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

    static const String ACCOUNT_NAME_TYPE_PROJECTION[];
    static const String ACCOUNT_TYPE_COUNT_PROJECTION[];
    static const AutoPtr<IIntent> ACCOUNTS_CHANGED_INTENT;

    static const String COUNT_OF_MATCHING_GRANTS;

    static const String SELECTION_AUTHTOKENS_BY_ACCOUNT;
    static const String COLUMNS_AUTHTOKENS_TYPE_AND_AUTHTOKEN[];

    static const String SELECTION_USERDATA_BY_ACCOUNT;
    static const String COLUMNS_EXTRAS_KEY_AND_VALUE[];

    static AutoPtr< ArrayOf<IAccount*> > EMPTY_ACCOUNT_ARRAY;

public:
    AutoPtr<IHandler> mMessageHandler;

private:
    AutoPtr<IContext> mContext;

    AutoPtr<IPackageManager> mPackageManager;
    AutoPtr<IUserManager> mUserManager;

    AutoPtr<IHandlerThread> mMessageThread;
    AutoPtr<IAccountAuthenticatorCache> mAuthenticatorCache;
    AutoPtr<DatabaseHelper> mOpenHelper;

    AutoPtr<HashMap<String, AutoPtr<AccountManagerServiceSession> > > mSessions;
    Object mSessionsLock;
    AutoPtr<IAtomicInteger32> mNotificationIds;

    HashMap<Int32, AutoPtr<UserAccounts> > mUsers;
    Object mUsersLock;

    HashMap<AutoPtr<IAccount>, Int32> mSigninRequiredNotificationIds;
    //static AtomicReference<AccountManagerService> sThis =
    //         new AtomicReference<AccountManagerService>();
    static AutoPtr<CAccountManagerService> sThis;

    friend class UserAccounts;
    friend class DatabaseHelper;
    friend class AccountManagerServiceSession;
    friend class CRemoveAccountSession;
    friend class CGetAuthTokenSession;
    friend class CGetAccountsByTypeAndFeatureSession;
};

class UserAccounts : public ElRefBase
{
public:
    UserAccounts(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId);

    ~UserAccounts();

private:
    Int32 mUserId;
    AutoPtr<CAccountManagerService::DatabaseHelper> mOpenHelper;
    HashMap<Pair<Pair<AutoPtr<IAccount>, String>*, AutoPtr<IInteger32> >*, AutoPtr<IInteger32> >
            mCredentialsPermissionNotificationIds;
    Object mCredentialsPermissionNotificationIdsLock;
    HashMap<AutoPtr<IAccount>, AutoPtr<IInteger32> > mSigninRequiredNotificationIds;
    Object mSigninRequiredNotificationIdsLock;
    Object mCacheLock;
    /** protected by the {@link #cacheLock} */
    HashMap<String, AutoPtr< ArrayOf<IAccount*> > > mAccountCache;
    /** protected by the {@link #cacheLock} */
    HashMap<AutoPtr<IAccount>, AutoPtr< HashMap<String, String> > > mUserDataCache;
    /** protected by the {@link #cacheLock} */
    HashMap<AutoPtr<IAccount>, AutoPtr< HashMap<String, String> > > mAuthTokenCache;

    friend class AccountManagerServiceSession;
    friend class CAccountManagerService;
    friend class CGetAuthTokenSession;
    friend class CGetAccountsByTypeAndFeatureSession;
};


} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_ACCOUNTS_CACCOUNTMANAGERSERVICE_H__
