
#include "elastos/droid/server/CLockSettingsService.h"
#include <elastos/droid/Manifest.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/etl/Algorithm.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Database.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::ISQLiteProgram;
using Elastos::Droid::Database::Sqlite::ISQLiteStatement;
using Elastos::Droid::Database::Sqlite::EIID_ISQLiteOpenHelper;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::EIID_IILockSettings;
// using Elastos::Droid::Internal::Widget::ILockPatternUtilsHelper;
// using Elastos::Droid::Internal::Widget::CLockPatternUtilsHelper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Text::TextUtils;
// using Elastos::Droid::Security::IKeyStore;
// using Elastos::Droid::Security::IKeyStoreHelper;
// using Elastos::Droid::Security::IKeyStoreHelper;

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Utility::Arrays;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataOutput;
using Elastos::IO::IRandomAccessFile;
using Elastos::IO::CRandomAccessFile;

namespace Elastos {
namespace Droid {
namespace Server {

static AutoPtr<ArrayOf<String> > InitCOLUMNS_FOR_QUERY()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    array->Set(0, String("value")/*COLUMN_VALUE*/);
    return NOERROR;
}

static AutoPtr<ArrayOf<String> > InitVALID_SETTINGS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(19);
    array->Set(0, ILockPatternUtils::LOCKOUT_PERMANENT_KEY);
    array->Set(1, ILockPatternUtils::LOCKOUT_ATTEMPT_DEADLINE);
    array->Set(2, ILockPatternUtils::PATTERN_EVER_CHOSEN_KEY);
    array->Set(3, ILockPatternUtils::PASSWORD_TYPE_KEY);

    array->Set(4, ILockPatternUtils::PASSWORD_TYPE_ALTERNATE_KEY);
    array->Set(5, ILockPatternUtils::LOCK_PASSWORD_SALT_KEY);
    array->Set(6, ILockPatternUtils::DISABLE_LOCKSCREEN_KEY);
    array->Set(7, ILockPatternUtils::LOCKSCREEN_OPTIONS);

    array->Set(8, ILockPatternUtils::LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK);
    array->Set(9, ILockPatternUtils::BIOMETRIC_WEAK_EVER_CHOSEN_KEY);
    array->Set(10, ILockPatternUtils::LOCKSCREEN_POWER_BUTTON_INSTANTLY_LOCKS);
    array->Set(11, ILockPatternUtils::PASSWORD_HISTORY_KEY);

    array->Set(12, ISettingsSecure::LOCK_PATTERN_ENABLED);
    array->Set(13, ISettingsSecure::LOCK_BIOMETRIC_WEAK_FLAGS);
    array->Set(14, ISettingsSecure::LOCK_PATTERN_VISIBLE);
    array->Set(15, ISettingsSecure::LOCK_PATTERN_TACTILE_FEEDBACK_ENABLED);
    array->Set(16, ISettingsSecure::LOCK_PATTERN_SIZE);
    array->Set(17, ISettingsSecure::LOCK_DOTS_VISIBLE);
    array->Set(18, ISettingsSecure::LOCK_SHOW_ERROR_PATH);
    return array;
}

// These are protected with a read permission

AutoPtr<ArrayOf<String> > InitREAD_PROFILE_PROTECTED_SETTINGS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(16);
    array->Set(0, ISettingsSecure::LOCK_SCREEN_OWNER_INFO_ENABLED);
    array->Set(1, ISettingsSecure::LOCK_SCREEN_OWNER_INFO);
    return array;
}

String CLockSettingsService::PERMISSION("android.permission.ACCESS_KEYGUARD_SECURE_STORAGE");
String CLockSettingsService::SYSTEM_DEBUGGABLE("ro.debuggable");
String CLockSettingsService::TAG("LockSettingsService");
String CLockSettingsService::TABLE("locksettings");
String CLockSettingsService::COLUMN_KEY("name");
String CLockSettingsService::COLUMN_USERID("user");
String CLockSettingsService::COLUMN_VALUE("value");

AutoPtr<ArrayOf<String> > CLockSettingsService::COLUMNS_FOR_QUERY = InitCOLUMNS_FOR_QUERY();

String CLockSettingsService::SYSTEM_DIRECTORY("/system/");
String CLockSettingsService::LOCK_PATTERN_FILE("gesture.key");
String CLockSettingsService::LOCK_PASSWORD_FILE("password.key");
AutoPtr<ArrayOf<String> > CLockSettingsService::VALID_SETTINGS = InitVALID_SETTINGS();
AutoPtr<ArrayOf<String> > CLockSettingsService::READ_PROFILE_PROTECTED_SETTINGS = InitREAD_PROFILE_PROTECTED_SETTINGS();


//===========================================================================
// CLockSettingsService::DatabaseHelper
//===========================================================================

String CLockSettingsService::DatabaseHelper::TAG("LockSettingsDB");
String CLockSettingsService::DatabaseHelper::DATABASE_NAME("locksettings.db");
const Int32 CLockSettingsService::DatabaseHelper::DATABASE_VERSION = 2;

