#include "org/conscrypt/CCryptoUpcalls.h"
#include "org/conscrypt/CryptoUpcalls.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(CCryptoUpcalls)

CAR_INTERFACE_IMPL(CCryptoUpcalls, Singleton, ICryptoUpcalls)

ECode CCryptoUpcalls::GetExternalProvider(
    /* [in] */ const String& algorithm,
    /* [out] */ IProvider** result)
{
    return CryptoUpcalls::GetExternalProvider(algorithm, result);
}

ECode CCryptoUpcalls::RawSignDigestWithPrivateKey(
    /* [in] */ IPrivateKey* key,
    /* [in] */ ArrayOf<Byte>* message,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    return CryptoUpcalls::RawSignDigestWithPrivateKey(key, message, result);
}

ECode CCryptoUpcalls::RawCipherWithPrivateKey(
    /* [in] */ IPrivateKey* key,
    /* [in] */ Boolean encrypt,
    /* [in] */ ArrayOf<Byte>* input,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    return CryptoUpcalls::RawCipherWithPrivateKey(key, encrypt, input, result);
}

} // namespace Conscrypt
} // namespace Org
