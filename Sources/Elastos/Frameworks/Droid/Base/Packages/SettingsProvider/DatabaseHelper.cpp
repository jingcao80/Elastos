
#include "DatabaseHelper.h"
#include "CSettingsProvider.h"
#include "elastos/droid/text/TextUtils.h"
#include "util/XmlUtils.h"
#include "R.h"
#include <R.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CStringWrapper;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::INumber;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Database::Sqlite::EIID_ISQLiteOpenHelper;
using Elastos::Droid::Internal::Content::IPackageHelper;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioManagerHelper;
using Elastos::Droid::Media::CAudioManagerHelper;
using Elastos::Droid::Media::IAudioServiceHelper;
using Elastos::Droid::Media::CAudioServiceHelper;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::ISettingsBookmarks;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::XmlUtils;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManager;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SettingsProvider {

const String DatabaseHelper::TAG("SettingsProvider");
const String DatabaseHelper::DATABASE_NAME("settings.db");
const Int32 DatabaseHelper::DATABASE_VERSION;

HashSet<String> DatabaseHelper::sValidTables;

const String DatabaseHelper::TABLE_SYSTEM("system");
const String DatabaseHelper::TABLE_SECURE("secure");
const String DatabaseHelper::TABLE_GLOBAL("global");

Boolean DatabaseHelper::InitValidTables()
{
    sValidTables.Insert(TABLE_SYSTEM);
    sValidTables.Insert(TABLE_SECURE);
    sValidTables.Insert(TABLE_GLOBAL);
    sValidTables.Insert(String("bluetooth_devices"));
    sValidTables.Insert(String("bookmarks"));

    // These are old.
    sValidTables.Insert(String("favorites"));
    sValidTables.Insert(String("gservices"));
    sValidTables.Insert(String("old_favorites"));
    return TRUE;
}
const Boolean DatabaseHelper::sValidTablesInited = InitValidTables();

String DatabaseHelper::DbNameForUser(
    /* [in] */ Int32 userHandle)
{
    // The owner gets the unadorned db name;
    if (userHandle == IUserHandle::USER_OWNER) {
        return DATABASE_NAME;
    }
    else {
        // Place the database in the user-specific data tree so that it's
        // cleaned up automatically when the user is deleted.
        AutoPtr<IEnvironment> environment;
        CEnvironment::AcquireSingleton((IEnvironment**)&environment);
        AutoPtr<IFile> systemDir;
        environment->GetUserSystemDirectory(userHandle, (IFile**)&systemDir);
        AutoPtr<IFile> databaseFile;
        CFile::New(systemDir, DATABASE_NAME, (IFile**)&databaseFile);
        String path;
        databaseFile->GetPath(&path);
        return path;
    }
}

DatabaseHelper::DatabaseHelper(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userHandle)
    : mContext(context)
    , mUserHandle(userHandle)
{
    Init(context, DbNameForUser(userHandle), NULL, DATABASE_VERSION);
    SetWriteAheadLoggingEnabled(true);
}

CAR_INTERFACE_IMPL(DatabaseHelper, ISQLiteOpenHelper)

Boolean DatabaseHelper::IsValidTable(
    /* [in] */ const String& name)
{
    return sValidTables.Find(name) != sValidTables.End();
}

void DatabaseHelper::CreateSecureTable(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("CREATE TABLE secure (_id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT UNIQUE ON CONFLICT REPLACE,value TEXT);"));
    db->ExecSQL(String("CREATE INDEX secureIndex1 ON secure (name);"));
}

void DatabaseHelper::CreateGlobalTable(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("CREATE TABLE global (_id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT UNIQUE ON CONFLICT REPLACE,value TEXT);"));
    db->ExecSQL(String("CREATE INDEX globalIndex1 ON global (name);"));
}

ECode DatabaseHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("CREATE TABLE system (_id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT UNIQUE ON CONFLICT REPLACE,value TEXT);"));
    db->ExecSQL(String("CREATE INDEX systemIndex1 ON system (name);"));

    CreateSecureTable(db);

    // Only create the global table for the singleton 'owner' user
    if (mUserHandle == IUserHandle::USER_OWNER) {
        CreateGlobalTable(db);
    }

    db->ExecSQL(String("CREATE TABLE bluetooth_devices (_id INTEGER PRIMARY KEY,name TEXT,addr TEXT,channel INTEGER,type INTEGER);"));
    db->ExecSQL(String("CREATE TABLE bookmarks (_id INTEGER PRIMARY KEY,title TEXT,folder TEXT,intent TEXT,shortcut INTEGER,ordering INTEGER);"));

    db->ExecSQL(String("CREATE INDEX bookmarksIndex1 ON bookmarks (folder);"));
    db->ExecSQL(String("CREATE INDEX bookmarksIndex2 ON bookmarks (shortcut);"));

    // Populate bookmarks table with initial bookmarks
    Boolean onlyCore = FALSE;
    // try {
    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    AutoPtr<IIPackageManager> packageManager;
    if (SUCCEEDED(serviceManager->GetService(String("package"), (IInterface**)&packageManager))) {
        packageManager->IsOnlyCoreApps(&onlyCore);
    }
    // } catch (RemoteException e) {
    // }
    if (!onlyCore) {
        LoadBookmarks(db);
    }

    // Load initial volume levels into DB
    LoadVolumeLevels(db);

    // Load inital settings values
    LoadSettings(db);
    return NOERROR;
}

ECode DatabaseHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 currentVersion)
{
    Logger::W(TAG, "Upgrading settings database from version %d to %d", oldVersion, currentVersion);

    Int32 upgradeVersion = oldVersion;

    // Pattern for upgrade blocks:
    //
    //    if (upgradeVersion == [the DATABASE_VERSION you set] - 1) {
    //        .. your upgrade logic..
    //        upgradeVersion = [the DATABASE_VERSION you set]
    //    }

    if (upgradeVersion == 20) {
        /*
         * Version 21 is part of the volume control refresh. There is no
         * longer a UI-visible for setting notification vibrate on/off (in
         * our design), but the functionality still exists. Force the
         * notification vibrate to on.
         */
        LoadVibrateSetting(db, TRUE);

        upgradeVersion = 21;
    }

    if (upgradeVersion < 22) {
        upgradeVersion = 22;
        // Upgrade the lock gesture storage location and format
        UpgradeLockPatternLocation(db);
    }

    if (upgradeVersion < 23) {
        db->ExecSQL(String("UPDATE favorites SET iconResource=0 WHERE iconType=0"));
        upgradeVersion = 23;
    }

    if (upgradeVersion == 23) {
        db->BeginTransaction();
        // try {
        db->ExecSQL(String("ALTER TABLE favorites ADD spanX INTEGER"));
        db->ExecSQL(String("ALTER TABLE favorites ADD spanY INTEGER"));
        // Shortcuts, applications, folders
        db->ExecSQL(String("UPDATE favorites SET spanX=1, spanY=1 WHERE itemType<=0"));
        // Photo frames, clocks
        db->ExecSQL(String("UPDATE favorites SET spanX=2, spanY=2 WHERE itemType=1000 or itemType=1002"));
        // Search boxes
        db->ExecSQL(String("UPDATE favorites SET spanX=4, spanY=1 WHERE itemType=1001"));
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 24;
    }

    if (upgradeVersion == 24) {
        db->BeginTransaction();
        // try {
        // The value of the constants for preferring wifi or preferring mobile have been
        // swapped, so reload the default.
        db->ExecSQL(String("DELETE FROM system WHERE name='network_preference'"));
        StringBuilder sb("INSERT INTO system ('name', 'value') values ('network_preference', '");
        sb += IConnectivityManager::DEFAULT_NETWORK_PREFERENCE;
        sb += "')";
        db->ExecSQL(sb.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 25;
    }

    if (upgradeVersion == 25) {
        db->BeginTransaction();
        // try {
        db->ExecSQL(String("ALTER TABLE favorites ADD uri TEXT"));
        db->ExecSQL(String("ALTER TABLE favorites ADD displayMode INTEGER"));
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 26;
    }

    if (upgradeVersion == 26) {
        // This introduces the new secure settings table.
        db->BeginTransaction();
        // try {
        CreateSecureTable(db);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 27;
    }

    if (upgradeVersion == 27) {
        AutoPtr< ArrayOf<String> > settingsToMove = ArrayOf<String>::Alloc(31);
        (*settingsToMove)[0] = ISettingsSecure::ADB_ENABLED;
        (*settingsToMove)[1] = ISettingsSecure::ANDROID_ID;
        (*settingsToMove)[2] = ISettingsSecure::BLUETOOTH_ON;
        (*settingsToMove)[3] = ISettingsSecure::DATA_ROAMING;
        (*settingsToMove)[4] = ISettingsSecure::DEVICE_PROVISIONED;
        (*settingsToMove)[5] = ISettingsSecure::HTTP_PROXY;
        (*settingsToMove)[6] = ISettingsSecure::INSTALL_NON_MARKET_APPS;
        (*settingsToMove)[7] = ISettingsSecure::LOCATION_PROVIDERS_ALLOWED;
        (*settingsToMove)[8] = ISettingsSecure::LOGGING_ID;
        (*settingsToMove)[9] = ISettingsSecure::NETWORK_PREFERENCE;
        (*settingsToMove)[10] = ISettingsSecure::PARENTAL_CONTROL_ENABLED;
        (*settingsToMove)[11] = ISettingsSecure::PARENTAL_CONTROL_LAST_UPDATE;
        (*settingsToMove)[12] = ISettingsSecure::PARENTAL_CONTROL_REDIRECT_URL;
        (*settingsToMove)[13] = ISettingsSecure::SETTINGS_CLASSNAME;
        (*settingsToMove)[14] = ISettingsSecure::USB_MASS_STORAGE_ENABLED;
        (*settingsToMove)[15] = ISettingsSecure::USE_GOOGLE_MAIL;
        (*settingsToMove)[16] = ISettingsSecure::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON;
        (*settingsToMove)[17] = ISettingsSecure::WIFI_NETWORKS_AVAILABLE_REPEAT_DELAY;
        (*settingsToMove)[18] = ISettingsSecure::WIFI_NUM_OPEN_NETWORKS_KEPT;
        (*settingsToMove)[19] = ISettingsSecure::WIFI_ON;
        (*settingsToMove)[20] = ISettingsSecure::WIFI_WATCHDOG_ACCEPTABLE_PACKET_LOSS_PERCENTAGE;
        (*settingsToMove)[21] = ISettingsSecure::WIFI_WATCHDOG_AP_COUNT;
        (*settingsToMove)[22] = ISettingsSecure::WIFI_WATCHDOG_BACKGROUND_CHECK_DELAY_MS;
        (*settingsToMove)[23] = ISettingsSecure::WIFI_WATCHDOG_BACKGROUND_CHECK_ENABLED;
        (*settingsToMove)[24] = ISettingsSecure::WIFI_WATCHDOG_BACKGROUND_CHECK_TIMEOUT_MS;
        (*settingsToMove)[25] = ISettingsSecure::WIFI_WATCHDOG_INITIAL_IGNORED_PING_COUNT;
        (*settingsToMove)[26] = ISettingsSecure::WIFI_WATCHDOG_MAX_AP_CHECKS;
        (*settingsToMove)[27] = ISettingsSecure::WIFI_WATCHDOG_ON;
        (*settingsToMove)[28] = ISettingsSecure::WIFI_WATCHDOG_PING_COUNT;
        (*settingsToMove)[29] = ISettingsSecure::WIFI_WATCHDOG_PING_DELAY_MS;
        (*settingsToMove)[30] = ISettingsSecure::WIFI_WATCHDOG_PING_TIMEOUT_MS;
        MoveSettingsToNewTable(db, TABLE_SYSTEM, TABLE_SECURE, settingsToMove, FALSE);
        upgradeVersion = 28;
    }

    if (upgradeVersion == 28 || upgradeVersion == 29) {
        // Note: The upgrade to 28 was flawed since it didn't delete the old
        // setting first before inserting. Combining 28 and 29 with the
        // fixed version.

        // This upgrade adds the STREAM_NOTIFICATION type to the list of
        // types affected by ringer modes (silent, vibrate, etc.)
        db->BeginTransaction();
        // try {
        db->ExecSQL(String("DELETE FROM system WHERE name='")
                + ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED + String("'"));
        Int32 newValue = (1 << IAudioManager::STREAM_RING) | (1 << IAudioManager::STREAM_NOTIFICATION)
                | (1 << IAudioManager::STREAM_SYSTEM);
        StringBuilder sb("INSERT INTO system ('name', 'value') values ('");
        sb += ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED;
        sb += "', '";
        sb += newValue;
        sb += "')";
        db->ExecSQL(sb.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 30;
    }

    if (upgradeVersion == 30) {
        /*
         * Upgrade 31 clears the title for all quick launch shortcuts so the
         * activities' titles will be resolved at display time. Also, the
         * folder is changed to '@quicklaunch'.
         */
        db->BeginTransaction();
        // try {
        db->ExecSQL(String("UPDATE bookmarks SET folder = '@quicklaunch'"));
        db->ExecSQL(String("UPDATE bookmarks SET title = ''"));
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 31;
    }

    if (upgradeVersion == 31) {
        /*
         * Animations are now managed in preferences, and may be
         * enabled or disabled based on product resources.
         */
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->ExecSQL(String("DELETE FROM system WHERE name='")
                + ISettingsSystem::WINDOW_ANIMATION_SCALE + String("'"));
        db->ExecSQL(String("DELETE FROM system WHERE name='")
                + ISettingsSystem::TRANSITION_ANIMATION_SCALE + String("'"));
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadDefaultAnimationSettings(stmt);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 32;
    }

    if (upgradeVersion == 32) {
        // The Wi-Fi watchdog SSID list is now seeded with the value of
        // the property ro.com.android.wifi-watchlist
        AutoPtr<ISystemProperties> prop;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&prop);
        String wifiWatchList;
        prop->Get(String("ro.com.android.wifi-watchlist"), &wifiWatchList);
        if (!TextUtils::IsEmpty(wifiWatchList)) {
            db->BeginTransaction();
            // try {
            StringBuilder sb("INSERT OR IGNORE INTO secure(name,value) values('");
            sb += "','";
            sb += wifiWatchList;
            sb += "');";
            db->ExecSQL(sb.ToString());
            db->SetTransactionSuccessful();
            // } finally {
            //     db.endTransaction();
            // }
            db->EndTransaction();
        }
        upgradeVersion = 33;
    }

    if (upgradeVersion == 33) {
        // Set the default zoom controls to: tap-twice to bring up +/-
        db->BeginTransaction();
        // try {
        db->ExecSQL(String("INSERT INTO system(name,value) values('zoom','2');"));
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 34;
    }

    if (upgradeVersion == 34) {
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR IGNORE INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadSecure35Settings(stmt);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 35;
    }
    // due to a botched merge from donut to eclair, the initialization of ASSISTED_GPS_ENABLED
    // was accidentally done out of order here.
    // to fix this, ASSISTED_GPS_ENABLED is now initialized while upgrading from 38 to 39,
    // and we intentionally do nothing from 35 to 36 now.
    if (upgradeVersion == 35) {
        upgradeVersion = 36;
    }

    if (upgradeVersion == 36) {
       // This upgrade adds the STREAM_SYSTEM_ENFORCED type to the list of
        // types affected by ringer modes (silent, vibrate, etc.)
        db->BeginTransaction();
        // try {
        db->ExecSQL(String("DELETE FROM system WHERE name='")
                + ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED + String("'"));
        Int32 newValue = (1 << IAudioManager::STREAM_RING)
                | (1 << IAudioManager::STREAM_NOTIFICATION)
                | (1 << IAudioManager::STREAM_SYSTEM)
                | (1 << IAudioManager::STREAM_SYSTEM_ENFORCED);
        StringBuilder sb("INSERT INTO system ('name', 'value') values ('");
        sb += ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED;
        sb += "', '";
        sb += newValue;
        sb += "')";
        db->ExecSQL(sb.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 37;
    }

    if (upgradeVersion == 37) {
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR IGNORE INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadStringSetting(stmt, ISettingsSystem::AIRPLANE_MODE_TOGGLEABLE_RADIOS,
                R::string::airplane_mode_toggleable_radios);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 38;
    }

    if (upgradeVersion == 38) {
        db->BeginTransaction();
        // try {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Boolean v;
        res->GetBoolean(R::bool_::assisted_gps_enabled, &v);
        String value = v ? String("1") : String("0");
        StringBuilder sb("INSERT OR IGNORE INTO secure(name,value) values('");
        sb += ISettingsGlobal::ASSISTED_GPS_ENABLED;
        sb += "','";
        sb += value;
        sb += "');";
        db->ExecSQL(sb.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 39;
    }

    if (upgradeVersion == 39) {
        UpgradeAutoBrightness(db);
        upgradeVersion = 40;
    }

    if (upgradeVersion == 40) {
        /*
         * All animations are now turned on by default!
         */
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->ExecSQL(String("DELETE FROM system WHERE name='")
                + ISettingsSystem::WINDOW_ANIMATION_SCALE + String("'"));
        db->ExecSQL(String("DELETE FROM system WHERE name='")
                + ISettingsSystem::TRANSITION_ANIMATION_SCALE + String("'"));
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadDefaultAnimationSettings(stmt);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 41;
    }

    if (upgradeVersion == 41) {
        /*
         * Initialize newly public haptic feedback setting
         */
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->ExecSQL(String("DELETE FROM system WHERE name='")
                + ISettingsSystem::HAPTIC_FEEDBACK_ENABLED + String("'"));
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadDefaultHapticSettings(stmt);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 42;
    }

    if (upgradeVersion == 42) {
        /*
         * Initialize new notification pulse setting
         */
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSystem::NOTIFICATION_LIGHT_PULSE, R::bool_::def_notification_pulse);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 43;
    }

    if (upgradeVersion == 43) {
        /*
         * This upgrade stores bluetooth volume separately from voice volume
         */
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR IGNORE INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        AutoPtr<IAudioManagerHelper> helper;
        CAudioManagerHelper::AcquireSingleton((IAudioManagerHelper**)&helper);
        AutoPtr< ArrayOf<Int32> > volume;
        helper->GetDefaultStreamVolume((ArrayOf<Int32>**)&volume);
        AutoPtr<IInteger32> integer;
        CInteger32::New((*volume)[IAudioManager::STREAM_BLUETOOTH_SCO], (IInteger32**)&integer);
        LoadSetting(stmt, ISettingsSystem::VOLUME_BLUETOOTH_SCO, integer);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 44;
    }

    if (upgradeVersion == 44) {
        /*
         * Gservices was moved into vendor/google.
         */
        db->ExecSQL(String("DROP TABLE IF EXISTS gservices"));
        db->ExecSQL(String("DROP INDEX IF EXISTS gservicesIndex1"));
        upgradeVersion = 45;
    }

    if (upgradeVersion == 45) {
         /*
          * New settings for MountService
          */
        db->BeginTransaction();
        // try {
        db->ExecSQL(String("INSERT INTO secure(name,value) values('") +
                ISettingsSecure::MOUNT_PLAY_NOTIFICATION_SND + String("','1');"));
        db->ExecSQL(String("INSERT INTO secure(name,value) values('") +
                ISettingsSecure::MOUNT_UMS_AUTOSTART + String("','0');"));
        db->ExecSQL(String("INSERT INTO secure(name,value) values('") +
                ISettingsSecure::MOUNT_UMS_PROMPT + String("','1');"));
        db->ExecSQL(String("INSERT INTO secure(name,value) values('" )+
                ISettingsSecure::MOUNT_UMS_NOTIFY_ENABLED + String("','1');"));
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 46;
    }

    if (upgradeVersion == 46) {
        /*
         * The password mode constants have changed; reset back to no
         * password.
         */
        db->BeginTransaction();
        // try {
        db->ExecSQL(String("DELETE FROM system WHERE name='lockscreen.password_type';"));
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 47;
   }


    if (upgradeVersion == 47) {
        /*
         * The password mode constants have changed again; reset back to no
         * password.
         */
        db->BeginTransaction();
        // try {
        db->ExecSQL(String("DELETE FROM system WHERE name='lockscreen.password_type';"));
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 48;
   }

   if (upgradeVersion == 48) {
        /*
        * Default recognition service no longer initialized here,
        * moved to RecognitionManagerService.
        */
        upgradeVersion = 49;
   }

   if (upgradeVersion == 49) {
        /*
        * New settings for new user interface noises.
        */
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadUISoundEffectsSettings(stmt);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 50;
   }

   if (upgradeVersion == 50) {
       /*
        * Install location no longer initiated here.
        */
       upgradeVersion = 51;
   }

   if (upgradeVersion == 51) {
        /* Move the lockscreen related settings to Secure, including some private ones. */
        AutoPtr< ArrayOf<String> > settingsToMove = ArrayOf<String>::Alloc(9);
        (*settingsToMove)[0] = ISettingsSecure::LOCK_PATTERN_ENABLED;
        (*settingsToMove)[1] = ISettingsSecure::LOCK_PATTERN_VISIBLE;
        (*settingsToMove)[2] = ISettingsSecure::LOCK_PATTERN_TACTILE_FEEDBACK_ENABLED;
        (*settingsToMove)[3] = String("lockscreen.password_type");
        (*settingsToMove)[4] = String("lockscreen.lockoutattemptdeadline");
        (*settingsToMove)[5] = String("lockscreen.patterneverchosen");
        (*settingsToMove)[6] = String("lock_pattern_autolock");
        (*settingsToMove)[7] = String("lockscreen.lockedoutpermanently");
        (*settingsToMove)[8] = String("lockscreen.password_salt");
        MoveSettingsToNewTable(db, TABLE_SYSTEM, TABLE_SECURE, settingsToMove, FALSE);
        upgradeVersion = 52;
   }

    if (upgradeVersion == 52) {
        // new vibration/silent mode settings
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSystem::VIBRATE_IN_SILENT, R::bool_::def_vibrate_in_silent);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 53;
    }

    if (upgradeVersion == 53) {
        /*
         * New settings for set install location UI no longer initiated here.
         */
        upgradeVersion = 54;
    }

    if (upgradeVersion == 54) {
        /*
         * Update the screen timeout value if set to never
         */
        db->BeginTransaction();
        // try {
        UpgradeScreenTimeoutFromNever(db);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 55;
    }

    if (upgradeVersion == 55) {
        /* Move the install location settings. */
        AutoPtr< ArrayOf<String> > settingsToMove = ArrayOf<String>::Alloc(2);
        (*settingsToMove)[0] = ISettingsGlobal::SET_INSTALL_LOCATION;
        (*settingsToMove)[1] = ISettingsGlobal::DEFAULT_INSTALL_LOCATION;
        MoveSettingsToNewTable(db, TABLE_SYSTEM, TABLE_SECURE, settingsToMove, FALSE);
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        AutoPtr<IInteger32> integer1, integer2;
        CInteger32::New(0, (IInteger32**)&integer1);
        CInteger32::New(IPackageHelper::APP_INSTALL_AUTO, (IInteger32**)&integer2);
        LoadSetting(stmt, ISettingsGlobal::SET_INSTALL_LOCATION, integer1);
        LoadSetting(stmt, ISettingsGlobal::DEFAULT_INSTALL_LOCATION, integer2);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 56;
    }

    if (upgradeVersion == 56) {
        /*
         * Add Bluetooth to list of toggleable radios in airplane mode
         */
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->ExecSQL(String("DELETE FROM system WHERE name='")
                + ISettingsSystem::AIRPLANE_MODE_TOGGLEABLE_RADIOS + String("'"));
        db->CompileStatement(String("INSERT OR IGNORE INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadStringSetting(stmt, ISettingsSystem::AIRPLANE_MODE_TOGGLEABLE_RADIOS,
                R::string::airplane_mode_toggleable_radios);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 57;
    }

    /************* The following are Honeycomb changes ************/

    if (upgradeVersion == 57) {
        /*
         * New settings to:
         *  1. Enable injection of accessibility scripts in WebViews.
         *  2. Define the key bindings for traversing web content in WebViews.
         */
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSecure::ACCESSIBILITY_SCRIPT_INJECTION,
                R::bool_::def_accessibility_script_injection);
        if (stmt) stmt->Close();
        AutoPtr<ISQLiteStatement> stmt1;
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt1);
        LoadStringSetting(stmt1, ISettingsSecure::ACCESSIBILITY_WEB_CONTENT_KEY_BINDINGS,
                R::string::def_accessibility_web_content_key_bindings);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt1) stmt1->Close();
        upgradeVersion = 58;
    }

    if (upgradeVersion == 58) {
        /* Add default for new Auto Time Zone */
        Int32 autoTimeValue = GetInt32ValueFromSystem(db, ISettingsSystem::AUTO_TIME, 0);
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        AutoPtr<IInteger32> integer;
        CInteger32::New(autoTimeValue, (IInteger32**)&integer);
        LoadSetting(stmt, ISettingsSystem::AUTO_TIME_ZONE, integer); // Sync timezone to NITZ if auto_time was enabled
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 59;
    }

    if (upgradeVersion == 59) {
        // Persistence for the rotation lock feature.
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSystem::USER_ROTATION, R::integer::def_user_rotation); // should be zero degrees
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 60;
    }

    if (upgradeVersion == 60) {
        // Don't do this for upgrades from Gingerbread
        // Were only required for intra-Honeycomb upgrades for testing
        // upgradeScreenTimeout(db);
        upgradeVersion = 61;
    }

    if (upgradeVersion == 61) {
        // Don't do this for upgrades from Gingerbread
        // Were only required for intra-Honeycomb upgrades for testing
        // upgradeScreenTimeout(db);
        upgradeVersion = 62;
    }

    // Change the default for screen auto-brightness mode
    if (upgradeVersion == 62) {
        // Don't do this for upgrades from Gingerbread
        // Were only required for intra-Honeycomb upgrades for testing
        // upgradeAutoBrightness(db);
        upgradeVersion = 63;
    }

    if (upgradeVersion == 63) {
        // This upgrade adds the STREAM_MUSIC type to the list of
        // types affected by ringer modes (silent, vibrate, etc.)
        db->BeginTransaction();
        // try {
        db->ExecSQL(String("DELETE FROM system WHERE name='")
                + ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED + String("'"));
        Int32 newValue = (1 << IAudioManager::STREAM_RING)
                | (1 << IAudioManager::STREAM_NOTIFICATION)
                | (1 << IAudioManager::STREAM_SYSTEM)
                | (1 << IAudioManager::STREAM_SYSTEM_ENFORCED)
                | (1 << IAudioManager::STREAM_MUSIC);
        StringBuilder sb("INSERT INTO system ('name', 'value') values ('");
        sb += ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED;
        sb += "', '";
        sb += newValue;
        sb += "')";
        db->ExecSQL(sb.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 64;
     }

    if (upgradeVersion == 64) {
        // New setting to configure the long press timeout.
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadIntegerSetting(stmt, ISettingsSecure::LONG_PRESS_TIMEOUT, R::integer::def_long_press_timeout_millis);
        stmt->Close();
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 65;
    }

    /************* The following are Ice Cream Sandwich changes ************/

    if (upgradeVersion == 65) {
        /*
         * Animations are removed from Settings. Turned on by default
         */
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->ExecSQL(String("DELETE FROM system WHERE name='")
                + ISettingsSystem::WINDOW_ANIMATION_SCALE + String("'"));
        db->ExecSQL(String("DELETE FROM system WHERE name='")
                + ISettingsSystem::TRANSITION_ANIMATION_SCALE + String("'"));
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadDefaultAnimationSettings(stmt);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 66;
    }

    if (upgradeVersion == 66) {
        // This upgrade makes sure that MODE_RINGER_STREAMS_AFFECTED is set
        // according to device voice capability
        db->BeginTransaction();
        // try {
        Int32 ringerModeAffectedStreams = (1 << IAudioManager::STREAM_RING) |
                (1 << IAudioManager::STREAM_NOTIFICATION) |
                (1 << IAudioManager::STREAM_SYSTEM) |
                (1 << IAudioManager::STREAM_SYSTEM_ENFORCED);
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Boolean v;
        if (res->GetBoolean(Elastos::Droid::R::bool_::config_voice_capable, &v), !v) {
            ringerModeAffectedStreams |= (1 << IAudioManager::STREAM_MUSIC);
        }
        db->ExecSQL(String("DELETE FROM system WHERE name='")
                + ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED + String("'"));
        StringBuilder sb("INSERT INTO system ('name', 'value') values ('");
        sb += ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED;
        sb += "', '";
        sb += ringerModeAffectedStreams;
        sb += "')";
        db->ExecSQL(sb.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 67;
    }

    if (upgradeVersion == 67) {
        // New setting to enable touch exploration.
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSecure::TOUCH_EXPLORATION_ENABLED,
                R::bool_::def_touch_exploration_enabled);
        stmt->Close();
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 68;
    }

    if (upgradeVersion == 68) {
        // Enable all system sounds by default
        db->BeginTransaction();
        // try {
        db->ExecSQL(String("DELETE FROM system WHERE name='")
                + ISettingsSystem::NOTIFICATIONS_USE_RING_VOLUME + String("'"));
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 69;
    }

    if (upgradeVersion == 69) {
        // Add RADIO_NFC to AIRPLANE_MODE_RADIO and AIRPLANE_MODE_TOGGLEABLE_RADIOS
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        String airplaneRadios;
        res->GetString(R::string::def_airplane_mode_radios, &airplaneRadios);
        String toggleableRadios;
        res->GetString(R::string::airplane_mode_toggleable_radios, &toggleableRadios);
        db->BeginTransaction();
        // try {
        StringBuilder sb("UPDATE system SET value='");
        sb += airplaneRadios;
        sb += "' WHERE name='";
        sb += ISettingsSystem::AIRPLANE_MODE_RADIOS;
        sb += "'";
        db->ExecSQL(sb.ToString());
        StringBuilder sb1("UPDATE system SET value='");
        sb1 += toggleableRadios;
        sb1 += "' WHERE name='";
        sb1 += ISettingsSystem::AIRPLANE_MODE_TOGGLEABLE_RADIOS;
        sb1 += "'";
        db->ExecSQL(sb1.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 70;
    }

    if (upgradeVersion == 70) {
        // Update all built-in bookmarks.  Some of the package names have changed.
        LoadBookmarks(db);
        upgradeVersion = 71;
    }

    if (upgradeVersion == 71) {
         // New setting to specify whether to speak passwords in accessibility mode.
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSecure::ACCESSIBILITY_SPEAK_PASSWORD,
                R::bool_::def_accessibility_speak_password);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 72;
    }

    if (upgradeVersion == 72) {
        // update vibration settings
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR REPLACE INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSystem::VIBRATE_IN_SILENT,
                R::bool_::def_vibrate_in_silent);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 73;
    }

    if (upgradeVersion == 73) {
        UpgradeVibrateSettingFromNone(db);
        upgradeVersion = 74;
    }

    if (upgradeVersion == 74) {
        // URL from which WebView loads a JavaScript based screen-reader.
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadStringSetting(stmt, ISettingsSecure::ACCESSIBILITY_SCREEN_READER_URL,
                R::string::def_accessibility_screen_reader_url);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 75;
    }

    if (upgradeVersion == 75) {
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        AutoPtr<ICursor> c;
        // try {
        AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
        (*args)[0] = String("_id");
        (*args)[1] = String("value");
        db->Query(TABLE_SECURE, args, String("name='lockscreen.disabled'"),
                NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&c);
        // only set default if it has not yet been set
        Int32 count;
        if (c == NULL || (c->GetCount(&count), count == 0)) {
            db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
            LoadBooleanSetting(stmt, ISettingsSystem::LOCKSCREEN_DISABLED,
                    R::bool_::def_lockscreen_disabled);
        }
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (c != null) c.close();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (c) c->Close();
        if (stmt) stmt->Close();
        upgradeVersion = 76;
    }

    /************* The following are Jelly Bean changes ************/

    if (upgradeVersion == 76) {
        // Removed VIBRATE_IN_SILENT setting
        db->BeginTransaction();
        // try {
        db->ExecSQL(String("DELETE FROM system WHERE name='")
                + ISettingsSystem::VIBRATE_IN_SILENT + String("'"));
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        // }
        db->EndTransaction();
        upgradeVersion = 77;
    }

    if (upgradeVersion == 77) {
        // Introduce "vibrate when ringing" setting
        LoadVibrateWhenRingingSetting(db);

        upgradeVersion = 78;
    }

    if (upgradeVersion == 78) {
        // The JavaScript based screen-reader URL changes in JellyBean.
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR REPLACE INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadStringSetting(stmt, ISettingsSecure::ACCESSIBILITY_SCREEN_READER_URL,
                R::string::def_accessibility_screen_reader_url);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 79;
    }

    if (upgradeVersion == 79) {
        // Before touch exploration was a global setting controlled by the user
        // via the UI. However, if the enabled accessibility services do not
        // handle touch exploration mode, enabling it makes no sense. Therefore,
        // now the services request touch exploration mode and the user is
        // presented with a dialog to allow that and if she does we store that
        // in the database. As a result of this change a user that has enabled
        // accessibility, touch exploration, and some accessibility services
        // may lose touch exploration state, thus rendering the device useless
        // unless sighted help is provided, since the enabled service(s) are
        // not in the list of services to which the user granted a permission
        // to put the device in touch explore mode. Here we are allowing all
        // enabled accessibility services to toggle touch exploration provided
        // accessibility and touch exploration are enabled and no services can
        // toggle touch exploration. Note that the user has already manually
        // enabled the services and touch exploration which means the she has
        // given consent to have these services work in touch exploration mode.
        Boolean accessibilityEnabled = GetInt32ValueFromTable(db, TABLE_SECURE,
                ISettingsSecure::ACCESSIBILITY_ENABLED, 0) == 1;
        Boolean touchExplorationEnabled = GetInt32ValueFromTable(db, TABLE_SECURE,
                ISettingsSecure::TOUCH_EXPLORATION_ENABLED, 0) == 1;
        if (accessibilityEnabled && touchExplorationEnabled) {
            String enabledServices = GetStringValueFromTable(db, TABLE_SECURE,
                    ISettingsSecure::ENABLED_ACCESSIBILITY_SERVICES, String(""));
            String touchExplorationGrantedServices = GetStringValueFromTable(db, TABLE_SECURE,
                    ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES, String(""));
            if (TextUtils::IsEmpty(touchExplorationGrantedServices)
                    && !TextUtils::IsEmpty(enabledServices)) {
                AutoPtr<ISQLiteStatement> stmt;
                // try {
                db->BeginTransaction();
                db->CompileStatement(String("INSERT OR REPLACE INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
                AutoPtr<ICharSequence> cs;
                CStringWrapper::New(enabledServices, (ICharSequence**)&cs);
                LoadSetting(stmt,
                        ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES,
                        cs);
                db->SetTransactionSuccessful();
                // } finally {
                //     db.endTransaction();
                //     if (stmt != null) stmt.close();
                // }
                db->EndTransaction();
                if (stmt) stmt->Close();
            }
        }
        upgradeVersion = 80;
    }

    // vvv Jelly Bean MR1 changes begin here vvv

    if (upgradeVersion == 80) {
        // update screensaver settings
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR REPLACE INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSecure::SCREENSAVER_ENABLED,
                Elastos::Droid::R::bool_::config_dreamsEnabledByDefault);
        LoadBooleanSetting(stmt, ISettingsSecure::SCREENSAVER_ACTIVATE_ON_DOCK,
                Elastos::Droid::R::bool_::config_dreamsActivatedOnDockByDefault);
        LoadBooleanSetting(stmt, ISettingsSecure::SCREENSAVER_ACTIVATE_ON_SLEEP,
                Elastos::Droid::R::bool_::config_dreamsActivatedOnSleepByDefault);
        LoadStringSetting(stmt, ISettingsSecure::SCREENSAVER_COMPONENTS,
                Elastos::Droid::R::string::config_dreamsDefaultComponent);
        LoadStringSetting(stmt, ISettingsSecure::SCREENSAVER_DEFAULT_COMPONENT,
                Elastos::Droid::R::string::config_dreamsDefaultComponent);

        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 81;
    }

    if (upgradeVersion == 81) {
        // Add package verification setting
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR REPLACE INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsGlobal::PACKAGE_VERIFIER_ENABLE,
                R::bool_::def_package_verifier_enable);
        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt) stmt->Close();
        upgradeVersion = 82;
    }

    if (upgradeVersion == 82) {
        // Move to per-user settings dbs
        if (mUserHandle == IUserHandle::USER_OWNER) {

            db->BeginTransaction();
            AutoPtr<ISQLiteStatement> stmt;
            // try {
            // Migrate now-global settings. Note that this happens before
            // new users can be created.
            CreateGlobalTable(db);
            AutoPtr< ArrayOf<String> > settingsToMove = HashsetToStringArray(CSettingsProvider::sSystemGlobalKeys);
            MoveSettingsToNewTable(db, TABLE_SYSTEM, TABLE_GLOBAL, settingsToMove, FALSE);
            settingsToMove = HashsetToStringArray(CSettingsProvider::sSecureGlobalKeys);
            MoveSettingsToNewTable(db, TABLE_SECURE, TABLE_GLOBAL, settingsToMove, FALSE);

            db->SetTransactionSuccessful();
            // } finally {
            //     db.endTransaction();
            //     if (stmt != null) stmt.close();
            // }
            db->EndTransaction();
            if (stmt) stmt->Close();
        }
        upgradeVersion = 83;
    }

    if (upgradeVersion == 83) {
        // 1. Setting whether screen magnification is enabled.
        // 2. Setting for screen magnification scale.
        // 3. Setting for screen magnification auto update.
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt,
                ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_ENABLED,
                R::bool_::def_accessibility_display_magnification_enabled);
        if (stmt) stmt->Close();
        AutoPtr<ISQLiteStatement> stmt1;
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt1);
        LoadFractionSetting(stmt1, ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_SCALE,
                R::fraction::def_accessibility_display_magnification_scale, 1);
        if (stmt1) stmt1->Close();
        AutoPtr<ISQLiteStatement> stmt2;
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt2);
        LoadBooleanSetting(stmt2,
                ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_AUTO_UPDATE,
                R::bool_::def_accessibility_display_magnification_auto_update);

        db->SetTransactionSuccessful();
        // } finally {
        //     db.endTransaction();
        //     if (stmt != null) stmt.close();
        // }
        db->EndTransaction();
        if (stmt2) stmt2->Close();
        upgradeVersion = 84;
    }

    if (upgradeVersion == 84) {
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            // try {
            // Patch up the slightly-wrong key migration from 82 -> 83 for those
            // devices that missed it, ignoring if the move is redundant
            AutoPtr< ArrayOf<String> > settingsToMove = ArrayOf<String>::Alloc(6);
            (*settingsToMove)[0] = ISettingsSecure::ADB_ENABLED;
            (*settingsToMove)[1] = ISettingsSecure::BLUETOOTH_ON;
            (*settingsToMove)[2] = ISettingsSecure::DATA_ROAMING;
            (*settingsToMove)[3] = ISettingsSecure::DEVICE_PROVISIONED;
            (*settingsToMove)[4] = ISettingsSecure::INSTALL_NON_MARKET_APPS;
            (*settingsToMove)[5] = ISettingsSecure::USB_MASS_STORAGE_ENABLED;
            MoveSettingsToNewTable(db, TABLE_SECURE, TABLE_GLOBAL, settingsToMove, TRUE);
            db->SetTransactionSuccessful();
            // } finally {
            //     db.endTransaction();
            //     if (stmt != null) stmt.close();
            // }
            db->EndTransaction();
        }
        upgradeVersion = 85;
    }

    if (upgradeVersion == 85) {
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            // try {
            // Fix up the migration, ignoring already-migrated elements, to snap up to
            // date with new changes to the set of global versus system/secure settings
            AutoPtr< ArrayOf<String> > settingsToMove = ArrayOf<String>::Alloc(1);
            (*settingsToMove)[0] = ISettingsSystem::STAY_ON_WHILE_PLUGGED_IN;
            MoveSettingsToNewTable(db, TABLE_SYSTEM, TABLE_GLOBAL, settingsToMove, TRUE);

            db->SetTransactionSuccessful();
            // } finally {
            //     db.endTransaction();
            // }
            db->EndTransaction();
        }
        upgradeVersion = 86;
    }

    if (upgradeVersion == 86) {
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            // try {
            AutoPtr< ArrayOf<String> > settingsToMove = ArrayOf<String>::Alloc(3);
            (*settingsToMove)[0] = ISettingsGlobal::PACKAGE_VERIFIER_ENABLE;
            (*settingsToMove)[1] = ISettingsGlobal::PACKAGE_VERIFIER_TIMEOUT;
            (*settingsToMove)[2] = ISettingsGlobal::PACKAGE_VERIFIER_DEFAULT_RESPONSE;
            MoveSettingsToNewTable(db, TABLE_SECURE, TABLE_GLOBAL, settingsToMove, TRUE);

            db->SetTransactionSuccessful();
            // } finally {
            //     db.endTransaction();
            // }
            db->EndTransaction();
        }
        upgradeVersion = 87;
    }

    if (upgradeVersion == 87) {
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            // try {
            AutoPtr< ArrayOf<String> > settingsToMove = ArrayOf<String>::Alloc(3);
            (*settingsToMove)[0] = ISettingsGlobal::DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS;
            (*settingsToMove)[1] = ISettingsGlobal::DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS;
            (*settingsToMove)[2] = ISettingsGlobal::GPRS_REGISTER_CHECK_PERIOD_MS;
            MoveSettingsToNewTable(db, TABLE_SECURE, TABLE_GLOBAL, settingsToMove, TRUE);

            db->SetTransactionSuccessful();
            // } finally {
            //     db.endTransaction();
            // }
            db->EndTransaction();
        }
        upgradeVersion = 88;
    }

    if (upgradeVersion == 88) {
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            // try {
            AutoPtr< ArrayOf<String> > settingsToMove = ArrayOf<String>::Alloc(31);
            (*settingsToMove)[0] = ISettingsGlobal::BATTERY_DISCHARGE_DURATION_THRESHOLD;
            (*settingsToMove)[1] = ISettingsGlobal::BATTERY_DISCHARGE_THRESHOLD;
            (*settingsToMove)[2] = ISettingsGlobal::SEND_ACTION_APP_ERROR;
            (*settingsToMove)[3] = ISettingsGlobal::DROPBOX_AGE_SECONDS;
            (*settingsToMove)[4] = ISettingsGlobal::DROPBOX_MAX_FILES;
            (*settingsToMove)[5] = ISettingsGlobal::DROPBOX_QUOTA_KB;
            (*settingsToMove)[6] = ISettingsGlobal::DROPBOX_QUOTA_PERCENT;
            (*settingsToMove)[7] = ISettingsGlobal::DROPBOX_RESERVE_PERCENT;
            (*settingsToMove)[8] = ISettingsGlobal::DROPBOX_TAG_PREFIX;
            (*settingsToMove)[9] = ISettingsGlobal::ERROR_LOGCAT_PREFIX;
            (*settingsToMove)[10] = ISettingsGlobal::SYS_FREE_STORAGE_LOG_INTERVAL;
            (*settingsToMove)[11] = ISettingsGlobal::DISK_FREE_CHANGE_REPORTING_THRESHOLD;
            (*settingsToMove)[12] = ISettingsGlobal::SYS_STORAGE_THRESHOLD_PERCENTAGE;
            (*settingsToMove)[13] = ISettingsGlobal::SYS_STORAGE_THRESHOLD_MAX_BYTES;
            (*settingsToMove)[14] = ISettingsGlobal::SYS_STORAGE_FULL_THRESHOLD_BYTES;
            (*settingsToMove)[15] = ISettingsGlobal::SYNC_MAX_RETRY_DELAY_IN_SECONDS;
            (*settingsToMove)[16] = ISettingsGlobal::CONNECTIVITY_CHANGE_DELAY;
            (*settingsToMove)[17] = ISettingsGlobal::CAPTIVE_PORTAL_DETECTION_ENABLED;
            (*settingsToMove)[18] = ISettingsGlobal::CAPTIVE_PORTAL_SERVER;
            (*settingsToMove)[19] = ISettingsGlobal::NSD_ON;
            (*settingsToMove)[20] = ISettingsGlobal::SET_INSTALL_LOCATION;
            (*settingsToMove)[21] = ISettingsGlobal::DEFAULT_INSTALL_LOCATION;
            (*settingsToMove)[22] = ISettingsGlobal::INET_CONDITION_DEBOUNCE_UP_DELAY;
            (*settingsToMove)[23] = ISettingsGlobal::INET_CONDITION_DEBOUNCE_DOWN_DELAY;
            (*settingsToMove)[24] = ISettingsGlobal::READ_EXTERNAL_STORAGE_ENFORCED_DEFAULT;
            (*settingsToMove)[25] = ISettingsGlobal::HTTP_PROXY;
            (*settingsToMove)[26] = ISettingsGlobal::GLOBAL_HTTP_PROXY_HOST;
            (*settingsToMove)[27] = ISettingsGlobal::GLOBAL_HTTP_PROXY_PORT;
            (*settingsToMove)[28] = ISettingsGlobal::GLOBAL_HTTP_PROXY_EXCLUSION_LIST;
            (*settingsToMove)[29] = ISettingsGlobal::SET_GLOBAL_HTTP_PROXY;
            (*settingsToMove)[30] = ISettingsGlobal::DEFAULT_DNS_SERVER;
            MoveSettingsToNewTable(db, TABLE_SECURE, TABLE_GLOBAL, settingsToMove, TRUE);
            db->SetTransactionSuccessful();
            // } finally {
            //     db.endTransaction();
            // }
            db->EndTransaction();
        }
        upgradeVersion = 89;
    }

    if (upgradeVersion == 89) {
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            // try {
            AutoPtr< ArrayOf<String> > prefixesToMove = ArrayOf<String>::Alloc(3);
            (*prefixesToMove)[0] = ISettingsGlobal::BLUETOOTH_HEADSET_PRIORITY_PREFIX;
            (*prefixesToMove)[1] = ISettingsGlobal::BLUETOOTH_A2DP_SINK_PRIORITY_PREFIX;
            (*prefixesToMove)[2] = ISettingsGlobal::BLUETOOTH_INPUT_DEVICE_PRIORITY_PREFIX;

            MovePrefixedSettingsToNewTable(db, TABLE_SECURE, TABLE_GLOBAL, prefixesToMove);

            db->SetTransactionSuccessful();
            // } finally {
            //     db.endTransaction();
            // }
            db->EndTransaction();
        }
        upgradeVersion = 90;
    }

    if (upgradeVersion == 90) {
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            // try {
            AutoPtr< ArrayOf<String> > systemToGlobal = ArrayOf<String>::Alloc(11);
            (*systemToGlobal)[0] = ISettingsGlobal::WINDOW_ANIMATION_SCALE;
            (*systemToGlobal)[1] = ISettingsGlobal::TRANSITION_ANIMATION_SCALE;
            (*systemToGlobal)[2] = ISettingsGlobal::ANIMATOR_DURATION_SCALE;
            (*systemToGlobal)[3] = ISettingsGlobal::FANCY_IME_ANIMATIONS;
            (*systemToGlobal)[4] = ISettingsGlobal::COMPATIBILITY_MODE;
            (*systemToGlobal)[5] = ISettingsGlobal::EMERGENCY_TONE;
            (*systemToGlobal)[6] = ISettingsGlobal::CALL_AUTO_RETRY;
            (*systemToGlobal)[7] = ISettingsGlobal::DEBUG_APP;
            (*systemToGlobal)[8] = ISettingsGlobal::WAIT_FOR_DEBUGGER;
            (*systemToGlobal)[9] = ISettingsGlobal::SHOW_PROCESSES;
            (*systemToGlobal)[10] = ISettingsGlobal::ALWAYS_FINISH_ACTIVITIES;

            AutoPtr< ArrayOf<String> > secureToGlobal = ArrayOf<String>::Alloc(2);
            (*secureToGlobal)[0] = ISettingsGlobal::PREFERRED_NETWORK_MODE;
            (*secureToGlobal)[1] = ISettingsGlobal::PREFERRED_CDMA_SUBSCRIPTION;

            MoveSettingsToNewTable(db, TABLE_SYSTEM, TABLE_GLOBAL, systemToGlobal, TRUE);
            MoveSettingsToNewTable(db, TABLE_SECURE, TABLE_GLOBAL, secureToGlobal, TRUE);

            db->SetTransactionSuccessful();
            // } finally {
            //     db.endTransaction();
            // }
            db->EndTransaction();
        }
        upgradeVersion = 91;
    }

    if (upgradeVersion == 91) {
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            // try {
            // Move ringer mode from system to global settings
            AutoPtr< ArrayOf<String> > settingsToMove = ArrayOf<String>::Alloc(1);
            (*settingsToMove)[0] = ISettingsGlobal::MODE_RINGER;
            MoveSettingsToNewTable(db, TABLE_SYSTEM, TABLE_GLOBAL, settingsToMove, TRUE);

            db->SetTransactionSuccessful();
            // } finally {
            //     db.endTransaction();
            // }
            db->EndTransaction();
        }
        upgradeVersion = 92;
    }

    if (upgradeVersion == 92) {
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR IGNORE INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
        if (mUserHandle == IUserHandle::USER_OWNER) {
            // consider existing primary users to have made it through user setup
            // if the globally-scoped device-provisioned bit is set
            // (indicating they already made it through setup as primary)
            Int32 deviceProvisioned = GetInt32ValueFromTable(db, TABLE_GLOBAL,
                    ISettingsGlobal::DEVICE_PROVISIONED, 0);
            AutoPtr<IInteger32> integer;
            CInteger32::New(deviceProvisioned, (IInteger32**)&integer);
            LoadSetting(stmt, ISettingsSecure::USER_SETUP_COMPLETE, integer);
        }
        else {
            // otherwise use the default
            LoadBooleanSetting(stmt, ISettingsSecure::USER_SETUP_COMPLETE, R::bool_::def_user_setup_complete);
        }
        // } finally {
        //     if (stmt != null) stmt.close();
        // }
        if (stmt) stmt->Close();
        upgradeVersion = 93;
    }

    if (upgradeVersion == 93) {
        // Redo this step, since somehow it didn't work the first time for some users
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            AutoPtr<ISQLiteStatement> stmt;
            // try {
            // Migrate now-global settings
            AutoPtr< ArrayOf<String> > settingsToMove = HashsetToStringArray(CSettingsProvider::sSystemGlobalKeys);
            MoveSettingsToNewTable(db, TABLE_SYSTEM, TABLE_GLOBAL, settingsToMove, TRUE);
            settingsToMove = HashsetToStringArray(CSettingsProvider::sSecureGlobalKeys);
            MoveSettingsToNewTable(db, TABLE_SECURE, TABLE_GLOBAL, settingsToMove, TRUE);

            db->SetTransactionSuccessful();
            // } finally {
            //     db.endTransaction();
            //     if (stmt != null) stmt.close();
            // }
            db->EndTransaction();
            if (stmt) stmt->Close();
        }
        upgradeVersion = 94;
    }

    if (upgradeVersion == 94) {
        // Add wireless charging started sound setting
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            AutoPtr<ISQLiteStatement> stmt;
            // try {
            db->CompileStatement(String("INSERT OR REPLACE INTO global(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
            LoadStringSetting(stmt, ISettingsGlobal::WIRELESS_CHARGING_STARTED_SOUND,
                    R::string::def_wireless_charging_started_sound);
            db->SetTransactionSuccessful();
            // } finally {
            //     db.endTransaction();
            //     if (stmt != null) stmt.close();
            // }
            db->EndTransaction();
            if (stmt) stmt->Close();
        }
        upgradeVersion = 95;
    }

    // *** Remember to update DATABASE_VERSION above!

    if (upgradeVersion != currentVersion) {
        Logger::W(TAG, "Got stuck trying to upgrade from version %d, must wipe the settings provider", upgradeVersion);
        db->ExecSQL(String("DROP TABLE IF EXISTS global"));
        db->ExecSQL(String("DROP TABLE IF EXISTS globalIndex1"));
        db->ExecSQL(String("DROP TABLE IF EXISTS system"));
        db->ExecSQL(String("DROP INDEX IF EXISTS systemIndex1"));
        db->ExecSQL(String("DROP TABLE IF EXISTS secure"));
        db->ExecSQL(String("DROP INDEX IF EXISTS secureIndex1"));
        db->ExecSQL(String("DROP TABLE IF EXISTS gservices"));
        db->ExecSQL(String("DROP INDEX IF EXISTS gservicesIndex1"));
        db->ExecSQL(String("DROP TABLE IF EXISTS bluetooth_devices"));
        db->ExecSQL(String("DROP TABLE IF EXISTS bookmarks"));
        db->ExecSQL(String("DROP INDEX IF EXISTS bookmarksIndex1"));
        db->ExecSQL(String("DROP INDEX IF EXISTS bookmarksIndex2"));
        db->ExecSQL(String("DROP TABLE IF EXISTS favorites"));
        OnCreate(db);

        // Added for diagnosing settings.db wipes after the fact
        StringBuilder wipeReason;
        wipeReason += oldVersion;
        wipeReason += "/";
        wipeReason += upgradeVersion;
        wipeReason += "/";
        wipeReason += currentVersion;
        db->ExecSQL(String("INSERT INTO secure(name,value) values('wiped_db_reason','") + wipeReason.ToString() + String("');"));
    }
    return NOERROR;
}

