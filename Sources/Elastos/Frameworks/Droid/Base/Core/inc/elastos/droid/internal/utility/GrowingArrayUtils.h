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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_GROWING_ARRAYUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_GROWING_ARRAYUTILS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

/**
 * A helper class that aims to provide comparable growth performance to ArrayList, but on primitive
 * arrays. Common array operations are implemented for efficient use in dynamic containers.
 *
 * All methods in this class assume that the length of an array is equivalent to its capacity and
 * NOT the number of elements in the array. The current size of the array is always passed in as a
 * parameter.
 *
 * @hide
 */
class GrowingArrayUtils
{
public:
    /**
     * Appends an element to the end of the array, growing the array if there is no more room.
     * @param array The array to which to append the element. This must NOT be null.
     * @param currentSize The number of elements in the array. Must be less than or equal to
     *                    array.length.
     * @param element The element to append.
     * @return the array to which the element was appended. This may be different than the given
     *         array.
     */
    template<typename T>
    static AutoPtr<ArrayOf<T> > Append(
        /* [in] */ ArrayOf<T>* array,
        /* [in] */ Int32 currentSize,
        /* [in] */ T element);

    template<typename T>
    static AutoPtr<ArrayOf<T> > Append(
        /* [in] */ AutoPtr< ArrayOf<T> >& array,
        /* [in] */ Int32 currentSize,
        /* [in] */ T element);

    /**
     * Inserts an element into the array at the specified index, growing the array if there is no
     * more room.
     *
     * @param array The array to which to append the element. Must NOT be null.
     * @param currentSize The number of elements in the array. Must be less than or equal to
     *                    array.length.
     * @param element The element to insert.
     * @return the array to which the element was appended. This may be different than the given
     *         array.
     */
    template<typename T>
    static AutoPtr<ArrayOf<T> > Insert(
        /* [in] */ ArrayOf<T>* array,
        /* [in] */ Int32 currentSize,
        /* [in] */ Int32 index,
        /* [in] */ T element);

    template<typename T>
    static AutoPtr<ArrayOf<T> > Insert(
        /* [in] */ AutoPtr< ArrayOf<T> >& array,
        /* [in] */ Int32 currentSize,
        /* [in] */ Int32 index,
        /* [in] */ T element);

    /**
     * Given the current size of an array, returns an ideal size to which the array should grow.
     * This is typically double the given size, but should not be relied upon to do so in the
     * future.
     */
    static Int32 GrowSize(
        /* [in] */ Int32 currentSize);

private:
    GrowingArrayUtils();
    GrowingArrayUtils(const GrowingArrayUtils&);
};

template<typename T>
AutoPtr<ArrayOf<T> > GrowingArrayUtils::Append(
    /* [in] */ ArrayOf<T>* array,
    /* [in] */ Int32 currentSize,
    /* [in] */ T element)
 {
    assert(array != NULL && currentSize <= array->GetLength());

    AutoPtr<ArrayOf<T> > result = array;
    if (currentSize + 1 > array->GetLength()) {
        // @SuppressWarnings("unchecked")
        result = ArrayUtils::NewUnpaddedArray(GrowSize(currentSize), element);
        result->Copy(array);
    }
    result->Set(currentSize, element);
    return result;
}

template<typename T>
AutoPtr<ArrayOf<T> > GrowingArrayUtils::Append(
    /* [in] */ AutoPtr< ArrayOf<T> >& array,
    /* [in] */ Int32 currentSize,
    /* [in] */ T element)
{
    return Append(array.Get(), currentSize, element);
}

template<typename T>
AutoPtr<ArrayOf<T> > GrowingArrayUtils::Insert(
    /* [in] */ ArrayOf<T>* array,
    /* [in] */ Int32 currentSize,
    /* [in] */ Int32 index,
    /* [in] */ T element)
{
    assert(array != NULL && currentSize <= array->GetLength());

    if (currentSize + 1 <= array->GetLength()) {
        array->Copy(index + 1, array, index, currentSize - index);
        array->Set(index, element);
        return array;
    }

    // @SuppressWarnings("unchecked")
    AutoPtr<ArrayOf<T> > newArray = ArrayUtils::NewUnpaddedArray(GrowSize(currentSize), element);
    newArray->Copy(array, index);
    newArray->Set(index, element);
    newArray->Copy(index + 1, array, index, array->GetLength() - index);
    return newArray;
}

template<typename T>
AutoPtr<ArrayOf<T> > GrowingArrayUtils::Insert(
    /* [in] */ AutoPtr< ArrayOf<T> >& array,
    /* [in] */ Int32 currentSize,
    /* [in] */ Int32 index,
    /* [in] */ T element)
{
    return Insert(array.Get(), currentSize, index, element);
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_GROWING_ARRAYUTILS_H__
