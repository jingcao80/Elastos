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

#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/privacy/CPrivacyPersistenceAdapter.h"
#include "elastos/droid/privacy/PrivacySettingsStub.h"
#include "elastos/droid/privacy/CPrivacySettings.h"
#include "elastos/droid/database/sqlite/SQLiteDatabase.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Utility::Etl::HashSet;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CByte;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IOutputStreamWriter;
using Elastos::IO::COutputStreamWriter;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Database::Sqlite::SQLiteDatabase;

namespace Elastos {
namespace Droid {
namespace Privacy {

static AutoPtr<ICharSequence> GetCharSequence(
    /* [in] */ const String& s)
{
    AutoPtr<ICharSequence> cs;
    CString::New(s, (ICharSequence**)&cs);
    return cs;
}

static AutoPtr<IByte> GetIByte(
    /* [in] */ Byte b)
{
    AutoPtr<IByte> ibyte;
    CByte::New(b, (IByte**)&ibyte);
    return ibyte;
}

String CPrivacyPersistenceAdapter::InitCreateTableSettings()
{
    StringBuilder sb(1024);
    sb += "CREATE TABLE IF NOT EXISTS ";
    sb += TABLE_SETTINGS +
        " ( " +                                                 // index
        " _id INTEGER PRIMARY KEY AUTOINCREMENT, " +             // 0
        " packageName TEXT, " +                                  // 1
        " uid INTEGER, " +                                       // 2
        " deviceIdSetting INTEGER, " +                           // 3
        " deviceId TEXT, " +                                     // 4
        " line1NumberSetting INTEGER, " +                        // 5
        " line1Number TEXT, " +                                  // 6
        " locationGpsSetting INTEGER, " +                        // 7
        " locationGpsLat TEXT, " +                               // 8
        " locationGpsLon TEXT, " +                               // 9
        " locationNetworkSetting INTEGER, " +                    // 10
        " locationNetworkLat TEXT, " +                           // 11
        " locationNetworkLon TEXT, " +                           // 12
        " networkInfoSetting INTEGER, " +                        // 13
        " simInfoSetting INTEGER, " +                            // 14
        " simSerialNumberSetting INTEGER, " +                    // 15
        " simSerialNumber TEXT, " +                              // 16
        " subscriberIdSetting INTEGER, " +                       // 17
        " subscriberId TEXT, " +                                 // 18
        " accountsSetting INTEGER, "  +                          // 19
        " accountsAuthTokensSetting INTEGER, " +                 // 20
        " outgoingCallsSetting INTEGER, "  +                     // 21
        " incomingCallsSetting INTEGER, " +                      // 22
        " contactsSetting INTEGER, " +                           // 23
        " calendarSetting INTEGER, " +                           // 24
        " mmsSetting INTEGER, " +                                // 25
        " smsSetting INTEGER, " +                                // 26
        " callLogSetting INTEGER, "  +                           // 27
        " bookmarksSetting INTEGER, " +                          // 28
        " systemLogsSetting INTEGER, "  +                        // 29
        " externalStorageSetting INTEGER, " +                    // 30
        " cameraSetting INTEGER, " +                             // 31
        " recordAudioSetting INTEGER, " +                        // 32
        " notificationSetting INTEGER, " +                       // 33
        " intentBootCompletedSetting INTEGER, "                 // 34
        " smsSendSetting INTEGER, " +                            // 35
        " phoneCallSetting INTEGER, " +                          // 36
        " ipTableProtectSetting INTEGER, " +                     // 37
        " iccAccessSetting INTEGER, " +                          // 38
        " addOnManagementSetting INTEGER, " +                    // 39   //this setting indicate if app is managed by addon or not
        " androidIdSetting INTEGER, " +                          // 40
        " androidId TEXT, " +                                    // 41
        " wifiInfoSetting INTEGER, " +                           // 42
        " switchConnectivitySetting INTEGER, " +                 // 43
        " sendMmsSetting INTEGER, "  +                           // 44
        " forceOnlineState INTEGER, " +                          // 45
        " switchWifiStateSetting INTEGER " +                     // 46
        ");";
    return sb.ToString();
}

String CPrivacyPersistenceAdapter::InitCreateTableMap()
{
    String s("CREATE TABLE IF NOT EXISTS ");
    s += TABLE_MAP +
        " ( name TEXT PRIMARY KEY, value TEXT );";
    return s;
}

String CPrivacyPersistenceAdapter::InitCreateTableAllowedContacts()
{
    String s("CREATE TABLE IF NOT EXISTS ");
    s += TABLE_ALLOWED_CONTACTS +
        " ( settings_id, contact_id, PRIMARY KEY(settings_id, contact_id) );";
    return s;
}

String CPrivacyPersistenceAdapter::InitInsertVersion()
{
    StringBuilder sb("INSERT OR REPLACE INTO ");
    sb += TABLE_MAP;
    sb += " (name, value) ";
    sb += "VALUES (\"db_version\", ";
    // sb += DATABASE_VERSION;
    sb += ");";
    return sb.ToString();
}

String CPrivacyPersistenceAdapter::InitInsertEnabled()
{
    String s("INSERT OR REPLACE INTO ");
    s += TABLE_MAP +
        " (name, value) " +
        "VALUES (\"enabled\", \"1\");";
    return s;
}

String CPrivacyPersistenceAdapter::InitInsertNotificationsEnabled()
{
    String s("INSERT OR REPLACE INTO ");
    s += TABLE_MAP +
        " (name, value) " +
        "VALUES (\"notifications_enabled\", \"1\");";
    return s;
}

AutoPtr< ArrayOf<String> > CPrivacyPersistenceAdapter::InitDatabaseFields()
{
    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(47);
    (*array)[0] = "_id";
    (*array)[1] = "packageName";
    (*array)[2] = "uid";
    (*array)[3] = "deviceIdSetting";
    (*array)[4] = "deviceId";
    (*array)[5] = "line1NumberSetting";
    (*array)[6] = "line1Number";
    (*array)[7] = "locationGpsSetting";
    (*array)[8] = "locationGpsLat";
    (*array)[9] = "locationGpsLon";
    (*array)[10] = "locationNetworkSetting";
    (*array)[11] = "locationNetworkLat";
    (*array)[12] = "locationNetworkLon";
    (*array)[13] = "networkInfoSetting";
    (*array)[14] = "simInfoSetting";
    (*array)[15] = "simSerialNumberSetting";
    (*array)[16] = "simSerialNumber";
    (*array)[17] = "subscriberIdSetting";
    (*array)[18] = "subscriberId";
    (*array)[19] = "accountsSetting";
    (*array)[20] = "accountsAuthTokensSetting";
    (*array)[21] = "outgoingCallsSetting";
    (*array)[22] = "incomingCallsSetting";
    (*array)[23] = "contactsSetting";
    (*array)[24] = "calendarSetting";
    (*array)[25] = "mmsSetting";
    (*array)[26] = "smsSetting";
    (*array)[27] = "callLogSetting";
    (*array)[28] = "bookmarksSetting";
    (*array)[29] = "systemLogsSetting";
    (*array)[30] = "externalStorageSetting";
    (*array)[31] = "cameraSetting";
    (*array)[32] = "recordAudioSetting";
    (*array)[33] = "notificationSetting";
    (*array)[34] = "intentBootCompletedSetting";
    (*array)[35] = "smsSendSetting";
    (*array)[36] = "phoneCallSetting";
    (*array)[37] = "ipTableProtectSetting";
    (*array)[38] = "iccAccessSetting";
    (*array)[39] = "addOnManagementSetting";
    (*array)[40] = "androidIdSetting";
    (*array)[41] = "androidId";
    (*array)[42] = "wifiInfoSetting";
    (*array)[43] = "switchConnectivitySetting";
    (*array)[44] = "sendMmsSetting";
    (*array)[45] = "forceOnlineState";
    (*array)[46] = "switchWifiStateSetting";
    return array;
}

const String CPrivacyPersistenceAdapter::TAG("CPrivacyPersistenceAdapter");
const String CPrivacyPersistenceAdapter::DATABASE_FILE("/data/system/privacy.db");
const String CPrivacyPersistenceAdapter::SETTINGS_DIRECTORY("/data/system/privacy");
const String CPrivacyPersistenceAdapter::TABLE_SETTINGS("settings");
const String CPrivacyPersistenceAdapter::TABLE_MAP("map");
const String CPrivacyPersistenceAdapter::TABLE_ALLOWED_CONTACTS("allowed_contacts");
const String CPrivacyPersistenceAdapter::TABLE_VERSION("version");

HashMap<String, AutoPtr<IInterface> > CPrivacyPersistenceAdapter::mSettingsCache(DEFAULT_CACHE_ENTRIES);

volatile Int32 CPrivacyPersistenceAdapter::sDbAccessThreads = 0;
Mutex CPrivacyPersistenceAdapter::sDbAccessThreadsLock;
volatile Int32 CPrivacyPersistenceAdapter::sDbVersion;

const String CPrivacyPersistenceAdapter::CREATE_TABLE_SETTINGS = InitCreateTableSettings();
const String CPrivacyPersistenceAdapter::CREATE_TABLE_MAP = InitCreateTableMap();
const String CPrivacyPersistenceAdapter::CREATE_TABLE_ALLOWED_CONTACTS = InitCreateTableAllowedContacts();
const String CPrivacyPersistenceAdapter::INSERT_VERSION = InitInsertVersion();
const String CPrivacyPersistenceAdapter::INSERT_ENABLED = InitInsertEnabled();
const String CPrivacyPersistenceAdapter::INSERT_NOTIFICATIONS_ENABLED = InitInsertNotificationsEnabled();
const AutoPtr< ArrayOf<String> > CPrivacyPersistenceAdapter::DATABASE_FIELDS = InitDatabaseFields();

const String CPrivacyPersistenceAdapter::SETTING_ENABLED("enabled");
const String CPrivacyPersistenceAdapter::SETTING_NOTIFICATIONS_ENABLED("notifications_enabled");
const String CPrivacyPersistenceAdapter::SETTING_DB_VERSION("db_version");
const String CPrivacyPersistenceAdapter::VALUE_TRUE("1");
const String CPrivacyPersistenceAdapter::VALUE_FALSE("0");

RWLock CPrivacyPersistenceAdapter::sDbLock;

CPrivacyPersistenceAdapter::CPrivacyPersistenceAdapter()
{}

CPrivacyPersistenceAdapter::~CPrivacyPersistenceAdapter()
{}

ECode CPrivacyPersistenceAdapter::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    // create the database and settings directory if we have write
    // permission and they do not exist
    AutoPtr<IFile> path;
    Boolean isExist;
    CFile::New(String("/data/system/"), (IFile**)&path);
    path->Exists(&isExist);
    if (!isExist) {
        Boolean success;
        path->Mkdir(&success);
        Logger::D(TAG, "create /data/system %s", success? "succeeded" : "failed");
    }
    Boolean canWrite;
    path->CanWrite(&canWrite);

