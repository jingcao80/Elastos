
#ifndef __ELASTOSX_CRYPTO_CCIPHER_H__
#define __ELASTOSX_CRYPTO_CCIPHER_H__

#include "_Elastosx_Crypto_CCipher.h"
#include "Cipher.h"

namespace Elastosx {
namespace Crypto {

CarClass(CCipher)
    , public Cipher
{
public:
    CAR_OBJECT_DECL()
};

}
}

#endif // __ELASTOSX_CRYPTO_CCIPHER_H__
