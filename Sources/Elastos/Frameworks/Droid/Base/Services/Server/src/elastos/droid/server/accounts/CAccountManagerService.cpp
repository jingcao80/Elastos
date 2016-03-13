
#include "accounts/CAccountManagerService.h"
#include "accounts/AccountAuthenticatorCache.h"
#include "accounts/CTestFeaturesSession.h"
#include "accounts/GrantCredentialsPermissionActivity.h"
#include "accounts/AccountManagerServiceSession.h"
#include "accounts/CRemoveAccountSession.h"
#include "accounts/CGetAccountsByTypeAndFeatureSession.h"
#include "accounts/CGetAuthTokenLabelSession.h"
#include "accounts/CGetAuthTokenSession.h"
#include "accounts/CAddAcountSession.h"
#include "accounts/CConfirmCredentialsAsUserSession.h"
#include "accounts/CUpdateCredentialsSession.h"
#include "accounts/CEditPropertiesSession.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::CInteger32;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Etl::HashSet;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::IPendingIntentHelper;
// using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IDatabaseUtils;
using Elastos::Droid::Database::CDatabaseUtils;
using Elastos::Droid::Database::Sqlite::EIID_ISQLiteOpenHelper;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Accounts::IAuthenticatorDescriptionHelper;
using Elastos::Droid::Accounts::CAuthenticatorDescriptionHelper;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::CAccount;
using Elastos::Droid::Accounts::CAccountAndUser;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

const String CAccountManagerService::TAG("AccountManagerService");
const Int32 CAccountManagerService::TIMEOUT_DELAY_MS = 1000 * 60;
const String CAccountManagerService::DATABASE_NAME("accounts.db");
const Int32 CAccountManagerService::DATABASE_VERSION = 4;
const Int32 CAccountManagerService::MESSAGE_TIMED_OUT = 3;
const String CAccountManagerService::TABLE_ACCOUNTS("accounts");
const String CAccountManagerService::ACCOUNTS_ID("_id");
const String CAccountManagerService::ACCOUNTS_NAME("name");
const String CAccountManagerService::ACCOUNTS_TYPE("type");
const String CAccountManagerService::ACCOUNTS_TYPE_COUNT("count(type)");
const String CAccountManagerService::ACCOUNTS_PASSWORD("password");
const String CAccountManagerService::TABLE_AUTHTOKENS("authtokens");
const String CAccountManagerService::AUTHTOKENS_ID("_id");
const String CAccountManagerService::AUTHTOKENS_ACCOUNTS_ID("accounts_id");
const String CAccountManagerService::AUTHTOKENS_TYPE("type");
const String CAccountManagerService::AUTHTOKENS_AUTHTOKEN("authtoken");
const String CAccountManagerService::TABLE_GRANTS("grants");
const String CAccountManagerService::GRANTS_ACCOUNTS_ID("accounts_id");
const String CAccountManagerService::GRANTS_AUTH_TOKEN_TYPE("auth_token_type");
const String CAccountManagerService::GRANTS_GRANTEE_UID("uid");
const String CAccountManagerService::TABLE_EXTRAS("extras");
const String CAccountManagerService::EXTRAS_ID("_id");
const String CAccountManagerService::EXTRAS_ACCOUNTS_ID("accounts_id");
const String CAccountManagerService::EXTRAS_KEY("key");
const String CAccountManagerService::EXTRAS_VALUE("value");
const String CAccountManagerService::TABLE_META("meta");
const String CAccountManagerService::META_KEY("key");
const String CAccountManagerService::META_VALUE("value");
const String CAccountManagerService::ACCOUNT_NAME_TYPE_PROJECTION[] =
        { ACCOUNTS_ID, ACCOUNTS_NAME, ACCOUNTS_TYPE };
const String CAccountManagerService::ACCOUNT_TYPE_COUNT_PROJECTION[] =
        { ACCOUNTS_TYPE, ACCOUNTS_TYPE_COUNT };

AutoPtr<IIntent> InitAccountsChangedIntent()
{
    AutoPtr<IIntent> i;
    ASSERT_SUCCEEDED(CIntent::New(IAccountManager::LOGIN_ACCOUNTS_CHANGED_ACTION,
            (IIntent**)&i));
    i->SetFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);

    return i;
}

const AutoPtr<IIntent> CAccountManagerService::ACCOUNTS_CHANGED_INTENT =
        InitAccountsChangedIntent();
const String CAccountManagerService::COUNT_OF_MATCHING_GRANTS = String("")
        + String("SELECT COUNT(*) FROM ") + TABLE_GRANTS + String(", ")
        + TABLE_ACCOUNTS + String(" WHERE ")
        + GRANTS_ACCOUNTS_ID + String("=") + ACCOUNTS_ID
        + String(" AND ") + GRANTS_GRANTEE_UID + String("=?")
        + String(" AND ") + GRANTS_AUTH_TOKEN_TYPE + String("=?")
        + String(" AND ") + ACCOUNTS_NAME + String("=?")
        + String(" AND ") + ACCOUNTS_TYPE + String("=?");
const String CAccountManagerService::SELECTION_AUTHTOKENS_BY_ACCOUNT =
        AUTHTOKENS_ACCOUNTS_ID + String("=(select _id FROM accounts WHERE name=? AND type=?)");
const String CAccountManagerService::COLUMNS_AUTHTOKENS_TYPE_AND_AUTHTOKEN[] =
        { AUTHTOKENS_TYPE, AUTHTOKENS_AUTHTOKEN };

const String CAccountManagerService::SELECTION_USERDATA_BY_ACCOUNT =
        EXTRAS_ACCOUNTS_ID + String("=(select _id FROM accounts WHERE name=? AND type=?)");
const String CAccountManagerService::COLUMNS_EXTRAS_KEY_AND_VALUE[] =
        { EXTRAS_KEY, EXTRAS_VALUE };
AutoPtr< ArrayOf<IAccount*> > CAccountManagerService::EMPTY_ACCOUNT_ARRAY =
        ArrayOf<IAccount*>::Alloc(0);
AutoPtr<CAccountManagerService> CAccountManagerService::sThis;

ECode CAccountManagerService::MessageHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case CAccountManagerService::MESSAGE_TIMED_OUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AccountManagerServiceSession* session =
                reinterpret_cast<AccountManagerServiceSession*>(obj.Get());
            if (session) {
                session->OnTimedOut();
            }
            break;
        }
        default:
            return E_ILLEGAL_STATE_EXCEPTION;
        }

    return NOERROR;
}

CAccountManagerService::DatabaseHelper::DatabaseHelper(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [in] */ const String& name)
{
    Init(context, name,
            NULL, CAccountManagerService::DATABASE_VERSION);
}

CAR_INTERFACE_IMPL(CAccountManagerService::DatabaseHelper, ISQLiteOpenHelper)

ECode CAccountManagerService::DatabaseHelper::GetDatabaseName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    return SQLiteOpenHelper::GetDatabaseName(name);
}

ECode CAccountManagerService::DatabaseHelper::SetWriteAheadLoggingEnabled(
    /* [in] */ Boolean enabled)
{
    return SQLiteOpenHelper::SetWriteAheadLoggingEnabled(enabled);
}

ECode CAccountManagerService::DatabaseHelper::GetWritableDatabase(
    /* [out] */ ISQLiteDatabase** database)
{
    VALIDATE_NOT_NULL(database);
    return SQLiteOpenHelper::GetWritableDatabase(database);
}

ECode CAccountManagerService::DatabaseHelper::GetReadableDatabase(
    /* [out] */ ISQLiteDatabase** database)
{
    VALIDATE_NOT_NULL(database);
    return SQLiteOpenHelper::GetReadableDatabase(database);
}

ECode CAccountManagerService::DatabaseHelper::Close()
{
    return SQLiteOpenHelper::Close();
}

ECode CAccountManagerService::DatabaseHelper::OnConfigure(
    /* [in] */ ISQLiteDatabase* db)
{
    return SQLiteOpenHelper::OnConfigure(db);
}

ECode CAccountManagerService::DatabaseHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("CREATE TABLE ") + CAccountManagerService::TABLE_ACCOUNTS
            + String(" ( ") + CAccountManagerService::ACCOUNTS_ID
            + String(" INTEGER PRIMARY KEY AUTOINCREMENT, ")
            + CAccountManagerService::ACCOUNTS_NAME + String(" TEXT NOT NULL, ")
            + CAccountManagerService::ACCOUNTS_TYPE + String(" TEXT NOT NULL, ")
            + CAccountManagerService::ACCOUNTS_PASSWORD + String(" TEXT, ")
            + String("UNIQUE(") + CAccountManagerService::ACCOUNTS_NAME
            + String(",") + CAccountManagerService::ACCOUNTS_TYPE + String("))"));

    db->ExecSQL(String("CREATE TABLE ") + CAccountManagerService::TABLE_AUTHTOKENS
            + String(" (  ") + CAccountManagerService::AUTHTOKENS_ID
            + String(" INTEGER PRIMARY KEY AUTOINCREMENT,  ")
            + CAccountManagerService::AUTHTOKENS_ACCOUNTS_ID + String(" INTEGER NOT NULL, ")
            + CAccountManagerService::AUTHTOKENS_TYPE + String(" TEXT NOT NULL,  ")
            + CAccountManagerService::AUTHTOKENS_AUTHTOKEN + String(" TEXT,  ")
            + String("UNIQUE (") + CAccountManagerService::AUTHTOKENS_ACCOUNTS_ID
            + String(",") + CAccountManagerService::AUTHTOKENS_TYPE + String("))"));

    CreateGrantsTable(db);

    db->ExecSQL(String("CREATE TABLE ") + CAccountManagerService::TABLE_EXTRAS
            + String(" ( ") + CAccountManagerService::EXTRAS_ID
            + String(" INTEGER PRIMARY KEY AUTOINCREMENT, ")
            + CAccountManagerService::EXTRAS_ACCOUNTS_ID + String(" INTEGER, ")
            + CAccountManagerService::EXTRAS_KEY + String(" TEXT NOT NULL, ")
            + CAccountManagerService::EXTRAS_VALUE + String(" TEXT, ")
            + String("UNIQUE(") + CAccountManagerService::EXTRAS_ACCOUNTS_ID
            + String(",") + CAccountManagerService::EXTRAS_KEY + String("))"));

    db->ExecSQL(String("CREATE TABLE ") + CAccountManagerService::TABLE_META
            + String(" ( ") + CAccountManagerService::META_KEY
            + String(" TEXT PRIMARY KEY NOT NULL, ") + CAccountManagerService::META_VALUE
            + String(" TEXT)"));

    CreateAccountsDeletionTrigger(db);

    return NOERROR;
}

void CAccountManagerService::DatabaseHelper::CreateAccountsDeletionTrigger(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("")
            + String(" CREATE TRIGGER ") + CAccountManagerService::TABLE_ACCOUNTS
            + String("Delete DELETE ON ") + CAccountManagerService::TABLE_ACCOUNTS
            + String(" BEGIN")
            + String("   DELETE FROM ") + CAccountManagerService::TABLE_AUTHTOKENS
            + String("     WHERE ") + CAccountManagerService::AUTHTOKENS_ACCOUNTS_ID
            + String("=OLD.") + CAccountManagerService::ACCOUNTS_ID + String(" ;")
            + String("   DELETE FROM ") + CAccountManagerService::TABLE_EXTRAS
            + String("     WHERE ") + CAccountManagerService::EXTRAS_ACCOUNTS_ID
            + String("=OLD.") + CAccountManagerService::ACCOUNTS_ID + String(" ;")
            + String("   DELETE FROM ") + CAccountManagerService::TABLE_GRANTS
            + String("     WHERE ") + CAccountManagerService::GRANTS_ACCOUNTS_ID
            + String("=OLD.") + CAccountManagerService::ACCOUNTS_ID + String(" ;")
            + String(" END"));
}

void CAccountManagerService::DatabaseHelper::CreateGrantsTable(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("CREATE TABLE ") + CAccountManagerService::TABLE_GRANTS
            + String(" (  ") + CAccountManagerService::GRANTS_ACCOUNTS_ID
            + String(" INTEGER NOT NULL, ") + CAccountManagerService::GRANTS_AUTH_TOKEN_TYPE
            + String(" STRING NOT NULL,  ") + CAccountManagerService::GRANTS_GRANTEE_UID
            + String(" INTEGER NOT NULL,  ") + String("UNIQUE (")
            + CAccountManagerService::GRANTS_ACCOUNTS_ID + String(",")
            + CAccountManagerService::GRANTS_AUTH_TOKEN_TYPE + String(",")
            + CAccountManagerService::GRANTS_GRANTEE_UID + String("))"));
}

