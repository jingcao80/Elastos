
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/database/AbstractCursor.h"
#include "elastos/droid/database/CDataSetObservable.h"
#include "elastos/droid/database/CContentObservable.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Core::AutoLock;
using Elastos::IO::EIID_ICloseable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {

const String AbstractCursor::TAG("AbstractCursor");

//=========================================================================================
// AbstractCursor::SelfContentObserver
//=========================================================================================

AbstractCursor::SelfContentObserver::SelfContentObserver()
{}

ECode AbstractCursor::SelfContentObserver::constructor(
    /* [in] */ IAbstractCursor* cursor)
{
    FAIL_RETURN(ContentObserver::constructor(NULL))

    IWeakReferenceSource::Probe(cursor)->GetWeakReference((IWeakReference**)&mCursor);
    return NOERROR;
}

ECode AbstractCursor::SelfContentObserver::DeliverSelfNotifications(
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = FALSE;
    return NOERROR;
}

ECode AbstractCursor::SelfContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoPtr<IAbstractCursor> cursor;
    mCursor->Resolve(EIID_IAbstractCursor, (IInterface**)&cursor);
    if (cursor != NULL) {
        return cursor->OnChange(FALSE);
    }
    return NOERROR;
}

//=========================================================================================
// AbstractCursor
//=========================================================================================

CAR_INTERFACE_IMPL_4(AbstractCursor, Object, IAbstractCursor, ICrossProcessCursor, ICursor, ICloseable);

AbstractCursor::AbstractCursor()
    : mPos(-1)
    , mRowIdColumnIndex(-1)
    , mCurrentRowID(0)
    , mClosed(FALSE)
    , mSelfObserverRegistered(FALSE)
    , mExtras(CBundle::EMPTY)
{
    CDataSetObservable::New((IDataSetObservable**)&mDataSetObservable);
    CContentObservable::New((IContentObservable**)&mContentObservable);
}

AbstractCursor::~AbstractCursor()
{
    mUpdatedRows.Clear();

    if (mSelfObserver != NULL && mSelfObserverRegistered) {
        mContentResolver->UnregisterContentObserver(mSelfObserver);
    }
    // try {
    if (!mClosed) Close();
    // } catch(Exception e) { }
}

ECode AbstractCursor::GetType(
    /* [in] */ Int32 column,
    /* [out] */ Int32* type)
{
    // Reflects the assumption that all commonly used field types (meaning everything
    // but blobs) are convertible to strings so it should be safe to call
    // getString to retrieve them.
    VALIDATE_NOT_NULL(type)
    *type = ICursor::FIELD_TYPE_STRING;
    return NOERROR;
}

// TODO implement getBlob in all cursor types
ECode AbstractCursor::GetBlob(
    /* [in] */ Int32 column,
    /* [out] */ ArrayOf<Byte>** blob)
{
//  throw new UnsupportedOperationException("getBlob is not supported");
    Slogger::E(TAG, "getBlob is not supported");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AbstractCursor::GetWindow(
    /* [out] */ ICursorWindow** window)
{
    VALIDATE_NOT_NULL(window)
    *window = NULL;
    return NOERROR;
}

ECode AbstractCursor::GetColumnCount(
    /* [out] */ Int32* columnCount)
{
    VALIDATE_NOT_NULL(columnCount)
    *columnCount = -1;
    AutoPtr< ArrayOf<String> > names;
    FAIL_RETURN(GetColumnNames((ArrayOf<String>**)&names));
    *columnCount = names->GetLength();
    return NOERROR;
}

ECode AbstractCursor::Deactivate()
{
    return OnDeactivateOrClose();
}

ECode AbstractCursor::OnDeactivateOrClose()
{
    if (mSelfObserver != NULL && mSelfObserverRegistered) {
        mContentResolver->UnregisterContentObserver(mSelfObserver);
        mSelfObserverRegistered = FALSE;
    }
    mDataSetObservable->NotifyInvalidated();
    return NOERROR;
}

ECode AbstractCursor::Requery(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mSelfObserver != NULL && !mSelfObserverRegistered) {
        mContentResolver->RegisterContentObserver(mNotifyUri, TRUE, mSelfObserver);
        mSelfObserverRegistered = TRUE;
    }
    mDataSetObservable->NotifyChanged();
    *result = TRUE;
    return NOERROR;
}

ECode AbstractCursor::IsClosed(
    /* [out] */ Boolean* closed)
{
    VALIDATE_NOT_NULL(closed)
    *closed = mClosed;
    return NOERROR;
}

ECode AbstractCursor::Close()
{
    mClosed = TRUE;
    IObservable::Probe(mContentObservable)->UnregisterAll();
    return OnDeactivateOrClose();
}

ECode AbstractCursor::OnMove(
    /* [in] */ Int32 oldPosition,
    /* [in] */ Int32 newPosition,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = TRUE;
    return NOERROR;
}

ECode AbstractCursor::CopyStringToBuffer(
    /* [in] */ Int32 columnIndex,
    /* [in] */ ICharArrayBuffer* buffer)
{
    // Default implementation, uses getString
    String result;
    GetString(columnIndex, &result);
    if (!result.IsNullOrEmpty()) {
        AutoPtr< ArrayOf<Char32> > data;
        buffer->GetData((ArrayOf<Char32>**)&data);
        if (data == NULL || data->GetLength() < result.GetLength()) {
            AutoPtr< ArrayOf<Char32> > resultArray = result.GetChars();
            buffer->SetData(resultArray);
        }
        else {
            AutoPtr< ArrayOf<Char32> > resultArray = result.GetChars();
            data->Copy(resultArray, 0, resultArray->GetLength());
        }
        buffer->SetSizeCopied(result.GetLength());
    }
    else {
        buffer->SetSizeCopied(0);
    }
    return NOERROR;
}

ECode AbstractCursor::GetPosition(
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position)
    *position = mPos;
    return NOERROR;
}

ECode AbstractCursor::MoveToPosition(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    // Make sure position isn't past the end of the cursor
    Int32 count;
    GetCount(&count);
    if(position >= count) {
        mPos = count;
        *succeeded = FALSE;
        return NOERROR;
    }

    // Make sure position isn't before the beginning of the cursor
    if(position < 0) {
        mPos = -1;
        *succeeded = FALSE;
        return NOERROR;
    }

    // Check for no-op moves, and skip the rest of the work for them
    if(position == mPos) {
        *succeeded = TRUE;
        return NOERROR;
    }

    Boolean result;
    OnMove(mPos, position, &result);
    if(result == FALSE) {
        mPos = -1;
    }
    else {
        mPos = position;
        if(mRowIdColumnIndex != -1) {
            GetInt64(mRowIdColumnIndex, &mCurrentRowID);
        }
    }
    *succeeded = result;
    return NOERROR;
}

ECode AbstractCursor::FillWindow(
    /* [in] */ Int32 position,
    /* [in] */ ICursorWindow* window)
{
    DatabaseUtils::CursorFillWindow(this, position, window);
    return NOERROR;
}

ECode AbstractCursor::Move(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return MoveToPosition(mPos + offset, succeeded);
}

ECode AbstractCursor::MoveToFirst(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return MoveToPosition(0, succeeded);
}

ECode AbstractCursor::MoveToLast(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    Int32 count;
    GetCount(&count);
    return MoveToPosition(count - 1, succeeded);
}

ECode AbstractCursor::MoveToNext(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    MoveToPosition(mPos + 1, succeeded);
    return NOERROR;
}

ECode AbstractCursor::MoveToPrevious(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return MoveToPosition(mPos - 1, succeeded);
}

ECode AbstractCursor::IsFirst(
    /* [out] */ Boolean* isFirst)
{
    VALIDATE_NOT_NULL(isFirst)
    Int32 count;
    *isFirst = mPos == 0 && (GetCount(&count), count != 0);
    return NOERROR;
}

ECode AbstractCursor::IsLast(
    /* [out] */ Boolean* isLast)
{
    VALIDATE_NOT_NULL(isLast)
    Int32 count;
    GetCount(&count);
    *isLast =  mPos == (count - 1) && count != 0;
    return NOERROR;
}

ECode AbstractCursor::IsBeforeFirst(
    /* [out] */ Boolean* isBeforeFirst)
{
    VALIDATE_NOT_NULL(isBeforeFirst)
    Int32 count;
    if (GetCount(&count), count == 0) {
        *isBeforeFirst = TRUE;
        return NOERROR;
    }
    *isBeforeFirst = mPos == -1;
    return NOERROR;
}

ECode AbstractCursor::IsAfterLast(
    /* [out] */ Boolean* isAfterLast)
{
    VALIDATE_NOT_NULL(isAfterLast)
    Int32 count;
    GetCount(&count);
    if (count == 0) {
        *isAfterLast = TRUE;
        return NOERROR;
    }
    *isAfterLast = mPos == count;
    return NOERROR;
}

ECode AbstractCursor::GetColumnIndex(
    /* [in] */ const String& _columnName,
    /* [out] */ Int32* columnIndex)
{
    VALIDATE_NOT_NULL(columnIndex)
    *columnIndex = -1;
    String columnName = _columnName;
    Int32 periodIndex = columnName.LastIndexOf('.');
    if (periodIndex != -1) {
        //Exception e = new Exception();
        Slogger::E(TAG, "requesting column name with table name -- %s", columnName.string());
        columnName = columnName.Substring(periodIndex + 1);
    }

    AutoPtr< ArrayOf<String> > columnNames;
    FAIL_RETURN(GetColumnNames((ArrayOf<String>**)&columnNames));
    Int32 length = columnNames->GetLength();
    for(Int32 i = 0; i < length; i++) {
        if((*columnNames)[i].EqualsIgnoreCase(columnName)) {
            *columnIndex = i;
            return NOERROR;
        }
    }

    if (FALSE) {
        Int32 count;
        if (GetCount(&count), count > 0) {
            Slogger::W(TAG, "Unknown column %s", columnName.string());
        }
    }
    *columnIndex = -1;
    return NOERROR;
}

ECode AbstractCursor::GetColumnIndexOrThrow(
    /* [in] */ const String& columnName,
    /* [out] */ Int32* i)
{
    VALIDATE_NOT_NULL(i)
    *i = -1;

    Int32 index;
    GetColumnIndex(columnName, &index);
    if (index < 0) {
        //throw new IllegalArgumentException("column '" + columnName + "' does not exist");
        Slogger::E(TAG, "column '%s' does not exist", columnName.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *i = index;
    return NOERROR;
}

ECode AbstractCursor::GetColumnName(
    /* [in] */ Int32 columnIndex,
    /* [out] */ String* columnName)
{
    VALIDATE_NOT_NULL(columnName)
    *columnName = NULL;
    AutoPtr< ArrayOf<String> > columnNames;
    FAIL_RETURN(GetColumnNames((ArrayOf<String>**)&columnNames));
    *columnName = (*columnNames)[columnIndex];
    return NOERROR;
}

ECode AbstractCursor::RegisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    return IObservable::Probe(mContentObservable)->RegisterObserver(observer);
}

ECode AbstractCursor::UnregisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    // cursor will unregister all observers when it close
    if(!mClosed) {
        return IObservable::Probe(mContentObservable)->UnregisterObserver(observer);
    }
    return NOERROR;
}

ECode AbstractCursor::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return IObservable::Probe(mDataSetObservable)->RegisterObserver(observer);
}

ECode AbstractCursor::UnregisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer)
{
    return IObservable::Probe(mDataSetObservable)->UnregisterObserver(observer);
}

ECode AbstractCursor::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoLock syncLock(mSelfObserverLock);
    mContentObservable->DispatchChange(selfChange);
    if (mNotifyUri != NULL && selfChange) {
        mContentResolver->NotifyChange(mNotifyUri, mSelfObserver);
    }
    return NOERROR;
}

ECode AbstractCursor::SetNotificationUri(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* notifyUri)
{
    return SetNotificationUri(cr, notifyUri, UserHandle::GetMyUserId());
}

ECode AbstractCursor::SetNotificationUri(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* notifyUri,
    /* [in] */ Int32 userHandle)
{
    {
        AutoLock syncLock(mSelfObserverLock);

        mNotifyUri = notifyUri;
        mContentResolver = cr;
        if (mSelfObserver != NULL) {
            mContentResolver->UnregisterContentObserver(mSelfObserver);
        }

        mSelfObserver = new SelfContentObserver();
        mSelfObserver->constructor(this);
        mContentResolver->RegisterContentObserver(mNotifyUri, TRUE, mSelfObserver);
        mSelfObserverRegistered = TRUE;
    }
    return NOERROR;
}

ECode AbstractCursor::GetNotificationUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)

    {    AutoLock syncLock(mSelfObserverLock);
        *uri = mNotifyUri;
        REFCOUNT_ADD(*uri)
    }
    return NOERROR;
}

ECode AbstractCursor::GetWantsAllOnMoveCalls(
    /* [out] */ Boolean* calls)
{
    VALIDATE_NOT_NULL(calls);
    *calls = FALSE;
    return NOERROR;
}

ECode AbstractCursor::SetExtras(
    /* [in] */ IBundle* extras)
{
    mExtras = (extras == NULL) ? CBundle::EMPTY.Get() : extras;
    return NOERROR;
}

ECode AbstractCursor::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    *extras = mExtras;
    REFCOUNT_ADD(*extras)
    return NOERROR;
}

ECode AbstractCursor::Respond(
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** v)
{
    VALIDATE_NOT_NULL(v)
    *v = CBundle::EMPTY;
    REFCOUNT_ADD(*v)
    return NOERROR;
}

Boolean AbstractCursor::IsFieldUpdated(
    /* [in] */ Int32 columnIndex)
{
    return FALSE;
}

AutoPtr<IInterface> AbstractCursor::GetUpdatedField(
    /* [in] */ Int32 columnIndex)
{
    return NULL;
}

ECode AbstractCursor::CheckPosition()
{
    Int32 count;
    if (-1 == mPos || (GetCount(&count), count == mPos)) {
        return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
