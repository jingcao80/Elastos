
#include "Elastos.CoreLibrary.Security.h"
#include "OpenSSLRSAKeyPairGenerator.h"

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLRSAKeyPairGenerator::
//=========================================
OpenSSLRSAKeyPairGenerator::OpenSSLRSAKeyPairGenerator()
    : mModulusBits(2048)
{
    mPublicExponent = ArrayOf<Byte>::Alloc(3);
    (*mPublicExponent)[0] = 0x01;
    (*mPublicExponent)[1] = 0x00;
    (*mPublicExponent)[2] = 0x01;
}

ECode OpenSSLRSAKeyPairGenerator::GenerateKeyPair(
    /* [out] */ IKeyPair** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // Int64 key = 0;
    // NativeCrypto::RSA_generate_key_ex(modulusBits,
    //         publicExponent, &key);
    // AutoPtr<IOpenSSLKey> key = new OpenSSLKey(key);

    // AutoPtr<IPrivateKey> privKey = OpenSSLRSAPrivateKey::GetInstance(key);
    // AutoPtr<IPublicKey> pubKey = new OpenSSLRSAPublicKey(key);

    // AutoPtr<IKeyPair> p = new KeyPair(pubKey, privKey);
    // *result = p;
    // REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAKeyPairGenerator::Initialize(
    /* [in] */ Int32 keysize,
    /* [in] */ ISecureRandom* random)
{
    mModulusBits = keysize;
    return NOERROR;
}

ECode OpenSSLRSAKeyPairGenerator::Initialize(
    /* [in] */ IAlgorithmParameterSpec* params,
    /* [in] */ ISecureRandom* random)
{
    assert(0 && "TODO");
    // if (IRSAKeyGenParameterSpec::Probe(params) == NULL) {
    //     // throw new InvalidAlgorithmParameterException("Only RSAKeyGenParameterSpec supported");
    //     return NOERROR;
    // }

    // AutoPtr<IRSAKeyGenParameterSpec> spec = IRSAKeyGenParameterSpec::Probe(params);

    // AutoPtr<IBigInteger> publicExponent = spec->GetPublicExponent();
    // if (publicExponent != NULL) {
    //     mPublicExponent = publicExponent->ToByteArray();
    // }

    // mModulusBits = spec->GetKeysize();
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