ECode CAccountManagerService::DatabaseHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    Slogger::E(CAccountManagerService::TAG,
            "upgrade from version %d to version %d", oldVersion, newVersion);

    if (oldVersion == 1) {
        // no longer need to do anything since the work is done
        // when upgrading from version 2
        oldVersion++;
    }

    if (oldVersion == 2) {
        CreateGrantsTable(db);
        db->ExecSQL(String("DROP TRIGGER ") + CAccountManagerService::TABLE_ACCOUNTS
                + String("Delete"));
        CreateAccountsDeletionTrigger(db);
        oldVersion++;
    }

    if (oldVersion == 3) {
        db->ExecSQL(String("UPDATE ") + CAccountManagerService::TABLE_ACCOUNTS
                + String(" SET ") + CAccountManagerService::ACCOUNTS_TYPE
                + String(" = 'com.google' WHERE ")
                + CAccountManagerService::ACCOUNTS_TYPE + String(" == 'com.google.GAIA'"));
        oldVersion++;
    }

    return NOERROR;
}

ECode CAccountManagerService::DatabaseHelper::OnDowngrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    return SQLiteOpenHelper::OnDowngrade(db, oldVersion, newVersion);
}

ECode CAccountManagerService::DatabaseHelper::OnOpen(
    /* [in] */ ISQLiteDatabase* db)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) Log.v(TAG, "opened database " + DATABASE_NAME);
    return NOERROR;
}


UserAccounts::UserAccounts(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
    : mUserId(userId)
{
    AutoLock lock(mCacheLock);
    String name;
    CAccountManagerService::GetDatabaseName(userId, &name);
    mOpenHelper = new CAccountManagerService::DatabaseHelper(context, userId, name);
}

UserAccounts::~UserAccounts()
{
    Pair<Pair<AutoPtr<IAccount>, String>*, AutoPtr<IInteger32> >* aiPair;
    Pair<AutoPtr<IAccount>, String>* asPair;
    HashMap<Pair<Pair<AutoPtr<IAccount>, String>*, AutoPtr<IInteger32> >*, AutoPtr<IInteger32> >::Iterator ait;
    for (ait = mCredentialsPermissionNotificationIds.Begin(); ait != mCredentialsPermissionNotificationIds.End();) {
        aiPair = ait->mFirst;
        if (aiPair) {
            if (aiPair->mFirst) delete aiPair->mFirst;
            delete aiPair;
        }
    }
    mCredentialsPermissionNotificationIds.Clear();

    mUserDataCache.Clear();
    mAuthTokenCache.Clear();
}

CAccountManagerService::PurgeOldGrantsAllBroadcastReceiver::PurgeOldGrantsAllBroadcastReceiver(
    /* [in] */ CAccountManagerService* host)
    : mHost(host)
{}

ECode CAccountManagerService::PurgeOldGrantsAllBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->PurgeOldGrantsAll();
    return NOERROR;
}


CAccountManagerService::OnUserRemovedBroadcastReceiver::OnUserRemovedBroadcastReceiver(
    /* [in] */ CAccountManagerService* host)
    : mHost(host)
{}

ECode CAccountManagerService::OnUserRemovedBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->OnUserRemoved(intent);
    return NOERROR;
}


CAccountManagerService::CAccountManagerService()
{
    mSessions = new HashMap<String, AutoPtr<AccountManagerServiceSession> >();

    ASSERT_SUCCEEDED(CAtomicInteger32::New(1,
            (IAtomicInteger32**)&mNotificationIds));
}

AutoPtr<CAccountManagerService> CAccountManagerService::GetSingleton()
{
    return sThis;
}

ECode CAccountManagerService::constructor(
    /* [in] */ IContext* context)
{
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IAccountAuthenticatorCache> cache = (IAccountAuthenticatorCache*)new AccountAuthenticatorCache(context);
    return constructor(context, pm, (Handle32)cache.Get());
}

ECode CAccountManagerService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ Handle32 authenticatorCache)
{
    mContext = context;
    mPackageManager = packageManager;

    FAIL_RETURN(CHandlerThread::New(String("AccountManagerService"),
            (IHandlerThread**)&mMessageThread));
    mMessageThread->Start();
    AutoPtr<ILooper> looper;
    mMessageThread->GetLooper((ILooper**)&looper);
    mMessageHandler = new MessageHandler(this);

    mAuthenticatorCache = (IAccountAuthenticatorCache*)authenticatorCache;
    mAuthenticatorCache->SetListener((IRegisteredServicesCacheListener*)this,
            NULL /* Handler */);

    sThis = this;

    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    intentFilter->AddDataScheme(String("package"));
    AutoPtr<IIntent> stickyIntent;
    AutoPtr<IBroadcastReceiver> br1= (IBroadcastReceiver*)new PurgeOldGrantsAllBroadcastReceiver(this);
    mContext->RegisterReceiver(br1, intentFilter, (IIntent**)&stickyIntent);

    AutoPtr<IIntentFilter> userFilter;
    CIntentFilter::New((IIntentFilter**)&userFilter);
    userFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    AutoPtr<IIntent> intent;
    AutoPtr<IBroadcastReceiver> br2= (IBroadcastReceiver*)new OnUserRemovedBroadcastReceiver(this);
    mContext->RegisterReceiver(br2, userFilter, (IIntent**)&intent);

    return NOERROR;
}

void CAccountManagerService::SystemReady()
{}

AutoPtr<IUserManager> CAccountManagerService::GetUserManager()
{
    if (mUserManager == NULL) {
        AutoPtr<IInterface> obj;
        ASSERT_SUCCEEDED(mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj));
        mUserManager = IUserManager::Probe(obj);
    }
    return mUserManager;
}

AutoPtr<UserAccounts> CAccountManagerService::InitUser(
    /* [in] */ Int32 userId)
{
    AutoLock lock(mUsersLock);
    AutoPtr<UserAccounts> accounts;
    HashMap<Int32, AutoPtr<UserAccounts> >::Iterator it = mUsers.Find(userId);
    if (it != mUsers.End()) {
        accounts = it->mSecond;
    }
    if (accounts == NULL) {
        accounts = new UserAccounts(mContext, userId);
        mUsers[userId] = accounts;
        PurgeOldGrants(accounts);
        ValidateAccountsInternal(accounts, TRUE /* invalidateAuthenticatorCache */);
    }
    return accounts;
}

void CAccountManagerService::PurgeOldGrantsAll()
{
    AutoLock lock(mUsersLock);
    HashMap<Int32, AutoPtr<UserAccounts> >::Iterator it;
    for (it = mUsers.Begin(); it != mUsers.End(); ++it) {
        PurgeOldGrants(it->mSecond);
    }
}

void CAccountManagerService::PurgeOldGrants (
    /* [in] */ UserAccounts* accounts)
{
    AutoLock lock(accounts->mCacheLock);
    AutoPtr<ISQLiteDatabase> db;
    ASSERT_SUCCEEDED(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db));
    AutoPtr<ICursor> cursor;
    AutoPtr< ArrayOf<String> > ids = ArrayOf<String>::Alloc(1);
    (*ids)[0] = GRANTS_GRANTEE_UID;
    String nullStr;
    ASSERT_SUCCEEDED(db->Query(TABLE_GRANTS, ids, nullStr, NULL,
            GRANTS_GRANTEE_UID, nullStr, nullStr, (ICursor**)&cursor));
    // try {
    Boolean result;
    while (cursor->MoveToNext(&result), result) {
        Int32 uid;
        cursor->GetInt32(0, &uid);
        AutoPtr< ArrayOf<String> > pkgs;
        mPackageManager->GetPackagesForUid(uid, (ArrayOf<String>**)&pkgs);
        Boolean packageExists = pkgs != NULL;
        if (packageExists) {
            continue;
        }
        Slogger::D(TAG, "deleting grants for UID %d because its package is no longer installed", uid);
        (*ids)[0] = StringUtils::ToString(uid);
        Int32 res;
        db->Delete(TABLE_GRANTS, GRANTS_GRANTEE_UID + String("=?"), ids, &res);
    }
    cursor->Close();
    // } finally {
    //     cursor.close();
    // }
}

void CAccountManagerService::ValidateAccounts(
    /* [in] */ Int32 userId)
{
    AutoPtr<UserAccounts> accounts = GetUserAccounts(userId);

    // Invalidate user-specific cache to make sure we catch any
    // removed authenticators.
    ValidateAccountsInternal(accounts, TRUE /* invalidateAuthenticatorCache */);
}

void CAccountManagerService::ValidateAccountsInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ Boolean invalidateAuthenticatorCache)
{
    if (invalidateAuthenticatorCache) {
        mAuthenticatorCache->InvalidateCache(accounts->mUserId);
    }

    HashSet< AutoPtr<IAuthenticatorDescription> > knownAuth;
    AutoPtr< List<AutoPtr<IRegisteredServicesCacheServiceInfo> > > services = mAuthenticatorCache->GetAllServices(accounts->mUserId);
    List<AutoPtr<IRegisteredServicesCacheServiceInfo> >::Iterator infoIt = services->Begin();
    for (; infoIt != services->End(); ++infoIt) {
        AutoPtr<IAuthenticatorDescription> type = (IAuthenticatorDescription*)(*infoIt)->mType.Get();
        knownAuth.Insert(type);
    }

    {
        AutoLock lock(accounts->mCacheLock);
        AutoPtr<ISQLiteDatabase> db;
        ASSERT_SUCCEEDED(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db));
        Boolean accountDeleted = FALSE;
        AutoPtr<ICursor> cursor;
        AutoPtr< ArrayOf<String> > accountsInfo = ArrayOf<String>::Alloc(3);
        (*accountsInfo)[0] = ACCOUNTS_ID;
        (*accountsInfo)[1] = ACCOUNTS_TYPE;
        (*accountsInfo)[2] = ACCOUNTS_NAME;
        String nullStr;
        db->Query(TABLE_ACCOUNTS, accountsInfo, nullStr, NULL,
                nullStr, nullStr, nullStr, (ICursor**)&cursor);
        // try {
        accounts->mAccountCache.Clear();
        HashMap<String, AutoPtr<List<String> > > accountNamesByType;
        Boolean result = FALSE;
        while (cursor->MoveToNext(&result), result) {
            Int64 accountId;
            cursor->GetInt64(0, &accountId);
            String accountType, accountName;
            cursor->GetString(1, &accountType);
            cursor->GetString(2, &accountName);

            AutoPtr<IAuthenticatorDescription> value;
            AutoPtr<IAuthenticatorDescriptionHelper> descHelper;
            ASSERT_SUCCEEDED(CAuthenticatorDescriptionHelper::AcquireSingleton(
                    (IAuthenticatorDescriptionHelper**)&descHelper));
            descHelper->NewKey(accountType, (IAuthenticatorDescription**)&value);
            HashSet< AutoPtr<IAuthenticatorDescription> >::Iterator it =
                    knownAuth.Find(value);
            if (it == knownAuth.End()) {
                Slogger::W(TAG, "deleting account %s because type %s no longer has a registered authenticator",
                        (const char*)accountName, (const char*)accountType);
                Int32 result;
                db->Delete(TABLE_ACCOUNTS,
                        ACCOUNTS_ID + String("=") + StringUtils::Int64ToString(accountId),
                        NULL, &result);
                accountDeleted = TRUE;
                AutoPtr<IAccount> account;
                ASSERT_SUCCEEDED(CAccount::New(accountName, accountType, (IAccount**)&account));
                accounts->mUserDataCache.Erase(account);
                accounts->mAuthTokenCache.Erase(account);
            }
            else {
                AutoPtr< List<String> > accountNames ;
                HashMap<String, AutoPtr<List<String> > >::Iterator it = accountNamesByType.Find(accountType);
                if (it != accountNamesByType.End()) {
                    accountNames = it->mSecond;
                }
                if (accountNames == NULL) {
                    accountNames = new List<String>();
                    accountNamesByType[accountType] = accountNames;
                }
                accountNames->PushBack(accountName);
            }
        }
        HashMap<String, AutoPtr<List<String> > >::Iterator it;
        for (it = accountNamesByType.Begin(); it != accountNamesByType.End(); ++it) {
            String accountType = it->mFirst;
            AutoPtr< List<String> > accountNames = it->mSecond;
            AutoPtr< ArrayOf<IAccount*> > accountsForType =
                    ArrayOf<IAccount*>::Alloc(accountNames->GetSize());
            List<String>::Iterator accountNameIt = accountNames->Begin();
            Int32 i = 0;
            for (; accountNameIt != accountNames->End(); ++accountNameIt, ++i) {
                String accountName = *accountNameIt;
                AutoPtr<IAccount> a;
                CAccount::New(accountName, accountType, (IAccount**)&a);
                accountsForType->Set(i, a);
            }
            accounts->mAccountCache[accountType] = accountsForType;

        }
        // } finally {
        cursor->Close();
        if (accountDeleted) {
            SendAccountsChangedBroadcast(accounts->mUserId);
        }
        // }
    }
}

AutoPtr<UserAccounts> CAccountManagerService::GetUserAccountsForCaller()
{
    return GetUserAccounts(UserHandle::GetCallingUserId());
}

