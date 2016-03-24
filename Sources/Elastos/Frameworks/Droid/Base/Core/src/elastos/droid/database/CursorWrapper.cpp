
#include "elastos/droid/database/CursorWrapper.h"

using Elastos::IO::ICloseable;
using Elastos::IO::EIID_ICloseable;

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL_3(CursorWrapper, Object, ICursorWrapper, ICursor, ICloseable)

ECode CursorWrapper::constructor(
    /* [in] */ ICursor* cursor)
{
    mCursor = cursor;
    return NOERROR;
}

ECode CursorWrapper::GetWrappedCursor(
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    *cursor = mCursor;
    REFCOUNT_ADD(*cursor)
    return NOERROR;
}

ECode CursorWrapper::Close()
{
    return ICloseable::Probe(mCursor)->Close();
}

ECode CursorWrapper::IsClosed(
    /* [out] */ Boolean* isClosed)
{
    VALIDATE_NOT_NULL(isClosed)
    return mCursor->IsClosed(isClosed);
}

ECode CursorWrapper::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    return mCursor->GetCount(count);
}

ECode CursorWrapper::Deactivate()
{
    return mCursor->Deactivate();
}

ECode CursorWrapper::MoveToFirst(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return mCursor->MoveToFirst(succeeded);
}

ECode CursorWrapper::GetColumnCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    return mCursor->GetColumnCount(count);
}

ECode CursorWrapper::GetColumnIndex(
    /* [in] */ const String& columnName,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    return mCursor->GetColumnIndex(columnName, index);
}

ECode CursorWrapper::GetColumnIndexOrThrow(
    /* [in] */ const String& columnName,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    return mCursor->GetColumnIndexOrThrow(columnName, index);
}

ECode CursorWrapper::GetColumnName(
    /* [in] */ Int32 columnIndex,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    return mCursor->GetColumnName(columnIndex, name);
}

ECode CursorWrapper::GetColumnNames(
    /* [out, callee] */ ArrayOf<String>** columnNames)
{
    VALIDATE_NOT_NULL(columnNames)
    return mCursor->GetColumnNames(columnNames);
}

ECode CursorWrapper::GetDouble(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value)
    return mCursor->GetDouble(columnIndex, value);
}

ECode CursorWrapper::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    return mCursor->GetExtras(extras);
}

ECode CursorWrapper::GetFloat(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    return mCursor->GetFloat(columnIndex, value);
}

ECode CursorWrapper::GetInt32(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    return mCursor->GetInt32(columnIndex, value);
}

ECode CursorWrapper::GetInt64(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    return mCursor->GetInt64(columnIndex, value);
}

ECode CursorWrapper::GetInt16(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value)
    return mCursor->GetInt16(columnIndex, value);
}

ECode CursorWrapper::GetString(
    /* [in] */ Int32 columnIndex,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    return mCursor->GetString(columnIndex, value);
}

ECode CursorWrapper::CopyStringToBuffer(
    /* [in] */ Int32 columnIndex,
    /* [in] */ ICharArrayBuffer* buffer)
{
    return mCursor->CopyStringToBuffer(columnIndex, buffer);
}

ECode CursorWrapper::GetBlob(
    /* [in] */  Int32 columnIndex,
    /* [out, callee] */ ArrayOf<Byte>** blob)
{
    VALIDATE_NOT_NULL(blob)
    return mCursor->GetBlob(columnIndex, blob);
}

ECode CursorWrapper::GetWantsAllOnMoveCalls(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    return mCursor->GetWantsAllOnMoveCalls(value);
}

ECode CursorWrapper::IsAfterLast(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mCursor->IsAfterLast(result);
}

ECode CursorWrapper::IsBeforeFirst(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mCursor->IsBeforeFirst(result);
}

ECode CursorWrapper::IsFirst(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mCursor->IsFirst(result);
}

ECode CursorWrapper::IsLast(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mCursor->IsLast(result);
}

ECode CursorWrapper::GetType(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    return mCursor->GetType(columnIndex, type);
}

ECode CursorWrapper::IsNull(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mCursor->IsNull(columnIndex, result);
}

ECode CursorWrapper::MoveToLast(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return mCursor->MoveToLast(succeeded);
}

ECode CursorWrapper::Move(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return mCursor->Move(offset, succeeded);
}

ECode CursorWrapper::MoveToPosition(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return mCursor->MoveToPosition(position, succeeded);
}

ECode CursorWrapper::MoveToNext(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return mCursor->MoveToNext(succeeded);
}

ECode CursorWrapper::GetPosition(
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position)
    return mCursor->GetPosition(position);
}

ECode CursorWrapper::MoveToPrevious(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return mCursor->MoveToPrevious(succeeded);
}

ECode CursorWrapper::RegisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    return mCursor->RegisterContentObserver(observer);
}

ECode CursorWrapper::RegisterDataSetObserver(
    /* [in] */IDataSetObserver* observer)
{
    return mCursor->RegisterDataSetObserver(observer);
}

ECode CursorWrapper::Requery(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    return mCursor->Requery(succeeded);
}

ECode CursorWrapper::Respond(
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    return mCursor->Respond(extras, bundle);
}

ECode CursorWrapper::SetNotificationUri(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* uri)
{
    return mCursor->SetNotificationUri(cr, uri);
}

ECode CursorWrapper::GetNotificationUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    return mCursor->GetNotificationUri(uri);
}

ECode CursorWrapper::UnregisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    return mCursor->UnregisterContentObserver(observer);
}

ECode CursorWrapper::UnregisterDataSetObserver(
    /* [in] */IDataSetObserver* observer)
{
    return mCursor->UnregisterDataSetObserver(observer);
}

} //Database
} //Droid
} //Elastos