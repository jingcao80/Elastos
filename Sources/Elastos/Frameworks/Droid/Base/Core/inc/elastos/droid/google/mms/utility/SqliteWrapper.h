#ifndef __ELASTOS_DROID_GOOGLE_MMS_UTILITY_SQLITEWRAPPER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_UTILITY_SQLITEWRAPPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package com.google.android.mms.util;
// import android.app.ActivityManager;
// import android.content.ContentResolver;
// import android.content.ContentValues;
// import android.content.Context;
// import android.database.Cursor;
// import android.database.sqlite.SQLiteException;
// import android.net.Uri;
// import android.util.Log;
// import android.widget.Toast;

using Elastos::Droid::Content::IContext;
//using Elastos::Droid::Database::Sqlite::ISQLiteException;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContentValues;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

class SqliteWrapper
    : public Object
{
public:
    static CARAPI CheckSQLiteException(
        /* [in] */ IContext* context);
        ///* [in] */ ISQLiteException* e);

    static CARAPI_(AutoPtr<ICursor>) Query(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder);

    static CARAPI_(Boolean) Requery(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor);

    static CARAPI_(Int32) Update(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* selectionArgs);

    static CARAPI_(Int32) Delete(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* selectionArgs);

    static CARAPI_(AutoPtr<IUri>) Insert(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values);

private:
    SqliteWrapper();

    // FIXME: It looks like outInfo.lowMemory does not work well as we expected.
    // after run command: adb shell fillup -p 100, outInfo.lowMemory is still false.
    static CARAPI_(Boolean) IsLowMemory(
        /* [in] */ IContext* context);

    // FIXME: need to optimize this method.
    static CARAPI_(Boolean) IsLowMemory();
        ///* [in] */ ISQLiteException* e);

private:
    static const String TAG;
    static const String SQLITE_EXCEPTION_DETAIL_MESSAGE;
};

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_UTILITY_SQLITEWRAPPER_H__
