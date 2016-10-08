#include "org/conscrypt/OpenSSLDSAKeyPairGenerator.h"
#include "org/conscrypt/NativeCrypto.h"
#include "org/conscrypt/OpenSSLDSAPrivateKey.h"
#include "org/conscrypt/OpenSSLDSAPublicKey.h"
#include "org/conscrypt/OpenSSLKey.h"

using Elastos::Security::CKeyPair;
using Elastos::Security::Interfaces::IDSAParams;
using Elastos::Security::Spec::IDSAParameterSpec;
using Elastos::Utility::IRandom;

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(OpenSSLDSAKeyPairGenerator, KeyPairGeneratorSpi,
        IOpenSSLDSAKeyPairGenerator)

OpenSSLDSAKeyPairGenerator::OpenSSLDSAKeyPairGenerator()
    : mPrimeBits(1024)
{
}

ECode OpenSSLDSAKeyPairGenerator::GenerateKeyPair(
    /* [out] */  IKeyPair** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > seed;
    if (mRandom == NULL) {
        seed = NULL;
    }
    else {
        seed = ArrayOf<Byte>::Alloc(20);
        IRandom::Probe(mRandom)->NextBytes(seed);
    }

    Int64 val;
    NativeCrypto::DSA_generate_key(mPrimeBits, seed, mG, mP, mQ, &val);
    AutoPtr<OpenSSLKey> key = new OpenSSLKey(val);

    AutoPtr<OpenSSLDSAPrivateKey> privKey = new OpenSSLDSAPrivateKey();
    privKey->constructor(IOpenSSLKey::Probe(key));
    AutoPtr<OpenSSLDSAPublicKey> pubKey = new OpenSSLDSAPublicKey();
    pubKey->constructor(IOpenSSLKey::Probe(key));

    return CKeyPair::New(IPublicKey::Probe(pubKey), IPrivateKey::Probe(privKey), result);
}

ECode OpenSSLDSAKeyPairGenerator::Initialize(
    /* [in] */ Int32 keysize,
    /* [in] */ ISecureRandom* random)
{
    mPrimeBits = keysize;
    mRandom = random;
    return NOERROR;
}

ECode OpenSSLDSAKeyPairGenerator::Initialize(
    /* [in] */ IAlgorithmParameterSpec* params,
    /* [in] */ ISecureRandom* random)
{
    mRandom = random;

    if (IDSAParameterSpec::Probe(params) != NULL) {
        AutoPtr<IDSAParameterSpec> dsaParams = IDSAParameterSpec::Probe(params);

        AutoPtr<IBigInteger> gInt;
        IDSAParams::Probe(dsaParams)->GetG((IBigInteger**)&gInt);
        if (gInt != NULL) {
            gInt->ToByteArray((ArrayOf<Byte>**)&mG);
        }

        AutoPtr<IBigInteger> pInt;
        IDSAParams::Probe(dsaParams)->GetP((IBigInteger**)&pInt);
        if (pInt != NULL) {
            pInt->ToByteArray((ArrayOf<Byte>**)&mP);
        }

        AutoPtr<IBigInteger> qInt;
        IDSAParams::Probe(dsaParams)->GetQ((IBigInteger**)&qInt);
        if (qInt != NULL) {
            qInt->ToByteArray((ArrayOf<Byte>**)&mQ);
        }
    }
    else if (params != NULL) {
        // throw new InvalidAlgorithmParameterException("Params must be DSAParameterSpec");
        return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