AutoPtr<UserAccounts> CAccountManagerService::GetUserAccounts(
    /* [in] */ Int32 userId)
{
    AutoLock lock(mUsersLock);
    AutoPtr<UserAccounts> accounts;
    HashMap<Int32, AutoPtr<UserAccounts> >::Iterator it = mUsers.Find(userId);
    if (it != mUsers.End()) {
        accounts = it->mSecond;
    }
    if (accounts == NULL) {
        accounts = InitUser(userId);
        mUsers[userId] = accounts;
    }
    return accounts;
}

void CAccountManagerService::OnUserRemoved(
    /* [in] */ IIntent* intent)
{
    Int32 userId;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &userId);
    if (userId < 1) return;

    AutoPtr<UserAccounts> accounts;
    {
        AutoLock lock(mUsersLock);
        HashMap<Int32, AutoPtr<UserAccounts> >::Iterator it = mUsers.Find(userId);
        if (it != mUsers.End()) {
            accounts = it->mSecond;
            mUsers.Erase(it);
        }
    }

    if (accounts == NULL) {
        AutoPtr<IFile> dbFile;
        String s;
        ASSERT_SUCCEEDED(GetDatabaseName(userId, &s));
        ASSERT_SUCCEEDED(CFile::New(s, (IFile**)&dbFile));
        Boolean result;
        dbFile->Delete(&result);
        return;
    }

    AutoLock lock(accounts->mCacheLock);
    accounts->mOpenHelper->Close();
    AutoPtr<IFile> dbFile;
    String s;
    ASSERT_SUCCEEDED(GetDatabaseName(userId, &s));
    ASSERT_SUCCEEDED(CFile::New(s, (IFile**)&dbFile));
    Boolean result;
    dbFile->Delete(&result);
}

ECode CAccountManagerService::OnServiceChanged(
    /* [in] */ IInterface* type,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean removed)
{
    Slogger::D(TAG, "onServiceChanged() for userId %d", userId);
    ValidateAccountsInternal(GetUserAccounts(userId),
            FALSE /* invalidateAuthenticatorCache */);

    return NOERROR;
}

ECode CAccountManagerService::GetPassword(
    /* [in] */ IAccount* account,
    /* [out] */ String* password)
{
    VALIDATE_NOT_NULL(password);
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "getPassword: " + account
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    // FAIL_RETURN(CheckAuthenticateAccountsPermission(account));

    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    *password = ReadPasswordInternal(accounts, account);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
    return NOERROR;
}

String CAccountManagerService::ReadPasswordInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account)
{
    if (account == NULL) {
        return String(NULL);
    }

    AutoLock lock(accounts->mCacheLock);
    AutoPtr<ISQLiteDatabase> db;
    ASSERT_SUCCEEDED(accounts->mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db));
    AutoPtr<ICursor> cursor;
    AutoPtr< ArrayOf<String> > accounts1 = ArrayOf<String>::Alloc(1);
    (*accounts1)[0] = ACCOUNTS_PASSWORD;
    AutoPtr< ArrayOf<String> > accounts2 = ArrayOf<String>::Alloc(2);
    account->GetName(&(*accounts2)[0]);
    account->GetType(&(*accounts2)[1]);
    String nullStr;
    ASSERT_SUCCEEDED(db->Query(TABLE_ACCOUNTS, accounts1,
            ACCOUNTS_NAME + String("=? AND ") + ACCOUNTS_TYPE+ String("=?"),
            accounts2, nullStr, nullStr, nullStr, (ICursor**)&cursor));
    // try {
    Boolean result;
    if (cursor->MoveToNext(&result), result) {
        String s;
        cursor->GetString(0, &s);
        cursor->Close();
        return s;
    }

    cursor->Close();
    return String(NULL);
    // } finally {
    //     cursor.close();
    // }
}

ECode CAccountManagerService::GetUserData(
    /* [in] */ IAccount* account,
    /* [in] */ const String& key,
    /* [out] */ String* userData)
{
    VALIDATE_NOT_NULL(userData);
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "getUserData: " + account
    //             + ", key " + key
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    if (key.IsNull()) {
        Slogger::E(TAG, "key is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("key is null");
    }
    FAIL_RETURN(CheckAuthenticateAccountsPermission(account));
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    *userData = ReadUserDataInternal(accounts, account, key);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
    return NOERROR;
}

ECode CAccountManagerService::GetAuthenticatorTypes(
    /* [out, callee] */ ArrayOf<IAuthenticatorDescription*>** _types)
{
    VALIDATE_NOT_NULL(_types);
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "getAuthenticatorTypes: "
    //             + "caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    // Collection<AccountAuthenticatorCache.ServiceInfo<AuthenticatorDescription>>
    //         authenticatorCollection = mAuthenticatorCache.getAllServices(userId);
    AutoPtr< List<AutoPtr<IRegisteredServicesCacheServiceInfo> > > services = mAuthenticatorCache->GetAllServices(
            UserHandle::GetCallingUserId());
    Int32 count = services->GetSize();
    AutoPtr< ArrayOf<IAuthenticatorDescription*> > types =
            ArrayOf<IAuthenticatorDescription*>::Alloc(count);
    List<AutoPtr<IRegisteredServicesCacheServiceInfo> >::Iterator it = services->Begin();
    for(Int32 i = 0; it != services->End(); ++i, ++it) {
        types->Set(i, (IAuthenticatorDescription*)(*it)->mType.Get());
    }

    *_types = types;
    REFCOUNT_ADD(*_types);
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CAccountManagerService::AddAccount(
    /* [in] */ IAccount* account,
    /* [in] */ const String& password,
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "addAccount: " + account
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    // FAIL_RETURN(CheckAuthenticateAccountsPermission(account));

    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    // fails if the account already exists
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    *result = AddAccountInternal(accounts, account, password, extras);
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // } finally {
    //     restoreCallingIdentity(identityToken);
    // }
}

Boolean CAccountManagerService::AddAccountInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ const String& password,
    /* [in] */ IBundle* extras)
{
    if (account == NULL) {
        return FALSE;
    }

    AutoLock lock(accounts->mCacheLock);
    AutoPtr<ISQLiteDatabase> db;
    ASSERT_SUCCEEDED(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db));
    db->BeginTransaction();
    // try {
    AutoPtr<IDatabaseUtils> helper;
    ASSERT_SUCCEEDED(CDatabaseUtils::AcquireSingleton((IDatabaseUtils**)&helper));
    Int64 numMatches;
    AutoPtr< ArrayOf<String> > accountInfo = ArrayOf<String>::Alloc(2);
    account->GetName(&(*accountInfo)[0]);
    account->GetType(&(*accountInfo)[1]);
    helper->Int64ForQuery(db,
            String("select count(*) from ") + TABLE_ACCOUNTS
                    + String(" WHERE ") + ACCOUNTS_NAME + String("=? AND ")
                    + ACCOUNTS_TYPE+ String("=?"),
            accountInfo, &numMatches);
    if (numMatches > 0) {
        Slogger::W(TAG, "insertAccountIntoDatabase: %p, skipping since the account already exists",
            account);
        db->EndTransaction();
        return FALSE;
    }
    AutoPtr<IContentValues> values;
    ASSERT_SUCCEEDED(CContentValues::New((IContentValues**)&values));
    AutoPtr<ICharSequence> cs;
    CString::New((*accountInfo)[0], (ICharSequence**)&cs);
    values->PutString(ACCOUNTS_NAME, cs);
    cs = NULL;
    CString::New((*accountInfo)[1], (ICharSequence**)&cs);
    values->PutString(ACCOUNTS_TYPE, cs);
    cs = NULL;
    CString::New(password, (ICharSequence**)&cs);
    values->PutString(ACCOUNTS_PASSWORD, cs);
    Int64 accountId;
    db->Insert(TABLE_ACCOUNTS, ACCOUNTS_NAME, values, &accountId);
    if (accountId < 0) {
        Slogger::W(TAG, "insertAccountIntoDatabase: %p, skipping the DB insert failed", account);
        db->EndTransaction();
        return FALSE;
    }
    if (extras != NULL) {
        AutoPtr<IObjectContainer> keys;
        extras->KeySet((IObjectContainer**)&keys);
        AutoPtr<IObjectEnumerator> enumerator;
        keys->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        Boolean hasNext;
        while(enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<ICharSequence> keyCS;
            enumerator->Current((IInterface**)&keyCS);
            String key;
            keyCS->ToString(&key);
            String value;
            extras->GetString(key, &value);
            if (InsertExtraLocked(db, accountId, key, value) < 0) {
                Slogger::W(TAG, "insertAccountIntoDatabase: %p, skipping since insertExtra failed for key %s",
                        account, (const char*)key);
                db->EndTransaction();
                return FALSE;
            }
        }
    }
    db->SetTransactionSuccessful();
    InsertAccountIntoCacheLocked(accounts, account);
    // } finally {
    db->EndTransaction();
    // }
    SendAccountsChangedBroadcast(accounts->mUserId);
    return TRUE;
}

Int64 CAccountManagerService::InsertExtraLocked(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int64 accountId,
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr<IContentValues> values;
    ASSERT_SUCCEEDED(CContentValues::New((IContentValues**)&values));
    AutoPtr<ICharSequence> csq;
    CString::New(key, (ICharSequence**)&csq);
    values->PutString(EXTRAS_KEY, csq);
    AutoPtr<IInteger64> id;
    CInteger64::New(accountId, (IInteger64**)&id);
    values->PutInt64(EXTRAS_ACCOUNTS_ID, id);
    csq = NULL;
    CString::New(value, (ICharSequence**)&csq);
    values->PutString(EXTRAS_VALUE, csq);
    Int64 result;
    db->Insert(TABLE_EXTRAS, EXTRAS_KEY, values, &result);
    return result;
}

ECode CAccountManagerService::HasFeatures(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ const ArrayOf<String>& features)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "hasFeatures: " + account
    //             + ", response " + response
    //             + ", features " + stringArrayToString(features)
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("response is null");
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    // if (features == NULL) {
    //     Slogger::E(TAG, "features is null");
    //     return E_ILLEGAL_ARGUMENT_EXCEPTION;
    //     // throw new IllegalArgumentException("features is null");
    // }
    FAIL_RETURN(CheckReadAccountsPermission());
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<CTestFeaturesSession> session;
    ECode ec = CTestFeaturesSession::NewByFriend((Handle32)accounts.Get(),
            response, account, features, (IIAccountManager*)this,
            (CTestFeaturesSession**)&session);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(identityToken);
        return ec;
    }
    session->Bind();
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }

    return NOERROR;
}

ECode CAccountManagerService::RemoveAccount(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "removeAccount: " + account
    //             + ", response " + response
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("response is null");
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    FAIL_RETURN(CheckManageAccountsPermission());
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    AutoPtr<IUserHandle> user;
    binderHelper->GetCallingUserHandle((IUserHandle**)&user);
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();

    CancelNotification(GetSigninRequiredNotificationId(accounts, account), user);
    {
        AutoLock lock(accounts->mCredentialsPermissionNotificationIdsLock);
        HashMap<Pair<Pair<AutoPtr<IAccount>, String>*, AutoPtr<IInteger32> >*, AutoPtr<IInteger32> >::Iterator it
                = accounts->mCredentialsPermissionNotificationIds.Begin();
        for (; it != accounts->mCredentialsPermissionNotificationIds.End(); ++it) {
            Pair<Pair<AutoPtr<IAccount>, String>*, AutoPtr<IInteger32> >* pair
                    = it->mFirst;
            if (account == pair->mFirst->mFirst) {
                AutoPtr<IInteger32> id = it->mSecond;
                Int32 value;
                id->GetValue(&value);
                CancelNotification(value, user);
            }
        }
    }

    // try {
    AutoPtr<CRemoveAccountSession> session;
    ECode ec = CRemoveAccountSession::NewByFriend((Handle32)accounts.Get(), response, account,
            (IIAccountManager*)this, (CRemoveAccountSession**)&session);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(identityToken);
        return ec;
    }
    session->Bind();
    Binder::RestoreCallingIdentity(identityToken);
    // } finally {
    //     restoreCallingIdentity(identityToken);
    // }
    return NOERROR;
}

void CAccountManagerService::RemoveAccountInternal(
    /* [in] */ IAccount* account)
{
    RemoveAccountInternal(GetUserAccountsForCaller(), account);
}

void CAccountManagerService::RemoveAccountInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account)
{
    AutoLock lock(accounts->mCacheLock);
    AutoPtr<ISQLiteDatabase> db;
    ASSERT_SUCCEEDED(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db));
    AutoPtr< ArrayOf<String> > accountInfo = ArrayOf<String>::Alloc(2);
    account->GetName(&(*accountInfo)[0]);
    account->GetType(&(*accountInfo)[1]);
    Int32 result;
    db->Delete(TABLE_ACCOUNTS, ACCOUNTS_NAME + String("=? AND ")
            + ACCOUNTS_TYPE+ String("=?"), accountInfo, &result);
    RemoveAccountFromCacheLocked(accounts, account);
    SendAccountsChangedBroadcast(accounts->mUserId);
}

