
#include "elastos/droid/utility/CRange.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(CRange, Object, IRange)

CAR_OBJECT_IMPL(CRange)

CRange::CRange()
{
}

ECode CRange::constructor(
    /* [in] */ const InterfaceID& typeId,
    /* [in] */ IInterface* lower,
    /* [in] */ IInterface* upper)
{
    mTypeId = typeId;
    mRange = new Range<IInterface>(lower, upper);
    return NOERROR;
}

ECode CRange::GetTypeId(
    /* [out] */ InterfaceID* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTypeId;
    return NOERROR;
}

ECode CRange::GetLower(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> temp = mRange->GetLower();
    *result = temp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CRange::GetUpper(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> temp = mRange->GetUpper();
    *result = temp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CRange::Contains(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRange->Contains(value);
    return NOERROR;
}

ECode CRange::Contains(
    /* [in] */ IRange* range,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRange->Contains(range);
    return NOERROR;
}

ECode CRange::Clamp(
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 val;
    IComparable::Probe(value)->CompareTo(mRange->mLower, &val);
    if (val < 0) {
        *result = mRange->mLower;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    IComparable::Probe(value)->CompareTo(mRange->mUpper, &val);
    if (val > 0) {
        *result = mRange->mUpper;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    *result = value;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CRange::Intersect(
    /* [in] */ IRange* rangeObj,
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    VALIDATE_NOT_NULL(rangeObj)
    CRange* other = (CRange*)rangeObj;
    Range<IInterface>* range = other->mRange;

    Int32 cmpLower, cmpUpper;
    IComparable::Probe(range->mLower)->CompareTo(mRange->mLower, &cmpLower);
    IComparable::Probe(range->mUpper)->CompareTo(mRange->mUpper, &cmpUpper);

    if (cmpLower <= 0 && cmpUpper >= 0) {
        // range includes this
        *result = this;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (cmpLower >= 0 && cmpUpper <= 0) {
        // this inludes range
        *result = rangeObj;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        AutoPtr<IRange> newRangeObj;
        CRange::New(mTypeId, cmpLower <= 0 ? mRange->mLower : range->mLower,
                cmpUpper >= 0 ? mRange->mUpper : range->mUpper,
                (IRange**)&newRangeObj);
        *result = newRangeObj;
        REFCOUNT_ADD(*result)
    }

    return NOERROR;
}

ECode CRange::Intersect(
    /* [in] */ IInterface* lower,
    /* [in] */ IInterface* upper,
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 cmpLower, cmpUpper;
    IComparable::Probe(lower)->CompareTo(mRange->mLower, &cmpLower);
    IComparable::Probe(upper)->CompareTo(mRange->mUpper, &cmpUpper);

    if (cmpLower <= 0 && cmpUpper >= 0) {
        // [lower, upper] includes this
        *result = this;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        AutoPtr<IRange> newRangeObj;
        CRange::New(mTypeId, cmpLower <= 0 ? mRange->mLower.Get() : lower,
                cmpUpper >= 0 ? mRange->mUpper.Get() : upper,
                (IRange**)&newRangeObj);
        *result = newRangeObj;
        REFCOUNT_ADD(*result)
    }

    return NOERROR;
}

ECode CRange::Extend(
    /* [in] */ IRange* rangeObj,
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)

    VALIDATE_NOT_NULL(rangeObj)
    CRange* other = (CRange*)rangeObj;
    Range<IInterface>* range = other->mRange;

    Int32 cmpLower, cmpUpper;
    IComparable::Probe(range->mLower)->CompareTo(mRange->mLower, &cmpLower);
    IComparable::Probe(range->mUpper)->CompareTo(mRange->mUpper, &cmpUpper);

    if (cmpLower <= 0 && cmpUpper >= 0) {
        // other includes this
        *result = rangeObj;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (cmpLower >= 0 && cmpUpper <= 0) {
        // this inludes other
        *result = this;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        AutoPtr<IRange> newRangeObj;
        CRange::New(mTypeId, cmpLower >= 0 ? mRange->mLower : range->mLower,
                cmpUpper <= 0 ? mRange->mUpper : range->mUpper,
                (IRange**)&newRangeObj);
        *result = newRangeObj;
        REFCOUNT_ADD(*result)
    }

    return NOERROR;
}

ECode CRange::Extend(
    /* [in] */ IInterface* lower,
    /* [in] */ IInterface* upper,
    /* [out] */ IRange** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 cmpLower, cmpUpper;
    IComparable::Probe(lower)->CompareTo(mRange->mLower, &cmpLower);
    IComparable::Probe(upper)->CompareTo(mRange->mUpper, &cmpUpper);

       if (cmpLower >= 0 && cmpUpper <= 0) {
        // this inludes other
        *result = this;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        AutoPtr<IRange> newRangeObj;
        CRange::New(mTypeId, cmpLower >= 0 ? mRange->mLower.Get() : lower,
                cmpUpper <= 0 ? mRange->mUpper.Get() : upper,
                (IRange**)&newRangeObj);
        *result = newRangeObj;
        REFCOUNT_ADD(*result)
    }

    return NOERROR;
}

ECode CRange::Extend(
    /* [in] */ IInterface* value,
    /* [out] */ IRange** result)
{
    return Extend(value, value, result);
}

ECode CRange::Create(
    /* [in] */ const InterfaceID& typeId,
    /* [in] */ IInterface *lower,
    /* [in] */ IInterface *upper,
    /* [out] */ IRange **result)
{
    return CRange::New(typeId, lower, upper, result);
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
