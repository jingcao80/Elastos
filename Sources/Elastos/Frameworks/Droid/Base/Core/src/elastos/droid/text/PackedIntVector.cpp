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

#include "elastos/droid/text/PackedIntVector.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/internal/utility/GrowingArrayUtils.h"

using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::GrowingArrayUtils;

namespace Elastos {
namespace Droid {
namespace Text {

PackedIntVector::PackedIntVector(
    /* [in] */ Int32 columns)
    : mColumns(columns)
    , mRows(0)
    , mRowGapStart(0)
    , mRowGapLength(mRows)
{
    mValueGap = ArrayOf<Int32>::Alloc(2 * columns);
}

PackedIntVector::~PackedIntVector()
{}

Int32 PackedIntVector::GetValue(
    /* [in] */ Int32 row,
    /* [in] */ Int32 column)
{
    Int32 columns = mColumns;

    if (((row | column) < 0) || (row >= Size()) || (column >= columns)) {
        //throw new IndexOutOfBoundsException(row + ", " + column);
        assert(0);
        return -1;
    }

    if (row >= mRowGapStart) {
        row += mRowGapLength;
    }

    Int32 value = (*mValues)[row * columns + column];

    if (row >= (*mValueGap)[column]) {
        value += (*mValueGap)[column + columns];
    }

    return value;
}

void PackedIntVector::SetValue(
    /* [in] */ Int32 row,
    /* [in] */ Int32 column,
    /* [in] */ Int32 value)
{
    if (((row | column) < 0) || (row >= Size()) || (column >= mColumns)) {
        //throw new IndexOutOfBoundsException(row + ", " + column);
        assert(0);
    }

    if (row >= mRowGapStart) {
        row += mRowGapLength;
    }

    if (row >= (*mValueGap)[column]) {
        value -= (*mValueGap)[column + mColumns];
    }

    (*mValues)[row * mColumns + column] = value;
}

void PackedIntVector::SetValueInternal(
    /* [in] */ Int32 row,
    /* [in] */ Int32 column,
    /* [in] */ Int32 value)
{
    if (row >= mRowGapStart) {
        row += mRowGapLength;
    }

    if (row >= (*mValueGap)[column]) {
        value -= (*mValueGap)[column + mColumns];
    }

    (*mValues)[row * mColumns + column] = value;
}

void PackedIntVector::AdjustValuesBelow(
    /* [in] */ Int32 startRow,
    /* [in] */ Int32 column,
    /* [in] */ Int32 delta)
{
    if (((startRow | column) < 0) || (startRow > Size()) ||
        (column >= Width())) {
        //throw new IndexOutOfBoundsException(startRow + ", " + column);
        assert(0);
    }

    if (startRow >= mRowGapStart) {
        startRow += mRowGapLength;
    }

    MoveValueGapTo(column, startRow);
    (*mValueGap)[column + mColumns] += delta;
}

void PackedIntVector::InsertAt(
    /* [in] */ Int32 row,
    /* [in] */ ArrayOf<Int32>* values)
{
    if ((row < 0) || (row > Size())) {
        //throw new IndexOutOfBoundsException("row " + row);
        assert(0);
    }

    if ((values != NULL) && values->GetLength() < Width()) {
        //throw new IndexOutOfBoundsException("value count " + values.length);
        assert(0);
    }

    MoveRowGapTo(row);

    if (mRowGapLength == 0) {
        GrowBuffer();
    }

    mRowGapStart++;
    mRowGapLength--;

    if (values == NULL) {
        for (Int32 i = mColumns - 1; i >= 0; i--) {
            SetValueInternal(row, i, 0);
        }
    }
    else {
        for (Int32 i = mColumns - 1; i >= 0; i--) {
            SetValueInternal(row, i, (*values)[i]);
        }
    }
}

void PackedIntVector::DeleteAt(
    /* [in] */ Int32 row,
    /* [in] */ Int32 count)
{
    if (((row | count) < 0) || (row + count > Size())) {
        //throw new IndexOutOfBoundsException(row + ", " + count);
        assert(0);
    }

    MoveRowGapTo(row + count);

    mRowGapStart -= count;
    mRowGapLength += count;

    // TODO: Reclaim memory when the new height is much smaller
    // than the allocated size.
}

Int32 PackedIntVector::Size()
{
    return mRows - mRowGapLength;
}

Int32 PackedIntVector::Width()
{
    return mColumns;
}

void PackedIntVector::GrowBuffer()
{
    Int32 columns = mColumns;
    AutoPtr<ArrayOf<Int32> > newvalues;
    newvalues = ArrayUtils::NewUnpaddedInt32Array(GrowingArrayUtils::GrowSize(Size()) * columns);
    Int32 newsize = newvalues->GetLength() / columns;

    AutoPtr<ArrayOf<Int32> > valuegap = mValueGap;
    Int32 rowgapstart = mRowGapStart;

    Int32 after = mRows - (rowgapstart + mRowGapLength);

    if (mValues != NULL) {
        for (Int32 i = 0; i < columns * rowgapstart; i++) {
            (*newvalues)[i] = (*mValues)[i];
        }
        for (Int32 i = 0, srcBegin = (mRows - after) * columns,
             desBegin = (newsize - after) * columns; i < after * columns; i++) {
            (*newvalues)[desBegin + i] = (*mValues)[srcBegin + i];
        }
    }

    for (Int32 i = 0; i < columns; i++) {
        if ((*valuegap)[i] >= rowgapstart) {
            (*valuegap)[i] += newsize - mRows;

            if ((*valuegap)[i] < rowgapstart) {
                (*valuegap)[i] = rowgapstart;
            }
        }
    }

    mRowGapLength += newsize - mRows;
    mRows = newsize;
    mValues = newvalues;
}

void PackedIntVector::MoveValueGapTo(
    /* [in] */ Int32 column,
    /* [in] */ Int32 where)
{
    if (where == (*mValueGap)[column]) {
        return;
    }
    else if (where > (*mValueGap)[column]) {
        for (Int32 i = (*mValueGap)[column]; i < where; i++) {
            (*mValues)[i * mColumns + column] += (*mValueGap)[column + mColumns];
        }
    }
    else /* where < valuegap[column] */ {
        for (Int32 i = where; i < (*mValueGap)[column]; i++) {
            (*mValues)[i * mColumns + column] -= (*mValueGap)[column + mColumns];
        }
    }

    (*mValueGap)[column] = where;
}

void PackedIntVector::MoveRowGapTo(
    /* [in] */ Int32 where)
{
    if (where == mRowGapStart) {
        return;
    }
    else if (where > mRowGapStart) {
        Int32 moving = where + mRowGapLength - (mRowGapStart + mRowGapLength);
        Int32 gapend = mRowGapStart + mRowGapLength;

        for (Int32 i = gapend; i < gapend + moving; i++) {
            Int32 destrow = i - gapend + mRowGapStart;

            for (Int32 j = 0; j < mColumns; j++) {
                Int32 val = (*mValues)[i * mColumns + j];

                if (i >= (*mValueGap)[j]) {
                    val += (*mValueGap)[j + mColumns];
                }

                if (destrow >= (*mValueGap)[j]) {
                    val -= (*mValueGap)[j + mColumns];
                }

                (*mValues)[destrow * mColumns + j] = val;
            }
        }
    }
    else /* where < mRowGapStart */ {
        Int32 moving = mRowGapStart - where;
        Int32 gapend = mRowGapStart + mRowGapLength;

        for (Int32 i = where + moving - 1; i >= where; i--) {
            Int32 destrow = i - where + gapend - moving;

            for (Int32 j = 0; j < mColumns; j++) {
                Int32 val = (*mValues)[i * mColumns + j];

                if (i >= (*mValueGap)[j]) {
                    val += (*mValueGap)[j + mColumns];
                }

                if (destrow >= (*mValueGap)[j]) {
                    val -= (*mValueGap)[j + mColumns];
                }

                (*mValues)[destrow * mColumns + j] = val;
            }
        }
    }

    mRowGapStart = where;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos