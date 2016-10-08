#include "org/conscrypt/COpenSSLECKeyPairGenerator.h"
#include "org/conscrypt/COpenSSLECGroupContext.h"
#include "org/conscrypt/COpenSSLECPrivateKey.h"
#include "org/conscrypt/COpenSSLECPublicKey.h"
#include "org/conscrypt/OpenSSLKey.h"
#include "org/conscrypt/NativeCrypto.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Security::CKeyPair;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::Spec::IECGenParameterSpec;
using Elastos::Security::Spec::IECParameterSpec;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IHashMap;

namespace Org {
namespace Conscrypt {

const String COpenSSLECKeyPairGenerator::ALGORITHM("EC");

const Int32 COpenSSLECKeyPairGenerator::DEFAULT_KEY_SIZE;

AutoPtr<IMap> COpenSSLECKeyPairGenerator::SIZE_TO_CURVE_NAME;

Boolean COpenSSLECKeyPairGenerator::Init()
{
    CHashMap::New((IHashMap**)&SIZE_TO_CURVE_NAME);

    /* NIST curves */
    SIZE_TO_CURVE_NAME->Put(CoreUtils::Convert((Int32)192),
            CoreUtils::Convert(String("prime192v1")));
    SIZE_TO_CURVE_NAME->Put(CoreUtils::Convert((Int32)224),
            CoreUtils::Convert(String("secp224r1")));
    SIZE_TO_CURVE_NAME->Put(CoreUtils::Convert((Int32)256),
            CoreUtils::Convert(String("prime256v1")));
    SIZE_TO_CURVE_NAME->Put(CoreUtils::Convert((Int32)384),
            CoreUtils::Convert(String("secp384r1")));
    SIZE_TO_CURVE_NAME->Put(CoreUtils::Convert((Int32)521),
            CoreUtils::Convert(String("secp521r1")));

    return TRUE;
}

Boolean COpenSSLECKeyPairGenerator::sIsInited
        = COpenSSLECKeyPairGenerator::Init();

CAR_OBJECT_IMPL(COpenSSLECKeyPairGenerator)

CAR_INTERFACE_IMPL(COpenSSLECKeyPairGenerator, KeyPairGenerator,
        IOpenSSLECKeyPairGenerator)

ECode COpenSSLECKeyPairGenerator::constructor()
{
    return KeyPairGenerator::constructor(ALGORITHM);
}

ECode COpenSSLECKeyPairGenerator::GenerateKeyPair(
    /* [out] */ IKeyPair** result)
{
    VALIDATE_NOT_NULL(result)
    if (mGroup == NULL) {
        AutoPtr<IInterface> obj;
        SIZE_TO_CURVE_NAME->Get(CoreUtils::Convert(DEFAULT_KEY_SIZE), (IInterface**)&obj);
        String curveName;
        ICharSequence::Probe(obj)->ToString(&curveName);

        COpenSSLECGroupContext::GetCurveByName(curveName, (IOpenSSLECGroupContext**)&mGroup);
    }

    Int64 ctx;
    mGroup->GetContext(&ctx);
    Int64 val;
    NativeCrypto::EC_KEY_generate_key(ctx, &val);
    AutoPtr<OpenSSLKey> key = new OpenSSLKey(val);
    AutoPtr<IOpenSSLECPublicKey> publicKey;
    COpenSSLECPublicKey::New(mGroup, IOpenSSLKey::Probe(key), (IOpenSSLECPublicKey**)&publicKey);
    AutoPtr<IOpenSSLECPrivateKey> privateKey;
    COpenSSLECPrivateKey::New(mGroup, IOpenSSLKey::Probe(key), (IOpenSSLECPrivateKey**)&privateKey);
    return CKeyPair::New(IPublicKey::Probe(publicKey), IPrivateKey::Probe(privateKey), result);
}

ECode COpenSSLECKeyPairGenerator::Initialize(
    /* [in] */ Int32 keysize,
    /* [in] */ ISecureRandom* random)
{
    AutoPtr<IInterface> obj;
    SIZE_TO_CURVE_NAME->Get(CoreUtils::Convert(keysize), (IInterface**)&obj);
    String name;
    ICharSequence::Probe(obj)->ToString(&name);
    if (name.IsNull()) {
        // throw new InvalidParameterException("unknown key size " + keysize);
        return E_INVALID_PARAMETER_EXCEPTION;
    }

    /*
     * Store the group in a temporary variable until we know this is a valid
     * group.
     */
    AutoPtr<IOpenSSLECGroupContext> possibleGroup;
    COpenSSLECGroupContext::GetCurveByName(name, (IOpenSSLECGroupContext**)&possibleGroup);
    if (possibleGroup == NULL) {
        // throw new InvalidParameterException("unknown curve " + name);
        return E_INVALID_PARAMETER_EXCEPTION;
    }

    mGroup = possibleGroup;
    return NOERROR;
}

ECode COpenSSLECKeyPairGenerator::Initialize(
    /* [in] */ IAlgorithmParameterSpec* param,
    /* [in] */ ISecureRandom* random)
{
    if (IECParameterSpec::Probe(param) != NULL) {
        AutoPtr<IECParameterSpec> ecParam = IECParameterSpec::Probe(param);

        COpenSSLECGroupContext::GetInstance(ecParam, (IOpenSSLECGroupContext**)&mGroup);
    }
    else if (IECGenParameterSpec::Probe(param) != NULL) {
        AutoPtr<IECGenParameterSpec> ecParam = IECGenParameterSpec::Probe(param);

        String curveName;
        ecParam->GetName(&curveName);

        /*
         * Store the group in a temporary variable until we know this is a
         * valid group.
         */
        AutoPtr<IOpenSSLECGroupContext> possibleGroup;
        COpenSSLECGroupContext::GetCurveByName(curveName, (IOpenSSLECGroupContext**)&possibleGroup);
        if (possibleGroup == NULL) {
            // throw new InvalidAlgorithmParameterException("unknown curve name: " + curveName);
            return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
        }

        mGroup = possibleGroup;
    }
    else {
        // throw new InvalidAlgorithmParameterException(
        //         "parameter must be ECParameterSpec or ECGenParameterSpec");
        return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
