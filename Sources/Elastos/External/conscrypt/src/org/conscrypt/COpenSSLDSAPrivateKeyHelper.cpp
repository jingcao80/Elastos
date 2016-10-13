#include "org/conscrypt/COpenSSLDSAPrivateKeyHelper.h"
#include "org/conscrypt/OpenSSLDSAPrivateKey.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(COpenSSLDSAPrivateKeyHelper)

CAR_INTERFACE_IMPL(COpenSSLDSAPrivateKeyHelper, Singleton, IOpenSSLDSAPrivateKey)

ECode COpenSSLDSAPrivateKeyHelper::GetInstance(
    /* [in] */ IDSAPrivateKey* dsaPrivateKey,
    /* [out] */ IOpenSSLKey** result)
{
    return OpenSSLDSAPrivateKey::GetInstance(dsaPrivateKey, result);
}

ECode COpenSSLDSAPrivateKeyHelper::WrapPlatformKey(
    /* [in] */ IDSAPrivateKey* dsaPrivateKey,
    /* [out] */ IOpenSSLKey** result)
{
    return OpenSSLDSAPrivateKey::WrapPlatformKey(dsaPrivateKey, result);
}

} // namespace Conscrypt
} // namespace Org
