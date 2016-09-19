
#include "CEllipticCurve.h"
#include <elastos/utility/Arrays.h>

using Elastos::Core::IComparable;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CEllipticCurve)
CAR_INTERFACE_IMPL(CEllipticCurve, Object, IEllipticCurve)
ECode CEllipticCurve::GetA(
    /* [out] */ IBigInteger **a)
{
    VALIDATE_NOT_NULL(a)
    *a = mA;
    REFCOUNT_ADD(*a)
    return NOERROR;
}

ECode CEllipticCurve::GetB(
    /* [out] */ IBigInteger **b)
{
    VALIDATE_NOT_NULL(b)
    *b = mB;
    REFCOUNT_ADD(*b)
    return NOERROR;
}

ECode CEllipticCurve::GetField(
    /* [out] */ IECField **field)
{
    VALIDATE_NOT_NULL(field)
    *field = mField;
    REFCOUNT_ADD(*field)
    return NOERROR;
}

ECode CEllipticCurve::GetSeed(
    /* [out, callee] */ ArrayOf<Byte> **seed)
{
    VALIDATE_NOT_NULL(seed)
    if (!mSeed) {
        *seed = NULL;
        return NOERROR;
    }
    else {
        // return copy
        AutoPtr<ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(mSeed->GetLength());
        ret->Copy(0, mSeed, 0, ret->GetLength());
        *seed = ret;
        REFCOUNT_ADD(*seed)
        return NOERROR;
    }
}

ECode CEllipticCurve::Equals(
    /* [in] */ IInterface *other,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    if (TO_IINTERFACE(this) == other) {
        *result = TRUE;
        return NOERROR;
    }
    if (!IEllipticCurve::Probe(other)) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IEllipticCurve> otherEc = IEllipticCurve::Probe(other);
    Boolean equal1, equal2, equal3;
    IObject::Probe(mField.Get())->Equals(IObject::Probe(((CEllipticCurve*)otherEc.Get())->mField.Get()), &equal1);
    mA->Equals(((CEllipticCurve*)otherEc.Get())->mA.Get(), &equal2);
    mB->Equals(((CEllipticCurve*)otherEc.Get())->mB.Get(), &equal3);
    *result = equal1 && equal2 && equal3
        && Arrays::Equals(mSeed, ((CEllipticCurve*)otherEc.Get())->mSeed);
    return NOERROR;
}

ECode CEllipticCurve::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    // hash init is delayed
    if (mHash == 0) {
        Int32 hash0 = 11;
        hash0 = hash0 * 31 + (IObject::Probe(mField.Get())->GetHashCode(hashCode), *hashCode);
        hash0 = hash0 * 31 + (IObject::Probe(mA.Get())->GetHashCode(hashCode), *hashCode);
        hash0 = hash0 * 31 + (IObject::Probe(mB.Get())->GetHashCode(hashCode), *hashCode);
        if (mSeed != NULL) {
            for (Int32 i = 0; i < mSeed->GetLength(); i++) {
                hash0 = hash0 * 31 + (*mSeed)[i];
            }
        }
        else {
            hash0 = hash0 * 31;
        }
        mHash = hash0;
    }
    *hashCode = mHash;
    return NOERROR;
}

ECode CEllipticCurve::constructor(
    /* [in] */ IECField *field,
    /* [in] */ IBigInteger *a,
    /* [in] */ IBigInteger *b,
    /* [in] */ ArrayOf<Byte> *seed)
{
    mField = field;
    if (!mField) {
        return E_NULL_POINTER_EXCEPTION;
    }

    mA = a;
    if (!mA) {
        return E_NULL_POINTER_EXCEPTION;
    }

    mB = b;
    if (!mB) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // make defensive copy
    if (!seed) {
        mSeed = NULL;
    }
    else {
        mSeed = ArrayOf<Byte>::Alloc(seed->GetLength());
        mSeed->Copy(0, seed, 0, mSeed->GetLength());
    }
    // check parameters for ECFieldFp and ECFieldF2m.
    // Check invariant: a and b must be in the field.
    // Check conditions for custom ECField are not specified.
    if (IECFieldFp::Probe(mField.Get())) {
        AutoPtr<IBigInteger> p;
        IECFieldFp::Probe(mField.Get())->GetP((IBigInteger**)&p);
        Int32 num, equal;
        mA->GetSignum(&num);
        IComparable::Probe(mA)->CompareTo(p.Get(), &equal);
        if (num < 0 || equal >= 0) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        mB->GetSignum(&num);
        IComparable::Probe(mB)->CompareTo(p.Get(), &equal);
        if (num < 0 || equal >= 0) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    else if (IECFieldF2m::Probe(mField.Get())) {
        Int32 fieldSizeInBits;
        mField->GetFieldSize(&fieldSizeInBits);
        Int32 bl;
        mA->BitLength(&bl);
        if (bl > fieldSizeInBits) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        mB->BitLength(&bl);
        if (bl > fieldSizeInBits) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CEllipticCurve::constructor(
    /* [in] */ IECField *field,
    /* [in] */ IBigInteger *a,
    /* [in] */ IBigInteger *b)
{
    return constructor(field, a, b, NULL);
}

}
}
}

