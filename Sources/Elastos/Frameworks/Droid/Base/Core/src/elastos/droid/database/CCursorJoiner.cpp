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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/database/CCursorJoiner.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL_3(CCursorJoiner, Object, ICursorJoiner, IIterable, IIterator);

CAR_OBJECT_IMPL(CCursorJoiner)

CCursorJoiner::CCursorJoiner()
    : mCompareResultIsValid(FALSE)
{}

AutoPtr<ArrayOf<Int32> > CCursorJoiner::BuildColumnIndiciesArray(
    /* [in] */ ICursor* cursor,
    /* [in] */ const ArrayOf<String>& columnNames)
{
    AutoPtr< ArrayOf<Int32> > columns = ArrayOf<Int32>::Alloc(columnNames.GetLength());
    for (Int32 i = 0; i < columnNames.GetLength(); i++) {
        cursor->GetColumnIndexOrThrow(columnNames[i], &(*columns)[i]);
    }
    return columns;
}

ECode CCursorJoiner::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)

    *it = this;
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode CCursorJoiner::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean r1, r2;
    if(mCompareResultIsValid) {
        switch(mCompareResult) {
            case CursorJoinerResult_BOTH:
                mCursorLeft->IsLast(&r1);
                mCursorRight->IsLast(&r2);
                *result = !r1 || !r2;
                return NOERROR;

            case CursorJoinerResult_LEFT:
                mCursorLeft->IsLast(&r1);
                mCursorRight->IsAfterLast(&r2);
                *result = !r1 || !r2;
                return NOERROR;

            case CursorJoinerResult_RIGHT:
                mCursorLeft->IsAfterLast(&r1);
                mCursorRight->IsLast(&r2);
                *result = !r1 || !r2;
                return NOERROR;

            default:
                // throw new IllegalStateException("bad value for mCompareResult, "
                //             + mCompareResult);
                Slogger::E(String("CursorJoiner"), "bad value for mCompareResult, %d", mCompareResult);
                return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    else {
        mCursorLeft->IsAfterLast(&r1);
        mCursorRight->IsAfterLast(&r2);
        *result = !r1 || !r2;
        return NOERROR;
    }
}

ECode CCursorJoiner::GetNext(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    Boolean hasNext;
    if (HasNext(&hasNext), !hasNext) {
        // throw new IllegalStateException("you must only call next() when hasNext() is true");
        Slogger::E(String("CursorJoiner"), "you must only call next() when hasNext() is true");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    IncrementCursors();
    HasNext(&hasNext);
    assert(hasNext);

    Boolean isAfterLast;
    mCursorLeft->IsAfterLast(&isAfterLast);
    Boolean hasLeft = !isAfterLast;
    mCursorRight->IsAfterLast(&isAfterLast);
    Boolean hasRight = !isAfterLast;

    if (hasLeft && hasRight) {
        PopulateValues(*mValues, mCursorLeft, *mColumnsLeft, 0 /* start filling at index 0 */);
        PopulateValues(*mValues, mCursorRight, *mColumnsRight, 1 /* start filling at index 1 */);
        Int32 _result;
        CompareStrings(*mValues, &_result);
        switch (_result) {
            case -1:
                mCompareResult = CursorJoinerResult_LEFT;
                break;
            case 0:
                mCompareResult = CursorJoinerResult_BOTH;
                break;
            case 1:
                mCompareResult = CursorJoinerResult_RIGHT;
                break;
        }
    }
    else if (hasLeft) {
        mCompareResult = CursorJoinerResult_LEFT;
    }
    else  {
        assert(hasRight);
        mCompareResult = CursorJoinerResult_RIGHT;
    }
    mCompareResultIsValid = TRUE;
    AutoPtr<IInteger32> int32Obj;
    return CInteger32::New(mCompareResult, (IInteger32**)&int32Obj);
    *result = int32Obj;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCursorJoiner::Remove()
{
//    throw new UnsupportedOperationException("not implemented");
    Slogger::E(String("CursorJoiner"), "not implemented");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

void CCursorJoiner::PopulateValues(
    /* [in] */ ArrayOf<String>& values,
    /* [in] */ ICursor* cursor,
    /* [in] */ const ArrayOf<Int32>& columnIndicies,
    /* [in] */ Int32 startingIndex)
{
    assert(startingIndex == 0 || startingIndex == 1);
    for (Int32 i = 0; i < columnIndicies.GetLength(); i++) {
        cursor->GetString(columnIndicies[i], &values[startingIndex + i * 2]);
    }
}

void CCursorJoiner::IncrementCursors()
{
    if(mCompareResultIsValid) {
        Boolean succeeded;
        switch(mCompareResult) {
            case CursorJoinerResult_LEFT:
                mCursorLeft->MoveToNext(&succeeded);
                break;
            case CursorJoinerResult_RIGHT:
                mCursorRight->MoveToNext(&succeeded);
                break;
            case CursorJoinerResult_BOTH:
                mCursorLeft->MoveToNext(&succeeded);
                mCursorRight->MoveToNext(&succeeded);
                break;
        }
        mCompareResultIsValid = FALSE;
    }
}

ECode CCursorJoiner::CompareStrings(
    /* [in] */ const ArrayOf<String>& values,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if ((values.GetLength() % 2) != 0) {
        // throw new IllegalArgumentException("you must specify an even number of values");
        Slogger::E(String("CursorJoiner"), "you must specify an even number of values");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    for (Int32 index = 0; index < values.GetLength(); index += 2) {
        if (values[index].IsNull()) {
            if (values[index + 1].IsNull()) continue;
            *result = -1;
            return NOERROR;
        }

        if (values[index + 1].IsNull()) {
            *result = 1;
            return NOERROR;
        }

        Int32 comp = values[index].Compare(values[index + 1]);
        if (comp != 0) {
            *result = comp < 0 ? -1 : 1;
            return NOERROR;
        }
    }

    *result = 0;
    return NOERROR;
}

ECode CCursorJoiner::constructor(
    /* [in] */ ICursor* cursorLeft,
    /* [in] */ const ArrayOf<String>& columnNamesLeft,
    /* [in] */ ICursor* cursorRight,
    /* [in] */ const ArrayOf<String>& columnNamesRight)
{
    if(columnNamesLeft.GetLength() != columnNamesRight.GetLength()) {
        // throw new IllegalArgumentException(
        //             "you must have the same number of columns on the left and right, "
        //                     + columnNamesLeft.length + " != " + columnNamesRight.length);
        Slogger::E(String("CursorJoiner"), "you must have the same number of columns on the left and right, %d != %d"
                , columnNamesLeft.GetLength(), columnNamesRight.GetLength());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mCursorLeft = cursorLeft;
    mCursorRight = cursorRight;

    Boolean succeeded;
    mCursorLeft->MoveToFirst(&succeeded);
    mCursorRight->MoveToFirst(&succeeded);

    mCompareResultIsValid = FALSE;

    mColumnsLeft = BuildColumnIndiciesArray(cursorLeft, columnNamesLeft);
    mColumnsRight = BuildColumnIndiciesArray(cursorRight, columnNamesRight);

    mValues = ArrayOf<String>::Alloc(mColumnsLeft->GetLength() * 2);
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
