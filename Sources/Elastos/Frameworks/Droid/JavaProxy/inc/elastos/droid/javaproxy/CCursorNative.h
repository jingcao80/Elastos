
#ifndef __ELASTOS_DROID_JAVAPROXY_CCURSORNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CCURSORNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CCursorNative.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <jni.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICharArrayBuffer;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::IO::ICloseable;
using Elastos::Utility::Etl::HashMap;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Database::IContentObserver)
DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Database::IDataSetObserver)

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CCursorNative)
    , public Object
    , public ICursor
    , public ICloseable
    , public IBinder
{
public:
    ~CCursorNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI GetCount(
        /* [out] */ Int32* count);

    CARAPI GetPosition(
        /* [out] */ Int32* position);

    CARAPI Move(
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* succeeded);

    CARAPI MoveToPosition(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* succeeded);

    CARAPI MoveToFirst(
        /* [out] */ Boolean* succeeded);

    CARAPI MoveToLast(
        /* [out] */ Boolean* succeeded);

    CARAPI MoveToNext(
        /* [out] */ Boolean* succeeded);

    CARAPI MoveToPrevious(
        /* [out] */ Boolean* succeeded);

    CARAPI IsFirst(
        /* [out] */ Boolean* isFirst);

    CARAPI IsLast(
        /* [out] */ Boolean* isLast);

    CARAPI IsBeforeFirst(
        /* [out] */ Boolean* isBeforeFirst);

    CARAPI IsAfterLast(
        /* [out] */ Boolean* isAfterLast);

    CARAPI GetColumnIndex(
        /* [in] */ const String& columnName,
        /* [out] */ Int32* columnIndex);

    CARAPI GetColumnIndexOrThrow(
        /* [in] */ const String& columnName,
        /* [out] */ Int32* columnIndex);

    CARAPI GetColumnName(
        /* [in] */ Int32 columnIndex,
        /* [out] */ String* columnName);

    CARAPI GetColumnNames(
        /* [out, callee] */ ArrayOf<String>** columnNames);

    CARAPI GetColumnCount(
        /* [out] */ Int32* columnCount);

    CARAPI GetBlob(
        /* [in] */ Int32 columnIndex,
        /* [out, callee] */ ArrayOf<Byte>** blob);

    CARAPI GetString(
        /* [in] */ Int32 columnIndex,
        /* [out] */ String* columnValue);

    CARAPI CopyStringToBuffer(
        /* [in] */ Int32 columnIndex,
        /* [in] */ ICharArrayBuffer* buf);

    CARAPI GetInt16(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Int16* columnValue);

    CARAPI GetInt32(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Int32* columnValue);

    CARAPI GetInt64(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Int64* columnValue);

    CARAPI GetFloat(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Float* columnValue);

    CARAPI GetDouble(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Double* columnValue);

    CARAPI GetType(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Int32* index);

    CARAPI IsNull(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Boolean* isNull);

    CARAPI Deactivate();

    CARAPI Requery(
        /* [out] */ Boolean* succeeded);

    CARAPI Close();

    CARAPI IsClosed(
        /* [out] */ Boolean* closed);

    CARAPI RegisterContentObserver(
        /* [in] */ IContentObserver* observer);

    CARAPI UnregisterContentObserver(
        /* [in] */ IContentObserver* observer);

    CARAPI RegisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    CARAPI UnregisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    CARAPI SetNotificationUri(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IUri* uri);

    CARAPI GetNotificationUri(
        /* [out] */ IUri** uri);

    CARAPI GetWantsAllOnMoveCalls(
        /* [out] */ Boolean* result);

    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    CARAPI Respond(
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;

    static HashMap<IContentObserver*, jobject> mContentObserverMap;
    static HashMap<IDataSetObserver*, jobject> mDataSetObserverMap;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CCURSORNATIVE_H__
