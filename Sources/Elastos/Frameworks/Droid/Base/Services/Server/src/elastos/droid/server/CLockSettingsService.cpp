
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <text/TextUtils.h>
#include "CLockSettingsService.h"
#include "elastos/droid/os/Binder.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::EIID_ISQLiteOpenHelper;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Text::TextUtils;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::IRandomAccessFile;
using Elastos::IO::CRandomAccessFile;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

String CLockSettingsService::DatabaseHelper::TAG("LockSettingsDB");
String CLockSettingsService::DatabaseHelper::DATABASE_NAME = String("locksettings.db");
const Int32 CLockSettingsService::DatabaseHelper::DATABASE_VERSION = 1;

CAR_INTERFACE_IMPL(CLockSettingsService::DatabaseHelper, ISQLiteOpenHelper);

CLockSettingsService::DatabaseHelper::DatabaseHelper(
    /* [in] */ IContext* context,
    /* [in] */ CLockSettingsService* host)
    : mHost(host)
{
    SQLiteOpenHelper::Init(context, DATABASE_NAME, NULL, DATABASE_VERSION);
    SetWriteAheadLoggingEnabled(TRUE);
}

void CLockSettingsService::DatabaseHelper::CreateTable(
    /* [in] */ ISQLiteDatabase* db)
{
    StringBuilder sb("CREATE TABLE ");
    sb += TABLE + " (" +
            "_id INTEGER PRIMARY KEY AUTOINCREMENT," +
            COLUMN_KEY + " TEXT," +
            COLUMN_USERID + " INTEGER," +
            COLUMN_VALUE + " TEXT" +
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
    // Nothing yet
    return NOERROR;
}

ECode CLockSettingsService::DatabaseHelper::GetDatabaseName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
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
    VALIDATE_NOT_NULL(database);
    return SQLiteOpenHelper::GetWritableDatabase(database);
}

ECode CLockSettingsService::DatabaseHelper::GetReadableDatabase(
    /* [out] */ ISQLiteDatabase** database)
{
    VALIDATE_NOT_NULL(database);
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


//class CLockSettingsService's implement.
String CLockSettingsService::TAG("LockSettingsService");
String CLockSettingsService::TABLE = String("locksettings");
String CLockSettingsService::COLUMN_KEY = String("name");
String CLockSettingsService::COLUMN_USERID = String("user");
String CLockSettingsService::COLUMN_VALUE = String("value");

AutoPtr<ArrayOf<String> > CLockSettingsService::COLUMNS_FOR_QUERY;

String CLockSettingsService::SYSTEM_DIRECTORY = String("/system/");
String CLockSettingsService::LOCK_PATTERN_FILE = String("gesture.key");
String CLockSettingsService::LOCK_PASSWORD_FILE = String("password.key");
AutoPtr<ArrayOf<String> > CLockSettingsService::VALID_SETTINGS;
Boolean CLockSettingsService::sInitArray = CLockSettingsService::InitArray();

Boolean CLockSettingsService::InitArray()
{
    COLUMNS_FOR_QUERY = ArrayOf<String>::Alloc(1);
    COLUMNS_FOR_QUERY->Set(0, COLUMN_VALUE);

    VALID_SETTINGS = ArrayOf<String>::Alloc(16);
    VALID_SETTINGS->Set(0, String("lockscreen.lockedoutpermanently") /*LockPatternUtils.LOCKOUT_PERMANENT_KEY*/);
    VALID_SETTINGS->Set(1, String("lockscreen.lockoutattemptdeadline") /*LockPatternUtils.LOCKOUT_ATTEMPT_DEADLINE*/);
    VALID_SETTINGS->Set(2, String("lockscreen.patterneverchosen") /*LockPatternUtils.PATTERN_EVER_CHOSEN_KEY*/);
    VALID_SETTINGS->Set(3, String("lockscreen.password_type") /*LockPatternUtils.PASSWORD_TYPE_KEY*/);

    VALID_SETTINGS->Set(4, String("lockscreen.password_type_alternate") /*LockPatternUtils.PASSWORD_TYPE_ALTERNATE_KEY*/);
    VALID_SETTINGS->Set(5, String("lockscreen.password_salt") /*LockPatternUtils.LOCK_PASSWORD_SALT_KEY*/);
    VALID_SETTINGS->Set(6, String("lockscreen.disabled") /*LockPatternUtils.DISABLE_LOCKSCREEN_KEY*/);
    VALID_SETTINGS->Set(7, String("lockscreen.options") /*LockPatternUtils.LOCKSCREEN_OPTIONS*/);

    VALID_SETTINGS->Set(8, String("lockscreen.biometric_weak_fallback") /*LockPatternUtils.LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK*/);
    VALID_SETTINGS->Set(9, String("lockscreen.biometricweakeverchosen") /*LockPatternUtils.BIOMETRIC_WEAK_EVER_CHOSEN_KEY*/);
    VALID_SETTINGS->Set(10, String("lockscreen.power_button_instantly_locks") /*LockPatternUtils.LOCKSCREEN_POWER_BUTTON_INSTANTLY_LOCKS*/);
    VALID_SETTINGS->Set(11, String("lockscreen.passwordhistory") /*LockPatternUtils.PASSWORD_HISTORY_KEY*/);

    VALID_SETTINGS->Set(12, String("lock_pattern_autolock")/*ISettingsSecure::LOCK_PATTERN_ENABLED*/);
    VALID_SETTINGS->Set(13, String("lock_biometric_weak_flags")/*ISettingsSecure::LOCK_BIOMETRIC_WEAK_FLAGS*/);
    VALID_SETTINGS->Set(14, String("lock_pattern_visible_pattern")/*ISettingsSecure::LOCK_PATTERN_VISIBLE*/);
    VALID_SETTINGS->Set(15, String("lock_pattern_tactile_feedback_enabled")/*ISettingsSecure::LOCK_PATTERN_TACTILE_FEEDBACK_ENABLED*/);
    return TRUE;
}

ECode CLockSettingsService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    // Open the database
    mOpenHelper = new DatabaseHelper(mContext, this);
    return NOERROR;
}

ECode CLockSettingsService::SystemReady()
{
    MigrateOldData();

    return NOERROR;
}

void CLockSettingsService::MigrateOldData()
{
    //try {
    String data;
    if ((GetString(String("migrated"), String(NULL), 0, &data), data) != NULL) {
        // Already migrated
        return;
    }

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    for (Int32 i = 0; i < VALID_SETTINGS->GetLength(); i++) {
        String validSetting = (*VALID_SETTINGS)[i];
        String value;
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        settingsSecure->GetString(cr, validSetting, &value);
        if (value != NULL) {
            SetString(validSetting, value, 0);
        }
    }
    // No need to move the password / pattern files. They're already in the right place.
    SetString(String("migrated"), String("TRUE"), 0);
        //Slog.i(TAG, "Migrated lock settings to new location");
    /*} catch (RemoteException re) {
        Slog.e(TAG, "Unable to migrate old data");
    }*/
}

void CLockSettingsService::CheckWritePermission(
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);

    Int32 appId = 0;
    helper->GetAppId(callingUid, &appId);
    if (appId != IProcess::SYSTEM_UID) {
        assert(0);
        // throw new SecurityException("uid=" + callingUid
        //         + " not authorized to write lock settings");
    }
}

void CLockSettingsService::CheckPasswordReadPermission(
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);

    Int32 appId = 0;
    helper->GetAppId(callingUid, &appId);
    if (appId != IProcess::SYSTEM_UID) {
        assert(0);
        // throw new SecurityException("uid=" + callingUid
        //         + " not authorized to read lock password");
    }
}

void CLockSettingsService::CheckReadPermission(
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);

    Int32 appId = 0, uId = 0;
    helper->GetAppId(callingUid, &appId);
    helper->GetUserId(callingUid, &uId);
    if (appId != IProcess::SYSTEM_UID && uId != userId) {
        assert(0);
        // throw new SecurityException("uid=" + callingUid
        //         + " not authorized to read settings of user " + userId);
    }
}

ECode CLockSettingsService::SetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean value,
    /* [in] */ Int32 userId)
{
    CheckWritePermission(userId);

    WriteToDb(key, value ? String("1") : String("0"), userId);
    return NOERROR;
}