ECode CAccountManagerService::InvalidateAuthToken(
    /* [in] */ const String& accountType,
    /* [in] */ const String& authToken)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "invalidateAuthToken: accountType " + accountType
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("accountType is null");
    }
    if (authToken.IsNull()) {
        Slogger::E(TAG, "authToken is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("authToken is null");
    }
    FAIL_RETURN(CheckManageAccountsOrUseCredentialsPermissions());
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    {
        AutoLock lock(accounts->mCacheLock);
        AutoPtr<ISQLiteDatabase> db;
        ASSERT_SUCCEEDED(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db));
        db->BeginTransaction();
        // try {
        InvalidateAuthTokenLocked(accounts, db, accountType, authToken);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        // }
    }
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
    return NOERROR;
}

void CAccountManagerService::InvalidateAuthTokenLocked(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& accountType,
    /* [in] */ const String& authToken)
{
    if (authToken.IsNull() || accountType.IsNull()) {
        return;
    }
    AutoPtr<ICursor> cursor;
    AutoPtr< ArrayOf<String> > params = ArrayOf<String>::Alloc(2);
    (*params)[0] = authToken;
    (*params)[1] = accountType;
    ASSERT_SUCCEEDED(db->RawQuery(
            String("SELECT ") + TABLE_AUTHTOKENS + String(".") + AUTHTOKENS_ID
                    + String(", ") + TABLE_ACCOUNTS + String(".") + ACCOUNTS_NAME
                    + String(", ") + TABLE_AUTHTOKENS + String(".") + AUTHTOKENS_TYPE
                    + String(" FROM ") + TABLE_ACCOUNTS
                    + String(" JOIN ") + TABLE_AUTHTOKENS
                    + String(" ON ") + TABLE_ACCOUNTS + String(".") + ACCOUNTS_ID
                    + String(" = ") + AUTHTOKENS_ACCOUNTS_ID
                    + String(" WHERE ") + AUTHTOKENS_AUTHTOKEN + String(" = ? AND ")
                    + TABLE_ACCOUNTS + String(".") + ACCOUNTS_TYPE + String(" = ?"),
            params, (ICursor**)&cursor));
    // try {
    Boolean result;
    while (cursor->MoveToNext(&result), result) {
        Int64 authTokenId;
        cursor->GetInt64(0, &authTokenId);
        String accountName, authTokenType;
        cursor->GetString(1, &accountName);
        cursor->GetString(2, &authTokenType);
        Int32 result;
        db->Delete(TABLE_AUTHTOKENS,
                AUTHTOKENS_ID + String("=") + StringUtils::Int64ToString(authTokenId),
                NULL, &result);
        AutoPtr<IAccount> account;
        CAccount::New(accountName, accountType, (IAccount**)&account);
        WriteAuthTokenIntoCacheLocked(accounts, db, account,
                authTokenType, String(NULL));
    }
    // } finally {
    cursor->Close();
    // }
}

Boolean CAccountManagerService::SaveAuthTokenToDatabase(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ const String& type,
    /* [in] */ const String& authToken)
{
    if (account == NULL || type.IsNull()) {
        return FALSE;
    }
    AutoPtr<IUserHandle> userH;
    CUserHandle::New(accounts->mUserId, (IUserHandle**)&userH);
    CancelNotification(GetSigninRequiredNotificationId(accounts, account), userH);

    AutoLock lock(accounts->mCacheLock);
    AutoPtr<ISQLiteDatabase> db;
    ASSERT_SUCCEEDED(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db));
    db->BeginTransaction();
    // try {
    Int64 accountId = GetAccountIdLocked(db, account);
    if (accountId < 0) {
        db->EndTransaction();
        return FALSE;
    }
    AutoPtr< ArrayOf<String> > ss = ArrayOf<String>::Alloc(1);
    (*ss)[0] = type;
    Int32 res;
    if (FAILED(db->Delete(TABLE_AUTHTOKENS,
            AUTHTOKENS_ACCOUNTS_ID + String("=") + StringUtils::Int64ToString(accountId)
            + String(" AND ") + AUTHTOKENS_TYPE + String("=?"), ss, &res))) {
        db->EndTransaction();
        return FALSE;
    }
    AutoPtr<IContentValues> values;
    if (FAILED(CContentValues::New((IContentValues**)&values))) {
        db->EndTransaction();
        return FALSE;
    }
    AutoPtr<IInteger32> aId;
    CInteger32::New(accountId, (IInteger32**)&aId);
    values->PutInt32(AUTHTOKENS_ACCOUNTS_ID, aId);
    AutoPtr<ICharSequence> cs;
    CString::New(type, (ICharSequence**)&cs);
    values->PutString(AUTHTOKENS_TYPE, cs);
    cs = NULL;
    CString::New(authToken, (ICharSequence**)&cs);
    values->PutString(AUTHTOKENS_AUTHTOKEN, cs);
    Int64 result;
    if (FAILED(db->Insert(TABLE_AUTHTOKENS, AUTHTOKENS_AUTHTOKEN, values, &result))) {
        db->EndTransaction();
        return TRUE;
    }
    if (result >= 0) {
        db->SetTransactionSuccessful();
        WriteAuthTokenIntoCacheLocked(accounts, db, account, type, authToken);
        db->EndTransaction();
        return TRUE;
    }

    db->EndTransaction();
    return FALSE;
    // } finally {
    //     db.endTransaction();
    // }
}

ECode CAccountManagerService::PeekAuthToken(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [out] */ String* token)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "peekAuthToken: " + account
    //             + ", authTokenType " + authTokenType
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("authTokenType is null");
    }
    FAIL_RETURN(CheckAuthenticateAccountsPermission(account));
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ReadAuthTokenInternal(accounts, account, authTokenType);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
    return NOERROR;
}

ECode CAccountManagerService::SetAuthToken(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ const String& authToken)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "setAuthToken: " + account
    //             + ", authTokenType " + authTokenType
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("authTokenType is null");
    }
    FAIL_RETURN(CheckAuthenticateAccountsPermission(account));
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    SaveAuthTokenToDatabase(accounts, account, authTokenType, authToken);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
    return NOERROR;
}

ECode CAccountManagerService::SetPassword(
    /* [in] */ IAccount* account,
    /* [in] */ const String& password)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "setAuthToken: " + account
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    FAIL_RETURN(CheckAuthenticateAccountsPermission(account));
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    SetPasswordInternal(accounts, account, password);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
    return NOERROR;
}

void CAccountManagerService::SetPasswordInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ const String& password)
{
    if (account == NULL) {
        return;
    }

    AutoLock lock(accounts->mCacheLock);
    AutoPtr<ISQLiteDatabase> db;
    ASSERT_SUCCEEDED(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db));
    db->BeginTransaction();
    // try {
    AutoPtr<IContentValues> values;
    if (FAILED(CContentValues::New((IContentValues**)&values))) {
        db->EndTransaction();
        return;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(password, (ICharSequence**)&cs);
    values->PutString(ACCOUNTS_PASSWORD, cs);
    Int64 accountId = GetAccountIdLocked(db, account);
    if (accountId >= 0) {
        AutoPtr< ArrayOf<String> > argsAccountId = ArrayOf<String>::Alloc(1);
        (*argsAccountId)[0] = StringUtils::ToString(accountId);
        Int32 result;
        if (FAILED(db->Update(TABLE_ACCOUNTS, values, ACCOUNTS_ID + String("=?"),
                argsAccountId, &result))) {
            db->EndTransaction();
            return;
        }
        if (FAILED(db->Delete(TABLE_AUTHTOKENS, AUTHTOKENS_ACCOUNTS_ID + String("=?"),
                argsAccountId, &result))) {
            db->EndTransaction();
            return;
        }
        accounts->mAuthTokenCache.Erase(account);
        db->SetTransactionSuccessful();
    }
    // } finally {
    db->EndTransaction();
    // }
    SendAccountsChangedBroadcast(accounts->mUserId);
}

void CAccountManagerService::SendAccountsChangedBroadcast(
    /* [in] */ Int32 userId)
{
    String action;
    ACCOUNTS_CHANGED_INTENT->GetAction(&action);
    Slogger::I(TAG, "the accounts changed, sending broadcast of %s", (const char*)action);
    AutoPtr<IUserHandle> userH;
    ASSERT_SUCCEEDED(CUserHandle::New(userId, (IUserHandle**)&userH));
    mContext->SendBroadcastAsUser(ACCOUNTS_CHANGED_INTENT, userH);
}

ECode CAccountManagerService::ClearPassword(
    /* [in] */ IAccount* account)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "clearPassword: " + account
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    FAIL_RETURN(CheckManageAccountsPermission());
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    SetPasswordInternal(accounts, account, String(NULL));
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
    return NOERROR;
}

ECode CAccountManagerService::SetUserData(
    /* [in] */ IAccount* account,
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "setUserData: " + account
    //             + ", key " + key
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (key.IsNull()) {
        Slogger::E(TAG, "key is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("key is null");
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    FAIL_RETURN(CheckAuthenticateAccountsPermission(account));
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    SetUserdataInternal(accounts, account, key, value);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
    return NOERROR;
}

void CAccountManagerService::SetUserdataInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    if (account == NULL || key.IsNull()) {
        return;
    }

    AutoLock lock(accounts->mCacheLock);
    AutoPtr<ISQLiteDatabase> db;
    ASSERT_SUCCEEDED(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db));
    db->BeginTransaction();
    // try {
    Int64 accountId = GetAccountIdLocked(db, account);
    if (accountId < 0) {
        db->EndTransaction();
        return;
    }
    Int64 extrasId = GetExtrasIdLocked(db, accountId, key);
    if (extrasId < 0 ) {
        extrasId = InsertExtraLocked(db, accountId, key, value);
        if (extrasId < 0) {
            db->EndTransaction();
            return;
        }
    }
    else {
        AutoPtr<IContentValues> values;
        CContentValues::New((IContentValues**)&values);
        AutoPtr<ICharSequence> cs;
        CString::New(value, (ICharSequence**)&cs);
        values->PutString(EXTRAS_VALUE, cs);
        Int32 result;
        if (FAILED(db->Update(TABLE_EXTRAS, values, EXTRAS_ID + String("=")
                + StringUtils::Int64ToString(extrasId), NULL, &result))) {
            db->EndTransaction();
            return;
        }
        if (1 != result) {
            db->EndTransaction();
            return;
        }

    }
    WriteUserDataIntoCacheLocked(accounts, db, account, key, value);
    db->SetTransactionSuccessful();
    // } finally {
    db->EndTransaction();
    // }
}

void CAccountManagerService::OnResult(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IBundle* result)
{
    if (result == NULL) {
        Slogger::E(TAG, "the result is unexpectedly null");
    }
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, getClass().getSimpleName() + " calling onResult() on response "
    //             + response);
    // }
    // try {
    if (FAILED(response->OnResult(result))) {
        // if the caller is dead then there is no one to care about remote
        // exceptions
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, "failure while notifying response", e);
        // }
    }
    // } catch (RemoteException e) {
    //     // if the caller is dead then there is no one to care about remote
    //     // exceptions
    //     if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //         Log.v(TAG, "failure while notifying response", e);
    //     }
    // }
}

ECode CAccountManagerService::GetAuthTokenLabel(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& accountType,
    /* [in] */ const String& authTokenType)
{
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("accountType is null");
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("authTokenType is null");
    }

    Int32 callingUid = Binder::GetCallingUid();
    Binder::ClearCallingIdentity();
    if (callingUid != IProcess::SYSTEM_UID) {
        Slogger::E(TAG, "can only call from system");
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("can only call from system");
    }
    AutoPtr<UserAccounts> accounts = GetUserAccounts(UserHandle::GetUserId(callingUid));
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<CGetAuthTokenLabelSession> session;
    ECode ec = CGetAuthTokenLabelSession::NewByFriend((Handle32)accounts.Get(), response, accountType,
            authTokenType, (IIAccountManager*)this, (CGetAuthTokenLabelSession**)&session);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(identityToken);
        return ec;
    }
    session->Bind();
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // } finally {
    //     restoreCallingIdentity(identityToken);
    // }
}

