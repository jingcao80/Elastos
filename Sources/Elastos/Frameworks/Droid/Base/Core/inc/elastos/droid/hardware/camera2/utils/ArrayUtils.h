
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_UTILS_ARRAYUTILS_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_UTILS_ARRAYUTILS_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Core::Object;
using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class ArrayUtils
{
public:
    /** Return the index of {@code needle} in the {@code array}, or else {@code -1} */
    template<typename T>
    static CARAPI GetArrayIndex(
        /* [in] */ ArrayOf<T>* array,
        /* [in] */ T needle,
        /* [out] */ Int32* value);

    template<typename T>
    static CARAPI GetArrayIndex(
        /* [in] */ IArrayOf* array,
        /* [in] */ T needle,
        /* [out] */ Int32* value);

    /**
     * Create an {@code int[]} from the {@code List<>} by using {@code convertFrom} and
     * {@code convertTo} as a one-to-one map (via the index).
     *
     * <p>Strings not appearing in {@code convertFrom} are ignored (with a logged warning);
     * strings appearing in {@code convertFrom} but not {@code convertTo} are silently
     * dropped.</p>
     *
     * @param list Source list of strings
     * @param convertFrom Conversion list of strings
     * @param convertTo Conversion list of ints
     * @return An array of ints where the values correspond to the ones in {@code convertTo}
     *         or {@code null} if {@code list} was {@code null}
     */
    static CARAPI ConvertStringListToIntArray(
        /* [in] */ IList* list,
        /* [in] */ ArrayOf<String>* convertFrom,
        /* [in] */ ArrayOf<Int32>* convertTo,
        /* [out, callee] */ ArrayOf<Int32>** outarr);

    static CARAPI ConvertStringArrayToIntArray(
        /* [in] */ ArrayOf<String>* list,
        /* [in] */ ArrayOf<String>* convertFrom,
        /* [in] */ ArrayOf<Int32>* convertTo,
        /* [out, callee] */ ArrayOf<Int32>** outarr);

    /**
     * Create an {@code List<Integer>} from the {@code List<>} by using {@code convertFrom} and
     * {@code convertTo} as a one-to-one map (via the index).
     *
     * <p>Strings not appearing in {@code convertFrom} are ignored (with a logged warning);
     * strings appearing in {@code convertFrom} but not {@code convertTo} are silently
     * dropped.</p>
     *
     * @param list Source list of strings
     * @param convertFrom Conversion list of strings
     * @param convertTo Conversion list of ints
     * @return A list of ints where the values correspond to the ones in {@code convertTo}
     *         or {@code null} if {@code list} was {@code null}
     */
    static CARAPI ConvertStringListToIntList(
        /* [in] */ IList* list,
        /* [in] */ ArrayOf<String>* convertFrom,
        /* [in] */ ArrayOf<Int32>* convertTo,
        /* [out] */ IList** outlist);

    static CARAPI ConvertStringArrayToIntList(
        /* [in] */ ArrayOf<String>* list,
        /* [in] */ ArrayOf<String>* convertFrom,
        /* [in] */ ArrayOf<Int32>* convertTo,
        /* [out] */ IList** outlist);

    /**
     * Convert the list of integers in {@code list} to an {@code int} array.
     *
     * <p>Every element in {@code list} must be non-{@code null}.</p>
     *
     * @param list a list of non-{@code null} integers
     *
     * @return a new int array containing all the elements from {@code list}
     *
     * @throws NullPointerException if any of the elements in {@code list} were {@code null}
     */
    static CARAPI ToIntArray(
        /* [in] */ IList* list,
        /* [out, callee] */ ArrayOf<Int32>** outarr);

    /**
     * Returns true if the given {@code array} contains the given element.
     *
     * @param array {@code array} to check for {@code elem}
     * @param elem {@code elem} to test for
     * @return {@code true} if the given element is contained
     */
    template<typename T>
    static CARAPI Contains(
        /* [in] */ ArrayOf<T>* array,
        /* [in] */ T elem,
        /* [out] */ Boolean* value);

    template<typename T>
    static CARAPI Contains(
        /* [in] */ IArrayOf* array,
        /* [in] */ T elem,
        /* [out] */ Boolean* value);

    /**
     * Return {@code true} if the {@code list} is only a single element equal to
     * {@code single}.
     */
    static CARAPI ArrayElementsEqualTo(
        /* [in] */ ArrayOf<String>* array,
        /* [in] */ const String& single,
        /* [out] */ Boolean* value);

private:
    ArrayUtils() {}

    ~ArrayUtils() {}

private:
    static const String TAG;
    static const Boolean VERBOSE;
};

template<typename T>
ECode ArrayUtils::GetArrayIndex(
    /* [in] */ ArrayOf<T>* array,
    /* [in] */ T needle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = -1;

    if (array == NULL) {
        return NOERROR;
    }

    for (Int32 i = 0; i < array->GetLength(); i++) {
        if ((*array)[i] == needle) {
            *value = i;
            return NOERROR;
        };
    }

    return NOERROR;
}

static ECode GetArrayIndex(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ IInterface* needle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = -1;

    if (array == NULL) {
        return NOERROR;
    }

    for (Int32 i = 0; i < array->GetLength(); i++) {
        if (Object::Equals((*array)[i], needle)) {
            *value = i;
            return NOERROR;
        };
    }

    return NOERROR;
}

template<typename T>
ECode ArrayUtils::GetArrayIndex(
    /* [in] */ IArrayOf* array,
    /* [in] */ T needle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = -1;

    if (array == NULL) {
        return NOERROR;
    }

    AutoPtr<IInterface> needleObj = (IInterface*)CoreUtils::Convert(needle);
    Int32 size;
    array->GetLength(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        if (Object::Equals(obj, needleObj)) {
            *value = i;
            return NOERROR;
        };
    }

    return NOERROR;
}

static ECode GetArrayIndex(
    /* [in] */ IArrayOf* array,
    /* [in] */ IInterface* needle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = -1;

    if (array == NULL) {
        return NOERROR;
    }

    Int32 size;
    array->GetLength(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        if (Object::Equals(obj, needle)) {
            *value = i;
            return NOERROR;
        };
    }

    return NOERROR;
}

template<typename T>
ECode ArrayUtils::Contains(
    /* [in] */ ArrayOf<T>* array,
    /* [in] */ T elem,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 value;
    GetArrayIndex(array, elem, &value);
    *result = (value != -1);
    return NOERROR;
}

template<typename T>
ECode ArrayUtils::Contains(
    /* [in] */ IArrayOf* array,
    /* [in] */ T elem,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 value;
    GetArrayIndex(array, elem, &value);
    *result = (value != -1);
    return NOERROR;
}


} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_UTILS_ARRAYUTILS_H__
