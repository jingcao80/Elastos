
#ifndef __ELASTOS_DROID_PROVIDER_TELEPHONY_CBLACKLISTPROVIDER_H__
#define __ELASTOS_DROID_PROVIDER_TELEPHONY_CBLACKLISTPROVIDER_H__

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.App.h"
#include "_Elastos_Droid_Providers_Telephony_CBlacklistProvider.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"

using Elastos::Droid::App::Backup::IBackupManager;
using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::Utility::IPair;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

CarClass(CBlacklistProvider)
    , public ContentProvider
{
private:
    class DatabaseHelper: public SQLiteOpenHelper
    {
    public:
        DatabaseHelper(
            /* [in] */ IContext* context,
            /* [in] */ CBlacklistProvider* host);

        // @Override
        CARAPI OnCreate(
            /* [in] */ ISQLiteDatabase* db);

        // @Override
        CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 newVersion);

    private:
        // Context to access resources with
        AutoPtr<IContext> mContext;
        CBlacklistProvider* mHost;
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
    CARAPI_(AutoPtr<IContentValues>) ValidateAndPrepareContentValues(
        /* [in] */ IContentValues* initialValues,
        /* [in] */ const String& uriNumber);

    CARAPI_(void) NotifyChange();

    /**
     * Normalizes the passed in number and tries to format it according to E164.
     * Returns a pair of
     * - normalized number
     * - Boolean indicating whether the number is a E164 number or not
     */
    static CARAPI_(AutoPtr<IPair>) NormalizeNumber(
        /* [in] */ IContext* context,
        /* [in] */ const String& number);

    static CARAPI_(String) ToE164Number(
        /* [in] */ IContext* context,
        /* [in] */ const String& src);

    static CARAPI_(Boolean) IsValidPhoneNumber(
        /* [in] */ const String& address);

    static CARAPI_(Boolean) InitMember();

private:
    static const String TAG; // = "BlacklistProvider";
    static const Boolean DEBUG; // = TRUE;

    static const String DATABASE_NAME; // = "blacklist.db";
    static const Int32 DATABASE_VERSION = 4;

    static const String BLACKLIST_TABLE; // = "blacklist";
    static const String COLUMN_NORMALIZED; // = "normalized_number";

    static const Int32 BL_ALL         = 0;
    static const Int32 BL_ID          = 1;
    static const Int32 BL_NUMBER      = 2;
    static const Int32 BL_PHONE       = 3;
    static const Int32 BL_MESSAGE     = 4;

    static AutoPtr<IUriMatcher> sURIMatcher; // = new UriMatcher(UriMatcher.NO_MATCH);

    AutoPtr<DatabaseHelper> mOpenHelper;
    AutoPtr<IBackupManager> mBackupManager;

    static Boolean s_Init;
};

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_TELEPHONY_CBLACKLISTPROVIDER_H__
