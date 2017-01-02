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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_PRECONDITIONS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_PRECONDITIONS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/Object.h>
#include <utils/CallStack.h>

using Elastos::Core::Object;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

/**
 * Fast and loose math routines.
 */
class Preconditions
{
public:
     /**
     * Ensures that an object reference passed as a parameter to the calling
     * method is not NULL.
     *
     * @param reference an object reference
     * @return the non-NULL reference that was validated
     * @throws NullPointerException if {@code reference} is NULL
     */
    template<typename T>
    static CARAPI CheckNotNull(
        /* [in] */ T* reference);

    static CARAPI CheckNotNull(
        /* [in] */ const String& reference);

    /**
     * Ensures that an object reference passed as a parameter to the calling
     * method is not NULL.
     *
     * @param reference an object reference
     * @param errorMessage the exception message to use if the check fails; will
     *     be converted to a string using {@link String#valueOf(Object)}
     * @return the non-NULL reference that was validated
     * @throws NullPointerException if {@code reference} is NULL
     */
    static CARAPI CheckNotNull(
        /* [in] */ IInterface* reference,
        /* [in] */ IObject* errorMessage);

    template<typename T>
    static CARAPI CheckNotNull(
        /* [in] */ T* reference,
        /* [in] */ const String& errorMessage);

    /**
     * Ensures the truth of an expression involving the state of the calling
     * instance, but not involving any parameters to the calling method.
     *
     * @param expression a boolean expression
     * @throws IllegalStateException if {@code expression} is false
     */
    static CARAPI CheckState(
        /* [in] */ Boolean expression);

    /**
     * Check the requested flags, throwing if any requested flags are outside
     * the allowed set.
     */
    static CARAPI CheckFlagsArgument(
        /* [in] */ Int32 requestedFlags,
        /* [in] */ Int32 allowedFlags);

    /**
     * Ensures that that the argument numeric value is non-negative.
     *
     * @param value a numeric Int32 value
     * @param errorMessage the exception message to use if the check fails
     * @return the validated numeric value
     * @throws IllegalArgumentException if {@code value} was negative
     */
    static CARAPI CheckArgumentNonnegative(
        /* [in] */ Int32 value,
        /* [in] */ const String& errorMessage);

    /**
     * Ensures that that the argument numeric value is non-negative.
     *
     * @param value a numeric Int64 value
     * @param errorMessage the exception message to use if the check fails
     * @return the validated numeric value
     * @throws IllegalArgumentException if {@code value} was negative
     */
    static CARAPI CheckArgumentNonnegative(
        /* [in] */ Int64 value,
        /* [in] */ const String& errorMessage);

    /**
     * Ensures that that the argument numeric value is positive.
     *
     * @param value a numeric Int32 value
     * @param errorMessage the exception message to use if the check fails
     * @return the validated numeric value
     * @throws IllegalArgumentException if {@code value} was not positive
     */
    static CARAPI CheckArgumentPositive(
        /* [in] */ Int32 value,
        /* [in] */ const String& errorMessage);

    /**
     * Ensures that the argument floating point value is a finite number.
     *
     * <p>A finite number is defined to be both representable (that is, not NaN) and
     * not infinite (that is neither positive or negative infinity).</p>
     *
     * @param value a floating point value
     * @param valueName the name of the argument to use if the check fails
     *
     * @return the validated floating point value
     *
     * @throws IllegalArgumentException if {@code value} was not finite
     */
    static CARAPI CheckArgumentFinite(
        /* [in] */ Float value,
        /* [in] */ const String& valueName);

    /**
     * Ensures that the argument floating point value is within the inclusive range.
     *
     * <p>While this can be used to range check against +/- infinity, note that all NaN numbers
     * will always be out of range.</p>
     *
     * @param value a floating point value
     * @param lower the lower endpoint of the inclusive range
     * @param upper the upper endpoint of the inclusive range
     * @param valueName the name of the argument to use if the check fails
     *
     * @return the validated floating point value
     *
     * @throws IllegalArgumentException if {@code value} was not within the range
     */
    static CARAPI CheckArgumentInRange(
        /* [in] */ Float value,
        /* [in] */ Float lower,
        /* [in] */ Float upper,
        /* [in] */ const String& valueName);

    /**
     * Ensures that the argument Int32 value is within the inclusive range.
     *
     * @param value a Int32 value
     * @param lower the lower endpoint of the inclusive range
     * @param upper the upper endpoint of the inclusive range
     * @param valueName the name of the argument to use if the check fails
     *
     * @return the validated Int32 value
     *
     * @throws IllegalArgumentException if {@code value} was not within the range
     */
    static CARAPI CheckArgumentInRange(
        /* [in] */ Int32 value,
        /* [in] */ Int32 lower,
        /* [in] */ Int32 upper,
        /* [in] */ const String& valueName);

    /**
     * Ensures that the array is not {@code NULL}, and none of its elements are {@code NULL}.
     *
     * @param value an array of boxed objects
     * @param valueName the name of the argument to use if the check fails
     *
     * @return the validated array
     *
     * @throws NullPointerException if the {@code value} or any of its elements were {@code NULL}
     */
    static CARAPI CheckArrayElementsNotNull(
        /* [in] */ ArrayOf<IInterface*>* value,
        /* [in] */ const String& valueName);

    /**
     * Ensures that the {@link Collection} is not {@code NULL}, and none of its elements are
     * {@code NULL}.
     *
     * @param value a {@link Collection} of boxed objects
     * @param valueName the name of the argument to use if the check fails
     *
     * @return the validated {@link Collection}
     *
     * @throws NullPointerException if the {@code value} or any of its elements were {@code NULL}
     */
    static CARAPI CheckCollectionElementsNotNull(
        /* [in] */ ICollection* value,
        /* [in] */ const String& valueName);

    /**
     * Ensures that the {@link Collection} is not {@code NULL}, and contains at least one element.
     *
     * @param value a {@link Collection} of boxed elements.
     * @param valueName the name of the argument to use if the check fails.

     * @return the validated {@link Collection}
     *
     * @throws NullPointerException if the {@code value} was {@code NULL}
     * @throws IllegalArgumentException if the {@code value} was empty
     */
    static CARAPI CheckCollectionNotEmpty(
        /* [in] */ ICollection* value,
        /* [in] */ const String& valueName);

    /**
     * Ensures that all elements in the argument floating point array are within the inclusive range
     *
     * <p>While this can be used to range check against +/- infinity, note that all NaN numbers
     * will always be out of range.</p>
     *
     * @param value a floating point array of values
     * @param lower the lower endpoint of the inclusive range
     * @param upper the upper endpoint of the inclusive range
     * @param valueName the name of the argument to use if the check fails
     *
     * @return the validated floating point value
     *
     * @throws IllegalArgumentException if any of the elements in {@code value} were out of range
     * @throws NullPointerException if the {@code value} was {@code NULL}
     */
    static CARAPI CheckArrayElementsInRange(
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Float lower,
        /* [in] */ Float upper,
        /* [in] */ const String& valueName);

private:
    Preconditions();

private:
    static const String TAG;
};

template<typename T>
ECode Preconditions::CheckNotNull(
    /* [in] */ T* reference)
{
    if (reference == NULL) {
        android::CallStack stack;
        stack.update();
        Logger::E(TAG, "%s, E_NULL_POINTER_EXCEPTION, backtrace:\n%s",
            __FUNCTION__, stack.toString("").string());
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

template<typename T>
ECode Preconditions::CheckNotNull(
    /* [in] */ T* reference,
    /* [in] */ const String& errorMessage)
{
    if (reference == NULL) {
        android::CallStack stack;
        stack.update();
        Logger::E(TAG, "%s, E_NULL_POINTER_EXCEPTION, msg:%s, backtrace:\n%s",
            __FUNCTION__, errorMessage.string(), stack.toString("").string());
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_PRECONDITIONS_H__