ECode CAccountManagerService::GetAuthToken(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Boolean notifyOnAuthFailure,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IBundle* options)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "getAuthToken: " + account
    //             + ", response " + response
    //             + ", authTokenType " + authTokenType
    //             + ", notifyOnAuthFailure " + notifyOnAuthFailure
    //             + ", expectActivityLaunch " + expectActivityLaunch
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("response is null");
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("authTokenType is null");
    }
    AutoPtr< ArrayOf<String> > perm = ArrayOf<String>::Alloc(1);
    (*perm)[0] = Elastos::Droid::Manifest::permission::USE_CREDENTIALS;
    FAIL_RETURN(CheckBinderPermission(perm));
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();

    AutoPtr<IAuthenticatorDescription> value;
    String accountType;
    account->GetType(&accountType);
    AutoPtr<IAuthenticatorDescriptionHelper> descHelper;
    ASSERT_SUCCEEDED(CAuthenticatorDescriptionHelper::AcquireSingleton(
            (IAuthenticatorDescriptionHelper**)&descHelper));
    descHelper->NewKey(accountType, (IAuthenticatorDescription**)&value);
    AutoPtr<IRegisteredServicesCacheServiceInfo> authenticatorInfo =
            mAuthenticatorCache->GetServiceInfo(value, accounts->mUserId);
    Boolean customTokens = FALSE;
    if (authenticatorInfo != NULL) {
        ((IAuthenticatorDescription*)authenticatorInfo->mType.Get())->GetCustomTokens(&customTokens);
    }

    // skip the check if customTokens
    Int32 callerUid = Binder::GetCallingUid();
    Boolean permissionGranted = customTokens ||
        PermissionIsGranted(account, authTokenType, callerUid);

    AutoPtr<IBundle> loginOptions;
    if (options != NULL) {
        loginOptions = options;
    }
    else {
        CBundle::New((IBundle**)&loginOptions);
    }
    // let authenticator know the identity of the caller
    loginOptions->PutInt32(IAccountManager::KEY_CALLER_UID, callerUid);
    loginOptions->PutInt32(IAccountManager::KEY_CALLER_PID, Binder::GetCallingPid());
    if (notifyOnAuthFailure) {
        loginOptions->PutBoolean(IAccountManager::KEY_NOTIFY_ON_FAILURE, TRUE);
    }

    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    // if the caller has permission, do the peek. otherwise go the more expensive
    // route of starting a Session
    if (!customTokens && permissionGranted) {
        String authToken = ReadAuthTokenInternal(accounts, account, authTokenType);
        if (authToken != NULL) {
            AutoPtr<IBundle> result;
            CBundle::New((IBundle**)&result);
            result->PutString(IAccountManager::KEY_AUTHTOKEN, authToken);
            String name, type;
            account->GetName(&name);
            account->GetType(&type);
            result->PutString(IAccountManager::KEY_ACCOUNT_NAME, name);
            result->PutString(IAccountManager::KEY_ACCOUNT_TYPE, type);
            OnResult(response, result);
            return NOERROR;
        }
    }

    AutoPtr<CGetAuthTokenSession> session;
    ECode ec = CGetAuthTokenSession::NewByFriend((Handle32)accounts.Get(), response,
            account, authTokenType, expectActivityLaunch, loginOptions,
            notifyOnAuthFailure, permissionGranted, customTokens,
            (IIAccountManager*)this, (CGetAuthTokenSession**)&session);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(identityToken);
        return ec;
    }
    session->Bind();
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
    return NOERROR;
}

void CAccountManagerService::CreateNoCredentialsPermissionNotification(
    /* [in] */ IAccount* account,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 userId)
{
    Int32 uid;
    intent->GetInt32Extra(
            GrantCredentialsPermissionActivity::EXTRAS_REQUESTING_UID, -1, &uid);
    String authTokenType, authTokenLabel;
    intent->GetStringExtra(
            GrantCredentialsPermissionActivity::EXTRAS_AUTH_TOKEN_TYPE, &authTokenType);
    intent->GetStringExtra(
            GrantCredentialsPermissionActivity::EXTRAS_AUTH_TOKEN_LABEL, &authTokenLabel);

    AutoPtr<INotification> n;
    ASSERT_SUCCEEDED(CNotification::New(R::drawable::stat_sys_warning, NULL,
            0 /* when */, (INotification**)&n));
    AutoPtr<ICharSequence> cs;
    String name;
    account->GetName(&name);
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr< ArrayOf<ICharSequence*> > args = ArrayOf<ICharSequence*>::Alloc(1);
    args->Set(0, cs);
    String titleAndSubtitle;
    mContext->GetString(R::string::permission_request_notification_with_subtitle,
            (ArrayOf<IInterface*>*)args.Get(), &titleAndSubtitle);
    Int32 index = titleAndSubtitle.IndexOf('\n');
    String title = titleAndSubtitle;
    String subtitle = String("");
    if (index > 0) {
        title = titleAndSubtitle.Substring(0, index);
        subtitle = titleAndSubtitle.Substring(index + 1);
    }
    AutoPtr<IUserHandle> user;
    CUserHandle::New(userId, (IUserHandle**)&user);
    AutoPtr<IPendingIntentHelper> piHelper;
    assert(0);
    // ASSERT_SUCCEEDED(CPendingIntentHelper::AcquireSingleton(
    //         (IPendingIntentHelper**)&piHelper));
    AutoPtr<IPendingIntent> pendingIntent;
    piHelper->GetActivityAsUser(mContext, 0, intent,
            IPendingIntent::FLAG_CANCEL_CURRENT, NULL, user, (IPendingIntent**)&pendingIntent);
    AutoPtr<ICharSequence> titleCs, subtitleCs;
    CString::New(title, (ICharSequence**)&titleCs);
    CString::New(subtitle, (ICharSequence**)&subtitleCs);
    n->SetLatestEventInfo(mContext, titleCs, subtitleCs, pendingIntent);
    AutoPtr<IInteger32> notificationId = GetCredentialPermissionNotificationId(
            account, authTokenType, uid);
    Int32 value;
    notificationId->GetValue(&value);
    InstallNotification(value, n, user);
}

AutoPtr<IIntent> CAccountManagerService::NewGrantCredentialsPermissionIntent(
    /* [in] */ IAccount* account,
    /* [in] */ Int32 uid,
    /* [in] */ IAccountAuthenticatorResponse* response,
    /* [in] */ const String& authTokenType,
    /* [in] */ const String& authTokenLabel)
{
    AutoPtr<IIntent> intent;
    assert(0);
    // CIntent::New(mContext, GrantCredentialsPermissionActivity::class);
    // See FLAG_ACTIVITY_NEW_TASK docs for limitations and benefits of the flag.
    // Since it was set in Eclair+ we can't change it without breaking apps using
    // the intent from a non-Activity context.
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    String s;
    GetCredentialPermissionNotificationId(account, authTokenType, uid)->ToString(&s);
    intent->AddCategory(s);

    intent->PutExtra(GrantCredentialsPermissionActivity::EXTRAS_ACCOUNT,
            (IParcelable*)account->Probe(EIID_IParcelable));
    intent->PutExtra(GrantCredentialsPermissionActivity::EXTRAS_AUTH_TOKEN_TYPE, authTokenType);
    intent->PutExtra(GrantCredentialsPermissionActivity::EXTRAS_RESPONSE,
            (IParcelable*)response->Probe(EIID_IParcelable));
    intent->PutExtra(GrantCredentialsPermissionActivity::EXTRAS_REQUESTING_UID, uid);

    return intent;
}

AutoPtr<IInteger32> CAccountManagerService::GetCredentialPermissionNotificationId(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Int32 uid)
{
    AutoPtr<IInteger32> id;
    AutoPtr<UserAccounts> accounts = GetUserAccounts(UserHandle::GetUserId(uid));

    typedef Pair<AutoPtr<IAccount>, String> AccountStringPair;
    {
        AutoLock lock(accounts->mCredentialsPermissionNotificationIdsLock);
        AutoPtr<IInteger32> integer32;
        CInteger32::New(uid, (IInteger32**)&integer32);
        Pair<AccountStringPair *, AutoPtr<IInteger32> >* key =
                new Pair<AccountStringPair *, AutoPtr<IInteger32> >(
                        new AccountStringPair(account, authTokenType), integer32);
        // Pair<Pair<AutoPtr<IAccount>, String>*, AutoPtr<IInteger32> >* key
        //         = new (Pair<AutoPtr<IAccount>, String>(account, authTokenType), integer32);
        HashMap<Pair<AccountStringPair*, AutoPtr<IInteger32> >*,
                AutoPtr<IInteger32> >::Iterator it =
                        accounts->mCredentialsPermissionNotificationIds.Find(key);
        if (it != accounts->mCredentialsPermissionNotificationIds.End()) {
            id = it->mSecond;
        }
        if (id == NULL) {
            Int32 value;
            mNotificationIds->IncrementAndGet(&value);
            CInteger32::New(value, (IInteger32**)&id);
            accounts->mCredentialsPermissionNotificationIds[key] = id;
        }
    }
    return id;
}

Int32 CAccountManagerService::GetSigninRequiredNotificationId(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account)
{
    AutoPtr<IInteger32> id;
    {
        AutoLock lock(accounts->mSigninRequiredNotificationIdsLock);
        HashMap<AutoPtr<IAccount>, AutoPtr<IInteger32> >::Iterator it =
                accounts->mSigninRequiredNotificationIds.Find(account);
        if (it != accounts->mSigninRequiredNotificationIds.End()) {
            id = it->mSecond;
        }
        if (id == NULL) {
            Int32 result;
            mNotificationIds->IncrementAndGet(&result);
            CInteger32::New(result, (IInteger32**)&id);
            accounts->mSigninRequiredNotificationIds[account] = id;
            //return result;
        }
    }
    Int32 value;
    id->GetValue(&value);
    return value;
}

ECode CAccountManagerService::AddAcount(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& accountType,
    /* [in] */ const String& authTokenType,
    /* [in] */ const ArrayOf<String>& requiredFeatures,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IBundle* optionsIn)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "addAccount: accountType " + accountType
    //             + ", response " + response
    //             + ", authTokenType " + authTokenType
    //             + ", requiredFeatures " + stringArrayToString(requiredFeatures)
    //             + ", expectActivityLaunch " + expectActivityLaunch
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("response is null");
    }
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("accountType is null");
    }
    FAIL_RETURN(CheckManageAccountsPermission());

    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int32 pid = Binder::GetCallingPid();
    Int32 uid = Binder::GetCallingUid();
    AutoPtr<IBundle> options;
    if (optionsIn == NULL) {
        CBundle::New((IBundle**)&options);
    }
    else {
        options = optionsIn;
    }
    options->PutInt32(IAccountManager::KEY_CALLER_UID, uid);
    options->PutInt32(IAccountManager::KEY_CALLER_PID, pid);

    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<CAddAcountSession> session;
    ECode ec = CAddAcountSession::NewByFriend((Handle32)accounts.Get(), response,
            accountType, expectActivityLaunch, authTokenType, requiredFeatures, options,
            (IIAccountManager*)this, (CAddAcountSession**)&session);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(identityToken);
        return ec;
    }
    session->Bind();
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // }
}

ECode CAccountManagerService::ConfirmCredentialsAsUser(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ IBundle* options,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ Int32 userId)
{
    // Only allow the system process to read accounts of other users
    if (userId != UserHandle::GetCallingUserId()
            && Binder::GetCallingUid() != Process::MyUid()) {
        Slogger::E(TAG, "User %d trying to confirm account credentials for %d", UserHandle::GetCallingUserId(), userId);
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("User " + UserHandle.getCallingUserId()
        //         + " trying to confirm account credentials for " + userId);
    }

    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "confirmCredentials: " + account
    //             + ", response " + response
    //             + ", expectActivityLaunch " + expectActivityLaunch
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("response is null");
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    FAIL_RETURN(CheckManageAccountsPermission());
    AutoPtr<UserAccounts> accounts = GetUserAccounts(userId);
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<CConfirmCredentialsAsUserSession> session;
    ECode ec = CConfirmCredentialsAsUserSession::NewByFriend((Handle32)accounts.Get(),
            response, expectActivityLaunch, account, options,
            (IIAccountManager*)this, (CConfirmCredentialsAsUserSession**)&session);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(identityToken);
        return ec;
    }
    session->Bind();
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // }
}

ECode CAccountManagerService::UpdateCredentials(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IBundle* options)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "updateCredentials: " + account
    //             + ", response " + response
    //             + ", authTokenType " + authTokenType
    //             + ", expectActivityLaunch " + expectActivityLaunch
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("response is null");
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("authTokenType is null");
    }
    FAIL_RETURN(CheckManageAccountsPermission());
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<CUpdateCredentialsSession> session;
    ECode ec = CUpdateCredentialsSession::NewByFriend((Handle32)accounts.Get(), response,
            expectActivityLaunch, account, authTokenType, options,
            (IIAccountManager*)this, (CUpdateCredentialsSession**)&session);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(identityToken);
        return ec;
    }
    session->Bind();
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // }
}

