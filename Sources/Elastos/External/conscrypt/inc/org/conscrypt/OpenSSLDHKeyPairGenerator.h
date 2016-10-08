
#ifndef __ORG_CONSCRYPT_OPENSSLDHKEYPAIRGENERATOR_H__
#define __ORG_CONSCRYPT_OPENSSLDHKEYPAIRGENERATOR_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/security/KeyPairGeneratorSpi.h>
#include "_Org.Conscrypt.h"

using Elastos::Math::IBigInteger;
using Elastos::Security::IKeyPair;
using Elastos::Security::ISecureRandom;
using Elastos::Security::KeyPairGeneratorSpi;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Org {
namespace Conscrypt {

class OpenSSLDHKeyPairGenerator
    : public IOpenSSLDHKeyPairGenerator
    , public KeyPairGeneratorSpi
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLDHKeyPairGenerator();

    virtual CARAPI GenerateKeyPair(
        /* [out] */  IKeyPair** result);

    virtual CARAPI Initialize(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom* random);

    virtual CARAPI Initialize(
        /* [in] */ IAlgorithmParameterSpec* params,
        /* [in] */ ISecureRandom* random);

private:
    /** The safe prime to use for the generated DH key pair. */
    AutoPtr<IBigInteger> mPrime;

    /** If {@code prime} is unspecified, this is the size of the generated prime. */
    Int32 mPrimeBits; // = 1024;

    static AutoPtr<IBigInteger> DEFAULT_GENERATOR; // = BigInteger.valueOf(2);

    AutoPtr<IBigInteger> mGenerator; // = DEFAULT_GENERATOR;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLDHKEYPAIRGENERATOR_H__
