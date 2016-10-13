#include "org/conscrypt/COpenSSLECPrivateKeyHelper.h"
#include "org/conscrypt/COpenSSLECPrivateKey.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(COpenSSLECPrivateKeyHelper)

CAR_INTERFACE_IMPL(COpenSSLECPrivateKeyHelper, Singleton, IOpenSSLECPrivateKey)

ECode COpenSSLECPrivateKeyHelper::WrapPlatformKey(
    /* [in] */ IECPrivateKey* ecPrivateKey,
    /* [out] */ IOpenSSLKey** result)
{
    return COpenSSLECPrivateKey::WrapPlatformKey(ecPrivateKey, result);
}

ECode COpenSSLECPrivateKeyHelper::GetInstance(
    /* [in] */ IECPrivateKey* ecPrivateKey,
    /* [out] */ IOpenSSLKey** result)
{
    return COpenSSLECPrivateKey::GetInstance(ecPrivateKey, result);
}

} // namespace Conscrypt
} // namespace Org
