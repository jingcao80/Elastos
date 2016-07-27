
#ifndef __ELASTOS_DROID_PROVIDER_TELEPHONY_CTELEPHONYPROVIDER_H__
#define __ELASTOS_DROID_PROVIDER_TELEPHONY_CTELEPHONYPROVIDER_H__

#include "_Elastos_Droid_Providers_Telephony_CTelephonyProvider.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"

using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Core::IInteger64;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

CarClass(CTelephonyProvider)
    , public ContentProvider
{
private:
    class DatabaseHelper: public SQLiteOpenHelper
    {
    public:
        /**
         * DatabaseHelper helper class for loading apns into a database.
         *
         * @param context of the user.
         */
        DatabaseHelper(
            /* [in] */ IContext* context,
            /* [in] */ CTelephonyProvider* host);

        // @Override
        CARAPI OnCreate(
            /* [in] */ ISQLiteDatabase* db);

        // @Override
        CARAPI OnOpen(
            /* [in] */ ISQLiteDatabase* db);

        // @Override
        CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 newVersion);

        static CARAPI_(AutoPtr<IContentValues>) SetDefaultValue(
            /* [in] */ IContentValues* values);

        CARAPI_(void) InitDatabase(
            /* [in] */ ISQLiteDatabase* db);

    private:
        static CARAPI_(Int32) GetVersion(
            /* [in] */ IContext* context);

        CARAPI_(void) CreateSimInfoTable(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) CreateCarriersTable(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(Int32) GetDefaultPreferredApnId(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) UpgradeForSubIdIfNecessary(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) UpgradeForProfileIdIfNecessary(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) UpgradeForMtuIfNecessary(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) UpgradeForSimInfoIfNecssary(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) UpgradeForSubscriptionInfoIfNecessary(
            /* [in] */ ISQLiteDatabase* db);

        /**
         * Gets the next row of apn values.
         *
         * @param parser the parser
         * @return the row or NULL if it's not an apn
         */
        CARAPI_(AutoPtr<IContentValues>) GetRow(
            /* [in] */ IXmlPullParser* parser);

        /*
         * Loads apns from xml file into the database
         *
         * @param db the sqlite database to write to
         * @param parser the xml parser
         *
         */
        CARAPI_(void) LoadApns(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ IXmlPullParser* parser);

        CARAPI_(void) InsertAddingDefaults(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ const String& table,
            /* [in] */ IContentValues* row);

    private:
        // Context to access resources with
        AutoPtr<IContext> mContext;
        CTelephonyProvider* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [out] */ Boolean* succeeded);

    // @Override
    CARAPI Query(
        /* [in] */ IUri* url,
        /* [in] */ ArrayOf<String>* projectionIn,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sort,
        /* [out] */ ICursor** result);

    // @Override
    CARAPI GetType(
        /* [in] */ IUri* url,
        /* [out] */ String* result);

    // @Override
    CARAPI Insert(
        /* [in] */ IUri* url,
        /* [in] */ IContentValues* initialValues,
        /* [out] */ IUri** uri);

    // @Override
    CARAPI Delete(
        /* [in] */ IUri* url,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [out] */ Int32* result);

    // @Override
    CARAPI Update(
        /* [in] */ IUri* url,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [out] */ Int32* result);

private:
    CARAPI_(void) SetPreferredApnId(
        /* [in] */ IInteger64* id,
        /* [in] */ Int64 subId);

    CARAPI_(Int64) GetPreferredApnId(
        /* [in] */ Int64 subId);

    CARAPI_(Int64) GetDefaultPreferredApnId();

    CARAPI CheckPermission();

    CARAPI_(void) RestoreDefaultAPN(
        /* [in] */ Int64 subId);

    /**
     * Log with debug
     *
     * @param s is string Log
     */
    static CARAPI_(void) Log(
        /* [in] */ const String& s);

    static CARAPI_(void) Loge(
        /* [in] */ const String& s);

    static Boolean InitMember();

private:
    static const String DATABASE_NAME;
    static const Boolean DBG;
    static const Boolean VDBG;

    static const Int32 DATABASE_VERSION;
    static const Int32 URL_UNKNOWN;
    static const Int32 URL_TELEPHONY;
    static const Int32 URL_CURRENT;
    static const Int32 URL_ID;
    static const Int32 URL_RESTOREAPN;
    static const Int32 URL_PREFERAPN;
    static const Int32 URL_PREFERAPN_NO_UPDATE;
    static const Int32 URL_SIMINFO;
    static const Int32 URL_TELEPHONY_USING_SUBID;
    static const Int32 URL_CURRENT_USING_SUBID;
    static const Int32 URL_RESTOREAPN_USING_SUBID;
    static const Int32 URL_PREFERAPN_USING_SUBID;
    static const Int32 URL_PREFERAPN_NO_UPDATE_USING_SUBID;
    static const Int32 URL_SIMINFO_USING_SUBID;

    static const String TAG;
    static const String CARRIERS_TABLE;
    static const String SIMINFO_TABLE;

    static const String PREF_FILE;
    static const String COLUMN_APN_ID;

    static const String PARTNER_APNS_PATH;

    static AutoPtr<IUriMatcher> s_urlMatcher;

    static AutoPtr<IContentValues> s_currentNullMap;
    static AutoPtr<IContentValues> s_currentSetMap;
    static Boolean s_Init;
    AutoPtr<DatabaseHelper> mOpenHelper;
};

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_TELEPHONY_CTELEPHONYPROVIDER_H__