ECode CLockSettingsService::DatabaseHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ CLockSettingsService* host)
{
    mHost = host;
    SQLiteOpenHelper::constructor(context, DATABASE_NAME, NULL, DATABASE_VERSION);
    SetWriteAheadLoggingEnabled(TRUE);
    return NOERROR;
}

void CLockSettingsService::DatabaseHelper::CreateTable(
    /* [in] */ ISQLiteDatabase* db)
{
    StringBuilder sb("CREATE TABLE ");
    sb += CLockSettingsService::TABLE + " (" +
        "_id INTEGER PRIMARY KEY AUTOINCREMENT," +
        CLockSettingsService::COLUMN_KEY + " TEXT," +
        CLockSettingsService::COLUMN_USERID + " INTEGER," +
        CLockSettingsService::COLUMN_VALUE + " TEXT" +
        ");";

    db->ExecSQL(sb.ToString());
}

ECode CLockSettingsService::DatabaseHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    CreateTable(db);
    InitializeDefaults(db);
    return NOERROR;
}

void CLockSettingsService::DatabaseHelper::InitializeDefaults(
    /* [in] */ ISQLiteDatabase* db)
{
    // Get the lockscreen default from a system property, if available
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    Boolean lockScreenDisable;
    sysProp->GetBoolean(String("ro.lockscreen.disable.default"), FALSE, &lockScreenDisable);
    if (lockScreenDisable) {
        mHost->WriteToDb(db, String("lockscreen.disabled")/*LockPatternUtils::DISABLE_LOCKSCREEN_KEY*/, String("1"), 0);
    }
}

ECode CLockSettingsService::DatabaseHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 currentVersion)
{
    Int32 upgradeVersion = oldVersion;
    if (upgradeVersion == 1) {
        // Set the initial value for {@link LockPatternUtils#LOCKSCREEN_WIDGETS_ENABLED}
        // during upgrade based on whether each user previously had widgets in keyguard.
        MaybeEnableWidgetSettingForUsers(db);
        upgradeVersion = 2;
    }

    if (upgradeVersion != DATABASE_VERSION) {
        Slogger::W(TAG, "Failed to upgrade database!");
    }
    return NOERROR;
}

ECode CLockSettingsService::DatabaseHelper::MaybeEnableWidgetSettingForUsers(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<IInterface> service;
    mHost->mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&service);
    AutoPtr<IUserManager> um = IUserManager::Probe(service);
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IList> users;
    um->GetUsers((IList**)&users);
    Int32 size;
    users->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        users->Get(i, (IInterface**)&obj);
        IUserInfo* ui = IUserInfo::Probe(obj);
        Int32 userId;
        ui->GetId(&userId);
        Boolean enabled;
        mHost->mLockPatternUtils->HasWidgetsEnabledInKeyguard(userId, &enabled);
        // Slogger::V(TAG, "Widget upgrade uid=" + userId + ", enabled="
        //         + enabled + ", w[]=" + mLockPatternUtils->GetAppWidgets());
        LoadSetting(db, ILockPatternUtils::LOCKSCREEN_WIDGETS_ENABLED, userId, enabled);
    }
    return NOERROR;
}

ECode CLockSettingsService::DatabaseHelper::LoadSetting(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& key,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean value)
{
    AutoPtr<ISQLiteStatement> stmt;
    // try {
    ECode ec = db->CompileStatement(
        String("INSERT OR REPLACE INTO Locksettings(name,user,value) VALUES(?,?,?);"),
        (ISQLiteStatement**)&stmt);
    ISQLiteProgram* sp = ISQLiteProgram::Probe(stmt);
    FAIL_GOTO(ec, _EXIT_)

    ec = sp->BindString(1, key);
    FAIL_GOTO(ec, _EXIT_)

    ec = sp->BindInt64(2, userId);
    FAIL_GOTO(ec, _EXIT_)

    ec = sp->BindInt64(3, value ? 1 : 0);
    FAIL_GOTO(ec, _EXIT_)

    ec = stmt->Execute();
    FAIL_GOTO(ec, _EXIT_)

    // } finally {
_EXIT_:
    if (stmt != NULL) ICloseable::Probe(stmt)->Close();
    // }
    return ec;
}

ECode CLockSettingsService::DatabaseHelper::GetDatabaseName(
    /* [out] */ String* name)
{
    return SQLiteOpenHelper::GetDatabaseName(name);
}

ECode CLockSettingsService::DatabaseHelper::SetWriteAheadLoggingEnabled(
    /* [in] */ Boolean enabled)
{
    return SQLiteOpenHelper::SetWriteAheadLoggingEnabled(enabled);
}

ECode CLockSettingsService::DatabaseHelper::GetWritableDatabase(
    /* [out] */ ISQLiteDatabase** database)
{
    return SQLiteOpenHelper::GetWritableDatabase(database);
}

