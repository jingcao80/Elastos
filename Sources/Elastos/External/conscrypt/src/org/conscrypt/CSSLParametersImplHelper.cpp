#include "org/conscrypt/CSSLParametersImplHelper.h"
#include "org/conscrypt/SSLParametersImpl.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(CSSLParametersImplHelper)

CAR_INTERFACE_IMPL(CSSLParametersImplHelper, Singleton, ISSLParametersImpl)

ECode CSSLParametersImplHelper::GetDefaultX509TrustManager(
    /* [out] */ IX509TrustManager** result)
{
    return SSLParametersImpl::GetDefaultX509TrustManager(result);
}

ECode CSSLParametersImplHelper::GetClientKeyType(
    /* [in] */ Byte keyType,
    /* [out] */ String* result)
{
    return SSLParametersImpl::GetClientKeyType(keyType, result);
}

} // namespace Conscrypt
} // namespace Org