ECode CLockSettingsService::SetInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 value,
    /* [in] */ Int32 userId)
{
    CheckWritePermission(userId);

    WriteToDb(key, StringUtils::Int64ToString(value), userId);
    return NOERROR;
}

ECode CLockSettingsService::SetString(
    /* [in] */ const String& key,
    /* [in] */ const String& value,
    /* [in] */ Int32 userId)
{
    CheckWritePermission(userId);

    WriteToDb(key, value, userId);
    return NOERROR;
}

ECode CLockSettingsService::GetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean defaultValue,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* res)
{
    //checkReadPermission(userId);
    VALIDATE_NOT_NULL(res);
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
    //checkReadPermission(userId);

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
    //checkReadPermission(userId);
    return ReadFromDb(key, defaultValue, userId, retValue);
}

String CLockSettingsService::GetLockPatternFilename(
    /* [in] */ Int32 userId)
{
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
        return dataSystemDirectory + LOCK_PATTERN_FILE;
    } else {
        AutoPtr<IFile> sysDir;
        env->GetUserSystemDirectory(userId, (IFile**)&sysDir);
        AutoPtr<IFile> tmpDir;
        CFile::New(sysDir, LOCK_PATTERN_FILE, (IFile**)&tmpDir);
        tmpDir->GetAbsolutePath(&absPath);
        return absPath;
    }
}

String CLockSettingsService::GetLockPasswordFilename(
    /* [in] */ Int32 userId)
{
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
    /* [in] */ const ArrayOf<Byte>& hash,
    /* [in] */ Int32 userId)
{
    CheckWritePermission(userId);

    WriteFile(GetLockPatternFilename(userId), (ArrayOf<Byte>*)&hash);
    return NOERROR;
}

ECode CLockSettingsService::CheckPattern(
    /* [in] */ const ArrayOf<Byte>& hash,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    CheckPasswordReadPermission(userId);
    // try {
    // Read all the bytes from the file
    AutoPtr<IRandomAccessFile> raf;
    CRandomAccessFile::New(GetLockPatternFilename(userId), String("r"), (IRandomAccessFile**)&raf);

    Int64 len = 0;
    raf->GetLength(&len);
    AutoPtr<ArrayOf<Byte> > stored = ArrayOf<Byte>::Alloc((Int32)len);
    Int32 got = 0;
    FAIL_GOTO(raf->ReadBytes(stored, 0, stored->GetLength(), &got), ERROR);
    raf->Close();
    if (got <= 0) {
        *res = TRUE;
        return NOERROR;
    }
    // Compare the hash from the file with the entered pattern's hash
    *res = stored->Equals(&hash);
    return NOERROR;
    // } catch (FileNotFoundException fnfe) {
    //     Slog.e(TAG, "Cannot read file " + fnfe);
    //     return true;
    // } catch (IOException ioe) {
    //     Slog.e(TAG, "Cannot read file " + ioe);
    //     return true;
    // }
ERROR:
    raf->Close();
    *res = TRUE;
    return NOERROR;
}

ECode CLockSettingsService::SetLockPassword(
    /* [in] */ const ArrayOf<Byte>& hash,
    /* [in] */ Int32 userId)
{
    CheckWritePermission(userId);

    WriteFile(GetLockPasswordFilename(userId), (ArrayOf<Byte>*)&hash);
    return NOERROR;
}

ECode CLockSettingsService::CheckPassword(
    /* [in] */ const ArrayOf<Byte>& hash,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    CheckPasswordReadPermission(userId);

    // try {
    // Read all the bytes from the file
    AutoPtr<IRandomAccessFile> raf;
    CRandomAccessFile::New(GetLockPasswordFilename(userId), String("r"), (IRandomAccessFile**)&raf);
    Int64 len = 0;
    raf->GetLength(&len);
    AutoPtr<ArrayOf<Byte> > stored = ArrayOf<Byte>::Alloc((Int32)len);
    Int32 got = 0;
    FAIL_GOTO(raf->ReadBytes(stored, 0, stored->GetLength(), &got), ERROR);
    raf->Close();
    if (got <= 0) {
        *res = TRUE;
        return NOERROR;
    }
    // Compare the hash from the file with the entered password's hash
    *res = stored->Equals(&hash);
    return NOERROR;
    // } catch (FileNotFoundException fnfe) {
    //     Slog.e(TAG, "Cannot read file " + fnfe);
    //     return true;
    // } catch (IOException ioe) {
    //     Slog.e(TAG, "Cannot read file " + ioe);
    //     return true;
    // }
ERROR:
    raf->Close();
    *res = TRUE;
    return NOERROR;
}