ECode CLockSettingsService::DatabaseHelper::GetReadableDatabase(
    /* [out] */ ISQLiteDatabase** database)
{
    return SQLiteOpenHelper::GetReadableDatabase(database);
}

ECode CLockSettingsService::DatabaseHelper::Close()
{
    return SQLiteOpenHelper::Close();
}

ECode CLockSettingsService::DatabaseHelper::OnConfigure(
    /* [in] */ ISQLiteDatabase* db)
{
    return SQLiteOpenHelper::OnConfigure(db);
}

ECode CLockSettingsService::DatabaseHelper::OnDowngrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    return SQLiteOpenHelper::OnDowngrade(db, oldVersion, newVersion);
}

ECode CLockSettingsService::DatabaseHelper::OnOpen(
    /* [in] */ ISQLiteDatabase* db)
{
    return SQLiteOpenHelper::OnOpen(db);
}

//===========================================================================
// CLockSettingsService::UserAddedBroadcastReceiver
//===========================================================================
CLockSettingsService::UserAddedBroadcastReceiver::UserAddedBroadcastReceiver(
    /* [in] */ CLockSettingsService* host)
    : mHost(host)
{}

ECode CLockSettingsService::UserAddedBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_USER_ADDED)) {
        Int32 userHandle;
        Int32 userSysUid = UserHandle::GetUid(userHandle, IProcess::SYSTEM_UID);
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &userHandle);

        assert(0 && "TODO");
        // AutoPtr<IKeyStoreHelper> helper;
        // CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&helper);
        // AutoPtr<IKeyStore> ks;
        // helper->GetInstance((IKeyStore**)&ks);

        // // Clear up keystore in case anything was left behind by previous users
        // ks->ResetUid(userSysUid);

        // If this user has a parent, sync with its keystore password
        AutoPtr<IInterface> service;
        mHost->mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&service);
        AutoPtr<IUserManager> um = IUserManager::Probe(service);
        AutoPtr<IUserInfo> parentInfo;
        um->GetProfileParent(userHandle, (IUserInfo**)&parentInfo);
        if (parentInfo != NULL) {
            Int32 id;
            parentInfo->GetId(&id);
            Int32 parentSysUid = UserHandle::GetUid(id, IProcess::SYSTEM_UID);
            // ks->SyncUid(parentSysUid, userSysUid);
        }
    }
    return NOERROR;
}

//===========================================================================
// CLockSettingsService::LockSettingsObserver
//===========================================================================
CAR_INTERFACE_IMPL(CLockSettingsService::LockSettingsObserver, Object, IProxyDeathRecipient)
CLockSettingsService::LockSettingsObserver::LockSettingsObserver(
    /* [in] */ CLockSettingsService* lss)
    : mHost(lss)
{}

ECode CLockSettingsService::LockSettingsObserver::ProxyDied()
{
    AutoPtr<LockSettingsObserver> lss = this;
    List<AutoPtr<LockSettingsObserver> >::Iterator it;
    it = Find(mHost->mObservers.Begin(), mHost->mObservers.End(), lss);
    if (it != mHost->mObservers.End()) {
        mHost->mObservers.Erase(it);
    }

    return NOERROR;
}

//===========================================================================
// CLockSettingsService
//===========================================================================

CAR_INTERFACE_IMPL_2(CLockSettingsService, Object, IILockSettings, IBinder)

CAR_OBJECT_IMPL(CLockSettingsService)

CLockSettingsService::CLockSettingsService()
    : mFirstCallToVold(TRUE)
{}

ECode CLockSettingsService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    // Open the database
    mOpenHelper = new DatabaseHelper();
    mOpenHelper->constructor(mContext, this);
    CLockPatternUtils::New(context, (ILockPatternUtils**)&mLockPatternUtils);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_USER_ADDED);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiverAsUser(mBroadcastReceiver, UserHandle::ALL, filter,
        String(NULL), NULL, (IIntent**)&intent);

    return NOERROR;
}

ECode CLockSettingsService::SystemReady()
{
    MigrateOldData();

    return NOERROR;
}

