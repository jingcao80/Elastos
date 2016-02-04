#ifndef _ELASTOS_DROID_MEDIA_UTILS_H__
#define _ELASTOS_DROID_MEDIA_UTILS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/Range.h"
#include "elastos/droid/utility/CRational.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::CRational;
using Elastos::Droid::Utility::ISize;
using Elastos::Droid::Utility::IRational;
using Elastos::Droid::Utility::IRange;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::Range;
using Elastos::Core::IComparator;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;

// package private
namespace Elastos{
namespace Droid {
namespace Media {

class Utils
{
private:
    class InterfaceCompare
        : public Object
        , public IComparator
    {
    public:
        InterfaceCompare();

        ~InterfaceCompare();

        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);
    };

public:
    /**
     * Sorts distinct (non-intersecting) range array in ascending order.
     * @throws java.lang.IllegalArgumentException if ranges are not distinct
     */
    static CARAPI_(void) SortDistinctRanges(
        /* [in] */ ArrayOf<IRange*>* ranges);

    /**
     * Returns the intersection of two sets of non-intersecting ranges
     * @param one a sorted set of non-intersecting ranges in ascending order
     * @param another another sorted set of non-intersecting ranges in ascending order
     * @return the intersection of the two sets, sorted in ascending order
     */
    static CARAPI_(AutoPtr<ArrayOf<IRange*> >) IntersectSortedDistinctRanges(
        /* [in] */ ArrayOf<IRange*>* one,
        /* [in] */ ArrayOf<IRange*>* another);

    /**
     * Returns the index of the range that contains a value in a sorted array of distinct ranges.
     * @param ranges a sorted array of non-intersecting ranges in ascending order
     * @param value the value to search for
     * @return if the value is in one of the ranges, it returns the index of that range.  Otherwise,
     * the return value is {@code (-1-index)} for the {@code index} of the range that is
     * immediately following {@code value}.
     */
    static CARAPI_(Int32) BinarySearchDistinctRanges(
        /* [in] */ ArrayOf<IRange*>* ranges,
        /* [in] */ IInterface* value);

    /**
     * Returns greatest common divisor
     */
    static CARAPI_(Int32) Gcd(
        /* [in] */ Int32 a,
        /* [in] */ Int32 b);

    /** Returns the equivalent factored range {@code newrange}, where for every
     * {@code e}: {@code newrange.contains(e)} implies that {@code range.contains(e * factor)},
     * and {@code !newrange.contains(e)} implies that {@code !range.contains(e * factor)}.
     */
    static CARAPI_(AutoPtr<Range<IInteger32> >) FactorRange(
        /* [in] */ Range<IInteger32>* range,
        /* [in] */ Int32 factor);

    /** Returns the equivalent factored range {@code newrange}, where for every
     * {@code e}: {@code newrange.contains(e)} implies that {@code range.contains(e * factor)},
     * and {@code !newrange.contains(e)} implies that {@code !range.contains(e * factor)}.
     */
    static CARAPI_(AutoPtr<Range<IInteger64> >) FactorRange(
        /* [in] */ Range<IInteger64>* range,
        /* [in] */ Int64 factor);

    static CARAPI_(AutoPtr< Range<CRational> >) ScaleRange(
        /* [in] */ Range<CRational>* range,
        /* [in] */ Int32 num,
        /* [in] */ Int32 den);

    static CARAPI_(AutoPtr<Range<IInteger32> >) AlignRange(
        /* [in] */ Range<IInteger32>* range,
        /* [in] */ Int32 align);

    static CARAPI_(Int32) DivUp(
        /* [in] */ Int32 num,
        /* [in] */ Int32 den);

    static CARAPI_(AutoPtr<Range<IInteger32> >) IntRangeFor(
        /* [in] */ Double v);

    static CARAPI_(AutoPtr<Range<IInteger64> >) Int64RangeFor(
        /* [in] */ Double v);

    static AutoPtr<ISize> ParseSize(
        /* [in] */ IInterface* o,
        /* [in] */ ISize* fallback);

    static Int32 ParseIntSafely(
        /* [in] */ IInterface* o,
        /* [in] */ Int32 fallback);

    static AutoPtr<Range<IInteger32> > ParseIntRange(
        /* [in] */ IInterface* o,
        /* [in] */ Range<IInteger32>* fallback);

    static AutoPtr<Range<IInteger64> > ParseInt64Range(
        /* [in] */ IInterface* o,
        /* [in] */ Range<IInteger64>* fallback);

    static AutoPtr<Range<IRational> > ParseRationalRange(
        /* [in] */ IInterface* o,
        /* [in] */ Range<IRational>* fallback);

    static AutoPtr<IPair> ParseSizeRange(
        /* [in] */ IInterface* o);

private:
    Utils();

    static CARAPI_(AutoPtr<CRational>) ScaleRatio(
        /* [in] */ CRational* ratio,
        /* [in] */ Int32 num,
        /* [in] */ Int32 den);

    static CARAPI_(Int64) DivUp(
        /* [in] */ Int64 num,
        /* [in] */ Int64 den);

    /**
     * Returns least common multiple
     */
    static CARAPI_(Int64) Lcm(
        /* [in] */ Int32 a,
        /* [in] */ Int32 b);

private:
    static const String TAG;
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_UTILS_H__