
#include "elastos/droid/server/accounts/AccountAuthenticatorCache.h"
#include "elastos/droid/server/accounts/CAccountManagerService.h"
#include "elastos/droid/server/accounts/CAddAccountAsUserSession.h"
#include "elastos/droid/server/accounts/CAddAcountSession.h"
#include "elastos/droid/server/accounts/CCompleteCloningAccountSession.h"
#include "elastos/droid/server/accounts/CConfirmCredentialsAsUserSession.h"
#include "elastos/droid/server/accounts/CCopyAccountToUserSession.h"
#include "elastos/droid/server/accounts/CEditPropertiesSession.h"
#include "elastos/droid/server/accounts/CGetAccountsByTypeAndFeatureSession.h"
#include "elastos/droid/server/accounts/CGetAuthTokenLabelSession.h"
#include "elastos/droid/server/accounts/CGetAuthTokenSession.h"
#include "elastos/droid/server/accounts/CRemoveAccountSession.h"
#include "elastos/droid/server/accounts/CTestFeaturesSession.h"
#include "elastos/droid/server/accounts/CUpdateCredentialsSession.h"
#include "elastos/droid/server/accounts/GrantCredentialsPermissionActivity.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Accounts.h>
#include <Elastos.Droid.Database.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/R.h>
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/droid/app/AppGlobals.h>
#include <elastos/droid/internal/utility/ArrayUtils.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

// using Elastos::Droid::App::CPendingIntentHelper;
// using Elastos::Droid::Os::CEnvironment;
// using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Accounts::CAccount;
using Elastos::Droid::Accounts::CAccountAndUser;
using Elastos::Droid::Accounts::CAccountAuthenticatorResponse;
using Elastos::Droid::Accounts::CAuthenticatorDescriptionHelper;
using Elastos::Droid::Accounts::ECLSID_CCantAddAccountActivity;
using Elastos::Droid::Accounts::EIID_IAccount;
using Elastos::Droid::Accounts::EIID_IIAccountAuthenticatorResponse;
using Elastos::Droid::Accounts::EIID_IIAccountManager;
using Elastos::Droid::Accounts::ICantAddAccountActivity;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::IAuthenticatorDescriptionHelper;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::EIID_IRegisteredServicesCacheListener;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IRegisteredServicesCacheServiceInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Database::CDatabaseUtils;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IDatabaseUtils;
using Elastos::Droid::Database::Sqlite::EIID_ISQLiteOpenHelper;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::R;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CPair;
using Elastos::Droid::Utility::IPair;
using Elastos::Core::CArrayOf;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::CString;
using Elastos::Core::IArrayOf;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger64;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::CFile;;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::CAtomicReference;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

//=============================================================================
// CAccountManagerService::UserAccounts
//=============================================================================
CAccountManagerService::UserAccounts::UserAccounts(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
    : mUserId(userId)
{
    CHashMap::New((IHashMap**)&mPreviousNameCache);
    synchronized(mCacheLock) {
        String name;
        CAccountManagerService::GetDatabaseName(userId, &name);
        mOpenHelper = new DatabaseHelper(context, userId, name);
    }
}

//=============================================================================
// CAccountManagerService::MessageHandler
//=============================================================================
CAccountManagerService::MessageHandler::MessageHandler(
    /* [in] */ CAccountManagerService* service)
    : mHost(service)
{}

ECode CAccountManagerService::MessageHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    if (what == MESSAGE_TIMED_OUT) {
        Session* session =
            reinterpret_cast<Session*>(obj.Get());
        if (session) {
            session->OnTimedOut();
        }
    }
    else if (what == MESSAGE_COPY_SHARED_ACCOUNT) {
        Int32 arg1, arg2;
        msg->GetArg1(&arg1);
        msg->GetArg2(&arg2);
        Boolean bNoUse;
        mHost->CopyAccountToUser(IAccount::Probe(obj), arg1, arg2, &bNoUse);
    }
    else {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

//=============================================================================
// CAccountManagerService::DatabaseHelper
//=============================================================================
const String CAccountManagerService::DatabaseHelper::TAG("DatabaseHelper");

CAccountManagerService::DatabaseHelper::DatabaseHelper(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [in] */ const String& name)
{
    SQLiteOpenHelper::constructor(context, name,
            NULL, CAccountManagerService::DATABASE_VERSION);
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
            + CAccountManagerService::ACCOUNTS_PREVIOUS_NAME + String(" TEXT, ")
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

    CreateSharedAccountsTable(db);

    CreateAccountsDeletionTrigger(db);

    return NOERROR;
}

void  CAccountManagerService::DatabaseHelper::CreateSharedAccountsTable(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("CREATE TABLE ") + TABLE_SHARED_ACCOUNTS + String(" ( ")
            + ACCOUNTS_ID + String(" INTEGER PRIMARY KEY AUTOINCREMENT, ")
            + ACCOUNTS_NAME + String(" TEXT NOT NULL, ")
            + ACCOUNTS_TYPE + String(" TEXT NOT NULL, ")
            + String("UNIQUE(") + ACCOUNTS_NAME + String(",") + ACCOUNTS_TYPE + String("))"));
}

void CAccountManagerService::DatabaseHelper::AddOldAccountNameColumn(
    /* [in] */ ISQLiteDatabase* db)
{

    db->ExecSQL(String("ALTER TABLE ") + TABLE_ACCOUNTS + String(" ADD COLUMN ") + ACCOUNTS_PREVIOUS_NAME);
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
    Slogger::E(TAG,
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

    if (oldVersion == 4) {
        CreateSharedAccountsTable(db);
        oldVersion++;
    }

    if (oldVersion == 5) {
        AddOldAccountNameColumn(db);
        oldVersion++;
    }

    if (oldVersion != newVersion) {
        Logger::E(TAG, "failed to upgrade version %d to version %d", oldVersion, newVersion);
    }
    return NOERROR;
}

ECode CAccountManagerService::DatabaseHelper::OnOpen(
    /* [in] */ ISQLiteDatabase* db)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) Logger::V(TAG, "opened database %s", DATABASE_NAME.string());
    return NOERROR;
}

//=============================================================================
// CAccountManagerService::PurgeOldGrantsAllBroadcastReceiver
//=============================================================================
CAccountManagerService::PurgeOldGrantsAllBroadcastReceiver::PurgeOldGrantsAllBroadcastReceiver(
    /* [in] */ CAccountManagerService* host)
    : mHost(host)
{}

ECode CAccountManagerService::PurgeOldGrantsAllBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // Don't delete accounts when updating a authenticator's
    // package.
    Boolean b;
    intent->GetBooleanExtra(IIntent::EXTRA_REPLACING, FALSE, &b);
    if (!b) {
        mHost->PurgeOldGrantsAll();
    }
    return NOERROR;
}

//=============================================================================
// CAccountManagerService::OnUserRemovedBroadcastReceiver
//=============================================================================
CAccountManagerService::OnUserRemovedBroadcastReceiver::OnUserRemovedBroadcastReceiver(
    /* [in] */ CAccountManagerService* host)
    : mHost(host)
{}

ECode CAccountManagerService::OnUserRemovedBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_USER_REMOVED.Equals(action)) {
        mHost->OnUserRemoved(intent);
    } else if (IIntent::ACTION_USER_STARTED.Equals(action)) {
        mHost->OnUserStarted(intent);
    }
    return NOERROR;
}

//=============================================================================
// CAccountManagerService::TestFeaturesSession
//=============================================================================
const String CAccountManagerService::TestFeaturesSession::TAG("TestFeaturesSession");

ECode CAccountManagerService::TestFeaturesSession::constructor(
    /* [in] */ IInterface* accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ const ArrayOf<String>& features,
    /* [in] */ IIAccountManager* host)
{
    String type;
    account->GetType(&type);
    FAIL_RETURN(Session::constructor((UserAccounts*) IObject::Probe(accounts), response,
            type, FALSE /* expectActivityLaunch */,
            TRUE /* stripAuthTokenFromResult */, (CAccountManagerService*) host))
    mFeatures = features.Clone();
    mAccount = account;
    return NOERROR;
}

ECode CAccountManagerService::TestFeaturesSession::Run()
{
    // try {
    ECode ec = mAuthenticator->HasFeatures(this,
            mAccount, mFeatures);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            OnError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION, String("remote exception"));
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode CAccountManagerService::TestFeaturesSession::OnResult(
    /* [in] */ IBundle* result)
{
    AutoPtr<IAccountManagerResponse> response = GetResponseAndClose();
    if (response != NULL) {
        // try {
        ECode ec;
        do {
            if (result == NULL) {
                return response->OnError(IAccountManager::ERROR_CODE_INVALID_RESPONSE,
                        String("null bundle"));
            }
            if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                Logger::V(TAG, "CAccountManagerService::TestFeaturesSession calling onResult() on response %s",
                        TO_CSTR(response));
            }
            AutoPtr<IBundle> newResult;
            CBundle::New((IBundle**)&newResult);
            Boolean value;
            ec = result->GetBoolean(IAccountManager::KEY_BOOLEAN_RESULT, FALSE, &value);
            if (FAILED(ec)) break;
            ec = newResult->PutBoolean(IAccountManager::KEY_BOOLEAN_RESULT, value);
            if (FAILED(ec)) break;
            ec = response->OnResult(newResult);
        } while(FALSE);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                // if the caller is dead then there is no one to care about remote exceptions
                if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                    Logger::V(TAG, "failure while notifying response %d", ec);
                }
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

String CAccountManagerService::TestFeaturesSession::ToDebugString(
    /* [in] */ Int64 now)
{
    AutoPtr<IList> list;
    Arrays::AsList(mFeatures, (IList**)&list);
    AutoPtr<ArrayOf<IInterface*> > features;
    list->ToArray((ArrayOf<IInterface*>**)&features);
    return Session::ToDebugString(now) + String(", hasFeatures")
            + String(", ") + TO_STR(mAccount)
            + String(", ") + (mFeatures != NULL ? TextUtils::Join(StringUtils::ParseCharSequence(String(",")), features) : String(NULL));
}

//=============================================================================
// CAccountManagerService::RemoveAccountSession
//=============================================================================
const String CAccountManagerService::RemoveAccountSession::TAG("RemoveAccountSession");

ECode CAccountManagerService::RemoveAccountSession::constructor(
    /* [in] */ IInterface* accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ IIAccountManager* host)
{
    String type;
    account->GetType(&type);
    FAIL_RETURN(Session::constructor((UserAccounts*) IObject::Probe(accounts), response,
            type, FALSE /* expectActivityLaunch */,
            TRUE /* stripAuthTokenFromResult */, (CAccountManagerService*)host))
    mAccount = account;
    return NOERROR;
}

String CAccountManagerService::RemoveAccountSession::ToDebugString(
    /* [in] */ Int64 now)
{
    String accountStr;
    mAccount->ToString(&accountStr);
    return Session::ToDebugString(now) + String(", removeAccount")
            + String(", account ") + accountStr;
}

ECode CAccountManagerService::RemoveAccountSession::Run()
{
    return mAuthenticator->GetAccountRemovalAllowed(
            this, mAccount);
}

ECode CAccountManagerService::RemoveAccountSession::OnResult(
    /* [in] */ IBundle* result)
{
    Boolean res1, res2;
    if (result != NULL &&
            (result->ContainsKey(IAccountManager::KEY_BOOLEAN_RESULT, &res1), res1) &&
            (result->ContainsKey(IAccountManager::KEY_INTENT, &res2), !res2)) {
        Boolean removalAllowed;
        result->GetBoolean(IAccountManager::KEY_BOOLEAN_RESULT, &removalAllowed);
        if (removalAllowed) {
            mHost->RemoveAccountInternal(mAccounts, mAccount);
        }
        AutoPtr<IAccountManagerResponse> response = GetResponseAndClose();
        if (response != NULL) {
            if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                Logger::V(TAG, "CAccountManagerService::RemoveAccountSession calling onResult() on response %s",
                        TO_CSTR(response));
            }
            AutoPtr<IBundle> result2;
            CBundle::New((IBundle**)&result2);
            result2->PutBoolean(IAccountManager::KEY_BOOLEAN_RESULT, removalAllowed);
            // try {
            ECode ec = response->OnResult(result2);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode) E_REMOTE_EXCEPTION == ec) {
                    // ignore
                }
                else
                    return ec;
            }
            // }
        }
    }
    return Session::OnResult(result);
}

//=============================================================================
// CAccountManagerService::GetAuthTokenLabelSession
//=============================================================================
String CAccountManagerService::GetAuthTokenLabelSession::ToDebugString(
    /* [in] */ Int64 now)
{
    return Session::ToDebugString(now)
            + String(", getAuthTokenLabel") + String(", ")
            + mAccountType + String(", authTokenType ") + mAuthTokenType;;
}

ECode CAccountManagerService::GetAuthTokenLabelSession::Run()
{
    return mAuthenticator->GetAuthTokenLabel(this,
            mAuthTokenType);
}

ECode CAccountManagerService::GetAuthTokenLabelSession::OnResult(
    /* [in] */ IBundle* result)
{
    if (result != NULL) {
        String label;
        result->GetString(IAccountManager::KEY_AUTH_TOKEN_LABEL, &label);
        AutoPtr<IBundle> bundle;
        FAIL_RETURN(CBundle::New((IBundle**)&bundle))
        bundle->PutString(IAccountManager::KEY_AUTH_TOKEN_LABEL, label);
        return Session::OnResult(bundle);
    }
    else {
        return Session::OnResult(result);
    }
}

ECode CAccountManagerService::GetAuthTokenLabelSession::constructor(
    /* [in] */ IInterface* accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& type,
    /* [in] */ const String& authTokenType,
    /* [in] */ IIAccountManager* host)
{
    FAIL_RETURN(Session::constructor((UserAccounts*) IObject::Probe(accounts), response,
            type, FALSE, FALSE /* stripAuthTokenFromResult */,
            (CAccountManagerService*)host))
    mAuthTokenType = authTokenType;
    return NOERROR;
}

//=============================================================================
// CAccountManagerService::GetAuthTokenSession
//=============================================================================
CAccountManagerService::GetAuthTokenSession::GetAuthTokenSession()
    : mNotifyOnAuthFailure(FALSE)
    , mPermissionGranted(FALSE)
{}

String CAccountManagerService::GetAuthTokenSession::ToDebugString(
    /* [in] */ Int64 now)
{
    if (mLoginOptions != NULL) {
        AutoPtr<ISet> value;
        mLoginOptions->GetKeySet((ISet**)&value);
    }
    String accountStr, optionStr;
    mAccount->ToString(&accountStr);
    IObject::Probe(mLoginOptions)->ToString(&optionStr);
    return Session::ToDebugString(now) + String(", getAuthToken")
            + String(", ") + accountStr
            + String(", authTokenType ") + mAuthTokenType
            + String(", loginOptions ") + optionStr
            + String(", notifyOnAuthFailure ") + StringUtils::BooleanToString(mNotifyOnAuthFailure);
}

ECode CAccountManagerService::GetAuthTokenSession::Run()
{
    // If the caller doesn't have permission then create and return the
    // "grant permission" intent instead of the "getAuthToken" intent.
    if (!mPermissionGranted) {
        return mAuthenticator->GetAuthTokenLabel(
                this, mAuthTokenType);
    }
    else {
        return mAuthenticator->GetAuthToken(
                this, mAccount,
                mAuthTokenType, mLoginOptions);
    }
}

ECode CAccountManagerService::GetAuthTokenSession::OnResult(
    /* [in] */ IBundle* result)
{
    if (result != NULL) {
        Boolean isContains;
        if (result->ContainsKey(IAccountManager::KEY_AUTH_TOKEN_LABEL, &isContains),
                isContains) {
            AutoPtr<IAccountAuthenticatorResponse> response;
            CAccountAuthenticatorResponse::New(this,
                    (IAccountAuthenticatorResponse**)&response);
            String value;
            result->GetString(IAccountManager::KEY_AUTH_TOKEN_LABEL, &value);
            AutoPtr<IIntent> intent = mHost->NewGrantCredentialsPermissionIntent(mAccount,
                    Binder::GetCallingUid(), response, mAuthTokenType, value);
            AutoPtr<IBundle> bundle;
            CBundle::New((IBundle**)&bundle);
            bundle->PutParcelable(IAccountManager::KEY_INTENT,
                    (IParcelable*)intent->Probe(EIID_IParcelable));
            return OnResult(bundle);
        }
        String authToken;
        result->GetString(IAccountManager::KEY_AUTHTOKEN, &authToken);
        if (!authToken.IsNull()) {
            String name, type;
            result->GetString(IAccountManager::KEY_ACCOUNT_NAME, &name);
            result->GetString(IAccountManager::KEY_ACCOUNT_TYPE, &type);
            AutoPtr<ICharSequence> nameCsq, typeCsq;
            CString::New(name, (ICharSequence**)&nameCsq);
            CString::New(type, (ICharSequence**)&typeCsq);
            if (TextUtils::IsEmpty(typeCsq) || TextUtils::IsEmpty(nameCsq)) {
                return OnError(IAccountManager::ERROR_CODE_INVALID_RESPONSE,
                        String("the type and name should not be empty"));
            }
            if (!mCustomTokens) {
                AutoPtr<IAccount> account;
                CAccount::New(name, type, (IAccount**)&account);
                Boolean bNoUse;
                mHost->SaveAuthTokenToDatabase(mAccounts, mAccount, mAuthTokenType, authToken, &bNoUse);
            }
        }

        AutoPtr<IParcelable> p;
        result->GetParcelable(IAccountManager::KEY_INTENT, (IParcelable**)&p);
        AutoPtr<IIntent> intent = (IIntent*)p->Probe(Elastos::Droid::Content::EIID_IIntent);
        if (intent != NULL && mNotifyOnAuthFailure && !mCustomTokens) {
            String s;
            result->GetString(IAccountManager::KEY_AUTH_FAILED_MESSAGE, &s);
            AutoPtr<ICharSequence> csq;
            CString::New(s, (ICharSequence**)&csq);
            mHost->DoNotification(mAccounts, mAccount, csq, intent, mAccounts->mUserId);
        }
    }
    return Session::OnResult(result);
}

ECode CAccountManagerService::GetAuthTokenSession::constructor(
    /* [in] */ IInterface* accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IBundle* loginOptions,
    /* [in] */ Boolean notifyOnAuthFailure,
    /* [in] */ Boolean permissionGranted,
    /* [in] */ Boolean customTokens,
    /* [in] */ IIAccountManager* host)
{
    String type;
    account->GetType(&type);
    FAIL_RETURN(Session::constructor((UserAccounts*) IObject::Probe(accounts), response,
            type, expectActivityLaunch, FALSE /* stripAuthTokenFromResult */,
            (CAccountManagerService*)host))
    mAccount = account;
    mAuthTokenType = authTokenType;
    mLoginOptions = loginOptions;
    mNotifyOnAuthFailure = notifyOnAuthFailure;
    mPermissionGranted = permissionGranted;
    mCustomTokens = customTokens;
    return NOERROR;
}

//=============================================================================
// CAccountManagerService::AddAcountSession
//=============================================================================
ECode CAccountManagerService::AddAcountSession::Run()
{
    return mAuthenticator->AddAccount(this,
            mAccountType, mAuthTokenType, mRequiredFeatures, mOptions);
}

String CAccountManagerService::AddAcountSession::ToDebugString(
    /* [in] */ Int64 now)
{
    AutoPtr<IList> list;
    Arrays::AsList(mRequiredFeatures, (IList**)&list);
    AutoPtr<ArrayOf<IInterface*> > requiredFeatures;
    list->ToArray((ArrayOf<IInterface*>**)&requiredFeatures);
    return Session::ToDebugString(now) + String(", addAccount")
            + String(", accountType ") + mAccountType
            + String(", requiredFeatures ")
            + (mRequiredFeatures != NULL ? TextUtils::Join(StringUtils::ParseCharSequence(String(",")), requiredFeatures) : String(NULL));
}

ECode CAccountManagerService::AddAcountSession::constructor(
    /* [in] */ IInterface* accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& accountType,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ const String& authTokenType,
    /* [in] */ const ArrayOf<String>& requiredFeatures,
    /* [in] */ IBundle* options,
    /* [in] */ IIAccountManager* host)
{
    FAIL_RETURN(Session::constructor((UserAccounts*) IObject::Probe(accounts), response,
            accountType, expectActivityLaunch, TRUE /* stripAuthTokenFromResult */,
            (CAccountManagerService*)host))
    mAuthTokenType = authTokenType;
    mRequiredFeatures = requiredFeatures.Clone();
    mOptions = options;
    return NOERROR;
}

//=============================================================================
// CAccountManagerService::ConfirmCredentialsAsUserSession
//=============================================================================
ECode CAccountManagerService::ConfirmCredentialsAsUserSession::Run()
{
    return mAuthenticator->ConfirmCredentials(this,
            mAccount, mOptions);
}

String CAccountManagerService::ConfirmCredentialsAsUserSession::ToDebugString(
    /* [in] */ Int64 now)
{
    String accountStr;
    mAccount->ToString(&accountStr);
    return Session::ToDebugString(now) + String(", confirmCredentials")
            + String(", ") + accountStr;
}

ECode CAccountManagerService::ConfirmCredentialsAsUserSession::constructor(
    /* [in] */ IInterface* accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IAccount* account,
    /* [in] */ IBundle* options,
    /* [in] */ IIAccountManager* host)
{
    String type;
    account->GetType(&type);
    FAIL_RETURN(Session::constructor((UserAccounts*) IObject::Probe(accounts), response,
            type, expectActivityLaunch, TRUE /* stripAuthTokenFromResult */,
            (CAccountManagerService*)host))
    mAccount = account;
    mOptions = options;
    return NOERROR;
}

//=============================================================================
// CAccountManagerService::UpdateCredentialsSession
//=============================================================================
ECode CAccountManagerService::UpdateCredentialsSession::Run()
{
    return mAuthenticator->UpdateCredentials(this,
            mAccount, mAuthTokenType, mLoginOptions);
}

String CAccountManagerService::UpdateCredentialsSession::ToDebugString(
    /* [in] */ Int64 now)
{
    if (mLoginOptions != NULL) {
        AutoPtr<ISet> value;
        mLoginOptions->GetKeySet((ISet**)&value);
    }
    String accountStr, optionStr;
    mAccount->ToString(&accountStr);
    IObject::Probe(mLoginOptions)->ToString(&optionStr);
    return Session::ToDebugString(now) + String(", updateCredentials")
            + String(", ") + accountStr
            + String(", authTokenType ") + mAuthTokenType
            + String(", loginOptions ") + optionStr;
}

ECode CAccountManagerService::UpdateCredentialsSession::constructor(
    /* [in] */ IInterface* accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ IBundle* loginOptions,
    /* [in] */ IIAccountManager* host)
{
    String type;
    account->GetType(&type);
    FAIL_RETURN(Session::constructor((UserAccounts*) IObject::Probe(accounts), response,
            type, expectActivityLaunch, TRUE /* stripAuthTokenFromResult */,
            (CAccountManagerService*)host))
    mAccount = account;
    mAuthTokenType = authTokenType;
    mLoginOptions = loginOptions;
    return NOERROR;
}

//=============================================================================
// CAccountManagerService::EditPropertiesSession
//=============================================================================
ECode CAccountManagerService::EditPropertiesSession::Run()
{
    return mAuthenticator->EditProperties(this,
            mAccountType);
}

String CAccountManagerService::EditPropertiesSession::ToDebugString(
    /* [in] */ Int64 now)
{
    return Session::ToDebugString(now) + String(", editProperties")
            + String(", accountType") + mAccountType;
}

ECode CAccountManagerService::EditPropertiesSession::constructor(
    /* [in] */ IInterface* accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String accountType,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IIAccountManager* host)
{
    return Session::constructor((UserAccounts*) IObject::Probe(accounts), response,
            accountType, expectActivityLaunch, TRUE /* stripAuthTokenFromResult */,
            (CAccountManagerService*) host);
}

//=============================================================================
// CAccountManagerService::GetAccountsByTypeAndFeatureSession
//=============================================================================
const String CAccountManagerService::GetAccountsByTypeAndFeatureSession::TAG("GetAccountsByTypeAndFeatureSession");

CAccountManagerService::GetAccountsByTypeAndFeatureSession::GetAccountsByTypeAndFeatureSession()
    : mCurrentAccount(0)
{}

ECode CAccountManagerService::GetAccountsByTypeAndFeatureSession::Run()
{
    synchronized(mAccounts->mCacheLock) {
        mHost->GetAccountsFromCacheLocked(mAccounts, mAccountType, mCallingUid,
                        String(NULL), (ArrayOf<IAccount*>**)&mAccountsOfType);
    }
    // check whether each account matches the requested features
    mAccountsWithFeatures = NULL;
    CArrayList::New(mAccountsOfType->GetLength(), (IList**)&mAccountsWithFeatures);
    mCurrentAccount = 0;

    return CheckAccount();
}

ECode CAccountManagerService::GetAccountsByTypeAndFeatureSession::CheckAccount()
{
    if (mCurrentAccount >= mAccountsOfType->GetLength()) {
        return SendResult();
    }

    AutoPtr<IIAccountAuthenticator> accountAuthenticator = mAuthenticator;
    if (accountAuthenticator == NULL) {
        // It is possible that the authenticator has died, which is indicated by
        // mAuthenticator being set to null. If this happens then just abort.
        // There is no need to send back a result or error in this case since
        // that already happened when mAuthenticator was cleared.
        if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::V(TAG, "checkAccount: aborting session since we are no longer"
                    " connected to the authenticator, %s", Session::ToDebugString().string());
        }
        return NOERROR;
    }
    // try {
    ECode ec = accountAuthenticator->HasFeatures(this,
            (*mAccountsOfType)[mCurrentAccount], mFeatures);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            OnError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION, String("remote exception"));
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode CAccountManagerService::GetAccountsByTypeAndFeatureSession::OnResult(
    /* [in] */ IBundle* result)
{
    mNumResults++;
    if (result == NULL) {
        return OnError(IAccountManager::ERROR_CODE_INVALID_RESPONSE, String("null bundle"));
    }
    Boolean value;
    if (result->GetBoolean(IAccountManager::KEY_BOOLEAN_RESULT, FALSE, &value)) {
        mAccountsWithFeatures->Add((*mAccountsOfType)[mCurrentAccount]);
    }
    mCurrentAccount++;
    return CheckAccount();
}

ECode CAccountManagerService::GetAccountsByTypeAndFeatureSession::SendResult()
{
    AutoPtr<IAccountManagerResponse> response = GetResponseAndClose();
    if (response != NULL) {
        // try {
        ECode ec;
        do {
            Int32 size;
            mAccountsWithFeatures->GetSize(&size);
            AutoPtr<ArrayOf<IAccount*> > accounts = ArrayOf<IAccount*>::Alloc(size);
            for (Int32 i = 0; i < accounts->GetLength(); ++i) {
                AutoPtr<IInterface> obj;
                mAccountsWithFeatures->Get(i, (IInterface**)&obj);
                accounts->Set(i, IAccount::Probe(obj));
            }
            if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                Logger::V(TAG, "CAccountManagerService::GetAccountsByTypeAndFeatureSession calling onResult() on response "
                        , TO_CSTR(response));
            }
            AutoPtr<IBundle> result;
            ec = CBundle::New((IBundle**)&result);
            if (FAILED(ec)) break;
            AutoPtr<ArrayOf<IParcelable*> > parcels = ArrayOf<IParcelable*>::Alloc(accounts->GetLength());
            for (Int32 i = 0; i < accounts->GetLength(); ++i)
                parcels->Set(i, IParcelable::Probe((*accounts)[i]));
            ec = result->PutParcelableArray(IAccountManager::KEY_ACCOUNTS, parcels);
            if (FAILED(ec)) break;
            ec = response->OnResult(result);
        } while(FALSE);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                // if the caller is dead then there is no one to care about remote exceptions
                if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                    Logger::V(TAG, "failure while notifying response %d", ec);
                }
            }
        }
        // }
    }
    return NOERROR;
}

String CAccountManagerService::GetAccountsByTypeAndFeatureSession::ToDebugString(
    /* [in] */ Int64 now)
{
    AutoPtr<ICharSequence> cs;
    CString::New(String(","), (ICharSequence**)&cs);
    AutoPtr<IList> list;
    Arrays::AsList(mFeatures, (IList**)&list);
    AutoPtr<ArrayOf<IInterface*> > features;
    list->ToArray((ArrayOf<IInterface*>**)&features);
    return Session::ToDebugString(now)
            + String(", getAccountsByTypeAndFeatures") + String(", ")
            + (mFeatures != NULL ? TextUtils::Join(cs, features) : String(NULL));
}

ECode CAccountManagerService::GetAccountsByTypeAndFeatureSession::constructor(
    /* [in] */ IInterface* accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& type,
    /* [in] */ const ArrayOf<String>& features,
    /* [in] */ Int32 callingUid,
    /* [in] */ IIAccountManager* host)
{
    FAIL_RETURN(Session::constructor((UserAccounts*) IObject::Probe(accounts), response,
            type, FALSE /* expectActivityLaunch */,
            TRUE /* stripAuthTokenFromResult */, (CAccountManagerService*)host))
    mFeatures = features.Clone();
    mCallingUid = callingUid;
    return NOERROR;
}

//=============================================================================
// CAccountManagerService::Session
//=============================================================================
CAR_INTERFACE_IMPL_4(CAccountManagerService::Session, Object, IIAccountAuthenticatorResponse, IBinder, IProxyDeathRecipient, IServiceConnection)

const String CAccountManagerService::Session::TAG("AccountManagerServiceSession");

CAccountManagerService::Session::Session()
    : mExpectActivityLaunch(FALSE)
    , mCreationTime(0)
    , mNumResults(0)
    , mNumRequestContinued(0)
    , mNumErrors(0)
    , mStripAuthTokenFromResult(FALSE)
{}

AutoPtr<IAccountManagerResponse> CAccountManagerService::Session::GetResponseAndClose()
{
    if (mResponse == NULL) {
        // this session has already been closed
        return NULL;
    }
    AutoPtr<IAccountManagerResponse> response = mResponse;
    Close(); // this clears mResponse so we need to save the response before this call
    return response;
}

void CAccountManagerService::Session::Close()
{
    synchronized(mHost->mSessions) {
        AutoPtr<IInterface> obj;
        mHost->mSessions->Remove(StringUtils::ParseCharSequence(ToString()), (IInterface**)&obj);
        if (obj == NULL) {
            // the session was already closed, so bail out now
            return;
        }
    }

    if (mResponse != NULL) {
        // stop listening for response deaths
        AutoPtr<IProxy> proxy = (IProxy*)mResponse->Probe(EIID_IProxy);
        assert(proxy != NULL);
        Boolean res;
        proxy->UnlinkToDeath(
            this, 0/* flags */, &res);

        // clear this so that we don't accidentally send any further results
        mResponse = NULL;
    }
    CancelTimeout();
    Unbind();
}

ECode CAccountManagerService::Session::ProxyDied()
{
    PFL_EX("ProxyDied()");
    mResponse = NULL;
    Close();
    return NOERROR;
}

String CAccountManagerService::Session::ToDebugString()
{
    return ToDebugString(SystemClock::GetElapsedRealtime());
}

String CAccountManagerService::Session::ToDebugString(
    /* [in] */ Int64 now)
{
    return String("Session: expectLaunch ") + StringUtils::BooleanToString(mExpectActivityLaunch)
            + String(", connected ") + StringUtils::BooleanToString(mAuthenticator != NULL)
            + String(", stats (") + StringUtils::ToString(mNumResults) + String("/")
            + StringUtils::ToString(mNumRequestContinued) + String("/")
            + StringUtils::ToString(mNumErrors) + String(")")
            + String(", lifetime ") + StringUtils::ToString((now - mCreationTime) / 1000.0);
}

ECode CAccountManagerService::Session::Bind()
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "initiating bind to authenticator type %s", mAccountType.string());
    }
    if (!BindToAuthenticator(mAccountType)) {
        Slogger::D(TAG, "bind attempt failed for %s", ToDebugString().string());
        return OnError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION,
                String("bind failure"));
    }
    return NOERROR;
}

void CAccountManagerService::Session::Unbind()
{
    if (mAuthenticator != NULL) {
        mAuthenticator = NULL;
        mHost->mContext->UnbindService(this);
    }
}

void CAccountManagerService::Session::ScheduleTimeout()
{
    AutoPtr<IMessage> msg;
    mHost->mMessageHandler->ObtainMessage(
        CAccountManagerService::MESSAGE_TIMED_OUT,
        TO_IINTERFACE(this), (IMessage**)&msg);
    Boolean result;
    mHost->mMessageHandler->SendMessageDelayed(msg,
        CAccountManagerService::TIMEOUT_DELAY_MS, &result);
}

void CAccountManagerService::Session::CancelTimeout()
{
    mHost->mMessageHandler->RemoveMessages(
        CAccountManagerService::MESSAGE_TIMED_OUT, TO_IINTERFACE(this));
}

ECode CAccountManagerService::Session::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    mAuthenticator = IIAccountAuthenticator::Probe(service);
    // try {
    ECode ec = Run();
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            OnError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION,
                    String("remote failure"));
        }
        else
            return ec;
    }
    return NOERROR;
    // }
}

ECode CAccountManagerService::Session::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    mAuthenticator = NULL;
    AutoPtr<IAccountManagerResponse> response = GetResponseAndClose();
    if (response != NULL) {
        // try {
        ECode ec = response->OnError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION,
                String("disconnected"));
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                    Logger::V(TAG, "Session.onServiceDisconnected: "
                            "caught RemoteException while responding %d", ec);
                }
                ec = NOERROR;
            }
        }
        return ec;
        // }
    }
    return NOERROR;
}

ECode CAccountManagerService::Session::OnTimedOut()
{
    AutoPtr<IAccountManagerResponse> response = GetResponseAndClose();
    if (response != NULL) {
        // try {
        ECode ec = response->OnError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION,
                String("timeout"));
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                    Logger::V(TAG, "Session.onTimedOut: caught RemoteException while responding %d",
                            ec);
                }
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode CAccountManagerService::Session::OnResult(
    /* [in] */ IBundle* result)
{
    mNumResults++;
    AutoPtr<IIntent> intent;
    AutoPtr<IParcelable> parcelable;
    result->GetParcelable(IAccountManager::KEY_INTENT, (IParcelable**)&parcelable);
    if (result != NULL
            && (intent = IIntent::Probe(parcelable)) != NULL) {
        /*
         * The Authenticator API allows third party authenticators to
         * supply arbitrary intents to other apps that they can run,
         * this can be very bad when those apps are in the system like
         * the System Settings.
         */
        Int32 authenticatorUid = Binder::GetCallingUid();
        Int64 bid = Binder::ClearCallingIdentity();
        // try {
        ECode ec;
        do {
            AutoPtr<IPackageManager> pm;
            ec = mHost->mContext->GetPackageManager((IPackageManager**)&pm);
            if (FAILED(ec)) break;
            AutoPtr<IResolveInfo> resolveInfo;
            ec = pm->ResolveActivityAsUser(intent, 0, mAccounts->mUserId, (IResolveInfo**)&resolveInfo);
            if (FAILED(ec)) break;
            AutoPtr<IActivityInfo> activityInfo;
            ec = resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
            if (FAILED(ec)) break;
            AutoPtr<IApplicationInfo> applicationInfo;
            ec = IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
            if (FAILED(ec)) break;
            Int32 targetUid;
            ec = applicationInfo->GetUid(&targetUid);
            if (FAILED(ec)) break;
            Int32 signCode;
            ec = pm->CheckSignatures(authenticatorUid, targetUid, &signCode);
            if (FAILED(ec)) break;
            if (IPackageManager::SIGNATURE_MATCH != signCode) {
                Logger::E(TAG, "Activity to be started with KEY_INTENT must share Authenticator's signatures");
                return E_SECURITY_EXCEPTION;
            }
        } while(FALSE);
        // } finally {
        Binder::RestoreCallingIdentity(bid);
        if (FAILED(ec)) return ec;
        // }
    }
    if (result != NULL) {
        String s;
        result->GetString(IAccountManager::KEY_AUTHTOKEN, &s);
        AutoPtr<ICharSequence> cs;
        CString::New(s, (ICharSequence**)&cs);
        if (!TextUtils::IsEmpty(cs)) {
            String accountName, accountType;
            result->GetString(IAccountManager::KEY_ACCOUNT_NAME, &accountName);
            result->GetString(IAccountManager::KEY_ACCOUNT_TYPE, &accountType);
            AutoPtr<ICharSequence> accountNameCs, accountTypeCs;
            CString::New(accountName, (ICharSequence**)&accountNameCs);
            CString::New(accountType, (ICharSequence**)&accountTypeCs);
            if (!TextUtils::IsEmpty(accountNameCs) && !TextUtils::IsEmpty(accountTypeCs)) {
                AutoPtr<IAccount> account;
                FAIL_RETURN(CAccount::New(accountName, accountType, (IAccount**)&account))
                AutoPtr<IUserHandle> userH;
                FAIL_RETURN(CUserHandle::New(mAccounts->mUserId, (IUserHandle**)&userH))
                mHost->CancelNotification(
                        mHost->GetSigninRequiredNotificationId(mAccounts, account), userH);
            }
        }
    }

    AutoPtr<IAccountManagerResponse> response;
    Boolean isContains = FALSE;
    if (mExpectActivityLaunch && result != NULL
            && (result->ContainsKey(IAccountManager::KEY_INTENT, &isContains), isContains)) {
        response = mResponse;
    }
    else {
        response = GetResponseAndClose();
    }
    if (response != NULL) {
        // try {
        ECode ec;
        do {
            if (result == NULL) {
                if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                    Logger::V(TAG, "CAccountManagerService::Session"
                            " calling onError() on response %s", TO_CSTR(response));
                }
                ec = response->OnError(IAccountManager::ERROR_CODE_INVALID_RESPONSE,
                        String("null bundle returned"));
                if (FAILED(ec)) break;
            }
            else {
                if (mStripAuthTokenFromResult) {
                    ec = result->Remove(IAccountManager::KEY_AUTHTOKEN);
                    if (FAILED(ec)) break;
                }
                if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                    Logger::V(TAG, "CAccountManagerService::Session"
                            " calling onResult() on response %s", TO_CSTR(response));
                }
                Int32 keyErrCode;
                result->GetInt32(IAccountManager::KEY_ERROR_CODE, -1, &keyErrCode);
                if ((keyErrCode > 0) &&
                        (intent == NULL)) {
                    // All AccountManager error codes are greater than 0
                    result->GetInt32(IAccountManager::KEY_ERROR_CODE, &keyErrCode);
                    String keyErrMsg;
                    result->GetString(IAccountManager::KEY_ERROR_MESSAGE, &keyErrMsg);
                    response->OnError(keyErrCode, keyErrMsg);
                } else {
                    response->OnResult(result);
                }
            }
        } while(FALSE);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                // if the caller is dead then there is no one to care about remote exceptions
                if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                    Logger::V(TAG, "failure while notifying response %d", ec);
                }
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode CAccountManagerService::Session::OnRequestContinued()
{
    mNumRequestContinued++;
    return NOERROR;
}

ECode CAccountManagerService::Session::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    mNumErrors++;
    AutoPtr<IAccountManagerResponse> response = GetResponseAndClose();
    if (response != NULL) {
        if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::V(TAG, "CAccountManagerService::Session"
                    " calling onError() on response ", TO_CSTR(response));
        }
        // try {
        ECode ec = response->OnError(errorCode, errorMessage);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                    Logger::V(TAG, "Session.onError: caught RemoteException while responding %d", ec);
                }
            }
            else
                return ec;
        }
        // }
    }
    else {
        if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::V(TAG, "Session.onError: already closed");
        }
    }

    return NOERROR;
}

Boolean CAccountManagerService::Session::BindToAuthenticator(
    /* [in] */ const String& authenticatorType)
{
    AutoPtr<IAuthenticatorDescription> value;
    AutoPtr<IAuthenticatorDescriptionHelper> helper;
    FAIL_RETURN(CAuthenticatorDescriptionHelper::AcquireSingleton(
            (IAuthenticatorDescriptionHelper**)&helper));
    helper->NewKey(authenticatorType, (IAuthenticatorDescription**)&value);
    AutoPtr<IRegisteredServicesCacheServiceInfo> authenticatorInfo;
    mHost->mAuthenticatorCache->GetServiceInfo(value, mAccounts->mUserId, (IRegisteredServicesCacheServiceInfo**)&authenticatorInfo);
    if (authenticatorInfo == NULL) {
        if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::V(TAG, "there is no authenticator for %s, bailing out",
                    authenticatorType.string());
        }
        return FALSE;
    }

    AutoPtr<IIntent> intent;
    FAIL_RETURN(CIntent::New((IIntent**)&intent))
    intent->SetAction(IAccountManager::ACTION_AUTHENTICATOR_INTENT);
    AutoPtr<IComponentName> componentName;
    authenticatorInfo->GetComponentName((IComponentName**)&componentName);
    intent->SetComponent(componentName);
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "performing bindService to %s", TO_CSTR(componentName));
    }
    Boolean result = FALSE;
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(mAccounts->mUserId, (IUserHandle**)&userHandle);
    if (mHost->mContext->BindServiceAsUser(intent, this, IContext::BIND_AUTO_CREATE,
            userHandle , &result), !result) {
        if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::V(TAG, "bindService to %s failed", TO_CSTR(componentName));
        }
        return FALSE;
    }

    return TRUE;
}

String CAccountManagerService::Session::ToString()
{
    return TAG + StringUtils::ToString(reinterpret_cast<Int64>(this));
}

ECode CAccountManagerService::Session::constructor(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& accountType,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ Boolean stripAuthTokenFromResult,
    /* [in] */ CAccountManagerService* host)
{
    // super();
    if (response == NULL) {
        Logger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mAccounts = accounts;
    mStripAuthTokenFromResult = stripAuthTokenFromResult;
    mResponse = response;
    mAccountType = accountType;
    mExpectActivityLaunch = expectActivityLaunch;
    mCreationTime = SystemClock::GetElapsedRealtime();
    mHost = host;
    synchronized(mHost->mSessions) {
        mHost->mSessions->Put(StringUtils::ParseCharSequence(ToString()), TO_IINTERFACE(this));
    }
    if (response != NULL) {
        AutoPtr<IProxy> proxy = (IProxy*)response->Probe(EIID_IProxy);
        assert(proxy != NULL);
        ECode ec = proxy->LinkToDeath(this, 0/* flags */);
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                mResponse = NULL;
                ProxyDied();
            }
            else
                return ec;
        }
    }

    return NOERROR;
}

ECode CAccountManagerService::Session::ToString(
    /* [in] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = ToString();
    return NOERROR;
}

//=============================================================================
// CAccountManagerService::CopyAccountToUserSession
//=============================================================================
ECode CAccountManagerService::CopyAccountToUserSession::constructor(
    /* [in] */ IInterface* accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ Boolean stripAuthTokenFromResult,
    /* [in] */ IIAccountManager* host,
    /* [in] */ IInterface* toAccounts)
{
    mAccount = account;
    mToAccounts = (UserAccounts*) IObject::Probe(toAccounts);
    String type;
    mAccount->GetType(&type);
    return Session::constructor((UserAccounts*) IObject::Probe(accounts), response, type, expectActivityLaunch, stripAuthTokenFromResult, (CAccountManagerService*) host);
}

ECode CAccountManagerService::CopyAccountToUserSession::Run()
{
    return mAuthenticator->GetAccountCredentialsForCloning(this, mAccount);
}

ECode CAccountManagerService::CopyAccountToUserSession::OnResult(
    /* [in] */ IBundle* result)
{
    if (result != NULL) {
        Boolean isOk;
        result->GetBoolean(IAccountManager::KEY_BOOLEAN_RESULT, FALSE, &isOk);
        if (isOk) {
            // Create a Session for the target user and pass in the bundle
            mHost->CompleteCloningAccount(result, mAccount, mToAccounts);
        }
        return NOERROR;
    } else {
        return Session::OnResult(result);
    }
    return NOERROR;
}

String CAccountManagerService::CopyAccountToUserSession::ToDebugString(
    /* [in] */ Int64 now)
{
    String type;
    mAccount->GetType(&type);
    return Session::ToDebugString(now) + String(", getAccountCredentialsForClone")
            + String(", ") + type;
}

//=============================================================================
// CAccountManagerService::CompleteCloningAccountSession
//=============================================================================
ECode CAccountManagerService::CompleteCloningAccountSession::constructor(
    /* [in] */ IInterface* accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ Boolean stripAuthTokenFromResult,
    /* [in] */ IIAccountManager* host,
    /* [in] */ IBundle* result)
{
    mAccount = account;
    mResult = result;
    String type;
    mAccount->GetType(&type);
    return Session::constructor((UserAccounts*) IObject::Probe(accounts), response, type, expectActivityLaunch, stripAuthTokenFromResult, (CAccountManagerService*) host);
}

ECode CAccountManagerService::CompleteCloningAccountSession::Run()
{
    // Confirm that the owner's account still exists before this step.
    AutoPtr<UserAccounts> owner = mHost->GetUserAccounts(IUserHandle::USER_OWNER);
    synchronized(owner->mCacheLock) {
        AutoPtr<ArrayOf<IAccount*> > ownerAccounts;
        mHost->GetAccounts(IUserHandle::USER_OWNER, (ArrayOf<IAccount*>**)&ownerAccounts);
        AutoPtr<IIterator> it;
        for (Int32 i = 0; i < ownerAccounts->GetLength(); ++i) {
            AutoPtr<IAccount> acc = (*ownerAccounts)[i];
            Boolean isEquals;
            IObject::Probe(acc)->Equals(mAccount, &isEquals);
            if (isEquals) {
                FAIL_RETURN(mAuthenticator->AddAccountFromCredentials(this, mAccount, mResult))
                break;
            }
        }
    }
    return NOERROR;
}

ECode CAccountManagerService::CompleteCloningAccountSession::OnResult(
    /* [in] */ IBundle* result)
{
    if (result != NULL) {
        Boolean isOk;
        result->GetBoolean(IAccountManager::KEY_BOOLEAN_RESULT, FALSE, &isOk);
        if (isOk) {
            // TODO: Anything?
        } else {
            // TODO: Show error notification
            // TODO: Should we remove the shadow account to avoid retries?
        }
        return NOERROR;
    } else {
        return Session::OnResult(result);
    }
    return NOERROR;
}

ECode CAccountManagerService::CompleteCloningAccountSession::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    return Session::OnError(errorCode,  errorMessage);
    // TODO: Show error notification to user
    // TODO: Should we remove the shadow account so that it doesn't keep trying?
}

String CAccountManagerService::CompleteCloningAccountSession::ToDebugString(
    /* [in] */ Int64 now)
{
    String type;
    mAccount->GetType(&type);
    return Session::ToDebugString(now) + String(", getAccountCredentialsForClone")
            + String(", ") + type;
}

//=============================================================================
// CAccountManagerService::AddAccountAsUserSession
//=============================================================================
ECode CAccountManagerService::AddAccountAsUserSession::constructor(
    /* [in] */ IInterface* accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& accountType,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ Boolean stripAuthTokenFromResult,
    /* [in] */ IIAccountManager* host,
    /* [in] */ const String& authTokenType,
    /* [in] */ ArrayOf<String>* requiredFeatures,
    /* [in] */ IBundle* options)
{
    mAuthTokenType = authTokenType;
    mRequiredFeatures = requiredFeatures;
    mOptions = options;
    return Session::constructor((UserAccounts*) IObject::Probe(accounts), response, accountType, expectActivityLaunch, stripAuthTokenFromResult, (CAccountManagerService*) host);
}

ECode CAccountManagerService::AddAccountAsUserSession::Run()
{
    return mAuthenticator->AddAccount(this, mAccountType, mAuthTokenType, mRequiredFeatures,
            mOptions);
}

String CAccountManagerService::AddAccountAsUserSession::ToDebugString(
    /* [in] */ Int64 now)
{
    AutoPtr<IList> list;
    Arrays::AsList(mRequiredFeatures, (IList**)&list);
    AutoPtr<ArrayOf<IInterface*> > requiredFeatures;
    list->ToArray((ArrayOf<IInterface*>**)&requiredFeatures);
    return Session::ToDebugString(now) + String(", addAccount")
            + String(", accountType ") + mAccountType
            + String(", requiredFeatures ")
            + (mRequiredFeatures != NULL
              ? TextUtils::Join(StringUtils::ParseCharSequence(String(",")), requiredFeatures)
              : String(NULL));
}

//=============================================================================
// CAccountManagerService
//=============================================================================
const String CAccountManagerService::TAG("AccountManagerService");
const Int32 CAccountManagerService::TIMEOUT_DELAY_MS = 1000 * 60;
const String CAccountManagerService::DATABASE_NAME("accounts.db");
const Int32 CAccountManagerService::DATABASE_VERSION = 6;
const Int32 CAccountManagerService::MESSAGE_TIMED_OUT = 3;
const Int32 CAccountManagerService::MESSAGE_COPY_SHARED_ACCOUNT = 4;
const String CAccountManagerService::TABLE_ACCOUNTS("accounts");
const String CAccountManagerService::ACCOUNTS_ID("_id");
const String CAccountManagerService::ACCOUNTS_NAME("name");
const String CAccountManagerService::ACCOUNTS_TYPE("type");
const String CAccountManagerService::ACCOUNTS_TYPE_COUNT("count(type)");
const String CAccountManagerService::ACCOUNTS_PASSWORD("password");
const String CAccountManagerService::ACCOUNTS_PREVIOUS_NAME("previous_name");
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
const String CAccountManagerService::TABLE_SHARED_ACCOUNTS("shared_accounts");
const String CAccountManagerService::ACCOUNT_NAME_TYPE_PROJECTION[] =
        { ACCOUNTS_ID, ACCOUNTS_NAME, ACCOUNTS_TYPE };
const String CAccountManagerService::ACCOUNT_TYPE_COUNT_PROJECTION[] =
        { ACCOUNTS_TYPE, ACCOUNTS_TYPE_COUNT };

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
AutoPtr<ArrayOf<IAccount*> > CAccountManagerService::EMPTY_ACCOUNT_ARRAY =
        ArrayOf<IAccount*>::Alloc(0);
AutoPtr<IAtomicReference> CAccountManagerService::sThis = InitAtomicRef();

CAR_OBJECT_IMPL(CAccountManagerService)

CAR_INTERFACE_IMPL_3(CAccountManagerService, Object, IBinder, IIAccountManager, IRegisteredServicesCacheListener)

CAccountManagerService::CAccountManagerService()
{
    CHashMap::New((IHashMap**)&mSessions);

    CAtomicInteger32::New(1, (IAtomicInteger32**)&mNotificationIds);
}

AutoPtr<CAccountManagerService> CAccountManagerService::GetSingleton()
{
    AutoPtr<CAccountManagerService> rev;
    AutoPtr<IInterface> obj;
    sThis->Get((IInterface**)&obj);
    rev = (CAccountManagerService*) IObject::Probe(obj);
    return rev;
}

ECode CAccountManagerService::constructor(
    /* [in] */ IContext* context)
{
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIAccountAuthenticatorCache> cache = (IIAccountAuthenticatorCache*) new AccountAuthenticatorCache();
    ((AccountAuthenticatorCache*) cache.Get())->constructor(context);
    return constructor(context, pm, cache);
}

ECode CAccountManagerService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ IIAccountAuthenticatorCache* authenticatorCache)
{
    mContext = context;
    mPackageManager = packageManager;

    AutoPtr<ILooper> looper;
    mMessageThread->GetLooper((ILooper**)&looper);
    mMessageHandler = new MessageHandler(this);

    mAuthenticatorCache = (IIAccountAuthenticatorCache*)authenticatorCache;
    mAuthenticatorCache->SetListener(this,
            NULL /* Handler */);

    sThis->Set(TO_IINTERFACE(this));

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
    userFilter->AddAction(IIntent::ACTION_USER_STARTED);
    AutoPtr<IIntent> intent;
    AutoPtr<IBroadcastReceiver> br2= (IBroadcastReceiver*)new OnUserRemovedBroadcastReceiver(this);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> all;
    helper->GetALL((IUserHandle**)&all);
    mContext->RegisterReceiverAsUser(br2, all, userFilter, String(NULL), NULL, (IIntent**)&intent);

    return NOERROR;
}

void CAccountManagerService::SystemReady()
{}

AutoPtr<IUserManager> CAccountManagerService::GetUserManager()
{
    if (mUserManager == NULL) {
        AutoPtr<IUserManagerHelper> helper;
        CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
        helper->Get(mContext, (IUserManager**)&mUserManager);
    }
    return mUserManager;
}

AutoPtr<CAccountManagerService::UserAccounts> CAccountManagerService::InitUserLocked(
    /* [in] */ Int32 userId)
{
    AutoPtr<IInterface> obj;
    mUsers->Get(userId, (IInterface**)&obj);
    AutoPtr<UserAccounts> accounts = (UserAccounts*) IObject::Probe(obj);
    if (accounts == NULL) {
        accounts = new UserAccounts(mContext, userId);
        mUsers->Append(userId, TO_IINTERFACE(accounts));
        PurgeOldGrants(accounts);
        ValidateAccountsInternal(accounts, TRUE /* invalidateAuthenticatorCache */);
    }
    return accounts;
}

void CAccountManagerService::PurgeOldGrantsAll()
{
    synchronized(mUsers) {
        Int32 size;
        mUsers->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            mUsers->ValueAt(i, (IInterface**)&obj);
            PurgeOldGrants((UserAccounts*) IObject::Probe(obj));
        }
    }
}

ECode CAccountManagerService::PurgeOldGrants(
    /* [in] */ UserAccounts* accounts)
{
    synchronized(accounts->mCacheLock) {
        AutoPtr<ISQLiteDatabase> db;
        FAIL_RETURN(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db))
        AutoPtr<ICursor> cursor;
        AutoPtr<ArrayOf<String> > ids = ArrayOf<String>::Alloc(1);
        (*ids)[0] = GRANTS_GRANTEE_UID;
        String nullStr;
        FAIL_RETURN(db->Query(TABLE_GRANTS, ids, nullStr, NULL,
                GRANTS_GRANTEE_UID, nullStr, nullStr, (ICursor**)&cursor))
        // try {
        ECode ec = NOERROR;
        Boolean result;
        while (cursor->MoveToNext(&result), result) {
            Int32 uid;
            ec = cursor->GetInt32(0, &uid);
            if (FAILED(ec)) break;
            AutoPtr<ArrayOf<String> > pkgs;
            ec = mPackageManager->GetPackagesForUid(uid, (ArrayOf<String>**)&pkgs);
            if (FAILED(ec)) break;
            Boolean packageExists = pkgs != NULL;
            if (packageExists) {
                continue;
            }
            Slogger::D(TAG, "deleting grants for UID %d because its package is no longer installed", uid);
            (*ids)[0] = StringUtils::ToString(uid);
            Int32 res;
            ec = db->Delete(TABLE_GRANTS, GRANTS_GRANTEE_UID + String("=?"), ids, &res);
            if (FAILED(ec)) break;
        }
        ICloseable::Probe(cursor)->Close();
        return ec;
    }
    return NOERROR;
}

void CAccountManagerService::ValidateAccounts(
    /* [in] */ Int32 userId)
{
    AutoPtr<UserAccounts> accounts = GetUserAccounts(userId);

    // Invalidate user-specific cache to make sure we catch any
    // removed authenticators.
    ValidateAccountsInternal(accounts, TRUE /* invalidateAuthenticatorCache */);
}

ECode CAccountManagerService::ValidateAccountsInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ Boolean invalidateAuthenticatorCache)
{
    if (invalidateAuthenticatorCache) {
        mAuthenticatorCache->InvalidateCache(accounts->mUserId);
    }

    AutoPtr<IHashSet> knownAuth;
    AutoPtr<ICollection> services;
    mAuthenticatorCache->GetAllServices(accounts->mUserId, (ICollection**)&services);
    AutoPtr<IIterator> it;
    services->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IRegisteredServicesCacheServiceInfo> service = IRegisteredServicesCacheServiceInfo::Probe(obj);
        obj = NULL;
        service->GetType((IInterface**)&obj);
        knownAuth->Add(obj);
    }

    synchronized(accounts->mCacheLock) {
        AutoPtr<ISQLiteDatabase> db;
        FAIL_RETURN(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db))
        Boolean accountDeleted = FALSE;
        AutoPtr<ICursor> cursor;
        AutoPtr<ArrayOf<String> > accountsInfo = ArrayOf<String>::Alloc(3);
        (*accountsInfo)[0] = ACCOUNTS_ID;
        (*accountsInfo)[1] = ACCOUNTS_TYPE;
        (*accountsInfo)[2] = ACCOUNTS_NAME;
        String nullStr;
        FAIL_RETURN(db->Query(TABLE_ACCOUNTS, accountsInfo, nullStr, NULL,
                nullStr, nullStr, nullStr, (ICursor**)&cursor))
        // try {
        ECode ec;
        do {
            accounts->mAccountCache.Clear();
            AutoPtr<IHashMap> accountNamesByType;
            Boolean result = FALSE;
            while (cursor->MoveToNext(&result), result) {
                Int64 accountId;
                ec = cursor->GetInt64(0, &accountId);
                if (FAILED(ec)) break;
                String accountType, accountName;
                ec = cursor->GetString(1, &accountType);
                if (FAILED(ec)) break;
                ec = cursor->GetString(2, &accountName);
                if (FAILED(ec)) break;

                AutoPtr<IAuthenticatorDescription> value;
                AutoPtr<IAuthenticatorDescriptionHelper> descHelper;
                CAuthenticatorDescriptionHelper::AcquireSingleton(
                        (IAuthenticatorDescriptionHelper**)&descHelper);
                ec = descHelper->NewKey(accountType, (IAuthenticatorDescription**)&value);
                if (FAILED(ec)) break;
                Boolean isContains;
                knownAuth->Contains(value, &isContains);
                if (!isContains) {
                    Slogger::W(TAG, "deleting account %s because type %s no longer has a registered authenticator",
                            (const char*)accountName, (const char*)accountType);
                    Int32 result;
                    ec = db->Delete(TABLE_ACCOUNTS,
                            ACCOUNTS_ID + String("=") + StringUtils::ToString(accountId),
                            NULL, &result);
                    if (FAILED(ec)) break;
                    accountDeleted = TRUE;
                    AutoPtr<IAccount> account;
                    CAccount::New(accountName, accountType, (IAccount**)&account);
                    accounts->mUserDataCache->Remove(account);
                    accounts->mAuthTokenCache->Remove(account);
                }
                else {
                    AutoPtr<IInterface> obj;
                    accountNamesByType->Get(StringUtils::ParseCharSequence(accountType), (IInterface**)&obj);
                    AutoPtr<IArrayList> accountNames = IArrayList::Probe(obj);
                    if (accountNames == NULL) {
                        CArrayList::New((IArrayList**)&accountNames);
                        accountNamesByType->Put(StringUtils::ParseCharSequence(accountType), accountNames);
                    }
                    accountNames->Add(StringUtils::ParseCharSequence(accountName));
                }
            }
            if (FAILED(ec)) break;
            AutoPtr<ISet> entrySet;
            IMap::Probe(accountNamesByType)->GetEntrySet((ISet**)&entrySet);
            AutoPtr<IIterator> it;
            entrySet->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IMapEntry> cur = IMapEntry::Probe(obj);
                obj = NULL;
                ec = cur->GetKey((IInterface**)&obj);
                if (FAILED(ec)) break;
                String accountType;
                IObject::Probe(obj)->ToString(&accountType);
                obj = NULL;
                ec = cur->GetValue((IInterface**)&obj);
                if (FAILED(ec)) break;
                AutoPtr<IArrayList> accountNames = IArrayList::Probe(obj);
                Int32 size;
                accountNames->GetSize(&size);
                AutoPtr<ArrayOf<IAccount*> > accountsForType = ArrayOf<IAccount*>::Alloc(size);
                Int32 i = 0;
                AutoPtr<IIterator> iter;
                accountNames->GetIterator((IIterator**)&iter);
                Boolean hasNext;
                while (iter->HasNext(&hasNext), hasNext) {
                    obj = NULL;
                    iter->GetNext((IInterface**)&obj);
                    String accountName;
                    IObject::Probe(obj)->ToString(&accountName);
                    AutoPtr<IAccount> a;
                    CAccount::New(accountName, accountType, (IAccount**)&a);
                    accountsForType->Set(i, a);
                }
                AutoPtr<IArrayOf> iAccountsForType;
                CArrayOf::New(EIID_IAccount, size, (IArrayOf**)&iAccountsForType);
                for (Int32 i = 0; i < accountsForType->GetLength(); ++i) {
                    iAccountsForType->Set(i, (*accountsForType)[i]);
                }
                accounts->mAccountCache->Put(StringUtils::ParseCharSequence(accountType), iAccountsForType);
            }
        } while(FALSE);
        // } finally {
        ICloseable::Probe(cursor)->Close();
        if (accountDeleted) {
            SendAccountsChangedBroadcast(accounts->mUserId);
        }
        return ec;
        // }
    }
    return NOERROR;
}

AutoPtr<CAccountManagerService::UserAccounts> CAccountManagerService::GetUserAccountsForCaller()
{
    return GetUserAccounts(UserHandle::GetCallingUserId());
}

AutoPtr<CAccountManagerService::UserAccounts> CAccountManagerService::GetUserAccounts(
    /* [in] */ Int32 userId)
{
    synchronized(mUsers) {
        AutoPtr<IInterface> obj;
        mUsers->Get(userId, (IInterface**)&obj);
        AutoPtr<UserAccounts> accounts = (UserAccounts*) IObject::Probe(obj);
        if (accounts == NULL) {
            accounts = InitUserLocked(userId);
            mUsers->Append(userId, TO_IINTERFACE(accounts));
        }
        return accounts;
    }
    return NOERROR;
}

void CAccountManagerService::OnUserRemoved(
    /* [in] */ IIntent* intent)
{
    Int32 userId;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &userId);
    if (userId < 1) return;

    AutoPtr<UserAccounts> accounts;
    synchronized(mUsers) {
        AutoPtr<IInterface> obj;
        mUsers->Get(userId, (IInterface**)&obj);
        mUsers->Remove(userId);
    }

    if (accounts == NULL) {
        AutoPtr<IFile> dbFile;
        String s;
        GetDatabaseName(userId, &s);
        CFile::New(s, (IFile**)&dbFile);
        Boolean result;
        dbFile->Delete(&result);
        return;
    }

    synchronized(accounts->mCacheLock) {
        accounts->mOpenHelper->Close();
        AutoPtr<IFile> dbFile;
        String s;
        GetDatabaseName(userId, &s);
        CFile::New(s, (IFile**)&dbFile);
        Boolean result;
        dbFile->Delete(&result);
    }
}

void CAccountManagerService::OnUserStarted(
    /* [in] */ IIntent* intent)
{
    Int32 userId;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &userId);
    if (userId < 1) return;

    // Check if there's a shared account that needs to be created as an account
    AutoPtr<ArrayOf<IAccount*> > sharedAccounts;
    GetSharedAccountsAsUser(userId, (ArrayOf<IAccount*>**)&sharedAccounts);
    if (sharedAccounts == NULL || sharedAccounts->GetLength() == 0) return;
    AutoPtr<ArrayOf<IAccount*> > accounts;
    GetAccountsAsUser(String(NULL), userId, (ArrayOf<IAccount*>**)&accounts);
    for (Int32 i = 0; i < sharedAccounts->GetLength(); ++i) {
        AutoPtr<IAccount> sa = IAccount::Probe((*sharedAccounts)[i]);
        if (ArrayUtils::Contains(accounts.Get(), sa.Get())) continue;
        // Account doesn't exist. Copy it now.
        Boolean bNoUse;
        CopyAccountToUser(sa, IUserHandle::USER_OWNER, userId, &bNoUse);
    }
}


ECode CAccountManagerService::OnServiceChanged(
    /* [in] */ IInterface* type,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean removed)
{
    ValidateAccountsInternal(GetUserAccounts(userId),
            FALSE /* invalidateAuthenticatorCache */);

    return NOERROR;
}

ECode CAccountManagerService::GetPassword(
    /* [in] */ IAccount* account,
    /* [out] */ String* password)
{
    VALIDATE_NOT_NULL(password);
    *password = String(NULL);

    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "getPassword: %s"
                ", caller's uid %d"
                ", pid %d", TO_CSTR(account), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckAuthenticateAccountsPermission(account))

    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = ReadPasswordInternal(accounts, account, password);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::ReadPasswordInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (account == NULL) {
        *result = String(NULL);
        return NOERROR;
    }

    synchronized(accounts->mCacheLock) {
        AutoPtr<ISQLiteDatabase> db;
        FAIL_RETURN(accounts->mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db))
        AutoPtr<ICursor> cursor;
        AutoPtr<ArrayOf<String> > accounts1 = ArrayOf<String>::Alloc(1);
        (*accounts1)[0] = ACCOUNTS_PASSWORD;
        AutoPtr<ArrayOf<String> > accounts2 = ArrayOf<String>::Alloc(2);
        account->GetName(&(*accounts2)[0]);
        account->GetType(&(*accounts2)[1]);
        String nullStr;
        FAIL_RETURN(db->Query(TABLE_ACCOUNTS, accounts1,
                ACCOUNTS_NAME + String("=? AND ") + ACCOUNTS_TYPE+ String("=?"),
                accounts2, nullStr, nullStr, nullStr, (ICursor**)&cursor))
        // try {
        ECode ec;
        do {
            Boolean isMoveToNextOk;
            if (cursor->MoveToNext(&isMoveToNextOk), isMoveToNextOk) {
                String s;
                ec = cursor->GetString(0, &s);
                if (FAILED(ec)) break;
                ICloseable::Probe(cursor)->Close();
                *result = s;
                return NOERROR;
            }
        } while(FALSE);

        // } finally {
        ICloseable::Probe(cursor)->Close();
        *result = String(NULL);
        return ec;
        // }
    }
    return NOERROR;
}

ECode CAccountManagerService::GetPreviousName(
    /* [in] */ IAccount* account,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "getPreviousName: %s, caller's uid %d, pid %d",
                TO_CSTR(account), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (account == NULL) {
        Logger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = ReadPreviousNameInternal(accounts, account, result);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::ReadPreviousNameInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    if  (account == NULL) {
        return NOERROR;
    }
    synchronized(accounts->mCacheLock) {
        AutoPtr<IInterface> obj;
        accounts->mPreviousNameCache->Get(account, (IInterface**)&obj);
        AutoPtr<IAtomicReference> previousNameRef = IAtomicReference::Probe(obj);
        if (previousNameRef == NULL) {
            AutoPtr<ISQLiteDatabase> db;
            accounts->mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
            AutoPtr<ICursor> cursor;
            AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
            String name;
            account->GetName(&name);
            array->Set(0, name);
            String type;
            account->GetType(&type);
            array->Set(1, type);
            AutoPtr<ArrayOf<String> > array0 = ArrayOf<String>::Alloc(1);
            array0->Set(0, ACCOUNTS_PREVIOUS_NAME);
            FAIL_RETURN(db->Query(
                    TABLE_ACCOUNTS,
                    array0,
                    ACCOUNTS_NAME + "=? AND " + ACCOUNTS_TYPE+ "=?",
                    array,
                    String(NULL),
                    String(NULL),
                    String(NULL), (ICursor**)&cursor))
            // try {
            ECode ec;
            do {
                Boolean isMoveToNextOk;
                if (cursor->MoveToNext(&isMoveToNextOk), isMoveToNextOk) {
                    String previousName;
                    ec = cursor->GetString(0, &previousName);
                    if (FAILED(ec)) break;
                    CAtomicReference::New(StringUtils::ParseCharSequence(previousName), (IAtomicReference**)&previousNameRef);
                    accounts->mPreviousNameCache->Put(account, previousNameRef);
                    *result = previousName;
                    return NOERROR;
                } else {
                    *result = String(NULL);
                    return NOERROR;
                }
            } while(FALSE);
            // } finally {
            ICloseable::Probe(cursor)->Close();
            if (FAILED(ec)) return ec;
            // }
        } else {
            AutoPtr<IInterface> obj;
            previousNameRef->Get((IInterface**)&obj);
            return IObject::Probe(obj)->ToString(result);
        }
    }
    return NOERROR;
}

ECode CAccountManagerService::GetUserData(
    /* [in] */ IAccount* account,
    /* [in] */ const String& key,
    /* [out] */ String* userData)
{
    VALIDATE_NOT_NULL(userData)
    *userData = String(NULL);

    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "getUserData: %s"
                ", key %s"
                ", caller's uid %d"
                ", pid %d", TO_CSTR(account), key.string(), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (key.IsNull()) {
        Slogger::E(TAG, "key is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckAuthenticateAccountsPermission(account))
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = ReadUserDataInternal(accounts, account, key, userData);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::GetAuthenticatorTypes(
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<IAuthenticatorDescription*>** _types)
{
    VALIDATE_NOT_NULL(_types);
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "getAuthenticatorTypes: "
                "for user id %d"
                "caller's uid %d"
                ", pid %d", userId, Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    // Only allow the system process to read accounts of other users
    String s;
    s.AppendFormat("User %d trying get authenticator types for %d", UserHandle::GetCallingUserId(), userId);
    EnforceCrossUserPermission(userId, s);
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        AutoPtr<ICollection> authenticatorCollection;
        ec = mAuthenticatorCache->GetAllServices(userId, (ICollection**)&authenticatorCollection);
        if (FAILED(ec)) break;
        Int32 count;
        authenticatorCollection->GetSize(&count);
        AutoPtr<ArrayOf<IAuthenticatorDescription*> > types =
                ArrayOf<IAuthenticatorDescription*>::Alloc(count);
        AutoPtr<IIterator> it;
        authenticatorCollection->GetIterator((IIterator**)&it);
        Boolean hasNext;
        Int32 i = 0;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IRegisteredServicesCacheServiceInfo> authenticator =
                    IRegisteredServicesCacheServiceInfo::Probe(obj);
            obj = NULL;
            ec = authenticator->GetType((IInterface**)&obj);
            if (FAILED(ec)) break;
            types->Set(i, IAuthenticatorDescription::Probe(obj));
            ++i;
        }
        if (FAILED(ec)) break;

        *_types = types;
        REFCOUNT_ADD(*_types);
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::EnforceCrossUserPermission(
    /* [in] */ Int32 userId,
    /* [in] */ const String& errorMessage)
{
    Int32 permissionCode;
    mContext->CheckCallingOrSelfPermission(
                Manifest::permission::INTERACT_ACROSS_USERS_FULL, &permissionCode);
    if (userId != UserHandle::GetCallingUserId()
            && Binder::GetCallingUid() != Process::MyUid()
            && permissionCode != IPackageManager::PERMISSION_GRANTED) {
        Logger::E(TAG, errorMessage.string());
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CAccountManagerService::AddAccountExplicitly(
    /* [in] */ IAccount* account,
    /* [in] */ const String& password,
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "addAccountExplicitly: %s, caller's uid %d, pid %d",
                TO_CSTR(account), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckAuthenticateAccountsPermission(account))
    /*
     * Child users are not allowed to add accounts. Only the accounts that are
     * shared by the parent profile can be added to child profile.
     *
     * TODO: Only allow accounts that were shared to be added by
     *     a limited user.
     */

    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    // fails if the account already exists
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = AddAccountInternal(accounts, account, password, extras, FALSE, result);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::CopyAccountToUser(
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userFrom,
    /* [in] */ Int32 userTo,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<UserAccounts> fromAccounts = GetUserAccounts(userFrom);
    AutoPtr<UserAccounts> toAccounts = GetUserAccounts(userTo);
    if (fromAccounts == NULL || toAccounts == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        AutoPtr<CCopyAccountToUserSession> session;
        CCopyAccountToUserSession::NewByFriend(TO_IINTERFACE(fromAccounts), NULL, account, FALSE,
                FALSE /* stripAuthTokenFromResult */,
                this, TO_IINTERFACE(toAccounts), (CCopyAccountToUserSession**)&session);
        ec = session->Bind();
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    if (FAILED(ec)) return ec;
    // }
    *result = TRUE;
    return NOERROR;
}

ECode CAccountManagerService::CompleteCloningAccount(
    /* [in] */ IBundle* result,
    /* [in] */ IAccount* account,
    /* [in] */ UserAccounts* targetUser)
{
    Int64 id = Binder::ClearCallingIdentity();
    // try {
    String type;
    account->GetType(&type);
    AutoPtr<CCompleteCloningAccountSession> session;
    CCompleteCloningAccountSession::NewByFriend(TO_IINTERFACE(targetUser), NULL, account, FALSE,
            FALSE /* stripAuthTokenFromResult */,
            this, result, (CCompleteCloningAccountSession**)&session);
    ECode ec = session->Bind();
    // } finally {
    Binder::RestoreCallingIdentity(id);
    return ec;
    // }
}

ECode CAccountManagerService::AddAccountInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ const String& password,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean restricted,
    /* [out] */ Boolean* result)
{
    if (account == NULL) {
        return FALSE;
    }

    synchronized(accounts->mCacheLock) {
        AutoPtr<ISQLiteDatabase> db;
        FAIL_RETURN(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db))
        db->BeginTransaction();
        // try {
        ECode ec;
        do {
            AutoPtr<IDatabaseUtils> helper;
            ec = CDatabaseUtils::AcquireSingleton((IDatabaseUtils**)&helper);
            if (FAILED(ec)) return ec;
            Int64 numMatches;
            AutoPtr<ArrayOf<String> > accountInfo = ArrayOf<String>::Alloc(2);
            ec = account->GetName(&(*accountInfo)[0]);
            if (FAILED(ec)) return ec;
            ec = account->GetType(&(*accountInfo)[1]);
            if (FAILED(ec)) return ec;
            ec = helper->Int64ForQuery(db,
                    String("select count(*) from ") + TABLE_ACCOUNTS
                            + String(" WHERE ") + ACCOUNTS_NAME + String("=? AND ")
                            + ACCOUNTS_TYPE+ String("=?"),
                    accountInfo, &numMatches);
            if (FAILED(ec)) return ec;
            if (numMatches > 0) {
                Slogger::W(TAG, "insertAccountIntoDatabase: %s, skipping since the account already exists",
                    TO_CSTR(account));
                ec = db->EndTransaction();
                if (FAILED(ec)) return ec;
                *result = FALSE;
                return NOERROR;
            }
            AutoPtr<IContentValues> values;
            ec = CContentValues::New((IContentValues**)&values);
            if (FAILED(ec)) return ec;
            AutoPtr<ICharSequence> cs;
            CString::New((*accountInfo)[0], (ICharSequence**)&cs);
            ec = values->Put(ACCOUNTS_NAME, cs);
            if (FAILED(ec)) return ec;
            cs = NULL;
            CString::New((*accountInfo)[1], (ICharSequence**)&cs);
            ec = values->Put(ACCOUNTS_TYPE, cs);
            if (FAILED(ec)) return ec;
            cs = NULL;
            CString::New(password, (ICharSequence**)&cs);
            ec = values->Put(ACCOUNTS_PASSWORD, cs);
            if (FAILED(ec)) return ec;
            Int64 accountId;
            ec = db->Insert(TABLE_ACCOUNTS, ACCOUNTS_NAME, values, &accountId);
            if (FAILED(ec)) return ec;
            if (accountId < 0) {
                Slogger::W(TAG, "insertAccountIntoDatabase: %s, skipping the DB insert failed", TO_CSTR(account));
                ec = db->EndTransaction();
                if (FAILED(ec)) return ec;
                *result = FALSE;
                return NOERROR;
            }
            if (extras != NULL) {
                AutoPtr<ISet> keys;
                extras->GetKeySet((ISet**)&keys);
                AutoPtr<IIterator> it;
                keys->GetIterator((IIterator**)&it);
                Boolean hasNext;
                while(it->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> obj;
                    it->GetNext((IInterface**)&obj);
                    String key;
                    IObject::Probe(obj)->ToString(&key);
                    String value;
                    extras->GetString(key, &value);
                    if (InsertExtraLocked(db, accountId, key, value) < 0) {
                        Slogger::W(TAG, "insertAccountIntoDatabase: %s, skipping since insertExtra failed for key %s",
                                TO_CSTR(account), key.string());
                        db->EndTransaction();
                        *result = FALSE;
                        return NOERROR;
                    }
                }
            }
            ec = db->SetTransactionSuccessful();
            if (FAILED(ec)) return ec;
            InsertAccountIntoCacheLocked(accounts, account);
        } while(FALSE);
        // } finally {
        db->EndTransaction();
        if (FAILED(ec)) return ec;
        // }
        SendAccountsChangedBroadcast(accounts->mUserId);
        if (accounts->mUserId == IUserHandle::USER_OWNER) {
            AddAccountToLimitedUsers(account);
        }
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

ECode CAccountManagerService::AddAccountToLimitedUsers(
    /* [in] */ IAccount* account)
{
    AutoPtr<IList> users;
    GetUserManager()->GetUsers((IList**)&users);
    AutoPtr<IIterator> it;
    users->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IUserInfo> user = IUserInfo::Probe(obj);
        Boolean isRestricted;
        user->IsRestricted(&isRestricted);
        if (isRestricted) {
            Int32 id;
            user->GetId(&id);
            Boolean bNoUse;
            AddSharedAccountAsUser(account, id, &bNoUse);
            // try {
            ECode ec;
            do {
                Boolean isUserRunning;
                ec = ActivityManagerNative::GetDefault()->IsUserRunning(id, FALSE, &isUserRunning);
                if (FAILED(ec)) break;
                if (isUserRunning) {
                    AutoPtr<IMessage> msg;
                    ec = mMessageHandler->ObtainMessage(
                            MESSAGE_COPY_SHARED_ACCOUNT, IUserHandle::USER_OWNER, id,
                            account, (IMessage**)&msg);
                    if (FAILED(ec)) break;
                    Boolean bNoUse;
                    ec = mMessageHandler->SendMessage(msg, &bNoUse);
                }
            } while(FALSE);
            // } catch (RemoteException re) {
            if (FAILED(ec)) {
                if ((ECode) E_REMOTE_EXCEPTION == ec) {
                    // Shouldn't happen
                }
                else
                    return ec;
            }
            // }
        }
    }
    return NOERROR;
}

Int64 CAccountManagerService::InsertExtraLocked(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int64 accountId,
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr<IContentValues> values;
    FAIL_RETURN(CContentValues::New((IContentValues**)&values))
    AutoPtr<ICharSequence> csq;
    CString::New(key, (ICharSequence**)&csq);
    values->Put(EXTRAS_KEY, csq);
    AutoPtr<IInteger64> id;
    CInteger64::New(accountId, (IInteger64**)&id);
    values->Put(EXTRAS_ACCOUNTS_ID, id);
    csq = NULL;
    CString::New(value, (ICharSequence**)&csq);
    values->Put(EXTRAS_VALUE, csq);
    Int64 result;
    db->Insert(TABLE_EXTRAS, EXTRAS_KEY, values, &result);
    return result;
}

ECode CAccountManagerService::HasFeatures(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<String>* features)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "hasFeatures: %s"
                ", response %s"
                ", features %s"
                ", caller's uid %d"
                ", pid %d", TO_CSTR(account), TO_CSTR(response), StringArrayToString(features).string(), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (features == NULL) {
        Slogger::E(TAG, "features is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckReadAccountsPermission())
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        AutoPtr<CTestFeaturesSession> session;
        ec = CTestFeaturesSession::NewByFriend(TO_IINTERFACE(accounts),
                response, account, *features, this,
                (CTestFeaturesSession**)&session);
        if (FAILED(ec)) break;
        ec = session->Bind();
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::RenameAccount(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* accountToRename,
    /* [in] */ const String& newName)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "renameAccount: %s -> %s, caller's uid %d, pid %d",
                TO_CSTR(accountToRename), newName.string(), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (accountToRename == NULL) {
        Logger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    CheckAuthenticateAccountsPermission(accountToRename);
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        AutoPtr<IAccount> resultingAccount;
        ec = RenameAccountInternal(accounts, accountToRename, newName, (IAccount**)&resultingAccount);
        if (FAILED(ec)) break;
        AutoPtr<IBundle> result;
        ec = CBundle::New((IBundle**)&result);
        if (FAILED(ec)) break;
        String name;
        resultingAccount->GetName(&name);
        ec = result->PutString(IAccountManager::KEY_ACCOUNT_NAME, name);
        if (FAILED(ec)) break;
        String type;
        resultingAccount->GetType(&type);
        ec = result->PutString(IAccountManager::KEY_ACCOUNT_TYPE, type);
        if (FAILED(ec)) break;
        // try {
        ec = response->OnResult(result);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                Logger::W(TAG, "%d", ec);
            }
        }
        // }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::RenameAccountInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* accountToRename,
    /* [in] */ const String& newName,
    /* [out] */ IAccount** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IAccount> resultAccount;
    /*
     * Cancel existing notifications. Let authenticators
     * re-post notifications as required. But we don't know if
     * the authenticators have bound their notifications to
     * now stale account name data.
     *
     * With a rename api, we might not need to do this anymore but it
     * shouldn't hurt.
     */
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(accounts->mUserId, (IUserHandle**)&userHandle);
    CancelNotification(
            GetSigninRequiredNotificationId(accounts, accountToRename),
            userHandle);
    synchronized(accounts->mCredentialsPermissionNotificationIds) {
        AutoPtr<ISet> keySet;
        accounts->mCredentialsPermissionNotificationIds->GetKeySet((ISet**)&keySet);
        AutoPtr<IIterator> it;
        keySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IPair> pair = IPair::Probe(obj);
            obj = NULL;
            pair->GetFirst((IInterface**)&obj);
            AutoPtr<IInterface> first;
            IPair::Probe(obj)->GetFirst((IInterface**)&first);
            Boolean isEquals;
            accountToRename->Equals(first, &isEquals);
            if (isEquals) {
                obj = NULL;
                accounts->mCredentialsPermissionNotificationIds->Get(pair, (IInterface**)&obj);
                AutoPtr<IInteger32> i32 = IInteger32::Probe(obj);
                Int32 id;
                i32->GetValue(&id);
                AutoPtr<IUserHandle> userHandle;
                CUserHandle::New(accounts->mUserId, (IUserHandle**)&userHandle);
                CancelNotification(id, userHandle);
            }
        }
    }
    synchronized(accounts->mCacheLock) {
        AutoPtr<ISQLiteDatabase> db;
        accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
        db->BeginTransaction();
        Boolean isSuccessful = FALSE;
        AutoPtr<IAccount> renamedAccount;
        String type;
        accountToRename->GetType(&type);
        CAccount::New(newName, type, (IAccount**)&renamedAccount);
        // try {
        ECode ec;
        do {
            AutoPtr<IContentValues> values;
            ec = CContentValues::New((IContentValues**)&values);
            if (FAILED(ec)) break;
            ec = values->Put(ACCOUNTS_NAME, newName);
            if (FAILED(ec)) break;
            String name;
            accountToRename->GetName(&name);
            ec = values->Put(ACCOUNTS_PREVIOUS_NAME, name);
            if (FAILED(ec)) break;
            Int64 accountId;
            GetAccountIdLocked(db, accountToRename, &accountId);
            if (FAILED(ec)) break;
            if (accountId >= 0) {
                AutoPtr<ArrayOf<String> > argsAccountId = ArrayOf<String>::Alloc(1);
                argsAccountId->Set(0, StringUtils::ToString(accountId));
                Int32 iNoUse;
                ec = db->Update(TABLE_ACCOUNTS, values, ACCOUNTS_ID + "=?", argsAccountId, &iNoUse);
                if (FAILED(ec)) break;
                ec = db->SetTransactionSuccessful();
                if (FAILED(ec)) break;
                isSuccessful = TRUE;
            }
        } while(FALSE);
        // } finally {
        db->EndTransaction();
        if (isSuccessful) {
            /*
             * Database transaction was successful. Clean up cached
             * data associated with the account in the user profile.
             */
            InsertAccountIntoCacheLocked(accounts, renamedAccount);
            /*
             * Extract the data and token caches before removing the
             * old account to preserve the user data associated with
             * the account.
             */
            AutoPtr<IInterface> obj;
            accounts->mUserDataCache->Get(accountToRename, (IInterface**)&obj);
            AutoPtr<IHashMap> tmpData = IHashMap::Probe(obj);
            obj = NULL;
            accounts->mAuthTokenCache->Get(accountToRename, (IInterface**)&obj);
            AutoPtr<IHashMap> tmpTokens = IHashMap::Probe(obj);
            RemoveAccountFromCacheLocked(accounts, accountToRename);
            /*
             * Update the cached data associated with the renamed
             * account.
             */
            accounts->mUserDataCache->Put(renamedAccount, tmpData);
            accounts->mAuthTokenCache->Put(renamedAccount, tmpTokens);
            AutoPtr<IAtomicReference> atomicRef;
            String name;
            accountToRename->GetName(&name);
            CAtomicReference::New(StringUtils::ParseCharSequence(name), (IAtomicReference**)&atomicRef);
            accounts->mPreviousNameCache->Put(renamedAccount, atomicRef);
            resultAccount = renamedAccount;

            if (accounts->mUserId == IUserHandle::USER_OWNER) {
                /*
                 * Owner's account was renamed, rename the account for
                 * those users with which the account was shared.
                 */
                AutoPtr<IList> users;
                mUserManager->GetUsers(TRUE, (IList**)&users);
                AutoPtr<IIterator> it;
                users->GetIterator((IIterator**)&it);
                Boolean hasNext;
                while (it->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> obj;
                    it->GetNext((IInterface**)&obj);
                    AutoPtr<IUserInfo> user = IUserInfo::Probe(obj);
                    Boolean isRestricted;
                    user->IsRestricted(&isRestricted);
                    Boolean isPrimary;
                    user->IsPrimary(&isPrimary);
                    if (!isPrimary && isRestricted) {
                        Int32 id;
                        user->GetId(&id);
                        Boolean bNoUse;
                        RenameSharedAccountAsUser(accountToRename, newName, id, &bNoUse);
                    }
                }
            }
            SendAccountsChangedBroadcast(accounts->mUserId);
        }
        if (FAILED(ec)) return ec;
        // }
    }
    *result = resultAccount;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CAccountManagerService::RemoveAccount(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "removeAccount: %s"
                ", response %s"
                ", caller's uid %d"
                ", pid %d", TO_CSTR(account), TO_CSTR(response), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckManageAccountsPermission())
    AutoPtr<IUserHandle> user = Binder::GetCallingUserHandle();
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int32 userId;
    user->GetIdentifier(&userId);;
    if (!CanUserModifyAccounts(userId)) {
        // try {
        ECode ec;
        // TODO: This should be ERROR_CODE_USER_RESTRICTED instead. See http://b/16322768
        ec = response->OnError(IAccountManager::ERROR_CODE_UNSUPPORTED_OPERATION,
                String("User cannot modify accounts"));
        // } catch (RemoteException re) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec)
                return ec;
        }
        // }
        return NOERROR;
    }
    String type;
    account->GetType(&type);
    if (!CanUserModifyAccountsForType(userId, type)) {
        // try {
        ECode ec;
        ec = response->OnError(IAccountManager::ERROR_CODE_MANAGEMENT_DISABLED_FOR_ACCOUNT_TYPE,
                String("User cannot modify accounts of this type (policy)."));
        // } catch (RemoteException re) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec)
                return ec;
        }
        // }
        return NOERROR;
    }
    Int64 identityToken = Binder::ClearCallingIdentity();

    CancelNotification(GetSigninRequiredNotificationId(accounts, account), user);
    synchronized(accounts->mCredentialsPermissionNotificationIds) {
        AutoPtr<ISet> keySet;
        accounts->mCredentialsPermissionNotificationIds->GetKeySet((ISet**)&keySet);
        AutoPtr<IIterator> it;
        keySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IPair> pair = IPair::Probe(obj);
            obj = NULL;
            pair->GetFirst((IInterface**)&obj);
            AutoPtr<IInterface> first;
            IPair::Probe(obj)->GetFirst((IInterface**)&first);
            Boolean isEquals;
            account->Equals(first, &isEquals);
            if (isEquals) {
                Int32 id;
                obj = NULL;
                accounts->mCredentialsPermissionNotificationIds->Get(pair, (IInterface**)&obj);
                IInteger32::Probe(obj)->GetValue(&id);
                CancelNotification(id, user);
            }
        }
    }

    // try {
    ECode ec;
    do {
        AutoPtr<CRemoveAccountSession> session;
        ec = CRemoveAccountSession::NewByFriend(TO_IINTERFACE(accounts), response, account, this, (CRemoveAccountSession**)&session);
        if (FAILED(ec)) break;
        ec = session->Bind();
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    if (FAILED(ec)) return ec;
    // }
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "removeAccount: %s, response %s, caller's uid %d, pid %d",
                TO_CSTR(account), TO_CSTR(response), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (response == NULL) {
        Logger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (account == NULL) {
        Logger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    CheckManageAccountsPermission();
    if (!CanUserModifyAccounts(userId)) {
        // try {
            // TODO: This should be ERROR_CODE_USER_RESTRICTED instead. See http://b/16322768
        ECode ec = response->OnError(IAccountManager::ERROR_CODE_UNSUPPORTED_OPERATION,
                String("User cannot modify accounts"));
        // } catch (RemoteException re) {
        if ((ECode) E_REMOTE_EXCEPTION == ec)
            ec = NOERROR;
        // }
        return ec;
    }
    account->GetType(&type);
    if (!CanUserModifyAccountsForType(userId, type)) {
        // try {
        ECode ec = response->OnError(IAccountManager::ERROR_CODE_MANAGEMENT_DISABLED_FOR_ACCOUNT_TYPE,
                String("User cannot modify accounts of this type (policy)."));
        // } catch (RemoteException re) {
        if ((ECode) E_REMOTE_EXCEPTION == ec)
            ec = NOERROR;
        // }
        return ec;
    }

    CancelNotification(GetSigninRequiredNotificationId(accounts, account), user);
    synchronized(accounts->mCredentialsPermissionNotificationIds) {
        AutoPtr<ISet> keySet;
        accounts->mCredentialsPermissionNotificationIds->GetKeySet((ISet**)&keySet);
        AutoPtr<IIterator> it;
        keySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IPair> pair = IPair::Probe(obj);
            obj = NULL;
            pair->GetFirst((IInterface**)&obj);
            AutoPtr<IInterface> first;
            IPair::Probe(obj)->GetFirst((IInterface**)&first);
            Boolean isEquals;
            account->Equals(first, &isEquals);
            if (isEquals) {
                obj = NULL;
                accounts->mCredentialsPermissionNotificationIds->Get(pair, (IInterface**)&obj);
                Int32 id;
                IInteger32::Probe(obj)->GetValue(&id);
                CancelNotification(id, user);
            }
        }
    }

    // try {
    do {
        AutoPtr<CRemoveAccountSession> session;
        ec = CRemoveAccountSession::NewByFriend(TO_IINTERFACE(accounts), response, account,
                this, (CRemoveAccountSession**)&session);
        if (FAILED(ec)) break;
        ec = session->Bind();
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::RemoveAccountAsUser(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userId)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "removeAccount: %s, response %s, caller's uid %d, pid %d, for user id %d",
                TO_CSTR(account), TO_CSTR(response), Binder::GetCallingUid(), Binder::GetCallingPid(), userId);
    }
    if (response == NULL) {
        Logger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (account == NULL) {
        Logger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Only allow the system process to modify accounts of other users
    String s;
    s.AppendFormat("User %d trying to remove account for %d", UserHandle::GetCallingUserId(), userId);
    EnforceCrossUserPermission(userId, s);
    CheckManageAccountsPermission();

    AutoPtr<UserAccounts> accounts = GetUserAccounts(userId);
    if (!CanUserModifyAccounts(userId)) {
        // try {
        ECode ec = response->OnError(IAccountManager::ERROR_CODE_USER_RESTRICTED,
                String("User cannot modify accounts"));
        // } catch (RemoteException re) {
        if ((ECode) E_REMOTE_EXCEPTION == ec)
            ec = NOERROR;
        // }
        return ec;
    }
    String type;
    account->GetType(&type);
    if (!CanUserModifyAccountsForType(userId, type)) {
        // try {
        ECode ec = response->OnError(IAccountManager::ERROR_CODE_MANAGEMENT_DISABLED_FOR_ACCOUNT_TYPE,
                String("User cannot modify accounts of this type (policy)."));
        // } catch (RemoteException re) {
        if ((ECode) E_REMOTE_EXCEPTION == ec)
            ec = NOERROR;
        // }
        return ec;
    }

    AutoPtr<IUserHandle> user;
    CUserHandle::New(userId, (IUserHandle**)&user);
    Int64 identityToken = Binder::ClearCallingIdentity();

    CancelNotification(GetSigninRequiredNotificationId(accounts, account), user);
    synchronized(accounts->mCredentialsPermissionNotificationIds) {
        AutoPtr<ISet> keySet;
        accounts->mCredentialsPermissionNotificationIds->GetKeySet((ISet**)&keySet);
        AutoPtr<IIterator> it;
        keySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IPair> pair = IPair::Probe(obj);
            obj = NULL;
            pair->GetFirst((IInterface**)&obj);
            AutoPtr<IInterface> first;
            IPair::Probe(obj)->GetFirst((IInterface**)&first);
            Boolean isEquals;
            account->Equals(first, &isEquals);
            if (isEquals) {
                obj = NULL;
                accounts->mCredentialsPermissionNotificationIds->Get(pair, (IInterface**)&obj);
                Int32 id;
                IInteger32::Probe(obj)->GetValue(&id);
                CancelNotification(id, user);
            }
        }
    }

    // try {
    ECode ec;
    do {
        AutoPtr<CRemoveAccountSession> session;
        ec = CRemoveAccountSession::NewByFriend(TO_IINTERFACE(accounts), response, account, this, (CRemoveAccountSession**)&session);
        if (FAILED(ec)) break;
        ec = session->Bind();
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::RemoveAccountInternal(
    /* [in] */ IAccount* account)
{
    return RemoveAccountInternal(GetUserAccountsForCaller(), account);
}

ECode CAccountManagerService::RemoveAccountInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account)
{
    synchronized(accounts->mCacheLock) {
        AutoPtr<ISQLiteDatabase> db;
        FAIL_RETURN(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db))
        AutoPtr<ArrayOf<String> > accountInfo = ArrayOf<String>::Alloc(2);
        account->GetName(&(*accountInfo)[0]);
        account->GetType(&(*accountInfo)[1]);
        Int32 result;
        db->Delete(TABLE_ACCOUNTS, ACCOUNTS_NAME + String("=? AND ")
                + ACCOUNTS_TYPE+ String("=?"), accountInfo, &result);
        RemoveAccountFromCacheLocked(accounts, account);
        SendAccountsChangedBroadcast(accounts->mUserId);
        if (accounts->mUserId == IUserHandle::USER_OWNER) {
            // Owner's account was removed, remove from any users that are sharing
            // this account.
            Int64 id = Binder::ClearCallingIdentity();
            // try {
            ECode ec;
            do {
                AutoPtr<IList> users;
                ec = mUserManager->GetUsers(TRUE, (IList**)&users);
                if (FAILED(ec)) break;
                AutoPtr<IIterator> it;
                users->GetIterator((IIterator**)&it);
                Boolean hasNext;
                while (it->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> obj;
                    it->GetNext((IInterface**)&obj);
                    AutoPtr<IUserInfo> user = IUserInfo::Probe(obj);
                    Boolean isRestricted;
                    ec = user->IsRestricted(&isRestricted);
                    if (FAILED(ec)) break;
                    Boolean isPrimary;
                    ec = user->IsPrimary(&isPrimary);
                    if (FAILED(ec)) break;
                    if (!isPrimary && isRestricted) {
                        Int32 id;
                        user->GetId(&id);
                        Boolean bNoUse;
                        ec = RemoveSharedAccountAsUser(account, id, &bNoUse);
                        if (FAILED(ec)) break;
                    }
                }
            } while(FALSE);
            // } finally {
            Binder::RestoreCallingIdentity(id);
            if (FAILED(ec))
                return ec;
            // }
        }
    }
    return NOERROR;
}

ECode CAccountManagerService::InvalidateAuthToken(
    /* [in] */ const String& accountType,
    /* [in] */ const String& authToken)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "invalidateAuthToken: accountType %s"
                ", caller's uid %d"
                ", pid %d", accountType.string(), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (authToken.IsNull()) {
        Slogger::E(TAG, "authToken is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckManageAccountsOrUseCredentialsPermissions())
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        synchronized(accounts->mCacheLock) {
            AutoPtr<ISQLiteDatabase> db;
            ec = accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
            if (FAILED(ec)) break;
            ec = db->BeginTransaction();
            if (FAILED(ec)) break;
            // try {
            InvalidateAuthTokenLocked(accounts, db, accountType, authToken);
            ec = db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            // }
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::InvalidateAuthTokenLocked(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& accountType,
    /* [in] */ const String& authToken)
{
    if (authToken.IsNull() || accountType.IsNull()) {
        return NOERROR;
    }
    AutoPtr<ICursor> cursor;
    AutoPtr<ArrayOf<String> > params = ArrayOf<String>::Alloc(2);
    (*params)[0] = authToken;
    (*params)[1] = accountType;
    db->RawQuery(
            String("SELECT ") + TABLE_AUTHTOKENS + String(".") + AUTHTOKENS_ID
                    + String(", ") + TABLE_ACCOUNTS + String(".") + ACCOUNTS_NAME
                    + String(", ") + TABLE_AUTHTOKENS + String(".") + AUTHTOKENS_TYPE
                    + String(" FROM ") + TABLE_ACCOUNTS
                    + String(" JOIN ") + TABLE_AUTHTOKENS
                    + String(" ON ") + TABLE_ACCOUNTS + String(".") + ACCOUNTS_ID
                    + String(" = ") + AUTHTOKENS_ACCOUNTS_ID
                    + String(" WHERE ") + AUTHTOKENS_AUTHTOKEN + String(" = ? AND ")
                    + TABLE_ACCOUNTS + String(".") + ACCOUNTS_TYPE + String(" = ?"),
            params, (ICursor**)&cursor);
    // try {
    ECode ec;
    Boolean result;
    while (cursor->MoveToNext(&result), result) {
        Int64 authTokenId;
        cursor->GetInt64(0, &authTokenId);
        String accountName, authTokenType;
        cursor->GetString(1, &accountName);
        cursor->GetString(2, &authTokenType);
        Int32 result;
        ec = db->Delete(TABLE_AUTHTOKENS,
                AUTHTOKENS_ID + String("=") + StringUtils::ToString(authTokenId),
                NULL, &result);
        if (FAILED(ec)) break;
        AutoPtr<IAccount> account;
        ec = CAccount::New(accountName, accountType, (IAccount**)&account);
        if (FAILED(ec)) break;
        WriteAuthTokenIntoCacheLocked(accounts, db, account,
                authTokenType, String(NULL));
    }
    // } finally {
    ICloseable::Probe(cursor)->Close();
    return ec;
    // }
}

ECode CAccountManagerService::SaveAuthTokenToDatabase(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ const String& type,
    /* [in] */ const String& authToken,
    /* [out] */ Boolean* _result)
{
    VALIDATE_NOT_NULL(_result)
    *_result = FALSE;

    if (account == NULL || type.IsNull()) {
        *_result = FALSE;
        return NOERROR;
    }
    AutoPtr<IUserHandle> userH;
    CUserHandle::New(accounts->mUserId, (IUserHandle**)&userH);
    CancelNotification(GetSigninRequiredNotificationId(accounts, account), userH);

    synchronized(accounts->mCacheLock) {
        AutoPtr<ISQLiteDatabase> db;
        FAIL_RETURN(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db))
        db->BeginTransaction();
        // try {
        ECode ec;
        do {
            Int64 accountId;
            GetAccountIdLocked(db, account, &accountId);
            if (accountId < 0) {
                *_result = FALSE;
                break;
            }
            AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(1);
            (*ss)[0] = type;
            Int32 res;
            ec = db->Delete(TABLE_AUTHTOKENS,
                    AUTHTOKENS_ACCOUNTS_ID + String("=") + StringUtils::ToString(accountId)
                    + String(" AND ") + AUTHTOKENS_TYPE + String("=?"), ss, &res);
            if (FAILED(ec)) break;
            AutoPtr<IContentValues> values;
            ec = CContentValues::New((IContentValues**)&values);
            if (FAILED(ec)) break;
            AutoPtr<IInteger32> aId;
            CInteger32::New(accountId, (IInteger32**)&aId);
            values->Put(AUTHTOKENS_ACCOUNTS_ID, aId);
            AutoPtr<ICharSequence> cs;
            CString::New(type, (ICharSequence**)&cs);
            values->Put(AUTHTOKENS_TYPE, cs);
            cs = NULL;
            CString::New(authToken, (ICharSequence**)&cs);
            values->Put(AUTHTOKENS_AUTHTOKEN, cs);
            Int64 result;
            ec = db->Insert(TABLE_AUTHTOKENS, AUTHTOKENS_AUTHTOKEN, values, &result);
            if (FAILED(ec)) break;
            if (result >= 0) {
                db->SetTransactionSuccessful();
                WriteAuthTokenIntoCacheLocked(accounts, db, account, type, authToken);
                *_result = TRUE;
                break;
            }
            *_result = FALSE;
        } while(FALSE);

        // } finally {
        db->EndTransaction();
        return ec;
        // }
    }
    return NOERROR;
}

ECode CAccountManagerService::PeekAuthToken(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [out] */ String* token)
{
    VALIDATE_NOT_NULL(token)
    *token = String(NULL);

    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "peekAuthToken: %s"
                ", authTokenType %s"
                ", caller's uid %d"
                ", pid %d", TO_CSTR(account), authTokenType.string(), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckAuthenticateAccountsPermission(account))
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ReadAuthTokenInternal(accounts, account, authTokenType);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // }
}

ECode CAccountManagerService::SetAuthToken(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ const String& authToken)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "setAuthToken: %s, authTokenType %s, caller's uid %d, pid %d",
                TO_CSTR(account), authTokenType.string(), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckAuthenticateAccountsPermission(account))
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    Boolean bNoUse;
    ECode ec = SaveAuthTokenToDatabase(accounts, account, authTokenType, authToken, &bNoUse);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::SetPassword(
    /* [in] */ IAccount* account,
    /* [in] */ const String& password)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "setAuthToken: %s, caller's uid %d, pid %d",
                TO_CSTR(account), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckAuthenticateAccountsPermission(account))
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = SetPasswordInternal(accounts, account, password);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::SetPasswordInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ const String& password)
{
    if (account == NULL) {
        return NOERROR;
    }

    synchronized(accounts->mCacheLock) {
        AutoPtr<ISQLiteDatabase> db;
        FAIL_RETURN(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db))
        db->BeginTransaction();
        // try {
        ECode ec;
        do {
            AutoPtr<IContentValues> values;
            ec = CContentValues::New((IContentValues**)&values);
            if (FAILED(ec)) break;
            AutoPtr<ICharSequence> cs;
            CString::New(password, (ICharSequence**)&cs);
            values->Put(ACCOUNTS_PASSWORD, cs);
            Int64 accountId;
            GetAccountIdLocked(db, account, &accountId);
            if (accountId >= 0) {
                AutoPtr<ArrayOf<String> > argsAccountId = ArrayOf<String>::Alloc(1);
                (*argsAccountId)[0] = StringUtils::ToString(accountId);
                Int32 result;
                ec = db->Update(TABLE_ACCOUNTS, values, ACCOUNTS_ID + String("=?"),
                        argsAccountId, &result);
                if (FAILED(ec)) break;
                ec = db->Delete(TABLE_AUTHTOKENS, AUTHTOKENS_ACCOUNTS_ID + String("=?"),
                        argsAccountId, &result);
                if (FAILED(ec)) break;
                accounts->mAuthTokenCache->Remove(account);
                ec = db->SetTransactionSuccessful();
            }
        } while(FALSE);
        // } finally {
        db->EndTransaction();
        if (FAILED(ec)) return ec;
        // }
        SendAccountsChangedBroadcast(accounts->mUserId);
    }
    return NOERROR;
}

void CAccountManagerService::SendAccountsChangedBroadcast(
    /* [in] */ Int32 userId)
{
    String action;
    ACCOUNTS_CHANGED_INTENT->GetAction(&action);
    Slogger::I(TAG, "the accounts changed, sending broadcast of %s", (const char*)action);
    AutoPtr<IUserHandle> userH;
    CUserHandle::New(userId, (IUserHandle**)&userH);
    mContext->SendBroadcastAsUser(ACCOUNTS_CHANGED_INTENT, userH);
}

ECode CAccountManagerService::ClearPassword(
    /* [in] */ IAccount* account)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "clearPassword: %s"
                ", caller's uid %d"
                ", pid %d", TO_CSTR(account), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckManageAccountsPermission())
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = SetPasswordInternal(accounts, account, String(NULL));
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::SetUserData(
    /* [in] */ IAccount* account,
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "setUserData: %s"
                ", key %s"
                ", caller's uid %d"
                ", pid %d", TO_CSTR(account), key.string(), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (key.IsNull()) {
        Slogger::E(TAG, "key is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckAuthenticateAccountsPermission(account))
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = SetUserdataInternal(accounts, account, key, value);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::SetUserdataInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    if (account == NULL || key.IsNull()) {
        return NOERROR;
    }

    synchronized(accounts->mCacheLock) {
        AutoPtr<ISQLiteDatabase> db;
        FAIL_RETURN(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db))
        db->BeginTransaction();
        // try {
        ECode ec = NOERROR;
        do {
            Int64 accountId;
            GetAccountIdLocked(db, account, &accountId);
            if (accountId < 0) {
                break;
            }
            Int64 extrasId;
            GetExtrasIdLocked(db, accountId, key, &extrasId);
            if (extrasId < 0 ) {
                extrasId = InsertExtraLocked(db, accountId, key, value);
                if (extrasId < 0) {
                    break;
                }
            }
            else {
                AutoPtr<IContentValues> values;
                CContentValues::New((IContentValues**)&values);
                AutoPtr<ICharSequence> cs;
                CString::New(value, (ICharSequence**)&cs);
                values->Put(EXTRAS_VALUE, cs);
                Int32 result;
                ec = db->Update(TABLE_EXTRAS, values, EXTRAS_ID + String("=")
                        + StringUtils::ToString(extrasId), NULL, &result);
                if (FAILED(ec)) break;
                if (1 != result) {
                    break;
                }

            }
            WriteUserDataIntoCacheLocked(accounts, db, account, key, value);
            ec = db->SetTransactionSuccessful();
        } while(FALSE);
        // } finally {
        db->EndTransaction();
        return ec;
        // }
    }
    return NOERROR;
}

ECode CAccountManagerService::OnResult(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IBundle* result)
{
    if (result == NULL) {
        Slogger::E(TAG, "the result is unexpectedly null");
    }
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "CAccountManagerService calling onResult() on response %s",
                TO_CSTR(response));
    }
    // try {
    ECode ec = response->OnResult(result);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            // if the caller is dead then there is no one to care about remote
            // exceptions
            if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                Logger::V(TAG, "failure while notifying response %d", ec);
            }
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode CAccountManagerService::GetAuthTokenLabel(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& accountType,
    /* [in] */ const String& authTokenType)
{
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 callingUid = Binder::GetCallingUid();
    Binder::ClearCallingIdentity();
    if (callingUid != IProcess::SYSTEM_UID) {
        Slogger::E(TAG, "can only call from system");
        return E_SECURITY_EXCEPTION;
    }
    AutoPtr<UserAccounts> accounts = GetUserAccounts(UserHandle::GetUserId(callingUid));
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        AutoPtr<CGetAuthTokenLabelSession> session;
        ec = CGetAuthTokenLabelSession::NewByFriend(TO_IINTERFACE(accounts), response, accountType,
                authTokenType, this, (CGetAuthTokenLabelSession**)&session);
        if (FAILED(ec)) break;
        ec = session->Bind();
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
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
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "getAuthToken: %s"
                ", response %s"
                ", authTokenType %s"
                ", notifyOnAuthFailure %s"
                ", expectActivityLaunch %s"
                ", caller's uid %d"
                ", pid %d", TO_CSTR(account), TO_CSTR(response), authTokenType.string(), StringUtils::BooleanToString(notifyOnAuthFailure).string(), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    ECode ec;
    do {
        if (account == NULL) {
            Slogger::W(TAG, "getAuthToken called with null account");
            ec = response->OnError(IAccountManager::ERROR_CODE_BAD_ARGUMENTS, String("account is null"));
            if (FAILED(ec)) break;
            return NOERROR;
        }
        if (authTokenType == NULL) {
            Slogger::W(TAG, "getAuthToken called with null authTokenType");
            ec = response->OnError(IAccountManager::ERROR_CODE_BAD_ARGUMENTS, String("authTokenType is null"));
            if (FAILED(ec)) break;
            return NOERROR;
        }
    } while(FALSE);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Slogger::W(TAG, "Failed to report error back to the client. %d", ec);
            return NOERROR;
        }
        else
            return ec;
    }
    // }

    AutoPtr<ArrayOf<String> > perm = ArrayOf<String>::Alloc(1);
    (*perm)[0] = Elastos::Droid::Manifest::permission::USE_CREDENTIALS;
    FAIL_RETURN(CheckBinderPermission(perm))
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();

    AutoPtr<IAuthenticatorDescription> value;
    String accountType;
    account->GetType(&accountType);
    AutoPtr<IAuthenticatorDescriptionHelper> descHelper;
    FAIL_RETURN(CAuthenticatorDescriptionHelper::AcquireSingleton(
            (IAuthenticatorDescriptionHelper**)&descHelper))
    descHelper->NewKey(accountType, (IAuthenticatorDescription**)&value);
    AutoPtr<IRegisteredServicesCacheServiceInfo> authenticatorInfo;
    mAuthenticatorCache->GetServiceInfo(value, accounts->mUserId, (IRegisteredServicesCacheServiceInfo**)&authenticatorInfo);
    Boolean customTokens = FALSE;
    if (authenticatorInfo != NULL) {
        AutoPtr<IInterface> type;
        authenticatorInfo->GetType((IInterface**)&type);
        IAuthenticatorDescription::Probe(type)->GetCustomTokens(&customTokens);
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
    do {
        // if the caller has permission, do the peek. otherwise go the more expensive
        // route of starting a Session
        if (!customTokens && permissionGranted) {
            String authToken = ReadAuthTokenInternal(accounts, account, authTokenType);
            if (authToken != NULL) {
                AutoPtr<IBundle> result;
                ec = CBundle::New((IBundle**)&result);
                if (FAILED(ec)) break;
                result->PutString(IAccountManager::KEY_AUTHTOKEN, authToken);
                String name, type;
                account->GetName(&name);
                account->GetType(&type);
                result->PutString(IAccountManager::KEY_ACCOUNT_NAME, name);
                result->PutString(IAccountManager::KEY_ACCOUNT_TYPE, type);
                ec = OnResult(response, result);
                break;
            }
        }

        AutoPtr<CGetAuthTokenSession> session;
        ec = CGetAuthTokenSession::NewByFriend(TO_IINTERFACE(accounts), response,
                account, authTokenType, expectActivityLaunch, loginOptions,
                notifyOnAuthFailure, permissionGranted, customTokens,
                this, (CGetAuthTokenSession**)&session);
        if (FAILED(ec)) break;
        ec = session->Bind();
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::CreateNoCredentialsPermissionNotification(
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
    FAIL_RETURN(CNotification::New(R::drawable::stat_sys_warning, NULL,
            0 /* when */, (INotification**)&n))
    AutoPtr<ICharSequence> cs;
    String name;
    account->GetName(&name);
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<ArrayOf<ICharSequence*> > args = ArrayOf<ICharSequence*>::Alloc(1);
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
    AutoPtr<IContext> contextForUser;
    GetContextForUser(user, (IContext**)&contextForUser);
    AutoPtr<IResources> res;
    contextForUser->GetResources((IResources**)&res);
    Int32 color;
    res->GetColor(
            R::color::system_notification_accent_color, &color);
    n->SetColor(color);
    AutoPtr<IPendingIntentHelper> piHelper;
    assert(0);
    // FAIL_RETURN(CPendingIntentHelper::AcquireSingleton(
    //         (IPendingIntentHelper**)&piHelper))
    AutoPtr<IPendingIntent> pendingIntent;
    piHelper->GetActivityAsUser(mContext, 0, intent,
            IPendingIntent::FLAG_CANCEL_CURRENT, NULL, user, (IPendingIntent**)&pendingIntent);
    AutoPtr<ICharSequence> titleCs, subtitleCs;
    CString::New(title, (ICharSequence**)&titleCs);
    CString::New(subtitle, (ICharSequence**)&subtitleCs);
    n->SetLatestEventInfo(contextForUser, titleCs, subtitleCs, pendingIntent);
    AutoPtr<IInteger32> notificationId = GetCredentialPermissionNotificationId(
            account, authTokenType, uid);
    Int32 value;
    notificationId->GetValue(&value);
    InstallNotification(value, n, user);
    return NOERROR;
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
    IObject::Probe(GetCredentialPermissionNotificationId(account, authTokenType, uid))->ToString(&s);
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

    synchronized(accounts->mCredentialsPermissionNotificationIds) {
        AutoPtr<IPair> pair;
        CPair::New(account, StringUtils::ParseCharSequence(authTokenType), (IPair**)&pair);
        CInteger32::New(uid, (IInteger32**)&id);
        AutoPtr<IPair> key;
        CPair::New(pair, id, (IPair**)&key);
        AutoPtr<IInterface> obj;
        accounts->mCredentialsPermissionNotificationIds->Get(key, (IInterface**)&obj);
        id = IInteger32::Probe(obj);
        if (id == NULL) {
            Int32 value;
            mNotificationIds->IncrementAndGet(&value);
            CInteger32::New(value, (IInteger32**)&id);
            accounts->mCredentialsPermissionNotificationIds->Put(key, id);
        }
    }
    return id;
}

Int32 CAccountManagerService::GetSigninRequiredNotificationId(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account)
{
    AutoPtr<IInteger32> id;
    synchronized(accounts->mSigninRequiredNotificationIds) {
        AutoPtr<IInterface> obj;
        accounts->mSigninRequiredNotificationIds->Get(account, (IInterface**)&obj);
        id = IInteger32::Probe(obj);
        if (id == NULL) {
            Int32 result;
            mNotificationIds->IncrementAndGet(&result);
            CInteger32::New(result, (IInteger32**)&id);
            accounts->mSigninRequiredNotificationIds->Put(account, id);
        }
    }
    Int32 value;
    id->GetValue(&value);
    return value;
}

ECode CAccountManagerService::AddAccount(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& accountType,
    /* [in] */ const String& authTokenType,
    /* [in] */ ArrayOf<String>* requiredFeatures,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IBundle* optionsIn)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "addAccount: accountType %s"
                ", response %s"
                ", authTokenType %s"
                ", requiredFeatures %s"
                ", expectActivityLaunch %s"
                ", caller's uid %d"
                ", pid %d",
                accountType.string(),
                TO_CSTR(response),
                authTokenType.string(),
                StringArrayToString(requiredFeatures).string(),
                StringUtils::BooleanToString(expectActivityLaunch).string(),
                Binder::GetCallingUid(),
                Binder::GetCallingPid());
    }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckManageAccountsPermission())

    // Is user disallowed from modifying accounts?
    Int32 userId;
    Binder::GetCallingUserHandle()->GetIdentifier(&userId);;
    if (!CanUserModifyAccounts(userId)) {
        // try {
        ECode ec = response->OnError(IAccountManager::ERROR_CODE_USER_RESTRICTED,
                String("User is not allowed to add an account!"));
        // } catch (RemoteException re) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec)
                return ec;
        }
        // }
        ShowCantAddAccount(IAccountManager::ERROR_CODE_USER_RESTRICTED, userId);
        return NOERROR;
    }
    if (!CanUserModifyAccountsForType(userId, accountType)) {
        // try {
        ECode ec = response->OnError(IAccountManager::ERROR_CODE_MANAGEMENT_DISABLED_FOR_ACCOUNT_TYPE,
                String("User cannot modify accounts of this type (policy)."));
        // } catch (RemoteException re) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec)
                return ec;
        }
        // }
        ShowCantAddAccount(IAccountManager::ERROR_CODE_MANAGEMENT_DISABLED_FOR_ACCOUNT_TYPE,
                userId);
        return NOERROR;
    }

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
    ECode ec;
    do {
        AutoPtr<CAddAcountSession> session;
        ec = CAddAcountSession::NewByFriend(TO_IINTERFACE(accounts), response,
                accountType, expectActivityLaunch, authTokenType, *requiredFeatures, options,
                this, (CAddAcountSession**)&session);
        if (FAILED(ec)) break;
        ec = session->Bind();
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::AddAccountAsUser(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& accountType,
    /* [in] */ const String& authTokenType,
    /* [in] */ ArrayOf<String>* requiredFeatures,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IBundle* optionsIn,
    /* [in] */ Int32 userId)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "addAccount: accountType %s, response %s, authTokenType %s"
                ", requiredFeatures %s, expectActivityLaunch %s"
                ", caller's uid %d, pid %d, for user id %d",
                accountType.string(), TO_CSTR(response), authTokenType.string(),
                StringArrayToString(requiredFeatures).string(),
                StringUtils::ToString(expectActivityLaunch).string(),
                Binder::GetCallingUid(), Binder::GetCallingPid(), userId);
    }
    if (response == NULL) {
        Logger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (accountType == NULL) {
        Logger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    CheckManageAccountsPermission();

    // Only allow the system process to add accounts of other users
    String s;
    s.AppendFormat("User %d trying to add account for %d", UserHandle::GetCallingUserId(), userId);
    EnforceCrossUserPermission(userId, s);

    // Is user disallowed from modifying accounts?
    if (!CanUserModifyAccounts(userId)) {
        // try {
        ECode ec = response->OnError(IAccountManager::ERROR_CODE_USER_RESTRICTED,
                String("User is not allowed to add an account!"));
        // } catch (RemoteException re) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec)
                return ec;
        }
        // }
        ShowCantAddAccount(IAccountManager::ERROR_CODE_USER_RESTRICTED, userId);
        return NOERROR;
    }
    if (!CanUserModifyAccountsForType(userId, accountType)) {
        // try {
        ECode ec = response->OnError(IAccountManager::ERROR_CODE_MANAGEMENT_DISABLED_FOR_ACCOUNT_TYPE,
                String("User cannot modify accounts of this type (policy)."));
        // } catch (RemoteException re) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec)
                return ec;
        }
        // }
        ShowCantAddAccount(IAccountManager::ERROR_CODE_MANAGEMENT_DISABLED_FOR_ACCOUNT_TYPE,
                userId);
        return NOERROR;
    }

    AutoPtr<UserAccounts> accounts = GetUserAccounts(userId);
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
    ECode ec;
    do {
        AutoPtr<CAddAccountAsUserSession> session;
        ec = CAddAccountAsUserSession::NewByFriend(TO_IINTERFACE(accounts), response, accountType, expectActivityLaunch,
                TRUE /* stripAuthTokenFromResult */, this, authTokenType, requiredFeatures, options, (CAddAccountAsUserSession**)&session);
        if (FAILED(ec)) break;
        ec = session->Bind();
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::ShowCantAddAccount(
    /* [in] */ Int32 errorCode,
    /* [in] */ Int32 userId)
{
    AutoPtr<IIntent> cantAddAccount;
    CIntent::New(mContext, ECLSID_CCantAddAccountActivity, (IIntent**)&cantAddAccount);
    cantAddAccount->PutExtra(ICantAddAccountActivity::EXTRA_ERROR_CODE, errorCode);
    cantAddAccount->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = mContext->StartActivityAsUser(cantAddAccount, new UserHandle(userId));
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
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
    String s;
    s.AppendFormat("User %d trying to confirm account credentials for %d", UserHandle::GetCallingUserId(), userId);
    EnforceCrossUserPermission(userId, s);

    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "confirmCredentials: %s"
                ", response %s"
                ", expectActivityLaunch %s"
                ", caller's uid %d"
                ", pid %d",
                TO_CSTR(account), TO_CSTR(response),
                StringUtils::BooleanToString(expectActivityLaunch).string(),
                Binder::GetCallingUid(),
                Binder::GetCallingPid());
    }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckManageAccountsPermission())
    AutoPtr<UserAccounts> accounts = GetUserAccounts(userId);
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        AutoPtr<CConfirmCredentialsAsUserSession> session;
        ec = CConfirmCredentialsAsUserSession::NewByFriend(TO_IINTERFACE(accounts),
                response, expectActivityLaunch, account, options,
                this, (CConfirmCredentialsAsUserSession**)&session);
        if (FAILED(ec)) break;
        ec = session->Bind();
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::UpdateCredentials(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IBundle* options)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "updateCredentials: %s"
                ", response %s"
                ", authTokenType %s"
                ", expectActivityLaunch %s"
                ", caller's uid %d"
                ", pid %d", TO_CSTR(account), TO_CSTR(response), authTokenType.string(),
                StringUtils::BooleanToString(expectActivityLaunch).string(),
                Binder::GetCallingUid(),
                Binder::GetCallingPid());
    }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckManageAccountsPermission())
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        AutoPtr<CUpdateCredentialsSession> session;
        ec = CUpdateCredentialsSession::NewByFriend(TO_IINTERFACE(accounts), response,
                expectActivityLaunch, account, authTokenType, options,
                this, (CUpdateCredentialsSession**)&session);
        if (FAILED(ec)) break;
        ec = session->Bind();
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::EditProperties(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& accountType,
    /* [in] */ Boolean expectActivityLaunch)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "editProperties: accountType %s"
                ", response %s"
                ", expectActivityLaunch %s"
                ", caller's uid %d"
                ", pid %d",
                accountType.string(), TO_CSTR(response),
                StringUtils::BooleanToString(expectActivityLaunch).string(),
                Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckManageAccountsPermission())
    AutoPtr<UserAccounts> accounts = GetUserAccountsForCaller();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        AutoPtr<CEditPropertiesSession> session;
        ec = CEditPropertiesSession::NewByFriend(TO_IINTERFACE(accounts), response,
                accountType, expectActivityLaunch, this,
                (CEditPropertiesSession**)&session);
        if (FAILED(ec)) break;
        ec = session->Bind();
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::GetAccounts(
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<IAccount*>** _accounts)
{
    VALIDATE_NOT_NULL(_accounts);

    FAIL_RETURN(CheckReadAccountsPermission())
    AutoPtr<UserAccounts> userAccounts = GetUserAccounts(userId);
    Int32 callingUid = Binder::GetCallingUid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    synchronized(userAccounts->mCacheLock) {
        AutoPtr<ArrayOf<IAccount*> > accounts;
        ec = GetAccountsFromCacheLocked(
                userAccounts, String(NULL), callingUid, String(NULL), (ArrayOf<IAccount*>**)&accounts);
        *_accounts = accounts;
        REFCOUNT_ADD(*_accounts);
    }
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::GetRunningAccounts(
    /* [out, callee] */ ArrayOf<IAccountAndUser*>** accounts)
{
    VALIDATE_NOT_NULL(accounts);
    *accounts = NULL;

    AutoPtr<ArrayOf<Int32> > runningUserIds;
    // try {
    ECode ec = ActivityManagerNative::GetDefault()->GetRunningUserIds(
            (ArrayOf<Int32>**)&runningUserIds);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            // Running in system_server; should never happen
            Logger::E(TAG, "%d", ec);
            return E_RUNTIME_EXCEPTION;
        }
    }
    // }
    return GetAccounts(*runningUserIds, accounts);
}

ECode CAccountManagerService::GetAllAccounts(
    /* [out, callee] */ ArrayOf<IAccountAndUser*>** accounts)
{
    VALIDATE_NOT_NULL(accounts);

    AutoPtr<IList> users;
    GetUserManager()->GetUsers((IList**)&users);
    Int32 count;
    users->GetSize(&count);
    AutoPtr<ArrayOf<Int32> > userIds = ArrayOf<Int32>::Alloc(count);
    AutoPtr<IIterator> it;
    users->GetIterator((IIterator**)&it);
    Boolean hasNext;
    Int32 i = 0;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> info;
        it->GetNext((IInterface**)&info);
        IUserInfo::Probe(info)->GetId(&(*userIds)[i]);
    }
    return GetAccounts(*userIds, accounts);
}

ECode CAccountManagerService::GetAccounts(
    /* [in] */ const ArrayOf<Int32>& userIds,
    /* [out, callee] */ ArrayOf<IAccountAndUser*>** accounts)
{
    VALIDATE_NOT_NULL(accounts);

    AutoPtr<ArrayOf<IAccountAndUser*> > runningAccounts;
    for (Int32 i = 0; i < userIds.GetLength(); ++i) {
        Int32 userId = userIds[i];
        AutoPtr<UserAccounts> userAccounts = GetUserAccounts(userId);
        if (userAccounts == NULL) continue;
        {
            AutoLock lock(userAccounts->mCacheLock);
            AutoPtr<ArrayOf<IAccount*> > accounts;
            GetAccountsFromCacheLocked(userAccounts, String(NULL),
                            Binder::GetCallingUid(), String(NULL), (ArrayOf<IAccount*>**)&accounts);
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
    /* [in] */ const String& type,
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<IAccount*>** _accounts)
{
    return GetAccountsAsUser(type, userId, String(NULL), -1, _accounts);
}

ECode CAccountManagerService::GetAccountsAsUser(
    /* [in] */ const String& type,
    /* [in] */ Int32 userId,
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 packageUid,
    /* [out, callee] */ ArrayOf<IAccount*>** _accounts)
{
    VALIDATE_NOT_NULL(_accounts);
    *_accounts = NULL;

    Int32 callingUid = Binder::GetCallingUid();
    // Only allow the system process to read accounts of other users
    Int32 callingOrSelfPermission;
    mContext->CheckCallingOrSelfPermission(
                    Manifest::permission::INTERACT_ACROSS_USERS_FULL, &callingOrSelfPermission);
    if (userId != UserHandle::GetCallingUserId()
                && callingUid != Process::MyUid()
                && callingOrSelfPermission != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E(TAG, "User %d trying to get account for %d", UserHandle::GetCallingUserId(), userId);
        return E_SECURITY_EXCEPTION;
    }

    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "getAccounts: accountType %s"
                ", caller's uid %d"
                ", pid %d", type.string(), Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    // If the original calling app was using the framework account chooser activity, we'll
    // be passed in the original caller's uid here, which is what should be used for filtering.
    if (packageUid != -1 && UserHandle::IsSameApp(callingUid, Process::MyUid())) {
        callingUid = packageUid;
    }
    FAIL_RETURN(CheckReadAccountsPermission())
    AutoPtr<UserAccounts> userAccounts = GetUserAccounts(userId);
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        synchronized(userAccounts->mCacheLock) {
            ec = GetAccountsFromCacheLocked(
                    userAccounts, type, callingUid, callingPackage, _accounts);
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::AddSharedAccountAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    HandleIncomingUser(userId, &userId);
    AutoPtr<ISQLiteDatabase> db;
    GetUserAccounts(userId)->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    String name;
    account->GetName(&name);
    values->Put(ACCOUNTS_NAME, name);
    String type;
    account->GetType(&type);
    values->Put(ACCOUNTS_TYPE, type);
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    array->Set(0, name);
    array->Set(1, type);
    Int32 iNoUse;
    db->Delete(TABLE_SHARED_ACCOUNTS, ACCOUNTS_NAME + String("=? AND ") + ACCOUNTS_TYPE+ String("=?"), array, &iNoUse);
    Int64 accountId;
    db->Insert(TABLE_SHARED_ACCOUNTS, ACCOUNTS_NAME, values, &accountId);
    if (accountId < 0) {
        Logger::W(TAG, "insertAccountIntoDatabase: %s, skipping the DB insert failed",
                TO_CSTR(account));
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode CAccountManagerService::RenameSharedAccountAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& newName,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    HandleIncomingUser(userId, &userId);
    AutoPtr<UserAccounts> accounts = GetUserAccounts(userId);
    AutoPtr<ISQLiteDatabase> db;
    accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(ACCOUNTS_NAME, newName);
    String name;
    account->GetName(&name);
    values->Put(ACCOUNTS_PREVIOUS_NAME, name);
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    array->Set(0, name);
    String type;
    account->GetType(&type);
    array->Set(1, type);
    Int32 r;
    db->Update(
            TABLE_SHARED_ACCOUNTS,
            values,
            ACCOUNTS_NAME + String("=? AND ") + ACCOUNTS_TYPE+ String("=?"),
            array, &r);
    if (r > 0) {
        // Recursively rename the account.
        AutoPtr<IAccount> iNoUse;
        RenameAccountInternal(accounts, account, newName, (IAccount**)&iNoUse);
    }
    *result = r > 0;
    return NOERROR;
}

ECode CAccountManagerService::RemoveSharedAccountAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    HandleIncomingUser(userId, &userId);
    AutoPtr<UserAccounts> accounts = GetUserAccounts(userId);
    AutoPtr<ISQLiteDatabase> db;
    accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    String name;
    account->GetName(&name);
    array->Set(0, name);
    String type;
    account->GetType(&type);
    array->Set(1, type);
    Int32 r;
    db->Delete(TABLE_SHARED_ACCOUNTS, ACCOUNTS_NAME + String("=? AND ") + ACCOUNTS_TYPE+ String("=?"),
            array, &r);
    if (r > 0) {
        RemoveAccountInternal(accounts, account);
    }
    *result = r > 0;
    return NOERROR;
}

ECode CAccountManagerService::GetSharedAccountsAsUser(
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<IAccount*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    HandleIncomingUser(userId, &userId);
    AutoPtr<UserAccounts> accounts = GetUserAccounts(userId);
    AutoPtr<IArrayList> accountList;
    CArrayList::New((IArrayList**)&accountList);
    AutoPtr<ICursor> cursor;
    // try {
    ECode ec;
    do {
        AutoPtr<ISQLiteDatabase> db;
        ec = accounts->mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
        if (FAILED(ec)) break;
        AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
        array->Set(0, ACCOUNTS_NAME);
        array->Set(1, ACCOUNTS_TYPE);
        ec = db->Query(TABLE_SHARED_ACCOUNTS, array,
                String(NULL), NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);
        if (FAILED(ec)) break;
        if (cursor != NULL) {
            Boolean isMoveToFirstOk;
            ec = cursor->MoveToFirst(&isMoveToFirstOk);
            if (FAILED(ec)) break;
            if (isMoveToFirstOk) {
                Int32 nameIndex;
                ec = cursor->GetColumnIndex(ACCOUNTS_NAME, &nameIndex);
                if (FAILED(ec)) break;
                Int32 typeIndex;
                ec = cursor->GetColumnIndex(ACCOUNTS_TYPE, &typeIndex);
                if (FAILED(ec)) break;
                Boolean isMoveToNextOk;
                do {
                    String name;
                    ec = cursor->GetString(nameIndex, &name);
                    if (FAILED(ec)) break;
                    String type;
                    ec = cursor->GetString(typeIndex, &type);
                    if (FAILED(ec)) break;
                    AutoPtr<IAccount> account;
                    ec = CAccount::New(name, type, (IAccount**)&account);
                    if (FAILED(ec)) break;
                    ec = accountList->Add(account);
                    if (FAILED(ec)) break;
                } while (cursor->MoveToNext(&isMoveToNextOk), isMoveToNextOk);
            }
        }
    } while(FALSE);
    // } finally {
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    if (FAILED(ec)) return ec;
    // }
    Int32 size;
    accountList->GetSize(&size);
    AutoPtr<ArrayOf<IAccount*> > accountArray = ArrayOf<IAccount*>::Alloc(size);
    AutoPtr<ArrayOf<IInterface*> > tmpArray;
    accountList->ToArray((ArrayOf<IInterface*>**)&tmpArray);
    for (Int32 i = 0; i < size; ++i) {
        accountArray->Set(i, IAccount::Probe((*tmpArray)[i]));
    }
    *result = accountArray;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CAccountManagerService::GetAccountsForPackage(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [out, callee] */ ArrayOf<IAccount*>** accounts)
{
    VALIDATE_NOT_NULL(accounts)
    *accounts = NULL;

    Int32 callingUid = Binder::GetCallingUid();
    if (!UserHandle::IsSameApp(callingUid, Process::MyUid())) {
        Logger::E(TAG, "getAccountsForPackage() called from unauthorized uid %d with uid=%d", callingUid, uid);
        return E_SECURITY_EXCEPTION;
    }
    return GetAccountsAsUser(String(NULL), UserHandle::GetCallingUserId(), packageName, uid, accounts);
}

ECode CAccountManagerService::GetAccountsByTypeForPackage(
    /* [in] */ const String& type,
    /* [in] */ const String& packageName,
    /* [out, callee] */ ArrayOf<IAccount*>** accounts)
{
    VALIDATE_NOT_NULL(accounts)
    *accounts = NULL;

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    array->Set(0, Manifest::permission::INTERACT_ACROSS_USERS);
    CheckBinderPermission(array);
    Int32 packageUid = -1;
    // try {
    ECode ec = AppGlobals::GetPackageManager()->GetPackageUid(
                packageName, UserHandle::GetCallingUserId(), &packageUid);
    // } catch (RemoteException re) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "Couldn't determine the packageUid for %s %d", packageName.string(), ec);
            *accounts = ArrayOf<IAccount*>::Alloc(0);
            REFCOUNT_ADD(*accounts);
            return NOERROR;
        }
        else
            return ec;
    }
    // }
    return GetAccountsAsUser(type, UserHandle::GetCallingUserId(), packageName, packageUid, accounts);
}

ECode CAccountManagerService::GetAccounts(
    /* [in] */ const String& accountType,
    /* [out, callee] */ ArrayOf<IAccount*>** accounts)
{
    return GetAccountsAsUser(accountType, UserHandle::GetCallingUserId(), accounts);
}

ECode CAccountManagerService::GetAccountsByFeatures(
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& accountType,
    /* [in] */ ArrayOf<String>* features)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "getAccounts: accountType , response %s, features %s, caller's uid %d, pid %d",
                accountType.string(), TO_CSTR(response),
                StringArrayToString(features).string(),
                Binder::GetCallingUid(), Binder::GetCallingPid());
    }
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckReadAccountsPermission())
    AutoPtr<UserAccounts> userAccounts = GetUserAccountsForCaller();
    Int32 callingUid = Binder::GetCallingUid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        if (features == NULL || features->GetLength() == 0) {
            AutoPtr<ArrayOf<IAccount*> > accounts;
            synchronized(userAccounts->mCacheLock) {
                GetAccountsFromCacheLocked(userAccounts, accountType, callingUid, String(NULL), (ArrayOf<IAccount*>**)&accounts);
            }
            AutoPtr<IBundle> result;
            ec = CBundle::New((IBundle**)&result);
            if (FAILED(ec)) break;
            AutoPtr<ArrayOf<IParcelable*> > parcels = ArrayOf<IParcelable*>::Alloc(
                    accounts->GetLength());
            for (Int32 i = 0; i < accounts->GetLength(); ++i) {
                parcels->Set(i, IParcelable::Probe((*accounts)[i]));
            }
            result->PutParcelableArray(IAccountManager::KEY_ACCOUNTS, parcels);
            ec = OnResult(response, result);
            break;
        }
        AutoPtr<CGetAccountsByTypeAndFeatureSession> session;
        ec = CGetAccountsByTypeAndFeatureSession::NewByFriend(TO_IINTERFACE(userAccounts),
                response, accountType, *features, callingUid, this,
                (CGetAccountsByTypeAndFeatureSession**)&session);
        if (FAILED(ec)) break;
        ec = session->Bind();
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::GetAccountIdLocked(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IAccount* account,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICursor> cursor;
    AutoPtr<ArrayOf<String> > ss1 = ArrayOf<String>::Alloc(1);
    (*ss1)[0] = ACCOUNTS_ID;
    AutoPtr<ArrayOf<String> > ss2 = ArrayOf<String>::Alloc(2);
    account->GetName(&(*ss2)[0]);
    account->GetType(&(*ss2)[1]);
    String nullStr;
    FAIL_RETURN(db->Query(TABLE_ACCOUNTS, ss1, String("name=? AND type=?"),
            ss2, nullStr, nullStr, nullStr, (ICursor**)&cursor))
    // try {
    ECode ec;
    do {
        Boolean isMoveToNextOk;
        cursor->MoveToNext(&isMoveToNextOk);
        if (isMoveToNextOk) {
            ec = cursor->GetInt64(0, result);
            break;
        }
        *result = -1;
    } while(FALSE);
    // } finally {
    ICloseable::Probe(cursor)->Close();
    return ec;
    // }
}