ECode CLockSettingsService::RemoveUser(
    /* [in] */ Int32 userId)
{
    CheckWritePermission(userId);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
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
    db->Delete(TABLE, COLUMN_USERID + String("='") + StringUtils::Int32ToString(userId) + String("'"), NULL, &value);
    db->SetTransactionSuccessful();
    //} finally {
    db->EndTransaction();
    //}

    return NOERROR;
}

void CLockSettingsService::WriteFile(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Byte>* hash)
{
    // try {
    // Write the hash to file
    AutoPtr<IRandomAccessFile> raf;
    CRandomAccessFile::New(name, String("rw"), (IRandomAccessFile**)&raf);
    // Truncate the file if pattern is null, to clear the lock
    if (hash == NULL || hash->GetLength() == 0) {
        raf->SetLength(0);
    } else {
        raf->WriteBytes(*hash, 0, hash->GetLength());
    }
    raf->Close();
    // } catch (IOException ioe) {
    //     Slog.e(TAG, "Error writing to file " + ioe);
    // }
}

void CLockSettingsService::WriteToDb(
    /* [in] */ const String& key,
    /* [in] */ const String& value,
    /* [in] */ Int32 userId)
{
    AutoPtr<ISQLiteDatabase> database;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&database);
    WriteToDb(database, key, value, userId);
}

void CLockSettingsService::WriteToDb(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& key,
    /* [in] */ const String& value,
    /* [in] */ Int32 userId)
{
    AutoPtr<IContentValues> cv;
    CContentValues::New((IContentValues**)&cv);
    AutoPtr<ICharSequence> strValue;
    CString::New(key, (ICharSequence**)&strValue);
    cv->PutString(COLUMN_KEY, strValue);

    AutoPtr<IInteger32> intValue;
    CInteger32::New(userId, (IInteger32**)&intValue);
    cv->PutInt32(COLUMN_USERID, intValue);

    strValue = NULL;
    CString::New(value, (ICharSequence**)&strValue);
    cv->PutString(COLUMN_VALUE, strValue);

    db->BeginTransaction();
    //try {
    AutoPtr<ArrayOf<String> > keys = ArrayOf<String>::Alloc(2);
    keys->Set(0, key);
    keys->Set(1, StringUtils::Int32ToString(userId));
    Int32 tmp = 0;
    db->Delete(TABLE, COLUMN_KEY + String("=? AND ") + COLUMN_USERID + String("=?"), keys, &tmp);

    Int64 id = 0;
    db->Insert(TABLE, String(NULL), cv, &id);
    db->SetTransactionSuccessful();
    //} finally {
    db->EndTransaction();
    //}
}

ECode CLockSettingsService::ReadFromDb(
    /* [in] */ const String& key,
    /* [in] */ const String& defaultValue,
    /* [in] */ Int32 userId,
    /* [out]*/ String* value)
{
    AutoPtr<ICursor> cursor;
    String result = defaultValue;
    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db))

    assert(db != NULL);
    String selection = COLUMN_USERID + String("=? AND ") + COLUMN_KEY + String("=?");
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    args->Set(0, StringUtils::Int32ToString(userId));
    args->Set(1, key);
    FAIL_RETURN(db->Query(TABLE, COLUMNS_FOR_QUERY, selection, args, String(NULL),
        String(NULL), String(NULL), (ICursor**)&cursor))

    if (cursor != NULL) {
        Boolean succeeded = FALSE;
        cursor->MoveToFirst(&succeeded);
        if (succeeded) {
            cursor->GetString(0, &result);
        }
        cursor->Close();
    }
    *value = result;
    return NOERROR;
}

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
