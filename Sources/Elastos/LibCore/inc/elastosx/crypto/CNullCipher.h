
#ifndef __ELASTOSX_CRYPTO_CNULLCIPHER_H__
#define __ELASTOSX_CRYPTO_CNULLCIPHER_H__

#include "_Elastosx_Crypto_CNullCipher.h"
#include "Cipher.h"

namespace Elastosx {
namespace Crypto {

CarClass(CNullCipher)
    , public Cipher
    , public INullCipher
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNullCipher();

    /**
     * Creates a new {@code NullCipher} instance.
     */
    CARAPI constructor();

};

}
}

#endif // __ELASTOSX_CRYPTO_CNULLCIPHER_H__
