#ifndef __ELASTOS_DROID_DATABASE_BULKCURSORTOCURSORADAPTOR_H__
#define __ELASTOS_DROID_DATABASE_BULKCURSORTOCURSORADAPTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/database/AbstractWindowedCursor.h"
#include "elastos/droid/database/CBulkCursorDescriptor.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * Adapts an {@link IBulkCursor} to a {@link Cursor} for use in the local process.
 *
 * {@hide}
 */
class BulkCursorToCursorAdaptor : public AbstractWindowedCursor
{
public:
    BulkCursorToCursorAdaptor();

    /**
     * Initializes the adaptor.
     * Must be called before first use.
     */
    CARAPI Initialize(
        /* [in] */ CBulkCursorDescriptor* d);

    /**
     * Gets a SelfDataChangeOberserver that can be sent to a remote
     * process to receive change notifications over IPC.
     *
     * @return A SelfContentObserver hooked up to this Cursor
     */
    CARAPI GetObserver(
        /* [out] */ IIContentObserver** observer);

    //@Override
    CARAPI GetCount(
        /* [out] */ Int32* count);

    //@Override
    CARAPI OnMove(
        /* [in] */ Int32 oldPosition,
        /* [in] */ Int32 newPosition,
        /* [out] */ Boolean* succeeded);

    //@Override
    CARAPI Deactivate();

    //@Override
    CARAPI Close();

    //@Override
    CARAPI Requery(
        /* [out] */ Boolean* succeeded);

    //@Override
    CARAPI GetColumnNames(
        /* [out] */ ArrayOf<String>** names);

    //@Override
    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    //@Override
    CARAPI Respond(
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** v);

    CARAPI IsBlob(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Boolean* result);

    CARAPI IsString(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Boolean* result);

    CARAPI IsInt64(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Boolean* result);

    CARAPI IsFloat(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Boolean* result);

    CARAPI SetWindow(
        /* [in] */ ICursorWindow* window);

    CARAPI HasWindow(
        /* [out] */ Boolean* result);

    CARAPI GetNotificationUri(
        /* [out] */ IUri** uri);

    CARAPI OnChange(
        /* [in] */ Boolean selfChange);

    CARAPI GetWindow(
        /* [out] */ ICursorWindow** window);

    CARAPI FillWindow(
        /* [in] */ Int32 pos,
        /* [in] */ ICursorWindow* window);

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

    CARAPI GetColumnCount(
        /* [out] */ Int32* columnCount);

    CARAPI GetBlob(
        /* [in] */ Int32 columnIndex,
        /* [out,callee] */ ArrayOf<Byte>** blob);

    CARAPI GetString(
        /* [in] */ Int32 columnIndex,
        /* [out] */ String* columnValue);

    CARAPI CopyStringToBuffer(
        /* [in] */ Int32 columnIndex,
        /* [in, out] */ ICharArrayBuffer* buf);

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

    CARAPI GetWantsAllOnMoveCalls(
        /* [out] */ Boolean* result);

private:
    CARAPI ThrowIfCursorIsClosed();

private:
    static const String TAG;

    AutoPtr<SelfContentObserver> mObserverBridge;
    AutoPtr<IBulkCursor> mBulkCursor;
    AutoPtr<ArrayOf<String> > mColumns;
    Boolean mWantsAllOnMoveCalls;
    Int32 mCount;
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_BULKCURSORTOCURSORADAPTOR_H__
