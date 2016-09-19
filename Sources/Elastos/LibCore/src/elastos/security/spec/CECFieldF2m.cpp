
#include "CECFieldF2m.h"
#include "elastos/math/CBigIntegerHelper.h"
#include <elastos/utility/Arrays.h>

using Elastos::Math::IBigIntegerHelper;
using Elastos::Math::CBigIntegerHelper;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CECFieldF2m)
CAR_INTERFACE_IMPL_2(CECFieldF2m, Object, IECFieldF2m, IECField)
ECode CECFieldF2m::GetFieldSize(
    /* [out] */ Int32 *size)
{
    VALIDATE_NOT_NULL(size)
    *size = mM;
    return NOERROR;
}

ECode CECFieldF2m::GetM(
    /* [out] */ Int32* m)
{
    VALIDATE_NOT_NULL(m)
    *m = mM;
    return NOERROR;
}

ECode CECFieldF2m::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    // object equals to itself
    if (TO_IINTERFACE(this) == obj) {
        *result = TRUE;
        return NOERROR;
    }
    if (IECFieldF2m::Probe(obj)) {
        AutoPtr<IECFieldF2m> o = IECFieldF2m::Probe(obj);
        // check m
        if (mM == ((CECFieldF2m*)o.Get())->mM) {
            // check rp
            if (mRp == NULL) {
                if (((CECFieldF2m*)o.Get())->mRp == NULL) {
                    // fields both with normal basis
                    *result = TRUE;
                    return NOERROR;
                }
            }
            else {
                // at least this field with polynomial basis
                // check that rp match
                // return this.rp.equals(o.rp);
                *result = Arrays::Equals(mKs, ((CECFieldF2m*)o.Get())->mKs);
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}


ECode CECFieldF2m::GetMidTermsOfReductionPolynomial(
    /* [out, callee] */ ArrayOf<Int32> **terms)
{
    VALIDATE_NOT_NULL(terms)
    // Defensively copies private array
    // to prevent subsequent modification
    // was: return ks == null ? null : (int[])ks.clone();
    if (mKs == NULL) {
        *terms = NULL;
    }
    else {
        AutoPtr<ArrayOf<Int32> > ret = ArrayOf<Int32>::Alloc(mKs->GetLength());
        ret->Copy(0, mKs, 0, ret->GetLength());
        *terms = ret;
        REFCOUNT_ADD(*terms)
    }
    return NOERROR;
}

ECode CECFieldF2m::GetReductionPolynomial(
    /* [out] */ IBigInteger **rp)
{
    VALIDATE_NOT_NULL(rp)
    *rp = mRp;
    REFCOUNT_ADD(*rp)
    return NOERROR;
}

ECode CECFieldF2m::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = (mRp == NULL) ? mM : (mM + (IObject::Probe(mRp)->GetHashCode(hashCode), *hashCode));
    return NOERROR;
}

ECode CECFieldF2m::constructor(
    /* [in] */ Int32 m)
{
    mM = m;
    if (mM <= 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CECFieldF2m::constructor(
    /* [in] */ Int32 m,
    /* [in] */ IBigInteger *rp)
{
    mM = m;
    if (mM <= 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mRp = rp;
    if (mRp == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // the leftmost bit must be (m+1)-th one,
    // set bits count must be 3 or 5,
    // bits 0 and m must be set
    Int32 rp_bc;
    mRp->BitCount(&rp_bc);
    Int32 bitLength;
    mRp->BitLength(&bitLength);
    Boolean ret;
    if (bitLength != m + 1 ||
        (rp_bc != TPB_LEN && rp_bc != PPB_LEN) ||
        ((mRp->TestBit(0, &ret), !ret) || (mRp->TestBit(m, &ret), !ret))) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // setup ks using rp:
    // allocate for mid terms only
    mKs = ArrayOf<Int32>::Alloc(rp_bc-2);
    // find midterm orders and set ks accordingly
    AutoPtr<IBigInteger> rpTmp, temp;
    rp->ClearBit(0, (IBigInteger**)&rpTmp);
    for (Int32 i = mKs->GetLength() - 1; i >= 0; i--) {
        rpTmp->GetLowestSetBit(&(*mKs)[i]);
        temp = NULL;
        rpTmp->ClearBit((*mKs)[i], (IBigInteger**)&temp);
        rpTmp = temp;
    }
    return NOERROR;
}

ECode CECFieldF2m::constructor(
    /* [in] */ Int32 m,
    /* [in] */ ArrayOf<Int32> *ks)
{
    mM = m;
    if (mM <= 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Defensively copies array parameter
    // to prevent subsequent modification.
    // NPE as specified if ks is null
    mKs = ArrayOf<Int32>::Alloc(ks->GetLength());
    mKs->Copy(0, ks, 0, mKs->GetLength());

    // no need to check for null already
    if (mKs->GetLength() != TPB_MID_LEN && mKs->GetLength() != PPB_MID_LEN) {
        // must be either trinomial or pentanomial basis
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // trinomial basis:
    // check that m > k >= 1, where k is ks[0]
    // pentanomial basis:
    // check that m > k3 > k2 > k1 >= 1
    // and kx in descending order, where
    // k3 is ks[0], k2 is ks[1], k1 is ks[2]
    Boolean checkFailed = FALSE;
    Int32 prev = mM;
    for (Int32 i = 0; i < mKs->GetLength(); i++) {
        if ((*mKs)[i] < prev) {
            prev = (*mKs)[i];
            continue;
        }
        checkFailed = TRUE;
        break;
    }
    if (checkFailed || prev < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Setup rp using ks:
    // bits 0 and m always set
    AutoPtr<IBigInteger> rpTmp, temp;
    AutoPtr<IBigIntegerHelper> helper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&helper);
    helper->GetONE((IBigInteger**)&rpTmp);
    rpTmp->SetBit(mM, (IBigInteger**)&temp);
    rpTmp = temp;
    // set remaining bits according to ks
    for (Int32 i = 0; i < mKs->GetLength(); i++) {
        temp = NULL;
        rpTmp->SetBit((*mKs)[i], (IBigInteger**)&temp);
        rpTmp = temp;
    }
    mRp = rpTmp;
    return NOERROR;
}

}
}
}

