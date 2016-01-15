#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CPRECONDITIONS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CPRECONDITIONS_H__

#include "_Elastos_Droid_Internal_Utility_CPreconditions.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

/**
 * Fast and loose math routines.
 */
CarClass(CPreconditions)
    , public Singleton
    , public IPreconditions
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Ensures that an object reference passed as a parameter to the calling
     * method is not NULL.
     *
     * @param reference an object reference
     * @return the non-NULL reference that was validated
     * @throws NullPointerException if {@code reference} is NULL
     */
    CARAPI CheckNotNull(
        /* [in] */ IInterface* reference);

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
    CARAPI CheckNotNull(
        /* [in] */ IInterface* reference,
        /* [in] */ IObject* errorMessage);

    CARAPI CheckNotNull(
        /* [in] */ IInterface* reference,
        /* [in] */ const String& errorMessage);

    /**
     * Ensures the truth of an expression involving the state of the calling
     * instance, but not involving any parameters to the calling method.
     *
     * @param expression a boolean expression
     * @throws IllegalStateException if {@code expression} is false
     */
    CARAPI CheckState(
        /* [in] */ Boolean expression);

    /**
     * Check the requested flags, throwing if any requested flags are outside
     * the allowed set.
     */
    CARAPI CheckFlagsArgument(
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
    CARAPI CheckArgumentNonnegative(
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
    CARAPI CheckArgumentNonnegative(
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
    CARAPI CheckArgumentPositive(
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
    CARAPI CheckArgumentFinite(
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
    CARAPI CheckArgumentInRange(
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
    CARAPI CheckArgumentInRange(
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
    CARAPI CheckArrayElementsNotNull(
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
    CARAPI CheckCollectionElementsNotNull(
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
    CARAPI CheckCollectionNotEmpty(
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
    CARAPI CheckArrayElementsInRange(
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Float lower,
        /* [in] */ Float upper,
        /* [in] */ const String& valueName);
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CPRECONDITIONS_H__
