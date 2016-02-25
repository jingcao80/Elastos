
#ifndef __ELASTOS_DROID_SETTINGSPROVIDER_DATABASEHELPER_H__
#define __ELASTOS_DROID_SETTINGSPROVIDER_DATABASEHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"
#include <elastos/utility/etl/HashSet.h>

using Elastos::Droid::Database::Sqlite::ISQLiteOpenHelper;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::ISQLiteStatement;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace SettingsProvider {

/**
 * Database helper class for {@link SettingsProvider}.
 * Mostly just has a bit {@link #onCreate} to initialize the database.
 */
class DatabaseHelper
    : public SQLiteOpenHelper
{
public:
    DatabaseHelper(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId);

    static CARAPI_(Boolean) IsValidTable(
        /* [in] */ const String& name);

    CARAPI OnCreate(
        /* [in] */ ISQLiteDatabase* database);

    CARAPI OnUpgrade(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int32 oldVersion,
        /* [in] */ Int32 newVersion);

protected:
    static CARAPI_(String) DbNameForUser(
        /* [in] */ Int32 userHandle);

private:
    static CARAPI_(Boolean) InitValidTables();

    CARAPI_(void) CreateSecureTable(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) CreateGlobalTable(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(AutoPtr< ArrayOf<String> >) HashsetToStringArray(
        /* [in] */ IHashSet* set);

    CARAPI_(void) MoveSettingsToNewTable(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& sourceTable,
        /* [in] */ const String& destTable,
        /* [in] */ ArrayOf<String>* settingsToMove,
        /* [in] */ Boolean doIgnore);

    /**
     * Move any settings with the given prefixes from the source table to the
     * destination table.
     */
    CARAPI_(void) MovePrefixedSettingsToNewTable(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& sourceTable,
        /* [in] */ const String& destTable,
        /* [in] */ ArrayOf<String>* prefixesToMove);

    CARAPI UpgradeLockPatternLocation(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI UpgradeScreenTimeoutFromNever(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeVibrateSettingFromNone(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeScreenTimeout(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeAutoBrightness(
        /* [in] */ ISQLiteDatabase* db);

    /**
     * Loads the default set of bookmarked shortcuts from an xml file.
     *
     * @param db The database to write the values into
     */
    CARAPI LoadBookmarks(
        /* [in] */ ISQLiteDatabase* db);

    /**
     * Loads the default volume levels. It is actually inserting the index of
     * the volume array for each of the volume controls.
     *
     * @param db the database to insert the volume levels into
     */
    CARAPI_(void) LoadVolumeLevels(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) LoadVibrateSetting(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Boolean deleteOld);

    CARAPI_(void) LoadVibrateWhenRingingSetting(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) LoadSettings(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) LoadSystemSettings(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) LoadUISoundEffectsSettings(
        /* [in] */ ISQLiteStatement* stmt);

    CARAPI_(void) LoadDefaultAnimationSettings(
        /* [in] */ ISQLiteStatement* stmt);

    CARAPI_(void) LoadDefaultHapticSettings(
        /* [in] */ ISQLiteStatement* stmt);

    CARAPI_(void) LoadSecureSettings(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) LoadSecure35Settings(
        /* [in] */ ISQLiteStatement* stmt);

    CARAPI_(void) LoadGlobalSettings(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) LoadSetting(
        /* [in] */ ISQLiteStatement* stmt,
        /* [in] */ const String& key,
        /* [in] */ IInterface* value);

    CARAPI_(void) LoadStringSetting(
        /* [in] */ ISQLiteStatement* stmt,
        /* [in] */ const String& key,
        /* [in] */ Int32 resid);

    CARAPI_(void) LoadBooleanSetting(
        /* [in] */ ISQLiteStatement* stmt,
        /* [in] */ const String& key,
        /* [in] */ Int32 resid);

    CARAPI_(void) LoadIntegerSetting(
        /* [in] */ ISQLiteStatement* stmt,
        /* [in] */ const String& key,
        /* [in] */ Int32 resid);

    CARAPI_(void) LoadFractionSetting(
        /* [in] */ ISQLiteStatement* stmt,
        /* [in] */ const String& key,
        /* [in] */ Int32 resid,
        /* [in] */ Int32 base);

    CARAPI_(Int32) GetInt32ValueFromSystem(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& name,
        /* [in] */ Int32 defaultValue);

    CARAPI_(Int32) GetInt32ValueFromTable(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [in] */ const String& name,
        /* [in] */ Int32 defaultValue);

    CARAPI_(String) GetStringValueFromTable(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [in] */ const String& name,
        /* [in] */ const String& defaultValue);

    CARAPI_(String) GetOldDefaultDeviceName();

    CARAPI_(String) GetDefaultDeviceName();

private:
    static const String TAG;
    static const String DATABASE_NAME;

    // Please, please please. If you update the database version, check to make sure the
    // database gets upgraded properly. At a minimum, please confirm that 'upgradeVersion'
    // is properly propagated through your change.  Not doing so will result in a loss of user
    // settings.
    static const Int32 DATABASE_VERSION;

    AutoPtr<IContext> mContext;
    Int32 mUserHandle;

    static HashSet<String> sValidTables;

    static const String TABLE_SYSTEM;
    static const String TABLE_SECURE;
    static const String TABLE_GLOBAL;

    static const Boolean sValidTablesInited;
};

} // namespace SettingsProvider
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGSPROVIDER_DATABASEHELPER_H__
