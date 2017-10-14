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

#include "elastos/droid/internal/database/SortCursor.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Database {

SortCursor::MyDataSetObserver::MyDataSetObserver(
    /* [in] */ SortCursor* owner)
    : mOwner(owner)
{
}

ECode SortCursor::MyDataSetObserver::OnChanged()
{
    // Reset our position so the optimizations in move-related code
    // don't screw us over
    mOwner->mPos = -1;
    return NOERROR;
}

ECode SortCursor::MyDataSetObserver::OnInvalidated()
{
    mOwner->mPos = -1;
    return NOERROR;
}

CAR_INTERFACE_IMPL(SortCursor, AbstractCursor, ISortCursor)

const String SortCursor::TAG("SortCursor");

SortCursor::SortCursor()
    : ROWCACHESIZE(64)
    , mLastCacheHit(-1)
{
    mRowNumCache = ArrayOf<Int32>::Alloc(ROWCACHESIZE);
    mCursorCache = ArrayOf<Int32>::Alloc(ROWCACHESIZE);
    mObserver = new MyDataSetObserver(this);
}

ECode SortCursor::constructor(
    /* [in] */ ArrayOf<ICursor*>* cursors,
    /* [in] */ const String& sortcolumn)
{
    VALIDATE_NOT_NULL(cursors)
    mCursors = cursors;

    Int32 length = mCursors->GetLength();
    mSortColumns = ArrayOf<Int32>::Alloc(length);
    for (Int32 i = 0 ; i < length ; i++) {
        if ((*mCursors)[i] == NULL)
            continue;

        // Register ourself as a data set observer
        (*mCursors)[i]->RegisterDataSetObserver(mObserver);

        Boolean res;
        (*mCursors)[i]->MoveToFirst(&res);

        // We don't catch the exception
        Int32 index;
        (*mCursors)[i]->GetColumnIndexOrThrow(sortcolumn, &index);
        (*mSortColumns)[i] = index;
    }
    mCursor = NULL;
    String smallest("");
    for (Int32 j = 0 ; j < length; j++) {
        Boolean isAfterLast;
        if ((*mCursors)[j] == NULL ||
            ((*mCursors)[j]->IsAfterLast(&isAfterLast), isAfterLast))
            continue;
        String current;
        (*mCursors)[j]->GetString((*mSortColumns)[j], &current);
        if (mCursor == NULL || current.CompareIgnoreCase(smallest) < 0) {
            smallest = current;
            mCursor = (*mCursors)[j];
        }
    }

    for (Int32 i = mRowNumCache->GetLength() - 1; i >= 0; i--) {
        (*mRowNumCache)[i] = -2;
    }

    mCurRowNumCache = ArrayOf<ArrayOf<Int32>* >::Alloc(ROWCACHESIZE);
    for (Int32 i = 0; i < ROWCACHESIZE; i++) {
        AutoPtr<ArrayOf<Int32> > item = ArrayOf<Int32>::Alloc(length);
        mCurRowNumCache->Set(i, item);
    }
    return NOERROR;
}

ECode SortCursor::GetCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 count = 0;
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0 ; i < length ; i++) {
        if ((*mCursors)[i] != NULL) {
            Int32 c;
            (*mCursors)[i]->GetCount(&c);
            count += c;
        }
    }
    *result = count;
    return NOERROR;
}

ECode SortCursor::OnMove(
    /* [in] */ Int32 oldPosition,
    /* [in] */ Int32 newPosition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (oldPosition == newPosition) {
        *result = TRUE;
        return NOERROR;
    }

    /* Find the right cursor
     * Because the client of this cursor (the listadapter/view) tends
     * to jump around in the cursor somewhat, a simple cache strategy
     * is used to avoid having to search all cursors from the start.
     * TODO: investigate strategies for optimizing random access and
     * reverse-order access.
     */
    Int32 cache_entry = newPosition % ROWCACHESIZE;

    if ((*mRowNumCache)[cache_entry] == newPosition) {
        Int32 which = (*mCursorCache)[cache_entry];
        mCursor = (*mCursors)[which];
        if (mCursor == NULL) {
            Logger::W(TAG, "onMove: cache results in a NULL cursor.");
            *result = FALSE;
            return NOERROR;
        }
        Boolean res;
        mCursor->MoveToPosition((*(*mCurRowNumCache)[cache_entry])[which], &res);
        mLastCacheHit = cache_entry;
        *result = TRUE;
        return NOERROR;
    }

    mCursor = NULL;
    Int32 length = mCursors->GetLength();

    if (mLastCacheHit >= 0) {
        for (Int32 i = 0; i < length; i++) {
            if ((*mCursors)[i] == NULL)
                continue;
            Boolean res;
            (*mCursors)[i]->MoveToPosition((*(*mCurRowNumCache)[mLastCacheHit])[i], &res);
        }
    }

    if (newPosition < oldPosition || oldPosition == -1) {
        for (Int32 i = 0 ; i < length; i++) {
            if ((*mCursors)[i] == NULL)
                continue;
            Boolean res;
            (*mCursors)[i]->MoveToFirst(&res);
        }
        oldPosition = 0;
    }
    if (oldPosition < 0) {
        oldPosition = 0;
    }

    // search forward to the new position
    Int32 smallestIdx = -1;
    for(Int32 i = oldPosition; i <= newPosition; i++) {
        String smallest("");
        smallestIdx = -1;
        for (Int32 j = 0 ; j < length; j++) {
            Boolean isAfterLast;
            if ((*mCursors)[j] == NULL ||
                ((*mCursors)[j]->IsAfterLast(&isAfterLast), isAfterLast)) {
                continue;
            }
            String current;
            (*mCursors)[j]->GetString((*mSortColumns)[j], &current);
            if (smallestIdx < 0 || current.CompareIgnoreCase(smallest) < 0) {
                smallest = current;
                smallestIdx = j;
            }
        }
        if (i == newPosition)
            break;
        if ((*mCursors)[smallestIdx] != NULL) {
            Boolean res;
            (*mCursors)[smallestIdx]->MoveToNext(&res);
        }
    }
    mCursor = (*mCursors)[smallestIdx];
    (*mRowNumCache)[cache_entry] = newPosition;
    (*mCursorCache)[cache_entry] = smallestIdx;
    for (Int32 i = 0; i < length; i++) {
        if ((*mCursors)[i] != NULL) {
            Int32 pos;
            (*mCursors)[i]->GetPosition(&pos);
            (*(*mCurRowNumCache)[cache_entry])[i] = pos;
        }
    }
    mLastCacheHit = -1;
    *result = TRUE;
    return NOERROR;
}

ECode SortCursor::GetString(
    /* [in] */ Int32 column,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mCursor->GetString(column, result);
}

ECode SortCursor::GetInt16(
    /* [in] */ Int32 column,
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result)
    return mCursor->GetInt16(column, result);
}

ECode SortCursor::GetInt32(
    /* [in] */ Int32 column,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mCursor->GetInt32(column, result);
}

ECode SortCursor::GetInt64(
    /* [in] */ Int32 column,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mCursor->GetInt64(column, result);
}

ECode SortCursor::GetFloat(
    /* [in] */ Int32 column,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    return mCursor->GetFloat(column, result);
}

ECode SortCursor::GetDouble(
    /* [in] */ Int32 column,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    return mCursor->GetDouble(column, result);
}

ECode SortCursor::GetType(
    /* [in] */ Int32 column,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mCursor->GetType(column, result);
}

ECode SortCursor::IsNull(
    /* [in] */ Int32 column,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mCursor->IsNull(column, result);
}

ECode SortCursor::GetBlob(
    /* [in] */ Int32 column,
    /* [out,callee]*/ ArrayOf<Byte>** blob)
{
    VALIDATE_NOT_NULL(blob)
    return mCursor->GetBlob(column, blob);
}

ECode SortCursor::GetColumnNames(
    /* [out,callee]*/ ArrayOf<String>** columnNames)
{
    VALIDATE_NOT_NULL(columnNames)
    *columnNames = NULL;
    if (mCursor != NULL) {
        return mCursor->GetColumnNames(columnNames);
    }
    else {
        // All of the cursors may be empty, but they can still return
        // this information.
        Int32 length = mCursors->GetLength();
        for (Int32 i = 0 ; i < length ; i++) {
            if ((*mCursors)[i] != NULL) {
                return (*mCursors)[i]->GetColumnNames(columnNames);
            }
        }
        Logger::E(TAG, "No cursor that can return names");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode SortCursor::Deactivate()
{
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0 ; i < length ; i++) {
        if ((*mCursors)[i] == NULL)
            continue;
        (*mCursors)[i]->Deactivate();
    }
    return NOERROR;
}

ECode SortCursor::Close()
{
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0 ; i < length ; i++) {
        if ((*mCursors)[i] == NULL)
            continue;
        ICloseable::Probe((*mCursors)[i])->Close();
    }
    return NOERROR;
}

ECode SortCursor::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0 ; i < length ; i++) {
        if ((*mCursors)[i] != NULL) {
            (*mCursors)[i]->RegisterDataSetObserver(observer);
        }
    }
    return NOERROR;
}

ECode SortCursor::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0 ; i < length ; i++) {
        if ((*mCursors)[i] != NULL) {
            (*mCursors)[i]->UnregisterDataSetObserver(observer);
        }
    }
    return NOERROR;
}

ECode SortCursor::Requery(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 length = mCursors->GetLength();
    for (Int32 i = 0 ; i < length ; i++) {
        if ((*mCursors)[i] == NULL)
            continue;

        Boolean res;
        (*mCursors)[i]->Requery(&res);
        if (res == FALSE) {
            *result = FALSE;
            return NOERROR;
        }
    }

    *result = TRUE;
    return NOERROR;
}

} // namespace Database
} // namespace Internal
} // namespace Droid
} // namespace Elastos
