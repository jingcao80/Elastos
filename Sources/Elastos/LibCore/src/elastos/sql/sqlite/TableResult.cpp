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

#include "TableResult.h"
#include "StringBuffer.h"

using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Sql {
namespace SQLite {

CAR_INTERFACE_IMPL_2(TableResult, Object, ITableResult, ICallback)

ECode TableResult::Columns(
    /* [in] */ ArrayOf<String> * coldata)
{
    VALIDATE_NOT_NULL(coldata)

    mColumn = coldata;
    mNcolumns = mColumn->GetLength();
    return NOERROR;
}

ECode TableResult::Types(
    /* [in] */ ArrayOf<String> * intypes)
{
    VALIDATE_NOT_NULL(intypes)
    mTypes = intypes;
    return NOERROR;
}

ECode TableResult::Newrow(
    /* [in] */ ArrayOf<String> * rowdata,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;
    VALIDATE_NOT_NULL(rowdata)

    if (rowdata && rowdata->GetLength()>0) {
        if (mMaxRows > 0 && mNrows >= mMaxRows) {
            mAtMaxRows = TRUE;
            *value = TRUE;
            return NOERROR;
        }
        mRows.PushBack(rowdata);
        mNrows++;
    }

    *value = FALSE;
    return NOERROR;
}

ECode TableResult::ToString(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    StringBuffer sb;
    Int32 i = 0;
    for (i = 0; i < mNcolumns; i++) {
        sb.Append((*mColumn)[i].IsNull() ? String("NULL") : (*mColumn)[i]);
        sb.AppendChar('|');
    }
    sb.AppendChar('\n');
    for (i = 0; i < mNrows; i++) {
        Int32 k = 0;
        AutoPtr<ArrayOf<String> > row = mRows[i];
        for (k = 0; k < mNcolumns; k++) {
            sb.Append((*row)[k].IsNull() ? String("NULL") : (*row)[k]);
            sb.AppendChar('|');
        }
        sb.AppendChar('\n');
    }
    *value = sb.ToString();
    return NOERROR;
}

TableResult::TableResult()
    : mNcolumns(0)
    , mNrows(0)
    , mMaxRows(0)
    , mAtMaxRows(FALSE)
{
}

TableResult::TableResult(
    /* [in] */ Int32 maxrows)
    : mNcolumns(0)
    , mNrows(0)
    , mMaxRows(maxrows)
    , mAtMaxRows(FALSE)
{
}

ECode TableResult::Clear()
{
    mColumn = ArrayOf<String>::Alloc(1);
    mTypes = NULL;
    // rows = new Vector<AutoPtr<ArrayOf<String> > >();
    mRows.Clear();
    mNcolumns = mNrows = 0;
    mAtMaxRows = FALSE;
    return NOERROR;
}

ECode TableResult::constructor()
{
    return Clear();
}

ECode TableResult::constructor(Int32 maxrows)
{
    mMaxRows = maxrows;
    return Clear();
}

} // namespace SQLite
} // namespace Sql
} // namespace Elastos