ECode CAccountManagerService::GetExtrasIdLocked(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int64 accountId,
    /* [in] */ const String& key,
    /* [out] */ Int64* result)
{
    AutoPtr<ICursor> cursor;
    AutoPtr<ArrayOf<String> > ss1 = ArrayOf<String>::Alloc(1);
    (*ss1)[0] = EXTRAS_ID;
    AutoPtr<ArrayOf<String> > ss2 = ArrayOf<String>::Alloc(1);
    (*ss2)[0] = key;
    String nullStr;
    FAIL_RETURN(db->Query(TABLE_EXTRAS, ss1,
            EXTRAS_ACCOUNTS_ID + String("=") + StringUtils::ToString(accountId)
            + String(" AND ") + EXTRAS_KEY + String("=?"),
            ss2, nullStr, nullStr, nullStr, (ICursor**)&cursor))
    // try {
    ECode ec;
    do {
        Boolean isMoveToNextOk;
        cursor->MoveToNext(&isMoveToNextOk);
        if (isMoveToNextOk) {
            ec = cursor->GetInt64(0, result);
            break;
        }
        *result = -1;
    } while(FALSE);
    // } finally {
    ICloseable::Probe(cursor)->Close();
    return ec;
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
                    Slogger::E(TAG, "User dir cannot be created: %s", TO_CSTR(userDir));
                    *name = String(NULL);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
            }
            Boolean succeeded;
            if (oldFile->RenameTo(databaseFile, &succeeded), !succeeded) {
                Slogger::E(TAG, "User dir cannot be migrated: %s", TO_CSTR(databaseFile));
                *name = String(NULL);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }
    return databaseFile->GetPath(name);
}

