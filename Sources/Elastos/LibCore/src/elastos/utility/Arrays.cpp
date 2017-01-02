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

#include "Arrays.h"
#include "CByte.h"
#include "CChar32.h"
#include "CInteger16.h"
#include "CInteger32.h"
#include "CInteger64.h"
#include "CFloat.h"
#include "CDouble.h"
#include "CString.h"
#include "CArrayList.h"
#include <cutils/log.h>

using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::IChar32;
using Elastos::Core::CChar32;
using Elastos::Core::IInteger16;
using Elastos::Core::CInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::IDouble;
using Elastos::Core::CDouble;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;


namespace Elastos {
namespace Utility {

ECode Arrays::CheckBinarySearchBounds(
    /* [in] */ Int32 startIndex,
    /* [in] */ Int32 endIndex,
    /* [in] */ Int32 length)
{
    if (startIndex > endIndex) {
        ALOGE("Arrays::CheckBinarySearchBounds: E_ILLEGAL_ARGUMENT_EXCEPTION: len: %d, start: %d, end: %d",
            length, startIndex, endIndex);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (startIndex < 0 || endIndex > length) {
        ALOGE("Arrays::CheckBinarySearchBounds: E_INDEX_OUT_OF_BOUNDS_EXCEPTION: len: %d, start: %d, end: %d",
            length, startIndex, endIndex);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

ECode Arrays::CheckStartAndEnd(
    /* [in] */ Int32 len,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (start < 0 || end > len) {
        ALOGE("Arrays::CheckStartAndEnd: E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION: len: %d, start: %d, end: %d",
            len, start, end);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (start > end) {
        ALOGE("Arrays::CheckStartAndEnd: E_ILLEGAL_ARGUMENT_EXCEPTION: len: %d, start: %d, end: %d",
            len, start, end);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Arrays::CheckOffsetAndCount(
    /* [in] */ Int32 arrayLength,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    if ((offset | count) < 0 || offset > arrayLength || arrayLength - offset < count) {
        ALOGE("Arrays::CheckOffsetAndCount: E_INDEX_OUT_OF_BOUNDS_EXCEPTION: arrayLength: %d, offset: %d, count: %d",
            arrayLength, offset, count);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

//====================================================================
// Arrays::ArrayList
//====================================================================
// CAR_INTERFACE_IMPL_2(Arrays::ArrayList, AbstractList, ISerializable, IRandomAccess)

// Arrays::ArrayList::ArrayList(
//     /* [in] */ ArrayOf<IInterface *> * storage)
// {
//     if (storage == NULL) {
// //        throw new NullPointerException("storage == null");
//         return;
//     }
//     mA = storage;
// }

// ECode Arrays::ArrayList::Contains(
//     /* [in] */ IInterface* object,
//     /* [out] */ Boolean* result)
// {
//     VALIDATE_NOT_NULL(result)
//     if (object != NULL) {
//         for (Int32 i = 0; i < mA->GetLength(); i++) {
//             AutoPtr<IInterface> element = (*mA)[i];
//             if (Object::Equals(object, element)) {
//                 *result = TRUE;
//                 return NOERROR;
//             }
//         }
//     }
//     else {
//         for (Int32 i = 0; i < mA->GetLength(); i++) {
//             AutoPtr<IInterface> element = (*mA)[i];
//             if (element == NULL) {
//                 *result = TRUE;
//                 return NOERROR;
//             }
//         }
//     }
//     *result = FALSE;
//     return NOERROR;
// }

// ECode Arrays::ArrayList::Get(
//     /* [in] */ Int32 location,
//     /* [out] */ IInterface** result)
// {
//     VALIDATE_NOT_NULL(result)
//     if (location < 0 || location >= mA->GetLength()) {
//         *result = NULL;
//         return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
//     }
//     *result = (*mA)[location];
//     REFCOUNT_ADD(*result)
//     return NOERROR;
// }

// ECode Arrays::ArrayList::IndexOf(
//     /* [in] */ IInterface* object,
//     /* [out] */ Int32* result)
// {
//     VALIDATE_NOT_NULL(result)
//     if (object != NULL) {
//         for (Int32 i = 0; i < mA->GetLength(); i++) {
//             if (Object::Equals(object, (*mA)[i])) {
//                 *result = i;
//                 return NOERROR;
//             }
//         }
//     }
//     else {
//         for (Int32 i = 0; i < mA->GetLength(); i++) {
//             if ((*mA)[i] == NULL) {
//                 *result = i;
//                 return NOERROR;
//             }
//         }
//     }
//     *result = -1;
//     return NOERROR;
// }

// ECode Arrays::ArrayList::LastIndexOf(
//     /* [in] */ IInterface* object,
//     /* [out] */ Int32* index)
// {
//     VALIDATE_NOT_NULL(index)
//     if (object != NULL) {
//         for (Int32 i = mA->GetLength() - 1; i >= 0; i--) {
//             if (Object::Equals(object, (*mA)[i])) {
//                 *index = i;
//                 return NOERROR;
//             }
//         }
//     }
//     else {
//         for (Int32 i = mA->GetLength() - 1; i >= 0; i--) {
//             if ((*mA)[i] == NULL) {
//                 *index = i;
//                 return NOERROR;
//             }
//         }
//     }
//     *index = -1;
//     return NOERROR;
// }

// ECode Arrays::ArrayList::Set(
//     /* [in] */ Int32 location,
//     /* [in] */ IInterface* object,
//     /* [out] */ IInterface** preObject)
// {
//     VALIDATE_NOT_NULL(preObject)
//     AutoPtr<IInterface> result = (*mA)[location];
//     mA->Set(location, object);
//     *preObject = result;
//     REFCOUNT_ADD(*preObject)
//     return NOERROR;
// }

// ECode Arrays::ArrayList::GetSize(
//     /* [out] */ Int32* size)
// {
//     VALIDATE_NOT_NULL(size)
//     *size = mA->GetLength();
//     return NOERROR;
// }

// ECode Arrays::ArrayList::ToArray(
//     /* [out] */ ArrayOf<IInterface *> ** outArray)
// {
//     VALIDATE_NOT_NULL(outArray)
//     *outArray = mA->Clone();
//     REFCOUNT_ADD(*outArray)
//     return NOERROR;
// }

// ECode Arrays::ArrayList::ToArray(
//     /* [in] */ ArrayOf<IInterface *> * contents,
//     /* [out] */ ArrayOf<IInterface *> ** outArray)
// {
//     VALIDATE_NOT_NULL(outArray)
//     Int32 size;
//     GetSize(&size);
//     AutoPtr<ArrayOf<IInterface*> > array = size > contents->GetLength() ?
//             ArrayOf<IInterface*>::Alloc(size) : contents;
//     array->Copy(mA, size);
//     if (size < contents->GetLength()) {
//         array->Set(size, NULL);
//     }
//     *outArray = array;
//     REFCOUNT_ADD(*outArray)
//     return NOERROR;
// }

//====================================================================
// Arrays
//====================================================================
AutoPtr<IList> Arrays::CreateArrayList(
    /* [in] */ Int32 length)
{
    // return new ArrayList<T>(array);
    AutoPtr<IList> al;
    CArrayList::New(length, (IList**)&al);
    return al;
}

ECode Arrays::AsList(
    /* [in] */ ArrayOf<Byte> * array,
    /* [out] */ IList** outList)
{
    VALIDATE_NOT_NULL(outList)
    *outList = NULL;

    if (array) {
        AutoPtr<IList> l = CreateArrayList(array->GetLength());

        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<IByte> obj;
            CByte::New((*array)[i], (IByte**)&obj);
            l->Add(i, obj);
        }

        *outList = l;
        REFCOUNT_ADD(*outList)
    }

    return NOERROR;
}

ECode Arrays::AsList(
    /* [in] */ ArrayOf<Char32> * array,
    /* [out] */ IList** outList)
{
    VALIDATE_NOT_NULL(outList)
    *outList = NULL;

    if (array) {
        AutoPtr<IList> l = CreateArrayList(array->GetLength());

        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<IChar32> obj;
            CChar32::New((*array)[i], (IChar32**)&obj);
            l->Add(i, obj);
        }

        *outList = l;
        REFCOUNT_ADD(*outList)
    }

    return NOERROR;
}

ECode Arrays::AsList(
    /* [in] */ ArrayOf<Int16> * array,
    /* [out] */ IList** outList)
{
    VALIDATE_NOT_NULL(outList)
    *outList = NULL;

    if (array) {
        AutoPtr<IList> l = CreateArrayList(array->GetLength());

        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<IInteger16> obj;
            CInteger16::New((*array)[i], (IInteger16**)&obj);
            l->Add(i, obj);
        }

        *outList = l;
        REFCOUNT_ADD(*outList)
    }

    return NOERROR;
}

ECode Arrays::AsList(
    /* [in] */ ArrayOf<Int32> * array,
    /* [out] */ IList** outList)
{
    VALIDATE_NOT_NULL(outList)
    *outList = NULL;

    if (array) {
        AutoPtr<IList> l = CreateArrayList(array->GetLength());

        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<IInteger32> obj;
            CInteger32::New((*array)[i], (IInteger32**)&obj);
            l->Add(i, obj);
        }

        *outList = l;
        REFCOUNT_ADD(*outList)
    }

    return NOERROR;
}

ECode Arrays::AsList(
    /* [in] */ ArrayOf<Int64> * array,
    /* [out] */ IList** outList)
{
    VALIDATE_NOT_NULL(outList)
    *outList = NULL;

    if (array) {
        AutoPtr<IList> l = CreateArrayList(array->GetLength());

        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<IInteger64> obj;
            CInteger64::New((*array)[i], (IInteger64**)&obj);
            l->Add(i, obj);
        }

        *outList = l;
        REFCOUNT_ADD(*outList)
    }

    return NOERROR;
}

ECode Arrays::AsList(
    /* [in] */ ArrayOf<Float> * array,
    /* [out] */ IList** outList)
{
    VALIDATE_NOT_NULL(outList)
    *outList = NULL;

    if (array) {
        AutoPtr<IList> l = CreateArrayList(array->GetLength());

        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<IFloat> obj;
            CFloat::New((*array)[i], (IFloat**)&obj);
            l->Add(i, obj);
        }

        *outList = l;
        REFCOUNT_ADD(*outList)
    }

    return NOERROR;
}

ECode Arrays::AsList(
    /* [in] */ ArrayOf<Double> * array,
    /* [out] */ IList** outList)
{
    VALIDATE_NOT_NULL(outList)
    *outList = NULL;

    if (array) {
        AutoPtr<IList> l = CreateArrayList(array->GetLength());

        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<IDouble> obj;
            CDouble::New((*array)[i], (IDouble**)&obj);
            l->Add(i, obj);
        }

        *outList = l;
        REFCOUNT_ADD(*outList)
    }

    return NOERROR;
}

ECode Arrays::AsList(
    /* [in] */ ArrayOf<String> * array,
    /* [out] */ IList** outList)
{
    VALIDATE_NOT_NULL(outList)
    *outList = NULL;

    if (array) {
        AutoPtr<IList> l = CreateArrayList(array->GetLength());

        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<ICharSequence> obj;
            CString::New((*array)[i], (ICharSequence**)&obj);
            l->Add(i, obj);
        }

        *outList = l;
        REFCOUNT_ADD(*outList)
    }

    return NOERROR;
}

ECode Arrays::BinarySearch(
    /* [in] */ ArrayOf<String>* array,
    /* [in] */ const String& value,
    /* [out] */ Int32* index)
{
    return BinarySearch(array, 0, array->GetLength(), value, index);
}

ECode Arrays::BinarySearch(
    /* [in] */ const AutoPtr<ArrayOf<String> > & array,
    /* [in] */ const String& value,
    /* [out] */ Int32* index)
{
    return BinarySearch(array.Get(), value, index);
}

ECode Arrays::BinarySearch(
    /* [in] */ ArrayOf<String>* array,
    /* [in] */ Int32 startIndex,
    /* [in] */ Int32 endIndex,
    /* [in] */ const String& value,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    *index = -1;
    assert(array != NULL);
    VALIDATE_NOT_NULL(array)

    ECode ec = CheckBinarySearchBounds(startIndex, endIndex, array->GetLength());
    if (FAILED(ec)) {
        ALOGE("Arrays::BinarySearch: error %08x, startIndex: %d, endIndex: %d, array length: %d",
            ec, startIndex, endIndex, array->GetLength());
        return ec;
    }

    Int32 lo = startIndex;
    Int32 hi = endIndex - 1;
    Int32 mid;

    String midVal;
    while (lo <= hi) {
        mid = (UInt32(lo + hi)) >> 1;
        midVal = (*array)[mid];

        if (midVal.Compare(value) < 0) {
            lo = mid + 1;
        }
        else if (midVal.Compare(value) > 0) {
            hi = mid - 1;
        }
        else {
            *index = mid;  // value found
            return NOERROR;
        }
    }
    *index = ~lo;  // value not present
    return NOERROR;
}

ECode Arrays::BinarySearch(
    /* [in] */ const AutoPtr<ArrayOf<String> > & array,
    /* [in] */ Int32 startIndex,
    /* [in] */ Int32 endIndex,
    /* [in] */ const String& value,
    /* [out] */ Int32* index)
{
    return BinarySearch(array.Get(), startIndex, endIndex, value, index);
}

Int32 Arrays::GetHashCode(
    /* [in] */ ArrayOf<Boolean>* array)
{
    if (array == NULL) {
        return 0;
    }

    Int32 hashCode = 1;
    for (Int32 i = 0; i < array->GetLength(); i++) {
        // 1231, 1237 are hash code values for boolean value
        hashCode = 31 * hashCode + ((*array)[i] ? 1231 : 1237);
    }
    return hashCode;
}

Int32 Arrays::GetHashCode(
    /* [in] */ ArrayOf<Int64>* array)
{
    if (array == NULL) {
        return 0;
    }

    Int32 hashCode = 1;
    Int64 elementValue;
    for (Int32 i = 0; i < array->GetLength(); i++) {
        elementValue = (*array)[i];
        /*
         * the hash code value for long value is (int) (value ^ (value >>>
         * 32))
         */
        hashCode = 31 * hashCode
                + (Int32) (elementValue ^ (elementValue >> 32));
    }
    return hashCode;
}

Int32 Arrays::GetHashCode(
    /* [in] */ ArrayOf<Float>* array)
{
    if (array == NULL) {
        return 0;
    }

    Int32 hashCode = 1;
    for (Int32 i = 0; i < array->GetLength(); i++) {
        /*
         * the hash code value for float value is
         * Float.floatToIntBits(value)
         */
        hashCode = 31 * hashCode + Elastos::Core::Math::FloatToInt32Bits((*array)[i]);
    }
    return hashCode;
}

Int32 Arrays::GetHashCode(
    /* [in] */ ArrayOf<Double>* array)
{
    if (array == NULL) {
        return 0;
    }

    Int32 hashCode = 1;
    Int64 v;
    for (Int32 i = 0; i < array->GetLength(); i++) {
        v = Elastos::Core::Math::DoubleToInt64Bits((*array)[i]);
        /*
         * the hash code value for double value is (int) (v ^ (v >>> 32))
         * where v = Double.doubleToLongBits(value)
         */
        hashCode = 31 * hashCode + (Int32) (v ^ (v >> 32));
    }
    return hashCode;
}

Boolean Arrays::Equals(
    /* [in] */ ArrayOf<Float>* array1,
    /* [in] */ ArrayOf<Float>* array2)
{
    if (array1 == array2) {
        return TRUE;
    }

    if (array1 == NULL || array2 == NULL || array1->GetLength() != array2->GetLength()) {
        return FALSE;
    }
    for (Int32 i = 0; i < array1->GetLength(); i++) {
        if (!Elastos::Core::Math::Equals((*array1)[i], (*array2)[i])) {
            return FALSE;
        }
    }
    return TRUE;
}

Boolean Arrays::Equals(
    /* [in] */ ArrayOf<Double>* array1,
    /* [in] */ ArrayOf<Double>* array2)
{
    if (array1 == array2) {
        return TRUE;
    }

    if (array1 == NULL || array2 == NULL || array1->GetLength() != array2->GetLength()) {
        return FALSE;
    }

    for (Int32 i = 0; i < array1->GetLength(); i++) {
        if (!Elastos::Core::Math::Equals((*array1)[i], (*array2)[i])) {
            return FALSE;
        }
    }

    return TRUE;
}

String Arrays::ToString(
    /* [in] */ ArrayOf<Char32>* array)
{
    if (array == NULL) {
        return String("NULL");
    }
    if (array->GetLength() == 0) {
        return String("[]");
    }
    StringBuilder sb(array->GetLength() * 3);
    sb.AppendChar('[');
    sb.AppendChar((*array)[0]);
    for (Int32 i = 1; i < array->GetLength(); i++) {
        sb.Append(", ");
        sb.AppendChar((*array)[i]);
    }
    sb.AppendChar(']');
    return sb.ToString();
}

ECode Arrays::DeepToString(
    /* [in] */ IInterface * obj,
    /* [in] */ IArrayOf * origArray,
    /* [in] */ StringBuilder& sb)
{
    IArrayOf* array = IArrayOf::Probe(obj);
    if (array == NULL) {
        String info = Object::ToString(obj);
        sb.Append(info);
        return NOERROR;
    }

    if (DeepToStringContains(origArray, obj)) {
        sb.Append("[...]");
    }
    else {
        sb.AppendChar('[');
        Int32 length;
        array->GetLength(&length);
        for (Int32 i = 0; i < length; ++i) {
            AutoPtr<IInterface> element;
            array->Get(i, (IInterface**)&element);
            if (i != 0) {
                sb += ", ";
            }
            DeepToString(element, origArray, sb);
        }
        sb.AppendChar(']');
    }

    return NOERROR;
}

Boolean Arrays::DeepToStringContains(
    /* [in] */ IArrayOf * origArray,
    /* [in] */ IInterface * obj)
{
    if (origArray == NULL) {
        return FALSE;
    }

    Int32 length;
    origArray->GetLength(&length);
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IInterface> element;
        origArray->Get(i, (IInterface**)&element);
        if (element.Get() == obj) {
            return TRUE;
        }
    }

    return FALSE;
}

} // namespace Utility
} // namespace Elastos
