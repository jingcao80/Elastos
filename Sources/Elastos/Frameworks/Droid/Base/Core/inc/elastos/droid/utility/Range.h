
#ifndef __ELASTOS_DROID_UTILITY_RANGE_H__
#define __ELASTOS_DROID_UTILITY_RANGE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::IComparable;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * Immutable class for describing the range of two numeric values.
 * <p>
 * A range (or "interval") defines the inclusive boundaries around a contiguous span of
 * values of some {@link Comparable} type; for example,
 * "integers from 1 to 100 inclusive."
 * </p>
 * <p>
 * All ranges are bounded, and the left side of the range is always {@code >=}
 * the right side of the range.
 * </p>
 *
 * <p>Although the implementation itself is immutable, there is no restriction that objects
 * stored must also be immutable. If mutable objects are stored here, then the range
 * effectively becomes mutable. </p>
 */
template<typename T>
class Range
    : public Object
{
public:
    friend class CRange;

    /**
     * Create a new immutable range.
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
     Range(
        /* [in] */ T* lower,
        /* [in] */ T* upper)
    {
        assert(lower != NULL && "lower must not be null");
        assert(upper != NULL && "upper must not be null");

        assert(IComparable::Probe(lower) != NULL);
        assert(IComparable::Probe(upper) != NULL);

        Int32 result;
        IComparable::Probe(lower)->CompareTo(TO_IINTERFACE(upper), &result);
        if (result > 0) {
            assert(0 && "lower must be less than or equal to upper");
        }

        mLower = lower;
        mUpper = upper;
    }

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
    static AutoPtr<Range<T> >  Create(
        /* [in] */ T* lower,
        /* [in] */ T* upper)
    {
        AutoPtr<Range<T> > range = new Range(lower, upper);
        return range;
    }

    /**
     * Get the lower endpoint.
     *
     * @return a non-{@code null} {@code T} reference
     */
    AutoPtr<T> GetLower()
    {
        return mLower;
    }

    /**
     * Get the upper endpoint.
     *
     * @return a non-{@code null} {@code T} reference
     */
    AutoPtr<T> GetUpper()
    {
        return mUpper;
    }

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
    Boolean Contains(
        /* [in] */ T* value)
    {
        assert(value != NULL && "upper must not be null");
        assert(IComparable::Probe(value) != NULL);

        Int32 lr, ur;
        IComparable::Probe(value)->CompareTo(mLower, &lr);
        IComparable::Probe(value)->CompareTo(mUpper, &ur);
        return lr >= 0 && ur <= 0;
    }

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
    Boolean Contains(
        /* [in] */ Range<T>* range)
    {
        assert(range != NULL && "upper must not be null");

        Int32 lr, ur;
        IComparable::Probe(range->mLower)->CompareTo(mLower, &lr);
        IComparable::Probe(range->mUpper)->CompareTo(mUpper, &ur);
        return lr >= 0 && ur <= 0;
    }

    /**
     * Compare two ranges for equality.
     *
     * <p>A range is considered equal if and only if both the lower and upper endpoints
     * are also equal.</p>
     *
     * @return {@code true} if the ranges are equal, {@code false} otherwise
     */
    Boolean Equals(
        /* [in] */ Range<T>* other)
    {
        Boolean result;
        Equals(TO_IINTERFACE(other), &result);
        return result;
    }

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result)
    {
        VALIDATE_NOT_NULL(result)
        *result = FALSE;

        if (obj == NULL) {
            return NOERROR;
        }
        else if (TO_IINTERFACE(this) == obj) {
            *result = TRUE;
            return NOERROR;
        }

        Range* other = (Range*)IObject::Probe(obj);
        *result = Object::Equals(mLower, other->mLower)
            && Object::Equals(mUpper, other->mUpper);
        return NOERROR;
    }

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
    AutoPtr<T> Clamp(
        /* [in] */ T* value)
    {
        assert(value != NULL && "value must not be null");
        assert(IComparable::Probe(value) != NULL);

        Boolean result;
        IComparable::Probe(value)->CompareTo(mLower, &result);
        if (result < 0) {
            return mLower;
        }

        IComparable::Probe(value)->CompareTo(mUpper, &result);
        if (result > 0) {
            return mUpper;
        }

        return value;
    }

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
    AutoPtr<Range<T> > Intersect(
        /* [in] */ Range<T>* range)
    {
        assert(range != NULL && "range must not be null");

        Int32 cmpLower, cmpUpper;
        IComparable::Probe(range->mLower)->CompareTo(mLower, &cmpLower);
        IComparable::Probe(range->mUpper)->CompareTo(mUpper, &cmpUpper);

        if (cmpLower <= 0 && cmpUpper >= 0) {
            // range includes this
            return this;
        }
        else if (cmpLower >= 0 && cmpUpper <= 0) {
            // this inludes range
            return range;
        }
        else {
            return Range<T>::Create(
                    cmpLower <= 0 ? mLower : range->mLower,
                    cmpUpper >= 0 ? mUpper : range->mUpper);
        }
    }

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
    AutoPtr<Range<T> > Intersect(
        /* [in] */ T* lower,
        /* [in] */ T* upper)
    {
        assert(lower != NULL && "lower must not be null");
        assert(upper != NULL && "upper must not be null");

        assert(IComparable::Probe(lower) != NULL);
        assert(IComparable::Probe(upper) != NULL);

        Int32 cmpLower, cmpUpper;
        IComparable::Probe(lower)->CompareTo(mLower, &cmpLower);
        IComparable::Probe(upper)->CompareTo(mUpper, &cmpUpper);

        if (cmpLower <= 0 && cmpUpper >= 0) {
            // [lower, upper] includes this
            return this;
        }
        else {
            return Range<T>::Create(
                    cmpLower <= 0 ? mLower : lower,
                    cmpUpper >= 0 ? mUpper : upper);
        }
    }

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
    AutoPtr<Range<T> > Extend(
        /* [in] */ Range<T>* range)
    {
        assert(range != NULL && "range must not be null");

        Int32 cmpLower, cmpUpper;
        IComparable::Probe(range->mLower)->CompareTo(mLower, &cmpLower);
        IComparable::Probe(range->mUpper)->CompareTo(mUpper, &cmpUpper);

        if (cmpLower <= 0 && cmpUpper >= 0) {
            // other includes this
            return range;
        }
        else if (cmpLower >= 0 && cmpUpper <= 0) {
            // this inludes other
            return this;
        }
        else {
            return Range<T>::Create(
                    cmpLower >= 0 ? mLower : range->mLower,
                    cmpUpper <= 0 ? mUpper : range->mUpper);
        }
    }

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
    AutoPtr<Range<T> > Extend(
        /* [in] */ T* lower,
        /* [in] */ T* upper)
    {
        assert(lower != NULL && "lower must not be null");
        assert(upper != NULL && "upper must not be null");

        assert(IComparable::Probe(lower) != NULL);
        assert(IComparable::Probe(upper) != NULL);

        Int32 cmpLower, cmpUpper;
        IComparable::Probe(lower)->CompareTo(mLower, &cmpLower);
        IComparable::Probe(upper)->CompareTo(mUpper, &cmpUpper);

           if (cmpLower >= 0 && cmpUpper <= 0) {
            // this inludes other
            return this;
        } else {
            return Range<T>::Create(
                    cmpLower >= 0 ? mLower : lower,
                    cmpUpper <= 0 ? mUpper : upper);
        }
    }

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
    AutoPtr<Range<T> > Extend(
        /* [in] */ T* value)
    {
        return Extend(value, value);
    }

    /**
     * Return the range as a string representation {@code "[lower, upper]"}.
     *
     * @return string representation of the range
     */
    CARAPI ToString(
        /* [out] */ String* result)
    {
        VALIDATE_NOT_NULL(result)
        StringBuilder sb("[");
        sb += Object::ToString(mLower);
        sb += ", ";
        sb += Object::ToString(mUpper);
        sb += "]";
        *result = sb.ToString();
        return NOERROR;
    }

    CARAPI GetHashCode(
        /* [out] */ Int32* hash)
    {
        VALIDATE_NOT_NULL(hash)
        int h = Object::GetHashCode(mLower);
        int x = Object::GetHashCode(mUpper);
        h = ((h << 5) - h) ^ x; // (h * 31) XOR x
        *hash = h;
        return NOERROR;
    }

private:
    AutoPtr<T> mLower;
    AutoPtr<T> mUpper;
};

} // namespace Utility
} // namepsace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_UTILITY_RANGE_H__
