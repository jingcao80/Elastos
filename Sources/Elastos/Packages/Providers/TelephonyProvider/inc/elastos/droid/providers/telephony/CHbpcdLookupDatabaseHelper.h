
#ifndef __ELASTOS_DROID_PROVIDER_TELEPHONY_CHBPCDLOOKUPDATABASEHELPER_H__
#define __ELASTOS_DROID_PROVIDER_TELEPHONY_CHBPCDLOOKUPDATABASEHELPER_H__

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Providers_Telephony_CHbpcdLookupDatabaseHelper.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

CarClass(CHbpcdLookupDatabaseHelper)
    , public SQLiteOpenHelper
{
public:
    CAR_OBJECT_DECL()

    CHbpcdLookupDatabaseHelper();

    CARAPI constructor();

    CARAPI constructor(
            /* [in] */ IContext* context);

    // @Override
    CARAPI OnCreate(
        /* [in] */ ISQLiteDatabase* db);

    // @Override
    CARAPI OnUpgrade(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int32 oldVersion,
        /* [in] */ Int32 newVersion);

private:
    CARAPI_(void) InitDatabase(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) LoadTable(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& tableName);

    CARAPI_(AutoPtr<IContentValues>) GetTableMccIddRow(
        /* [in] */ IXmlPullParser* parser);

    CARAPI_(AutoPtr<IContentValues>) GetTableMccLookupTableRow(
        /* [in] */ IXmlPullParser* parser);

    CARAPI_(AutoPtr<IContentValues>) GetTableMccSidConflictRow(
        /* [in] */ IXmlPullParser* parser);

    CARAPI_(AutoPtr<IContentValues>) GetTableMccSidRangeRow(
        /* [in] */ IXmlPullParser* parser);

    CARAPI_(AutoPtr<IContentValues>) GetTableNanpAreaCodeRow(
        /* [in] */ IXmlPullParser* parser);

    CARAPI_(AutoPtr<IContentValues>) GetTableArbitraryMccSidMatch(
        /* [in] */ IXmlPullParser* parser);

private:
    static const String TAG; // = "HbpcdLockupDatabaseHelper";
    static const Boolean DEBUG; // = TRUE;

    static const String DATABASE_NAME; // = "HbpcdLookup.db";
    static const Int32 DATABASE_VERSION = 1;

    AutoPtr<IContext> mContext;
};

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_TELEPHONY_CHBPCDLOOKUPDATABASEHELPER_H__