ECode CAccountManagerService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)

    *binder = IBinder::Probe(this);
    REFCOUNT_ADD(*binder)
    return NOERROR;
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
//         fout.println(String("Permission Denial: can't dump AccountsManager from from pid=")
//                 + Binder::GetCallingPid() + String(", uid=") + Binder::GetCallingUid()
//                 + String(" without permission ") + Elastos::Droid::Manifest::permission::DUMP);
//         return;
//     }
//     Boolean isCheckinRequest = scanArgs(args, String("--checkin")) || scanArgs(args, String("-c"));
//     final IndentingPrintWriter ipw = new IndentingPrintWriter(fout, String("  "));

//     final List<UserInfo> users = getUserManager().getUsers();
//     for (UserInfo user : users) {
//         ipw.println(String("User ") + user + String(":"));
//         ipw.increaseIndent();
//         dumpUser(getUserAccounts(user.id), fd, ipw, args, isCheckinRequest);
//         ipw.println();
//         ipw.decreaseIndent();
//     }
// }

// private void dumpUser(UserAccounts userAccounts, FileDescriptor fd, PrintWriter fout,
//         String[] args, Boolean isCheckinRequest) {
//     synchronized(userAccounts.cacheLock) {
//         final SQLiteDatabase db = userAccounts.openHelper.getReadableDatabase();

//         if (isCheckinRequest) {
//             // This is a checkin request. *Only* upload the account types and the count of each.
//             Cursor cursor = db.query(TABLE_ACCOUNTS, ACCOUNT_TYPE_COUNT_PROJECTION,
//                     null, null, ACCOUNTS_TYPE, null, null);
//             try {
//                 while (cursor.moveToNext()) {
//                     // print type,count
//                     fout.println(cursor.getString(0) + String(",") + cursor.getString(1));
//                 }
//             } finally {
//                 if (cursor != null) {
//                     cursor.close();
//                 }
//             }
//         } else {
//             Account[] accounts = getAccountsFromCacheLocked(userAccounts, null /* type */,
//                     Process::MyUid(), null);
//             fout.println(String("Accounts: ") + accounts.length);
//             for (Account account : accounts) {
//                 fout.println(String("  ") + account);
//             }

//             fout.println();
//             synchronized(mSessions) {
//                 final long now = SystemClock::ElapsedRealtime();
//                 fout.println(String("Active Sessions: ") + mSessions.size());
//                 for (Session session : mSessions.values()) {
//                     fout.println(String("  ") + session.toDebugString(now));
//                 }
//             }

//             fout.println();
//             mAuthenticatorCache.dump(fd, fout, args, userAccounts.userId);
//         }
//     }
// }

ECode CAccountManagerService::DoNotification(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ ICharSequence* message,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 userId)
{
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::V(TAG, "doNotification: %s intent:%s", TO_CSTR(message), TO_CSTR(intent));
        }

        AutoPtr<IComponentName> component;
        String className;
        if ((intent->GetComponent((IComponentName**)&component), component != NULL) &&
                (component->GetClassName(&className), className.Equals("GrantCredentialsPermissionActivity"))) {
            CreateNoCredentialsPermissionNotification(account, intent, userId);
        }
        else {
            Int32 notificationId = GetSigninRequiredNotificationId(accounts, account);
            intent->AddCategory(StringUtils::ToString(notificationId));
            AutoPtr<INotification> n;
            ec = CNotification::New(R::drawable::stat_sys_warning, NULL,
                    0 /* when */, (INotification**)&n);
            if (FAILED(ec)) break;
            AutoPtr<IUserHandle> user;
            CUserHandle::New(userId, (IUserHandle**)&user);
            AutoPtr<IContext> contextForUser;
            GetContextForUser(user, (IContext**)&contextForUser);
            AutoPtr<ICharSequence> cs;
            contextForUser->GetText(R::string::notification_title, (ICharSequence**)&cs);
            String notificationTitleFormat;
            cs->ToString(&notificationTitleFormat);
            AutoPtr<IResources> res;
            contextForUser->GetResources((IResources**)&res);
            Int32 color;
            res->GetColor(
                    R::color::system_notification_accent_color, &color);
            n->SetColor(color);
            AutoPtr<ICharSequence> namecs;
            String name;
            account->GetName(&name);
            CString::New(name, (ICharSequence**)&namecs);
            AutoPtr<IPendingIntentHelper> helper;
            assert(0);
            // FAIL_RETURN(CPendingIntentHelper::AcquireSingleton(
            //         (IPendingIntentHelper**)&helper))
            AutoPtr<IPendingIntent> pi;
            ec = helper->GetActivityAsUser(
                    mContext, 0, intent, IPendingIntent::FLAG_CANCEL_CURRENT,
                    NULL, user, (IPendingIntent**)&pi);
            if (FAILED(ec)) break;
            ec = n->SetLatestEventInfo(mContext,
                    namecs/*String.format(notificationTitleFormat, account.name)*/,
                    message, pi);
            if (FAILED(ec)) break;
            InstallNotification(notificationId, n, user);
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

void CAccountManagerService::InstallNotification(
    /* [in] */ Int32 notificationId,
    /* [in] */ INotification* n,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
    AutoPtr<INotificationManager> nm = INotificationManager::Probe(obj);
    nm->NotifyAsUser(String(NULL), notificationId, n, user);
}

ECode CAccountManagerService::CancelNotification(
    /* [in] */ Int32 id,
    /* [in] */ IUserHandle* user)
{
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        AutoPtr<IInterface> obj;
        ec = mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
        if (FAILED(ec)) break;
        AutoPtr<INotificationManager> nm = INotificationManager::Probe(obj);

        ec = nm->CancelAsUser(String(NULL), id, user);
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
    // }
}

ECode CAccountManagerService::CheckBinderPermission(
    /* [in] */ ArrayOf<String>* permissions)
{
    Int32 uid = Binder::GetCallingUid();

    for (Int32 i = 0; i < permissions->GetLength(); ++i) {
        String perm = (*permissions)[i];
        Int32 value;
        FAIL_RETURN(mContext->CheckCallingOrSelfPermission(perm, &value))
        if (value == IPackageManager::PERMISSION_GRANTED) {
            if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                Logger::V(TAG, "  caller uid %d has %s", uid, perm.string());
            }
            return NOERROR;
        }
    }

    String msg;
    msg.AppendFormat("caller uid %d lacks any of ,%s", uid, Arrays::ToString(permissions).string());
    Logger::W(TAG, "  %s", msg.string());
    Logger::E(TAG, msg.string());
    return E_SECURITY_EXCEPTION;
}

ECode CAccountManagerService::HandleIncomingUser(
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    // try {
    ECode ec = ActivityManagerNative::GetDefault()->HandleIncomingUser(
            Binder::GetCallingPid(), Binder::GetCallingUid(), userId, TRUE, TRUE, String(""), String(NULL), result);
    // } catch (RemoteException re) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            // Shouldn't happen, local.
        }
        else
            return ec;
    }
    // }
    *result = userId;
    return NOERROR;
}

ECode CAccountManagerService::IsPrivileged(
    /* [in] */ Int32 callingUid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IPackageManager> userPackageManager;
    // try {
    ECode ec;
    do {
        AutoPtr<IUserHandle> userH;
        CUserHandle::New(UserHandle::GetUserId(callingUid), (IUserHandle**)&userH);
        AutoPtr<IContext> context;
        ec = mContext->CreatePackageContextAsUser(
                String("android"), 0, userH, (IContext**)&context);
        if (FAILED(ec)) break;
        ec = context->GetPackageManager((IPackageManager**)&userPackageManager);
    } while(FALSE);
    if (FAILED(ec)) {
        if ((ECode) E_NAME_NOT_FOUND_EXCEPTION == ec) {
            *result = FALSE;
            return NOERROR;
        }
        return ec;
    }

    AutoPtr<ArrayOf<String> > packages;
    userPackageManager->GetPackagesForUid(callingUid, (ArrayOf<String>**)&packages);
    for (Int32 i = 0; i < packages->GetLength(); ++i) {
        // try {
        do {
            AutoPtr<IPackageInfo> packageInfo;
            ec = userPackageManager->GetPackageInfo((*packages)[i], 0 /* flags */,
                    (IPackageInfo**)&packageInfo);
            if (FAILED(ec)) break;
            if (packageInfo != NULL) {
                AutoPtr<IApplicationInfo> appInfo;
                ec = packageInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
                if (FAILED(ec)) break;
                Int32 flags;
                appInfo->GetFlags(&flags);
                if ((flags & IApplicationInfo::FLAG_PRIVILEGED) != 0) {
                    *result = TRUE;
                    return NOERROR;
                }
            }
        } while(FALSE);
        if (FAILED(ec)) {
            if ((ECode) E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION == ec) {
                *result = FALSE;
                return NOERROR;
            }
            return ec;
        }
    }
    *result = FALSE;
    return NOERROR;
}

Boolean CAccountManagerService::PermissionIsGranted(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Int32 callerUid)
{
    Boolean isPrivileged;
    IsPrivileged(callerUid, &isPrivileged);
    Boolean fromAuthenticator = FALSE;
    if (account != NULL) {
        String type;
        account->GetType(&type);
        fromAuthenticator = HasAuthenticatorUid(type, callerUid);
    }
    Boolean hasExplicitGrants = account != NULL
            && HasExplicitlyGrantedPermission(account, authTokenType, callerUid);
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "checkGrantsOrCallingUidAgainstAuthenticator: caller uid %d"
                ", %s: is authenticator? %s, has explicit permission? %s",
                callerUid,
                TO_CSTR(account),
                StringUtils::BooleanToString(fromAuthenticator).string(),
                StringUtils::BooleanToString(hasExplicitGrants).string());
    }
    return fromAuthenticator || hasExplicitGrants || isPrivileged;
}

Boolean CAccountManagerService::HasAuthenticatorUid(
    /* [in] */ const String& accountType,
    /* [in] */ Int32 callingUid)
{
    AutoPtr<ICollection> services;
    mAuthenticatorCache->GetAllServices(
            UserHandle::GetUserId(callingUid), (ICollection**)&services);
    AutoPtr<IIterator> it;
    services->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IRegisteredServicesCacheServiceInfo> serviceInfo = IRegisteredServicesCacheServiceInfo::Probe(obj);
        obj = NULL;
        serviceInfo->GetType((IInterface**)&obj);
        AutoPtr<IAuthenticatorDescription> type = IAuthenticatorDescription::Probe(obj);
        String descType;
        type->GetType(&descType);
        if (descType.Equals(accountType)) {
            Int32 result;
            Int32 uid;
            serviceInfo->GetUid(&uid);
            return (uid == callingUid) ||
                    (mPackageManager->CheckSignatures(uid, callingUid, &result),
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

    synchronized(accounts->mCacheLock) {
        AutoPtr<ISQLiteDatabase> db;
        accounts->mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
        AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(4);
        (*args)[0] = StringUtils::ToString(callerUid);
        (*args)[1] = authTokenType;
        account->GetName(&(*args)[2]);
        account->GetType(&(*args)[3]);
        AutoPtr<IDatabaseUtils> helper;
        FAIL_RETURN(CDatabaseUtils::AcquireSingleton((IDatabaseUtils**)&helper))
        Int64 result;
        helper->Int64ForQuery(db, COUNT_OF_MATCHING_GRANTS, args, &result);
        Boolean permissionGranted = result != 0;

        if (!permissionGranted) {
            AutoPtr<IActivityManagerHelper> activityHelper;
            FAIL_RETURN(CActivityManagerHelper::AcquireSingleton(
                    (IActivityManagerHelper**)&activityHelper))
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
    return FALSE;
}

ECode CAccountManagerService::CheckCallingUidAgainstAuthenticator(
    /* [in] */ IAccount* account)
{
    Int32 uid = Binder::GetCallingUid();
    String type;
    account->GetType(&type);
    if (account == NULL || !HasAuthenticatorUid(type, uid)) {
        String msg;
        msg.AppendFormat("caller uid %d is different than the authenticator's uid", uid);
        Logger::W(TAG, msg.string());
        Logger::E(TAG, msg.string());
        return E_SECURITY_EXCEPTION;
    }
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "caller uid %d is the same as the authenticator's uid", uid);
    }
    return NOERROR;
}

ECode CAccountManagerService::CheckAuthenticateAccountsPermission(
    /* [in] */ IAccount* account)
{
    AutoPtr<ArrayOf<String> > perms = ArrayOf<String>::Alloc(1);
    (*perms)[0] = Elastos::Droid::Manifest::permission::AUTHENTICATE_ACCOUNTS;
    FAIL_RETURN(CheckBinderPermission(perms))
    return CheckCallingUidAgainstAuthenticator(account);
}

ECode CAccountManagerService::CheckReadAccountsPermission()
{
    AutoPtr<ArrayOf<String> > perms = ArrayOf<String>::Alloc(1);
    (*perms)[0] = Elastos::Droid::Manifest::permission::GET_ACCOUNTS;
    return CheckBinderPermission(perms);
}

ECode CAccountManagerService::CheckManageAccountsPermission()
{
    AutoPtr<ArrayOf<String> > perms = ArrayOf<String>::Alloc(1);
    (*perms)[0] = Elastos::Droid::Manifest::permission::MANAGE_ACCOUNTS;
    return CheckBinderPermission(perms);
}

ECode CAccountManagerService::CheckManageAccountsOrUseCredentialsPermissions()
{
    AutoPtr<ArrayOf<String> > perms = ArrayOf<String>::Alloc(2);
    (*perms)[0] = Elastos::Droid::Manifest::permission::MANAGE_ACCOUNTS;
    (*perms)[1] = Elastos::Droid::Manifest::permission::USE_CREDENTIALS;
    return CheckBinderPermission(perms);
}

Boolean CAccountManagerService::CanUserModifyAccounts(
    /* [in] */ Int32 userId)
{
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(userId, (IUserHandle**)&userHandle);
    AutoPtr<IBundle> bundle;
    GetUserManager()->GetUserRestrictions(userHandle, (IBundle**)&bundle);
    Boolean isOk;
    bundle->GetBoolean(IUserManager::DISALLOW_MODIFY_ACCOUNTS, &isOk);
    if (isOk) {
        return FALSE;
    }
    return TRUE;
}

Boolean CAccountManagerService::CanUserModifyAccountsForType(
    /* [in] */ Int32 userId,
    /* [in] */ const String& accountType)
{
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&service);
    AutoPtr<IDevicePolicyManager> dpm = IDevicePolicyManager::Probe(service);
    AutoPtr<ArrayOf<String> > typesArray;
    dpm->GetAccountTypesWithManagementDisabledAsUser(userId, (ArrayOf<String>**)&typesArray);
    if (typesArray == NULL) {
        return TRUE;
    }
    for (Int32 i = 0; i < typesArray->GetLength(); ++i) {
        String forbiddenType = (*typesArray)[i];
        if (forbiddenType.Equals(accountType)) {
            return FALSE;
        }
    }
    return TRUE;
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
    }

    if (value) {
        GrantAppPermission(account, authTokenType, uid);
    }
    else {
        RevokeAppPermission(account, authTokenType, uid);
    }
    return NOERROR;
}

