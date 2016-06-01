
#include "elastos/droid/google/mms/utility/SqliteWrapper.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

//=====================================================================
//                            SqliteWrapper
//=====================================================================
const String SqliteWrapper::TAG("SqliteWrapper");
const String SqliteWrapper::SQLITE_EXCEPTION_DETAIL_MESSAGE("unable to open database file");

ECode SqliteWrapper::CheckSQLiteException(
    /* [in] */ IContext* context,
    /* [in] */ ISQLiteException* e)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(e);
    // ==================before translated======================
    // if (isLowMemory(e)) {
    //     Toast.makeText(context, com.android.internal.R.string.low_memory,
    //             Toast.LENGTH_SHORT).show();
    // } else {
    //     throw e;
    // }
    assert(0);
    return NOERROR;
}

AutoPtr<ICursor> SqliteWrapper::Query(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder)
{
    // ==================before translated======================
    // try {
    //     return resolver.query(uri, projection, selection, selectionArgs, sortOrder);
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Catch a SQLiteException when query: ", e);
    //     checkSQLiteException(context, e);
    //     return null;
    // }
    assert(0);
    AutoPtr<ICursor> empty;
    return empty;
}

Boolean SqliteWrapper::Requery(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor)
{
    // ==================before translated======================
    // try {
    //     return cursor.requery();
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Catch a SQLiteException when requery: ", e);
    //     checkSQLiteException(context, e);
    //     return false;
    // }
    assert(0);
    return FALSE;
}

Int32 SqliteWrapper::Update(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    // ==================before translated======================
    // try {
    //     return resolver.update(uri, values, where, selectionArgs);
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Catch a SQLiteException when update: ", e);
    //     checkSQLiteException(context, e);
    //     return -1;
    // }
    assert(0);
    return 0;
}

Int32 SqliteWrapper::Delete(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    // ==================before translated======================
    // try {
    //     return resolver.delete(uri, where, selectionArgs);
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Catch a SQLiteException when delete: ", e);
    //     checkSQLiteException(context, e);
    //     return -1;
    // }
    assert(0);
    return 0;
}

AutoPtr<IUri> SqliteWrapper::Insert(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values)
{
    // ==================before translated======================
    // try {
    //     return resolver.insert(uri, values);
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Catch a SQLiteException when insert: ", e);
    //     checkSQLiteException(context, e);
    //     return null;
    // }
    assert(0);
    AutoPtr<IUri> empty;
    return empty;
}

SqliteWrapper::SqliteWrapper()
{
    // ==================before translated======================
    // // Forbidden being instantiated.
}

Boolean SqliteWrapper::IsLowMemory(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // if (null == context) {
    //     return false;
    // }
    //
    // ActivityManager am = (ActivityManager)
    //                 context.getSystemService(Context.ACTIVITY_SERVICE);
    // ActivityManager.MemoryInfo outInfo = new ActivityManager.MemoryInfo();
    // am.getMemoryInfo(outInfo);
    //
    // return outInfo.lowMemory;
    assert(0);
    return FALSE;
}

Boolean SqliteWrapper::IsLowMemory(
    /* [in] */ ISQLiteException* e)
{
    // ==================before translated======================
    // return e.getMessage().equals(SQLITE_EXCEPTION_DETAIL_MESSAGE);
    assert(0);
    return FALSE;
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