void CLockSettingsService::MigrateOldData()
{
    // These Settings moved before multi-user was enabled, so we only have to do it for the
    // root user.
    ECode ec = NOERROR;
    String data;
    GetString(String("migrated"), String(NULL), 0, &data);
    if (data == NULL) {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        Int32 length = VALID_SETTINGS->GetLength();
        for (Int32 i = 0; i < length; ++i) {
            String validSetting = (*VALID_SETTINGS)[i];
            String value;
            Settings::Secure::GetString(cr, validSetting, &value);
            if (value != NULL) {
                SetString(validSetting, value, 0);
            }
        }
        // No need to move the password / pattern files. They're already in the right place.
        SetString(String("migrated"), String("TRUE"), 0);
        Slogger::I(TAG, "Migrated lock settings to new location");
    }

    // These Settings changed after multi-user was enabled, hence need to be moved per user.
    GetString(String("migrated_user_specific"), String(NULL), 0, &data);
    if (data == NULL) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&service);
        AutoPtr<IUserManager> um = IUserManager::Probe(service);
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<IList> users;
        um->GetUsers((IList**)&users);
        Int32 size;
        users->GetSize(&size);
        Boolean bval;
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            users->Get(i, (IInterface**)&obj);
            IUserInfo* ui = IUserInfo::Probe(obj);
            // Migrate owner info
            Int32 userId;
            ui->GetId(&userId);
            String OWNER_INFO = ISettingsSecure::LOCK_SCREEN_OWNER_INFO;
            String ownerInfo;
            Settings::Secure::GetStringForUser(cr, OWNER_INFO, userId, &ownerInfo);
            if (ownerInfo != NULL) {
                SetString(OWNER_INFO, ownerInfo, userId);
                Settings::Secure::PutStringForUser(cr, ownerInfo, String(""), userId, &bval);
            }

            // Migrate owner info enabled.  Note there was a bug where older platforms only
            // stored this value if the checkbox was toggled at least once. The code detects
            // this case by handling the exception.
            String OWNER_INFO_ENABLED = ISettingsSecure::LOCK_SCREEN_OWNER_INFO_ENABLED;
            Boolean enabled;

            Int32 ivalue;
            Int64 lvalue;
            ec = Settings::Secure::GetInt32ForUser(cr, OWNER_INFO_ENABLED, userId, &ivalue);
            if (ec == (ECode)E_SETTING_NOT_FOUND_EXCEPTION) {
                // Setting was never stored. Store it if the string is not empty.
                if (!TextUtils::IsEmpty(ownerInfo)) {
                    GetInt64(OWNER_INFO_ENABLED, 1, userId, &lvalue);
                }
            }
            else {
                enabled = ivalue != 0;
                GetInt64(OWNER_INFO_ENABLED, enabled ? 1 : 0, userId, &lvalue);
            }

            Settings::Secure::PutInt32ForUser(cr, OWNER_INFO_ENABLED, 0, userId, &bval);
        }

        // No need to move the password / pattern files. They're already in the right place.
        SetString(String("migrated_user_specific"), String("TRUE"), 0);
        Slogger::I("CLockSettingsService", "Migrated per-user lock settings to new location");
    }

// _EXIT_:
//     if (ec == (ECode)E_REMOTE_EXCEPTION) {
//         Slogger::E("CLockSettingsService", "Unable to migrate old data");
//     }
}

ECode CLockSettingsService::CheckWritePermission(
    /* [in] */ Int32 userId)
{
    return mContext->EnforceCallingOrSelfPermission(PERMISSION, String("LockSettingsWrite"));
}

ECode CLockSettingsService::CheckPasswordReadPermission(
    /* [in] */ Int32 userId)
{
    return mContext->EnforceCallingOrSelfPermission(PERMISSION, String("LockSettingsRead"));
}

ECode CLockSettingsService::CheckReadPermission(
    /* [in] */ const String& requestedKey,
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    Int32 perm;
    for (Int32 i = 0; i < READ_PROFILE_PROTECTED_SETTINGS->GetLength(); i++) {
        String key = (*READ_PROFILE_PROTECTED_SETTINGS)[i];
        mContext->CheckCallingOrSelfPermission(Manifest::permission::READ_PROFILE, &perm);
        if (key.Equals(requestedKey) && perm != IPackageManager::PERMISSION_GRANTED) {
            Slogger::E(TAG, "uid=%d needs permission %s to read %s for user %d.",
                callingUid, Manifest::permission::READ_PROFILE.string(), requestedKey.string(), userId);
            return E_SECURITY_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CLockSettingsService::SetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean value,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(CheckWritePermission(userId))

    return WriteToDb(key, value ? String("1") : String("0"), userId);
}

ECode CLockSettingsService::SetInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 value,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(CheckWritePermission(userId))

    return WriteToDb(key, StringUtils::ToString(value), userId);
}

ECode CLockSettingsService::SetString(
    /* [in] */ const String& key,
    /* [in] */ const String& value,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(CheckWritePermission(userId))

    return WriteToDb(key, value, userId);
}

ECode CLockSettingsService::GetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean defaultValue,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    FAIL_RETURN(CheckReadPermission(key, userId))
    String value;
    FAIL_RETURN(ReadFromDb(key, String(NULL), userId, &value))
    *res = TextUtils::IsEmpty(value) ?
            defaultValue : (value.Equals("1") || value.Equals("TRUE"));
    return NOERROR;
}

ECode CLockSettingsService::GetInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 defaultValue,
    /* [in] */ Int32 userId,
    /* [out] */ Int64* res)
{
    VALIDATE_NOT_NULL(res);
    *res = 0;

    CheckReadPermission(key, userId);

    String value;
    FAIL_RETURN(ReadFromDb(key, String(NULL), userId, &value))
    *res = TextUtils::IsEmpty(value) ? defaultValue : StringUtils::ParseInt64(value);
    return NOERROR;
}