ECode CAccountManagerService::GrantAppPermission(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Int32 uid)
{
    if (account == NULL || authTokenType == NULL) {
        Slogger::E(TAG, "grantAppPermission: called with invalid arguments"/*, new Exception()*/);
        return NOERROR;
    }
    AutoPtr<UserAccounts> accounts = GetUserAccounts(UserHandle::GetUserId(uid));

    synchronized(accounts->mCacheLock) {
        AutoPtr<ISQLiteDatabase> db;
        FAIL_RETURN(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db))
        db->BeginTransaction();
        // try {
        ECode ec;
        do {
            Int64 accountId;
            GetAccountIdLocked(db, account, &accountId);
            if (accountId >= 0) {
                AutoPtr<IContentValues> values;
                ec = CContentValues::New((IContentValues**)&values);
                if (FAILED(ec)) break;
                AutoPtr<IInteger64> integer64;
                CInteger64::New(accountId, (IInteger64**)&integer64);
                values->Put(GRANTS_ACCOUNTS_ID, integer64);
                AutoPtr<ICharSequence> csq;
                CString::New(authTokenType, (ICharSequence**)&csq);
                values->Put(GRANTS_AUTH_TOKEN_TYPE, csq);
                AutoPtr<IInteger32> integer32;
                CInteger32::New(uid, (IInteger32**)&integer32);
                values->Put(GRANTS_GRANTEE_UID, integer32);
                Int64 result;
                ec = db->Insert(TABLE_GRANTS, GRANTS_ACCOUNTS_ID, values, &result);
                if (FAILED(ec)) break;
                ec = db->SetTransactionSuccessful();
            }
        } while(FALSE);
        // } finally {
        db->EndTransaction();
        if (FAILED(ec)) return ec;
        // }
        AutoPtr<IUserHandle> userH;
        CUserHandle::New(accounts->mUserId, (IUserHandle**)&userH);
        AutoPtr<IInteger32> integer32 = GetCredentialPermissionNotificationId(
                account, authTokenType, uid);
        Int32 value;
        integer32->GetValue(&value);
        CancelNotification(value, userH);
    }
    return NOERROR;
}

ECode CAccountManagerService::RevokeAppPermission(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Int32 uid)
{
    if (account == NULL || authTokenType == NULL) {
        Slogger::E(TAG, "revokeAppPermission: called with invalid arguments"/*, new Exception()*/);
        return NOERROR;
    }
    AutoPtr<UserAccounts> accounts = GetUserAccounts(UserHandle::GetUserId(uid));

    synchronized(accounts->mCacheLock) {
        AutoPtr<ISQLiteDatabase> db;
        FAIL_RETURN(accounts->mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db))
        db->BeginTransaction();
        // try {
        ECode ec;
        do {
            Int64 accountId;
            GetAccountIdLocked(db, account, &accountId);
            if (accountId >= 0) {
                AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(3);
                (*args)[0] = StringUtils::ToString(accountId);
                (*args)[1] = authTokenType;
                (*args)[2] = StringUtils::ToString(uid);
                Int32 result;
                ec = db->Delete(TABLE_GRANTS, GRANTS_ACCOUNTS_ID + String("=? AND ")
                        + GRANTS_AUTH_TOKEN_TYPE + String("=? AND ")
                        + GRANTS_GRANTEE_UID + String("=?"), args, &result);
                if (FAILED(ec)) break;
                ec = db->SetTransactionSuccessful();
            }
        } while(FALSE);
        // } finally {
        db->EndTransaction();
        if (FAILED(ec)) return ec;
        // }
        AutoPtr<IUserHandle> userH;
        CUserHandle::New(accounts->mUserId, (IUserHandle**)&userH);
        AutoPtr<IInteger32> integer32 = GetCredentialPermissionNotificationId(
                account, authTokenType, uid);
        Int32 value;
        integer32->GetValue(&value);
        CancelNotification(value, userH);
    }
    return NOERROR;
}

String CAccountManagerService::StringArrayToString(
    /* [in] */ ArrayOf<String>* value)
{
    Int32 size = value->GetLength();
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<ICharSequence> cs;
        CString::New((*value)[i], (ICharSequence**)&cs);
        array->Set(i, cs);
    }
    return value != NULL ? (String("[") + TextUtils::Join(StringUtils::ParseCharSequence(String(",")), array) + String("]"))
            : String(NULL);
}

void CAccountManagerService::RemoveAccountFromCacheLocked(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account)
{
    String type;
    account->GetType(&type);
    AutoPtr<IInterface> obj;
    accounts->mAccountCache->Get(StringUtils::ParseCharSequence(type), (IInterface**)&obj);
    AutoPtr<IArrayOf> oldAccountsForType = IArrayOf::Probe(obj);
    if (oldAccountsForType != NULL) {
        AutoPtr<IArrayList> newAccountsList;
        CArrayList::New((IArrayList**)&newAccountsList);
        Int32 size;
        oldAccountsForType->GetLength(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            oldAccountsForType->Get(i, (IInterface**)&obj);
            AutoPtr<IAccount> curAccount = IAccount::Probe(obj);
            Boolean isEquals;
            IObject::Probe(curAccount)->Equals(account, &isEquals);
            if (!isEquals) {
                newAccountsList->Add(curAccount);
            }
        }
        Boolean isEmpty;
        newAccountsList->IsEmpty(&isEmpty);
        if (isEmpty) {
            accounts->mAccountCache->Remove(StringUtils::ParseCharSequence(type));
        }
        else {
            Int32 size;
            newAccountsList->GetSize(&size);
            AutoPtr<ArrayOf<IInterface*> > array;
            newAccountsList->ToArray(array, (ArrayOf<IInterface*>**)&array);
            AutoPtr<IArrayOf> newAccountsForType;
            CArrayOf::New(EIID_IAccount, size, (IArrayOf**)&newAccountsForType);
            for (Int32 i = 0; i < size; ++i) {
                newAccountsForType->Set(i, (*array)[i]);
            }
            account->GetType(&type);
            accounts->mAccountCache->Put(StringUtils::ParseCharSequence(type), newAccountsForType);
        }
    }
    accounts->mUserDataCache->Remove(account);
    accounts->mAuthTokenCache->Remove(account);
    accounts->mPreviousNameCache->Remove(account);
}

void CAccountManagerService::InsertAccountIntoCacheLocked(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account)
{
    String type;
    account->GetType(&type);
    AutoPtr<IInterface> obj;
    accounts->mAccountCache->Get(StringUtils::ParseCharSequence(type), (IInterface**)&obj);
    AutoPtr<IArrayOf> accountsForType = IArrayOf::Probe(obj);
    Int32 oldLength = 0;
    AutoPtr<IArrayOf> newAccountsForType;
    CArrayOf::New(EIID_IAccount, oldLength + 1, (IArrayOf**)&newAccountsForType);
    if (accountsForType != NULL) {
        accountsForType->GetLength(&oldLength);
        for (Int32 i = 0; i < oldLength; ++i) {
            obj = NULL;
            accountsForType->Get(i, (IInterface**)&obj);
            newAccountsForType->Set(i, obj);
        }
    }
    newAccountsForType->Set(oldLength, account);
    accounts->mAccountCache->Put(StringUtils::ParseCharSequence(type), newAccountsForType);
}

ECode CAccountManagerService::FilterSharedAccounts(
    /* [in] */ UserAccounts* userAccounts,
    /* [in] */ ArrayOf<IAccount*>* unfiltered,
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& callingPackage,
    /* [out, callee] */ ArrayOf<IAccount*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (GetUserManager() == NULL || userAccounts == NULL || userAccounts->mUserId < 0
            || callingUid == Process::MyUid()) {
        *result = unfiltered;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    AutoPtr<IUserInfo> user;
    mUserManager->GetUserInfo(userAccounts->mUserId, (IUserInfo**)&user);
    if (user != NULL) {
        Boolean isRestricted;
        user->IsRestricted(&isRestricted);
        if (isRestricted) {
            AutoPtr<ArrayOf<String> > packages;
            mPackageManager->GetPackagesForUid(callingUid, (ArrayOf<String>**)&packages);
            // If any of the packages is a white listed package, return the full set,
            // otherwise return non-shared accounts only.
            // This might be a temporary way to specify a whitelist
            AutoPtr<IResources> res;
            mContext->GetResources((IResources**)&res);
            String whiteList;
            res->GetString(
                    R::string::config_appsAuthorizedForSharedAccounts, &whiteList);
            for (Int32 i = 0; i < packages->GetLength(); ++i) {
                String packageName = (*packages)[i];
                if (whiteList.Contains(String(";") + packageName + String(";"))) {
                    *result = unfiltered;
                    REFCOUNT_ADD(*result)
                    return NOERROR;
                }
            }
            AutoPtr<IArrayList> allowed;
            CArrayList::New((IArrayList**)&allowed);
            AutoPtr<ArrayOf<IAccount*> > sharedAccounts;
            GetSharedAccountsAsUser(userAccounts->mUserId, (ArrayOf<IAccount*>**)&sharedAccounts);
            if (sharedAccounts == NULL || sharedAccounts->GetLength() == 0) {
                *result = unfiltered;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
            String requiredAccountType("");
            // try {
            ECode ec;
            do {
                // If there's an explicit callingPackage specified, check if that package
                // opted in to see restricted accounts.
                if (callingPackage != NULL) {
                    AutoPtr<IPackageInfo> pi;
                    ec = mPackageManager->GetPackageInfo(callingPackage, 0, (IPackageInfo**)&pi);
                    if (FAILED(ec)) break;
                    if (pi != NULL) {
                        String type;
                        ec = pi->GetRestrictedAccountType(&type);
                        if (FAILED(ec)) break;
                        if(!type.IsNull()) {
                            requiredAccountType = type;
                        }
                    }
                } else {
                    // Otherwise check if the callingUid has a package that has opted in
                    for (Int32 i = 0; i < packages->GetLength(); ++i) {
                        String packageName = (*packages)[i];
                        AutoPtr<IPackageInfo> pi;
                        ec = mPackageManager->GetPackageInfo(packageName, 0, (IPackageInfo**)&pi);
                        if (FAILED(ec)) break;
                        if (pi != NULL) {
                            String type;
                            ec = pi->GetRestrictedAccountType(&type);
                            if (FAILED(ec)) break;
                            if(!type.IsNull()) {
                                requiredAccountType = type;
                                break;
                            }
                        }
                    }
                }
            } while(FALSE);
            // } catch (NameNotFoundException nnfe) {
            if (FAILED(ec)) {
                if ((ECode) E_NAME_NOT_FOUND_EXCEPTION != ec) return ec;
            }
            // }
            for (Int32 i = 0; i < unfiltered->GetLength(); ++i) {
                AutoPtr<IAccount> account = (*unfiltered)[i];
                String type;
                account->GetType(&type);
                if (type.Equals(requiredAccountType)) {
                    allowed->Add(account);
                } else {
                    Boolean found = FALSE;
                    for (Int32 j = 0; j < sharedAccounts->GetLength(); ++j) {
                        AutoPtr<IAccount> shared = (*sharedAccounts)[j];
                        Boolean isEquals;
                        shared->Equals(account, &isEquals);
                        if (isEquals) {
                            found = TRUE;
                            break;
                        }
                    }
                    if (!found) {
                        allowed->Add(account);
                    }
                }
            }
            Int32 size;
            allowed->GetSize(&size);
            AutoPtr<ArrayOf<IAccount*> > filtered = ArrayOf<IAccount*>::Alloc(size);
            AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(size);
            allowed->ToArray((ArrayOf<IInterface*>**)&array);
            for (Int32 i = 0; i < size; ++i) {
                filtered->Set(i, IAccount::Probe((*array)[i]));
            }
            *result = filtered;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    } else {
        *result = unfiltered;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return NOERROR;
}

ECode CAccountManagerService::GetAccountsFromCacheLocked(
    /* [in] */ UserAccounts* userAccounts,
    /* [in] */ const String& accountType,
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& callingPackage,
    /* [out] */ ArrayOf<IAccount*>** result)
{
    VALIDATE_NOT_NULL(result)

    if (!accountType.IsNull()) {
        AutoPtr<IInterface> obj;
        userAccounts->mAccountCache->Get(StringUtils::ParseCharSequence(accountType), (IInterface**)&obj);
        AutoPtr<IArrayOf> accounts = IArrayOf::Probe(obj);
        if (accounts == NULL) {
            *result = EMPTY_ACCOUNT_ARRAY;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        else {
            Int32 length;
            accounts->GetLength(&length);
            AutoPtr<ArrayOf<IAccount*> > tmpArray = ArrayOf<IAccount*>::Alloc(length);
            for (Int32 i = 0; i < length; ++i) {
                AutoPtr<IInterface> obj;
                accounts->Get(i, (IInterface**)&obj);
                tmpArray->Set(i, IAccount::Probe(obj));
            }
            FilterSharedAccounts(userAccounts, tmpArray,
                    callingUid, callingPackage, result);
            return NOERROR;
        }
    }
    else {
        Int32 totalLength = 0;
        AutoPtr<ICollection> values;
        userAccounts->mAccountCache->GetValues((ICollection**)&values);
        AutoPtr<IIterator> it;
        values->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            Int32 length;
            IArrayOf::Probe(obj)->GetLength(&length);
            totalLength += length;
        }
        if (totalLength == 0) {
            *result = EMPTY_ACCOUNT_ARRAY;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        AutoPtr<ArrayOf<IAccount*> > accounts = ArrayOf<IAccount*>::Alloc(totalLength);
        totalLength = 0;
        it = NULL;
        values->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IArrayOf> accountsOfType = IArrayOf::Probe(obj);
            Int32 length;
            accountsOfType->GetLength(&length);
            for (Int32 i = 0; i < length; ++i) {
                obj = NULL;
                accountsOfType->Get(i, (IInterface**)&obj);
                accounts->Set(i, IAccount::Probe(obj));
            }
            totalLength += length;
        }
        FilterSharedAccounts(userAccounts, accounts, callingUid, callingPackage, result);
        return NOERROR;
    }
    return NOERROR;
}

void CAccountManagerService::WriteUserDataIntoCacheLocked(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IAccount* account,
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr<IInterface> obj;
    accounts->mUserDataCache->Get(account, (IInterface**)&obj);
    AutoPtr<IHashMap> userDataForAccount = IHashMap::Probe(obj);
    if (userDataForAccount == NULL) {
        ReadUserDataForAccountFromDatabaseLocked(db, account, (IHashMap**)&userDataForAccount);
        accounts->mUserDataCache->Put(account, userDataForAccount);
    }
    if (value.IsNull()) {
        userDataForAccount->Remove(StringUtils::ParseCharSequence(key));
    }
    else {
        userDataForAccount->Put(StringUtils::ParseCharSequence(key), StringUtils::ParseCharSequence(value));
    }
}

void CAccountManagerService::WriteAuthTokenIntoCacheLocked(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IAccount* account,
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr<IInterface> obj;
    accounts->mAuthTokenCache->Get(account, (IInterface**)&obj);
    AutoPtr<IHashMap> authTokensForAccount = IHashMap::Probe(obj);
    if (authTokensForAccount == NULL) {
        ReadAuthTokensForAccountFromDatabaseLocked(db, account, (IHashMap**)&authTokensForAccount);
        accounts->mAuthTokenCache->Put(account, authTokensForAccount);
    }
    if (value.IsNull()) {
        authTokensForAccount->Remove(StringUtils::ParseCharSequence(key));
    }
    else {
        authTokensForAccount->Put(StringUtils::ParseCharSequence(key), StringUtils::ParseCharSequence(value));
    }
}

String CAccountManagerService::ReadAuthTokenInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType)
{
    synchronized(accounts->mCacheLock) {
        AutoPtr<IInterface> obj;
        accounts->mAuthTokenCache->Get(account, (IInterface**)&obj);
        AutoPtr<IHashMap> authTokensForAccount = IHashMap::Probe(obj);
        if (authTokensForAccount == NULL) {
            // need to populate the cache for this account
            AutoPtr<ISQLiteDatabase> db;
            accounts->mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
            ReadAuthTokensForAccountFromDatabaseLocked(db, account, (IHashMap**)&authTokensForAccount);
            accounts->mAuthTokenCache->Put(account, authTokensForAccount);
        }
        obj = NULL;
        authTokensForAccount->Get(StringUtils::ParseCharSequence(authTokenType), (IInterface**)&obj);
        return TO_STR(obj);
    }
    return String(NULL);
}

ECode CAccountManagerService::ReadUserDataInternal(
    /* [in] */ UserAccounts* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ const String& key,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(accounts->mCacheLock) {
        AutoPtr<IInterface> obj;
        accounts->mUserDataCache->Get(account, (IInterface**)&obj);
        AutoPtr<IHashMap> userDataForAccount = IHashMap::Probe(obj);
        if (userDataForAccount == NULL) {
            // need to populate the cache for this account
            AutoPtr<ISQLiteDatabase> db;
            accounts->mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
            ReadUserDataForAccountFromDatabaseLocked(db, account, (IHashMap**)&userDataForAccount);
            accounts->mUserDataCache->Put(account, userDataForAccount);
        }
        obj = NULL;
        userDataForAccount->Get(StringUtils::ParseCharSequence(key), (IInterface**)&obj);
        IObject::Probe(obj)->ToString(result);
    }
    return NOERROR;
}

ECode CAccountManagerService::ReadUserDataForAccountFromDatabaseLocked(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IAccount* account,
    /* [out] */ IHashMap** result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IHashMap> userDataForAccount;
    CHashMap::New((IHashMap**)&userDataForAccount);
    AutoPtr<ICursor> cursor;
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    account->GetName(&(*args)[0]);
    account->GetType(&(*args)[1]);
    AutoPtr<ArrayOf<String> > params = ArrayOf<String>::Alloc(2);
    params->Set(0, COLUMNS_EXTRAS_KEY_AND_VALUE[0]);
    params->Set(1, COLUMNS_EXTRAS_KEY_AND_VALUE[1]);
    String nullStr;
    FAIL_RETURN(db->Query(TABLE_EXTRAS,
            params,
            SELECTION_USERDATA_BY_ACCOUNT, args,
            nullStr, nullStr, nullStr, (ICursor**)&cursor))
    // try {
    ECode ec;
    do {
        Boolean isMoveToNextOk;
        while (cursor->MoveToNext(&isMoveToNextOk), isMoveToNextOk) {
            String tmpkey, value;
            ec = cursor->GetString(0, &tmpkey);
            if (FAILED(ec)) break;
            ec = cursor->GetString(1, &value);
            if (FAILED(ec)) break;
            userDataForAccount->Put(StringUtils::ParseCharSequence(tmpkey), StringUtils::ParseCharSequence(value));
        }
    } while(FALSE);
    // } finally {
    ICloseable::Probe(cursor)->Close();
    if (FAILED(ec)) return ec;
    // }
    *result = userDataForAccount;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CAccountManagerService::ReadAuthTokensForAccountFromDatabaseLocked(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IAccount* account,
    /* [out] */ IHashMap** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IHashMap> authTokensForAccount;
    CHashMap::New((IHashMap**)&authTokensForAccount);
    AutoPtr<ICursor> cursor;
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    account->GetName(&(*args)[0]);
    account->GetType(&(*args)[1]);

    AutoPtr<ArrayOf<String> > params = ArrayOf<String>::Alloc(2);
    params->Set(0, COLUMNS_AUTHTOKENS_TYPE_AND_AUTHTOKEN[0]);
    params->Set(1, COLUMNS_AUTHTOKENS_TYPE_AND_AUTHTOKEN[1]);
    String nullStr;
    FAIL_RETURN(db->Query(TABLE_AUTHTOKENS,
            params, SELECTION_AUTHTOKENS_BY_ACCOUNT, args,
            nullStr, nullStr, nullStr, (ICursor**)&cursor))
    // try {
    ECode ec;
    do {
        Boolean isMoveToNextOk;
        while (cursor->MoveToNext(&isMoveToNextOk), isMoveToNextOk) {
            String type, authToken;
            ec = cursor->GetString(0, &type);
            if (FAILED(ec)) break;
            ec = cursor->GetString(1, &authToken);
            if (FAILED(ec)) break;
            authTokensForAccount->Put(StringUtils::ParseCharSequence(type), StringUtils::ParseCharSequence(authToken));
        }
    } while(FALSE);
    // } finally {
    ICloseable::Probe(cursor)->Close();
    if (FAILED(ec)) return ec;
    // }
    *result = authTokensForAccount;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CAccountManagerService::GetContextForUser(
    /* [in] */ IUserHandle* user,
    /* [out] */ IContext** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // try {
    ECode ec;
    do {
        String packageName;
        ec = mContext->GetPackageName(&packageName);
        if (FAILED(ec)) break;
        AutoPtr<IContext> iNoUse;
        ec = mContext->CreatePackageContextAsUser(packageName, 0, user, (IContext**)&iNoUse);
    } while(FALSE);
    // } catch (NameNotFoundException e) {
    if (FAILED(ec)) {
        if ((ECode) E_NAME_NOT_FOUND_EXCEPTION == ec) {
            // Default to mContext, not finding the package system is running as is unlikely.
            *result = mContext;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    return ec;
    // }
}

ECode CAccountManagerService::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(TO_IINTERFACE(this))->ToString(result);
}

AutoPtr<IIntent> CAccountManagerService::InitAccountsChangedIntent()
{
    AutoPtr<IIntent> i;
    CIntent::New(IAccountManager::LOGIN_ACCOUNTS_CHANGED_ACTION,
            (IIntent**)&i);
    i->SetFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);

    return i;
}

AutoPtr<IAtomicReference> CAccountManagerService::InitAtomicRef()
{
    AutoPtr<IAtomicReference> rev;
    CAtomicReference::New((IAtomicReference**)&rev);
    return rev;
}

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos
