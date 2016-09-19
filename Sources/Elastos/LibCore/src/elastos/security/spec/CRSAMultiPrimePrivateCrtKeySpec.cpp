
#include "CRSAMultiPrimePrivateCrtKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CRSAMultiPrimePrivateCrtKeySpec)
CAR_INTERFACE_IMPL(CRSAMultiPrimePrivateCrtKeySpec, RSAPrivateKeySpec, IRSAMultiPrimePrivateCrtKeySpec)
ECode CRSAMultiPrimePrivateCrtKeySpec::GetCrtCoefficient(
    /* [out] */ IBigInteger **crtCoefficient)
{
    VALIDATE_NOT_NULL(crtCoefficient)
    *crtCoefficient = mCrtCoefficient;
    REFCOUNT_ADD(*crtCoefficient)
    return NOERROR;
}

ECode CRSAMultiPrimePrivateCrtKeySpec::GetOtherPrimeInfo(
    /* [out, callee] */ ArrayOf<IRSAOtherPrimeInfo*> **primeInfo)
{
    VALIDATE_NOT_NULL(primeInfo)
    // Clone array (if not null) to prevent subsequent modification
    if (mOtherPrimeInfo == NULL) {
        *primeInfo = NULL;
        return NOERROR;
    }
    else {
        AutoPtr<ArrayOf<IRSAOtherPrimeInfo*> > ret =
            ArrayOf<IRSAOtherPrimeInfo*>::Alloc(mOtherPrimeInfo->GetLength());
        ret->Copy(0, mOtherPrimeInfo, 0, ret->GetLength());
        *primeInfo = ret;
        REFCOUNT_ADD(*primeInfo)
        return NOERROR;
    }
}

ECode CRSAMultiPrimePrivateCrtKeySpec::GetPrimeExponentP(
    /* [out] */ IBigInteger **p)
{
    VALIDATE_NOT_NULL(p)
    *p = mPrimeExponentP;
    REFCOUNT_ADD(*p)
    return NOERROR;
}

ECode CRSAMultiPrimePrivateCrtKeySpec::GetPrimeExponentQ(
    /* [out] */ IBigInteger **q)
{
    VALIDATE_NOT_NULL(q)
    *q = mPrimeExponentQ;
    REFCOUNT_ADD(*q)
    return NOERROR;
}

ECode CRSAMultiPrimePrivateCrtKeySpec::GetPrimeP(
    /* [out] */ IBigInteger **p)
{
    VALIDATE_NOT_NULL(p)
    *p = mPrimeP;
    REFCOUNT_ADD(*p)
    return NOERROR;
}

ECode CRSAMultiPrimePrivateCrtKeySpec::GetPrimeQ(
    /* [out] */ IBigInteger **q)
{
    VALIDATE_NOT_NULL(q)
    *q = mPrimeQ;
    REFCOUNT_ADD(*q)
    return NOERROR;
}

ECode CRSAMultiPrimePrivateCrtKeySpec::GetPublicExponent(
    /* [out] */ IBigInteger **pubExponent)
{
    VALIDATE_NOT_NULL(pubExponent)
    *pubExponent = mPublicExponent;
    REFCOUNT_ADD(*pubExponent)
    return NOERROR;
}

ECode CRSAMultiPrimePrivateCrtKeySpec::constructor(
    /* [in] */ IBigInteger *modulus,
    /* [in] */ IBigInteger *publicExponent,
    /* [in] */ IBigInteger *privateExponent,
    /* [in] */ IBigInteger *primeP,
    /* [in] */ IBigInteger *primeQ,
    /* [in] */ IBigInteger *primeExponentP,
    /* [in] */ IBigInteger *primeExponentQ,
    /* [in] */ IBigInteger *crtCoefficient,
    /* [in] */ ArrayOf<IRSAOtherPrimeInfo*> *otherPrimeInfo)
{
    RSAPrivateKeySpec::constructor(modulus, privateExponent);

    // Perform checks specified
    if (modulus == NULL) {
        //throw new NullPointerException("modulus == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (privateExponent == NULL) {
        //throw new NullPointerException("privateExponent == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (publicExponent == NULL) {
        //throw new NullPointerException("publicExponent == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (primeP == NULL) {
        //throw new NullPointerException("primeP == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (primeQ == NULL) {
        //throw new NullPointerException("primeQ == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (primeExponentP == NULL) {
        //throw new NullPointerException("primeExponentP == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (primeExponentQ == NULL) {
        //throw new NullPointerException("primeExponentQ == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (crtCoefficient == NULL) {
        //throw new NullPointerException("crtCoefficient == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (otherPrimeInfo) {
        if (!otherPrimeInfo->GetLength()) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        // Clone array to prevent subsequent modification
        mOtherPrimeInfo = ArrayOf<IRSAOtherPrimeInfo*>::Alloc(otherPrimeInfo->GetLength());
        mOtherPrimeInfo->Copy(0, otherPrimeInfo, 0, mOtherPrimeInfo->GetLength());
    }
    else {
        mOtherPrimeInfo = NULL;
    }

    mPublicExponent = publicExponent;
    mPrimeP = primeP;
    mPrimeQ = primeQ;
    mPrimeExponentP = primeExponentP;
    mPrimeExponentQ = primeExponentQ;
    mCrtCoefficient = crtCoefficient;
    return NOERROR;
}

}
}
}
