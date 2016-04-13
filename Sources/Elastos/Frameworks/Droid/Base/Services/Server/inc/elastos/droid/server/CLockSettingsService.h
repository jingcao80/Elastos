
#ifndef __ELASTOS_DROID_SERVER_CLOCKSETTINGSSERVICE_H__
#define __ELASTOS_DROID_SERVER_CLOCKSETTINGSSERVICE_H__

#include "_Elastos_Droid_Server_CLockSettingsService.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/database/sqlite/SQLiteOpenHelper.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::Internal::Os::IBackgroundThread;
using Elastos::Droid::Internal::Widget::IILockSettings;
using Elastos::Droid::Internal::Widget::IILockSettingsObserver;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Keeps the lock pattern/password data and related settings for each user.
 * Used by LockPatternUtils. Needs to be a service because Settings app also needs
 * to be able to save lockscreen information for secondary users.
 * @hide
 */
CarClass(CLockSettingsService)
    , public Object
    , public IILockSettings
    , public IBinder
{
private:
    class UserAddedBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        UserAddedBroadcastReceiver(
            /* [in] */ CLockSettingsService* lss);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CLockSettingsService* mHost;
    };

    class DatabaseHelper
        : public SQLiteOpenHelper
    {
    public:
        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ CLockSettingsService* host);

        CARAPI OnCreate(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 currentVersion);

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

        CARAPI OnDowngrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 newVersion);

        CARAPI OnOpen(
            /* [in] */ ISQLiteDatabase* db);

    private:
        CARAPI MaybeEnableWidgetSettingForUsers(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) CreateTable(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) InitializeDefaults(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI LoadSetting(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ const String& key,
            /* [in] */ Int32 userId,
            /* [in] */ Boolean value);

    private:
        static String TAG;
        static String DATABASE_NAME;

        static const Int32 DATABASE_VERSION;

        CLockSettingsService* mHost;
    };

    class LockSettingsObserver
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        LockSettingsObserver(
            /* [in] */ CLockSettingsService* lss);

        //@Override
        CARAPI ProxyDied();

    public:
        AutoPtr<IILockSettingsObserver> mRemote;

    private:
        CLockSettingsService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CLockSettingsService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SystemReady();

    CARAPI SetBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean value,
        /* [in] */ Int32 userId);

    CARAPI SetInt64(
        /* [in] */ const String& key,
        /* [in] */ Int64 value,
        /* [in] */ Int32 userId);

    CARAPI SetString(
        /* [in] */ const String& key,
        /* [in] */ const String& value,
        /* [in] */ Int32 userId);

    CARAPI GetBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean defaultValue,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* res);

    CARAPI GetInt64(
        /* [in] */ const String& key,
        /* [in] */ Int64 defaultValue,
        /* [in] */ Int32 userId,
        /* [out] */ Int64* res);

    CARAPI GetString(
        /* [in] */ const String& key,
        /* [in] */ const String& defaultValue,
        /* [in] */ Int32 userId,
        /* [out] */ String* retValue);

    CARAPI GetLockPatternSize(
        /* [in] */ Int32 userId,
        /* [out] */ Byte* result);

    CARAPI RegisterObserver(
        /* [in] */ IILockSettingsObserver* remote);

    CARAPI UnregisterObserver(
        /* [in] */ IILockSettingsObserver* remote);

    CARAPI NotifyObservers(
        /* [in] */ const String& key,
        /* [in] */ Int32 userId);

    CARAPI HavePassword(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* res);

    CARAPI HavePattern(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* res);

    CARAPI SetLockPattern(
        /* [in] */ const String& hash,
        /* [in] */ Int32 userId);

    CARAPI SetLockPassword(
        /* [in] */ const String& password,
        /* [in] */ Int32 userId);

    CARAPI CheckPattern(
        /* [in] */ const String& hash,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* res);

    CARAPI CheckPassword(
        /* [in] */ const String& hash,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* res);

    CARAPI CheckVoldPassword(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* res);

    CARAPI RemoveUser(
        /* [in] */ Int32 userId);

    CARAPI ToString(
        /* [out] */ String* str)
    {
        return Object::ToString(str);
    }

private:
    CARAPI_(void) MigrateOldData();

    CARAPI CheckWritePermission(
        /* [in] */ Int32 userId);

    CARAPI CheckPasswordReadPermission(
        /* [in] */ Int32 userId);

    CARAPI CheckReadPermission(
        /* [in] */ const String& requestedKey,
        /* [in] */ Int32 userId);

    Int32 GetUserParentOrSelfId(
        /* [in] */ Int32 userId);

    void MaybeUpdateKeystore(
        /* [in] */ const String& password,
        /* [in] */ Int32 userHandle);

    CARAPI_(String) GetLockPatternFilename(
        /* [in] */ Int32 userId);

    CARAPI_(String) GetLockPatternFilename(
        /* [in] */ Int32 userId,
        /* [in] */ Boolean defaultSize);

    CARAPI_(String) GetLockPasswordFilename(
        /* [in] */ Int32 userId);

    CARAPI WriteFile(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Byte>* hash);

    CARAPI WriteToDb(
        /* [in] */ const String& key,
        /* [in] */ const String& value,
        /* [in] */ Int32 userId);

    CARAPI WriteToDb(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& key,
        /* [in] */ const String& value, Int32 userId);

    CARAPI ReadFromDb(
        /* [in] */ const String& key,
        /* [in] */ const String& defaultValue,
        /* [in] */ Int32 userId,
        /* [out]*/ String* value);

    AutoPtr<IIMountService> GetMountService();

    CARAPI_(Boolean) IsDefaultSize(
        /* [in] */ Int32 userId);

private:

    static String PERMISSION;
    static String SYSTEM_DEBUGGABLE;

    AutoPtr<DatabaseHelper> mOpenHelper;
    static String TAG;

    static String TABLE;
    static String COLUMN_KEY;
    static String COLUMN_USERID;
    static String COLUMN_VALUE;

    static AutoPtr<ArrayOf<String> > COLUMNS_FOR_QUERY;

    static String SYSTEM_DIRECTORY;
    static String LOCK_PATTERN_FILE;
    static String LOCK_PASSWORD_FILE;

    AutoPtr<IContext> mContext;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    Boolean mFirstCallToVold;

    List<AutoPtr<LockSettingsObserver> > mObservers;
    Object mObserversLock;

    static AutoPtr<ArrayOf<String> > VALID_SETTINGS;

    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;

    static AutoPtr<ArrayOf<String> > READ_PROFILE_PROTECTED_SETTINGS;
};

}// namespace Server
}// namespace Droid
}// namespace Elastos

#endif  // __ELASTOS_DROID_SERVER_CLOCKSETTINGSSERVICE_H__
