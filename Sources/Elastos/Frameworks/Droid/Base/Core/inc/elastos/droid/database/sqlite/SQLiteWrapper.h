#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITEWRAPPER_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITEWRAPPER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Core.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {
/**
 * @hide
 */
class SQLiteWrapper
{
public:
    static CARAPI CheckSQLiteException(
        /* [in] */ IContext* context,
        /* [in] */ ECode e);

    static CARAPI Query(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** result);

    static CARAPI Requery(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [out] */ Boolean* result);

    static CARAPI Update(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* result);

    static CARAPI Delete(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* result);

    static CARAPI Insert(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [out] */ IUri** result);

private:
    SQLiteWrapper() {}

    // FIXME: need to optimize this method.
    static CARAPI_(Boolean) IsLowMemory(
        /* [in] */ ECode e);

private:
    const static String TAG;
    const static String SQLITE_EXCEPTION_DETAIL_MESSAGE;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_SQLITE_SQLITEWRAPPER_H__
