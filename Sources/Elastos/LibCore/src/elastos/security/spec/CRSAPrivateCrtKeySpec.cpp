
#include "CRSAPrivateCrtKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CRSAPrivateCrtKeySpec)
CAR_INTERFACE_IMPL(CRSAPrivateCrtKeySpec, RSAPrivateKeySpec, IRSAPrivateCrtKeySpec)
ECode CRSAPrivateCrtKeySpec::GetCrtCoefficient(
    /* [out] */ IBigInteger **coefficient)
{
    VALIDATE_NOT_NULL(coefficient)
    *coefficient = mCrtCoefficient;
    REFCOUNT_ADD(*coefficient)
    return NOERROR;
}

ECode CRSAPrivateCrtKeySpec::GetPrimeExponentP(
    /* [out] */ IBigInteger **p)
{
    VALIDATE_NOT_NULL(p)
    *p = mPrimeExponentP;
    REFCOUNT_ADD(*p)
    return NOERROR;
}

ECode CRSAPrivateCrtKeySpec::GetPrimeExponentQ(
    /* [out] */ IBigInteger **q)
{
    VALIDATE_NOT_NULL(q)
    *q = mPrimeExponentQ;
    REFCOUNT_ADD(*q)
    return NOERROR;
}

ECode CRSAPrivateCrtKeySpec::GetPrimeP(
    /* [out] */ IBigInteger **p)
{
    VALIDATE_NOT_NULL(p)
    *p = mPrimeP;
    REFCOUNT_ADD(*p)
    return NOERROR;
}

ECode CRSAPrivateCrtKeySpec::GetPrimeQ(
    /* [out] */ IBigInteger **q)
{
    VALIDATE_NOT_NULL(q)
    *q = mPrimeQ;
    REFCOUNT_ADD(*q)
    return NOERROR;
}

ECode CRSAPrivateCrtKeySpec::GetPublicExponent(
    /* [out] */ IBigInteger **pubExponent)
{
    VALIDATE_NOT_NULL(pubExponent)
    *pubExponent = mPublicExponent;
    REFCOUNT_ADD(*pubExponent)
    return NOERROR;
}

ECode CRSAPrivateCrtKeySpec::constructor(
    /* [in] */ IBigInteger *modulus,
    /* [in] */ IBigInteger *publicExponent,
    /* [in] */ IBigInteger *privateExponent,
    /* [in] */ IBigInteger *primeP,
    /* [in] */ IBigInteger *primeQ,
    /* [in] */ IBigInteger *primeExponentP,
    /* [in] */ IBigInteger *primeExponentQ,
    /* [in] */ IBigInteger *crtCoefficient)
{
    RSAPrivateKeySpec::constructor(modulus, privateExponent);

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