AutoPtr< ArrayOf<String> > DatabaseHelper::HashsetToStringArray(
    /* [in] */ HashSet<String>& set)
{
    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(set.GetSize());
    HashSet<String>::Iterator it = set.Begin();
    for (Int32 i = 0; it != set.End(); ++it, ++i) {
        (*array)[i] = *it;
    }
    return array;
}

void DatabaseHelper::MoveSettingsToNewTable(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& sourceTable,
    /* [in] */ const String& destTable,
    /* [in] */ ArrayOf<String>* settingsToMove,
    /* [in] */ Boolean doIgnore)
{
    // Copy settings values from the source table to the dest, and remove from the source
    AutoPtr<ISQLiteStatement> insertStmt;
    AutoPtr<ISQLiteStatement> deleteStmt;

    db->BeginTransaction();
    // try {
    StringBuilder sb("INSERT ");
    sb += (doIgnore ? " OR IGNORE " : "");
    sb += " INTO ";
    sb += destTable;
    sb += " (name,value) SELECT name,value FROM ";
    sb += sourceTable;
    sb += " WHERE name=?";
    db->CompileStatement(sb.ToString(), (ISQLiteStatement**)&insertStmt);
    db->CompileStatement(String("DELETE FROM ") + sourceTable + String(" WHERE name=?"), (ISQLiteStatement**)&deleteStmt);

    for (Int32 i = 0; i < settingsToMove->GetLength(); ++i) {
        String setting = (*settingsToMove)[i];
        insertStmt->BindString(1, setting);
        insertStmt->Execute();

        deleteStmt->BindString(1, setting);
        deleteStmt->Execute();
    }
    db->SetTransactionSuccessful();
    // } finally {
    //     db.endTransaction();
    //     if (insertStmt != null) {
    //         insertStmt.close();
    //     }
    //     if (deleteStmt != null) {
    //         deleteStmt.close();
    //     }
    // }
    db->EndTransaction();
    if (insertStmt) insertStmt->Close();
    if (deleteStmt) deleteStmt->Close();
}

