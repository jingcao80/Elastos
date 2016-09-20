
#ifndef __ORG_CONSCRYPT_OPENSSLRSAKEYPAIRGENERATOR_H__
#define __ORG_CONSCRYPT_OPENSSLRSAKEYPAIRGENERATOR_H__

#include "Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Security::IKeyPair;
using Elastos::Security::ISecureRandom;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Org {
namespace Conscrypt {

class OpenSSLRSAKeyPairGenerator 
    :  public Object // public KeyPairGeneratorSpi 
{
public:
    OpenSSLRSAKeyPairGenerator();
    
    CARAPI GenerateKeyPair(
        /* [out] */ IKeyPair** result);

    CARAPI Initialize(
        /* [in] */ Int32 keysize, 
        /* [in] */ ISecureRandom* random);

    CARAPI Initialize(
        /* [in] */ IAlgorithmParameterSpec* params, 
        /* [in] */ ISecureRandom* random);

private:
    /**
     * Default modulus size is 0x10001 (65537)
     */
    AutoPtr<ArrayOf<Byte> > mPublicExponent;

    /**
     * Default RSA key size 2048 bits.
     */
    Int32 mModulusBits;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLRSAKEYPAIRGENERATOR_H__