#include "org/conscrypt/COpenSSLECPublicKeyHelper.h"
#include "org/conscrypt/COpenSSLECPublicKey.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(COpenSSLECPublicKeyHelper)

CAR_INTERFACE_IMPL(COpenSSLECPublicKeyHelper, Singleton, IOpenSSLECPublicKey)

ECode COpenSSLECPublicKeyHelper::GetInstance(
    /* [in] */ IECPublicKey* ecPublicKey,
    /* [out] */ IOpenSSLKey** result)
{
    return COpenSSLECPublicKey::GetInstance(ecPublicKey, result);
}

} // namespace Conscrypt
} // namespace Org