void DatabaseHelper::MovePrefixedSettingsToNewTable(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& sourceTable,
    /* [in] */ const String& destTable,
    /* [in] */ ArrayOf<String>* prefixesToMove)
{
    AutoPtr<ISQLiteStatement> insertStmt;
    AutoPtr<ISQLiteStatement> deleteStmt;

    db->BeginTransaction();
    // try {
    StringBuilder sb("INSERT INTO ");
    sb += destTable;
    sb += " (name,value) SELECT name,value FROM ";
    sb += sourceTable;
    sb += " WHERE substr(name,0,?)=?";
    db->CompileStatement(sb.ToString(), (ISQLiteStatement**)&insertStmt);
    db->CompileStatement(
            String("DELETE FROM ") + sourceTable + String(" WHERE substr(name,0,?)=?"), (ISQLiteStatement**)&deleteStmt);

    for (Int32 i = 0; i < prefixesToMove->GetLength(); ++i) {
        String prefix = (*prefixesToMove)[i];
        insertStmt->BindInt64(1, prefix.GetLength() + 1);
        insertStmt->BindString(2, prefix);
        insertStmt->Execute();

        deleteStmt->BindInt64(1, prefix.GetLength() + 1);
        deleteStmt->BindString(2, prefix);
        deleteStmt->Execute();
    }
    db->SetTransactionSuccessful();
    // } finally {
    //     db.endTransaction();
    //     if (insertStmt != null) {
    //         insertStmt.close();
    //     }
    //     if (deleteStmt != null) {
    //         deleteStmt.close();
    //     }
    // }
    db->EndTransaction();
    if (insertStmt) insertStmt->Close();
    if (deleteStmt) deleteStmt->Close();
}

