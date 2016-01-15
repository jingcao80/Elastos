
#include "CBigDecimal.h"
#include "CBigDecimalHelper.h"

namespace Elastos {
namespace Math {

CAR_INTERFACE_IMPL(CBigDecimalHelper, Singleton, IBigDecimalHelper)

CAR_SINGLETON_IMPL(CBigDecimalHelper)

ECode CBigDecimalHelper::ValueOf(
    /* [in] */ Int64 unscaledVal,
    /* [in] */ Int32 scale,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    return CBigDecimal::ValueOf(unscaledVal, scale, result);
}

ECode CBigDecimalHelper::ValueOf(
    /* [in] */ Int64 unscaledVal,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    return CBigDecimal::ValueOf(unscaledVal, result);
}

ECode CBigDecimalHelper::ValueOf(
    /* [in] */ Double val,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    return CBigDecimal::ValueOf(val, result);
}

ECode CBigDecimalHelper::GetZERO(
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CBigDecimal::ZERO;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigDecimalHelper::GetONE(
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CBigDecimal::ONE;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigDecimalHelper::GetTEN(
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CBigDecimal::TEN;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Math
} // namespace Elastos