ECode CLockSettingsService::GetString(
    /* [in] */ const String& key,
    /* [in] */ const String& defaultValue,
    /* [in] */ Int32 userId,
    /* [out] */ String* retValue)
{
    VALIDATE_NOT_NULL(retValue);
    *retValue = "";

    FAIL_RETURN(CheckReadPermission(key, userId))
    return ReadFromDb(key, defaultValue, userId, retValue);
}

ECode CLockSettingsService::GetLockPatternSize(
    /* [in] */ Int32 userId,
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result);

    // try {
    Int64 size = 0;
    GetInt64(ISettingsSecure::LOCK_PATTERN_SIZE, -1, userId, &size);
    if (size > 0 && size < 128) {
        *result = (Byte) size;
        return NOERROR;
    }
    // } catch (RemoteException re) {
    //     //Any invalid size handled below
    // }
    *result = ILockPatternUtils::PATTERN_SIZE_DEFAULT;
    return NOERROR;
}

Boolean CLockSettingsService::IsDefaultSize(
    /* [in] */ Int32 userId)
{
    Byte res;
    GetLockPatternSize(userId, &res);
    return res == ILockPatternUtils::PATTERN_SIZE_DEFAULT;
}

ECode CLockSettingsService::RegisterObserver(
    /* [in] */ IILockSettingsObserver* remote)
{
    AutoPtr<ISystemProperties> sysprop;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysprop);
    synchronized(mObserversLock) {
        List<AutoPtr<LockSettingsObserver> >::Iterator it = mObservers.Begin();
        for (; it != mObservers.End(); ++it) {
            LockSettingsObserver* lso = *it;
            if (lso->mRemote.Get() == remote) {
                String value;
                sysprop->Get(SYSTEM_DEBUGGABLE, String("0"), &value);
                Boolean isDebuggable = value.Equals("1");
                if (isDebuggable) {
                    Slogger::E(TAG, "Observer was already registered.");
                    return E_ILLEGAL_STATE_EXCEPTION;
                }
                else {
                    Slogger::E(TAG, "Observer was already registered.");
                    return NOERROR;
                }
            }
        }

        AutoPtr<LockSettingsObserver> o = new LockSettingsObserver(this);
        o->mRemote = remote;
        AutoPtr<IBinder> binder = IBinder::Probe(o->mRemote);
        IProxy* proxy = ((IProxy*)o->mRemote->Probe(EIID_IProxy));
        if (proxy != NULL) proxy->LinkToDeath((IProxyDeathRecipient*)o.Get(), 0);
        mObservers.PushBack(o);
    }
    return NOERROR;
}

