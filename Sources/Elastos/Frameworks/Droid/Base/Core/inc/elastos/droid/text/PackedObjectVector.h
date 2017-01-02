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

#ifndef __ELASTOS_DROID_TEXT_PACKEDOBJECTVECTOR_H__
#define __ELASTOS_DROID_TEXT_PACKEDOBJECTVECTOR_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/internal/utility/GrowingArrayUtils.h"
#include <elastos/core/Object.h>
#include <libcore/utility/EmptyArray.h>

using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::GrowingArrayUtils;
using Libcore::Utility::EmptyArray;

namespace Elastos {
namespace Droid {
namespace Text {

template<typename E/*implements IInterface*/>
class PackedObjectVector : public ElRefBase
{
public:
    PackedObjectVector(
        /* [in] */ Int32 columns);

    virtual CARAPI_(AutoPtr<E>) GetValue(
        /* [in] */ Int32 row,
        /* [in] */ Int32 column);

    virtual CARAPI_(void) SetValue(
        /* [in] */ Int32 row,
        /* [in] */ Int32 column,
        /* [in] */ E* value);

    virtual CARAPI_(void) InsertAt(
        /* [in] */ Int32 row,
        /* [in] */ ArrayOf<E*>* values);

    virtual CARAPI_(void) DeleteAt(
        /* [in] */ Int32 row,
        /* [in] */ Int32 count);

    virtual CARAPI_(Int32) Size();

    virtual CARAPI_(Int32) Width();

    virtual CARAPI_(void) Dump();

private:
    CARAPI_(void) GrowBuffer();

    CARAPI_(void) MoveRowGapTo(
        /* [in] */ Int32 where);

private:
    Int32 mColumns;
    Int32 mRows;

    Int32 mRowGapStart;
    Int32 mRowGapLength;

    AutoPtr< ArrayOf<IInterface*> > mValues;
};

template<typename E>
PackedObjectVector<E>::PackedObjectVector(
    /* [in] */ Int32 columns)
    : mColumns(columns)
    , mRows(0)
    , mRowGapStart(0)
    , mRowGapLength(mRows)
{
    mValues = EmptyArray::OBJECT;
}

template<typename E>
AutoPtr<E> PackedObjectVector<E>::GetValue(
    /* [in] */ Int32 row,
    /* [in] */ Int32 column)
{
    if (row >= mRowGapStart) {
        row += mRowGapLength;
    }

    AutoPtr<IInterface> value = (*mValues)[row * mColumns + column];

    return (E*)value.Get();
}

template<typename E>
void PackedObjectVector<E>::SetValue(
    /* [in] */ Int32 row,
    /* [in] */ Int32 column,
    /* [in] */ E* value)
{
    if (row >= mRowGapStart) {
        row += mRowGapLength;
    }

    mValues->Set(row * mColumns + column, value);
}

template<typename E>
void PackedObjectVector<E>::InsertAt(
    /* [in] */ Int32 row,
    /* [in] */ ArrayOf<E*>* values)
{
    MoveRowGapTo(row);

    if (mRowGapLength == 0) {
        GrowBuffer();
    }

    mRowGapStart++;
    mRowGapLength--;

    if (values == NULL) {
        for (Int32 i = 0; i < mColumns; i++) {
            SetValue(row, i, NULL);
        }
    }
    else {
        for (Int32 i = 0; i < mColumns; i++) {
            SetValue(row, i, (*values)[i]);
        }
    }
}

template<typename E>
void PackedObjectVector<E>::DeleteAt(
    /* [in] */ Int32 row,
    /* [in] */ Int32 count)
{
    MoveRowGapTo(row + count);

    mRowGapStart -= count;
    mRowGapLength += count;

    if (mRowGapLength > Size() * 2) {
        // dump();
        // growBuffer();
    }
}

template<typename E>
Int32 PackedObjectVector<E>::Size()
{
    return mRows - mRowGapLength;
}

template<typename E>
Int32 PackedObjectVector<E>::Width()
{
    return mColumns;
}

template<typename E>
void PackedObjectVector<E>::GrowBuffer()
{
    AutoPtr< ArrayOf<IInterface*> > newvalues = ArrayUtils::NewUnpaddedObjectArray(
            GrowingArrayUtils::GrowSize(Size()) * mColumns);
    Int32 newsize = newvalues->GetLength() / mColumns;

    Int32 after = mRows - (mRowGapStart + mRowGapLength);

    newvalues->Copy(0, mValues, 0, mColumns * mRowGapStart);
    newvalues->Copy((newsize - after) * mColumns, mValues, (mRows - after) * mColumns, after * mColumns);

    mRowGapLength += newsize - mRows;
    mRows = newsize;
    mValues = newvalues;
}

template<typename E>
void PackedObjectVector<E>::MoveRowGapTo(
    /* [in] */ Int32 where)
{
    if (where == mRowGapStart) {
        return;
    }

    if (where > mRowGapStart) {
        Int32 moving = where + mRowGapLength - (mRowGapStart + mRowGapLength);

        for (Int32 i = mRowGapStart + mRowGapLength;
            i < mRowGapStart + mRowGapLength + moving; i++) {

            Int32 destrow = i - (mRowGapStart + mRowGapLength) + mRowGapStart;

            for (Int32 j = 0; j < mColumns; j++) {
                AutoPtr<IInterface> val = (*mValues)[i * mColumns + j];

                mValues->Set(destrow * mColumns + j, val);
            }
        }
    }
    else /* where < mRowGapStart */
    {
        Int32 moving = mRowGapStart - where;

        for (Int32 i = where + moving - 1; i >= where; i--){

            Int32 destrow = i - where + mRowGapStart + mRowGapLength - moving;

            for (Int32 j = 0; j < mColumns; j++){
                IInterface* val = (*mValues)[i * mColumns + j];

                mValues->Set(destrow * mColumns + j, val);
            }
        }
    }

    mRowGapStart = where;
}

template<typename E>
void PackedObjectVector<E>::Dump()
{
    assert(0);
#if 0
    for (Int32 i = 0; i < mRows; i++)
    {
        for (Int32 j = 0; j < mColumns; j++)
        {
            IInterface* val = (*mValues)[i * mColumns + j];

            //if (i < mRowGapStart || i >= mRowGapStart + mRowGapLength)
            //    //System.out.print(val + " ");
            //else
            //    //System.out.print("(" + val + ") ");
        }

        //System.out.print(" << \n");
    }

    //System.out.print("-----\n\n");
#endif
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_PACKEDOBJECTVECTOR_H__
