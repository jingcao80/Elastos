//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
    return mCursor->IsClosed(isClosed);
}

ECode CursorWrapper::GetCount(
    /* [out] */ Int32* count)
{
    return mCursor->GetCount(count);
}

ECode CursorWrapper::Deactivate()
{
    return mCursor->Deactivate();
}

ECode CursorWrapper::MoveToFirst(
    /* [out] */ Boolean* succeeded)
{
    return mCursor->MoveToFirst(succeeded);
}

ECode CursorWrapper::GetColumnCount(
    /* [out] */ Int32* count)
{
    return mCursor->GetColumnCount(count);
}

ECode CursorWrapper::GetColumnIndex(
    /* [in] */ const String& columnName,
    /* [out] */ Int32* index)
{
    return mCursor->GetColumnIndex(columnName, index);
}

ECode CursorWrapper::GetColumnIndexOrThrow(
    /* [in] */ const String& columnName,
    /* [out] */ Int32* index)
{
    return mCursor->GetColumnIndexOrThrow(columnName, index);
}

ECode CursorWrapper::GetColumnName(
    /* [in] */ Int32 columnIndex,
    /* [out] */ String* name)
{
    return mCursor->GetColumnName(columnIndex, name);
}

ECode CursorWrapper::GetColumnNames(
    /* [out, callee] */ ArrayOf<String>** columnNames)
{
    return mCursor->GetColumnNames(columnNames);
}

ECode CursorWrapper::GetDouble(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Double* value)
{
    return mCursor->GetDouble(columnIndex, value);
}

ECode CursorWrapper::GetExtras(
    /* [out] */ IBundle** extras)
{
    return mCursor->GetExtras(extras);
}

ECode CursorWrapper::GetFloat(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Float* value)
{
    return mCursor->GetFloat(columnIndex, value);
}

ECode CursorWrapper::GetInt32(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int32* value)
{
    return mCursor->GetInt32(columnIndex, value);
}

ECode CursorWrapper::GetInt64(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int64* value)
{
    return mCursor->GetInt64(columnIndex, value);
}

ECode CursorWrapper::GetInt16(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int16* value)
{
    return mCursor->GetInt16(columnIndex, value);
}

ECode CursorWrapper::GetString(
    /* [in] */ Int32 columnIndex,
    /* [out] */ String* value)
{
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
    return mCursor->GetBlob(columnIndex, blob);
}

ECode CursorWrapper::GetWantsAllOnMoveCalls(
    /* [out] */ Boolean* value)
{
    return mCursor->GetWantsAllOnMoveCalls(value);
}

ECode CursorWrapper::IsAfterLast(
    /* [out] */ Boolean* result)
{
    return mCursor->IsAfterLast(result);
}

ECode CursorWrapper::IsBeforeFirst(
    /* [out] */ Boolean* result)
{
    return mCursor->IsBeforeFirst(result);
}

ECode CursorWrapper::IsFirst(
    /* [out] */ Boolean* result)
{
    return mCursor->IsFirst(result);
}

ECode CursorWrapper::IsLast(
    /* [out] */ Boolean* result)
{
    return mCursor->IsLast(result);
}

ECode CursorWrapper::GetType(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int32* type)
{
    return mCursor->GetType(columnIndex, type);
}

ECode CursorWrapper::IsNull(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* result)
{
    return mCursor->IsNull(columnIndex, result);
}

ECode CursorWrapper::MoveToLast(
    /* [out] */ Boolean* succeeded)
{
    return mCursor->MoveToLast(succeeded);
}

ECode CursorWrapper::Move(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* succeeded)
{
    return mCursor->Move(offset, succeeded);
}

ECode CursorWrapper::MoveToPosition(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* succeeded)
{
    return mCursor->MoveToPosition(position, succeeded);
}

ECode CursorWrapper::MoveToNext(
    /* [out] */ Boolean* succeeded)
{
    return mCursor->MoveToNext(succeeded);
}

ECode CursorWrapper::GetPosition(
    /* [out] */ Int32* position)
{
    return mCursor->GetPosition(position);
}

ECode CursorWrapper::MoveToPrevious(
    /* [out] */ Boolean* succeeded)
{
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
    return mCursor->Requery(succeeded);
}

ECode CursorWrapper::Respond(
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** bundle)
{
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