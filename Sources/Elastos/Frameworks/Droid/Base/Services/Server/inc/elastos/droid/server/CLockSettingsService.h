
#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_CLOCKSETTINGSSERVICE_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_CLOCKSETTINGSSERVICE_H__

#include "_Elastos_Droid_Widget_Internal_CLockSettingsService.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <database/sqlite/SQLiteOpenHelper.h>

using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::ISQLiteOpenHelper;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

/**
 * Keeps the lock pattern/password data and related settings for each user.
 * Used by LockPatternUtils. Needs to be a service because Settings app also needs
 * to be able to save lockscreen information for secondary users.
 * @hide
 */
CarClass(CLockSettingsService)
{
private:
    class DatabaseHelper
        : public ElRefBase
        , public ISQLiteOpenHelper
        , public SQLiteOpenHelper
    {
    private:
        static String TAG;
        static String DATABASE_NAME;

        static const Int32 DATABASE_VERSION;

        CARAPI_(void) CreateTable(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) InitializeDefaults(
            /* [in] */ ISQLiteDatabase* db);

    public:
        CAR_INTERFACE_DECL();

        DatabaseHelper(
            /* [in] */ IContext* context,
            /* [in] */ CLockSettingsService* host);

        virtual CARAPI OnCreate(
            /* [in] */ ISQLiteDatabase* db);

        virtual CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 currentVersion);

        virtual CARAPI GetDatabaseName(
            /* [out] */ String* name);

        virtual CARAPI SetWriteAheadLoggingEnabled(
            /* [in] */ Boolean enabled);

        virtual CARAPI GetWritableDatabase(
            /* [out] */ ISQLiteDatabase** database);

        virtual CARAPI GetReadableDatabase(
            /* [out] */ ISQLiteDatabase** database);

        virtual CARAPI Close();

        virtual CARAPI OnConfigure(
            /* [in] */ ISQLiteDatabase* db);

        virtual CARAPI OnDowngrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 newVersion);

        virtual CARAPI OnOpen(
            /* [in] */ ISQLiteDatabase* db);

    private:
        CLockSettingsService* mHost;
    };

public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SystemReady();

    virtual CARAPI SetBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean value,
        /* [in] */ Int32 userId);

    virtual CARAPI SetInt64(
        /* [in] */ const String& key,
        /* [in] */ Int64 value,
        /* [in] */ Int32 userId);

    virtual CARAPI SetString(
        /* [in] */ const String& key,
        /* [in] */ const String& value,
        /* [in] */ Int32 userId);

    virtual CARAPI GetBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean defaultValue,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* res);

    virtual CARAPI GetInt64(
        /* [in] */ const String& key,
        /* [in] */ Int64 defaultValue,
        /* [in] */ Int32 userId,
        /* [out] */ Int64* res);

    virtual CARAPI GetString(
        /* [in] */ const String& key,
        /* [in] */ const String& defaultValue,
        /* [in] */ Int32 userId,
        /* [out] */ String* retValue);

    virtual CARAPI HavePassword(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* res);

    virtual CARAPI HavePattern(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* res);

    virtual CARAPI SetLockPattern(
        /* [in] */ const ArrayOf<Byte>& hash,
        /* [in] */ Int32 userId);

    virtual CARAPI CheckPattern(
        /* [in] */ const ArrayOf<Byte>& hash,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* res);

    virtual CARAPI SetLockPassword(
        /* [in] */ const ArrayOf<Byte>& hash,
        /* [in] */ Int32 userId);

    virtual CARAPI CheckPassword(
        /* [in] */ const ArrayOf<Byte>& hash,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* res);

    virtual CARAPI RemoveUser(
        /* [in] */ Int32 userId);

private:
    CARAPI_(void) MigrateOldData();

    static CARAPI_(void) CheckWritePermission(
        /* [in] */ Int32 userId);

    static CARAPI_(void) CheckPasswordReadPermission(
        /* [in] */ Int32 userId);

    static CARAPI_(void) CheckReadPermission(
        /* [in] */ Int32 userId);

    CARAPI_(String) GetLockPatternFilename(
        /* [in] */ Int32 userId);

    CARAPI_(String) GetLockPasswordFilename(
        /* [in] */ Int32 userId);

    CARAPI_(void) WriteFile(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Byte>* hash);

    CARAPI_(void) WriteToDb(
        /* [in] */ const String& key,
        /* [in] */ const String& value,
        /* [in] */ Int32 userId);

    CARAPI_(void) WriteToDb(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& key,
        /* [in] */ const String& value, Int32 userId);

    CARAPI ReadFromDb(
        /* [in] */ const String& key,
        /* [in] */ const String& defaultValue,
        /* [in] */ Int32 userId,
        /* [out]*/ String* value);

private:
    static Boolean InitArray();

private:
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

    static AutoPtr<ArrayOf<String> > VALID_SETTINGS;
    static Boolean sInitArray;
};

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif  // __ELASTOS_DROID_WIDGET_INTERNAL_CLOCKSETTINGSSERVICE_H__
