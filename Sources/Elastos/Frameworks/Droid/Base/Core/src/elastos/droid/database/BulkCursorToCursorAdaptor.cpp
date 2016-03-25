
#include "elastos/droid/database/BulkCursorToCursorAdaptor.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include "elastos/droid/os/CBundle.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::CBundle;
using Elastos::IO::ICloseable;
using Elastos::IO::EIID_ICloseable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {

const String BulkCursorToCursorAdaptor::TAG("BulkCursor");

BulkCursorToCursorAdaptor::BulkCursorToCursorAdaptor()
{
    mObserverBridge = new SelfContentObserver();
    mObserverBridge->constructor(this);
}

ECode BulkCursorToCursorAdaptor::Initialize(
    /* [in] */ CBulkCursorDescriptor* d)
{
    mBulkCursor = d->mCursor;
    mColumns = d->mColumnNames;
    mRowIdColumnIndex = DatabaseUtils::FindRowIdColumnIndex(mColumns);
    mWantsAllOnMoveCalls = d->mWantsAllOnMoveCalls;
    mCount = d->mCount;
    if (d->mWindow != NULL) {
        SetWindow(d->mWindow);
    }
    return NOERROR;
}

ECode BulkCursorToCursorAdaptor::GetObserver(
    /* [out] */ IIContentObserver** observer)
{
    VALIDATE_NOT_NULL(observer)
    return mObserverBridge->GetContentObserver(observer);
}

ECode BulkCursorToCursorAdaptor::ThrowIfCursorIsClosed()
{
    if (mBulkCursor == NULL) {
        //throw new StaleDataException("Attempted to access a cursor after it has been closed.");
        Slogger::E(TAG, "Attempted to access a cursor after it has been closed.");
        return E_STALE_DATA_EXCEPTION;
    }
    return NOERROR;
}

ECode BulkCursorToCursorAdaptor::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    FAIL_RETURN(ThrowIfCursorIsClosed())
    *count = mCount;
    return NOERROR;
}

ECode BulkCursorToCursorAdaptor::OnMove(
    /* [in] */ Int32 oldPosition,
    /* [in] */ Int32 newPosition,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    FAIL_RETURN(ThrowIfCursorIsClosed())
    //try {
    // Make sure we have the proper window
    Int32 pos, num;
    if (mWindow == NULL || (mWindow->GetStartPosition(&pos), newPosition < pos)
            || (mWindow->GetNumRows(&num), newPosition >= (pos + num))) {
        AutoPtr<ICursorWindow> win;
        ECode ec = mBulkCursor->GetWindow(newPosition, (ICursorWindow**)&win);
        if (FAILED(ec)) {
            // We tried to get a window and failed
            Slogger::E(TAG, "Unable to get window because the remote process is dead");
            *succeeded = FALSE;
            return ec;
        }
        SetWindow(win);
    }
    else if (mWantsAllOnMoveCalls) {
        ECode ec = mBulkCursor->OnMove(newPosition);
        if (FAILED(ec)) {
            // We tried to get a window and failed
            Slogger::E(TAG, "Unable to get window because the remote process is dead");
            *succeeded = FALSE;
            return NOERROR;
        }
    }
    //} catch (RemoteException ex) {
        // We tried to get a window and failed
        // Log.e(TAG, "Unable to get window because the remote process is dead");
        // return false;
    //}

    if (mWindow == NULL) {
        *succeeded = FALSE;
        return NOERROR;
    }
    *succeeded = TRUE;
    return NOERROR;
}

ECode BulkCursorToCursorAdaptor::Deactivate()
{
    // This will call onInvalidated(), so make sure to do it before calling release,
    // which is what actually makes the data set invalid.
    FAIL_RETURN(AbstractWindowedCursor::Deactivate())
    ECode ec = NOERROR;
    if(mBulkCursor != NULL) {
        //try {
        ec = mBulkCursor->Deactivate();
        //} catch (RemoteException ex) {
        if(FAILED(ec)) {
            Slogger::W(TAG, "Remote process exception when deactivating");
        }
        //}
    }
    return ec;
}

ECode BulkCursorToCursorAdaptor::Close()
{
    FAIL_RETURN(AbstractWindowedCursor::Close())
    ECode ec = NOERROR;
    if (mBulkCursor != NULL) {
        //try {
        ec = mBulkCursor->Close();
        //} catch (RemoteException ex) {
        if(FAILED(ec)) {
            Slogger::W(TAG, String("Remote process exception when closing"));
        }
        //} finally {
        mBulkCursor = NULL;
        //}
    }
    return ec;
}

ECode BulkCursorToCursorAdaptor::Requery(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    FAIL_RETURN(ThrowIfCursorIsClosed())
    Boolean issuccess;

    //try {
    AutoPtr<IIContentObserver> observer;
    GetObserver((IIContentObserver**)&observer);
    ECode ec = mBulkCursor->Requery(observer, &mCount);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Unable to requery because the remote process exception , 0x%08x", ec);
        Deactivate();
        *succeeded = FALSE;
        return ec;
    }
    if (mCount != -1) {
        mPos = -1;
        CloseWindow();

        // super.requery() will call onChanged. Do it here instead of relying on the
        // observer from the far side so that observers can see a correct value for mCount
        // when responding to onChanged.
        AbstractWindowedCursor::Requery(&issuccess);
        *succeeded = TRUE;
    }
    else {
        Deactivate();
        *succeeded = FALSE;
    }
    //} catch (Exception ex) {
        // Log.e(TAG, "Unable to requery because the remote process exception " + ex.getMessage());
        // deactivate();
        // return false;
    //}
    return NOERROR;
}

ECode BulkCursorToCursorAdaptor::GetColumnNames(
    /* [out] */ ArrayOf<String>** names)
{
    FAIL_RETURN(ThrowIfCursorIsClosed())
    *names = mColumns;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode BulkCursorToCursorAdaptor::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    FAIL_RETURN(ThrowIfCursorIsClosed())
    //try {
    return mBulkCursor->GetExtras(extras);
    //} catch (RemoteException e) {
        // This should never happen because the system kills processes that are using remote
        // cursors when the provider process is killed.
        // throw new RuntimeException(e);
    //}
}

ECode BulkCursorToCursorAdaptor::Respond(
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** v)
{
    VALIDATE_NOT_NULL(v)
    //try {
    ECode ec = mBulkCursor->Respond(extras, v);
    //} catch (RemoteException e) {
        // the system kills processes that are using remote cursors when the provider process
        // is killed, but this can still happen if this is being called from the system process,
        // so, better to log and return an empty bundle.
     if(FAILED(ec)) {
        Slogger::W(TAG, "respond() threw RemoteException, returning an empty bundle. 0x%08x");
        *v =  CBundle::EMPTY;
        REFCOUNT_ADD(*v)
     }
    //}
    return ec;
}

ECode BulkCursorToCursorAdaptor::IsBlob(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return AbstractWindowedCursor::IsBlob(columnIndex, result);
}

ECode BulkCursorToCursorAdaptor::IsString(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return AbstractWindowedCursor::IsString(columnIndex, result);
}

ECode BulkCursorToCursorAdaptor::IsInt64(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return AbstractWindowedCursor::IsInt64(columnIndex, result);
}

ECode BulkCursorToCursorAdaptor::IsFloat(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return AbstractWindowedCursor::IsFloat(columnIndex, result);
}

ECode BulkCursorToCursorAdaptor::SetWindow(
    /* [in] */ ICursorWindow* window)
{
    return AbstractWindowedCursor::SetWindow(window);
}

ECode BulkCursorToCursorAdaptor::HasWindow(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return AbstractWindowedCursor::HasWindow(result);
}

ECode BulkCursorToCursorAdaptor::GetNotificationUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    return AbstractWindowedCursor::GetNotificationUri(uri);
}

ECode BulkCursorToCursorAdaptor::OnChange(
    /* [in] */ Boolean selfChange)
{
    return AbstractWindowedCursor::OnChange(selfChange);
}

ECode BulkCursorToCursorAdaptor::GetWindow(
    /* [out] */ ICursorWindow** window)
{
     VALIDATE_NOT_NULL(window)
    return AbstractWindowedCursor::GetWindow(window);
}

ECode BulkCursorToCursorAdaptor::FillWindow(
    /* [in] */ Int32 pos,
    /* [in] */ ICursorWindow* window)
{
    return AbstractWindowedCursor::FillWindow(pos, window);
}

ECode BulkCursorToCursorAdaptor::GetPosition(
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position)
    return AbstractWindowedCursor::GetPosition(position);
}

ECode BulkCursorToCursorAdaptor::Move(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return AbstractWindowedCursor::Move(offset, succeeded);
}

ECode BulkCursorToCursorAdaptor::MoveToPosition(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return AbstractWindowedCursor::MoveToPosition(position, succeeded);
}

ECode BulkCursorToCursorAdaptor::MoveToFirst(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return AbstractWindowedCursor::MoveToFirst(succeeded);
}

ECode BulkCursorToCursorAdaptor::MoveToLast(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return AbstractWindowedCursor::MoveToLast(succeeded);
}

ECode BulkCursorToCursorAdaptor::MoveToNext(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return AbstractWindowedCursor::MoveToNext(succeeded);
}

ECode BulkCursorToCursorAdaptor::MoveToPrevious(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return AbstractWindowedCursor::MoveToPrevious(succeeded);
}

ECode BulkCursorToCursorAdaptor::IsFirst(
    /* [out] */ Boolean* isFirst)
{
    VALIDATE_NOT_NULL(isFirst)
    return AbstractWindowedCursor::IsFirst(isFirst);
}

ECode BulkCursorToCursorAdaptor::IsLast(
    /* [out] */ Boolean* isLast)
{
    VALIDATE_NOT_NULL(isLast)
    return AbstractWindowedCursor::IsLast(isLast);
}

ECode BulkCursorToCursorAdaptor::IsBeforeFirst(
    /* [out] */ Boolean* isBeforeFirst)
{
    VALIDATE_NOT_NULL(isBeforeFirst)
    return AbstractWindowedCursor::IsBeforeFirst(isBeforeFirst);
}

ECode BulkCursorToCursorAdaptor::IsAfterLast(
    /* [out] */ Boolean* isAfterLast)
{
    VALIDATE_NOT_NULL(isAfterLast)
    return AbstractWindowedCursor::IsAfterLast(isAfterLast);
}

ECode BulkCursorToCursorAdaptor::GetColumnIndex(
    /* [in] */ const String& columnName,
    /* [out] */ Int32* columnIndex)
{
    VALIDATE_NOT_NULL(columnIndex)
    return AbstractWindowedCursor::GetColumnIndex(columnName, columnIndex);
}

ECode BulkCursorToCursorAdaptor::GetColumnIndexOrThrow(
    /* [in] */ const String& columnName,
    /* [out] */ Int32* columnIndex)
{
    VALIDATE_NOT_NULL(columnIndex)
    return AbstractWindowedCursor::GetColumnIndexOrThrow(columnName, columnIndex);
}

ECode BulkCursorToCursorAdaptor::GetColumnName(
    /* [in] */ Int32 columnIndex,
    /* [out] */ String* columnName)
{
    VALIDATE_NOT_NULL(columnName)
    return AbstractWindowedCursor::GetColumnName(columnIndex, columnName);
}

ECode BulkCursorToCursorAdaptor::GetColumnCount(
    /* [out] */ Int32* columnCount)
{
    VALIDATE_NOT_NULL(columnCount)
    return AbstractWindowedCursor::GetColumnCount(columnCount);
}

ECode BulkCursorToCursorAdaptor::GetBlob(
    /* [in] */ Int32 columnIndex,
    /* [out, callee] */ ArrayOf<Byte>** blob)
{
    VALIDATE_NOT_NULL(blob)
    return AbstractWindowedCursor::GetBlob(columnIndex, blob);
}

ECode BulkCursorToCursorAdaptor::GetString(
    /* [in] */ Int32 columnIndex,
    /* [out] */ String* columnValue)
{
    VALIDATE_NOT_NULL(columnValue)
    return AbstractWindowedCursor::GetString(columnIndex, columnValue);
}

ECode BulkCursorToCursorAdaptor::CopyStringToBuffer(
    /* [in] */ Int32 columnIndex,
    /* [in, out] */ ICharArrayBuffer* buf)
{
    VALIDATE_NOT_NULL(buf)
    return AbstractWindowedCursor::CopyStringToBuffer(columnIndex, buf);
}

ECode BulkCursorToCursorAdaptor::GetInt16(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int16* columnValue)
{
    VALIDATE_NOT_NULL(columnValue)
    return AbstractWindowedCursor::GetInt16(columnIndex, columnValue);
}

ECode BulkCursorToCursorAdaptor::GetInt32(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int32* columnValue)
{
    VALIDATE_NOT_NULL(columnValue)
    return AbstractWindowedCursor::GetInt32(columnIndex, columnValue);
}

ECode BulkCursorToCursorAdaptor::GetInt64(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int64* columnValue)
{
    VALIDATE_NOT_NULL(columnValue)
    return AbstractWindowedCursor::GetInt64(columnIndex, columnValue);
}

ECode BulkCursorToCursorAdaptor::GetFloat(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Float* columnValue)
{
    VALIDATE_NOT_NULL(columnValue)
    return AbstractWindowedCursor::GetFloat(columnIndex, columnValue);
}

ECode BulkCursorToCursorAdaptor::GetDouble(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Double* columnValue)
{
    VALIDATE_NOT_NULL(columnValue)
    return AbstractWindowedCursor::GetDouble(columnIndex, columnValue);
}

ECode BulkCursorToCursorAdaptor::GetType(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    return AbstractWindowedCursor::GetType(columnIndex, index);
}

ECode BulkCursorToCursorAdaptor::IsNull(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* isNull)
{
    VALIDATE_NOT_NULL(isNull)
    return AbstractWindowedCursor::IsNull(columnIndex, isNull);
}

ECode BulkCursorToCursorAdaptor::IsClosed(
    /* [out] */ Boolean* closed)
{
    VALIDATE_NOT_NULL(closed)
    return AbstractWindowedCursor::IsClosed(closed);
}

ECode BulkCursorToCursorAdaptor::RegisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    return AbstractWindowedCursor::RegisterContentObserver(observer);
}

ECode BulkCursorToCursorAdaptor::UnregisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    return AbstractWindowedCursor::UnregisterContentObserver(observer);
}

ECode BulkCursorToCursorAdaptor::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return AbstractWindowedCursor::RegisterDataSetObserver(observer);
}

ECode BulkCursorToCursorAdaptor::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return AbstractWindowedCursor::UnregisterDataSetObserver(observer);
}

ECode BulkCursorToCursorAdaptor::SetNotificationUri(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* uri)
{
    return AbstractWindowedCursor::SetNotificationUri(cr, uri);
}

ECode BulkCursorToCursorAdaptor::GetWantsAllOnMoveCalls(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return AbstractWindowedCursor::GetWantsAllOnMoveCalls(result);
}

} //Database
} //Droid
} //Elastos
