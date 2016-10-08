#include "org/conscrypt/COpenSSLECGroupContextHelper.h"
#include "org/conscrypt/COpenSSLECGroupContext.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(COpenSSLECGroupContextHelper)

CAR_INTERFACE_IMPL(COpenSSLECGroupContextHelper, Singleton, IOpenSSLECGroupContext)

ECode COpenSSLECGroupContextHelper::GetCurveByName(
    /* [in] */ const String& curveName,
    /* [out] */ IOpenSSLECGroupContext** result)
{
    return COpenSSLECGroupContext::GetCurveByName(curveName, result);
}

ECode COpenSSLECGroupContextHelper::GetInstance(
    /* [in] */ Int32 type,
    /* [in] */ IBigInteger* p,
    /* [in] */ IBigInteger* a,
    /* [in] */ IBigInteger* b,
    /* [in] */ IBigInteger* x,
    /* [in] */ IBigInteger* y,
    /* [in] */ IBigInteger* n,
    /* [in] */ IBigInteger* h,
    /* [out] */ IOpenSSLECGroupContext** result)
{
    return COpenSSLECGroupContext::GetInstance(
            type, p, a, b, x, y, n, h, result);
}

ECode COpenSSLECGroupContextHelper::GetInstance(
    /* [in] */ IECParameterSpec* params,
    /* [out] */ IOpenSSLECGroupContext** result)
{
    return COpenSSLECGroupContext::GetInstance(params, result);
}

} // namespace Conscrypt
} // namespace Org
