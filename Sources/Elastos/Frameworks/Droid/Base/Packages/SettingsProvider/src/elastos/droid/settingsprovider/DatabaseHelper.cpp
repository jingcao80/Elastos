
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Provider.h>
#include "elastos/droid/settingsprovider/DatabaseHelper.h"
#include "elastos/droid/settingsprovider/SettingsProvider.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/text/TextUtils.h"
#include "R.h"
#include <elastos/droid/R.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

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
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Database::Sqlite::EIID_ISQLiteOpenHelper;
using Elastos::Droid::Database::Sqlite::ISQLiteProgram;
using Elastos::Droid::Internal::Content::IPackageHelper;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::ILockPatternUtilsHelper;
using Elastos::Droid::Internal::Widget::CLockPatternUtilsHelper;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioManagerHelper;
// using Elastos::Droid::Media::CAudioManagerHelper;
using Elastos::Droid::Media::IAudioServiceHelper;
using Elastos::Droid::Media::CAudioServiceHelper;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::Build;
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
using Elastos::Droid::Provider::ISettingsNameValueTable;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManager;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::INumber;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SettingsProvider {

const String DatabaseHelper::TAG("SettingsProvider");
const String DatabaseHelper::DATABASE_NAME("settings.db");
const Int32 DatabaseHelper::DATABASE_VERSION = 113;

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
    SQLiteOpenHelper::constructor(context, DbNameForUser(userHandle), NULL, DATABASE_VERSION);
}

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
    Logger::W(TAG, "Upgrading settings database from version %d to %d",
            oldVersion, currentVersion);

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
        db->EndTransaction();
        // }
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
        db->EndTransaction();
        // }
        upgradeVersion = 25;
    }

    if (upgradeVersion == 25) {
        db->BeginTransaction();
        // try {
        db->ExecSQL(String("ALTER TABLE favorites ADD uri TEXT"));
        db->ExecSQL(String("ALTER TABLE favorites ADD displayMode INTEGER"));
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        // }
        upgradeVersion = 26;
    }

    if (upgradeVersion == 26) {
        // This introduces the new secure settings table.
        db->BeginTransaction();
        // try {
        CreateSecureTable(db);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        // }
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
        StringBuilder builder;
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED;
        builder += "'";
        db->ExecSQL(builder.ToString());
        Int32 newValue = (1 << IAudioManager::STREAM_RING)
                | (1 << IAudioManager::STREAM_NOTIFICATION)
                | (1 << IAudioManager::STREAM_SYSTEM);
        builder.Reset();
        builder += "INSERT INTO system ('name', 'value') values ('";
        builder += ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED;
        builder += "', '";
        builder += newValue;
        builder += "')";
        db->ExecSQL(builder.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        // }

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
        db->EndTransaction();
        // }
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
        StringBuilder builder;
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::WINDOW_ANIMATION_SCALE;
        builder += "'";
        db->ExecSQL(builder.ToString());

        builder.Reset();
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::TRANSITION_ANIMATION_SCALE;
        builder += "'";
        db->ExecSQL(builder.ToString());
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadDefaultAnimationSettings(stmt);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
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
            sb += ISettingsSecure::WIFI_WATCHDOG_WATCH_LIST;
            sb += "','";
            sb += wifiWatchList;
            sb += "');";
            db->ExecSQL(sb.ToString());
            db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            // }
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
        db->EndTransaction();
        // }
        upgradeVersion = 34;
    }

    if (upgradeVersion == 34) {
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR IGNORE INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadSecure35Settings(stmt);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
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
        StringBuilder builder;
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED;
        builder += "'";
        db->ExecSQL(builder.ToString());
        Int32 newValue = (1 << IAudioManager::STREAM_RING)
                | (1 << IAudioManager::STREAM_NOTIFICATION)
                | (1 << IAudioManager::STREAM_SYSTEM)
                | (1 << IAudioManager::STREAM_SYSTEM_ENFORCED);
        builder.Reset();
        builder += "INSERT INTO system ('name', 'value') values ('";
        builder += ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED;
        builder += "', '";
        builder += newValue;
        builder += "')";
        db->ExecSQL(builder.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        // }
        upgradeVersion = 37;
    }

    if (upgradeVersion == 37) {
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR IGNORE INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadStringSetting(stmt, ISettingsSystem::AIRPLANE_MODE_TOGGLEABLE_RADIOS,
                R::string::airplane_mode_toggleable_radios);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
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
        db->EndTransaction();
        // }

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
        StringBuilder builder;
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::WINDOW_ANIMATION_SCALE;
        builder += "'";
        db->ExecSQL(builder.ToString());

        builder.Reset();
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::TRANSITION_ANIMATION_SCALE;
        builder += "'";
        db->ExecSQL(builder.ToString());
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadDefaultAnimationSettings(stmt);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 41;
    }

    if (upgradeVersion == 41) {
        /*
         * Initialize newly public haptic feedback setting
         */
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        StringBuilder builder;
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::HAPTIC_FEEDBACK_ENABLED;
        builder += "'";
        db->ExecSQL(builder.ToString());
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadDefaultHapticSettings(stmt);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 42;
    }

    if (upgradeVersion == 42) {
        /*
         * Initialize new notification pulse setting
         */
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSystem::NOTIFICATION_LIGHT_PULSE,
                R::bool_::def_notification_pulse);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 43;
    }

    if (upgradeVersion == 43) {
        /*
         * This upgrade stores bluetooth volume separately from voice volume
         */
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR IGNORE INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        AutoPtr<IAudioManagerHelper> helper;
        assert(0 && "TODO");
        // CAudioManagerHelper::AcquireSingleton((IAudioManagerHelper**)&helper);
        AutoPtr< ArrayOf<Int32> > volume;
        // helper->GetDefaultStreamVolume((ArrayOf<Int32>**)&volume);
        LoadSetting(stmt, ISettingsSystem::VOLUME_BLUETOOTH_SCO,
                CoreUtils::Convert((*volume)[IAudioManager::STREAM_BLUETOOTH_SCO]));
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
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
        StringBuilder builder;
        builder += "INSERT INTO secure(name,value) values('";
        builder += ISettingsSecure::MOUNT_PLAY_NOTIFICATION_SND;
        builder += "','1');";
        db->ExecSQL(builder.ToString());

        builder.Reset();
        builder += "INSERT INTO secure(name,value) values('";
        builder += ISettingsSecure::MOUNT_UMS_AUTOSTART;
        builder += "','0');";
        db->ExecSQL(builder.ToString());

        builder.Reset();
        builder += "INSERT INTO secure(name,value) values('";
        builder += ISettingsSecure::MOUNT_UMS_PROMPT;
        builder += "','1');";
        db->ExecSQL(builder.ToString());

        builder.Reset();
        builder += "INSERT INTO secure(name,value) values('";
        builder += ISettingsSecure::MOUNT_UMS_NOTIFY_ENABLED;
        builder += "','1');";
        db->ExecSQL(builder.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        // }
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
        db->EndTransaction();
        // }
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
        db->EndTransaction();
        // }
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
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadUISoundEffectsSettings(stmt);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }

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
        (*settingsToMove)[3] = "lockscreen.password_type";
        (*settingsToMove)[4] = "lockscreen.lockoutattemptdeadline";
        (*settingsToMove)[5] = "lockscreen.patterneverchosen";
        (*settingsToMove)[6] = "lock_pattern_autolock";
        (*settingsToMove)[7] = "lockscreen.lockedoutpermanently";
        (*settingsToMove)[8] = "lockscreen.password_salt";

        MoveSettingsToNewTable(db, TABLE_SYSTEM, TABLE_SECURE, settingsToMove, FALSE);
        upgradeVersion = 52;
   }

    if (upgradeVersion == 52) {
        // new vibration/silent mode settings
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSystem::VIBRATE_IN_SILENT,
                R::bool_::def_vibrate_in_silent);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }

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
        db->EndTransaction();
        // }

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
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadSetting(stmt, ISettingsGlobal::SET_INSTALL_LOCATION,
                CoreUtils::Convert(0));
        LoadSetting(stmt, ISettingsGlobal::DEFAULT_INSTALL_LOCATION,
                CoreUtils::Convert(IPackageHelper::APP_INSTALL_AUTO));
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 56;
    }

    if (upgradeVersion == 56) {
        /*
         * Add Bluetooth to list of toggleable radios in airplane mode
         */
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        StringBuilder builder;
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::AIRPLANE_MODE_TOGGLEABLE_RADIOS;
        builder += "'";
        db->ExecSQL(builder.ToString());
        db->CompileStatement(String("INSERT OR IGNORE INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadStringSetting(stmt, ISettingsSystem::AIRPLANE_MODE_TOGGLEABLE_RADIOS,
                R::string::airplane_mode_toggleable_radios);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
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
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSecure::ACCESSIBILITY_SCRIPT_INJECTION,
                R::bool_::def_accessibility_script_injection);
        if (stmt) ICloseable::Probe(stmt)->Close();
        stmt = NULL;
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadStringSetting(stmt, ISettingsSecure::ACCESSIBILITY_WEB_CONTENT_KEY_BINDINGS,
                R::string::def_accessibility_web_content_key_bindings);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 58;
    }

    if (upgradeVersion == 58) {
        /* Add default for new Auto Time Zone */
        Int32 autoTimeValue = GetInt32ValueFromSystem(db, ISettingsSystem::AUTO_TIME, 0);
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);

        LoadSetting(stmt, ISettingsSystem::AUTO_TIME_ZONE,
                CoreUtils::Convert(autoTimeValue)); // Sync timezone to NITZ if auto_time was enabled
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 59;
    }

    if (upgradeVersion == 59) {
        // Persistence for the rotation lock feature.
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSystem::USER_ROTATION, R::integer::def_user_rotation); // should be zero degrees
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
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
        StringBuilder builder;
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED;
        builder += "'";

        db->ExecSQL(builder.ToString());
        Int32 newValue = (1 << IAudioManager::STREAM_RING)
                | (1 << IAudioManager::STREAM_NOTIFICATION)
                | (1 << IAudioManager::STREAM_SYSTEM)
                | (1 << IAudioManager::STREAM_SYSTEM_ENFORCED)
                | (1 << IAudioManager::STREAM_MUSIC);
        builder.Reset();
        builder += "INSERT INTO system ('name', 'value') values ('";
        builder += ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED;
        builder += "', '";
        builder += newValue;
        builder += "')";
        db->ExecSQL(builder.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        // }
        upgradeVersion = 64;
     }

    if (upgradeVersion == 64) {
        // New setting to configure the long press timeout.
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadIntegerSetting(stmt, ISettingsSecure::LONG_PRESS_TIMEOUT,
                R::integer::def_long_press_timeout_millis);
        ICloseable::Probe(stmt)->Close();
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
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
        StringBuilder builder;
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::WINDOW_ANIMATION_SCALE;
        builder += "'";
        db->ExecSQL(builder.ToString());

        builder.Reset();
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::TRANSITION_ANIMATION_SCALE;
        builder += "'";
        db->ExecSQL(builder.ToString());
        db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadDefaultAnimationSettings(stmt);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
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

        StringBuilder builder;
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED;
        builder += "'";
        db->ExecSQL(builder.ToString());

        builder.Reset();
        builder += "INSERT INTO system ('name', 'value') values ('";
        builder += ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED;
        builder += "', '";
        builder += ringerModeAffectedStreams;
        builder += "')";
        db->ExecSQL(builder.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        // }
        upgradeVersion = 67;
    }

    if (upgradeVersion == 67) {
        // New setting to enable touch exploration.
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSecure::TOUCH_EXPLORATION_ENABLED,
                R::bool_::def_touch_exploration_enabled);
        ICloseable::Probe(stmt)->Close();
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 68;
    }

    if (upgradeVersion == 68) {
        // Enable all system sounds by default
        db->BeginTransaction();
        // try {
        StringBuilder builder;
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::NOTIFICATIONS_USE_RING_VOLUME;
        builder += "'";
        db->ExecSQL(builder.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        // }
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
        StringBuilder builder("UPDATE system SET value='");
        builder += airplaneRadios;
        builder += "' WHERE name='";
        builder += ISettingsSystem::AIRPLANE_MODE_RADIOS;
        builder += "'";
        db->ExecSQL(builder.ToString());

        builder.Reset();
        builder += "UPDATE system SET value='";
        builder += toggleableRadios;
        builder += "' WHERE name='";
        builder += ISettingsSystem::AIRPLANE_MODE_TOGGLEABLE_RADIOS;
        builder += "'";
        db->ExecSQL(builder.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        // }
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
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSecure::ACCESSIBILITY_SPEAK_PASSWORD,
                R::bool_::def_accessibility_speak_password);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 72;
    }

    if (upgradeVersion == 72) {
        // update vibration settings
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR REPLACE INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSystem::VIBRATE_IN_SILENT,
                R::bool_::def_vibrate_in_silent);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
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
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadStringSetting(stmt, ISettingsSecure::ACCESSIBILITY_SCREEN_READER_URL,
                R::string::def_accessibility_screen_reader_url);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 75;
    }

    if (upgradeVersion == 75) {
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        AutoPtr<ICursor> c;
        // try {
        AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
        (*args)[0] = "_id";
        (*args)[1] = "value";
        db->Query(TABLE_SECURE, args, String("name='lockscreen.disabled'"),
                NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&c);
        // only set default if it has not yet been set
        Int32 count;
        if (c == NULL || (c->GetCount(&count), count == 0)) {
            db->CompileStatement(String("INSERT INTO system(name,value) VALUES(?,?);"),
                    (ISQLiteStatement**)&stmt);
            LoadBooleanSetting(stmt, ISettingsSystem::LOCKSCREEN_DISABLED,
                    R::bool_::def_lockscreen_disabled);
        }
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (c) ICloseable::Probe(c)->Close();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 76;
    }

    /************* The following are Jelly Bean changes ************/

    if (upgradeVersion == 76) {
        // Removed VIBRATE_IN_SILENT setting
        db->BeginTransaction();
        // try {
        StringBuilder builder;
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::VIBRATE_IN_SILENT;
        builder += "'";
        db->ExecSQL(builder.ToString());
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        // }

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
        db->CompileStatement(String("INSERT OR REPLACE INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadStringSetting(stmt, ISettingsSecure::ACCESSIBILITY_SCREEN_READER_URL,
                R::string::def_accessibility_screen_reader_url);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
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
            String touchExplorationGrantedServices = GetStringValueFromTable(db,
                    TABLE_SECURE,
                    ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES,
                    String(""));
            if (TextUtils::IsEmpty(touchExplorationGrantedServices)
                    && !TextUtils::IsEmpty(enabledServices)) {
                AutoPtr<ISQLiteStatement> stmt;
                // try {
                db->BeginTransaction();
                db->CompileStatement(String("INSERT OR REPLACE INTO secure(name,value) VALUES(?,?);"),
                        (ISQLiteStatement**)&stmt);
                LoadSetting(stmt,
                        ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES,
                        CoreUtils::Convert(enabledServices));
                db->SetTransactionSuccessful();
                // } finally {
                db->EndTransaction();
                if (stmt) ICloseable::Probe(stmt)->Close();
                // }
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
        db->CompileStatement(String("INSERT OR REPLACE INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
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
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 81;
    }

    if (upgradeVersion == 81) {
        // Add package verification setting
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR REPLACE INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsGlobal::PACKAGE_VERIFIER_ENABLE,
                R::bool_::def_package_verifier_enable);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
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
            AutoPtr< ArrayOf<String> > settingsToMove = HashsetToStringArray(SettingsProvider::sSystemGlobalKeys);
            MoveSettingsToNewTable(db, TABLE_SYSTEM, TABLE_GLOBAL, settingsToMove, FALSE);
            settingsToMove = HashsetToStringArray(SettingsProvider::sSecureGlobalKeys);
            MoveSettingsToNewTable(db, TABLE_SECURE, TABLE_GLOBAL, settingsToMove, FALSE);

            db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            if (stmt) ICloseable::Probe(stmt)->Close();
            // }
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
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt,
                ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_ENABLED,
                R::bool_::def_accessibility_display_magnification_enabled);
        if (stmt) ICloseable::Probe(stmt)->Close();
        stmt = NULL;
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadFractionSetting(stmt, ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_SCALE,
                R::fraction::def_accessibility_display_magnification_scale, 1);
        if (stmt) ICloseable::Probe(stmt)->Close();
        stmt = NULL;
        db->CompileStatement(String("INSERT INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt,
                ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_AUTO_UPDATE,
                R::bool_::def_accessibility_display_magnification_auto_update);

        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 84;
    }

    if (upgradeVersion == 84) {
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            // AutoPtr<ISQLiteStatement> stmt;
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
            db->EndTransaction();
            //     if (stmt != null) stmt.close();
            // }
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
            db->EndTransaction();
            // }
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
            db->EndTransaction();
            // }
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
            db->EndTransaction();
            // }
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
            db->EndTransaction();
            // }
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
            db->EndTransaction();
            // }
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
            (*secureToGlobal)[1] = ISettingsGlobal::CDMA_SUBSCRIPTION_MODE;

            MoveSettingsToNewTable(db, TABLE_SYSTEM, TABLE_GLOBAL, systemToGlobal, TRUE);
            MoveSettingsToNewTable(db, TABLE_SECURE, TABLE_GLOBAL, secureToGlobal, TRUE);

            db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            // }
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
            db->EndTransaction();
            // }
        }
        upgradeVersion = 92;
    }

    if (upgradeVersion == 92) {
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR IGNORE INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        if (mUserHandle == IUserHandle::USER_OWNER) {
            // consider existing primary users to have made it through user setup
            // if the globally-scoped device-provisioned bit is set
            // (indicating they already made it through setup as primary)
            Int32 deviceProvisioned = GetInt32ValueFromTable(db, TABLE_GLOBAL,
                    ISettingsGlobal::DEVICE_PROVISIONED, 0);
            LoadSetting(stmt, ISettingsSecure::USER_SETUP_COMPLETE,
                    CoreUtils::Convert(deviceProvisioned));
        }
        else {
            // otherwise use the default
            LoadBooleanSetting(stmt, ISettingsSecure::USER_SETUP_COMPLETE,
                    R::bool_::def_user_setup_complete);
        }
        // } finally {
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 93;
    }

    if (upgradeVersion == 93) {
        // Redo this step, since somehow it didn't work the first time for some users
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            // try {
            // Migrate now-global settings
            AutoPtr< ArrayOf<String> > settingsToMove = HashsetToStringArray(SettingsProvider::sSystemGlobalKeys);
            MoveSettingsToNewTable(db, TABLE_SYSTEM, TABLE_GLOBAL, settingsToMove, TRUE);
            settingsToMove = HashsetToStringArray(SettingsProvider::sSecureGlobalKeys);
            MoveSettingsToNewTable(db, TABLE_SECURE, TABLE_GLOBAL, settingsToMove, TRUE);

            db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            // }
        }
        upgradeVersion = 94;
    }

    if (upgradeVersion == 94) {
        // Add wireless charging started sound setting
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            AutoPtr<ISQLiteStatement> stmt;
            // try {
            db->CompileStatement(String("INSERT OR REPLACE INTO global(name,value) VALUES(?,?);"),
                    (ISQLiteStatement**)&stmt);
            LoadStringSetting(stmt, ISettingsGlobal::WIRELESS_CHARGING_STARTED_SOUND,
                    R::string::def_wireless_charging_started_sound);
            db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            if (stmt) ICloseable::Probe(stmt)->Close();
            // }
        }
        upgradeVersion = 95;
    }

    if (upgradeVersion == 95) {
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            // try {
            AutoPtr< ArrayOf<String> > settingsToMove = ArrayOf<String>::Alloc(1);
            (*settingsToMove)[0] = ISettingsGlobal::BUGREPORT_IN_POWER_MENU;
            MoveSettingsToNewTable(db, TABLE_SECURE, TABLE_GLOBAL, settingsToMove, TRUE);
            db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            // }
        }
        upgradeVersion = 96;
    }

    if (upgradeVersion == 96) {
        // NOP bump due to a reverted change that some people got on upgrade.
        upgradeVersion = 97;
    }

    if (upgradeVersion == 97) {
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            AutoPtr<ISQLiteStatement> stmt;
            // try {
            db->CompileStatement(String("INSERT OR REPLACE INTO global(name,value) VALUES(?,?);"),
                    (ISQLiteStatement**)&stmt);
            LoadIntegerSetting(stmt, ISettingsGlobal::LOW_BATTERY_SOUND_TIMEOUT,
                    R::integer::def_low_battery_sound_timeout);
            db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            if (stmt) ICloseable::Probe(stmt)->Close();
            // }
        }
        upgradeVersion = 98;
    }

    if (upgradeVersion == 98) {
        // no-op; LOCK_SCREEN_SHOW_NOTIFICATIONS now handled in version 106
        upgradeVersion = 99;
    }

    if (upgradeVersion == 99) {
        // no-op; HEADS_UP_NOTIFICATIONS_ENABLED now handled in version 100
        upgradeVersion = 100;
    }

    if (upgradeVersion == 100) {
        // note: LOCK_SCREEN_SHOW_NOTIFICATIONS now handled in version 106
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            AutoPtr<ISQLiteStatement> stmt;
            // try {
            db->CompileStatement(String("INSERT OR REPLACE INTO global(name,value) VALUES(?,?);"),
                    (ISQLiteStatement**)&stmt);
            LoadIntegerSetting(stmt, ISettingsGlobal::HEADS_UP_NOTIFICATIONS_ENABLED,
                    R::integer::def_heads_up_enabled);
            db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            if (stmt) ICloseable::Probe(stmt)->Close();
            // }
        }
        upgradeVersion = 101;
    }

    if (upgradeVersion == 101) {
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            AutoPtr<ISQLiteStatement> stmt;
            // try {
            db->CompileStatement(String("INSERT OR IGNORE INTO global(name,value) VALUES(?,?);"),
                    (ISQLiteStatement**)&stmt);
            LoadSetting(stmt, ISettingsGlobal::DEVICE_NAME,
                    CoreUtils::Convert(GetDefaultDeviceName()));
            db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            if (stmt) ICloseable::Probe(stmt)->Close();
            // }
        }
        upgradeVersion = 102;
    }

    if (upgradeVersion == 102) {
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
            // The INSTALL_NON_MARKET_APPS setting is becoming per-user rather
            // than device-global.
        if (mUserHandle == IUserHandle::USER_OWNER) {
            // In the owner user, the global table exists so we can migrate the
            // entry from there to the secure table, preserving its value.
            AutoPtr< ArrayOf<String> > globalToSecure = ArrayOf<String>::Alloc(1);
            (*globalToSecure)[0] = ISettingsSecure::INSTALL_NON_MARKET_APPS;
            MoveSettingsToNewTable(db, TABLE_GLOBAL, TABLE_SECURE, globalToSecure, TRUE);
        }
        else {
            // Secondary users' dbs don't have the global table, so institute the
            // default.
            db->CompileStatement(String("INSERT OR IGNORE INTO secure(name,value) VALUES(?,?);"),
                    (ISQLiteStatement**)&stmt);
            LoadBooleanSetting(stmt, ISettingsSecure::INSTALL_NON_MARKET_APPS,
                    R::bool_::def_install_non_market_apps);
        }
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 103;
    }

    if (upgradeVersion == 103) {
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR REPLACE INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSecure::WAKE_GESTURE_ENABLED,
                R::bool_::def_wake_gesture_enabled);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 104;
    }

    if (upgradeVersion < 105) {
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            AutoPtr<ISQLiteStatement> stmt;
            // try {
            db->CompileStatement(String("INSERT OR IGNORE INTO global(name,value) VALUES(?,?);"),
                    (ISQLiteStatement**)&stmt);
            LoadBooleanSetting(stmt, ISettingsGlobal::GUEST_USER_ENABLED,
                    R::bool_::def_guest_user_enabled);
            db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            if (stmt) ICloseable::Probe(stmt)->Close();
            // }
        }
        upgradeVersion = 105;
    }

    if (upgradeVersion < 106) {
        // LOCK_SCREEN_SHOW_NOTIFICATIONS is now per-user.
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR IGNORE INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadIntegerSetting(stmt, ISettingsSecure::LOCK_SCREEN_SHOW_NOTIFICATIONS,
                R::integer::def_lock_screen_show_notifications);
        if (mUserHandle == IUserHandle::USER_OWNER) {
            const Int32 oldShow = GetInt32ValueFromTable(db,
                    TABLE_GLOBAL, ISettingsSecure::LOCK_SCREEN_SHOW_NOTIFICATIONS, -1);
            if (oldShow >= 0) {
                // overwrite the default with whatever you had
                LoadSetting(stmt, ISettingsSecure::LOCK_SCREEN_SHOW_NOTIFICATIONS,
                        CoreUtils::Convert(oldShow));
                AutoPtr<ISQLiteStatement> deleteStmt;
                db->CompileStatement(String("DELETE FROM global WHERE name=?"),
                        (ISQLiteStatement**)&deleteStmt);
                ISQLiteProgram::Probe(deleteStmt)->BindString(1, ISettingsSecure::LOCK_SCREEN_SHOW_NOTIFICATIONS);
                deleteStmt->Execute();
            }
        }
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 106;
    }

    if (upgradeVersion < 107) {
        // Add trusted sound setting
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            AutoPtr<ISQLiteStatement> stmt;
            // try {
            db->CompileStatement(String("INSERT OR REPLACE INTO global(name,value) VALUES(?,?);"),
                    (ISQLiteStatement**)&stmt);
            LoadStringSetting(stmt, ISettingsGlobal::TRUSTED_SOUND,
                    R::string::def_trusted_sound);
            db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            if (stmt) ICloseable::Probe(stmt)->Close();
            // }
        }
        upgradeVersion = 107;
    }

    if (upgradeVersion < 108) {
        // Reset the auto-brightness setting to default since the behavior
        // of the feature is now quite different and is being presented to
        // the user in a new way as "adaptive brightness".
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR REPLACE INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSystem::SCREEN_BRIGHTNESS_MODE,
                R::bool_::def_screen_brightness_automatic_mode);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 108;
    }

    if (upgradeVersion < 109) {
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR IGNORE INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadBooleanSetting(stmt, ISettingsSecure::LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS,
                R::bool_::def_lock_screen_allow_private_notifications);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 109;
    }

    if (upgradeVersion < 110) {
        // The SIP_CALL_OPTIONS value SIP_ASK_EACH_TIME is being deprecated.
        // If the SIP_CALL_OPTIONS setting is set to SIP_ASK_EACH_TIME, default to
        // SIP_ADDRESS_ONLY.
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("UPDATE system SET value = ? WHERE name = ? AND value = ?;"),
                (ISQLiteStatement**)&stmt);
        ISQLiteProgram::Probe(stmt)->BindString(1, ISettingsSystem::SIP_ADDRESS_ONLY);
        ISQLiteProgram::Probe(stmt)->BindString(2, ISettingsSystem::SIP_CALL_OPTIONS);
        ISQLiteProgram::Probe(stmt)->BindString(3, ISettingsSystem::SIP_ASK_ME_EACH_TIME);
        stmt->Execute();
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 110;
    }

    if (upgradeVersion < 111) {
        // reset ringer mode, so it doesn't force zen mode to follow
        if (mUserHandle == IUserHandle::USER_OWNER) {
            db->BeginTransaction();
            AutoPtr<ISQLiteStatement> stmt;
            // try {
            db->CompileStatement(String("INSERT OR REPLACE INTO global(name,value) VALUES(?,?);"),
                    (ISQLiteStatement**)&stmt);
            LoadSetting(stmt, ISettingsGlobal::MODE_RINGER,
                    CoreUtils::Convert(IAudioManager::RINGER_MODE_NORMAL));
            db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            if (stmt) ICloseable::Probe(stmt)->Close();
            // }
        }
        upgradeVersion = 111;
    }

    if (upgradeVersion < 112) {
        if (mUserHandle == IUserHandle::USER_OWNER) {
            // When device name was added, we went with Manufacturer + Model, device name should
            // actually be Model only.
            // Update device name to Model if it wasn't modified by user.
            db->BeginTransaction();
            AutoPtr<ISQLiteStatement> stmt;
            // try {
            db->CompileStatement(String("UPDATE global SET value = ?  WHERE name = ? AND value = ?"),
                    (ISQLiteStatement**)&stmt);
            ISQLiteProgram::Probe(stmt)->BindString(1, GetDefaultDeviceName()); // new default device name
            ISQLiteProgram::Probe(stmt)->BindString(2, ISettingsGlobal::DEVICE_NAME);
            ISQLiteProgram::Probe(stmt)->BindString(3, GetOldDefaultDeviceName()); // old default device name
            stmt->Execute();
            db->SetTransactionSuccessful();
            // } finally {
            db->EndTransaction();
            if (stmt) ICloseable::Probe(stmt)->Close();
            // }
        }
        upgradeVersion = 112;
    }

    if (upgradeVersion < 113) {
        db->BeginTransaction();
        AutoPtr<ISQLiteStatement> stmt;
        // try {
        db->CompileStatement(String("INSERT OR IGNORE INTO secure(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);
        LoadIntegerSetting(stmt, ISettingsSecure::SLEEP_TIMEOUT,
                R::integer::def_sleep_timeout);
        db->SetTransactionSuccessful();
        // } finally {
        db->EndTransaction();
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
        upgradeVersion = 113;
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
        wipeReason += "');";
        db->ExecSQL(String("INSERT INTO secure(name,value) values('wiped_db_reason','") + wipeReason.ToString());
    }
    return NOERROR;
}

AutoPtr< ArrayOf<String> > DatabaseHelper::HashsetToStringArray(
    /* [in] */ IHashSet* set)
{
    Int32 size;
    set->GetSize(&size);

    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(size);

    AutoPtr< ArrayOf<IInterface*> > inArray = ArrayOf<IInterface*>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        (*inArray)[i] = CoreUtils::Convert((*array)[i]);
    }

    AutoPtr< ArrayOf<IInterface*> > outArray;
    set->ToArray(inArray, (ArrayOf<IInterface*>**)&outArray);

    AutoPtr< ArrayOf<String> > result = ArrayOf<String>::Alloc(outArray->GetLength());
    for (Int32 i = 0; i < outArray->GetLength(); i++) {
        String str;
        ICharSequence::Probe((*outArray)[i])->ToString(&str);
        (*result)[i] = str;
    }

    return result;
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
    sb += doIgnore ? " OR IGNORE " : "";
    sb += " INTO ";
    sb += destTable;
    sb += " (name,value) SELECT name,value FROM ";
    sb += sourceTable;
    sb += " WHERE name=?";
    db->CompileStatement(sb.ToString(), (ISQLiteStatement**)&insertStmt);
    sb.Reset();
    sb += "DELETE FROM ";
    sb += sourceTable;
    sb += " WHERE name=?";
    db->CompileStatement(sb.ToString(), (ISQLiteStatement**)&deleteStmt);

    for (Int32 i = 0; i < settingsToMove->GetLength(); ++i) {
        String setting = (*settingsToMove)[i];
        ISQLiteProgram::Probe(insertStmt)->BindString(1, setting);
        insertStmt->Execute();

        ISQLiteProgram::Probe(deleteStmt)->BindString(1, setting);
        deleteStmt->Execute();
    }
    db->SetTransactionSuccessful();
    // } finally {
    db->EndTransaction();
    if (insertStmt) ICloseable::Probe(insertStmt)->Close();
    if (deleteStmt) ICloseable::Probe(deleteStmt)->Close();
    // }
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

    sb.Reset();
    sb += "DELETE FROM ";
    sb += sourceTable;
    sb += " WHERE substr(name,0,?)=?";
    db->CompileStatement(sb.ToString(), (ISQLiteStatement**)&deleteStmt);

    for (Int32 i = 0; i < prefixesToMove->GetLength(); ++i) {
        String prefix = (*prefixesToMove)[i];
        ISQLiteProgram::Probe(insertStmt)->BindInt64(1, prefix.GetLength() + 1);
        ISQLiteProgram::Probe(insertStmt)->BindString(2, prefix);
        insertStmt->Execute();

        ISQLiteProgram::Probe(deleteStmt)->BindInt64(1, prefix.GetLength() + 1);
        ISQLiteProgram::Probe(deleteStmt)->BindString(2, prefix);
        deleteStmt->Execute();
    }
    db->SetTransactionSuccessful();
    // } finally {
    db->EndTransaction();
    if (insertStmt) ICloseable::Probe(insertStmt)->Close();
    if (deleteStmt) ICloseable::Probe(deleteStmt)->Close();
    // }
}

ECode DatabaseHelper::UpgradeLockPatternLocation(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<ICursor> c;
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    (*args)[0] = "_id";
    (*args)[1] = "value";
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
            AutoPtr<ILockPatternUtils> lpu;
            CLockPatternUtils::New(mContext, (ILockPatternUtils**)&lpu);
            AutoPtr<ILockPatternUtilsHelper> helper;
            CLockPatternUtilsHelper::AcquireSingleton((ILockPatternUtilsHelper**)&helper);
            AutoPtr<IList> cellPattern;
            helper->StringToPattern(lockPattern, (IList**)&cellPattern);
            ECode ec = lpu->SaveLockPattern(cellPattern);
            if (FAILED(ec)) {
                // Don't want corrupted lock pattern to hang the reboot process
            }
            // } catch (IllegalArgumentException e) {
            //     // Don't want corrupted lock pattern to hang the reboot process
            // }
        }
        ICloseable::Probe(c)->Close();
        Int32 v;
        db->Delete(TABLE_SYSTEM, String("name='lock_pattern'"), NULL, &v);
    }
    else {
        ICloseable::Probe(c)->Close();
    }
    return NOERROR;
}

ECode DatabaseHelper::UpgradeScreenTimeoutFromNever(
    /* [in] */ ISQLiteDatabase* db)
{
    // See if the timeout is -1 (for "Never").
    AutoPtr<ICursor> c;
    AutoPtr< ArrayOf<String> > args1 = ArrayOf<String>::Alloc(2);
    (*args1)[0] = "_id";
    (*args1)[1] = "value";
    AutoPtr< ArrayOf<String> > args2 = ArrayOf<String>::Alloc(2);
    (*args2)[0] = ISettingsSystem::SCREEN_OFF_TIMEOUT;
    (*args2)[1] = "-1";
    FAIL_RETURN(db->Query(TABLE_SYSTEM, args1, String("name=? AND value=?"),
            args2, String(NULL), String(NULL), String(NULL), (ICursor**)&c))

    AutoPtr<ISQLiteStatement> stmt;
    Int32 count;
    if (c->GetCount(&count), count > 0) {
        ICloseable::Probe(c)->Close();
        // try {
        db->CompileStatement(String("INSERT OR REPLACE INTO system(name,value) VALUES(?,?);"),
                (ISQLiteStatement**)&stmt);

        // Set the timeout to 30 minutes in milliseconds
        LoadSetting(stmt, ISettingsSystem::SCREEN_OFF_TIMEOUT,
                CoreUtils::Convert(StringUtils::ToString(30 * 60 * 1000)));
        // } finally {
        if (stmt) ICloseable::Probe(stmt)->Close();
        // }
    }
    else {
        ICloseable::Probe(c)->Close();
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
                IAudioManager::VIBRATE_TYPE_RINGER,
                IAudioManager::VIBRATE_SETTING_ONLY_SILENT,
                &vibrateSetting);
    }
    // Apply the same setting to the notification vibrate value
    helper->GetValueForVibrateSetting(vibrateSetting,
            IAudioManager::VIBRATE_TYPE_NOTIFICATION, vibrateSetting, &vibrateSetting);

    AutoPtr<ISQLiteStatement> stmt;
    // try {
    db->CompileStatement(String("INSERT OR REPLACE INTO system(name,value) VALUES(?,?);"),
            (ISQLiteStatement**)&stmt);

    LoadSetting(stmt, ISettingsSystem::VIBRATE_ON,
            CoreUtils::Convert(vibrateSetting));
    // } finally {
    if (stmt) ICloseable::Probe(stmt)->Close();
    // }
}

void DatabaseHelper::UpgradeScreenTimeout(
    /* [in] */ ISQLiteDatabase* db)
{
    // Change screen timeout to current default
    db->BeginTransaction();
    AutoPtr<ISQLiteStatement> stmt;
    // try {
    db->CompileStatement(String("INSERT OR REPLACE INTO system(name,value) VALUES(?,?);"),
            (ISQLiteStatement**)&stmt);
    LoadIntegerSetting(stmt, ISettingsSystem::SCREEN_OFF_TIMEOUT,
            R::integer::def_screen_off_timeout);
    db->SetTransactionSuccessful();
    // } finally {
    db->EndTransaction();
    if (stmt) ICloseable::Probe(stmt)->Close();
    // }
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
    db->EndTransaction();
    // }
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
    FAIL_RETURN(res->GetXml(R::xml::bookmarks, (IXmlResourceParser**)&parser));
    AutoPtr<IXmlPullParser> pullParser = IXmlPullParser::Probe(parser);
    ECode ec = XmlUtils::BeginDocument(pullParser, String("bookmarks"));
    if (FAILED(ec)) {
        Logger::W(TAG, "Got execption parsing bookmarks. 0x%08x", ec);
        return ec;
    }

    Int32 depth;
    pullParser->GetDepth(&depth);
    Int32 type, dep;
    while (((pullParser->Next(&type), type != IXmlPullParser::END_TAG) ||
            (pullParser->GetDepth(&dep), dep > depth)) && type != IXmlPullParser::END_DOCUMENT) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String name;
        pullParser->GetName(&name);
        if (!name.Equals("bookmark")) {
            break;
        }

        String pkg, cls, shortcutStr, category;
        pullParser->GetAttributeValue(String(NULL), String("package"), &pkg);
        pullParser->GetAttributeValue(String(NULL), String("class"), &cls);
        pullParser->GetAttributeValue(String(NULL), String("shortcut"), &shortcutStr);
        pullParser->GetAttributeValue(String(NULL), String("category"), &category);

        Int32 shortcutValue = shortcutStr.GetChar(0);
        if (TextUtils::IsEmpty(shortcutStr)) {
            Logger::W(TAG, "Unable to get shortcut for: %s/%s" , pkg.string(),
                    cls.string());
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
                packageManager->CanonicalToCurrentPackageNames(*args,
                        (ArrayOf<String>**)&packages);
                cn = NULL;
                CComponentName::New((*packages)[0], cls, (IComponentName**)&cn);
                // try {
                info = NULL;
                if (FAILED(packageManager->GetActivityInfo(cn, 0, (IActivityInfo**)&info))) {
                    Logger::W(TAG, "Unable to add bookmark: %s/%s", pkg.string(),
                            cls.string());
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
            IPackageItemInfo::Probe(info)->LoadLabel(packageManager, (ICharSequence**)&cs);
            cs->ToString(&title);
        }
        else if (!category.IsNull()) {
            AutoPtr<IIntentHelper> helper;
            CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
            helper->MakeMainSelectorActivity(IIntent::ACTION_MAIN, category,
                    (IIntent**)&intent);
            title = "";
        }
        else {
            Logger::W(TAG, "Unable to add bookmark for shortcut %s: missing package/class or category attributes",
                    shortcutStr.string());
            continue;
        }

        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        String str;
        intent->ToUri(0, &str);
        values->Put(ISettingsBookmarks::INTENT,
                CoreUtils::Convert(str));
        values->Put(ISettingsBookmarks::TITLE,
                CoreUtils::Convert(title));
        values->Put(ISettingsBookmarks::SHORTCUT,
                CoreUtils::Convert(shortcutStr));
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
    db->CompileStatement(String("INSERT OR IGNORE INTO system(name,value) VALUES(?,?);"),
            (ISQLiteStatement**)&stmt);

    AutoPtr<IAudioManagerHelper> helper;
    assert(0 && "TODO");
    // CAudioManagerHelper::AcquireSingleton((IAudioManagerHelper**)&helper);
    AutoPtr< ArrayOf<Int32> > volume;
    // helper->GetDefaultStreamVolume((ArrayOf<Int32>**)&volume);

    LoadSetting(stmt, ISettingsSystem::VOLUME_MUSIC,
            CoreUtils::Convert((*volume)[IAudioManager::STREAM_MUSIC]));
    LoadSetting(stmt, ISettingsSystem::VOLUME_RING,
            CoreUtils::Convert((*volume)[IAudioManager::STREAM_RING]));
    LoadSetting(stmt, ISettingsSystem::VOLUME_SYSTEM,
            CoreUtils::Convert((*volume)[IAudioManager::STREAM_SYSTEM]));
    LoadSetting(stmt, ISettingsSystem::VOLUME_VOICE,
            CoreUtils::Convert((*volume)[IAudioManager::STREAM_VOICE_CALL]));
    LoadSetting(stmt, ISettingsSystem::VOLUME_ALARM,
            CoreUtils::Convert((*volume)[IAudioManager::STREAM_ALARM]));
    LoadSetting(stmt, ISettingsSystem::VOLUME_NOTIFICATION,
            CoreUtils::Convert((*volume)[IAudioManager::STREAM_NOTIFICATION]));
    LoadSetting(stmt, ISettingsSystem::VOLUME_BLUETOOTH_SCO,
            CoreUtils::Convert((*volume)[IAudioManager::STREAM_BLUETOOTH_SCO]));

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

    LoadSetting(stmt, ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED,
            CoreUtils::Convert(ringerModeAffectedStreams));

    LoadSetting(stmt, ISettingsSystem::MUTE_STREAMS_AFFECTED,
            CoreUtils::Convert((1 << IAudioManager::STREAM_MUSIC) |
            (1 << IAudioManager::STREAM_RING) |
            (1 << IAudioManager::STREAM_NOTIFICATION) |
            (1 << IAudioManager::STREAM_SYSTEM)));
    // } finally {
    if (stmt) ICloseable::Probe(stmt)->Close();
    // }

    LoadVibrateWhenRingingSetting(db);
}

void DatabaseHelper::LoadVibrateSetting(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Boolean deleteOld)
{
    if (deleteOld) {
        StringBuilder builder;
        builder += "DELETE FROM system WHERE name='";
        builder += ISettingsSystem::VIBRATE_ON;
        builder += "'";
        db->ExecSQL(builder.ToString());
    }

    AutoPtr<ISQLiteStatement> stmt;
    // try {
    db->CompileStatement(String("INSERT OR IGNORE INTO system(name,value) VALUES(?,?);"),
            (ISQLiteStatement**)&stmt);

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

    LoadSetting(stmt, ISettingsSystem::VIBRATE_ON, CoreUtils::Convert(vibrate));
    // } finally {
    if (stmt) ICloseable::Probe(stmt)->Close();
    // }
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
    db->CompileStatement(String("INSERT OR IGNORE INTO system(name,value) VALUES(?,?);"),
            (ISQLiteStatement**)&stmt);

    LoadSetting(stmt, ISettingsSystem::VIBRATE_WHEN_RINGING,
            CoreUtils::Convert(vibrateWhenRinging ? 1 : 0));
    // } finally {
    if (stmt) ICloseable::Probe(stmt)->Close();
    // }
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
    db->CompileStatement(String("INSERT OR IGNORE INTO system(name,value) VALUES(?,?);"),
            (ISQLiteStatement**)&stmt);

    LoadBooleanSetting(stmt, ISettingsSystem::DIM_SCREEN,
            R::bool_::def_dim_screen);
    LoadIntegerSetting(stmt, ISettingsSystem::SCREEN_OFF_TIMEOUT,
            R::integer::def_screen_off_timeout);

    // Set default cdma DTMF type
    AutoPtr<IInteger32> integer;
    CInteger32::New(0, (IInteger32**)&integer);
    LoadSetting(stmt, ISettingsSystem::DTMF_TONE_TYPE_WHEN_DIALING, integer);

    // Set default hearing aid
    LoadSetting(stmt, ISettingsSystem::HEARING_AID, integer);

    // Set default tty mode
    LoadSetting(stmt, ISettingsSystem::TTY_MODE, integer);

    LoadIntegerSetting(stmt, ISettingsSystem::SCREEN_BRIGHTNESS,
            R::integer::def_screen_brightness);

    LoadBooleanSetting(stmt, ISettingsSystem::SCREEN_BRIGHTNESS_MODE,
            R::bool_::def_screen_brightness_automatic_mode);

    LoadDefaultAnimationSettings(stmt);

    LoadBooleanSetting(stmt, ISettingsSystem::ACCELEROMETER_ROTATION,
            R::bool_::def_accelerometer_rotation);

    LoadDefaultHapticSettings(stmt);

    LoadBooleanSetting(stmt, ISettingsSystem::NOTIFICATION_LIGHT_PULSE,
            R::bool_::def_notification_pulse);

    LoadUISoundEffectsSettings(stmt);

    LoadIntegerSetting(stmt, ISettingsSystem::POINTER_SPEED,
            R::integer::def_pointer_speed);
    // } finally {
    if (stmt) ICloseable::Probe(stmt)->Close();
    // }
}

void DatabaseHelper::LoadUISoundEffectsSettings(
    /* [in] */ ISQLiteStatement* stmt)
{
    LoadBooleanSetting(stmt, ISettingsSystem::DTMF_TONE_WHEN_DIALING,
            R::bool_::def_dtmf_tones_enabled);
    LoadBooleanSetting(stmt, ISettingsSystem::SOUND_EFFECTS_ENABLED,
            R::bool_::def_sound_effects_enabled);
    LoadBooleanSetting(stmt, ISettingsSystem::HAPTIC_FEEDBACK_ENABLED,
            R::bool_::def_haptic_feedback);

    LoadIntegerSetting(stmt, ISettingsSystem::LOCKSCREEN_SOUNDS_ENABLED,
            R::integer::def_lockscreen_sounds_enabled);
}

void DatabaseHelper::LoadDefaultAnimationSettings(
    /* [in] */ ISQLiteStatement* stmt)
{
    LoadFractionSetting(stmt, ISettingsSystem::WINDOW_ANIMATION_SCALE,
            R::fraction::def_window_animation_scale, 1);
    LoadFractionSetting(stmt, ISettingsSystem::TRANSITION_ANIMATION_SCALE,
            R::fraction::def_window_transition_scale, 1);
}

void DatabaseHelper::LoadDefaultHapticSettings(
    /* [in] */ ISQLiteStatement* stmt)
{
    LoadBooleanSetting(stmt, ISettingsSystem::HAPTIC_FEEDBACK_ENABLED,
            R::bool_::def_haptic_feedback);
}

void DatabaseHelper::LoadSecureSettings(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<ISQLiteStatement> stmt;
    // try {
    db->CompileStatement(String("INSERT OR IGNORE INTO secure(name,value) VALUES(?,?);"),
            (ISQLiteStatement**)&stmt);

    LoadStringSetting(stmt, ISettingsSecure::LOCATION_PROVIDERS_ALLOWED,
            R::string::def_location_providers_allowed);

    AutoPtr<ISystemProperties> prop;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&prop);
    String wifiWatchList;
    prop->Get(String("ro.com.android.wifi-watchlist"), &wifiWatchList);
    if (!TextUtils::IsEmpty(wifiWatchList)) {
        LoadSetting(stmt, ISettingsSecure::WIFI_WATCHDOG_WATCH_LIST,
                CoreUtils::Convert(wifiWatchList));
    }

    // Don't do this.  The SystemServer will initialize ADB_ENABLED from a
    // persistent system property instead.
    //LoadSetting(stmt, ISettingsSecure::ADB_ENABLED, 0);

    // Allow mock locations default, based on build
    String location;
    prop->Get(String("ro.allow.mock.location"), &location);
    LoadSetting(stmt, ISettingsSecure::ALLOW_MOCK_LOCATION,
            CoreUtils::Convert(location.Equals("1") ? 1 : 0));

    LoadSecure35Settings(stmt);

    LoadBooleanSetting(stmt, ISettingsSecure::MOUNT_PLAY_NOTIFICATION_SND,
            R::bool_::def_mount_play_notification_snd);

    LoadBooleanSetting(stmt, ISettingsSecure::MOUNT_UMS_AUTOSTART,
            R::bool_::def_mount_ums_autostart);

    LoadBooleanSetting(stmt, ISettingsSecure::MOUNT_UMS_PROMPT,
            R::bool_::def_mount_ums_prompt);

    LoadBooleanSetting(stmt, ISettingsSecure::MOUNT_UMS_NOTIFY_ENABLED,
            R::bool_::def_mount_ums_notify_enabled);

    LoadBooleanSetting(stmt, ISettingsSecure::ACCESSIBILITY_SCRIPT_INJECTION,
            R::bool_::def_accessibility_script_injection);

    LoadStringSetting(stmt, ISettingsSecure::ACCESSIBILITY_WEB_CONTENT_KEY_BINDINGS,
            R::string::def_accessibility_web_content_key_bindings);

    LoadIntegerSetting(stmt, ISettingsSecure::LONG_PRESS_TIMEOUT,
            R::integer::def_long_press_timeout_millis);

    LoadBooleanSetting(stmt, ISettingsSecure::TOUCH_EXPLORATION_ENABLED,
            R::bool_::def_touch_exploration_enabled);

    LoadBooleanSetting(stmt, ISettingsSecure::ACCESSIBILITY_SPEAK_PASSWORD,
            R::bool_::def_accessibility_speak_password);

    LoadStringSetting(stmt, ISettingsSecure::ACCESSIBILITY_SCREEN_READER_URL,
            R::string::def_accessibility_screen_reader_url);

    Boolean disable;
    if (prop->GetBoolean(String("ro.lockscreen.disable.default"), FALSE, &disable), disable) {
        LoadSetting(stmt, ISettingsSystem::LOCKSCREEN_DISABLED,
                CoreUtils::Convert(String("1")));
    }
    else {
        LoadBooleanSetting(stmt, ISettingsSystem::LOCKSCREEN_DISABLED,
                R::bool_::def_lockscreen_disabled);
    }

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

    LoadBooleanSetting(stmt, ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_ENABLED,
            R::bool_::def_accessibility_display_magnification_enabled);

    LoadFractionSetting(stmt, ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_SCALE,
            R::fraction::def_accessibility_display_magnification_scale, 1);

    LoadBooleanSetting(stmt, ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_AUTO_UPDATE,
            R::bool_::def_accessibility_display_magnification_auto_update);

    LoadBooleanSetting(stmt, ISettingsSecure::USER_SETUP_COMPLETE,
            R::bool_::def_user_setup_complete);

    LoadStringSetting(stmt, ISettingsSecure::IMMERSIVE_MODE_CONFIRMATIONS,
            R::string::def_immersive_mode_confirmations);

    LoadBooleanSetting(stmt, ISettingsSecure::INSTALL_NON_MARKET_APPS,
            R::bool_::def_install_non_market_apps);

    LoadBooleanSetting(stmt, ISettingsSecure::WAKE_GESTURE_ENABLED,
            R::bool_::def_wake_gesture_enabled);

    LoadIntegerSetting(stmt, ISettingsSecure::LOCK_SCREEN_SHOW_NOTIFICATIONS,
            R::integer::def_lock_screen_show_notifications);

    LoadBooleanSetting(stmt, ISettingsSecure::LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS,
            R::bool_::def_lock_screen_allow_private_notifications);

    LoadIntegerSetting(stmt, ISettingsSecure::SLEEP_TIMEOUT,
            R::integer::def_sleep_timeout);
    // } finally {
    if (stmt) ICloseable::Probe(stmt)->Close();
    // }
}

void DatabaseHelper::LoadSecure35Settings(
    /* [in] */ ISQLiteStatement* stmt)
{
    LoadBooleanSetting(stmt, ISettingsSecure::BACKUP_ENABLED,
            R::bool_::def_backup_enabled);

    LoadStringSetting(stmt, ISettingsSecure::BACKUP_TRANSPORT,
            R::string::def_backup_transport);
}

void DatabaseHelper::LoadGlobalSettings(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<ISQLiteStatement> stmt;
    // try {
    db->CompileStatement(String("INSERT OR IGNORE INTO global(name,value) VALUES(?,?);"),
            (ISQLiteStatement**)&stmt);

    // --- Previously in 'system'
    LoadBooleanSetting(stmt, ISettingsGlobal::AIRPLANE_MODE_ON,
            R::bool_::def_airplane_mode_on);

    LoadStringSetting(stmt, ISettingsGlobal::AIRPLANE_MODE_RADIOS,
            R::string::def_airplane_mode_radios);

    LoadStringSetting(stmt, ISettingsGlobal::AIRPLANE_MODE_TOGGLEABLE_RADIOS,
            R::string::airplane_mode_toggleable_radios);

    LoadBooleanSetting(stmt, ISettingsGlobal::ASSISTED_GPS_ENABLED,
            R::bool_::assisted_gps_enabled);

    LoadBooleanSetting(stmt, ISettingsGlobal::AUTO_TIME,
            R::bool_::def_auto_time); // Sync time to NITZ

    LoadBooleanSetting(stmt, ISettingsGlobal::AUTO_TIME_ZONE,
            R::bool_::def_auto_time_zone); // Sync timezone to NITZ

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

    LoadSetting(stmt, ISettingsGlobal::STAY_ON_WHILE_PLUGGED_IN,
            CoreUtils::Convert(v));

    LoadIntegerSetting(stmt, ISettingsGlobal::WIFI_SLEEP_POLICY,
            R::integer::def_wifi_sleep_policy);

    LoadSetting(stmt, ISettingsGlobal::MODE_RINGER,
            CoreUtils::Convert(IAudioManager::RINGER_MODE_NORMAL));

    // --- Previously in 'secure'
    LoadBooleanSetting(stmt, ISettingsGlobal::PACKAGE_VERIFIER_ENABLE,
            R::bool_::def_package_verifier_enable);

    LoadBooleanSetting(stmt, ISettingsGlobal::WIFI_ON, R::bool_::def_wifi_on);

    LoadBooleanSetting(stmt, ISettingsGlobal::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON,
            R::bool_::def_networks_available_notification_on);

    LoadBooleanSetting(stmt, ISettingsGlobal::BLUETOOTH_ON,
            R::bool_::def_bluetooth_on);

    // Enable or disable Cell Broadcast SMS
    LoadSetting(stmt, ISettingsGlobal::CDMA_CELL_BROADCAST_SMS,
            CoreUtils::Convert(IRILConstants::CDMA_CELL_BROADCAST_SMS_DISABLED));

    // Data roaming default, based on build
    String dataroaming;
    prop->Get(String("ro.com.android.dataroaming"), String("false"), &dataroaming);
    LoadSetting(stmt, ISettingsGlobal::DATA_ROAMING,
            CoreUtils::Convert(dataroaming.EqualsIgnoreCase("true")? 1 : 0));

    LoadBooleanSetting(stmt, ISettingsGlobal::DEVICE_PROVISIONED,
            R::bool_::def_device_provisioned);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 maxBytes;
    res->GetInteger(R::integer::def_download_manager_max_bytes_over_mobile, &maxBytes);
    if (maxBytes > 0) {
        LoadSetting(stmt, ISettingsGlobal::DOWNLOAD_MAX_BYTES_OVER_MOBILE,
                CoreUtils::Convert(StringUtils::ToString(maxBytes)));
    }

    Int32 recommendedMaxBytes;
    res->GetInteger(R::integer::def_download_manager_recommended_max_bytes_over_mobile,
            &recommendedMaxBytes);
    if (recommendedMaxBytes > 0) {
        LoadSetting(stmt, ISettingsGlobal::DOWNLOAD_RECOMMENDED_MAX_BYTES_OVER_MOBILE,
                CoreUtils::Convert(StringUtils::ToString(recommendedMaxBytes)));
    }

    // Mobile Data default, based on build
    String mobiledata;
    prop->Get(String("ro.com.android.mobiledata"), String("true"), &mobiledata);
    LoadSetting(stmt, ISettingsGlobal::MOBILE_DATA,
            CoreUtils::Convert(mobiledata.EqualsIgnoreCase("true") ? 1 : 0));

    LoadBooleanSetting(stmt, ISettingsGlobal::NETSTATS_ENABLED,
            R::bool_::def_netstats_enabled);

    LoadBooleanSetting(stmt, ISettingsGlobal::USB_MASS_STORAGE_ENABLED,
            R::bool_::def_usb_mass_storage_enabled);

    LoadIntegerSetting(stmt, ISettingsGlobal::WIFI_MAX_DHCP_RETRY_COUNT,
            R::integer::def_max_dhcp_retries);

    LoadBooleanSetting(stmt, ISettingsGlobal::WIFI_DISPLAY_ON,
            R::bool_::def_wifi_display_on);

    LoadStringSetting(stmt, ISettingsGlobal::LOCK_SOUND, R::string::def_lock_sound);
    LoadStringSetting(stmt, ISettingsGlobal::UNLOCK_SOUND,
            R::string::def_unlock_sound);
    LoadStringSetting(stmt, ISettingsGlobal::TRUSTED_SOUND,
            R::string::def_trusted_sound);
    LoadIntegerSetting(stmt, ISettingsGlobal::POWER_SOUNDS_ENABLED,
            R::integer::def_power_sounds_enabled);
    LoadStringSetting(stmt, ISettingsGlobal::LOW_BATTERY_SOUND,
            R::string::def_low_battery_sound);
    LoadIntegerSetting(stmt, ISettingsGlobal::DOCK_SOUNDS_ENABLED,
            R::integer::def_dock_sounds_enabled);
    LoadStringSetting(stmt, ISettingsGlobal::DESK_DOCK_SOUND,
            R::string::def_desk_dock_sound);
    LoadStringSetting(stmt, ISettingsGlobal::DESK_UNDOCK_SOUND,
            R::string::def_desk_undock_sound);
    LoadStringSetting(stmt, ISettingsGlobal::CAR_DOCK_SOUND,
            R::string::def_car_dock_sound);
    LoadStringSetting(stmt, ISettingsGlobal::CAR_UNDOCK_SOUND,
            R::string::def_car_undock_sound);
    LoadStringSetting(stmt, ISettingsGlobal::WIRELESS_CHARGING_STARTED_SOUND,
            R::string::def_wireless_charging_started_sound);

    LoadIntegerSetting(stmt, ISettingsGlobal::DOCK_AUDIO_MEDIA_ENABLED,
            R::integer::def_dock_audio_media_enabled);

    LoadSetting(stmt, ISettingsGlobal::SET_INSTALL_LOCATION,
            CoreUtils::Convert(0));
    LoadSetting(stmt, ISettingsGlobal::DEFAULT_INSTALL_LOCATION,
            CoreUtils::Convert(IPackageHelper::APP_INSTALL_INTERNAL));

    // Set default cdma emergency tone
    LoadSetting(stmt, ISettingsGlobal::EMERGENCY_TONE, CoreUtils::Convert(0));

    // Set default cdma call auto retry
    LoadSetting(stmt, ISettingsGlobal::CALL_AUTO_RETRY, CoreUtils::Convert(0));

    // Set the preferred network mode to target desired value or Default
    // value defined in RILConstants
    Int32 type;
    prop->GetInt32(String("ro.telephony.default_network"),
            IRILConstants::PREFERRED_NETWORK_MODE, &type);
    LoadSetting(stmt, ISettingsGlobal::PREFERRED_NETWORK_MODE,
            CoreUtils::Convert(type));

    // Set the preferred cdma subscription source to target desired value or default
    // value defined in CdmaSubscriptionSourceManager
    prop->GetInt32(String("ro.telephony.default_cdma_sub"), 1
            /*ICdmaSubscriptionSourceManager::PREFERRED_CDMA_SUBSCRIPTION*/, &type);
    LoadSetting(stmt, ISettingsGlobal::CDMA_SUBSCRIPTION_MODE,
            CoreUtils::Convert(type));

    LoadIntegerSetting(stmt, ISettingsGlobal::LOW_BATTERY_SOUND_TIMEOUT,
            R::integer::def_low_battery_sound_timeout);

    LoadIntegerSetting(stmt, ISettingsGlobal::WIFI_SCAN_ALWAYS_AVAILABLE,
            R::integer::def_wifi_scan_always_available);

    LoadIntegerSetting(stmt, ISettingsGlobal::HEADS_UP_NOTIFICATIONS_ENABLED,
            R::integer::def_heads_up_enabled);

    LoadSetting(stmt, ISettingsGlobal::DEVICE_NAME,
            CoreUtils::Convert(GetDefaultDeviceName()));

    LoadBooleanSetting(stmt, ISettingsGlobal::GUEST_USER_ENABLED,
            R::bool_::def_guest_user_enabled);
    // --- New global settings start here
    // } finally {
    if (stmt) ICloseable::Probe(stmt)->Close();
    // }
}

void DatabaseHelper::LoadSetting(
    /* [in] */ ISQLiteStatement* stmt,
    /* [in] */ const String& key,
    /* [in] */ IInterface* value)
{
    assert(stmt != NULL);
    ISQLiteProgram::Probe(stmt)->BindString(1, key);
    String str;
    if (INumber::Probe(value) != NULL) {
        IObject::Probe(value)->ToString(&str);
    }
    else if (ICharSequence::Probe(value) != NULL) {
        ICharSequence::Probe(value)->ToString(&str);
    }
    ISQLiteProgram::Probe(stmt)->BindString(2, str);
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
    LoadSetting(stmt, key, CoreUtils::Convert(str));
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
    LoadSetting(stmt, key, CoreUtils::Convert(value ? String("1") : String("0")));
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
    LoadSetting(stmt, key, CoreUtils::Convert(value));
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
    LoadSetting(stmt, key, CoreUtils::Convert(value));
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
    (*args)[0] = ISettingsNameValueTable::VALUE;

    StringBuilder builder;
    builder += "name='";
    builder += name;
    builder += "'";

    db->Query(table, args, builder.ToString(),
            NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&c);
    Boolean result;
    if (c != NULL && (c->MoveToFirst(&result), result)) {
        String val;
        c->GetString(0, &val);
        if (c) ICloseable::Probe(c)->Close();
        return val.IsNull() ? defaultValue : val;
    }
    // } finally {
    if (c) ICloseable::Probe(c)->Close();
    // }
    return defaultValue;
}

String DatabaseHelper::GetOldDefaultDeviceName()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    (*args)[0] = CoreUtils::Convert(Build::MANUFACTURER);
    (*args)[1] = CoreUtils::Convert(Build::MODEL);

    String str;
    res->GetString(R::string::def_device_name, args, &str);
    return str;
}

String DatabaseHelper::GetDefaultDeviceName()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    (*args)[0] = CoreUtils::Convert(Build::MODEL);

    String str;
    res->GetString(R::string::def_device_name_simple, args, &str);
    return str;
}

} // namespace SettingsProvider
} // namespace Droid
} // namespace Elastos
