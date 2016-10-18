
#ifndef __ELASTOS_DROID_UTILITY_CRANGE_H__
#define __ELASTOS_DROID_UTILITY_CRANGE_H__

#include "_Elastos_Droid_Utility_CRange.h"
#include "elastos/droid/utility/Range.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CRange)
    , public Object
    , public IRange
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CRange();

    CARAPI constructor(
        /* [in] */ const InterfaceID& typeId,
        /* [in] */ IInterface* lower,
        /* [in] */ IInterface* upper);

    CARAPI GetTypeId(
        /* [out] */ InterfaceID* typeId);

    /**
     * Get the lower endpoint.
     *
     * @return a non-{@code null} {@code T} reference
     */
    CARAPI GetLower(
        /* [out] */ IInterface** result);

    /**
     * Get the upper endpoint.
     *
     * @return a non-{@code null} {@code T} reference
     */
    CARAPI GetUpper(
        /* [out] */ IInterface** result);

    /**
     * Checks if the {@code value} is within the bounds of this range.
     *
     * <p>A value is considered to be within this range if it's {@code >=}
     * the lower endpoint <i>and</i> {@code <=} the upper endpoint (using the {@link Comparable}
     * interface.)</p>
     *
     * @param value a non-{@code null} {@code T} reference
     * @return {@code true} if the value is within this inclusive range, {@code false} otherwise
     *
     * @throws NullPointerException if {@code value} was {@code null}
     */
    CARAPI Contains(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result);

    /**
     * Checks if another {@code range} is within the bounds of this range.
     *
     * <p>A range is considered to be within this range if both of its endpoints
     * are within this range.</p>
     *
     * @param range a non-{@code null} {@code T} reference
     * @return {@code true} if the range is within this inclusive range, {@code false} otherwise
     *
     * @throws NullPointerException if {@code range} was {@code null}
     */
    CARAPI Contains(
        /* [in] */ IRange* range,
        /* [out] */ Boolean* result);

    /**
     * Clamps {@code value} to this range.
     *
     * <p>If the value is within this range, it is returned.  Otherwise, if it
     * is {@code <} than the lower endpoint, the lower endpoint is returned,
     * else the upper endpoint is returned. Comparisons are performed using the
     * {@link Comparable} interface.</p>
     *
     * @param value a non-{@code null} {@code T} reference
     * @return {@code value} clamped to this range.
     */
    CARAPI Clamp(
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** result);

    /**
     * Returns the intersection of this range and another {@code range}.
     * <p>
     * E.g. if a {@code <} b {@code <} c {@code <} d, the
     * intersection of [a, c] and [b, d] ranges is [b, c].
     * As the endpoints are object references, there is no guarantee
     * which specific endpoint reference is used from the input ranges:</p>
     * <p>
     * E.g. if a {@code ==} a' {@code <} b {@code <} c, the
     * intersection of [a, b] and [a', c] ranges could be either
     * [a, b] or ['a, b], where [a, b] could be either the exact
     * input range, or a newly created range with the same endpoints.</p>
     *
     * @param range a non-{@code null} {@code Range<T>} reference
     * @return the intersection of this range and the other range.
     *
     * @throws NullPointerException if {@code range} was {@code null}
     * @throws IllegalArgumentException if the ranges are disjoint.
     */
    CARAPI Intersect(
        /* [in] */ IRange* range,
        /* [out] */ IRange** result);

    /**
     * Returns the intersection of this range and the inclusive range
     * specified by {@code [lower, upper]}.
     * <p>
     * See {@link #intersect(Range)} for more details.</p>
     *
     * @param lower a non-{@code null} {@code T} reference
     * @param upper a non-{@code null} {@code T} reference
     * @return the intersection of this range and the other range
     *
     * @throws NullPointerException if {@code lower} or {@code upper} was {@code null}
     * @throws IllegalArgumentException if the ranges are disjoint.
     */
    CARAPI Intersect(
        /* [in] */ IInterface* lower,
        /* [in] */ IInterface* upper,
        /* [out] */ IRange** result);

    /**
     * Returns the smallest range that includes this range and
     * another {@code range}.
     * <p>
     * E.g. if a {@code <} b {@code <} c {@code <} d, the
     * extension of [a, c] and [b, d] ranges is [a, d].
     * As the endpoints are object references, there is no guarantee
     * which specific endpoint reference is used from the input ranges:</p>
     * <p>
     * E.g. if a {@code ==} a' {@code <} b {@code <} c, the
     * extension of [a, b] and [a', c] ranges could be either
     * [a, c] or ['a, c], where ['a, c] could be either the exact
     * input range, or a newly created range with the same endpoints.</p>
     *
     * @param range a non-{@code null} {@code Range<T>} reference
     * @return the extension of this range and the other range.
     *
     * @throws NullPointerException if {@code range} was {@code null}
     */
    CARAPI Extend(
        /* [in] */ IRange* range,
        /* [out] */ IRange** result);

    /**
     * Returns the smallest range that includes this range and
     * the inclusive range specified by {@code [lower, upper]}.
     * <p>
     * See {@link #extend(Range)} for more details.</p>
     *
     * @param lower a non-{@code null} {@code T} reference
     * @param upper a non-{@code null} {@code T} reference
     * @return the extension of this range and the other range.
     *
     * @throws NullPointerException if {@code lower} or {@code
     *                              upper} was {@code null}
     */
    CARAPI Extend(
        /* [in] */ IInterface* lower,
        /* [in] */ IInterface* upper,
        /* [out] */ IRange** result);

    /**
     * Returns the smallest range that includes this range and
     * the {@code value}.
     * <p>
     * See {@link #extend(Range)} for more details, as this method is
     * equivalent to {@code extend(Range.create(value, value))}.</p>
     *
     * @param value a non-{@code null} {@code T} reference
     * @return the extension of this range and the value.
     *
     * @throws NullPointerException if {@code value} was {@code null}
     */
    CARAPI Extend(
        /* [in] */ IInterface* value,
        /* [out] */ IRange** result);

    /**
     * Create a new immutable range, with the argument types inferred.
     *
     * <p>
     * The endpoints are {@code [lower, upper]}; that
     * is the range is bounded. {@code lower} must be {@link Comparable#compareTo lesser or equal}
     * to {@code upper}.
     * </p>
     *
     * @param lower The lower endpoint (inclusive)
     * @param upper The upper endpoint (inclusive)
     *
     * @throws NullPointerException if {@code lower} or {@code upper} is {@code null}
     */
    static CARAPI Create(
        /* [in] */ const InterfaceID& typeId,
        /* [in] */ IInterface* lower,
        /* [in] */ IInterface* upper,
        /* [out] */ IRange** result);

private:
    AutoPtr<Range<IInterface> > mRange;
    InterfaceID mTypeId;
};

}
}
}
#endif // __ELASTOS_DROID_UTILITY_CRANGE_H__
