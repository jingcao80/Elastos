
#include "CBigIntegerHelper.h"
#include "CBigInteger.h"

namespace Elastos {
namespace Math {

CAR_INTERFACE_IMPL(CBigIntegerHelper, Singleton, IBigIntegerHelper)

CAR_SINGLETON_IMPL(CBigIntegerHelper)

ECode CBigIntegerHelper::ValueOf(
    /* [in] */ Int64 value,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    return CBigInteger::ValueOf(value, result);
}

ECode CBigIntegerHelper::ProbablePrime(
    /* [in] */ Int32 bitLength,
    /* [in] */ IRandom* random,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    return CBigInteger::ProbablePrime(bitLength, random, result);
}

ECode CBigIntegerHelper::GetMINUSOne(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CBigInteger::MINUS_ONE;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigIntegerHelper::GetZERO(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CBigInteger::ZERO;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigIntegerHelper::GetONE(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CBigInteger::ONE;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigIntegerHelper::GetTEN(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CBigInteger::TEN;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Math
} // namespace Elastos