ECode CAccountManagerService::EditProperties(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& accountType,
    /* [in] */ Boolean expectActivityLaunch)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "editProperties: accountType " + accountType
    //             + ", response " + response
    //             + ", expectActivityLaunch " + expectActivityLaunch
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("response is null");
    }
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("accountType is null");
    }
    FAIL_RETURN(CheckManageAccountsPermission());
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<CEditPropertiesSession> session;
    ECode ec = CEditPropertiesSession::NewByFriend((Handle32)accounts.Get(), response,
            accountType, expectActivityLaunch, (IIAccountManager*)this,
            (CEditPropertiesSession**)&session);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(identityToken);
        return ec;
    }
    session->Bind();
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // }
}

ECode CAccountManagerService::GetAccounts(
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<IAccount*>** _accounts)
{
    VALIDATE_NOT_NULL(_accounts);
    FAIL_RETURN(CheckReadAccountsPermission());
    AutoPtr<UserAccounts> userAccounts = GetUserAccounts(userId);
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    {
        AutoLock lock(userAccounts->mCacheLock);
        AutoPtr< ArrayOf<IAccount*> > accounts = GetAccountsFromCacheLocked(
                userAccounts, String(NULL));
        *_accounts = accounts;
        REFCOUNT_ADD(*_accounts);
    }
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // } finally {
    //     restoreCallingIdentity(identityToken);
    // }
}

ECode CAccountManagerService::GetRunningAccounts(
    /* [out, callee] */ ArrayOf<IAccountAndUser*>** accounts)
{
    VALIDATE_NOT_NULL(accounts);
    AutoPtr< ArrayOf<Int32> > runningUserIds;
    // try {
    FAIL_RETURN(ActivityManagerNative::GetDefault()->GetRunningUserIds(
            (ArrayOf<Int32>**)&runningUserIds));
    // } catch (RemoteException e) {
    //     // Running in system_server; should never happen
    //     throw new RuntimeException(e);
    // }
    return GetAccounts(*runningUserIds, accounts);
}

ECode CAccountManagerService::GetAllAccounts(
    /* [out, callee] */ ArrayOf<IAccountAndUser*>** accounts)
{
    VALIDATE_NOT_NULL(accounts);

    AutoPtr<IObjectContainer> users;
    assert(0);
    // GetUserManager()->GetUsers((IObjectContainer**)&users);
    Int32 count;
    users->GetObjectCount(&count);
    AutoPtr<IObjectEnumerator> enumerator;
    users->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    AutoPtr< ArrayOf<Int32> > userIds = ArrayOf<Int32>::Alloc(count);
    Boolean hasNext;
    Int32 i = 0;
    while(enumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IUserInfo> info;
        enumerator->Current((IInterface**)&info);
        info->GetId(&(*userIds)[i]);
    }
    return GetAccounts(*userIds, accounts);
}

ECode CAccountManagerService::GetAccounts(
    /* [in] */ const ArrayOf<Int32>& userIds,
    /* [out, callee] */ ArrayOf<IAccountAndUser*>** accounts)
{
    VALIDATE_NOT_NULL(accounts);

    AutoPtr< ArrayOf<IAccountAndUser*> > runningAccounts;
    {
        AutoLock lock(mUsersLock);
        HashMap<Int32, AutoPtr<UserAccounts> >::Iterator it;
        for (Int32 i = 0; i < userIds.GetLength(); ++i) {
            Int32 userId = userIds[i];
            AutoPtr<UserAccounts> userAccounts = GetUserAccounts(userId);
            if (userAccounts == NULL) continue;
            AutoLock lock(userAccounts->mCacheLock);
            AutoPtr< ArrayOf<IAccount*> > accounts =
                    GetAccountsFromCacheLocked(userAccounts, String(NULL));
            runningAccounts = ArrayOf<IAccountAndUser*>::Alloc(accounts->GetLength());
            for (Int32 a = 0; a < accounts->GetLength(); a++) {
                AutoPtr<IAccountAndUser> user;
                CAccountAndUser::New((*accounts)[a], userId, (IAccountAndUser**)&user);
                runningAccounts->Set(a, user);
            }
        }
    }
    *accounts = runningAccounts;
    REFCOUNT_ADD(*accounts);
    return NOERROR;
}

ECode CAccountManagerService::GetAccountsAsUser(
    /* [in] */ const String& accountType,
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<IAccount*>** _accounts)
{
    VALIDATE_NOT_NULL(_accounts);
    // Only allow the system process to read accounts of other users
    if (userId != UserHandle::GetCallingUserId()
            && Binder::GetCallingUid() != Process::MyUid()) {
        Slogger::E(TAG, "User %d trying to get account for %d", UserHandle::GetCallingUserId(), userId);
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("User " + UserHandle.getCallingUserId()
        //         + " trying to get account for " + userId);
    }

    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "getAccounts: accountType " + type
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    FAIL_RETURN(CheckReadAccountsPermission());
    AutoPtr<UserAccounts> userAccounts = GetUserAccounts(userId);
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    {
        AutoLock lock(userAccounts->mCacheLock);
        AutoPtr< ArrayOf<IAccount*> > accounts = GetAccountsFromCacheLocked(
                userAccounts, accountType);
        *_accounts = accounts;
        REFCOUNT_ADD(*_accounts);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
    return NOERROR;
}

ECode CAccountManagerService::GetAccounts(
    /* [in] */ const String& accountType,
    /* [out, callee] */ ArrayOf<IAccount*>** accounts)
{
    VALIDATE_NOT_NULL(accounts);
    return GetAccountsAsUser(accountType, UserHandle::GetCallingUserId(), accounts);
}

ECode CAccountManagerService::GetAccountsByFeatures(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& accountType,
    /* [in] */ ArrayOf<String>* features)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "getAccounts: accountType " + type
    //             + ", response " + response
    //             + ", features " + stringArrayToString(features)
    //             + ", caller's uid " + Binder.getCallingUid()
    //             + ", pid " + Binder.getCallingPid());
    // }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("response is null");
    }
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("accountType is null");
    }
    FAIL_RETURN(CheckReadAccountsPermission());
    AutoPtr<UserAccounts> userAccounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    if (features == NULL || features->GetLength() == 0) {
        AutoPtr< ArrayOf<IAccount*> > accounts;
        {
            AutoLock lock(userAccounts->mCacheLock);
            accounts = GetAccountsFromCacheLocked(userAccounts, accountType);
        }
        AutoPtr<IBundle> result;
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&result));
        AutoPtr< ArrayOf<IParcelable*> > parcels = ArrayOf<IParcelable*>::Alloc(
                accounts->GetLength());
        for (Int32 i = 0; i < accounts->GetLength(); ++i) {
            parcels->Set(i, IParcelable::Probe((*accounts)[i]));
        }
        result->PutParcelableArray(IAccountManager::KEY_ACCOUNTS, parcels);
        OnResult(response, result);
        return NOERROR;
    }
    AutoPtr<CGetAccountsByTypeAndFeatureSession> session;
    ECode ec = CGetAccountsByTypeAndFeatureSession::NewByFriend((Handle32)userAccounts.Get(),
            response, accountType, *features, (IIAccountManager*)this,
            (CGetAccountsByTypeAndFeatureSession**)&session);
    if (FAILED(ec)) {
        Binder::RestoreCallingIdentity(identityToken);
        return ec;
    }
    session->Bind();
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
    return NOERROR;
}

Int64 CAccountManagerService::GetAccountIdLocked(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IAccount* account)
{
    AutoPtr<ICursor> cursor;
    AutoPtr< ArrayOf<String> > ss1 = ArrayOf<String>::Alloc(1);
    (*ss1)[0] = ACCOUNTS_ID;
    AutoPtr< ArrayOf<String> > ss2 = ArrayOf<String>::Alloc(2);
    account->GetName(&(*ss2)[0]);
    account->GetType(&(*ss2)[1]);
    String nullStr;
    db->Query(TABLE_ACCOUNTS, ss1, String("name=? AND type=?"),
            ss2, nullStr, nullStr, nullStr, (ICursor**)&cursor);
    // try {
    Boolean result;
    Int64 value = -1;
    if (cursor->MoveToNext(&result), result) {
        cursor->GetInt64(0, &value);
    }
    cursor->Close();
    return value;
    // } finally {
    //     cursor.close();
    // }
}

Int64 CAccountManagerService::GetExtrasIdLocked(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int64 accountId,
    /* [in] */ const String& key)
{
    AutoPtr<ICursor> cursor;
    AutoPtr< ArrayOf<String> > ss1 = ArrayOf<String>::Alloc(1);
    (*ss1)[0] = EXTRAS_ID;
    AutoPtr< ArrayOf<String> > ss2 = ArrayOf<String>::Alloc(1);
    (*ss2)[0] = key;
    String nullStr;
    db->Query(TABLE_EXTRAS, ss1,
            EXTRAS_ACCOUNTS_ID + String("=") + StringUtils::Int64ToString(accountId)
            + String(" AND ") + EXTRAS_KEY + String("=?"),
            ss2, nullStr, nullStr, nullStr, (ICursor**)&cursor);
    // try {
    Boolean result;
    Int64 value = -1;
    if (cursor->MoveToNext(&result), result) {
        Int64 value;
        cursor->GetInt64(0, &value);
    }
    cursor->Close();
    return value;
    // } finally {
    //     cursor.close();
    // }
}

ECode CAccountManagerService::GetDatabaseName(
    /* [in] */ Int32 userId,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> systemDir;
    env->GetSystemSecureDirectory((IFile**)&systemDir);
    AutoPtr<IFile> userSysDir;
    env->GetUserSystemDirectory(userId, (IFile**)&userSysDir);
    AutoPtr<IFile> databaseFile;
    CFile::New(userSysDir, DATABASE_NAME,
            (IFile**)&databaseFile);
    if (userId == 0) {
        // Migrate old file, if it exists, to the new location.
        // Make sure the new file doesn't already exist. A dummy file could have been
        // accidentally created in the old location, causing the new one to become corrupted
        // as well.
        AutoPtr<IFile> oldFile;
        CFile::New(systemDir, DATABASE_NAME, (IFile**)&oldFile);
        Boolean isExist;
        if ((oldFile->Exists(&isExist), isExist) &&
                (databaseFile->Exists(&isExist), !isExist)) {
            // Check for use directory; create if it doesn't exist, else renameTo will fail
            AutoPtr<IFile> userDir;
            env->GetUserSystemDirectory(userId, (IFile**)&userDir);
            if (userDir->Exists(&isExist), !isExist) {
                Boolean succeeded;
                if (userDir->Mkdirs(&succeeded), !succeeded) {
                    Slogger::E(TAG, "User dir cannot be created: %p", userDir.Get());
                    *name = String(NULL);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    // throw new IllegalStateException("User dir cannot be created: " + userDir);
                }
            }
            Boolean succeeded;
            if (oldFile->RenameTo(databaseFile, &succeeded), !succeeded) {
                Slogger::E(TAG, "User dir cannot be migrated: %p", databaseFile.Get());
                *name = String(NULL);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
                // throw new IllegalStateException("User dir cannot be migrated: " + databaseFile);
            }
        }
    }
    return databaseFile->GetPath(name);
}

ECode CAccountManagerService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    // return asBinder();
    assert(0);
    return E_NOT_IMPLEMENTED;
}

Boolean CAccountManagerService::ScanArgs(
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ const String& value)
{
    if (args != NULL) {
        for (Int32 i = 0; i < args->GetLength(); ++i) {
            if (value.Equals((*args)[i])) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

// protected void dump(FileDescriptor fd, PrintWriter fout, String[] args) {
//     if (mContext.checkCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP)
//             != PackageManager.PERMISSION_GRANTED) {
//         fout.println("Permission Denial: can't dump AccountsManager from from pid="
//                 + Binder.getCallingPid() + ", uid=" + Binder.getCallingUid()
//                 + " without permission " + Elastos::Droid::Manifest::permission::DUMP);
//         return;
//     }
//     Boolean isCheckinRequest = scanArgs(args, "--checkin") || scanArgs(args, "-c");
//     final IndentingPrintWriter ipw = new IndentingPrintWriter(fout, "  ");

//     final List<UserInfo> users = getUserManager().getUsers();
//     for (UserInfo user : users) {
//         ipw.println("User " + user + ":");
//         ipw.increaseIndent();
//         dumpUser(getUserAccounts(user.id), fd, ipw, args, isCheckinRequest);
//         ipw.println();
//         ipw.decreaseIndent();
//     }
// }

// private void dumpUser(UserAccounts userAccounts, FileDescriptor fd, PrintWriter fout,
//         String[] args, boolean isCheckinRequest) {
//     synchronized(userAccounts.cacheLock) {
//         final SQLiteDatabase db = userAccounts.openHelper.getReadableDatabase();

//         if (isCheckinRequest) {
//             // This is a checkin request. *Only* upload the account types and the count of each.
//             Cursor cursor = db.query(TABLE_ACCOUNTS, ACCOUNT_TYPE_COUNT_PROJECTION,
//                     null, null, ACCOUNTS_TYPE, null, null);
//             try {
//                 while (cursor.moveToNext()) {
//                     // print type,count
//                     fout.println(cursor.getString(0) + "," + cursor.getString(1));
//                 }
//             } finally {
//                 if (cursor != null) {
//                     cursor.close();
//                 }
//             }
//         } else {
//             Account[] accounts = getAccountsFromCacheLocked(userAccounts, null /* type */);
//             fout.println("Accounts: " + accounts.length);
//             for (Account account : accounts) {
//                 fout.println("  " + account);
//             }

//             fout.println();
//             synchronized(mSessions) {
//                 final long now = SystemClock.elapsedRealtime();
//                 fout.println("Active Sessions: " + mSessions.size());
//                 for (Session session : mSessions.values()) {
//                     fout.println("  " + session.toDebugString(now));
//                 }
//             }

//             fout.println();
//             mAuthenticatorCache.dump(fd, fout, args, userAccounts.userId);
//         }
//     }
// }

void CAccountManagerService::DoNotification(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ ICharSequence* message,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 userId)
{
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "doNotification: " + message + " intent:" + intent);
    // }

    AutoPtr<IComponentName> component;
    String className;
    if ((intent->GetComponent((IComponentName**)&component), component != NULL) &&
            (component->GetClassName(&className), className.Equals("GrantCredentialsPermissionActivity"))) {
        CreateNoCredentialsPermissionNotification(account, intent, userId);
    }
    else {
        Int32 notificationId = GetSigninRequiredNotificationId(accounts, account);
        intent->AddCategory(StringUtils::Int32ToString(notificationId));
        AutoPtr<INotification> n;
        if (FAILED(CNotification::New(R::drawable::stat_sys_warning, NULL,
                0 /* when */, (INotification**)&n))) {
            Binder::RestoreCallingIdentity(identityToken);
            return;
        }
        AutoPtr<IUserHandle> user;
        CUserHandle::New(userId, (IUserHandle**)&user);
        AutoPtr<ICharSequence> cs;
        mContext->GetText(R::string::notification_title, (ICharSequence**)&cs);
        String notificationTitleFormat;
        cs->ToString(&notificationTitleFormat);
        AutoPtr<ICharSequence> namecs;
        String name;
        account->GetName(&name);
        CString::New(name, (ICharSequence**)&namecs);
        AutoPtr<IPendingIntentHelper> helper;
        assert(0);
        // ASSERT_SUCCEEDED(CPendingIntentHelper::AcquireSingleton(
        //         (IPendingIntentHelper**)&helper));
        AutoPtr<IPendingIntent> pi;
        if (FAILED(helper->GetActivityAsUser(
                mContext, 0, intent, IPendingIntent::FLAG_CANCEL_CURRENT,
                NULL, user, (IPendingIntent**)&pi))) {
            Binder::RestoreCallingIdentity(identityToken);
            return;
        }
        if (FAILED(n->SetLatestEventInfo(mContext,
                namecs/*String.format(notificationTitleFormat, account.name)*/,
                message, pi))) {
            Binder::RestoreCallingIdentity(identityToken);
            return;
        }
        InstallNotification(notificationId, n, user);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
}

void CAccountManagerService::InstallNotification(
    /* [in] */ Int32 notificationId,
    /* [in] */ INotification* n,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<IInterface> obj;
    ASSERT_SUCCEEDED(mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj));
    AutoPtr<INotificationManager> nm = INotificationManager::Probe(obj);
    nm->NotifyAsUser(String(NULL), notificationId, n, user);
}

void CAccountManagerService::CancelNotification(
    /* [in] */ Int32 id,
    /* [in] */ IUserHandle* user)
{
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IInterface> obj;
    ASSERT_SUCCEEDED(mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj));
    AutoPtr<INotificationManager> nm = INotificationManager::Probe(obj);

    nm->CancelAsUser(String(NULL), id, user);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
}

ECode CAccountManagerService::CheckBinderPermission(
    /* [in] */ ArrayOf<String>* permissions)
{
    Int32 uid = Binder::GetCallingUid();

    for (Int32 i = 0; i < permissions->GetLength(); ++i) {
        Int32 value;
        FAIL_RETURN(mContext->CheckCallingOrSelfPermission((*permissions)[i], &value));
        if (value == IPackageManager::PERMISSION_GRANTED) {
            // if (Log.isLoggable(TAG, Log.VERBOSE)) {
            //     Log.v(TAG, "  caller uid " + uid + " has " + perm);
            // }
            return NOERROR;
        }
    }

    // String msg = "caller uid " + uid + " lacks any of " + TextUtils.join(",", permissions);
    Slogger::W(TAG, "  caller uid %d lacks any of ", uid);
    return E_SECURITY_EXCEPTION;
    // throw new SecurityException(msg);
}

Boolean CAccountManagerService::InSystemImage(
    /* [in] */ Int32  callingUid)
{
    AutoPtr<IPackageManager> userPackageManager;
    // try {
    AutoPtr<IUserHandle> userH;
    CUserHandle::New(UserHandle::GetUserId(callingUid), (IUserHandle**)&userH);
    AutoPtr<IContext> context;
    if (FAILED(mContext->CreatePackageContextAsUser(
            String("android"), 0, userH, (IContext**)&context))) {
        return FALSE;
    }
    if (FAILED(context->GetPackageManager((IPackageManager**)&userPackageManager))) {
        return FALSE;
    }
    // } catch (NameNotFoundException e) {
    //     return false;
    // }

    AutoPtr< ArrayOf<String> > packages;
    userPackageManager->GetPackagesForUid(callingUid, (ArrayOf<String>**)&packages);
    for (Int32 i = 0; i < packages->GetLength(); ++i) {
        // try {
        AutoPtr<IPackageInfo> packageInfo;
        if (FAILED(userPackageManager->GetPackageInfo((*packages)[i], 0 /* flags */,
                (IPackageInfo**)&packageInfo))) {
            return FALSE;
        }
        if (packageInfo != NULL) {
            AutoPtr<IApplicationInfo> appInfo;
            packageInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 flags;
            appInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                return TRUE;
            }
        }
        // } catch (PackageManager.NameNotFoundException e) {
        //     return false;
        // }
    }
    return FALSE;
}

Boolean CAccountManagerService::PermissionIsGranted(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Int32 callerUid)
{
    Boolean inSystemImage = InSystemImage(callerUid);
    Boolean fromAuthenticator = FALSE;
    if (account != NULL) {
        String type;
        account->GetType(&type);
        fromAuthenticator = HasAuthenticatorUid(type, callerUid);
    }
    Boolean hasExplicitGrants = account != NULL
            && HasExplicitlyGrantedPermission(account, authTokenType, callerUid);
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "checkGrantsOrCallingUidAgainstAuthenticator: caller uid "
    //             + callerUid + ", " + account
    //             + ": is authenticator? " + fromAuthenticator
    //             + ", has explicit permission? " + hasExplicitGrants);
    // }
    return fromAuthenticator || hasExplicitGrants || inSystemImage;
}

Boolean CAccountManagerService::HasAuthenticatorUid(
    /* [in] */ const String& accountType,
    /* [in] */ Int32 callingUid)
{
    AutoPtr< List<AutoPtr<IRegisteredServicesCacheServiceInfo> > > services = mAuthenticatorCache->GetAllServices(
            UserHandle::GetUserId(callingUid));
    List<AutoPtr<IRegisteredServicesCacheServiceInfo> >::Iterator it = services->Begin();
    for (; it != services->End(); ++it) {
        AutoPtr<IRegisteredServicesCacheServiceInfo> serviceInfo = *it;
        AutoPtr<IAuthenticatorDescription> type = (IAuthenticatorDescription*)serviceInfo->mType.Get();
        assert(type != NULL);
        String descType;
        type->GetType(&descType);
        if (descType.Equals(accountType)) {
            Int32 result;
            return (serviceInfo->mUid == callingUid) ||
                    (mPackageManager->CheckSignatures(serviceInfo->mUid, callingUid, &result),
                    result == IPackageManager::SIGNATURE_MATCH);
        }
    }

    return FALSE;
}

Boolean CAccountManagerService::HasExplicitlyGrantedPermission(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Int32 callerUid)
{
    if (callerUid == IProcess::SYSTEM_UID) {
        return TRUE;
    }
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();

    AutoLock lock(accounts->mCacheLock);
    AutoPtr<ISQLiteDatabase> db;
    accounts->mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(4);
    (*args)[0] = StringUtils::ToString(callerUid);
    (*args)[1] = authTokenType;
    account->GetName(&(*args)[2]);
    account->GetType(&(*args)[3]);
    AutoPtr<IDatabaseUtils> helper;
    ASSERT_SUCCEEDED(CDatabaseUtils::AcquireSingleton((IDatabaseUtils**)&helper));
    Int64 result;
    helper->Int64ForQuery(db, COUNT_OF_MATCHING_GRANTS, args, &result);
    Boolean permissionGranted = result != 0;

    if (!permissionGranted) {
        AutoPtr<IActivityManagerHelper> activityHelper;
        ASSERT_SUCCEEDED(CActivityManagerHelper::AcquireSingleton(
                (IActivityManagerHelper**)&activityHelper));
        Boolean isRunning;
        if (activityHelper->IsRunningInTestHarness(&isRunning), isRunning) {
            // TODO: Skip this check when running automated tests. Replace this
            // with a more general solution.
            Slogger::D(TAG, "no credentials permission for usage of %p, %s by uid %d but ignoring since device is in test harness.",
                    account, (const char*)authTokenType, callerUid);
            return TRUE;
        }
    }
    return permissionGranted;
}

ECode CAccountManagerService::CheckCallingUidAgainstAuthenticator(
    /* [in] */ IAccount* account)
{
    Int32 uid = Binder::GetCallingUid();
    if (account != NULL) {
        String type;
        account->GetType(&type);
        if (HasAuthenticatorUid(type, uid)) {
            return NOERROR;
        }
    }
    Slogger::W(TAG, "caller uid %d is different than the authenticator's uid", uid);
    return E_SECURITY_EXCEPTION;
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "caller uid " + uid + " is the same as the authenticator's uid");
    // }
}

ECode CAccountManagerService::CheckAuthenticateAccountsPermission(
    /* [in] */ IAccount* account)
{
    AutoPtr< ArrayOf<String> > perms = ArrayOf<String>::Alloc(1);
    (*perms)[0] = Elastos::Droid::Manifest::permission::AUTHENTICATE_ACCOUNTS;
    FAIL_RETURN(CheckBinderPermission(perms));
    return CheckCallingUidAgainstAuthenticator(account);
}

ECode CAccountManagerService::CheckReadAccountsPermission()
{
    AutoPtr< ArrayOf<String> > perms = ArrayOf<String>::Alloc(1);
    (*perms)[0] = Elastos::Droid::Manifest::permission::GET_ACCOUNTS;
    return CheckBinderPermission(perms);
}

ECode CAccountManagerService::CheckManageAccountsPermission()
{
    AutoPtr< ArrayOf<String> > perms = ArrayOf<String>::Alloc(1);
    (*perms)[0] = Elastos::Droid::Manifest::permission::MANAGE_ACCOUNTS;
    return CheckBinderPermission(perms);
}

ECode CAccountManagerService::CheckManageAccountsOrUseCredentialsPermissions()
{
    AutoPtr< ArrayOf<String> > perms = ArrayOf<String>::Alloc(2);
    (*perms)[0] = Elastos::Droid::Manifest::permission::MANAGE_ACCOUNTS;
    (*perms)[1] = Elastos::Droid::Manifest::permission::USE_CREDENTIALS;
    return CheckBinderPermission(perms);
}

ECode CAccountManagerService::UpdateAppPermission(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean value)
{
    Int32 callingUid = Binder::GetCallingUid();

    if (callingUid != IProcess::SYSTEM_UID) {
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException();
    }

    if (value) {
        GrantAppPermission(account, authTokenType, uid);
    }
    else {
        RevokeAppPermission(account, authTokenType, uid);
    }
    return NOERROR;
}

void CAccountManagerService::GrantAppPermission(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Int32 uid)
{
    if (account == NULL || authTokenType == NULL) {
        Slogger::E(TAG, "grantAppPermission: called with invalid arguments"/*, new Exception()*/);
        return;
    }
    AutoPtr<UserAccounts> accounts = GetUserAccounts(UserHandle::GetUserId(uid));

    AutoLock lock(accounts->mCacheLock);
    AutoPtr<ISQLiteDatabase> db;
    ASSERT_SUCCEEDED(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db));
    db->BeginTransaction();
    // try {
    Int64 accountId = GetAccountIdLocked(db, account);
    if (accountId >= 0) {
        AutoPtr<IContentValues> values;
        ASSERT_SUCCEEDED(CContentValues::New((IContentValues**)&values));
        AutoPtr<IInteger64> integer64;
        CInteger64::New(accountId, (IInteger64**)&integer64);
        values->PutInt64(GRANTS_ACCOUNTS_ID, integer64);
        AutoPtr<ICharSequence> csq;
        CString::New(authTokenType, (ICharSequence**)&csq);
        values->PutString(GRANTS_AUTH_TOKEN_TYPE, csq);
        AutoPtr<IInteger32> integer32;
        CInteger32::New(uid, (IInteger32**)&integer32);
        values->PutInt32(GRANTS_GRANTEE_UID, integer32);
        Int64 result;
        if (FAILED(db->Insert(TABLE_GRANTS, GRANTS_ACCOUNTS_ID, values, &result))) {
            goto fail;
        }
        db->SetTransactionSuccessful();
    }
    // } finally {