    // create the database and settings directory if we have write permission and they do not exist
    if (canWrite) {
        AutoPtr<IFile> file;
        CFile::New(DATABASE_FILE, (IFile**)&file);
        Boolean dExist;
        file->Exists(&dExist);

        file = NULL;
        CFile::New(SETTINGS_DIRECTORY, (IFile**)&file);
        Boolean sExist;
        file->Exists(&sExist);
        if (!dExist || !sExist) {
            RWLock::AutoWLock lock(sDbLock);
            ECode ec = NOERROR;
            if (!dExist) {
                ec = CreateDatabase();
            }
            if (SUCCEEDED(ec) && !sExist) {
                CreateSettingsDir();
            }
        }

        // upgrade if needed
        sDbVersion = GetDbVersion();
        if (sDbVersion < DATABASE_VERSION) {
//TODO: TO Complete
//           UpgradeDatabase();
        }
    }
    else {
        Logger::D(TAG, "/data/system/ has no write permission\n");
    }
    return NOERROR;
}

void CPrivacyPersistenceAdapter::UpgradeDatabase()
{
    if (sDbVersion < DATABASE_VERSION) {
        Logger::I(TAG, "PrivacyPersistenceAdapter:upgradeDatabase - upgrading DB from version %d to %d", sDbVersion, DATABASE_VERSION);

        AutoPtr<ISQLiteDatabase> db;

        switch (sDbVersion) {
        case 1:
        case 2:
        case 3:
//            try {
            {
                AutoLock lock(sDbAccessThreadsLock);
                sDbAccessThreads++;
            }
//            if (LOG_OPEN_AND_CLOSE) Log.d(TAG, "PrivacyPersistenceAdapter:upgradeDatabase: Increment DB access threads: now " + Integer.toString(sDbAccessThreads));
            db = GetDatabase();
            Boolean opened;
            if (db != NULL && (db->IsOpen(&opened), opened)) {
                RWLock::AutoWLock lock(sDbLock);
//                try {
                // check the db version again to make sure that another thread has not already done the upgrade
                // in the meantime
                if (sDbVersion < DATABASE_VERSION) {
                    Boolean purged;
                    AutoPtr<IFile> settingsDir;
                    AutoPtr< ArrayOf<IFile*> > sFiles;
                    db->BeginTransaction();
//                        try {
                    FAIL_GOTO(db->ExecSQL(String("DROP TABLE IF EXISTS ") + TABLE_VERSION + ";"), ERROR1);
                    FAIL_GOTO(db->ExecSQL(CREATE_TABLE_ALLOWED_CONTACTS), ERROR1);
                    FAIL_GOTO(db->ExecSQL(CREATE_TABLE_MAP), ERROR1);
                    FAIL_GOTO(db->ExecSQL(INSERT_VERSION), ERROR1);
                    FAIL_GOTO(db->ExecSQL(INSERT_ENABLED), ERROR1);
                    FAIL_GOTO(db->ExecSQL(INSERT_NOTIFICATIONS_ENABLED), ERROR1);
                    PurgeSettings(&purged);
                    // remove uid dirs from the settings directory
                    // TODO: improve handling so that if an exception happens while
                    //      this process is in progress, it doesn't leave the filesystem
                    //      entries in an invalid state

                    CFile::New(SETTINGS_DIRECTORY, (IFile**)&settingsDir);
                    settingsDir->ListFiles((ArrayOf<IFile*>**)&sFiles);
                    for (Int32 i = 0; i < sFiles->GetLength(); ++i) {
                        AutoPtr<IFile> packageDir = (*sFiles)[i];
                        AutoPtr< ArrayOf<IFile*> > pFiles;
                        packageDir->ListFiles((ArrayOf<IFile*>**)&pFiles);
                        for (Int32 j = 0; j < pFiles->GetLength(); ++j) {
                            AutoPtr<IFile> uidDir = (*pFiles)[i];
                            Boolean isDir;
                            uidDir->IsDirectory(&isDir);
                            if (isDir) {
                                AutoPtr< ArrayOf<IFile*> > settingsFiles;
                                uidDir->ListFiles((ArrayOf<IFile*>**)&settingsFiles);
                                // copy the first found (most likely
                                // the only one) one level up
                                if ((*settingsFiles)[0] != NULL) {
                                    String fName;
                                    (*settingsFiles)[0]->GetName(&fName);
                                    String packageDirString;
                                    packageDir->ToString(&packageDirString);
                                    String pathString = packageDirString + "/" + fName;

                                    AutoPtr<IFile> newPath;
                                    CFile::New(pathString, (IFile**)&newPath);
                                    Boolean deleted;
                                    newPath->Delete(&deleted);
                                    Boolean renamed;
                                    (*settingsFiles)[0]->RenameTo(newPath, &renamed);
                                    DeleteRecursive(uidDir);
                                }
                            }
                        }
                    }
                    db->SetTransactionSuccessful();
                    sDbVersion = DATABASE_VERSION;
//                        } finally {
ERROR1:
                    db->EndTransaction();
//                        }
                }
                else {
                    // The database has been upgraded elsewhere; end the db transaction
                    // and don't make any further changes
                }
            }
//        } catch (SQLException e) {
//            Log.e(TAG, "PrivacyPersistenceAdapter:upgradeDatabase: SQLException occurred performing database upgrade", e);
//        } finally {
            CloseIdleDatabase();
//        }
        break;

        case 4:
            // most current version, do nothing
            Logger::I(TAG, "PrivacyPersistenceAdapter:upgradeDatabase: Database is already at the most recent version");
            break;
        }
    }
}

Int32 CPrivacyPersistenceAdapter::GetDbVersion()
{
    String versionString;
    GetValue(SETTING_DB_VERSION, &versionString);
    if (versionString.IsNullOrEmpty()) {
        Logger::E(TAG, "PrivacyPersistenceAdapter:getDbVersion: getValue returned null; assuming version = 1");
        return 1;
    }
    else {
//        try {
        return StringUtils::ParseInt32(versionString);
//        } catch (Exception e) {
//            Log.e(TAG, "PrivacyPersistenceAdapter:getDbVersion: failed to parse database version; returning 1");
//            return 1;
//        }
    }
}

ECode CPrivacyPersistenceAdapter::GetValue(
    /* [in] */ const String& name,
    /* [out]*/ String* value)
{
    VALIDATE_NOT_NULL(value);

    String output;
    {
        AutoLock lock(sDbAccessThreadsLock);
        ++sDbAccessThreads;
    }
//    if (LOG_OPEN_AND_CLOSE) Log.d(TAG, "PrivacyPersistenceAdapter:getValue: Increment DB access threads: now " + Integer.toString(sDbAccessThreads));

    AutoPtr<ISQLiteDatabase> db = GetDatabase();
    Boolean opened;
    if (db == NULL || !(db->IsOpen(&opened),opened)) {
        Logger::E(TAG, "PrivacyPersistenceAdapter:getValue: Database not obtained while getting value for name: %s", name.string());
        *value = String(NULL);
        return NOERROR;
    }

    {
        RWLock::AutoRLock lock(sDbLock);
//    try {
        AutoPtr< ArrayOf<String> > vArray = ArrayOf<String>::Alloc(1);
        (*vArray)[0] = "value";
        AutoPtr< ArrayOf<String> > nArray = ArrayOf<String>::Alloc(1);
        (*nArray)[0] = name;
        AutoPtr<ICursor> c;
        Query(db, TABLE_MAP, vArray, String("name=?"), nArray,
                String(NULL), String(NULL), String(NULL), String(NULL), (ICursor**)&c);
        Int32 count;
        Boolean toFirst;
        if (c != NULL && (c->GetCount(&count), count) > 0 && (c->MoveToFirst(&toFirst), toFirst)) {
            Int32 index;
            c->GetColumnIndex(String("value"), &index);
            c->GetString(index, &output);
            c->Close();
        }
        else {
            Logger::W(TAG, "PrivacyPersistenceAdapter:getValue: Could not get value for name: %s", name.string());
        }
//    } catch (Exception e) {
//        Log.e(TAG, "PrivacyPersistenceAdapter:getValue: Exception occurred while getting value for name: " + name, e);
//    }
    }
//    } finally {
    CloseIdleDatabase();
//    }
    *value = output;
    return NOERROR;
}

ECode CPrivacyPersistenceAdapter::SetValue(
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [out]*/ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(TAG, "setValue - name %s value %s", name.string(), value.string());
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    AutoPtr<ICharSequence> cs = GetCharSequence(name);
    values->PutString(String("name"), cs);
    cs = GetCharSequence(value);
    values->PutString(String("value"), cs);

    Boolean success = FALSE;

//    try {
    {
        AutoLock lock(sDbAccessThreadsLock);
        ++sDbAccessThreads;
    }
//        if (LOG_OPEN_AND_CLOSE) Log.d(TAG, "PrivacyPersistenceAdapter:setValue: Increment DB access threads: now " + Integer.toString(sDbAccessThreads));
    AutoPtr<ISQLiteDatabase> db = GetDatabase();
    Boolean opened;
    if (db == NULL || !(db->IsOpen(&opened), opened)) {
        Logger::E(TAG, "PrivacyPersistenceAdapter:setValue: Database not obtained while setting value for name: %s", name.string());
        *result = FALSE;
        return NOERROR;
    }

        // updating the version is atomic, but we need to use a lock
        // to make sure we don't try to get/update the version while the DB is being created or upgraded
    {
        RWLock::AutoWLock lock(sDbLock);

        Int64 id;
        db->Replace(TABLE_MAP, String(NULL), values, &id);
        success = id != -1;
    }
//    } finally {
    CloseIdleDatabase();
//    }

    *result = success;
    return NOERROR;
}

ECode CPrivacyPersistenceAdapter::GetSettings(
    /* [in] */ const String& packageName,
    /* [out] */ IPrivacySettings** _privacySettings)
{
    VALIDATE_NOT_NULL(_privacySettings);

    AutoPtr<IPrivacySettings> privacySettings;

    if (packageName.IsNullOrEmpty()) {
//        throw new InvalidParameterException(
//                "PrivacyPersistenceAdapter:getSettings:insufficient application identifier - package name is required");
        return E_INVALID_ARGUMENT;
    }

    if (mUseCache) {
        AutoPtr<IInterface> cacheResult;
        HashMap<String, AutoPtr<IInterface> >::Iterator it = mSettingsCache.Find(packageName);
        if (it != mSettingsCache.End()) {
            cacheResult = it->mSecond;
        }
        if (cacheResult != NULL) {
            if (LOG_CACHE) Logger::D(TAG, "PrivacyPersistenceAdapter:getSettings: Cache hit for %s", packageName.string());
            //if the cached object is a stub, then it means that there is no privacy settings for that package, and null should be returned
            if (IPrivacySettings::Probe(cacheResult) != NULL) {
                if (LOG_CACHE) Logger::D(TAG, "PrivacyPersistenceAdapter:Cached result is not a stub:%s", packageName.string());
                *_privacySettings = IPrivacySettings::Probe(cacheResult);
                REFCOUNT_ADD(*_privacySettings);
                return NOERROR;
            }
            else {
                if (LOG_CACHE) Logger::D(TAG, "PrivacyPersistenceAdapter:Cached result is a stub, return null:%s", packageName.string());
                *_privacySettings = NULL;
                return NOERROR;
            }
        }
        else {
            if (LOG_CACHE) Logger::D(TAG, "PrivacyPersistenceAdapter:getSettings: Cache miss for %s", packageName.string());
        }
    }

//    try {
        // indicate that the DB is being read to prevent closing by other threads
    {
        AutoLock lock(sDbAccessThreadsLock);
        ++sDbAccessThreads;
    }
//        if (LOG_OPEN_AND_CLOSE) Log.d(TAG, "PrivacyPersistenceAdapter:getSettings: Increment DB access threads: now " + Integer.toString(sDbAccessThreads));
    AutoPtr<ISQLiteDatabase> db = GetDatabase();
//    } catch (SQLiteException e) {
//        Log.e(TAG, "getSettings - database could not be opened", e);
//        closeIdleDatabase();
//        throw e;
//    }

    {
        RWLock::AutoRLock lock(sDbLock);
//    try {
        AutoPtr< ArrayOf<String> > pArray = ArrayOf<String>::Alloc(1);
        (*pArray)[0] = packageName;
        AutoPtr<ICursor> cursor;
        Query(db, TABLE_SETTINGS, DATABASE_FIELDS, String("packageName=?"),
                pArray, String(NULL), String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);
        if (cursor != NULL) {
            Int32 count;
            cursor->GetCount(&count);
            if (count == 0) {
                // No settings are present: log that and do nothing
                Logger::D(TAG, "PrivacyPersistenceAdapter:getSettingsfound for package %s", packageName.string());
            }
            else {
                if (count > 1) {
                    Logger::W(TAG, "Multiple privacy settings found for package %s", packageName.string());
                }
                Boolean toFirst;
                cursor->MoveToFirst(&toFirst);
                if (toFirst) {
                    Int32 cid;
                    cursor->GetInt32(0, &cid);
                    // String cpkgName;
                    // cursor->GetString(1, &cpkgName);
                    Int32 cuid;
                    cursor->GetInt32(2, &cuid);
                    CPrivacySettings::New(cid, packageName, cuid, (IPrivacySettings**)&privacySettings);

                    Int16 deviceIdSetting;
                    cursor->GetInt16(3, &deviceIdSetting);
                    String deviceId;
                    cursor->GetString(4, &deviceId);
                    Int16 line1NumberSetting;
                    cursor->GetInt16(5, &line1NumberSetting);
                    String line1Number;
                    cursor->GetString(6, &line1Number);
                    Int16 locationGpsSetting;
                    cursor->GetInt16(7, &locationGpsSetting);
                    String locationGpsLat;
                    cursor->GetString(8, &locationGpsLat);
                    String locationGpsLon;
                    cursor->GetString(9, &locationGpsLon);
                    Int16 locationNetworkSetting;
                    cursor->GetInt16(10, &locationNetworkSetting);
                    String locationNetworkLat;
                    cursor->GetString(11, &locationNetworkLat);
                    String locationNetworkLon;
                    cursor->GetString(12, &locationNetworkLon);
                    Int16 networkInfoSetting;
                    cursor->GetInt16(13,&networkInfoSetting);
                    Int16 simInfoSetting;
                    cursor->GetInt16(14, &simInfoSetting);
                    Int16 simSerialNumberSetting;
                    cursor->GetInt16(15, &simSerialNumberSetting);
                    String simSerialNumber;
                    cursor->GetString(16, &simSerialNumber);
                    Int16 subscriberIdSetting;
                    cursor->GetInt16(17, &subscriberIdSetting);
                    String subscriberId;
                    cursor->GetString(18, &subscriberId);
                    Int16 accountsSetting;
                    cursor->GetInt16(19, &accountsSetting);
                    Int16 accountsAuthTokensSetting;
                    cursor->GetInt16(20, &accountsAuthTokensSetting);
                    Int16 outgoingCallsSetting;
                    cursor->GetInt16(21, &outgoingCallsSetting);
                    Int16 incomingCallsSetting;
                    cursor->GetInt16(22, &incomingCallsSetting);
                    Int16 contactsSetting;
                    cursor->GetInt16(23, &contactsSetting);
                    Int16 calendarSetting;
                    cursor->GetInt16(24, &calendarSetting);
                    Int16 mmsSetting;
                    cursor->GetInt16(25, &mmsSetting);
                    Int16 smsSetting;
                    cursor->GetInt16(26, &smsSetting);
                    Int16 callLogSetting;
                    cursor->GetInt16(27, &callLogSetting);
                    Int16 bookmarksSetting;
                    cursor->GetInt16(28, &bookmarksSetting);
                    Int16 systemLogsSetting;
                    cursor->GetInt16(29, &systemLogsSetting);
                    Int16 externalStorageSetting;
                    cursor->GetInt16(30, &externalStorageSetting);
                    Int16 cameraSetting;
                    cursor->GetInt16(31, &cameraSetting);
                    Int16 recordAudioSetting;
                    cursor->GetInt16(32, &recordAudioSetting);
                    Int16 notificationSetting;
                    cursor->GetInt16(33, &notificationSetting);
                    Int16 intentBootCompletedSetting;
                    cursor->GetInt16(34, &intentBootCompletedSetting);
                    Int16 smsSendSetting;
                    cursor->GetInt16(35, &smsSendSetting);
                    Int16 phoneCallSetting;
                    cursor->GetInt16(36, &phoneCallSetting);
                    Int16 ipTableProtectSetting;
                    cursor->GetInt16(37, &ipTableProtectSetting);
                    Int16 iccAccessSetting;
                    cursor->GetInt16(38, &iccAccessSetting);
                    Int16 addOnManagementSetting;
                    cursor->GetInt16(39, &addOnManagementSetting);
                    Int16 androidIdSetting;
                    cursor->GetInt16(40, &androidIdSetting);
                    String androidID;
                    cursor->GetString(41, &androidID);
                    Int16 wifiInfoSetting;
                    cursor->GetInt16(42, &wifiInfoSetting);
                    Int16 switchConnectivitySetting;
                    cursor->GetInt16(43, &switchConnectivitySetting);
                    Int16 sendMmsSetting;
                    cursor->GetInt16(44, &sendMmsSetting);
                    Int16 forceOnlineState;
                    cursor->GetInt16(45, &forceOnlineState);
                    Int16 switchWifiStateSetting;
                    cursor->GetInt16(46, &switchWifiStateSetting);

                    privacySettings->SetDeviceIdSetting((Byte)deviceIdSetting);
                    privacySettings->SetDeviceId(deviceId);
                    privacySettings->SetLine1NumberSetting((Byte)line1NumberSetting);
                    privacySettings->SetLine1Number(line1Number);
                    privacySettings->SetLocationGpsSetting((Byte)locationGpsSetting);
                    privacySettings->SetLocationGpsLat(locationGpsLat);
                    privacySettings->SetLocationGpsLon(locationGpsLon);
                    privacySettings->SetLocationNetworkSetting((Byte)locationNetworkSetting);
                    privacySettings->SetLocationNetworkLat(locationNetworkLat);
                    privacySettings->SetLocationNetworkLon(locationNetworkLon);
                    privacySettings->SetNetworkInfoSetting((Byte)networkInfoSetting);
                    privacySettings->SetSimInfoSetting(simInfoSetting);
                    privacySettings->SetSimSerialNumberSetting((Byte)simSerialNumberSetting);
                    privacySettings->SetSimSerialNumber(simSerialNumber);
                    privacySettings->SetSubscriberIdSetting((Byte)subscriberIdSetting);
                    privacySettings->SetSubscriberId(subscriberId);
                    privacySettings->SetAccountsSetting((Byte)accountsSetting);
                    privacySettings->SetAccountsAuthTokensSetting((Byte)accountsAuthTokensSetting);
                    privacySettings->SetOutgoingCallsSetting((Byte)outgoingCallsSetting);
                    privacySettings->SetIncomingCallsSetting((Byte)incomingCallsSetting);
                    privacySettings->SetContactsSetting((Byte)contactsSetting);
                    privacySettings->SetCalendarSetting((Byte)calendarSetting);
                    privacySettings->SetMmsSetting((Byte)mmsSetting);
                    privacySettings->SetSmsSetting((Byte)smsSetting);
                    privacySettings->SetCallLogSetting((Byte)callLogSetting);
                    privacySettings->SetBookmarksSetting((Byte)bookmarksSetting);
                    privacySettings->SetSystemLogsSetting((Byte)systemLogsSetting);
                    privacySettings->SetCameraSetting((Byte)cameraSetting);
                    privacySettings->SetRecordAudioSetting((Byte)recordAudioSetting);
                    privacySettings->SetNotificationSetting((Byte)notificationSetting);
                    privacySettings->SetIntentBootCompletedSetting((Byte)intentBootCompletedSetting);
                    privacySettings->SetAllowedContacts(NULL);
                    privacySettings->SetSmsSendSetting((Byte)smsSendSetting);
                    privacySettings->SetPhoneCallSetting((Byte)phoneCallSetting);
                    privacySettings->SetIpTableProtectSetting((Byte)ipTableProtectSetting);
                    privacySettings->SetIccAccessSetting((Byte)iccAccessSetting);
                    privacySettings->SetAddOnManagementSetting((Byte)addOnManagementSetting);
                    privacySettings->SetAndroidIdSetting((Byte)androidIdSetting);
                    privacySettings->SetAndroidID(androidID);
                    privacySettings->SetWifiInfoSetting((Byte)wifiInfoSetting);
                    privacySettings->SetSwitchConnectivitySetting((Byte)switchConnectivitySetting);
                    privacySettings->SetSendMmsSetting((Byte)sendMmsSetting);
                    privacySettings->SetForceOnlineState((Byte)forceOnlineState);
                    privacySettings->SetSwitchWifiStateSetting((Byte)switchWifiStateSetting);

                    // pengyang make sure cursor is close
                    if (cursor != NULL) cursor->Close();
                    // get allowed contacts IDs if necessary
                    AutoPtr< ArrayOf<String> > cArray = ArrayOf<String>::Alloc(1);
                    (*cArray)[0] = "contact_id";

                    Int32 pId;
                    privacySettings->GetId(&pId);
                    AutoPtr<IInteger32> integerId;
                    CInteger32::New(pId, (IInteger32**)&integerId);
                    String idString;
                    integerId->ToString(&idString);
                    AutoPtr< ArrayOf<String> > idArray = ArrayOf<String>::Alloc(1);
                    (*idArray)[0] = idString;

                    cursor = NULL;
                    Query(db, TABLE_ALLOWED_CONTACTS, cArray,
                            String("settings_id=?"),
                            idArray, String(NULL),
                            String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);
                    if (cursor != NULL && count > 0) {
                        AutoPtr< ArrayOf<Int32> > allowedContacts = ArrayOf<Int32>::Alloc(count);
                        Boolean hasNext;
                        while (cursor->MoveToNext(&hasNext), hasNext) {
                            Int32 position;
                            cursor->GetPosition(&position);
                            (*allowedContacts)[position] = cid;
                        }
                        privacySettings->SetAllowedContacts(allowedContacts);
                    }
                }
            }
        }

        if (mUseCache) {
            if (privacySettings != NULL) {
                mSettingsCache[packageName] = privacySettings->Probe(EIID_IInterface);
                if (LOG_CACHE) Logger::D(TAG, "PrivacyPersistenceAdapter:getSettings: Cache put for%s", packageName.string());
            }
            else {
                mSettingsCache[packageName] = new PrivacySettingsStub();
                if (LOG_CACHE) Logger::D(TAG, "PrivacyPersistenceAdapter:getSettings: Cache stub put for%s", packageName.string());
            }
        }
//    } catch (Exception e) {
//        Log.e(TAG, "getSettings - failed to get settings for package: " + packageName, e);
//    } finally {
        if (cursor != NULL) cursor->Close();
    }
    CloseIdleDatabase();
//    }

    *_privacySettings = privacySettings;
    REFCOUNT_ADD(*_privacySettings);
    return NOERROR;
}

ECode CPrivacyPersistenceAdapter::SaveSettings(
    /* [in] */ IPrivacySettings* s,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    String packageName;
    s->GetPackageName(&packageName);
    if (packageName.IsNullOrEmpty()) {
        Logger::E(TAG, "saveSettings - either package name is missing");
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    AutoPtr<ICharSequence> cs = GetCharSequence(packageName);
    values->PutString(String("packageName"), cs);
    // values.put("uid", uid);
    AutoPtr<IInteger32> duid;
    CInteger32::New(DUMMY_UID, (IInteger32**)&duid);
    values->PutInt32(String("uid"), duid);

    Byte setting;
    s->GetDeviceIdSetting(&setting);
    AutoPtr<IByte> b = GetIByte(setting);
    values->PutByte(String("deviceIdSetting"), b);
    String deviceId;
    s->GetDeviceId(&deviceId);
    cs = GetCharSequence(deviceId);
    values->PutString(String("deviceId"), cs);

    Byte line1NumberSetting;
    s->GetLine1NumberSetting(&line1NumberSetting);
    b = GetIByte(line1NumberSetting);
    values->PutByte(String("line1NumberSetting"), b);
    String line1Number;
    s->GetLine1Number(&line1Number);
    cs = GetCharSequence(line1Number);
    values->PutString(String("line1Number"), cs);

    Byte locationGpsSetting;
    s->GetLocationGpsSetting(&locationGpsSetting);
    b = GetIByte(locationGpsSetting);
    values->PutByte(String("locationGpsSetting"), b);

    String locationGpsLat;
    s->GetLocationGpsLat(&locationGpsLat);
    cs = GetCharSequence(locationGpsLat);
    values->PutString(String("locationGpsLat"), cs);
    String locationGpsLon;
    s->GetLocationGpsLon(&locationGpsLon);
    cs = GetCharSequence(locationGpsLon);
    values->PutString(String("locationGpsLon"), cs);

    Byte locationNetworkSetting;
    s->GetLocationNetworkSetting(&locationNetworkSetting);
    b = GetIByte(locationNetworkSetting);
    values->PutByte(String("locationNetworkSetting"), b);
    String locationNetworkLat;
    s->GetLocationNetworkLat(&locationNetworkLat);
    cs = GetCharSequence(locationNetworkLat);
    values->PutString(String("locationNetworkLat"), cs);
    String locationNetworkLon;
    s->GetLocationNetworkLon(&locationNetworkLon);
    cs = GetCharSequence(locationNetworkLon);
    values->PutString(String("locationNetworkLon"), cs);

    Byte networkInfoSetting;
    s->GetNetworkInfoSetting(&networkInfoSetting);
    b = GetIByte(networkInfoSetting);
    values->PutByte(String("networkInfoSetting"), b);
    Byte simInfoSetting;
    s->GetSimInfoSetting(&simInfoSetting);
    b = GetIByte(simInfoSetting);
    values->PutByte(String("simInfoSetting"), b);

    Byte simSerialNumberSetting;
    s->GetSimSerialNumberSetting(&simSerialNumberSetting);
    b = GetIByte(simSerialNumberSetting);
    values->PutByte(String("simSerialNumberSetting"), b);
    String simSerialNumber;
    s->GetSimSerialNumber(&simSerialNumber);
    cs = GetCharSequence(simSerialNumber);
    values->PutString(String("simSerialNumber"), cs);
    Byte subscriberIdSetting;
    s->GetSubscriberIdSetting(&subscriberIdSetting);
    b = GetIByte(subscriberIdSetting);
    values->PutByte(String("subscriberIdSetting"), b);
    String subscriberId;
    s->GetSubscriberId(&subscriberId);
    cs = GetCharSequence(subscriberId);
    values->PutString(String("subscriberId"), cs);

    Byte accountsSetting;
    s->GetAccountsSetting(&accountsSetting);
    b = GetIByte(accountsSetting);
    values->PutByte(String("accountsSetting"), b);
    Byte accountsAuthTokensSetting;
    s->GetAccountsAuthTokensSetting(&accountsAuthTokensSetting);
    b = GetIByte(accountsAuthTokensSetting);
    values->PutByte(String("accountsAuthTokensSetting"), b);
    Byte outgoingCallsSetting;
    s->GetOutgoingCallsSetting(&outgoingCallsSetting);
    b = GetIByte(outgoingCallsSetting);
    values->PutByte(String("outgoingCallsSetting"), b);
    Byte incomingCallsSetting;
    s->GetIncomingCallsSetting(&incomingCallsSetting);
    b = GetIByte(incomingCallsSetting);
    values->PutByte(String("incomingCallsSetting"), b);

    Byte contactsSetting;
    s->GetContactsSetting(&contactsSetting);
    b = GetIByte(contactsSetting);
    values->PutByte(String("contactsSetting"), b);
    Byte calendarSetting;
    s->GetCalendarSetting(&calendarSetting);
    b = GetIByte(calendarSetting);
    values->PutByte(String("calendarSetting"), b);
    Byte mmsSetting;
    s->GetMmsSetting(&mmsSetting);
    b = GetIByte(mmsSetting);
    values->PutByte(String("mmsSetting"), b);
    Byte smsSetting;
    s->GetSmsSetting(&smsSetting);
    b = GetIByte(smsSetting);
    values->PutByte(String("smsSetting"), b);
    Byte callLogSetting;
    s->GetCallLogSetting(&callLogSetting);
    b = GetIByte(callLogSetting);
    values->PutByte(String("callLogSetting"), b);
    Byte bookmarksSetting;
    s->GetBookmarksSetting(&bookmarksSetting);
    b = GetIByte(bookmarksSetting);
    values->PutByte(String("bookmarksSetting"), b);
    Byte systemLogsSetting;
    s->GetSystemLogsSetting(&systemLogsSetting);
    b = GetIByte(systemLogsSetting);
    values->PutByte(String("systemLogsSetting"), b);
    Byte notificationSetting;
    s->GetNotificationSetting(&notificationSetting);
    b = GetIByte(notificationSetting);
    values->PutByte(String("notificationSetting"), b);
    Byte intentBootCompletedSetting;
    s->GetIntentBootCompletedSetting(&intentBootCompletedSetting);
    b = GetIByte(intentBootCompletedSetting);
    values->PutByte(String("intentBootCompletedSetting"), b);

    // values.put("externalStorageSetting", s.getExternalStorageSetting());
    Byte cameraSetting;
    s->GetCameraSetting(&cameraSetting);
    b = GetIByte(cameraSetting);
    values->PutByte(String("cameraSetting"), b);
    Byte recordAudioSetting;
    s->GetRecordAudioSetting(&recordAudioSetting);
    b = GetIByte(recordAudioSetting);
    values->PutByte(String("recordAudioSetting"), b);
    Byte smsSendSetting;
    s->GetSmsSendSetting(&smsSendSetting);
    b = GetIByte(smsSendSetting);
    values->PutByte(String("smsSendSetting"), b);
    Byte phoneCallSetting;
    s->GetPhoneCallSetting(&phoneCallSetting);
    b = GetIByte(phoneCallSetting);
    values->PutByte(String("phoneCallSetting"), b);
    Byte ipTableProtectSetting;
    s->GetIpTableProtectSetting(&ipTableProtectSetting);
    b = GetIByte(ipTableProtectSetting);
    values->PutByte(String("ipTableProtectSetting"), b);
    Byte iccAccessSetting;
    s->GetIccAccessSetting(&iccAccessSetting);
    b = GetIByte(iccAccessSetting);
    values->PutByte(String("iccAccessSetting"), b);
    Byte addOnManagementSetting;
    s->GetAddOnManagementSetting(&addOnManagementSetting);
    b = GetIByte(addOnManagementSetting);
    values->PutByte(String("addOnManagementSetting"), b);
    Byte androidIdSetting;
    s->GetAndroidIdSetting(&androidIdSetting);
    b = GetIByte(androidIdSetting);
    values->PutByte(String("androidIdSetting"), b);
    String androidId;
    s->GetAndroidID(&androidId);
    cs = GetCharSequence(androidId);
    values->PutString(String("androidId"), cs);
    Byte wifiInfoSetting;
    s->GetWifiInfoSetting(&wifiInfoSetting);
    b = GetIByte(wifiInfoSetting);
    values->PutByte(String("wifiInfoSetting"), b);
    Byte switchConnectivitySetting;
    s->GetSwitchConnectivitySetting(&switchConnectivitySetting);
    b = GetIByte(switchConnectivitySetting);
    values->PutByte(String("switchConnectivitySetting"), b);
    Byte sendMmsSetting;
    s->GetSendMmsSetting(&sendMmsSetting);
    b = GetIByte(sendMmsSetting);
    values->PutByte(String("sendMmsSetting"), b);
    Byte forceOnlineState;
    s->GetForceOnlineState(&forceOnlineState);
    b = GetIByte(forceOnlineState);
    values->PutByte(String("forceOnlineState"), b);
    Byte switchWifiStateSetting;
    s->GetSwitchWifiStateSetting(&switchWifiStateSetting);
    b = GetIByte(switchWifiStateSetting);
    values->PutByte(String("switchWifiStateSetting"), b);

//    try {
    {
        AutoLock lock(sDbAccessThreadsLock);
        ++sDbAccessThreads;
    }
//        if (LOG_OPEN_AND_CLOSE) Log.d(TAG, "PrivacyPersistenceAdapter:saveSettings: Increment DB access threads: now " + Integer.toString(sDbAccessThreads));
    AutoPtr<ICursor> cursor;
    AutoPtr<ISQLiteDatabase> db = GetDatabase();
    Boolean opened;
    if (db != NULL && (db->IsOpen(&opened), opened)) {
        RWLock::AutoWLock lock(sDbLock);
//            try {
        db->BeginTransaction();
//                try {
                // save settings to the DB
        Int32 id;
        s->GetId(&id);
        if (id != -1) { // existing entry -> update
            AutoPtr<IInteger32> integerId;
            CInteger32::New(id, (IInteger32**)&integerId);
            String stringId;
            integerId->ToString(&stringId);

            AutoPtr< ArrayOf<String> > idArray = ArrayOf<String>::Alloc(1);
            (*idArray)[0] = stringId;
            Int32 updateNum = 0;
            db->Update(TABLE_SETTINGS, values, String("_id=?"), idArray, &updateNum);
            if (updateNum < 1) {
//                            throw new Exception("saveSettings - failed to update database entry");
                return E_SQLITE_DATABASE_CORRUPT_EXCEPTION;
            }

            Int32 dlNum;
            db->Delete(TABLE_ALLOWED_CONTACTS, String("settings_id=?"), idArray, &dlNum);
            AutoPtr< ArrayOf<Int32> > allowedContacts;
            s->GetAllowedContacts((ArrayOf<Int32>**)&allowedContacts);
            if (allowedContacts != NULL) {
                AutoPtr<IContentValues> contactsValues;
                CContentValues::New((IContentValues**)&contactsValues);
                for (Int32 i = 0; i < allowedContacts->GetLength(); i++) {
                    contactsValues->PutInt32(String("settings_id"), integerId);
                    AutoPtr<IInteger32> value;
                    CInteger32::New((*allowedContacts)[i], (IInteger32**)&value);
                    contactsValues->PutInt32(String("contact_id"), value);
                    Int64 insertNum;
                    db->Insert(TABLE_ALLOWED_CONTACTS, String(NULL), contactsValues, &insertNum);
                    if (insertNum == -1) {
//                                    throw new Exception(
//                                            "PrivacyPersistenceAdapter:saveSettings: failed to update database entry (contacts)");
                        return E_SQLITE_DATABASE_CORRUPT_EXCEPTION;
                    }
                }
            }
        }
        else { // new entry -> insert if no duplicates exist
            // Log.d(TAG,
            // "saveSettings - new entry; verifying if duplicates exist");
            AutoPtr< ArrayOf<String> > idArray = ArrayOf<String>::Alloc(1);
            (*idArray)[0] = String("_id");
            String pkgName;
            s->GetPackageName(&pkgName);
            AutoPtr< ArrayOf<String> > nArray = ArrayOf<String>::Alloc(1);
            (*nArray)[0] = pkgName;
            cursor = NULL;
            db->Query(TABLE_SETTINGS, idArray, String("packageName=?"),
                    nArray, String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);
            if (cursor != NULL) {
                Int32 count;
                cursor->GetCount(&count);
                if (count == 1) { // exactly one entry
                    // exists -> update
                    // Log.d(TAG, "saveSettings - updating existing entry");
                    Int32 updateNum;
                    db->Update(TABLE_SETTINGS, values, String("packageName=?"), nArray, &updateNum);
                    Boolean toFirst;
                    cursor->MoveToFirst(&toFirst);
                    if (updateNum < 1) {
//                                    throw new Exception("saveSettings - failed to update database entry");
                        return E_SQLITE_DATABASE_CORRUPT_EXCEPTION;
                    }
                    if (toFirst) {
                        Int32 idAlt;
                        cursor->GetInt32(0, &idAlt); // id of the found
                        // duplicate entry
                        AutoPtr<IInteger32> integerIdAlt;
                        CInteger32::New(idAlt, (IInteger32**)&integerIdAlt);
                        String sIdAlt;
                        integerIdAlt->ToString(&sIdAlt);
                        AutoPtr< ArrayOf<String> > aArray = ArrayOf<String>::Alloc(1);
                        (*aArray)[0] = sIdAlt;
                        Int32 dNum;
                        db->Delete(TABLE_ALLOWED_CONTACTS, String("settings_id=?"), aArray, &dNum);
                        AutoPtr< ArrayOf<Int32> > allowedContacts;
                        s->GetAllowedContacts((ArrayOf<Int32>**)&allowedContacts);
                        if (allowedContacts != NULL) {
                            AutoPtr<IContentValues> contactsValues;
                            CContentValues::New((IContentValues**)&contactsValues);
                            for (Int32 i = 0; i < allowedContacts->GetLength(); i++) {
                                contactsValues->PutInt32(String("settings_id"), integerIdAlt);
                                AutoPtr<IInteger32> value;
                                CInteger32::New((*allowedContacts)[i], (IInteger32**)&value);
                                contactsValues->PutInt32(String("contact_id"), value);
                                Int64 iNum;
                                db->Insert(TABLE_ALLOWED_CONTACTS, String(NULL), contactsValues, &iNum);
                                if (iNum == -1) {
//                                                throw new Exception(
//                                                        "saveSettings - failed to update database entry (contacts)");
                                    return E_SQLITE_DATABASE_CORRUPT_EXCEPTION;
                                }
                            }
                        }
                    }
                }
                else if (count == 0) { // no entries -> insert
                    // Log.d(TAG, "saveSettings - inserting new entry");
                    Int64 rowId = -1;
                    db->Insert(TABLE_SETTINGS, String(NULL), values, &rowId);
                    if (rowId == -1) {
//                                    throw new Exception(
//                                            "PrivacyPersistenceAdapter:saveSettings - failed to insert new record into DB");
                        return E_SQLITE_DATABASE_CORRUPT_EXCEPTION;
                    }

                    AutoPtr<IInteger64> integerRowId;
                    CInteger64::New(rowId, (IInteger64**)&integerRowId);
                    String rowIdString;
                    integerRowId->ToString(&rowIdString);
                    AutoPtr< ArrayOf<String> > ridArray = ArrayOf<String>::Alloc(1);
                    (*ridArray)[0] = rowIdString;
                    Int32 dNum;
                    db->Delete(TABLE_ALLOWED_CONTACTS, String("settings_id=?"), ridArray, &dNum);
                    AutoPtr< ArrayOf<Int32> > allowedContacts;
                    s->GetAllowedContacts((ArrayOf<Int32>**)&allowedContacts);
                    if (allowedContacts != NULL) {
                        AutoPtr<IContentValues> contactsValues;
                        CContentValues::New((IContentValues**)&contactsValues);
                        for (Int32 i = 0; i < allowedContacts->GetLength(); i++) {
                            contactsValues->PutInt64(String("settings_id"), integerRowId);
                            AutoPtr<IInteger32> value;
                            CInteger32::New((*allowedContacts)[i], (IInteger32**)&value);
                            contactsValues->PutInt32(String("contact_id"), value);
                            Int64 iNum;
                            db->Insert(TABLE_ALLOWED_CONTACTS, String(NULL), contactsValues, &iNum);
                            if (iNum == -1) {
//                                            throw new Exception(
//                                                    "PrivacyPersistenceAdapter:saveSettings:failed to update database entry (contacts)");
                                return E_SQLITE_DATABASE_CORRUPT_EXCEPTION;
                            }
                        }
                    }
                }
                else {
                    // something went totally wrong and there are
                    // multiple entries for same identifier
//                                throw new Exception("PrivacyPersistenceAdapter:saveSettings:duplicate entries in the privacy.db");
                    return E_SQLITE_DATABASE_CORRUPT_EXCEPTION;
                }
            }
            else {
                // jump to catch block to avoid marking transaction as
                // successful
//                            throw new Exception("PrivacyPersistenceAdapter:saveSettings:cursor is null, database access failed");
                return E_SQLITE_DATABASE_CORRUPT_EXCEPTION;
            }
        }

        // save settings to plain text file (for access from core libraries)
        if (!WriteExternalSettings(String("systemLogsSetting"), packageName, s)) {
//                        throw new Exception("PrivacyPersistenceAdapter:saveSettings:failed to write systemLogsSettings file");
            return E_RUNTIME_EXCEPTION;
        }

        if (!WriteExternalSettings(String("ipTableProtectSetting"), packageName, s)) {
//                        throw new Exception("PrivacyPersistenceAdapter:saveSettings:failed to write ipTableProtectSetting file");
            return E_RUNTIME_EXCEPTION;
        }

        // mark DB transaction successful (commit the changes)
        db->SetTransactionSuccessful();

        if (mUseCache) {
            //TODO: determine where this should actually go (i.e. should we delete from cache even if we fail to save the settings?)
            mSettingsCache.Erase(packageName);
            if (LOG_CACHE) Logger::D(TAG, "PrivacyPersistenceAdapter:deleteSettings: Cache remove for%s", packageName.string());
        }
//                } finally {
        db->EndTransaction(); // we want to transition from set transaction successful to end as fast as possible to avoid errors (see the Android docs)

        if (cursor != NULL) {
            cursor->Close();
        }
        *result = TRUE;
    }
//    } catch (Exception e) {
//        Log.e(TAG, "PrivacyPersistenceAdapter:saveSettings: saving for " + packageName + " failed", e);
//    } finally {
    CloseIdleDatabase();
//    }
    return NOERROR;
}

Boolean CPrivacyPersistenceAdapter::WriteExternalSettings(
    /* [in] */ const String& settingsName,
    /* [in] */ const String& packageName,
    /* [in] */ IPrivacySettings* s)
{
    // save settings to plain text file (for access from core libraries)
    AutoPtr<IFile> settingsPackageDir;
    String fpath("/data/system/privacy/");
    fpath += packageName;
    fpath += "/";
    CFile::New(fpath, (IFile**)&settingsPackageDir);

    AutoPtr<IFile> systemLogsSettingFile;
    String spath("/data/system/privacy/");
    spath += packageName;
    spath += "/";
    spath += "/";
    spath += settingsName;
    CFile::New(spath, (IFile**)&systemLogsSettingFile);

    {
        RWLock::AutoWLock lock(sDbLock);
//    try {
        Boolean maked;
        settingsPackageDir->Mkdirs(&maked);
        Boolean readSuccess;
        settingsPackageDir->SetReadable(TRUE, FALSE, &readSuccess);
        Boolean exeSuccess;
        settingsPackageDir->SetExecutable(TRUE, FALSE, &exeSuccess);
        // create the setting files and make them readable
        Boolean created;
        systemLogsSettingFile->CreateNewFile(&created);
        Boolean sReadable;
        systemLogsSettingFile->SetReadable(TRUE, FALSE, &sReadable);
        // write settings to files
        // Log.d(TAG, "saveSettings - writing to file");
        AutoPtr<IFileOutputStream> fStream;
        CFileOutputStream::New(systemLogsSettingFile, (IFileOutputStream**)&fStream);
        AutoPtr<IOutputStreamWriter> writer;
        COutputStreamWriter::New(fStream, (IOutputStreamWriter**)&writer);
        // now decide which feature of setting we have to save
        if (settingsName.Equals("systemLogsSetting")) {
            Byte slSetting;
            s->GetSystemLogsSetting(&slSetting);
            AutoPtr<IByte> slSettingIB;
            CByte::New(slSetting, (IByte**)&slSettingIB);
            String slSettingString;
            slSettingIB->ToString(&slSettingString);
            slSettingString += "";
            Logger::E(TAG, "COutputStreamWriter::AppendCharSequence() has problem.");
            //writer->AppendCharSequence(GetCharSequence(slSettingString)); // Mike Wu
        }
        else if (settingsName.Equals("ipTableProtectSetting")) {
            Byte ipTPSetting;
            s->GetIpTableProtectSetting(&ipTPSetting);
            AutoPtr<IByte> ipTPSettingIB;
            CByte::New(ipTPSetting, (IByte**)&ipTPSettingIB);
            String ipTPSettingString;
            ipTPSettingIB->ToString(&ipTPSettingString);
            ipTPSettingString += "";
            Logger::E(TAG, "COutputStreamWriter::AppendCharSequence() has problem.");
            //writer->AppendCharSequence(GetCharSequence(ipTPSettingString));  // Mike Wu
        }
//TODO:
//        writer->Flush();
//        writer->Close();
//    } catch (IOException e) {
//        // jump to catch block to avoid marking transaction as successful
//        throw new Exception("saveSettings - could not write settings to file", e);
//    }
    }
    return TRUE;
}

ECode CPrivacyPersistenceAdapter::DeleteSettings(
    /* [in] */const String& packageName,
    /* [out] */Boolean* pResult)
{
    VALIDATE_NOT_NULL(pResult);
    Boolean result = TRUE;

//    try {
    {
        AutoLock lock(sDbAccessThreadsLock);
        ++sDbAccessThreads;
    }
//        if (LOG_OPEN_AND_CLOSE) Log.d(TAG, "PrivacyPersistenceAdapter:deleteSettings: Increment DB access threads: now " + Integer.toString(sDbAccessThreads));
    AutoPtr<ISQLiteDatabase> db = GetDatabase();

    {
        RWLock::AutoWLock lock(sDbLock);
//        try {
        db->BeginTransaction();
                // make sure this ends up in a consistent state
    //            try {
                    // try deleting contacts allowed entries; do not fail if deletion
                    // not possible
                    // TODO: restructure this into a more efficient query (ideally a
                    // single query without a cursor)
        AutoPtr< ArrayOf<String> > idArray = ArrayOf<String>::Alloc(1);
        (*idArray)[0] = String("_id");
        AutoPtr< ArrayOf<String> > nArray = ArrayOf<String>::Alloc(1);
        (*nArray)[0] = packageName;
        AutoPtr<ICursor> c;
        db->Query(TABLE_SETTINGS, idArray, String("packageName=?"),
                nArray, String(NULL), String(NULL), String(NULL), (ICursor**)&c);

        Int32 count;
        Boolean toFirst;
        if (c != NULL && (c->GetCount(&count), count) > 0 && (c->MoveToFirst(&toFirst), toFirst)) {
            Int32 id;
            c->GetInt32(0, &id);
            AutoPtr<IInteger32> integerId;
            CInteger32::New(id, (IInteger32**)&integerId);
            String idString;
            integerId->ToString(&idString);
            AutoPtr< ArrayOf<String> > dArray = ArrayOf<String>::Alloc(1);
            (*dArray)[0] = idString;
            Int32 dNum;
            db->Delete(TABLE_ALLOWED_CONTACTS, String("settings_id=?"),
                    dArray, &dNum);
            c->Close();
        }
        else {
            Logger::E(TAG, "deleteSettings - database entry for %s not found", packageName.string());
        }
        AutoPtr< ArrayOf<String> > pArray = ArrayOf<String>::Alloc(1);
        (*pArray)[0] = packageName;
        Int32 dIndex;
        db->Delete(TABLE_SETTINGS, String("packageName=?"), pArray, &dIndex);
        if (dIndex == 0) {
            Logger::E(TAG, "deleteSettings - database entry for %s not found", packageName.string());
        }

        // delete settings from plain text file (for access from core
        // libraries)
        String sDir("/data/system/privacy/");
        sDir += packageName;
        sDir += "/";
        AutoPtr<IFile> settingsPackageDir;
        CFile::New(sDir, (IFile**)&settingsPackageDir);

        String sysDir("/data/system/privacy/");
        sysDir += packageName;
        sysDir += "/systemLogsSetting";
        AutoPtr<IFile> systemLogsSettingFile;
        CFile::New(sysDir, (IFile**)&systemLogsSettingFile);

        // delete the setting files
        Boolean deleted;
        systemLogsSettingFile->Delete(&deleted);
        // delete the parent directories
        AutoPtr< ArrayOf<String> > files;
        settingsPackageDir->List((ArrayOf<String>**)&files);
        if (files == NULL || files->GetLength() == 0) {
            settingsPackageDir->Delete(&deleted);
        }

        db->SetTransactionSuccessful();

        if (mUseCache) {
            //TODO: determine where this should actually go (i.e. should we delete from cache even if we fail to delete the settings?)
            mSettingsCache.Erase(packageName);
            if (LOG_CACHE) Logger::D(TAG, "PrivacyPersistenceAdapter:deleteSettings: Cache remove for%s", packageName.string());
        }
    //            } finally {
        db->EndTransaction();
//            }
    }
//    } catch (SQLiteException e) {
//        result = false;
//        Log.e(TAG, "PrivacyPersistenceAdapter:deleteSettings: failed to open the database, or open a transaction", e);
//    } catch (Exception e) {
//        result = false;
//        Log.e(TAG, "PrivacyPersistenceAdapter:deleteSettings - could not delete settings", e);
//    } finally {
    CloseIdleDatabase();
//    }

    *pResult = result;
    return NOERROR;
}

ECode CPrivacyPersistenceAdapter::Query(
    /* [in] */ ISQLiteDatabase* db_,
    /* [in] */ const String& table,
    /* [in] */ ArrayOf<String>* columns,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& orderBy,
    /* [in] */ const String& limit,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    *cursor = NULL;
    VALIDATE_NOT_NULL(db_);

    AutoPtr<ISQLiteDatabase> db = db_;
    // make sure getting settings does not fail because of
    // IllegalStateException (db already closed)
    AutoPtr<ICursor> c;
    ECode ec = NOERROR;
    Boolean opened;
    for (Int32 i = 0; i < RETRY_QUERY_COUNT; i++) {
        if (c != NULL) {
            c->Close();
            c = NULL;
        }
        ec = db->Query(table, columns, selection, selectionArgs,
            groupBy, having, orderBy, limit, (ICursor**)&c);
        if (FAILED(ec)) {
            if (db != NULL && (db->IsOpen(&opened), opened)) {
                db->Close();
            }
            db = GetDatabase();
        }
        else {
            break;
        }
    }

    if (FAILED(ec)) {
//        throw new Exception("query - failed to execute query on the DB");
        return E_RUNTIME_EXCEPTION;
    }

    *cursor = c;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode CPrivacyPersistenceAdapter::RawQuery(
    /* [in] */ ISQLiteDatabase* db_,
    /* [in] */ const String& sql,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    *cursor = NULL;
    VALIDATE_NOT_NULL(db_);

    AutoPtr<ISQLiteDatabase> db = db_;
    // make sure getting settings does not fail because of
    // IllegalStateException (db already closed)
    AutoPtr<ICursor> c;
    Boolean opened;
    ECode ec = NOERROR;
    for (Int32 i = 0; i < RETRY_QUERY_COUNT; i++) {
        if (c != NULL) {
            c->Close();
            c = NULL;
        }

        ec = db->RawQuery(sql, NULL, (ICursor**)&c);
        if (FAILED(ec)) {
            if (db != NULL && (db->IsOpen(&opened), opened)) {
                db->Close();
                db = NULL;
            }
            db = GetDatabase();
       }
       else {
            break;
       }
    }

    if (FAILED(ec)) {
//        throw new Exception("query - failed to execute query on the DB");
        return E_RUNTIME_EXCEPTION;
    }

    *cursor = c;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode CPrivacyPersistenceAdapter::PurgeSettings(
    /* [out] */ Boolean * pResult)
{
    VALIDATE_NOT_NULL(pResult);
    Boolean result = TRUE;

    // get installed apps
    HashSet<String> apps;
    AutoPtr<IPackageManager> pMan;
    mContext->GetPackageManager((IPackageManager**)&pMan);
    AutoPtr<IObjectContainer> installedApps;
    pMan->GetInstalledApplications(0, (IObjectContainer**)&installedApps);
    AutoPtr<IObjectEnumerator> appsEnumerator;
    installedApps->GetObjectEnumerator((IObjectEnumerator**)&appsEnumerator);
    Boolean hasNext = FALSE;
    while (appsEnumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        appsEnumerator->Current((IInterface**)&obj);
        AutoPtr<IApplicationInfo> appInfo = IApplicationInfo::Probe(obj);
        String pkgName;
        appInfo->GetPackageName(&pkgName);
        apps.Insert(pkgName);
    }

//    try {
    {
        AutoLock lock(sDbAccessThreadsLock);
        ++sDbAccessThreads;
    }
//        if (LOG_OPEN_AND_CLOSE) Log.d(TAG, "PrivacyPersistenceAdapter:purgeSettings: Increment DB access threads: now " + Integer.toString(sDbAccessThreads));

        // delete obsolete entries from DB and update outdated entries
    AutoPtr<ISQLiteDatabase> db = GetDatabase();
    if (db == NULL) {
        Logger::E(TAG, "PrivacyPersistenceAdapter:purgeSettings: db could not be obtained");
        *pResult = FALSE;
        return NOERROR;
    }

    {
        RWLock::AutoWLock lock(sDbLock);
//        try {
        db->BeginTransaction();
    //            try {
        AutoPtr< ArrayOf<String> > pArray = ArrayOf<String>::Alloc(1);
        (*pArray)[0] = String("packageName");
        AutoPtr<ICursor> cursor;
        Query(db, TABLE_SETTINGS, pArray, String(NULL), (ArrayOf<String>*)NULL, String(NULL), String(NULL),
                String(NULL), String(NULL), (ICursor**)&cursor);
        Int32 ccount;
        Boolean toFirst;
        if (cursor != NULL && (cursor->GetCount(&ccount), ccount) > 0 && (cursor->MoveToFirst(&toFirst), toFirst)) {
            Boolean moveNext;
            do {
                String packageName;
                cursor->GetString(0, &packageName);

                if (apps.Find(packageName) == apps.End()) {
                    AutoPtr< ArrayOf<String> > pArray = ArrayOf<String>::Alloc(1);
                    (*pArray)[0] = packageName;
                    Int32 dNum;
                    db->Delete(TABLE_SETTINGS, String("packageName = ?"), pArray, &dNum);
                }
            } while (cursor->MoveToNext(&moveNext), moveNext);
        }
        // delete obsolete settings directories
        AutoPtr<IFile> settingsDir;
        CFile::New(SETTINGS_DIRECTORY, (IFile**)&settingsDir);
        AutoPtr< ArrayOf<IFile*> > sFiles;
        settingsDir->ListFiles((ArrayOf<IFile*>**)&sFiles);
        for (Int32 i = 0; i< sFiles->GetLength(); ++i) {
            AutoPtr<IFile> packageDir = (*sFiles)[i];
            String packageName;
            packageDir->GetName(&packageName);

            if (apps.Find(packageName) == apps.End()) { // remove package dir if no such
               // app installed
               DeleteRecursive(packageDir);
            }
        }
        db->SetTransactionSuccessful();
    //            } finally {
        if (cursor != NULL) {
            cursor->Close();
        }
        db->EndTransaction();
//            }
    }
//    } catch (Exception e) {
//        Log.e(TAG, "PrivacyPersistenceAdapter:purgeSettings - purging DB failed", e);
//        result = false;
//    } finally {
    CloseIdleDatabase();
//    }

    *pResult = result;
    return NOERROR;
}

void CPrivacyPersistenceAdapter::DeleteRecursive(
    /* [in] */ IFile* fileOrDirectory)
{
    Boolean isDir;
    fileOrDirectory->IsDirectory(&isDir);
    if (isDir) {
        AutoPtr< ArrayOf<IFile*> > files;
        fileOrDirectory->ListFiles((ArrayOf<IFile*>**)&files);
        for (Int32 i = 0; i < files->GetLength(); ++i) {
            AutoPtr<IFile> child = (*files)[i];
            DeleteRecursive(child);
        }
    }
    Boolean deleted;
    fileOrDirectory->Delete(&deleted);
}

ECode CPrivacyPersistenceAdapter::CreateDatabase()
{
    Logger::I(TAG, "createDatabase - creating privacy database file");
//    try {
    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(SQLiteDatabase::OpenDatabase(DATABASE_FILE, NULL,
            ISQLiteDatabase::OPEN_READWRITE | ISQLiteDatabase::CREATE_IF_NECESSARY, (ISQLiteDatabase**)&db));
    Logger::I(TAG, "createDatabase - creating privacy database");
    if (db != NULL) {
        db->ExecSQL(CREATE_TABLE_SETTINGS);
        db->ExecSQL(CREATE_TABLE_ALLOWED_CONTACTS);
        db->ExecSQL(CREATE_TABLE_MAP);
        db->ExecSQL(INSERT_VERSION);
        db->ExecSQL(INSERT_ENABLED);
        db->ExecSQL(INSERT_NOTIFICATIONS_ENABLED);
        // Log.d(TAG, "createDatabase - closing connection to privacy.db");
        Boolean opened;
        if (db->IsOpen(&opened), opened) {
            db->Close();
        }
    }
    else {
        return E_SQLITE_DATABASE_CORRUPT_EXCEPTION;
    }
//    } catch (SQLException e) {
//        Log.e(TAG, "createDatabase - failed to create privacy database", e);
//    }
    return NOERROR;
}

void CPrivacyPersistenceAdapter::CreateSettingsDir()
{
    // create settings directory (for settings accessed from core libraries)
    AutoPtr<IFile> settingsDir;
    CFile::New(String("/data/system/privacy/"), (IFile**)&settingsDir);
    Boolean maked;
    settingsDir->Mkdirs(&maked);
    // make it readable for everybody
    Boolean success;
    settingsDir->SetReadable(TRUE, FALSE, &success);
    settingsDir->SetExecutable(TRUE, FALSE, &success);
}

AutoPtr<ISQLiteDatabase> CPrivacyPersistenceAdapter::GetDatabase()
{
    AutoLock lock(mLock);
    Boolean opened;
    Boolean readOnly;
    if (mDb == NULL || !(mDb->IsOpen(&opened), opened)
        || (mDb->IsReadOnly(&readOnly), readOnly)) {
        if (LOG_OPEN_AND_CLOSE) {
            Logger::D(TAG, "PrivacyPersistenceAdapter:closeIdleDatabase: Opening privacy database");
        }
        if (mDb) {
            mDb->Close();
            mDb = NULL;
        }
        SQLiteDatabase::OpenDatabase(DATABASE_FILE, NULL,
            ISQLiteDatabase::OPEN_READWRITE, (ISQLiteDatabase**)&mDb);
    }
    return mDb;
}

void CPrivacyPersistenceAdapter::CloseIdleDatabase()
{
    AutoLock lock(sDbAccessThreadsLock);
    --sDbAccessThreads;
//        if (LOG_OPEN_AND_CLOSE) Log.d(TAG, "PrivacyPersistenceAdapter:closeIdleDatabase: Decrement DB access threads: now " + Integer.toString(sDbAccessThreads));
        // only close DB if no other threads are reading
    Boolean opened;
    if (sDbAccessThreads == 0 && mDb != NULL && (mDb->IsOpen(&opened), opened)) {
        if (mAutoCloseDb) {
            if (LOG_OPEN_AND_CLOSE) Logger::D(TAG, "PrivacyPersistenceAdapter:closeIdleDatabase: Closing the PDroid database");
            mDb->Close();
        }
        else {
            if (LOG_OPEN_AND_CLOSE) Logger::D(TAG, "PrivacyPersistenceAdapter:closeIdleDatabase: Open and close DB disabled: not closing");
        }
    }
}

}
}
}
