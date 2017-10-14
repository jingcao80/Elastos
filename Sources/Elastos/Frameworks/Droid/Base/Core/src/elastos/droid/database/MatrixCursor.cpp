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

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/database/MatrixCursor.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::INumber;
using Elastos::Core::IDouble;
using Elastos::Core::IFloat;
using Elastos::Core::IInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::ICharSequence;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::StringUtils;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {

MatrixCursor::RowBuilder::RowBuilder()
    : mRow(0)
    , mEndIndex(0)
    , mIndex(0)
{}

MatrixCursor::RowBuilder::~RowBuilder()
{}

ECode MatrixCursor::RowBuilder::constructor(
    /* [in] */ Int32 row,
    /* [in] */ IMatrixCursor* owner)
{
    mRow = row;
    mIndex = row * (((MatrixCursor*)owner)->mColumnCount);
    mOwner = (MatrixCursor*)owner;
    mEndIndex = mIndex + ((MatrixCursor*)owner)->mColumnCount;
    return NOERROR;
}

CAR_INTERFACE_IMPL(MatrixCursor::RowBuilder, Object, IRowBuilder)

ECode MatrixCursor::RowBuilder::Add(
    /* [in] */ IInterface* columnValue)
{
    if (mIndex == mEndIndex) {
//      throw new CursorIndexOutOfBoundsException("No more columns left.");
        Slogger::E(String("RowBuilder"), "No more columns left.");
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    mOwner->mData->Set(mIndex++,  columnValue);
    return NOERROR;
}

ECode MatrixCursor::RowBuilder::Add(
    /* [in] */ const String& columnName,
    /* [in] */ IInterface* value)
{
    for (Int32 i = 0; i < mOwner->mColumnNames->GetLength(); i++) {
        if (columnName.Equals((*(mOwner->mColumnNames))[i])) {
            mOwner->mData->Set((mRow * mOwner->mColumnCount) + i , value);
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(MatrixCursor, AbstractCursor, IMatrixCursor)

MatrixCursor::MatrixCursor()
    : mRowCount(0)
    , mColumnCount(0)
{}

ECode MatrixCursor::constructor(
    /* [in] */ ArrayOf<String>* columnNames,
    /* [in] */ Int32 initialCapacity)
{
    mColumnNames = columnNames;
    mColumnCount = columnNames->GetLength();

    if (initialCapacity < 1) {
        initialCapacity = 1;
    }

    mData = ArrayOf<IInterface*>::Alloc(mColumnCount * initialCapacity);
    return NOERROR;
}

ECode MatrixCursor::constructor(
    /* [in] */ ArrayOf<String>* columnNames)
{
    return constructor(columnNames, 16);
}

ECode MatrixCursor::Get(
    /* [in] */ Int32 column,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    if (column < 0 || column >= mColumnCount) {
        // throw new CursorIndexOutOfBoundsException("Requested column: "
        //         + column + ", # of columns: " +  columnCount);
        Slogger::E(String("MatrixCursor"), "Requested column: %d, # of columns: ", column, mColumnCount);
        return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (mPos < 0) {
        // throw new CursorIndexOutOfBoundsException("Before first row.");
        Slogger::E(String("MatrixCursor"), "Before first row.");
        return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (mPos >= mRowCount) {
        // throw new CursorIndexOutOfBoundsException("After last row.");
        Slogger::E(String("MatrixCursor"), "After last row.");
        return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    *obj = (*mData)[mPos * mColumnCount + column];
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode MatrixCursor::NewRow(
    /* [out] */ IRowBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)

    const Int32 row = mRowCount++;
    const Int32 endIndex = mRowCount * mColumnCount;
    EnsureCapacity(endIndex);
    AutoPtr<RowBuilder> m_builder = new RowBuilder();
    m_builder->constructor(row, this);
    *builder = IRowBuilder::Probe(m_builder);
    REFCOUNT_ADD(*builder)
    return NOERROR;
}

ECode MatrixCursor::AddRow(
    /* [in] */ const ArrayOf<IInterface*>& columnValues)
{
    if (columnValues.GetLength() != mColumnCount) {
//         throw new IllegalArgumentException("columnNames.length = "
//                    + columnCount + ", columnValues.length = "
//                    + columnValues.length);
        Slogger::E(String("MatrixCursor"), "columnNames.length = %d, columnValues.length = %d"
                , mColumnCount, columnValues.GetLength());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 start = mRowCount++ *mColumnCount;
    EnsureCapacity(start + mColumnCount);
    mData->Copy(start, &columnValues, 0, mColumnCount);
    return NOERROR;
}

ECode MatrixCursor::AddRow(
    /* [in] */ IIterable* columnValues)
{
    Int32 start = mRowCount * mColumnCount;
    Int32 end = start + mColumnCount;
    EnsureCapacity(end);

    //if (columnValues instanceof ArrayList<?>) {
    //    addRow((ArrayList<?>) columnValues, start);
    //    return;
    //}

    Int32 current = start;
    AutoPtr< ArrayOf<IInterface*> > localData = mData;
    AutoPtr<IIterator> it;
    columnValues->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> columnValue;
        it->GetNext((IInterface**)&columnValue);
        if (current == end) {
            // TODO: null out row?
            //throw new IllegalArgumentException(
            //        "columnValues.size() > columnNames.length");
            Slogger::E(String("MatrixCursor"), "columnValues.size() > columnNames.length");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        localData->Set(current++, columnValue);
    }

    if (current != end) {
        // TODO: null out row?
        //throw new IllegalArgumentException(
        //        "columnValues.size() < columnNames.length");
        Slogger::E(String("MatrixCursor"), "columnValues.size() < columnNames.length");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Increase row count here in case we encounter an exception.
    mRowCount++;
    return NOERROR;
}

//MatrixCursor::AddRow(
//        /* [in] */ ArrayList<?> columnValues,
//        /* [in] */ Int32 start)
/*{
    Int32 size = columnValues.GetSize();
    if (size != columnCount) {
        //throw new IllegalArgumentException("columnNames.length = "
        //        + columnCount + ", columnValues.size() = " + size);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mRowCount++;
    ArrayOf< AutoPtr<IInterface> >* localData = mData;
    for (Int32 i = 0; i < size; i++) {
        (*localData)[start + i] = columnValues[i];
    }
}*/

void MatrixCursor::EnsureCapacity(
    /* [in] */ Int32 size)
{
    Int32 oldSize = mData->GetLength();
    if (size > oldSize) {
        AutoPtr< ArrayOf<IInterface*> > oldData = mData;
        Int32 newSize = oldSize * 2;
        if (newSize < size) {
            newSize = size;
        }
        mData = ArrayOf<IInterface *>::Alloc(newSize);
        mData->Copy(0, oldData, 0, oldSize);
    }
}

ECode MatrixCursor::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = mRowCount;
    return NOERROR;
}

ECode MatrixCursor::GetColumnNames(
    /* [out, callee] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names)
    *names = mColumnNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode MatrixCursor::GetString(
    /* [in] */ Int32 column,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    AutoPtr<IInterface> value;
    Get(column, (IInterface**)&value);
    if (value == NULL) {
        *str = String(NULL);
        return NOERROR;
    }
    ICharSequence::Probe(value)->ToString(str);
    return NOERROR;
}

ECode MatrixCursor::GetInt16(
    /* [in] */ Int32 column,
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<IInterface> _value;
    String tmpStr;
    Get(column, (IInterface**)&_value);
    if (_value == NULL) {
        *value = 0;
        return NOERROR;
    }
    if (INumber::Probe(_value) != NULL) {
        INumber::Probe(_value)->Int16Value(value);
        return NOERROR;
    }
    *value = StringUtils::ParseInt16(TO_STR(_value));
    return NOERROR;
}

ECode MatrixCursor::GetInt32(
    /* [in] */ Int32 column,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<IInterface> _value;
    Get(column, (IInterface**)&_value);
    if (_value == NULL) {
        *value = 0;
        return NOERROR;
    }
    if (INumber::Probe(_value) != NULL) {
        INumber::Probe(_value)->Int32Value(value);
        return NOERROR;
    }
    *value = StringUtils::ParseInt32(TO_STR(_value));
    return NOERROR;
}

ECode MatrixCursor::GetInt64(
    /* [in] */ Int32 column,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<IInterface> _value;
    Get(column, (IInterface**)&_value);
    if (_value == NULL) {
        *value = 0;
        return NOERROR;
    }
    if (INumber::Probe(_value) != NULL) {
        INumber::Probe(_value)->Int64Value(value);
        return NOERROR;
    }
    *value = StringUtils::ParseInt64(TO_STR(_value));
    return NOERROR;
}

ECode MatrixCursor::GetFloat(
    /* [in] */ Int32 column,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<IInterface> _value;
    Get(column, (IInterface**)&_value);
    if (_value == NULL) {
        *value = 0.0f;
        return NOERROR;
    }
    if (INumber::Probe(_value) != NULL) {
        INumber::Probe(_value)->FloatValue(value);
        return NOERROR;
    }
    *value = StringUtils::ParseFloat(TO_STR(_value));
    return NOERROR;
}

ECode MatrixCursor::GetDouble(
    /* [in] */ Int32 column,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<IInterface> _value;
    Get(column, (IInterface**)&_value);
    if (_value == NULL) {
        *value = 0.0l;
        return NOERROR;
    }
    if (INumber::Probe(_value) != NULL) {
        INumber::Probe(_value)->DoubleValue(value);
        return NOERROR;
    }
    *value = StringUtils::ParseDouble(TO_STR(_value));
    return NOERROR;
}

ECode MatrixCursor::GetBlob(
    /* [in] */ Int32 column,
    /* [out, callee] */ ArrayOf<Byte>** blob)
{
    VALIDATE_NOT_NULL(blob)

    AutoPtr<IInterface> value;
    Get(column, (IInterface**)&value);
    if (value == NULL) {
        *blob = NULL;
        return NOERROR;
    }
    if (IArrayOf::Probe(value) != NULL) {
        AutoPtr<IArrayOf> array = IArrayOf::Probe(value);
        Int32 len;
        array->GetLength(&len);
        AutoPtr< ArrayOf<Byte> > _blob = ArrayOf<Byte>::Alloc(len);
        for (Int32 i = 0; i < len; ++i) {
            AutoPtr<IInterface> b;
            array->Get(i, (IInterface**)&b);
            IByte::Probe(b)->GetValue(&(*_blob)[i]);
        }
        *blob = _blob;
        REFCOUNT_ADD(*blob)
        return NOERROR;
    }
    *blob = NULL;
    return NOERROR;
}

//@Override
ECode MatrixCursor::GetType(
    /* [in] */ Int32 column,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)

    AutoPtr<IInterface> value;
    Get(column, (IInterface**)&value);
    *type = DatabaseUtils::GetTypeOfObject(value);
    return NOERROR;
}

ECode MatrixCursor::IsNull(
    /* [in] */ Int32 column,
    /* [out] */ Boolean* isNull)
{
    VALIDATE_NOT_NULL(isNull)

    AutoPtr<IInterface> value;
    Get(column, (IInterface**)&value);
    *isNull = value == NULL;
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
