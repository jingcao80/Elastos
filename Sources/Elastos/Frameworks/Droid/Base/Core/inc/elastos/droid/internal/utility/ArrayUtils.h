#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_ARRAYUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_ARRAYUTILS_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Utility::IArraySet;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class ECO_PUBLIC ArrayUtils
{
public:
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) NewUnpaddedByteArray(
        /* [in] */ Int32 minLen);

    static CARAPI_(AutoPtr<ArrayOf<Char32> >) NewUnpaddedChar32Array(
        /* [in] */ Int32 minLen);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) NewUnpaddedInt32Array(
        /* [in] */ Int32 minLen);

    static CARAPI_(AutoPtr<ArrayOf<Int64> >) NewUnpaddedInt64Array(
        /* [in] */ Int32 minLen);

    static CARAPI_(AutoPtr<ArrayOf<Boolean> >) NewUnpaddedBooleanArray(
        /* [in] */ Int32 minLen);

    static CARAPI_(AutoPtr<ArrayOf<Float> >) NewUnpaddedFloatArray(
        /* [in] */ Int32 minLen);

    static CARAPI_(AutoPtr<ArrayOf<Double> >) NewUnpaddedDoubleArray(
        /* [in] */ Int32 minLen);

    static CARAPI_(AutoPtr<ArrayOf<IInterface*> >) NewUnpaddedObjectArray(
        /* [in] */ Int32 minLen);

    template<typename T>
    static CARAPI_(AutoPtr<ArrayOf<T> >) NewUnpaddedArray(
        /* [in] */ Int32 minLen,
        /* [in] */ T typeDeduceObj);

    /**
     * Checks if the beginnings of two byte arrays are equal.
     *
     * @param array1 the first byte array
     * @param array2 the second byte array
     * @param length the number of bytes to check
     * @return TRUE if they're equal, FALSE otherwise
     */
    static CARAPI_(Boolean) Equals(
        /* [in] */ ArrayOf<Byte>* array1,
        /* [in] */ ArrayOf<Byte>* array2,
        /* [in] */ Int32 length);

    /**
     * Returns an empty array of the specified type.  The intent is that
     * it will return the same empty array every time to avoid reallocation,
     * although this is not guaranteed.
     */
    // public static <T> T[] emptyArray(Class<T> kind) {
    //     if (kind == Object.class) {
    //         return (T[]) EmptyArray.OBJECT;
    //     }

    //     Int32 bucket = (kind.hashCode() & 0x7FFFFFFF) % CACHE_SIZE;
    //     Object cache = sCache[bucket];

    //     if (cache == NULL || cache.getClass().getComponentType() != kind) {
    //         cache = Array.newInstance(kind, 0);
    //         sCache[bucket] = cache;

    //         // Log.e("cache", "new empty " + kind.getName() + " at " + bucket);
    //     }

    //     return (T[]) cache;
    // }

    /**
     * Checks if given array is NULL or has zero elements.
     */
    template<typename T>
    static CARAPI_(Boolean) IsEmpty(
        /* [in] */ ArrayOf<T>* array);

    /**
     * Checks that value is present as at least one of the elements of the array.
     * @param array the array to check in
     * @param value the value to check for
     * @return TRUE if the value is present in the array
     */
    template<typename T>
    static CARAPI_(Boolean) Contains(
        /* [in] */ ArrayOf<T>* array,
        /* [in] */ T value);

    /**
     * Return first index of {@code value} in {@code array}, or {@code -1} if
     * not found.
     */
    template<typename T>
    static CARAPI_(Int32) IndexOf(
        /* [in] */ ArrayOf<T>* array,
        /* [in] */ T value);

    static CARAPI_(Int32) IndexOf(
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [in] */ IInterface* value);

    /**
     * Test if all {@code check} items are contained in {@code array}.
     */
    template<typename T>
    static CARAPI_(Boolean) ContainsAll(
        /* [in] */ ArrayOf<T>* array,
        /* [in] */ ArrayOf<T>* check);

    static CARAPI_(Int64) Total(
        /* [in] */ ArrayOf<Int64> *array);

    /**
     * Appends an element to a copy of the array and returns the copy.
     * @param array The original array, or NULL to represent an empty array.
     * @param element The element to add.
     * @return A new array that contains all of the elements of the original array
     * with the specified element added at the end.
     */
    template<typename T>
    static CARAPI_(AutoPtr<ArrayOf<T> >) AppendElement(
        /* [in] */ ArrayOf<T>* array,
        /* [in] */ T element);

    /**
     * Removes an element from a copy of the array and returns the copy.
     * If the element is not present, then the original array is returned unmodified.
     * @param array The original array, or NULL to represent an empty array.
     * @param element The element to remove.
     * @return A new array that contains all of the elements of the original array
     * except the first copy of the specified element removed.  If the specified element
     * was not present, then returns the original array.  Returns NULL if the result
     * would be an empty array.
     */
    template<typename T>
    static CARAPI_(AutoPtr<ArrayOf<T> >) RemoveElement(
        /* [in] */ ArrayOf<T>* array,
        /* [in] */ T element);

    static CARAPI_(AutoPtr< ArrayOf<Int32> >) AppendInt32(
        /* [in] */ ArrayOf<Int32>* cur,
        /* [in] */ Int32 val);

    static CARAPI_(AutoPtr< ArrayOf<Int32> >) RemoveInt32(
        /* [in] */ ArrayOf<Int32>* cur,
        /* [in] */ Int32 val);

    /**
     * Appends a new value to a copy of the array and returns the copy.  If
     * the value is already present, the original array is returned
     * @param cur The original array, or NULL to represent an empty array.
     * @param val The value to add.
     * @return A new array that contains all of the values of the original array
     * with the new value added, or the original array.
     */
    static CARAPI_(AutoPtr< ArrayOf<Int64> >) AppendInt64(
        /* [in] */ ArrayOf<Int64>* cur,
        /* [in] */ Int64 val);

    static CARAPI_(AutoPtr< ArrayOf<Int64> >) RemoveInt64(
        /* [in] */ ArrayOf<Int64>* cur,
        /* [in] */ Int64 val);

    static CARAPI_(AutoPtr< ArrayOf<Int64> >) CloneOrNull(
        /* [in] */ ArrayOf<Int64>* array);

    static CARAPI_(AutoPtr<IArraySet>) Add(
        /* [in] */ IArraySet* _cur,
        /* [in] */ IInterface* val);

    static CARAPI_(AutoPtr<IArraySet>) Remove(
        /* [in] */ IArraySet* cur,
        /* [in] */ IInterface* val);

    static CARAPI_(Boolean) Contains(
        /* [in] */ IArraySet* cur,
        /* [in] */ IInterface* val);

    static CARAPI_(AutoPtr<IArrayList>) Add(
        /* [in] */ IArrayList* _cur,
        /* [in] */ IInterface* val);

    static CARAPI_(AutoPtr<IArrayList>) Remove(
        /* [in] */ IArrayList* cur,
        /* [in] */ IInterface* val);

    static CARAPI_(Boolean) Contains(
        /* [in] */ IArrayList* cur,
        /* [in] */ IInterface* val);

private:
    ECO_LOCAL ArrayUtils() {}
};

template<typename T>
AutoPtr<ArrayOf<T> > ArrayUtils::NewUnpaddedArray(
    /* [in] */ Int32 minLen,
    /* [in] */ T typeDeduceObj)
{
    AutoPtr<ArrayOf<T> > array = ArrayOf<T>::Alloc(minLen);
    return array;
}

template<typename T>
Boolean ArrayUtils::IsEmpty(
    /* [in] */ ArrayOf<T>* array)
{
    return array == NULL || array->GetLength() == 0;
}

template<typename T>
Boolean ArrayUtils::Contains(
    /* [in] */ ArrayOf<T>* array,
    /* [in] */ T value)
{
    return IndexOf(array, value) != -1;
}

template<typename T>
Int32 ArrayUtils::IndexOf(
    /* [in] */ ArrayOf<T>* array,
    /* [in] */ T value)
{
    if (array == NULL)
        return -1;
    for (Int32 i = 0; i < array->GetLength(); i++) {
        if ((*array)[i] == NULL) {
            if (value == NULL)
                return i;
        }
        else {
            if (value == (*array)[i])
                return i;
        }
    }
    return -1;
}

template<typename T>
Boolean ArrayUtils::ContainsAll(
    /* [in] */ ArrayOf<T>* array,
    /* [in] */ ArrayOf<T>* check)
{
    Int32 length = check != NULL ? check->GetLength() : 0;
    for (Int32 i = 0; i < length; i++) {
        if (!Contains(array, (*check)[i])) {
            return FALSE;
        }
    }
    return TRUE;
}

template<typename T>
AutoPtr<ArrayOf<T> > ArrayUtils::AppendElement(
    /* [in] */ ArrayOf<T>* array,
    /* [in] */ T element)
{
    AutoPtr<ArrayOf<T> > result;
    Int32 end;
    if (array != NULL) {
        end = array->GetLength();
        result = ArrayOf<T>::Alloc(end + 1);
        result->Copy(array, end);
    }
    else {
        end = 0;
        result = ArrayOf<T>::Alloc(1);
    }
    (*result)->Set(end, element);
    return result;
}

template<typename T>
AutoPtr<ArrayOf<T> > ArrayUtils::RemoveElement(
    /* [in] */ ArrayOf<T>* array,
    /* [in] */ T element)
{
    if (array != NULL) {
        Int32 length = array->GetLength();
        for (Int32 i = 0; i < length; i++) {
            if ((*array)[i] == element) {
                if (length == 1) {
                    return NULL;
                }
                AutoPtr<ArrayOf<T> > result = ArrayOf<T>::Alloc(length - 1);
                if (i > 0) {
                    result->Copy(array, i);
                }
                if (i < (length - 1)) {
                    result->Copy(i, array, i + 1, length - i -1);
                }
                return result;
            }
        }
    }
    return array;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_ARRAYUTILS_H__