fail:
    db->EndTransaction();
    // }
    AutoPtr<IUserHandle> userH;
    CUserHandle::New(accounts->mUserId, (IUserHandle**)&userH);
    AutoPtr<IInteger32> integer32 = GetCredentialPermissionNotificationId(
            account, authTokenType, uid);
    Int32 value;
    integer32->GetValue(&value);
    CancelNotification(value, userH);
}

void CAccountManagerService::RevokeAppPermission(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Int32 uid)
{
    if (account == NULL || authTokenType == NULL) {
        Slogger::E(TAG, "revokeAppPermission: called with invalid arguments"/*, new Exception()*/);
        return;
    }
    AutoPtr<UserAccounts> accounts = GetUserAccounts(UserHandle::GetUserId(uid));

    AutoLock lock(accounts->mCacheLock);
    AutoPtr<ISQLiteDatabase> db;
    ASSERT_SUCCEEDED(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db));
    db->BeginTransaction();
    // try {
    Int64 accountId = GetAccountIdLocked(db, account);
    if (accountId >= 0) {
        AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(3);
        (*args)[0] = StringUtils::ToString(accountId);
        (*args)[1] = authTokenType;
        (*args)[2] = StringUtils::ToString(uid);
        Int32 result;
        if (FAILED(db->Delete(TABLE_GRANTS, GRANTS_ACCOUNTS_ID + String("=? AND ")
                + GRANTS_AUTH_TOKEN_TYPE + String("=? AND ")
                + GRANTS_GRANTEE_UID + String("=?"), args, &result))) {
            goto fail;
        }
        db->SetTransactionSuccessful();
    }
    // } finally {
    //     db.endTransaction();
    // }
fail:
    db->EndTransaction();
    AutoPtr<IUserHandle> userH;
    CUserHandle::New(accounts->mUserId, (IUserHandle**)&userH);
    AutoPtr<IInteger32> integer32 = GetCredentialPermissionNotificationId(
            account, authTokenType, uid);
    Int32 value;
    integer32->GetValue(&value);
    CancelNotification(value, userH);
}

String CAccountManagerService::StringArrayToString(
    /* [in] */ ArrayOf<String>* value)
{
    return value != NULL ? (String("[") + /*TextUtils.join(",", value) +*/ String("]"))
            : String(NULL);
}

void CAccountManagerService::RemoveAccountFromCacheLocked(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account)
{
    AutoPtr< ArrayOf<IAccount*> > oldAccountsForType;
    String type;
    account->GetType(&type);
    HashMap<String, AutoPtr< ArrayOf<IAccount*> > >::Iterator it =
            accounts->mAccountCache.Find(type);
    if (it != accounts->mAccountCache.End()) {
        oldAccountsForType = it->mSecond;
    }
    if (oldAccountsForType != NULL) {
        List< AutoPtr<IAccount> > newAccountsList;
        for (Int32 i = 0; i < oldAccountsForType->GetLength(); ++i) {
            if ((*oldAccountsForType)[i] != account) {
                newAccountsList.PushBack((*oldAccountsForType)[i]);
            }
        }
        if (newAccountsList.Begin() == newAccountsList.End()) {
            accounts->mAccountCache.Erase(it);
        }
        else {
            AutoPtr< ArrayOf<IAccount*> > newAccountsForType =
                    ArrayOf<IAccount*>::Alloc(newAccountsList.GetSize());
            List< AutoPtr<IAccount> >::Iterator accountIt = newAccountsList.Begin();
            Int32 i = 0;
            for (; accountIt != newAccountsList.End(); ++accountIt, ++i) {
                newAccountsForType->Set(i, *accountIt);
            }
            account->GetType(&type);
            accounts->mAccountCache[type] = newAccountsForType;
        }
    }
    accounts->mUserDataCache.Erase(account);
    accounts->mAuthTokenCache.Erase(account);
}

void CAccountManagerService::InsertAccountIntoCacheLocked(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account)
{
    AutoPtr< ArrayOf<IAccount*> > accountsForType;
    String type;
    account->GetType(&type);
    HashMap<String, AutoPtr< ArrayOf<IAccount*> > >::Iterator it =
            accounts->mAccountCache.Find(type);
    if (it != accounts->mAccountCache.End()) {
        accountsForType = it->mSecond;
    }
    Int32 oldLength = (accountsForType != NULL) ? accountsForType->GetLength() : 0;
    AutoPtr< ArrayOf<IAccount*> > newAccountsForType =
            ArrayOf<IAccount*>::Alloc(oldLength + 1);
    if (accountsForType != NULL) {
        newAccountsForType->Copy(accountsForType, oldLength);
    }
    newAccountsForType->Set(oldLength, account);
    accounts->mAccountCache[type] = newAccountsForType;
}

AutoPtr< ArrayOf<IAccount*> > CAccountManagerService::GetAccountsFromCacheLocked(
    /* [in] */ UserAccounts* userAccounts,
    /* [in] */ const String& accountType)
{
    if (!accountType.IsNull()) {
        AutoPtr< ArrayOf<IAccount*> > accounts;
        HashMap<String, AutoPtr< ArrayOf<IAccount*> > >::Iterator it =
                userAccounts->mAccountCache.Find(accountType);
        if (it != userAccounts->mAccountCache.End()) {
            accounts = it->mSecond;
        }
        if (accounts == NULL) {
            return EMPTY_ACCOUNT_ARRAY;
        }
        else {
            return accounts;
        }
    }
    else {
        Int32 totalLength = 0;
        HashMap<String, AutoPtr< ArrayOf<IAccount*> > >::Iterator it =
                userAccounts->mAccountCache.Begin();
        for (; it != userAccounts->mAccountCache.End(); ++it) {
            totalLength += it->mSecond->GetLength();
        }
        if (totalLength == 0) {
            return EMPTY_ACCOUNT_ARRAY;
        }
        AutoPtr< ArrayOf<IAccount*> > accounts = ArrayOf<IAccount*>::Alloc(totalLength);
        totalLength = 0;
        it = userAccounts->mAccountCache.Begin();
        for (; it != userAccounts->mAccountCache.End(); ++it) {
            AutoPtr< ArrayOf<IAccount*> > accountsOfType = it->mSecond;
            accounts->Copy(totalLength, accountsOfType, accountsOfType->GetLength());
            totalLength += accountsOfType->GetLength();
        }
        return accounts;
    }
}

void CAccountManagerService::WriteUserDataIntoCacheLocked(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IAccount* account,
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr< HashMap<String, String> > userDataForAccount;
    HashMap<AutoPtr<IAccount>, AutoPtr<HashMap<String, String> > >::Iterator it =
            accounts->mUserDataCache.Find(account);
    if (it != accounts->mUserDataCache.End()) {
        userDataForAccount = it->mSecond;
    }
    if (userDataForAccount == NULL) {
        userDataForAccount = ReadUserDataForAccountFromDatabaseLocked(db, account);
        accounts->mUserDataCache[account] = userDataForAccount;
    }
    if (value.IsNull()) {
        userDataForAccount->Erase(key);
    }
    else {
        (*userDataForAccount)[key] = value;
    }
}

void CAccountManagerService::WriteAuthTokenIntoCacheLocked(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IAccount* account,
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr< HashMap<String, String> > authTokensForAccount;
    HashMap<AutoPtr<IAccount>, AutoPtr< HashMap<String, String> > >::Iterator it =
            accounts->mAuthTokenCache.Find(account);
    if (it != accounts->mAuthTokenCache.End()) {
        authTokensForAccount = it->mSecond;
    }
    if (authTokensForAccount == NULL) {
        authTokensForAccount = ReadAuthTokensForAccountFromDatabaseLocked(db, account);
        accounts->mAuthTokenCache[account] = authTokensForAccount;
    }
    if (value.IsNull()) {
        authTokensForAccount->Erase(key);
    }
    else {
        (*authTokensForAccount)[key] = value;
    }
}

String CAccountManagerService::ReadAuthTokenInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType)
{
    AutoLock lock(accounts->mCacheLock);
    AutoPtr< HashMap<String, String> > authTokensForAccount;
    HashMap<AutoPtr<IAccount>, AutoPtr< HashMap<String, String> > >::Iterator it =
            accounts->mAuthTokenCache.Find(account);
    if (it != accounts->mAuthTokenCache.End()) {
        authTokensForAccount = it->mSecond;
    }
    if (authTokensForAccount == NULL) {
        // need to populate the cache for this account
        AutoPtr<ISQLiteDatabase> db;
        accounts->mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
        authTokensForAccount = ReadAuthTokensForAccountFromDatabaseLocked(db, account);
        accounts->mAuthTokenCache[account] = authTokensForAccount;
    }
    String value(NULL);
    HashMap<String, String>::Iterator strIt = authTokensForAccount->Find(authTokenType);
    if (strIt != authTokensForAccount->End()) {
        value = strIt->mSecond;
    }
    return value;
}

String CAccountManagerService::ReadUserDataInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ const String& key)
{
    AutoLock lock(accounts->mCacheLock);
    AutoPtr< HashMap<String, String> > userDataForAccount;
    HashMap<AutoPtr<IAccount>, AutoPtr< HashMap<String, String> > >::Iterator it =
            accounts->mUserDataCache.Find(account);
    if (it != accounts->mUserDataCache.End()) {
        userDataForAccount = it->mSecond;
    }
    if (userDataForAccount == NULL) {
        // need to populate the cache for this account
        AutoPtr<ISQLiteDatabase> db;
        accounts->mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
        userDataForAccount = ReadUserDataForAccountFromDatabaseLocked(db, account);
        accounts->mUserDataCache[account] = userDataForAccount;
    }
    String value(NULL);
    HashMap<String, String>::Iterator strIt = userDataForAccount->Find(key);
    if (strIt != userDataForAccount->End()) {
        value = strIt->mSecond;
    }
    return value;
}

AutoPtr<HashMap<String, String> > CAccountManagerService::ReadUserDataForAccountFromDatabaseLocked(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IAccount* account)
{
    AutoPtr<HashMap<String, String> > userDataForAccount = new HashMap<String, String>();
    AutoPtr<ICursor> cursor;
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    account->GetName(&(*args)[0]);
    account->GetType(&(*args)[1]);
    AutoPtr< ArrayOf<String> > params = ArrayOf<String>::Alloc(2);
    params->Set(0, COLUMNS_EXTRAS_KEY_AND_VALUE[0]);
    params->Set(1, COLUMNS_EXTRAS_KEY_AND_VALUE[1]);
    String nullStr;
    ASSERT_SUCCEEDED(db->Query(TABLE_EXTRAS,
            params,
            SELECTION_USERDATA_BY_ACCOUNT, args,
            nullStr, nullStr, nullStr, (ICursor**)&cursor));
    // try {
    Boolean result;
    while (cursor->MoveToNext(&result), result) {
        String tmpkey, value;
        cursor->GetString(0, &tmpkey);
        cursor->GetString(1, &value);
        (*userDataForAccount)[tmpkey] = value;
    }
    // } finally {
    cursor->Close();
    // }
    return userDataForAccount;
}

AutoPtr<HashMap<String, String> > CAccountManagerService::ReadAuthTokensForAccountFromDatabaseLocked(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IAccount* account)
{
    AutoPtr<HashMap<String, String> > authTokensForAccount = new HashMap<String, String>();
    AutoPtr<ICursor> cursor;
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    account->GetName(&(*args)[0]);
    account->GetType(&(*args)[1]);

    AutoPtr< ArrayOf<String> > params = ArrayOf<String>::Alloc(2);
    params->Set(0, COLUMNS_AUTHTOKENS_TYPE_AND_AUTHTOKEN[0]);
    params->Set(1, COLUMNS_AUTHTOKENS_TYPE_AND_AUTHTOKEN[1]);
    String nullStr;
    ASSERT_SUCCEEDED(db->Query(TABLE_AUTHTOKENS,
            params, SELECTION_AUTHTOKENS_BY_ACCOUNT, args,
            nullStr, nullStr, nullStr, (ICursor**)&cursor));
    // try {
    Boolean result;
    while (cursor->MoveToNext(&result), result) {
        String type, authToken;
        cursor->GetString(0, &type);
        cursor->GetString(1, &authToken);
        (*authTokensForAccount)[type] = authToken;
    }
    // } finally {
    cursor->Close();
    // }
    return authTokensForAccount;
}

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos
