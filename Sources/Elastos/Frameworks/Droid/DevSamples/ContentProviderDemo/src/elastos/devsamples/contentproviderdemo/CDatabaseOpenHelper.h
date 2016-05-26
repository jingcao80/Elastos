#ifndef __Elastos_DevSamples_ContentProviderDemo_CDatabaseOpenHelper_H__
#define __Elastos_DevSamples_ContentProviderDemo_CDatabaseOpenHelper_H__

#include "_Elastos_DevSamples_ContentProviderDemo_CDatabaseOpenHelper.h"
#include <Elastos.Droid.Content.h>
#include <elastos/droid/database/sqlite/SQLiteOpenHelper.h>

using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;

namespace Elastos {
namespace DevSamples {
namespace ContentProviderDemo {

CarClass(CDatabaseOpenHelper)
    , public SQLiteOpenHelper
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI OnCreate(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 newVersion);

    CARAPI Add(
        /* [in] */ const String& username,
        /* [in] */ const String& sex,
        /* [in] */ const String& email);
};

} // namespace ContentProviderDemo
} // namespace DevSamples
} // namespace Elastos

#endif //__Elastos_DevSamples_ContentProviderDemo_CDatabaseOpenHelper_H__
