
#include "CMathContextHelper.h"
#include "CMathContext.h"

namespace Elastos {
namespace Math {

CAR_INTERFACE_IMPL(CMathContextHelper, Singleton, IMathContextHelper)

CAR_SINGLETON_IMPL(CMathContextHelper)

ECode CMathContextHelper::GetDECIMAL32(
    /* [out] */ IMathContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = CMathContext::DECIMAL32;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CMathContextHelper::GetDECIMAL64(
    /* [out] */ IMathContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = CMathContext::DECIMAL64;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CMathContextHelper::GetDECIMAL128(
    /* [out] */ IMathContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = CMathContext::DECIMAL128;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CMathContextHelper::GetUNLIMITED(
    /* [out] */ IMathContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = CMathContext::UNLIMITED;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

} // namespace Math
} // namespace Elastos