ECode DatabaseHelper::UpgradeLockPatternLocation(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<ICursor> c;
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    (*args)[0] = String("_id");
    (*args)[1] = String("value");
    FAIL_RETURN(db->Query(TABLE_SYSTEM, args, String("name='lock_pattern'"),
            NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&c))
    Int32 count;
    if (c->GetCount(&count), count > 0) {
        Boolean result;
        c->MoveToFirst(&result);
        String lockPattern;
        c->GetString(1, &lockPattern);
        if (!TextUtils::IsEmpty(lockPattern)) {
            // Convert lock pattern
            // try {
            Logger::E(TAG, "TODO: LockPatternUtils and LockPatternView has not been export!!!!!");
            assert(0);
            // LockPatternUtils lpu = new LockPatternUtils(mContext);
            // List<LockPatternView.Cell> cellPattern =
            //         LockPatternUtils.stringToPattern(lockPattern);
            // lpu.saveLockPattern(cellPattern);
            // } catch (IllegalArgumentException e) {
            //     // Don't want corrupted lock pattern to hang the reboot process
            // }
        }
        c->Close();
        Int32 v;
        db->Delete(TABLE_SYSTEM, String("name='lock_pattern'"), NULL, &v);
    }
    else {
        c->Close();
    }
    return NOERROR;
}

ECode DatabaseHelper::UpgradeScreenTimeoutFromNever(
    /* [in] */ ISQLiteDatabase* db)
{
    // See if the timeout is -1 (for "Never").
    AutoPtr<ICursor> c;
    AutoPtr< ArrayOf<String> > args1 = ArrayOf<String>::Alloc(2);
    (*args1)[0] = String("_id");
    (*args1)[1] = String("value");
    AutoPtr< ArrayOf<String> > args2 = ArrayOf<String>::Alloc(2);
    (*args2)[0] = ISettingsSystem::SCREEN_OFF_TIMEOUT;
    (*args2)[1] = String("-1");
    FAIL_RETURN(db->Query(TABLE_SYSTEM, args1, String("name=? AND value=?"),
            args2, String(NULL), String(NULL), String(NULL), (ICursor**)&c))

    AutoPtr<ISQLiteStatement> stmt;
    Int32 count;
    if (c->GetCount(&count), count > 0) {
        c->Close();
        // try {
        db->CompileStatement(String("INSERT OR REPLACE INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);

        // Set the timeout to 30 minutes in milliseconds
        String str = StringUtils::ToString(30 * 60 * 1000);
        AutoPtr<ICharSequence> cs;
        CStringWrapper::New(str, (ICharSequence**)&cs);
        LoadSetting(stmt, ISettingsSystem::SCREEN_OFF_TIMEOUT, cs);
        // } finally {
        //     if (stmt != null) stmt.close();
        // }
        if (stmt) stmt->Close();
    }
    else {
        c->Close();
    }
    return NOERROR;
}

void DatabaseHelper::UpgradeVibrateSettingFromNone(
    /* [in] */ ISQLiteDatabase* db)
{
    Int32 vibrateSetting = GetInt32ValueFromSystem(db, ISettingsSystem::VIBRATE_ON, 0);
    // If the ringer vibrate value is invalid, set it to the default
    AutoPtr<IAudioServiceHelper> helper;
    CAudioServiceHelper::AcquireSingleton((IAudioServiceHelper**)&helper);
    if ((vibrateSetting & 3) == IAudioManager::VIBRATE_SETTING_OFF) {
        helper->GetValueForVibrateSetting(0,
                IAudioManager::VIBRATE_TYPE_RINGER, IAudioManager::VIBRATE_SETTING_ONLY_SILENT, &vibrateSetting);
    }
    // Apply the same setting to the notification vibrate value
    helper->GetValueForVibrateSetting(vibrateSetting,
            IAudioManager::VIBRATE_TYPE_NOTIFICATION, vibrateSetting, &vibrateSetting);

    AutoPtr<ISQLiteStatement> stmt;
    // try {
    db->CompileStatement(String("INSERT OR REPLACE INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
    AutoPtr<IInteger32> integer;
    CInteger32::New(vibrateSetting, (IInteger32**)&integer);
    LoadSetting(stmt, ISettingsSystem::VIBRATE_ON, integer);
    // } finally {
    //     if (stmt != null)
    //         stmt.close();
    // }
    if (stmt) stmt->Close();
}

void DatabaseHelper::UpgradeScreenTimeout(
    /* [in] */ ISQLiteDatabase* db)
{
    // Change screen timeout to current default
    db->BeginTransaction();
    AutoPtr<ISQLiteStatement> stmt;
    // try {
    db->CompileStatement(String("INSERT OR REPLACE INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
    LoadIntegerSetting(stmt, ISettingsSystem::SCREEN_OFF_TIMEOUT, R::integer::def_screen_off_timeout);
    db->SetTransactionSuccessful();
    // } finally {
    //     db.endTransaction();
    //     if (stmt != null)
    //         stmt.close();
    // }
    db->EndTransaction();
    if (stmt) stmt->Close();
}

void DatabaseHelper::UpgradeAutoBrightness(
    /* [in] */ ISQLiteDatabase* db)
{
    db->BeginTransaction();
    // try {
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean v;
    res->GetBoolean(R::bool_::def_screen_brightness_automatic_mode, &v);
    String value = v ? String("1") : String("0");
    StringBuilder sb("INSERT OR REPLACE INTO system(name,value) values('");
    sb += ISettingsSystem::SCREEN_BRIGHTNESS_MODE;
    sb += "','";
    sb += value;
    sb += "');";
    db->ExecSQL(sb.ToString());
    db->SetTransactionSuccessful();
    // } finally {
    //     db.endTransaction();
    // }
    db->EndTransaction();
}

ECode DatabaseHelper::LoadBookmarks(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);

    AutoPtr<IPackageManager> packageManager;
    mContext->GetPackageManager((IPackageManager**)&packageManager);
    // try {
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IXmlResourceParser> parser;
    FAIL_RETURN(res->GetXml(R::xml::bookmarks, (IXmlResourceParser**)&parser))
    ECode ec = XmlUtils::BeginDocument(parser, String("bookmarks"));
    if (FAILED(ec)) {
        Logger::W(TAG, "Got execption parsing bookmarks. 0x%08x", ec);
        return ec;
    }

    Int32 depth;
    parser->GetDepth(&depth);
    Int32 type, dep;
    while (((parser->Next(&type), type != IXmlPullParser::END_TAG) ||
            (parser->GetDepth(&dep), dep > depth)) && type != IXmlPullParser::END_DOCUMENT) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String name;
        parser->GetName(&name);
        if (!name.Equals("bookmark")) {
            break;
        }

        String pkg, cls, shortcutStr, category;
        parser->GetAttributeValue(String(NULL), String("package"), &pkg);
        parser->GetAttributeValue(String(NULL), String("class"), &cls);
        parser->GetAttributeValue(String(NULL), String("shortcut"), &shortcutStr);
        parser->GetAttributeValue(String(NULL), String("category"), &category);

        Int32 shortcutValue = shortcutStr.GetChar(0);
        if (TextUtils::IsEmpty(shortcutStr)) {
            Logger::W(TAG, "Unable to get shortcut for: %s/%s" , pkg.string(), cls.string());
            continue;
        }

        AutoPtr<IIntent> intent;
        String title;
        if (!pkg.IsNull() && !cls.IsNull()) {
            AutoPtr<IActivityInfo> info;
            AutoPtr<IComponentName> cn;
            CComponentName::New(pkg, cls, (IComponentName**)&cn);
            // try {
            if (FAILED(packageManager->GetActivityInfo(cn, 0, (IActivityInfo**)&info))) {
                AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
                (*args)[0] = pkg;
                AutoPtr< ArrayOf<String> > packages;
                packageManager->CanonicalToCurrentPackageNames(*args, (ArrayOf<String>**)&packages);
                cn = NULL;
                CComponentName::New((*packages)[0], cls, (IComponentName**)&cn);
                // try {
                info = NULL;
                if (FAILED(packageManager->GetActivityInfo(cn, 0, (IActivityInfo**)&info))) {
                    Logger::W(TAG, "Unable to add bookmark: %s/%s", pkg.string(), cls.string());
                    continue;
                }
                // } catch (PackageManager.NameNotFoundException e1) {
                //     Log.w(TAG, "Unable to add bookmark: " + pkg + "/" + cls, e);
                //     continue;
                // }
            }
            // } catch (PackageManager.NameNotFoundException e) {
            //     String[] packages = packageManager.canonicalToCurrentPackageNames(
            //             new String[] { pkg });
            //     cn = new ComponentName(packages[0], cls);
            //     try {
            //         info = packageManager.getActivityInfo(cn, 0);
            //     } catch (PackageManager.NameNotFoundException e1) {
            //         Log.w(TAG, "Unable to add bookmark: " + pkg + "/" + cls, e);
            //         continue;
            //     }
            // }

            CIntent::New(IIntent::ACTION_MAIN, NULL, (IIntent**)&intent);
            intent->AddCategory(IIntent::CATEGORY_LAUNCHER);
            intent->SetComponent(cn);
            AutoPtr<ICharSequence> cs;
            info->LoadLabel(packageManager, (ICharSequence**)&cs);
            cs->ToString(&title);
        }
        else if (!category.IsNull()) {
            AutoPtr<IIntentHelper> helper;
            CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
            helper->MakeMainSelectorActivity(IIntent::ACTION_MAIN, category, (IIntent**)&intent);
            title = String("");
        }
        else {
            Logger::W(TAG, "Unable to add bookmark for shortcut %s: missing package/class or category attributes", shortcutStr.string());
            continue;
        }

        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        String str;
        intent->ToUri(0, &str);
        AutoPtr<ICharSequence> cs1, cs2, cs3;
        CStringWrapper::New(str, (ICharSequence**)&cs1);
        CStringWrapper::New(title, (ICharSequence**)&cs2);
        CStringWrapper::New(shortcutStr, (ICharSequence**)&cs3);
        values->PutString(ISettingsBookmarks::INTENT, cs1);
        values->PutString(ISettingsBookmarks::TITLE, cs2);
        values->PutString(ISettingsBookmarks::SHORTCUT, cs3);
        AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
        (*args)[0] = StringUtils::ToString(shortcutValue);
        Int32 v;
        db->Delete(String("bookmarks"), String("shortcut = ?"), args, &v);
        Int64 result;
        db->Insert(String("bookmarks"), String(NULL), values, &result);
    }
    // } catch (XmlPullParserException e) {
    //     Log.w(TAG, "Got execption parsing bookmarks.", e);
    // } catch (IOException e) {
    //     Log.w(TAG, "Got execption parsing bookmarks.", e);
    // }
    return NOERROR;
}

void DatabaseHelper::LoadVolumeLevels(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<ISQLiteStatement> stmt;
    // try {
    db->CompileStatement(String("INSERT OR IGNORE INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);

    AutoPtr<IAudioManagerHelper> helper;
    CAudioManagerHelper::AcquireSingleton((IAudioManagerHelper**)&helper);
    AutoPtr< ArrayOf<Int32> > volume;
    helper->GetDefaultStreamVolume((ArrayOf<Int32>**)&volume);
    AutoPtr<IInteger32> integer1, integer2, integer3, integer4, integer5, integer6, integer7;
    CInteger32::New((*volume)[IAudioManager::STREAM_MUSIC], (IInteger32**)&integer1);
    CInteger32::New((*volume)[IAudioManager::STREAM_RING], (IInteger32**)&integer2);
    CInteger32::New((*volume)[IAudioManager::STREAM_SYSTEM], (IInteger32**)&integer3);
    CInteger32::New((*volume)[IAudioManager::STREAM_VOICE_CALL], (IInteger32**)&integer4);
    CInteger32::New((*volume)[IAudioManager::STREAM_ALARM], (IInteger32**)&integer5);
    CInteger32::New((*volume)[IAudioManager::STREAM_NOTIFICATION], (IInteger32**)&integer6);
    CInteger32::New((*volume)[IAudioManager::STREAM_BLUETOOTH_SCO], (IInteger32**)&integer7);
    LoadSetting(stmt, ISettingsSystem::VOLUME_MUSIC, integer1);
    LoadSetting(stmt, ISettingsSystem::VOLUME_RING, integer2);
    LoadSetting(stmt, ISettingsSystem::VOLUME_SYSTEM, integer3);
    LoadSetting(stmt, ISettingsSystem::VOLUME_VOICE, integer4);
    LoadSetting(stmt, ISettingsSystem::VOLUME_ALARM, integer5);
    LoadSetting(stmt, ISettingsSystem::VOLUME_NOTIFICATION, integer6);
    LoadSetting(stmt, ISettingsSystem::VOLUME_BLUETOOTH_SCO, integer7);

    // By default:
    // - ringtones, notification, system and music streams are affected by ringer mode
    // on non voice capable devices (tablets)
    // - ringtones, notification and system streams are affected by ringer mode
    // on voice capable devices (phones)
    Int32 ringerModeAffectedStreams = (1 << IAudioManager::STREAM_RING) |
            (1 << IAudioManager::STREAM_NOTIFICATION) |
            (1 << IAudioManager::STREAM_SYSTEM) |
            (1 << IAudioManager::STREAM_SYSTEM_ENFORCED);
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean value;
    if (res->GetBoolean(Elastos::Droid::R::bool_::config_voice_capable, &value), !value) {
        ringerModeAffectedStreams |= (1 << IAudioManager::STREAM_MUSIC);
    }
    AutoPtr<IInteger32> integer8;
    CInteger32::New(ringerModeAffectedStreams, (IInteger32**)&integer8);
    LoadSetting(stmt, ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED, integer8);

    AutoPtr<IInteger32> integer9;
    CInteger32::New(((1 << IAudioManager::STREAM_MUSIC) |
             (1 << IAudioManager::STREAM_RING) |
             (1 << IAudioManager::STREAM_NOTIFICATION) |
             (1 << IAudioManager::STREAM_SYSTEM)), (IInteger32**)&integer9);
    LoadSetting(stmt, ISettingsSystem::MUTE_STREAMS_AFFECTED, integer9);
    // } finally {
    //     if (stmt != null) stmt.close();
    // }
    if (stmt) stmt->Close();

    LoadVibrateWhenRingingSetting(db);
}

void DatabaseHelper::LoadVibrateSetting(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Boolean deleteOld)
{
    if (deleteOld) {
        db->ExecSQL(String("DELETE FROM system WHERE name='") + ISettingsSystem::VIBRATE_ON + String("'"));
    }

    AutoPtr<ISQLiteStatement> stmt;
    // try {
    db->CompileStatement(String("INSERT OR IGNORE INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);

    // Vibrate on by default for ringer, on for notification
    Int32 vibrate = 0;
    AutoPtr<IAudioServiceHelper> helper;
    CAudioServiceHelper::AcquireSingleton((IAudioServiceHelper**)&helper);
    helper->GetValueForVibrateSetting(vibrate, IAudioManager::VIBRATE_TYPE_NOTIFICATION,
            IAudioManager::VIBRATE_SETTING_ONLY_SILENT, &vibrate);
    Int32 temp;
    helper->GetValueForVibrateSetting(vibrate, IAudioManager::VIBRATE_TYPE_RINGER,
            IAudioManager::VIBRATE_SETTING_ONLY_SILENT, &temp);
    vibrate |= temp;
    AutoPtr<IInteger32> integer;
    CInteger32::New(vibrate, (IInteger32**)&integer);
    LoadSetting(stmt, ISettingsSystem::VIBRATE_ON, integer);
    // } finally {
    //     if (stmt != null) stmt.close();
    // }
    if (stmt) stmt->Close();
}

void DatabaseHelper::LoadVibrateWhenRingingSetting(
    /* [in] */ ISQLiteDatabase* db)
{
    // The default should be off. VIBRATE_SETTING_ONLY_SILENT should also be ignored here.
    // Phone app should separately check whether AudioManager#getRingerMode() returns
    // RINGER_MODE_VIBRATE, with which the device should vibrate anyway.
    Int32 vibrateSetting = GetInt32ValueFromSystem(db, ISettingsSystem::VIBRATE_ON,
            IAudioManager::VIBRATE_SETTING_OFF);
    Boolean vibrateWhenRinging = ((vibrateSetting & 3) == IAudioManager::VIBRATE_SETTING_ON);

    AutoPtr<ISQLiteStatement> stmt;
    // try {
    db->CompileStatement(String("INSERT OR IGNORE INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);
    AutoPtr<IInteger32> integer;
    CInteger32::New(vibrateWhenRinging ? 1 : 0, (IInteger32**)&integer);
    LoadSetting(stmt, ISettingsSystem::VIBRATE_WHEN_RINGING, integer);
    // } finally {
    //     if (stmt != null) stmt.close();
    // }
    if (stmt) stmt->Close();
}

void DatabaseHelper::LoadSettings(
    /* [in] */ ISQLiteDatabase* db)
{
    LoadSystemSettings(db);
    LoadSecureSettings(db);
    // The global table only exists for the 'owner' user
    if (mUserHandle == IUserHandle::USER_OWNER) {
        LoadGlobalSettings(db);
    }
}

void DatabaseHelper::LoadSystemSettings(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<ISQLiteStatement> stmt;
    // try {
    db->CompileStatement(String("INSERT OR IGNORE INTO system(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);

    LoadBooleanSetting(stmt, ISettingsSystem::DIM_SCREEN, R::bool_::def_dim_screen);
    LoadIntegerSetting(stmt, ISettingsSystem::SCREEN_OFF_TIMEOUT, R::integer::def_screen_off_timeout);
    // Set default cdma DTMF type
    AutoPtr<IInteger32> integer;
    CInteger32::New(0, (IInteger32**)&integer);
    LoadSetting(stmt, ISettingsSystem::DTMF_TONE_TYPE_WHEN_DIALING, integer);

    // Set default hearing aid
    LoadSetting(stmt, ISettingsSystem::HEARING_AID, integer);

    // Set default tty mode
    LoadSetting(stmt, ISettingsSystem::TTY_MODE, integer);

    LoadIntegerSetting(stmt, ISettingsSystem::SCREEN_BRIGHTNESS, R::integer::def_screen_brightness);

    LoadBooleanSetting(stmt, ISettingsSystem::SCREEN_BRIGHTNESS_MODE, R::bool_::def_screen_brightness_automatic_mode);

    LoadDefaultAnimationSettings(stmt);

    LoadBooleanSetting(stmt, ISettingsSystem::ACCELEROMETER_ROTATION, R::bool_::def_accelerometer_rotation);

    LoadDefaultHapticSettings(stmt);

    LoadBooleanSetting(stmt, ISettingsSystem::NOTIFICATION_LIGHT_PULSE, R::bool_::def_notification_pulse);

    LoadUISoundEffectsSettings(stmt);

    LoadIntegerSetting(stmt, ISettingsSystem::POINTER_SPEED, R::integer::def_pointer_speed);
    /* Below is the AW custom code */
    LoadStringSetting(stmt, ISettingsSystem::ACCELEROMETER_COORDINATE, R::string::def_accelerometer_coordinate);

    LoadBooleanSetting(stmt, ISettingsSystem::IS_SCAN_TF_CARD, R::bool_::def_is_scan_tf_card);

    LoadStringSetting(stmt, ISettingsSystem::TIME_12_24, R::string::def_time_12_24);
    LoadStringSetting(stmt, ISettingsSystem::DISPLAY_ADAPTION_MODE, R::string::def_screen_adaption_mode);

    LoadBooleanSetting(stmt,ISettingsSystem::IS_SCAN_USB_HOST, R::bool_::def_is_scan_usb_host);
    LoadBooleanSetting(stmt,ISettingsSystem::IS_SCAN_SATA, R::bool_::def_is_scan_sata);

    /* add by Gary. start {{----------------------------------- */
    /* 2011-11-1 */
    /* support shortcut keys with color keys for some specific websites and apps */
    LoadStringSetting(stmt, ISettingsSystem::SHORTCUT_KEY_0, R::string::def_shortcut_key_0);
    LoadStringSetting(stmt, ISettingsSystem::SHORTCUT_KEY_1, R::string::def_shortcut_key_1);
    LoadStringSetting(stmt, ISettingsSystem::SHORTCUT_KEY_2, R::string::def_shortcut_key_2);
    LoadStringSetting(stmt, ISettingsSystem::SHORTCUT_KEY_3, R::string::def_shortcut_key_3);
    /* add by Gary. end   -----------------------------------}} */

    /* add by huanglong */
    AutoPtr<IWindowManager> wm;
    mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&wm);
    AutoPtr<IDisplay> display;
    wm->GetDefaultDisplay((IDisplay**)&display);
    Int32 width, height;
    display->GetWidth(&width);
    display->GetHeight(&height);
    if ((width * 3.0 / (height * 5.0) != 1.0) && (width * 5.0 / (height * 3.0) != 1.0)) {
        LoadBooleanSetting(stmt, ISettingsSystem::DISPLAY_ADAPTION_ENABLE, R::bool_::def_display_adation_enable);
    }
    else {
        AutoPtr<IBoolean> b;
        CBoolean::New(FALSE, (IBoolean**)&b);
        LoadSetting(stmt, ISettingsSystem::DISPLAY_ADAPTION_ENABLE, b);
    }
    /* add by Gary. start {{----------------------------------- */
    /* 2011-12-6 */
    /* adjust the display area */
    LoadIntegerSetting(stmt, ISettingsSystem::DISPLAY_AREA_RATIO, R::integer::def_display_area_ratio);
    /* add by Gary. end   -----------------------------------}} */

    LoadBooleanSetting(stmt, ISettingsSystem::SMART_BRIGHTNESS_ENABLE, R::bool_::def_smart_brightness_enable);
    LoadBooleanSetting(stmt, ISettingsSystem::SMART_BRIGHTNESS_PREVIEW_ENABLE, R::bool_::def_smart_brightness_preview_enable);

    /* add by Gary. start {{----------------------------------- */
    /* 2011-12-10 */
    /* record the display format and the advance of the mouse in mouse mode */
    LoadStringSetting(stmt, ISettingsSystem::DISPLY_OUTPUT_FORMAT, R::string::def_display_output_format);
    LoadIntegerSetting(stmt, ISettingsSystem::MOUSE_ADVANCE, R::integer::def_mouse_advance);
    /* add by Gary. end   -----------------------------------}} */

    /* add by Gary. start {{----------------------------------- */
    /* 2011-12-11 */
    /* record the color parameters : brightness, contrast,and saturation */
    LoadIntegerSetting(stmt, ISettingsSystem::COLOR_BRIGHTNESS, R::integer::def_color_brightness);
    LoadIntegerSetting(stmt, ISettingsSystem::COLOR_CONTRAST, R::integer::def_color_contrast);
    LoadIntegerSetting(stmt, ISettingsSystem::COLOR_SATURATION, R::integer::def_color_saturation);
    /* add by Gary. end   -----------------------------------}} */

    /* add by Gary. start {{----------------------------------- */
    /* 2011-12-11 */
    /* record the audio output type */
    LoadStringSetting(stmt, ISettingsSystem::AUDIO_OUTPUT_TYPE, R::string::def_audio_output_type);
    /* add by Gary. end   -----------------------------------}} */

    /* add by Gary. start {{----------------------------------- */
    /* 2012-2-27 */
    /* record the audio output channel */
    LoadStringSetting(stmt, ISettingsSystem::AUDIO_OUTPUT_CHANNEL, R::string::def_audio_output_channel);
    /* add by Gary. end   -----------------------------------}} */

    /* add by Gary. start {{----------------------------------- */
    /* 2011-12-17 */
    /* directly power off when long press on the power key */
    LoadBooleanSetting(stmt, ISettingsSystem::DIRECTLY_POWER_OFF, R::bool_::def_directly_power_off);
    /* add by Gary. end   -----------------------------------}} */

    /* add by Gary. start {{----------------------------------- */
    /* 2012-4-27 */
    /* add a switch to control BD folder play mode */
    LoadBooleanSetting(stmt, ISettingsSystem::BD_FOLDER_PLAY_MODE, R::bool_::def_bd_folder_play_mode);
    /* add by Gary. end   -----------------------------------}} */
    LoadIntegerSetting(stmt, ISettingsSystem::HDMI_OUTPUT_MODE, R::integer::def_hdmi_output_mode);
    LoadBooleanSetting(stmt, ISettingsSystem::BOOT_FAST_ENABLE, R::bool_::def_boot_fast_enable);
    LoadBooleanSetting(stmt, ISettingsSystem::ENABLE_PASS_THROUGH, R::bool_::def_enable_pass_through);
    // } finally {
    //     if (stmt != null) stmt.close();
    // }
    if (stmt) stmt->Close();
}

void DatabaseHelper::LoadUISoundEffectsSettings(
    /* [in] */ ISQLiteStatement* stmt)
{
    LoadBooleanSetting(stmt, ISettingsSystem::DTMF_TONE_WHEN_DIALING, R::bool_::def_dtmf_tones_enabled);
    LoadBooleanSetting(stmt, ISettingsSystem::SOUND_EFFECTS_ENABLED, R::bool_::def_sound_effects_enabled);
    LoadBooleanSetting(stmt, ISettingsSystem::HAPTIC_FEEDBACK_ENABLED, R::bool_::def_haptic_feedback);

    LoadIntegerSetting(stmt, ISettingsSystem::LOCKSCREEN_SOUNDS_ENABLED, R::integer::def_lockscreen_sounds_enabled);
}

void DatabaseHelper::LoadDefaultAnimationSettings(
    /* [in] */ ISQLiteStatement* stmt)
{
    LoadFractionSetting(stmt, ISettingsSystem::WINDOW_ANIMATION_SCALE, R::fraction::def_window_animation_scale, 1);
    LoadFractionSetting(stmt, ISettingsSystem::TRANSITION_ANIMATION_SCALE, R::fraction::def_window_transition_scale, 1);
}

void DatabaseHelper::LoadDefaultHapticSettings(
    /* [in] */ ISQLiteStatement* stmt)
{
    LoadBooleanSetting(stmt, ISettingsSystem::HAPTIC_FEEDBACK_ENABLED, R::bool_::def_haptic_feedback);
}

void DatabaseHelper::LoadSecureSettings(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<ISQLiteStatement> stmt;
    // try {
    db->CompileStatement(String("INSERT OR IGNORE INTO secure(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);

    LoadStringSetting(stmt, ISettingsSecure::LOCATION_PROVIDERS_ALLOWED, R::string::def_location_providers_allowed);


    /* add by chenjd, start {{-------------------------*/
    /* 2012-11-15 */
    /* load default wifi country code*/
    LoadStringSetting(stmt, ISettingsSecure::WIFI_COUNTRY_CODE, R::string::wifi_country_code);

    /* add by Gary. start {{----------------------------------- */
    /* 2012-1-6 */
    /* init the defalut value of the ethernet */
    LoadBooleanSetting(stmt, ISettingsSecure::ETHERNET_ON, R::bool_::def_ethernet_on);
    LoadBooleanSetting(stmt, ISettingsSecure::ETHERNET_CONF, R::bool_::def_ethernet_conf);
    LoadBooleanSetting(stmt, ISettingsSecure::ETHERNET_MODE, R::bool_::def_ethernet_mode);
    LoadStringSetting(stmt, ISettingsSecure::ETHERNET_IFNAME, R::string::def_ethernet_name);
    /* add by Gary. end   -----------------------------------}} */


    /* add by shanxiaoxi@allwinnertech.com, start {{-------------------------*/
    /* 2013-10-14 */
    /* load default wifi cheat option*/
    LoadBooleanSetting(stmt, ISettingsSecure::WIFI_CHEAT_ON, R::bool_::def_wifi_cheat_on);

    AutoPtr<ISystemProperties> prop;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&prop);
    String wifiWatchList;
    prop->Get(String("ro.com.android.wifi-watchlist"), &wifiWatchList);
    if (!TextUtils::IsEmpty(wifiWatchList)) {
        AutoPtr<ICharSequence> cs;
        CStringWrapper::New(wifiWatchList, (ICharSequence**)&cs);
        LoadSetting(stmt, ISettingsSecure::WIFI_WATCHDOG_WATCH_LIST, cs);
    }

    // Don't do this.  The SystemServer will initialize ADB_ENABLED from a
    // persistent system property instead.
    //LoadSetting(stmt, ISettingsSecure::ADB_ENABLED, 0);

    // Allow mock locations default, based on build
    String location;
    prop->Get(String("ro.allow.mock.location"), &location);
    AutoPtr<IInteger32> integer;
    CInteger32::New(location.Equals("1") ? 1 : 0, (IInteger32**)&integer);
    LoadSetting(stmt, ISettingsSecure::ALLOW_MOCK_LOCATION, integer);

    LoadSecure35Settings(stmt);

    LoadBooleanSetting(stmt, ISettingsSecure::MOUNT_PLAY_NOTIFICATION_SND, R::bool_::def_mount_play_notification_snd);

    LoadBooleanSetting(stmt, ISettingsSecure::MOUNT_UMS_AUTOSTART, R::bool_::def_mount_ums_autostart);

    LoadBooleanSetting(stmt, ISettingsSecure::MOUNT_UMS_PROMPT, R::bool_::def_mount_ums_prompt);

    LoadBooleanSetting(stmt, ISettingsSecure::MOUNT_UMS_NOTIFY_ENABLED, R::bool_::def_mount_ums_notify_enabled);

    LoadBooleanSetting(stmt, ISettingsSecure::ACCESSIBILITY_SCRIPT_INJECTION, R::bool_::def_accessibility_script_injection);

    LoadStringSetting(stmt, ISettingsSecure::ACCESSIBILITY_WEB_CONTENT_KEY_BINDINGS, R::string::def_accessibility_web_content_key_bindings);

    LoadIntegerSetting(stmt, ISettingsSecure::LONG_PRESS_TIMEOUT, R::integer::def_long_press_timeout_millis);

    LoadBooleanSetting(stmt, ISettingsSecure::TOUCH_EXPLORATION_ENABLED, R::bool_::def_touch_exploration_enabled);

    LoadBooleanSetting(stmt, ISettingsSecure::ACCESSIBILITY_SPEAK_PASSWORD, R::bool_::def_accessibility_speak_password);

    LoadStringSetting(stmt, ISettingsSecure::ACCESSIBILITY_SCREEN_READER_URL, R::string::def_accessibility_screen_reader_url);

    Boolean disable;
    if (prop->GetBoolean(String("ro.lockscreen.disable.default"), FALSE, &disable), disable) {
        AutoPtr<ICharSequence> cs;
        CStringWrapper::New(String("1"), (ICharSequence**)&cs);
        LoadSetting(stmt, ISettingsSystem::LOCKSCREEN_DISABLED, cs);
    }
    else {
        LoadBooleanSetting(stmt, ISettingsSystem::LOCKSCREEN_DISABLED, R::bool_::def_lockscreen_disabled);
    }

    LoadBooleanSetting(stmt, ISettingsSecure::SCREENSAVER_ENABLED, Elastos::Droid::R::bool_::config_dreamsEnabledByDefault);
    LoadBooleanSetting(stmt, ISettingsSecure::SCREENSAVER_ACTIVATE_ON_DOCK, Elastos::Droid::R::bool_::config_dreamsActivatedOnDockByDefault);
    LoadBooleanSetting(stmt, ISettingsSecure::SCREENSAVER_ACTIVATE_ON_SLEEP, Elastos::Droid::R::bool_::config_dreamsActivatedOnSleepByDefault);
    LoadStringSetting(stmt, ISettingsSecure::SCREENSAVER_COMPONENTS, Elastos::Droid::R::string::config_dreamsDefaultComponent);
    LoadStringSetting(stmt, ISettingsSecure::SCREENSAVER_DEFAULT_COMPONENT, Elastos::Droid::R::string::config_dreamsDefaultComponent);

    LoadBooleanSetting(stmt, ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_ENABLED,
            R::bool_::def_accessibility_display_magnification_enabled);

    LoadFractionSetting(stmt, ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_SCALE,
            R::fraction::def_accessibility_display_magnification_scale, 1);

    LoadBooleanSetting(stmt, ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_AUTO_UPDATE,
            R::bool_::def_accessibility_display_magnification_auto_update);

    LoadBooleanSetting(stmt, ISettingsSecure::USER_SETUP_COMPLETE, R::bool_::def_user_setup_complete);

    /* add by Gary. start {{----------------------------------- */
    /* 2011-12-25 */
    /* init the default input method */
    LoadStringSetting(stmt, ISettingsSecure::DEFAULT_INPUT_METHOD, R::string::def_input_method);
    /* add by Gary. end   -----------------------------------}} */

    /* add by huanglong ,init the pppoe value*/
    LoadStringSetting(stmt,ISettingsSecure::PPPOE_INTERFACE, R::string::def_pppoe_interface);
    LoadBooleanSetting(stmt,ISettingsSecure::PPPOE_AUTO_CONN, R::bool_::def_pppoe_auto_conn);
    LoadBooleanSetting(stmt,ISettingsSecure::PPPOE_ENABLE, R::bool_::def_pppoe_enable);
    //add by kinier
    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(String("1"), (ICharSequence**)&cs);
    LoadSetting(stmt, String("system_work_mode"), cs);
    // } finally {
    //     if (stmt != null) stmt.close();
    // }
    if (stmt) stmt->Close();
}

void DatabaseHelper::LoadSecure35Settings(
    /* [in] */ ISQLiteStatement* stmt)
{
    LoadBooleanSetting(stmt, ISettingsSecure::BACKUP_ENABLED, R::bool_::def_backup_enabled);

    LoadStringSetting(stmt, ISettingsSecure::BACKUP_TRANSPORT, R::string::def_backup_transport);
}

void DatabaseHelper::LoadGlobalSettings(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<ISQLiteStatement> stmt;
    // try {
    db->CompileStatement(String("INSERT OR IGNORE INTO global(name,value) VALUES(?,?);"), (ISQLiteStatement**)&stmt);

    // --- Previously in 'system'
    LoadBooleanSetting(stmt, ISettingsGlobal::AIRPLANE_MODE_ON, R::bool_::def_airplane_mode_on);

    LoadStringSetting(stmt, ISettingsGlobal::AIRPLANE_MODE_RADIOS, R::string::def_airplane_mode_radios);

    LoadStringSetting(stmt, ISettingsGlobal::AIRPLANE_MODE_TOGGLEABLE_RADIOS, R::string::airplane_mode_toggleable_radios);

    LoadBooleanSetting(stmt, ISettingsGlobal::ASSISTED_GPS_ENABLED, R::bool_::assisted_gps_enabled);

    LoadBooleanSetting(stmt, ISettingsGlobal::AUTO_TIME, R::bool_::def_auto_time); // Sync time to NITZ

    LoadBooleanSetting(stmt, ISettingsGlobal::AUTO_TIME_ZONE, R::bool_::def_auto_time_zone); // Sync timezone to NITZ

    AutoPtr<ISystemProperties> prop;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&prop);
    String qemu;
    prop->Get(String("ro.kernel.qemu"), &qemu);
    Int32 v;
    if (qemu.Equals("1")) {
        v = 1;
    }
    else {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Boolean value;
        res->GetBoolean(R::bool_::def_stay_on_while_plugged_in, &value);
        if (value) v = 1;
        else v = 0;
    }
    AutoPtr<IInteger32> integer1;
    CInteger32::New(v, (IInteger32**)&integer1);

    LoadSetting(stmt, ISettingsGlobal::STAY_ON_WHILE_PLUGGED_IN, integer1);

    LoadIntegerSetting(stmt, ISettingsGlobal::WIFI_SLEEP_POLICY, R::integer::def_wifi_sleep_policy);

    AutoPtr<IInteger32> integer2;
    CInteger32::New(IAudioManager::RINGER_MODE_NORMAL, (IInteger32**)&integer2);
    LoadSetting(stmt, ISettingsGlobal::MODE_RINGER, integer2);

    // --- Previously in 'secure'
    LoadBooleanSetting(stmt, ISettingsGlobal::PACKAGE_VERIFIER_ENABLE, R::bool_::def_package_verifier_enable);

    LoadBooleanSetting(stmt, ISettingsGlobal::WIFI_ON, R::bool_::def_wifi_on);

    LoadBooleanSetting(stmt, ISettingsGlobal::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON,
            R::bool_::def_networks_available_notification_on);

    LoadBooleanSetting(stmt, ISettingsGlobal::BLUETOOTH_ON, R::bool_::def_bluetooth_on);

    // Enable or disable Cell Broadcast SMS
    Logger::E(TAG, "TODO: RILConstants.CDMA_CELL_BROADCAST_SMS_DISABLED has not been defined!!!");
    // LoadSetting(stmt, ISettingsGlobal::CDMA_CELL_BROADCAST_SMS,
    //         RILConstants.CDMA_CELL_BROADCAST_SMS_DISABLED);

    // Data roaming default, based on build
    String dataroaming;
    prop->Get(String("ro.com.android.dataroaming"), String("false"), &dataroaming);
    AutoPtr<IInteger32> integer3;
    CInteger32::New(dataroaming.EqualsIgnoreCase("true")? 1 : 0, (IInteger32**)&integer3);
    LoadSetting(stmt, ISettingsGlobal::DATA_ROAMING, integer3);

    LoadBooleanSetting(stmt, ISettingsGlobal::DEVICE_PROVISIONED, R::bool_::def_device_provisioned);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 maxBytes;
    res->GetInteger(R::integer::def_download_manager_max_bytes_over_mobile, &maxBytes);
    if (maxBytes > 0) {
        AutoPtr<ICharSequence> cs;
        CStringWrapper::New(StringUtils::Int32ToString(maxBytes), (ICharSequence**)&cs);
        LoadSetting(stmt, ISettingsGlobal::DOWNLOAD_MAX_BYTES_OVER_MOBILE, cs);
    }

    Int32 recommendedMaxBytes;
    res->GetInteger(R::integer::def_download_manager_recommended_max_bytes_over_mobile, &recommendedMaxBytes);
    if (recommendedMaxBytes > 0) {
        AutoPtr<ICharSequence> cs;
        CStringWrapper::New(StringUtils::Int32ToString(recommendedMaxBytes), (ICharSequence**)&cs);
        LoadSetting(stmt, ISettingsGlobal::DOWNLOAD_RECOMMENDED_MAX_BYTES_OVER_MOBILE, cs);
    }

    // Mobile Data default, based on build
    String mobiledata;
    prop->Get(String("ro.com.android.mobiledata"), String("true"), &mobiledata);
    AutoPtr<IInteger32> integer4;
    CInteger32::New(mobiledata.EqualsIgnoreCase("true") ? 1 : 0, (IInteger32**)&integer4);
    LoadSetting(stmt, ISettingsGlobal::MOBILE_DATA, integer4);

    LoadBooleanSetting(stmt, ISettingsGlobal::NETSTATS_ENABLED, R::bool_::def_netstats_enabled);

    LoadBooleanSetting(stmt, ISettingsGlobal::INSTALL_NON_MARKET_APPS, R::bool_::def_install_non_market_apps);

    LoadIntegerSetting(stmt, ISettingsGlobal::NETWORK_PREFERENCE, R::integer::def_network_preference);

    LoadBooleanSetting(stmt, ISettingsGlobal::USB_MASS_STORAGE_ENABLED, R::bool_::def_usb_mass_storage_enabled);

    LoadIntegerSetting(stmt, ISettingsGlobal::WIFI_MAX_DHCP_RETRY_COUNT, R::integer::def_max_dhcp_retries);

    LoadBooleanSetting(stmt, ISettingsGlobal::WIFI_DISPLAY_ON, R::bool_::def_wifi_display_on);

    LoadStringSetting(stmt, ISettingsGlobal::LOCK_SOUND, R::string::def_lock_sound);
    LoadStringSetting(stmt, ISettingsGlobal::UNLOCK_SOUND, R::string::def_unlock_sound);
    LoadIntegerSetting(stmt, ISettingsGlobal::POWER_SOUNDS_ENABLED, R::integer::def_power_sounds_enabled);
    LoadStringSetting(stmt, ISettingsGlobal::LOW_BATTERY_SOUND, R::string::def_low_battery_sound);
    LoadIntegerSetting(stmt, ISettingsGlobal::DOCK_SOUNDS_ENABLED, R::integer::def_dock_sounds_enabled);
    LoadStringSetting(stmt, ISettingsGlobal::DESK_DOCK_SOUND, R::string::def_desk_dock_sound);
    LoadStringSetting(stmt, ISettingsGlobal::DESK_UNDOCK_SOUND, R::string::def_desk_undock_sound);
    LoadStringSetting(stmt, ISettingsGlobal::CAR_DOCK_SOUND, R::string::def_car_dock_sound);
    LoadStringSetting(stmt, ISettingsGlobal::CAR_UNDOCK_SOUND, R::string::def_car_undock_sound);
    LoadStringSetting(stmt, ISettingsGlobal::WIRELESS_CHARGING_STARTED_SOUND,
            R::string::def_wireless_charging_started_sound);

    AutoPtr<IInteger32> integer5;
    CInteger32::New(0, (IInteger32**)&integer5);
    LoadSetting(stmt, ISettingsGlobal::SET_INSTALL_LOCATION, integer5);
    AutoPtr<IInteger32> integer6;
    CInteger32::New(IPackageHelper::APP_INSTALL_INTERNAL, (IInteger32**)&integer6);
    LoadSetting(stmt, ISettingsGlobal::DEFAULT_INSTALL_LOCATION, integer6);

    // Set default cdma emergency tone
    AutoPtr<IInteger32> integer7;
    CInteger32::New(0, (IInteger32**)&integer7);
    LoadSetting(stmt, ISettingsGlobal::EMERGENCY_TONE, integer7);

    // Set default cdma call auto retry
    LoadSetting(stmt, ISettingsGlobal::CALL_AUTO_RETRY, integer7);

    // Set the preferred network mode to 0 = Global, CDMA default
    // Int32 type;
    // if (TelephonyManager.getLteOnCdmaModeStatic() == PhoneConstants.LTE_ON_CDMA_TRUE) {
    //     type = Phone.NT_MODE_GLOBAL;
    // }
    // else {
    //     type = SystemProperties.getInt("ro.telephony.default_network",
    //             RILConstants.PREFERRED_NETWORK_MODE);
    // }
    Logger::E(TAG, "TODO: default_network type has not been defined!!!");
    LoadSetting(stmt, ISettingsGlobal::PREFERRED_NETWORK_MODE, integer7/*type*/);

    // --- New global settings start here
    // } finally {
    //     if (stmt != null) stmt.close();
    // }
    if (stmt) stmt->Close();
}

void DatabaseHelper::LoadSetting(
    /* [in] */ ISQLiteStatement* stmt,
    /* [in] */ const String& key,
    /* [in] */ IInterface* value)
{
    assert(stmt != NULL);
    stmt->BindString(1, key);
    String str;
    if (INumber::Probe(value) != NULL) {
        INumber::Probe(value)->ToString(&str);
    }
    else if (ICharSequence::Probe(value) != NULL) {
        ICharSequence::Probe(value)->ToString(&str);
    }
    stmt->BindString(2, str);
    stmt->Execute();
}

void DatabaseHelper::LoadStringSetting(
    /* [in] */ ISQLiteStatement* stmt,
    /* [in] */ const String& key,
    /* [in] */ Int32 resid)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    String str;
    res->GetString(resid, &str);
    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(str, (ICharSequence**)&cs);
    LoadSetting(stmt, key, cs);
}

void DatabaseHelper::LoadBooleanSetting(
    /* [in] */ ISQLiteStatement* stmt,
    /* [in] */ const String& key,
    /* [in] */ Int32 resid)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean value;
    res->GetBoolean(resid, &value);
    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(value ? String("1") : String("0"), (ICharSequence**)&cs);
    LoadSetting(stmt, key, cs);
}

void DatabaseHelper::LoadIntegerSetting(
    /* [in] */ ISQLiteStatement* stmt,
    /* [in] */ const String& key,
    /* [in] */ Int32 resid)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 value;
    res->GetInteger(resid, &value);
    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(StringUtils::Int32ToString(value), (ICharSequence**)&cs);
    LoadSetting(stmt, key, cs);
}

void DatabaseHelper::LoadFractionSetting(
    /* [in] */ ISQLiteStatement* stmt,
    /* [in] */ const String& key,
    /* [in] */ Int32 resid,
    /* [in] */ Int32 base)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Float value;
    res->GetFraction(resid, base, base, &value);
    AutoPtr<IFloat> f;
    CFloat::New(value, (IFloat**)&f);
    LoadSetting(stmt, key, f);
}

Int32 DatabaseHelper::GetInt32ValueFromSystem(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& name,
    /* [in] */ Int32 defaultValue)
{
    return GetInt32ValueFromTable(db, TABLE_SYSTEM, name, defaultValue);
}

Int32 DatabaseHelper::GetInt32ValueFromTable(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [in] */ const String& name,
    /* [in] */ Int32 defaultValue)
{
    String value = GetStringValueFromTable(db, table, name, String(NULL));
    return value.IsNull() ? defaultValue : StringUtils::ParseInt32(value);
}

String DatabaseHelper::GetStringValueFromTable(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [in] */ const String& name,
    /* [in] */ const String& defaultValue)
{
    AutoPtr<ICursor> c;
    // try {
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = ISettingsSystem::VALUE;
    db->Query(table, args, String("name='") + name + String("'"),
            NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&c);
    Boolean result;
    if (c != NULL && (c->MoveToFirst(&result), result)) {
        String val;
        c->GetString(0, &val);
        return val.IsNull() ? defaultValue : val;
    }
    // } finally {
    //     if (c != null) c.close();
    // }
    return defaultValue;
}

ECode DatabaseHelper::GetDatabaseName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    return SQLiteOpenHelper::GetDatabaseName(name);
}

ECode DatabaseHelper::SetWriteAheadLoggingEnabled(
    /* [in] */ Boolean enabled)
{
    return SQLiteOpenHelper::SetWriteAheadLoggingEnabled(enabled);
}

ECode DatabaseHelper::GetWritableDatabase(
    /* [out] */ ISQLiteDatabase** database)
{
    VALIDATE_NOT_NULL(database)
    return SQLiteOpenHelper::GetWritableDatabase(database);
}

ECode DatabaseHelper::GetReadableDatabase(
    /* [out] */ ISQLiteDatabase** database)
{
    VALIDATE_NOT_NULL(database)
    return SQLiteOpenHelper::GetReadableDatabase(database);
}

ECode DatabaseHelper::Close()
{
    return SQLiteOpenHelper::Close();
}

ECode DatabaseHelper::OnConfigure(
    /* [in] */ ISQLiteDatabase* db)
{
    return SQLiteOpenHelper::OnConfigure(db);
}

ECode DatabaseHelper::OnDowngrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    return SQLiteOpenHelper::OnDowngrade(db, oldVersion, newVersion);
}

ECode DatabaseHelper::OnOpen(
    /* [in] */ ISQLiteDatabase* db)
{
    return SQLiteOpenHelper::OnOpen(db);
}

} // namespace SettingsProvider
} // namespace Packages
} // namespace Droid
} // namespace Elastos
