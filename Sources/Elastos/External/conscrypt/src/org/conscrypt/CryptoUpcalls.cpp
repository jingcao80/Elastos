
#include "CryptoUpcalls.h"

namespace Org {
namespace Conscrypt {

ECode CryptoUpcalls::RawSignDigestWithPrivateKey(
    /* [in] */ IPrivateKey* key,
    /* [in] */ ArrayOf<Byte>* message,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    assert(0);
    return NOERROR;
}

ECode CryptoUpcalls::RawCipherWithPrivateKey(
    /* [in] */ IPrivateKey* key,
    /* [in] */ Boolean encrypt,
    /* [in] */ ArrayOf<Byte>* input,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    assert(0);
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