ECode CLockSettingsService::UnregisterObserver(
    /* [in] */ IILockSettingsObserver* remote)
{
    synchronized(mObserversLock) {
        List<AutoPtr<LockSettingsObserver> >::Iterator it = mObservers.Begin();
        for (; it != mObservers.End(); ++it) {
            LockSettingsObserver* lso = *it;
            if (lso->mRemote.Get() == remote) {
                mObservers.Erase(it);
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

ECode CLockSettingsService::NotifyObservers(
    /* [in] */ const String& key,
    /* [in] */ Int32 userId)
{
    synchronized(mObserversLock) {
        ECode ec = NOERROR;
        List<AutoPtr<LockSettingsObserver> >::Iterator it = mObservers.Begin();
        for (; it != mObservers.End(); ++it) {
            LockSettingsObserver* lso = *it;
            ec = lso->mRemote->OnLockSettingChanged(key, userId);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                // The stack trace is not really helpful here.
                Slogger::E(TAG, "Failed to notify ILockSettingsObserver: ");
            }
        }
    }
    return NOERROR;
}

Int32 CLockSettingsService::GetUserParentOrSelfId(
    /* [in] */ Int32 userId)
{
    if (userId != 0) {
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
        AutoPtr<IUserManager> um = IUserManager::Probe(obj);
        AutoPtr<IUserInfo> pi;
        um->GetProfileParent(userId, (IUserInfo**)&pi);
        if (pi != NULL) {
            Int32 id;
            pi->GetId(&id);
            return id;
        }
    }
    return userId;
}


String CLockSettingsService::GetLockPatternFilename(
    /* [in] */ Int32 uid)
{
    return GetLockPatternFilename(uid, IsDefaultSize(uid));
}

String CLockSettingsService::GetLockPatternFilename(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean defaultSize)
{
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dataDir;
    env->GetDataDirectory((IFile**)&dataDir);
    assert(dataDir != NULL);
    String absPath;
    dataDir->GetAbsolutePath(&absPath);
    String dataSystemDirectory = absPath + SYSTEM_DIRECTORY;
    String patternFile = defaultSize ? String("") : String("cm_");
    patternFile += LOCK_PATTERN_FILE;

    if (userId == 0) {
        // Leave it in the same place for user 0
        return dataSystemDirectory + patternFile;
    }
    else {
        AutoPtr<IFile> sysDir;
        env->GetUserSystemDirectory(userId, (IFile**)&sysDir);
        AutoPtr<IFile> tmpDir;
        CFile::New(sysDir, patternFile, (IFile**)&tmpDir);
        tmpDir->GetAbsolutePath(&absPath);
        return absPath;
    }
}

String CLockSettingsService::GetLockPasswordFilename(
    /* [in] */ Int32 uid)
{
    Int32 userId = GetUserParentOrSelfId(uid);
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dataDir;
    env->GetDataDirectory((IFile**)&dataDir);
    assert(dataDir != NULL);
    String absPath;
    dataDir->GetAbsolutePath(&absPath);

    String dataSystemDirectory = absPath + SYSTEM_DIRECTORY;
    if (userId == 0) {
        // Leave it in the same place for user 0
        return dataSystemDirectory + LOCK_PASSWORD_FILE;
    } else {
        AutoPtr<IFile> sysDir;
        env->GetUserSystemDirectory(userId, (IFile**)&sysDir);
        AutoPtr<IFile> tmpDir;
        CFile::New(sysDir, LOCK_PASSWORD_FILE, (IFile**)&tmpDir);
        tmpDir->GetAbsolutePath(&absPath);
        return absPath;
    }
}

ECode CLockSettingsService::HavePassword(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    // Do we need a permissions check here?
    AutoPtr<IFile> tmp;
    CFile::New(GetLockPasswordFilename(userId), (IFile**)&tmp);
    Int64 len = 0;
    tmp->GetLength(&len);
    *res = len > 0;
    return NOERROR;
}

void CLockSettingsService::MaybeUpdateKeystore(
    /* [in] */ const String& password,
    /* [in] */ Int32 userHandle)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> um = IUserManager::Probe(obj);

    assert(0 && "TODO");
    // AutoPtr<IKeyStoreHelper> helper;
    // CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&helper);
    // AutoPtr<IKeyStore> ks;
    // helper->GetInstance((IKeyStore**)&ks);

    AutoPtr<IList> profiles;
    um->GetProfiles(userHandle, (IList**)&profiles);
    Boolean shouldReset = TextUtils::IsEmpty(password);

    // For historical reasons, don't wipe a non-empty keystore if we have a single user with a
    // single profile.
    Int32 size;
    profiles->GetSize(&size);
    if (userHandle == IUserHandle::USER_OWNER && size == 1) {
        // if (!ks->IsEmpty()) {
        //     shouldReset = FALSE;
        // }
    }

    // AutoPtr<IIterator> it;
    // profiles->GetIterator((IIterator**)&it);
    // Boolean hasNext;
    // while (it->HasNext(&hasNext)) {
    //     AutoPtr<IInterface> obj;
    //     it->GetNext((IInterface**)&obj);
    //     IUserInfo* ui = IUserInfo::Probe(obj);
    //     Int32 profileUid = UserHandle::GetUid(pi.id, IProcess::SYSTEM_UID);
    //     if (shouldReset) {
    //         ks->ResetUid(profileUid);
    //     } else {
    //         ks->PasswordUid(password, profileUid);
    //     }
    // }
}

ECode CLockSettingsService::HavePattern(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    // Do we need a permissions check here?
    AutoPtr<IFile> tmp;
    CFile::New(GetLockPatternFilename(userId), (IFile**)&tmp);
    Int64 len = 0;
    tmp->GetLength(&len);
    *res = len > 0;
    return NOERROR;
}

ECode CLockSettingsService::SetLockPattern(
    /* [in] */ const String& pattern,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(CheckWritePermission(userId))

    MaybeUpdateKeystore(pattern, userId);

    AutoPtr<ArrayOf<Byte> > hash;
    AutoPtr<IList> list;
    mLockPatternUtils->StringToPattern(pattern, (IList**)&list);
    mLockPatternUtils->PatternToHash(list, (ArrayOf<Byte>**)&hash);
    Boolean defaultSize = IsDefaultSize(userId);
    WriteFile(GetLockPatternFilename(userId, defaultSize), hash);
    WriteFile(GetLockPatternFilename(userId, !defaultSize), NULL);
    return NOERROR;
}

ECode CLockSettingsService::SetLockPassword(
    /* [in] */ const String& password,
    /* [in] */ Int32 userId)
{
    CheckWritePermission(userId);

    MaybeUpdateKeystore(password, userId);

    AutoPtr<ArrayOf<Byte> > hash;
    mLockPatternUtils->PasswordToHash(password, userId, (ArrayOf<Byte>**)&hash);
    return WriteFile(GetLockPasswordFilename(userId), hash);
}

ECode CLockSettingsService::CheckPattern(
    /* [in] */ const String& pattern,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    FAIL_RETURN(CheckPasswordReadPermission(userId))

    AutoPtr<IList> list;
    AutoPtr<ArrayOf<Byte> > bytes;
    Boolean matched;

    // try {
    // Read all the bytes from the file
    AutoPtr<IRandomAccessFile> raf;
    CRandomAccessFile::New(GetLockPatternFilename(userId),
        String("r"), (IRandomAccessFile**)&raf);

    Int64 len = 0;
    raf->GetLength(&len);
    AutoPtr<ArrayOf<Byte> > stored = ArrayOf<Byte>::Alloc((Int32)len);
    Int32 got = 0;
    FAIL_GOTO(raf->Read(stored, 0, stored->GetLength(), &got), ERROR);
    ICloseable::Probe(raf)->Close();
    if (got <= 0) {
        *res = TRUE;
        return NOERROR;
    }

    mLockPatternUtils->StringToPattern(pattern, (IList**)&list);
    mLockPatternUtils->PatternToHash(list, (ArrayOf<Byte>**)&bytes);
    matched = Arrays::Equals(stored, bytes);
    if (matched && !TextUtils::IsEmpty(pattern)) {
        MaybeUpdateKeystore(pattern, userId);
    }
    *res = matched;
    return NOERROR;
    // } catch (FileNotFoundException fnfe) {
    //     Slogger::E(TAG, "Cannot read file " + fnfe);
    //     return TRUE;
    // } catch (IOException ioe) {
    //     Slogger::E(TAG, "Cannot read file " + ioe);
    //     return TRUE;
    // }
ERROR:
    ICloseable::Probe(raf)->Close();
    *res = TRUE;
    return NOERROR;
}

ECode CLockSettingsService::CheckPassword(
    /* [in] */ const String& password,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    FAIL_RETURN(CheckPasswordReadPermission(userId))

    Int64 len = 0;
    Int32 got = 0;
    Boolean matched;
    AutoPtr<ArrayOf<Byte> > hash;
    AutoPtr<ArrayOf<Byte> > stored;

    // try {
    // Read all the bytes from the file
    AutoPtr<IRandomAccessFile> raf;
    ECode ec = CRandomAccessFile::New(GetLockPasswordFilename(userId), String("r"), (IRandomAccessFile**)&raf);
    FAIL_GOTO(ec, ERROR)

    raf->GetLength(&len);
    stored = ArrayOf<Byte>::Alloc((Int32)len);
    FAIL_GOTO(raf->Read(stored, 0, stored->GetLength(), &got), ERROR);
    ICloseable::Probe(raf)->Close();
    if (got <= 0) {
        *res = TRUE;
        return NOERROR;
    }
    // Compare the hash from the file with the entered password's hash
    mLockPatternUtils->PasswordToHash(password, userId, (ArrayOf<Byte>**)&hash);
    matched = Arrays::Equals(stored, hash);
    if (matched && !TextUtils::IsEmpty(password)) {
        MaybeUpdateKeystore(password, userId);
    }
    *res = matched;
    return NOERROR;
    // } catch (FileNotFoundException fnfe) {
    //     Slogger::E(TAG, "Cannot read file " + fnfe);
    //     return TRUE;
    // } catch (IOException ioe) {
    //     Slogger::E(TAG, "Cannot read file " + ioe);
    //     return TRUE;
    // }
ERROR:
    ICloseable::Probe(raf)->Close();
    *res = TRUE;
    return NOERROR;
}

ECode CLockSettingsService::CheckVoldPassword(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;

    if (!mFirstCallToVold) {
        return NOERROR;
    }
    mFirstCallToVold = FALSE;

    FAIL_RETURN(CheckPasswordReadPermission(userId))

    // There's no guarantee that this will safely connect, but if it fails
    // we will simply show the lock screen when we shouldn't, so relatively
    // benign. There is an outside chance something nasty would happen if
    // this service restarted before vold stales out the password in this
    // case. The nastiness is limited to not showing the lock screen when
    // we should, within the first minute of decrypting the phone if this
    // service can't connect to vold, it restarts, and then the new instance
    // does successfully connect.
    AutoPtr<IIMountService> service = GetMountService();
    String password;
    service->GetPassword(&password);
    service->ClearPassword();
    if (password == NULL) {
        return NOERROR;
    }

    Boolean bval;
    mLockPatternUtils->IsLockPatternEnabled(&bval);
    if (bval) {
        if (CheckPattern(password, userId, &bval), bval) {
            *res = TRUE;
            return NOERROR;
        }
    }

    mLockPatternUtils->IsLockPasswordEnabled(&bval);
    if (bval) {
        if (CheckPassword(password, userId, &bval), bval) {
            *res = TRUE;
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CLockSettingsService::RemoveUser(
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(CheckWritePermission(userId))

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&service);
    AutoPtr<IUserManager> um = IUserManager::Probe(service);
    AutoPtr<IUserInfo> parentInfo;
    um->GetProfileParent(userId, (IUserInfo**)&parentInfo);
    if (parentInfo == NULL) {
        //try {
        AutoPtr<IFile> file;
        CFile::New(GetLockPasswordFilename(userId), (IFile**)&file);
        Boolean exist = FALSE, isDeleted = FALSE;
        if (file->Exists(&exist), exist) {
            file->Delete(&isDeleted);
        }

        file = NULL;
        CFile::New(GetLockPatternFilename(userId), (IFile**)&file);
        if (file->Exists(&exist), exist) {
            file->Delete(&isDeleted);
        }

        db->BeginTransaction();
        Int32 value = 0;
        db->Delete(TABLE, COLUMN_USERID + String("='") + StringUtils::ToString(userId) + String("'"), NULL, &value);
        db->SetTransactionSuccessful();
        //} finally {
        db->EndTransaction();
        //}
    }

    assert(0 && "TODO");
    // AutoPtr<IKeyStoreHelper> helper;
    // CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&helper);
    // AutoPtr<IKeyStore> ks;
    // helper->GetInstance((IKeyStore**)&ks);
    // Int32 userUid = UserHandle::GetUid(userId, IProcess::SYSTEM_UID);
    // ks->ResetUid(userUid);
    return NOERROR;
}

ECode CLockSettingsService::WriteFile(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Byte>* hash)
{
    // try {
    // Write the hash to file
    AutoPtr<IRandomAccessFile> raf;
    CRandomAccessFile::New(name, String("rw"), (IRandomAccessFile**)&raf);
    // Truncate the file if pattern is NULL, to clear the lock
    if (hash == NULL || hash->GetLength() == 0) {
        raf->SetLength(0);
    } else {
        IDataOutput::Probe(raf)->Write(hash, 0, hash->GetLength());
    }
    ICloseable::Probe(raf)->Close();
    // } catch (IOException ioe) {
    //     Slogger::E(TAG, "Error writing to file " + ioe);
    // }
    return NOERROR;
}

ECode CLockSettingsService::WriteToDb(
    /* [in] */ const String& key,
    /* [in] */ const String& value,
    /* [in] */ Int32 userId)
{
    AutoPtr<ISQLiteDatabase> database;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&database);
    WriteToDb(database, key, value, userId);
    NotifyObservers(key, userId);
    return NOERROR;
}

ECode CLockSettingsService::WriteToDb(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& key,
    /* [in] */ const String& value,
    /* [in] */ Int32 userId)
{
    ECode ec = NOERROR;
    AutoPtr<IContentValues> cv;
    CContentValues::New((IContentValues**)&cv);
    AutoPtr<ICharSequence> strValue;
    CString::New(key, (ICharSequence**)&strValue);
    cv->Put(COLUMN_KEY, strValue);

    AutoPtr<IInteger32> intValue;
    CInteger32::New(userId, (IInteger32**)&intValue);
    cv->Put(COLUMN_USERID, intValue);

    strValue = NULL;
    CString::New(value, (ICharSequence**)&strValue);
    cv->Put(COLUMN_VALUE, strValue);

    db->BeginTransaction();
    //try {
    AutoPtr<ArrayOf<String> > keys = ArrayOf<String>::Alloc(2);
    keys->Set(0, key);
    keys->Set(1, StringUtils::ToString(userId));
    Int32 tmp = 0;
    ec = db->Delete(TABLE, COLUMN_KEY + String("=? AND ") + COLUMN_USERID + String("=?"), keys, &tmp);
    FAIL_RETURN(ec)

    Int64 id = 0;
    ec = db->Insert(TABLE, String(NULL), cv, &id);
    FAIL_RETURN(ec)

    db->SetTransactionSuccessful();
    //} finally {
    ec = db->EndTransaction();
    //}
    return ec;
}

ECode CLockSettingsService::ReadFromDb(
    /* [in] */ const String& key,
    /* [in] */ const String& defaultValue,
    /* [in] */ Int32 userId,
    /* [out]*/ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    AutoPtr<ICursor> cursor;
    String result = defaultValue;
    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db))

    assert(db != NULL);
    String selection = COLUMN_USERID + String("=? AND ") + COLUMN_KEY + String("=?");
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    args->Set(0, StringUtils::ToString(userId));
    args->Set(1, key);
    FAIL_RETURN(db->Query(TABLE, COLUMNS_FOR_QUERY, selection, args, String(NULL),
        String(NULL), String(NULL), (ICursor**)&cursor))

    if (cursor != NULL) {
        Boolean succeeded = FALSE;
        cursor->MoveToFirst(&succeeded);
        if (succeeded) {
            cursor->GetString(0, &result);
        }
        ICloseable::Probe(cursor)->Close();
    }
    *value = result;
    return NOERROR;
}

AutoPtr<IIMountService> CLockSettingsService::GetMountService()
{
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> obj;
    sm->GetService(String("mount"), (IInterface**)&obj);
    if (obj != NULL) {
        return IIMountService::Probe(obj);
    }
    return NULL;
}

}// namespace Server
}// namespace Droid
}// namespace Elastos
