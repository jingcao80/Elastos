#include "CNullCipher.h"

using Elastos::Security::IKey;
using Elastos::Security::ISecureRandom;

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(CNullCipher, Cipher, INullCipher)

CNullCipher::CNullCipher()
{
}

ECode CNullCipher::constructor()
{
//TODO: Need INullCipherSpi
    // AutoPtr<INullCipherSpi> spi;
    // FAIL_RETURN(CNullCipherSpi::New((INullCipherSpi**)&spi));
    // Cipher::constructor(spi, NULL, NULL);
    // try {
    AutoPtr<IKey> key;
    AutoPtr<ISecureRandom> sr;
    return this->Init(ICipher::ENCRYPT_MODE, key, sr);
    // } catch (InvalidKeyException e) {
    // }
}

} // Crypto
} // Elastosx