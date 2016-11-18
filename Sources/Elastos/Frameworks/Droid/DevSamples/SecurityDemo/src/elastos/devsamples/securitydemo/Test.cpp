
#include "elastos/devsamples/securitydemo/Test.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Security.h"
#include "elastosx/crypto/Cipher.h"
#include <elastos/utility/Arrays.h>
#include "elastos/utility/logging/Logger.h"

using Elastos::Math::CBigInteger;
using Elastos::Math::IBigInteger;
using Elastos::Security::CKeyFactoryHelper;
using Elastos::Security::CSignatureHelper;
using Elastos::Security::CSecureRandomHelper;
using Elastos::Security::IKeyFactory;
using Elastos::Security::IKeyFactoryHelper;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::ISignature;
using Elastos::Security::ISignatureHelper;
using Elastos::Security::ISecureRandomHelper;
using Elastos::Security::Spec::CRSAPrivateKeySpec;
using Elastos::Security::Spec::CRSAPublicKeySpec;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::IRSAPrivateKeySpec;
using Elastos::Security::Spec::IRSAPublicKeySpec;
using Elastos::Security::Spec::IPKCS8EncodedKeySpec;
using Elastos::Security::Spec::CPKCS8EncodedKeySpec;
using Elastos::Security::Spec::IX509EncodedKeySpec;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;
using Elastosx::Crypto::Cipher;
using Elastosx::Crypto::CKeyGeneratorHelper;
using Elastosx::Crypto::CSecretKeyFactoryHelper;
using Elastosx::Crypto::ICipher;
using Elastosx::Crypto::IKeyGenerator;
using Elastosx::Crypto::IKeyGeneratorHelper;
using Elastosx::Crypto::ISecretKey;
using Elastosx::Crypto::ISecretKeyFactory;
using Elastosx::Crypto::ISecretKeyFactoryHelper;
using Elastosx::Crypto::Spec::CIvParameterSpec;
using Elastosx::Crypto::Spec::CSecretKeySpec;
using Elastosx::Crypto::Spec::CDESedeKeySpec;
using Elastosx::Crypto::Spec::IDESedeKeySpec;
using Elastosx::Crypto::Spec::ISecretKeySpec;
using Elastosx::Crypto::Spec::IIvParameterSpec;

namespace Elastos {
namespace DevSamples {
namespace SecurityDemo {

static AutoPtr<IBigInteger> InitRSA_2048_modulus()
{
    Byte data[] = {
        (Byte) 0x00, (Byte) 0xe0, (Byte) 0x47, (Byte) 0x3e, (Byte) 0x8a, (Byte) 0xb8, (Byte) 0xf2, (Byte) 0x28,
        (Byte) 0x4f, (Byte) 0xeb, (Byte) 0x9e, (Byte) 0x74, (Byte) 0x2f, (Byte) 0xf9, (Byte) 0x74, (Byte) 0x8f,
        (Byte) 0xa1, (Byte) 0x18, (Byte) 0xed, (Byte) 0x98, (Byte) 0x63, (Byte) 0x3c, (Byte) 0x92, (Byte) 0xf5,
        (Byte) 0x2a, (Byte) 0xeb, (Byte) 0x7a, (Byte) 0x2e, (Byte) 0xbe, (Byte) 0x0d, (Byte) 0x3b, (Byte) 0xe6,
        (Byte) 0x03, (Byte) 0x29, (Byte) 0xbe, (Byte) 0x76, (Byte) 0x6a, (Byte) 0xd1, (Byte) 0x0e, (Byte) 0xb6,
        (Byte) 0xa5, (Byte) 0x15, (Byte) 0xd0, (Byte) 0xd2, (Byte) 0xcf, (Byte) 0xd9, (Byte) 0xbe, (Byte) 0xa7,
        (Byte) 0x93, (Byte) 0x0f, (Byte) 0x0c, (Byte) 0x30, (Byte) 0x65, (Byte) 0x37, (Byte) 0x89, (Byte) 0x9f,
        (Byte) 0x79, (Byte) 0x58, (Byte) 0xcd, (Byte) 0x3e, (Byte) 0x85, (Byte) 0xb0, (Byte) 0x1f, (Byte) 0x88,
        (Byte) 0x18, (Byte) 0x52, (Byte) 0x4d, (Byte) 0x31, (Byte) 0x25, (Byte) 0x84, (Byte) 0xa9, (Byte) 0x4b,
        (Byte) 0x25, (Byte) 0x1e, (Byte) 0x36, (Byte) 0x25, (Byte) 0xb5, (Byte) 0x41, (Byte) 0x41, (Byte) 0xed,
        (Byte) 0xbf, (Byte) 0xee, (Byte) 0x19, (Byte) 0x88, (Byte) 0x08, (Byte) 0xe1, (Byte) 0xbb, (Byte) 0x97,
        (Byte) 0xfc, (Byte) 0x7c, (Byte) 0xb4, (Byte) 0x9b, (Byte) 0x9e, (Byte) 0xaa, (Byte) 0xaf, (Byte) 0x68,
        (Byte) 0xe9, (Byte) 0xc9, (Byte) 0x8d, (Byte) 0x7d, (Byte) 0x0e, (Byte) 0xdc, (Byte) 0x53, (Byte) 0xbb,
        (Byte) 0xc0, (Byte) 0xfa, (Byte) 0x00, (Byte) 0x34, (Byte) 0x35, (Byte) 0x6d, (Byte) 0x63, (Byte) 0x05,
        (Byte) 0xfb, (Byte) 0xbc, (Byte) 0xc3, (Byte) 0xc7, (Byte) 0x00, (Byte) 0x14, (Byte) 0x05, (Byte) 0x38,
        (Byte) 0x6a, (Byte) 0xbb, (Byte) 0xc8, (Byte) 0x73, (Byte) 0xcb, (Byte) 0x0f, (Byte) 0x3e, (Byte) 0xf7,
        (Byte) 0x42, (Byte) 0x5f, (Byte) 0x3d, (Byte) 0x33, (Byte) 0xdf, (Byte) 0x7b, (Byte) 0x31, (Byte) 0x5a,
        (Byte) 0xe0, (Byte) 0x36, (Byte) 0xd2, (Byte) 0xa0, (Byte) 0xb6, (Byte) 0x6a, (Byte) 0xfd, (Byte) 0x47,
        (Byte) 0x50, (Byte) 0x3b, (Byte) 0x16, (Byte) 0x9b, (Byte) 0xf3, (Byte) 0x6e, (Byte) 0x3b, (Byte) 0x51,
        (Byte) 0x62, (Byte) 0x51, (Byte) 0x5b, (Byte) 0x71, (Byte) 0x5f, (Byte) 0xda, (Byte) 0x83, (Byte) 0xde,
        (Byte) 0xaf, (Byte) 0x2c, (Byte) 0x58, (Byte) 0xae, (Byte) 0xb9, (Byte) 0xab, (Byte) 0xfb, (Byte) 0x30,
        (Byte) 0x97, (Byte) 0xc3, (Byte) 0xcc, (Byte) 0x9d, (Byte) 0xd9, (Byte) 0xdb, (Byte) 0xe5, (Byte) 0xef,
        (Byte) 0x29, (Byte) 0x6c, (Byte) 0x17, (Byte) 0x61, (Byte) 0x39, (Byte) 0x02, (Byte) 0x8e, (Byte) 0x8a,
        (Byte) 0x67, (Byte) 0x1e, (Byte) 0x63, (Byte) 0x05, (Byte) 0x6d, (Byte) 0x45, (Byte) 0xf4, (Byte) 0x01,
        (Byte) 0x88, (Byte) 0xd2, (Byte) 0xc4, (Byte) 0x13, (Byte) 0x34, (Byte) 0x90, (Byte) 0x84, (Byte) 0x5d,
        (Byte) 0xe5, (Byte) 0x2c, (Byte) 0x25, (Byte) 0x34, (Byte) 0xe9, (Byte) 0xc6, (Byte) 0xb2, (Byte) 0x47,
        (Byte) 0x8c, (Byte) 0x07, (Byte) 0xbd, (Byte) 0xae, (Byte) 0x92, (Byte) 0x88, (Byte) 0x23, (Byte) 0xb6,
        (Byte) 0x2d, (Byte) 0x06, (Byte) 0x6c, (Byte) 0x77, (Byte) 0x70, (Byte) 0xf9, (Byte) 0xf6, (Byte) 0x3f,
        (Byte) 0x3d, (Byte) 0xba, (Byte) 0x24, (Byte) 0x7f, (Byte) 0x53, (Byte) 0x08, (Byte) 0x44, (Byte) 0x74,
        (Byte) 0x7b, (Byte) 0xe7, (Byte) 0xaa, (Byte) 0xa8, (Byte) 0x5d, (Byte) 0x85, (Byte) 0x3b, (Byte) 0x8b,
        (Byte) 0xd2, (Byte) 0x44, (Byte) 0xac, (Byte) 0xec, (Byte) 0x3d, (Byte) 0xe3, (Byte) 0xc8, (Byte) 0x9a,
        (Byte) 0xb4, (Byte) 0x64, (Byte) 0x53, (Byte) 0xab, (Byte) 0x4d, (Byte) 0x24, (Byte) 0xc3, (Byte) 0xac,
        (Byte) 0x69,
    };

    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::AllocInplace(data, sizeof(data)/sizeof(data[0]));
    AutoPtr<IBigInteger> obj;
    CBigInteger::New(*bytes, (IBigInteger**)&obj);
    return obj;
}

static AutoPtr<IBigInteger> InitRSA_2048_publicExponent()
{
    Byte data[] = {
        (Byte) 0x01, (Byte) 0x00, (Byte) 0x01,
    };

    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::AllocInplace(data, sizeof(data)/sizeof(data[0]));
    AutoPtr<IBigInteger> obj;
    CBigInteger::New(*bytes, (IBigInteger**)&obj);
    return obj;
}

static AutoPtr<IBigInteger> RSA_2048_modulus = InitRSA_2048_modulus();
static AutoPtr<IBigInteger> RSA_2048_publicExponent = InitRSA_2048_publicExponent();

static Byte Vector2Data[] = {
    (Byte) 0x54, (Byte) 0x68, (Byte) 0x69, (Byte) 0x73, (Byte) 0x20, (Byte) 0x69, (Byte) 0x73, (Byte) 0x20,
    (Byte) 0x61, (Byte) 0x20, (Byte) 0x73, (Byte) 0x69, (Byte) 0x67, (Byte) 0x6e, (Byte) 0x65, (Byte) 0x64,
    (Byte) 0x20, (Byte) 0x6d, (Byte) 0x65, (Byte) 0x73, (Byte) 0x73, (Byte) 0x61, (Byte) 0x67, (Byte) 0x65,
    (Byte) 0x20, (Byte) 0x66, (Byte) 0x72, (Byte) 0x6f, (Byte) 0x6d, (Byte) 0x20, (Byte) 0x4b, (Byte) 0x65,
    (Byte) 0x6e, (Byte) 0x6e, (Byte) 0x79, (Byte) 0x20, (Byte) 0x52, (Byte) 0x6f, (Byte) 0x6f, (Byte) 0x74,
    (Byte) 0x2e, (Byte) 0x0a,
};

static Byte MD5withRSA_Vector2Signature[] = {
    (Byte) 0x04, (Byte) 0x17, (Byte) 0x83, (Byte) 0x10, (Byte) 0xe2, (Byte) 0x6e, (Byte) 0xdf, (Byte) 0xa9,
    (Byte) 0xae, (Byte) 0xd2, (Byte) 0xdc, (Byte) 0x5f, (Byte) 0x70, (Byte) 0x1d, (Byte) 0xaf, (Byte) 0x54,
    (Byte) 0xc0, (Byte) 0x5f, (Byte) 0x0b, (Byte) 0x2c, (Byte) 0xe6, (Byte) 0xd0, (Byte) 0x00, (Byte) 0x18,
    (Byte) 0x4c, (Byte) 0xf6, (Byte) 0x8f, (Byte) 0x18, (Byte) 0x10, (Byte) 0x74, (Byte) 0x90, (Byte) 0x99,
    (Byte) 0xa9, (Byte) 0x90, (Byte) 0x3c, (Byte) 0x5a, (Byte) 0x38, (Byte) 0xd3, (Byte) 0x3d, (Byte) 0x48,
    (Byte) 0xcf, (Byte) 0x31, (Byte) 0xaf, (Byte) 0x12, (Byte) 0x98, (Byte) 0xfb, (Byte) 0x66, (Byte) 0xe8,
    (Byte) 0x58, (Byte) 0xec, (Byte) 0xca, (Byte) 0xe1, (Byte) 0x42, (Byte) 0xf9, (Byte) 0x84, (Byte) 0x17,
    (Byte) 0x6f, (Byte) 0x4c, (Byte) 0x3e, (Byte) 0xc4, (Byte) 0x40, (Byte) 0xc6, (Byte) 0x70, (Byte) 0xb0,
    (Byte) 0x38, (Byte) 0xf3, (Byte) 0x47, (Byte) 0xeb, (Byte) 0x6f, (Byte) 0xcb, (Byte) 0xea, (Byte) 0x21,
    (Byte) 0x41, (Byte) 0xf3, (Byte) 0xa0, (Byte) 0x3e, (Byte) 0x42, (Byte) 0xad, (Byte) 0xa5, (Byte) 0xad,
    (Byte) 0x5d, (Byte) 0x2c, (Byte) 0x1a, (Byte) 0x8e, (Byte) 0x3e, (Byte) 0xb3, (Byte) 0xa5, (Byte) 0x78,
    (Byte) 0x3d, (Byte) 0x56, (Byte) 0x09, (Byte) 0x93, (Byte) 0xc9, (Byte) 0x93, (Byte) 0xd3, (Byte) 0xd2,
    (Byte) 0x9a, (Byte) 0xc5, (Byte) 0xa5, (Byte) 0x2e, (Byte) 0xb2, (Byte) 0xd8, (Byte) 0x37, (Byte) 0xc7,
    (Byte) 0x13, (Byte) 0x1a, (Byte) 0x0b, (Byte) 0xda, (Byte) 0x50, (Byte) 0x28, (Byte) 0x6d, (Byte) 0x47,
    (Byte) 0x65, (Byte) 0x52, (Byte) 0xcd, (Byte) 0xe7, (Byte) 0xec, (Byte) 0x57, (Byte) 0x00, (Byte) 0x41,
    (Byte) 0x34, (Byte) 0x28, (Byte) 0xb9, (Byte) 0x8b, (Byte) 0x03, (Byte) 0x41, (Byte) 0xb6, (Byte) 0xd5,
    (Byte) 0xa8, (Byte) 0xef, (Byte) 0xd3, (Byte) 0xdd, (Byte) 0x80, (Byte) 0xd5, (Byte) 0x69, (Byte) 0xe4,
    (Byte) 0xf0, (Byte) 0x4d, (Byte) 0xa4, (Byte) 0x7d, (Byte) 0x60, (Byte) 0x2f, (Byte) 0xef, (Byte) 0x79,
    (Byte) 0x07, (Byte) 0x75, (Byte) 0xeb, (Byte) 0xf7, (Byte) 0x4b, (Byte) 0x43, (Byte) 0x41, (Byte) 0xdb,
    (Byte) 0x33, (Byte) 0xad, (Byte) 0x9c, (Byte) 0x7b, (Byte) 0x78, (Byte) 0x83, (Byte) 0x34, (Byte) 0x77,
    (Byte) 0xe4, (Byte) 0x80, (Byte) 0xbe, (Byte) 0xe6, (Byte) 0x6f, (Byte) 0xdd, (Byte) 0xac, (Byte) 0xa5,
    (Byte) 0x37, (Byte) 0xcf, (Byte) 0xb5, (Byte) 0x44, (Byte) 0x11, (Byte) 0x77, (Byte) 0x96, (Byte) 0x45,
    (Byte) 0xf9, (Byte) 0xae, (Byte) 0x48, (Byte) 0xa6, (Byte) 0xbe, (Byte) 0x30, (Byte) 0x32, (Byte) 0xeb,
    (Byte) 0x43, (Byte) 0x6f, (Byte) 0x66, (Byte) 0x39, (Byte) 0x57, (Byte) 0xf8, (Byte) 0xe6, (Byte) 0x60,
    (Byte) 0x31, (Byte) 0xd0, (Byte) 0xfc, (Byte) 0xcf, (Byte) 0x9f, (Byte) 0xe5, (Byte) 0x3d, (Byte) 0xcf,
    (Byte) 0xbd, (Byte) 0x7b, (Byte) 0x13, (Byte) 0x20, (Byte) 0xce, (Byte) 0x11, (Byte) 0xfd, (Byte) 0xe5,
    (Byte) 0xff, (Byte) 0x90, (Byte) 0x85, (Byte) 0xdf, (Byte) 0xca, (Byte) 0x3d, (Byte) 0xd9, (Byte) 0x44,
    (Byte) 0x16, (Byte) 0xc2, (Byte) 0x32, (Byte) 0x28, (Byte) 0xc7, (Byte) 0x01, (Byte) 0x6d, (Byte) 0xea,
    (Byte) 0xcb, (Byte) 0x0d, (Byte) 0x85, (Byte) 0x08, (Byte) 0x6f, (Byte) 0xcb, (Byte) 0x41, (Byte) 0x6a,
    (Byte) 0x3c, (Byte) 0x0f, (Byte) 0x3d, (Byte) 0x38, (Byte) 0xb5, (Byte) 0x61, (Byte) 0xc5, (Byte) 0x64,
    (Byte) 0x64, (Byte) 0x81, (Byte) 0x4c, (Byte) 0xcd, (Byte) 0xd1, (Byte) 0x6a, (Byte) 0x87, (Byte) 0x28,
    (Byte) 0x02, (Byte) 0xaf, (Byte) 0x8f, (Byte) 0x59, (Byte) 0xe5, (Byte) 0x67, (Byte) 0x25, (Byte) 0x00,
};

static AutoPtr<IBigInteger> InitRSA_2048_privateExponent()
{
    Byte data[] = {
        (Byte) 0x37, (Byte) 0x78, (Byte) 0x47, (Byte) 0x76, (Byte) 0xa5, (Byte) 0xf1, (Byte) 0x76, (Byte) 0x98,
        (Byte) 0xf5, (Byte) 0xac, (Byte) 0x96, (Byte) 0x0d, (Byte) 0xfb, (Byte) 0x83, (Byte) 0xa1, (Byte) 0xb6,
        (Byte) 0x75, (Byte) 0x64, (Byte) 0xe6, (Byte) 0x48, (Byte) 0xbd, (Byte) 0x05, (Byte) 0x97, (Byte) 0xcf,
        (Byte) 0x8a, (Byte) 0xb8, (Byte) 0x08, (Byte) 0x71, (Byte) 0x86, (Byte) 0xf2, (Byte) 0x66, (Byte) 0x9c,
        (Byte) 0x27, (Byte) 0xa9, (Byte) 0xec, (Byte) 0xbd, (Byte) 0xd4, (Byte) 0x80, (Byte) 0xf0, (Byte) 0x19,
        (Byte) 0x7a, (Byte) 0x80, (Byte) 0xd0, (Byte) 0x73, (Byte) 0x09, (Byte) 0xe6, (Byte) 0xc6, (Byte) 0xa9,
        (Byte) 0x6f, (Byte) 0x92, (Byte) 0x53, (Byte) 0x31, (Byte) 0xe5, (Byte) 0x7f, (Byte) 0x8b, (Byte) 0x4a,
        (Byte) 0xc6, (Byte) 0xf4, (Byte) 0xd4, (Byte) 0x5e, (Byte) 0xda, (Byte) 0x45, (Byte) 0xa2, (Byte) 0x32,
        (Byte) 0x69, (Byte) 0xc0, (Byte) 0x9f, (Byte) 0xc4, (Byte) 0x28, (Byte) 0xc0, (Byte) 0x7a, (Byte) 0x4e,
        (Byte) 0x6e, (Byte) 0xdf, (Byte) 0x73, (Byte) 0x8a, (Byte) 0x15, (Byte) 0xde, (Byte) 0xc9, (Byte) 0x7f,
        (Byte) 0xab, (Byte) 0xd2, (Byte) 0xf2, (Byte) 0xbb, (Byte) 0x47, (Byte) 0xa1, (Byte) 0x4f, (Byte) 0x20,
        (Byte) 0xea, (Byte) 0x72, (Byte) 0xfc, (Byte) 0xfe, (Byte) 0x4c, (Byte) 0x36, (Byte) 0xe0, (Byte) 0x1a,
        (Byte) 0xda, (Byte) 0x77, (Byte) 0xbd, (Byte) 0x13, (Byte) 0x7c, (Byte) 0xd8, (Byte) 0xd4, (Byte) 0xda,
        (Byte) 0x10, (Byte) 0xbb, (Byte) 0x16, (Byte) 0x2e, (Byte) 0x94, (Byte) 0xa4, (Byte) 0x66, (Byte) 0x29,
        (Byte) 0x71, (Byte) 0xf1, (Byte) 0x75, (Byte) 0xf9, (Byte) 0x85, (Byte) 0xfa, (Byte) 0x18, (Byte) 0x8f,
        (Byte) 0x05, (Byte) 0x6c, (Byte) 0xb9, (Byte) 0x7e, (Byte) 0xe2, (Byte) 0x81, (Byte) 0x6f, (Byte) 0x43,
        (Byte) 0xab, (Byte) 0x9d, (Byte) 0x37, (Byte) 0x47, (Byte) 0x61, (Byte) 0x24, (Byte) 0x86, (Byte) 0xcd,
        (Byte) 0xa8, (Byte) 0xc1, (Byte) 0x61, (Byte) 0x96, (Byte) 0xc3, (Byte) 0x08, (Byte) 0x18, (Byte) 0xa9,
        (Byte) 0x95, (Byte) 0xec, (Byte) 0x85, (Byte) 0xd3, (Byte) 0x84, (Byte) 0x67, (Byte) 0x79, (Byte) 0x12,
        (Byte) 0x67, (Byte) 0xb3, (Byte) 0xbf, (Byte) 0x21, (Byte) 0xf2, (Byte) 0x73, (Byte) 0x71, (Byte) 0x0a,
        (Byte) 0x69, (Byte) 0x25, (Byte) 0x86, (Byte) 0x25, (Byte) 0x76, (Byte) 0x84, (Byte) 0x1c, (Byte) 0x5b,
        (Byte) 0x67, (Byte) 0x12, (Byte) 0xc1, (Byte) 0x2d, (Byte) 0x4b, (Byte) 0xd2, (Byte) 0x0a, (Byte) 0x2f,
        (Byte) 0x32, (Byte) 0x99, (Byte) 0xad, (Byte) 0xb7, (Byte) 0xc1, (Byte) 0x35, (Byte) 0xda, (Byte) 0x5e,
        (Byte) 0x95, (Byte) 0x15, (Byte) 0xab, (Byte) 0xda, (Byte) 0x76, (Byte) 0xe7, (Byte) 0xca, (Byte) 0xf2,
        (Byte) 0xa3, (Byte) 0xbe, (Byte) 0x80, (Byte) 0x55, (Byte) 0x1d, (Byte) 0x07, (Byte) 0x3b, (Byte) 0x78,
        (Byte) 0xbf, (Byte) 0x11, (Byte) 0x62, (Byte) 0xc4, (Byte) 0x8a, (Byte) 0xd2, (Byte) 0xb7, (Byte) 0xf4,
        (Byte) 0x74, (Byte) 0x3a, (Byte) 0x02, (Byte) 0x38, (Byte) 0xee, (Byte) 0x4d, (Byte) 0x25, (Byte) 0x2f,
        (Byte) 0x7d, (Byte) 0x5e, (Byte) 0x7e, (Byte) 0x65, (Byte) 0x33, (Byte) 0xcc, (Byte) 0xae, (Byte) 0x64,
        (Byte) 0xcc, (Byte) 0xb3, (Byte) 0x93, (Byte) 0x60, (Byte) 0x07, (Byte) 0x5a, (Byte) 0x2f, (Byte) 0xd1,
        (Byte) 0xe0, (Byte) 0x34, (Byte) 0xec, (Byte) 0x3a, (Byte) 0xe5, (Byte) 0xce, (Byte) 0x9c, (Byte) 0x40,
        (Byte) 0x8c, (Byte) 0xcb, (Byte) 0xf0, (Byte) 0xe2, (Byte) 0x5e, (Byte) 0x41, (Byte) 0x14, (Byte) 0x02,
        (Byte) 0x16, (Byte) 0x87, (Byte) 0xb3, (Byte) 0xdd, (Byte) 0x47, (Byte) 0x54, (Byte) 0xae, (Byte) 0x81,
    };

    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::AllocInplace(data, sizeof(data)/sizeof(data[0]));
    AutoPtr<IBigInteger> obj;
    CBigInteger::New(*bytes, (IBigInteger**)&obj);
    return obj;
}

static AutoPtr<IBigInteger> RSA_2048_privateExponent = InitRSA_2048_privateExponent();

ECode SignatureTest::SignatureWithRSA()
{
    ECode ec = NOERROR;
    AutoPtr<IKeyFactoryHelper> helper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
    AutoPtr<IKeyFactory> kf;
    ec = helper->GetInstance(String("RSA"), (IKeyFactory**)&kf);
    ASSERT_SUCCEEDED(ec);

    //Sign
    AutoPtr<IRSAPrivateKeySpec> privateKeySpec;
    CRSAPrivateKeySpec::New(RSA_2048_modulus,
            RSA_2048_privateExponent, (IRSAPrivateKeySpec**)&privateKeySpec);
    AutoPtr<IPrivateKey> privKey;
    kf->GeneratePrivate(IKeySpec::Probe(privateKeySpec), (IPrivateKey**)&privKey);

    AutoPtr<ISignatureHelper> sh;
    CSignatureHelper::AcquireSingleton((ISignatureHelper**)&sh);
    //test history:
    String algorithm("MD5withRSA");
    // MD5WithRSA ok
    // String algorithm("SHA1WithRSA");
    // SHA1WithRSA ok
    //String algorithm("SHA224WithRSA");
    // SHA224WithRSA ok
    //String algorithm("SHA256WithRSA");
    // SHA256WithRSA ok
    //String algorithm("SHA384WithRSA");
    // SHA384WithRSA ok
    //String algorithm("SHA512WithRSA");
    // SHA512WithRSA ok
    //String algorithm("SHA1withDSA");
    // SHA1withDSA
    //String algorithm("NONEwithRSA");
    // NONEwithRSA ok

    Logger::D("SignatureTest", "[TODO] algorithm:%s=====", algorithm.string());
    AutoPtr<ISignature> sig;
    ec = sh->GetInstance(algorithm, (ISignature**)&sig);
    ASSERT_SUCCEEDED(ec);

    sig->InitSign(privKey);
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::AllocInplace(Vector2Data
            , sizeof(Vector2Data)/sizeof(Vector2Data[0]));
    ec = sig->Update(bytes);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > signature;
    ec = sig->Sign((ArrayOf<Byte>**)&signature);
    ASSERT_SUCCEEDED(ec);
    assert(signature != NULL && "Signature must not be null");

    if (algorithm.Equals(String("MD5withRSA"))) {
        AutoPtr<ArrayOf<Byte> > bytes2 = ArrayOf<Byte>::AllocInplace(MD5withRSA_Vector2Signature
                , sizeof(MD5withRSA_Vector2Signature)/sizeof(MD5withRSA_Vector2Signature[0]));
        Boolean e = Arrays::Equals(signature, bytes2);
        assert("Signature should match expected" && e);
    } else {
        Logger::D("SignatureTest", "Signature match not be tested");
    }

    //Verify
    AutoPtr<IRSAPublicKeySpec> keySpec;
    CRSAPublicKeySpec::New(RSA_2048_modulus, RSA_2048_publicExponent, (IRSAPublicKeySpec**)&keySpec);
    AutoPtr<IPublicKey> pubKey;
    ec = kf->GeneratePublic(IKeySpec::Probe(keySpec), (IPublicKey**)&pubKey);
    ASSERT_SUCCEEDED(ec);
    assert(pubKey != NULL);

    ec = sig->InitVerify(pubKey);
    ASSERT_SUCCEEDED(ec);
    ec = sig->Update(bytes);

    Boolean isVerified = FALSE;
    ec = sig->Verify(signature, &isVerified);
    ASSERT_SUCCEEDED(ec);
    assert("Signature must match expected signature" && isVerified);
    return NOERROR;
}

ECode SignatureTest::SignatureWithEC()
{
    ECode ec = NOERROR;
    AutoPtr<IKeyFactoryHelper> helper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
    AutoPtr<IKeyFactory> kf;
    ec = helper->GetInstance(String("EC"), (IKeyFactory**)&kf);
    ASSERT_SUCCEEDED(ec);

    //Sign
    AutoPtr<ArrayOf<Byte> > ec_private = ArrayOf<Byte>::Alloc(67);
    ec_private->Set(0, 0x30); ec_private->Set(1, 0x41); ec_private->Set(2, 0x2); ec_private->Set(3, 0x1);
    ec_private->Set(4, 0x0); ec_private->Set(5, 0x30); ec_private->Set(6, 0x13); ec_private->Set(7, 0x6);
    ec_private->Set(8, 0x7); ec_private->Set(9, 0x2a); ec_private->Set(10, 0x86); ec_private->Set(11, 0x48);
    ec_private->Set(12, 0xce); ec_private->Set(13, 0x3d); ec_private->Set(14, 0x2); ec_private->Set(15, 0x1);
    ec_private->Set(16, 0x6); ec_private->Set(17, 0x8); ec_private->Set(18, 0x2a); ec_private->Set(19, 0x86);
    ec_private->Set(20, 0x48); ec_private->Set(21, 0xce); ec_private->Set(22, 0x3d); ec_private->Set(23, 0x3);
    ec_private->Set(24, 0x1); ec_private->Set(25, 0x7); ec_private->Set(26, 0x4); ec_private->Set(27, 0x27);
    ec_private->Set(28, 0x30); ec_private->Set(29, 0x25); ec_private->Set(30, 0x2); ec_private->Set(31, 0x1);
    ec_private->Set(32, 0x1); ec_private->Set(33, 0x4); ec_private->Set(34, 0x20); ec_private->Set(35, 0x1c);
    ec_private->Set(36, 0x4c); ec_private->Set(37, 0x8a); ec_private->Set(38, 0x6d); ec_private->Set(39, 0x9b);
    ec_private->Set(40, 0xd8); ec_private->Set(41, 0x6f); ec_private->Set(42, 0x6); ec_private->Set(43, 0xd8);
    ec_private->Set(44, 0xd2); ec_private->Set(45, 0x78); ec_private->Set(46, 0x21); ec_private->Set(47, 0x38);
    ec_private->Set(48, 0xa3); ec_private->Set(49, 0xce); ec_private->Set(50, 0xaa); ec_private->Set(51, 0xfa);
    ec_private->Set(52, 0x9c); ec_private->Set(53, 0x79); ec_private->Set(54, 0x93); ec_private->Set(55, 0x72);
    ec_private->Set(56, 0x41); ec_private->Set(57, 0xdc); ec_private->Set(58, 0x35); ec_private->Set(59, 0xb2);
    ec_private->Set(60, 0x54); ec_private->Set(61, 0xf8); ec_private->Set(62, 0xdb); ec_private->Set(63, 0xd6);
    ec_private->Set(64, 0x75); ec_private->Set(65, 0xae); ec_private->Set(66, 0x85);

    AutoPtr<IPKCS8EncodedKeySpec> ecPrivatekeySpec;
    CPKCS8EncodedKeySpec::New(ec_private, (IPKCS8EncodedKeySpec**)&ecPrivatekeySpec);
    AutoPtr<IPrivateKey> privKey;
    kf->GeneratePrivate(IKeySpec::Probe(ecPrivatekeySpec), (IPrivateKey**)&privKey);

    AutoPtr<ISignatureHelper> sh;
    CSignatureHelper::AcquireSingleton((ISignatureHelper**)&sh);
    //test history:
    //String algorithm("SHA1withDSA");
    // SHA1withDSA
    //String algorithm("ECDSA");
    // ECDSA ok
    //String algorithm("SHA224withECDSA");
    // SHA224withECDSA ok
    //String algorithm("SHA256withECDSA");
    // SHA256withECDSA ok
    //String algorithm("SHA384withECDSA");
    // SHA384withECDSA ok
    String algorithm("SHA512withECDSA");
    // SHA512withECDSA ok

    Logger::D("SignatureTest", "[TODO] algorithm:%s=====", algorithm.string());
    AutoPtr<ISignature> sig;
    ec = sh->GetInstance(algorithm, (ISignature**)&sig);
    ASSERT_SUCCEEDED(ec);

    sig->InitSign(privKey);
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::AllocInplace(Vector2Data
            , sizeof(Vector2Data)/sizeof(Vector2Data[0]));
    ec = sig->Update(bytes);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > signature;
    ec = sig->Sign((ArrayOf<Byte>**)&signature);
    ASSERT_SUCCEEDED(ec);
    assert(signature != NULL && "Signature must not be null");
    Logger::D("SignatureTest", "Signature match not be tested");

    //Verify
    AutoPtr<ArrayOf<Byte> > ec_public = ArrayOf<Byte>::Alloc(91);
    ec_public->Set(0, 0x30); ec_public->Set(1, 0x59); ec_public->Set(2, 0x30); ec_public->Set(3, 0x13);
    ec_public->Set(4, 0x6); ec_public->Set(5, 0x7); ec_public->Set(6, 0x2a); ec_public->Set(7, 0x86);
    ec_public->Set(8, 0x48); ec_public->Set(9, 0xce); ec_public->Set(10, 0x3d); ec_public->Set(11, 0x2);
    ec_public->Set(12, 0x1); ec_public->Set(13, 0x6); ec_public->Set(14, 0x8); ec_public->Set(15, 0x2a);
    ec_public->Set(16, 0x86); ec_public->Set(17, 0x48); ec_public->Set(18, 0xce); ec_public->Set(19, 0x3d);
    ec_public->Set(20, 0x3); ec_public->Set(21, 0x1); ec_public->Set(22, 0x7); ec_public->Set(23, 0x3);
    ec_public->Set(24, 0x42); ec_public->Set(25, 0x0); ec_public->Set(26, 0x4); ec_public->Set(27, 0x46);
    ec_public->Set(28, 0x2a); ec_public->Set(29, 0xb1); ec_public->Set(30, 0xd); ec_public->Set(31, 0x7);
    ec_public->Set(32, 0xa4); ec_public->Set(33, 0x1b); ec_public->Set(34, 0x5b); ec_public->Set(35, 0xb7);
    ec_public->Set(36, 0x18); ec_public->Set(37, 0x35); ec_public->Set(38, 0x2f); ec_public->Set(39, 0xba);
    ec_public->Set(40, 0x93); ec_public->Set(41, 0x5); ec_public->Set(42, 0xf8); ec_public->Set(43, 0x78);
    ec_public->Set(44, 0x8f); ec_public->Set(45, 0x23); ec_public->Set(46, 0x5f); ec_public->Set(47, 0x38);
    ec_public->Set(48, 0x62); ec_public->Set(49, 0x2d); ec_public->Set(50, 0x11); ec_public->Set(51, 0x96);
    ec_public->Set(52, 0x48); ec_public->Set(53, 0x38); ec_public->Set(54, 0x45); ec_public->Set(55, 0x10);
    ec_public->Set(56, 0x50); ec_public->Set(57, 0x67); ec_public->Set(58, 0xde); ec_public->Set(59, 0x2);
    ec_public->Set(60, 0x94); ec_public->Set(61, 0xb6); ec_public->Set(62, 0x86); ec_public->Set(63, 0xa9);
    ec_public->Set(64, 0x66); ec_public->Set(65, 0x96); ec_public->Set(66, 0xe); ec_public->Set(67, 0x36);
    ec_public->Set(68, 0xde); ec_public->Set(69, 0x56); ec_public->Set(70, 0x63); ec_public->Set(71, 0xe);
    ec_public->Set(72, 0x6b); ec_public->Set(73, 0xb); ec_public->Set(74, 0x1f); ec_public->Set(75, 0xd3);
    ec_public->Set(76, 0x84); ec_public->Set(77, 0x9a); ec_public->Set(78, 0x2b); ec_public->Set(79, 0x33);
    ec_public->Set(80, 0xcf); ec_public->Set(81, 0xe4); ec_public->Set(82, 0x5f); ec_public->Set(83, 0x67);
    ec_public->Set(84, 0x44); ec_public->Set(85, 0x8c); ec_public->Set(86, 0x3); ec_public->Set(87, 0x8f);
    ec_public->Set(88, 0x2a); ec_public->Set(89, 0x8e); ec_public->Set(90, 0x7d);

    AutoPtr<IX509EncodedKeySpec> x509EncodedKeySpec;
    CX509EncodedKeySpec::New(ec_public, (IX509EncodedKeySpec**)&x509EncodedKeySpec);
    IKeySpec* keySpec = IKeySpec::Probe(x509EncodedKeySpec);

    AutoPtr<IPublicKey> pubKey;
    kf->GeneratePublic(keySpec, (IPublicKey**)&pubKey);

    ASSERT_SUCCEEDED(ec);
    assert(pubKey != NULL);

    ec = sig->InitVerify(pubKey);
    ASSERT_SUCCEEDED(ec);
    ec = sig->Update(bytes);

    Boolean isVerified = FALSE;
    ec = sig->Verify(signature, &isVerified);
    ASSERT_SUCCEEDED(ec);
    assert("Signature must match expected signature" && isVerified);
    return NOERROR;
}

static AutoPtr<ArrayOf<Byte> > GetPublicKeyEncodedByteArray()
{
    AutoPtr<ArrayOf<Byte> > dsa_public = ArrayOf<Byte>::Alloc(444);
    dsa_public->Set(0, 0x30); dsa_public->Set(1, 0x82); dsa_public->Set(2, 0x1); dsa_public->Set(3, 0xb8);
    dsa_public->Set(4, 0x30); dsa_public->Set(5, 0x82); dsa_public->Set(6, 0x1); dsa_public->Set(7, 0x2c);
    dsa_public->Set(8, 0x6); dsa_public->Set(9, 0x7); dsa_public->Set(10, 0x2a); dsa_public->Set(11, 0x86);
    dsa_public->Set(12, 0x48); dsa_public->Set(13, 0xce); dsa_public->Set(14, 0x38); dsa_public->Set(15, 0x4);
    dsa_public->Set(16, 0x1); dsa_public->Set(17, 0x30); dsa_public->Set(18, 0x82); dsa_public->Set(19, 0x1);
    dsa_public->Set(20, 0x1f); dsa_public->Set(21, 0x2); dsa_public->Set(22, 0x81); dsa_public->Set(23, 0x81);
    dsa_public->Set(24, 0x0); dsa_public->Set(25, 0xfd); dsa_public->Set(26, 0x7f); dsa_public->Set(27, 0x53);
    dsa_public->Set(28, 0x81); dsa_public->Set(29, 0x1d); dsa_public->Set(30, 0x75); dsa_public->Set(31, 0x12);
    dsa_public->Set(32, 0x29); dsa_public->Set(33, 0x52); dsa_public->Set(34, 0xdf); dsa_public->Set(35, 0x4a);
    dsa_public->Set(36, 0x9c); dsa_public->Set(37, 0x2e); dsa_public->Set(38, 0xec); dsa_public->Set(39, 0xe4);
    dsa_public->Set(40, 0xe7); dsa_public->Set(41, 0xf6); dsa_public->Set(42, 0x11); dsa_public->Set(43, 0xb7);
    dsa_public->Set(44, 0x52); dsa_public->Set(45, 0x3c); dsa_public->Set(46, 0xef); dsa_public->Set(47, 0x44);
    dsa_public->Set(48, 0x0); dsa_public->Set(49, 0xc3); dsa_public->Set(50, 0x1e); dsa_public->Set(51, 0x3f);
    dsa_public->Set(52, 0x80); dsa_public->Set(53, 0xb6); dsa_public->Set(54, 0x51); dsa_public->Set(55, 0x26);
    dsa_public->Set(56, 0x69); dsa_public->Set(57, 0x45); dsa_public->Set(58, 0x5d); dsa_public->Set(59, 0x40);
    dsa_public->Set(60, 0x22); dsa_public->Set(61, 0x51); dsa_public->Set(62, 0xfb); dsa_public->Set(63, 0x59);
    dsa_public->Set(64, 0x3d); dsa_public->Set(65, 0x8d); dsa_public->Set(66, 0x58); dsa_public->Set(67, 0xfa);
    dsa_public->Set(68, 0xbf); dsa_public->Set(69, 0xc5); dsa_public->Set(70, 0xf5); dsa_public->Set(71, 0xba);
    dsa_public->Set(72, 0x30); dsa_public->Set(73, 0xf6); dsa_public->Set(74, 0xcb); dsa_public->Set(75, 0x9b);
    dsa_public->Set(76, 0x55); dsa_public->Set(77, 0x6c); dsa_public->Set(78, 0xd7); dsa_public->Set(79, 0x81);
    dsa_public->Set(80, 0x3b); dsa_public->Set(81, 0x80); dsa_public->Set(82, 0x1d); dsa_public->Set(83, 0x34);
    dsa_public->Set(84, 0x6f); dsa_public->Set(85, 0xf2); dsa_public->Set(86, 0x66); dsa_public->Set(87, 0x60);
    dsa_public->Set(88, 0xb7); dsa_public->Set(89, 0x6b); dsa_public->Set(90, 0x99); dsa_public->Set(91, 0x50);
    dsa_public->Set(92, 0xa5); dsa_public->Set(93, 0xa4); dsa_public->Set(94, 0x9f); dsa_public->Set(95, 0x9f);
    dsa_public->Set(96, 0xe8); dsa_public->Set(97, 0x4); dsa_public->Set(98, 0x7b); dsa_public->Set(99, 0x10);
    dsa_public->Set(100, 0x22); dsa_public->Set(101, 0xc2); dsa_public->Set(102, 0x4f); dsa_public->Set(103, 0xbb);
    dsa_public->Set(104, 0xa9); dsa_public->Set(105, 0xd7); dsa_public->Set(106, 0xfe); dsa_public->Set(107, 0xb7);
    dsa_public->Set(108, 0xc6); dsa_public->Set(109, 0x1b); dsa_public->Set(110, 0xf8); dsa_public->Set(111, 0x3b);
    dsa_public->Set(112, 0x57); dsa_public->Set(113, 0xe7); dsa_public->Set(114, 0xc6); dsa_public->Set(115, 0xa8);
    dsa_public->Set(116, 0xa6); dsa_public->Set(117, 0x15); dsa_public->Set(118, 0xf); dsa_public->Set(119, 0x4);
    dsa_public->Set(120, 0xfb); dsa_public->Set(121, 0x83); dsa_public->Set(122, 0xf6); dsa_public->Set(123, 0xd3);
    dsa_public->Set(124, 0xc5); dsa_public->Set(125, 0x1e); dsa_public->Set(126, 0xc3); dsa_public->Set(127, 0x2);
    dsa_public->Set(128, 0x35); dsa_public->Set(129, 0x54); dsa_public->Set(130, 0x13); dsa_public->Set(131, 0x5a);
    dsa_public->Set(132, 0x16); dsa_public->Set(133, 0x91); dsa_public->Set(134, 0x32); dsa_public->Set(135, 0xf6);
    dsa_public->Set(136, 0x75); dsa_public->Set(137, 0xf3); dsa_public->Set(138, 0xae); dsa_public->Set(139, 0x2b);
    dsa_public->Set(140, 0x61); dsa_public->Set(141, 0xd7); dsa_public->Set(142, 0x2a); dsa_public->Set(143, 0xef);
    dsa_public->Set(144, 0xf2); dsa_public->Set(145, 0x22); dsa_public->Set(146, 0x3); dsa_public->Set(147, 0x19);
    dsa_public->Set(148, 0x9d); dsa_public->Set(149, 0xd1); dsa_public->Set(150, 0x48); dsa_public->Set(151, 0x1);
    dsa_public->Set(152, 0xc7); dsa_public->Set(153, 0x2); dsa_public->Set(154, 0x15); dsa_public->Set(155, 0x0);
    dsa_public->Set(156, 0x97); dsa_public->Set(157, 0x60); dsa_public->Set(158, 0x50); dsa_public->Set(159, 0x8f);
    dsa_public->Set(160, 0x15); dsa_public->Set(161, 0x23); dsa_public->Set(162, 0xb); dsa_public->Set(163, 0xcc);
    dsa_public->Set(164, 0xb2); dsa_public->Set(165, 0x92); dsa_public->Set(166, 0xb9); dsa_public->Set(167, 0x82);
    dsa_public->Set(168, 0xa2);
    dsa_public->Set(169, 0xeb); dsa_public->Set(170, 0x84); dsa_public->Set(171, 0xb); dsa_public->Set(172, 0xf0);
    dsa_public->Set(173, 0x58); dsa_public->Set(174, 0x1c); dsa_public->Set(175, 0xf5); dsa_public->Set(176, 0x2);
    dsa_public->Set(177, 0x81); dsa_public->Set(178, 0x81); dsa_public->Set(179, 0x0); dsa_public->Set(180, 0xf7);
    dsa_public->Set(181, 0xe1); dsa_public->Set(182, 0xa0); dsa_public->Set(183, 0x85); dsa_public->Set(184, 0xd6);
    dsa_public->Set(185, 0x9b); dsa_public->Set(186, 0x3d); dsa_public->Set(187, 0xde); dsa_public->Set(188, 0xcb);
    dsa_public->Set(189, 0xbc); dsa_public->Set(190, 0xab); dsa_public->Set(191, 0x5c); dsa_public->Set(192, 0x36);
    dsa_public->Set(193, 0xb8); dsa_public->Set(194, 0x57); dsa_public->Set(195, 0xb9); dsa_public->Set(196, 0x79);
    dsa_public->Set(197, 0x94); dsa_public->Set(198, 0xaf); dsa_public->Set(199, 0xbb); dsa_public->Set(200, 0xfa);
    dsa_public->Set(201, 0x3a); dsa_public->Set(202, 0xea); dsa_public->Set(203, 0x82); dsa_public->Set(204, 0xf9);
    dsa_public->Set(205, 0x57); dsa_public->Set(206, 0x4c); dsa_public->Set(207, 0xb); dsa_public->Set(208, 0x3d);
    dsa_public->Set(209, 0x7); dsa_public->Set(210, 0x82); dsa_public->Set(211, 0x67); dsa_public->Set(212, 0x51);
    dsa_public->Set(213, 0x59); dsa_public->Set(214, 0x57); dsa_public->Set(215, 0x8e); dsa_public->Set(216, 0xba);
    dsa_public->Set(217, 0xd4); dsa_public->Set(218, 0x59); dsa_public->Set(219, 0x4f); dsa_public->Set(220, 0xe6);
    dsa_public->Set(221, 0x71); dsa_public->Set(222, 0x7); dsa_public->Set(223, 0x10); dsa_public->Set(224, 0x81);
    dsa_public->Set(225, 0x80); dsa_public->Set(226, 0xb4); dsa_public->Set(227, 0x49); dsa_public->Set(228, 0x16);
    dsa_public->Set(229, 0x71); dsa_public->Set(230, 0x23); dsa_public->Set(231, 0xe8); dsa_public->Set(232, 0x4c);
    dsa_public->Set(233, 0x28); dsa_public->Set(234, 0x16); dsa_public->Set(235, 0x13); dsa_public->Set(236, 0xb7);
    dsa_public->Set(237, 0xcf); dsa_public->Set(238, 0x9); dsa_public->Set(239, 0x32); dsa_public->Set(240, 0x8c);
    dsa_public->Set(241, 0xc8); dsa_public->Set(242, 0xa6); dsa_public->Set(243, 0xe1); dsa_public->Set(244, 0x3c);
    dsa_public->Set(245, 0x16); dsa_public->Set(246, 0x7a); dsa_public->Set(247, 0x8b); dsa_public->Set(248, 0x54);
    dsa_public->Set(249, 0x7c); dsa_public->Set(250, 0x8d); dsa_public->Set(251, 0x28); dsa_public->Set(252, 0xe0);
    dsa_public->Set(253, 0xa3); dsa_public->Set(254, 0xae); dsa_public->Set(255, 0x1e); dsa_public->Set(256, 0x2b);
    dsa_public->Set(257, 0xb3); dsa_public->Set(258, 0xa6); dsa_public->Set(259, 0x75); dsa_public->Set(260, 0x91);
    dsa_public->Set(261, 0x6e); dsa_public->Set(262, 0xa3); dsa_public->Set(263, 0x7f); dsa_public->Set(264, 0xb);
    dsa_public->Set(265, 0xfa); dsa_public->Set(266, 0x21); dsa_public->Set(267, 0x35); dsa_public->Set(268, 0x62);
    dsa_public->Set(269, 0xf1); dsa_public->Set(270, 0xfb); dsa_public->Set(271, 0x62); dsa_public->Set(272, 0x7a);
    dsa_public->Set(273, 0x1); dsa_public->Set(274, 0x24); dsa_public->Set(275, 0x3b); dsa_public->Set(276, 0xcc);
    dsa_public->Set(277, 0xa4); dsa_public->Set(278, 0xf1); dsa_public->Set(279, 0xbe); dsa_public->Set(280, 0xa8);
    dsa_public->Set(281, 0x51); dsa_public->Set(282, 0x90); dsa_public->Set(283, 0x89); dsa_public->Set(284, 0xa8);
    dsa_public->Set(285, 0x83); dsa_public->Set(286, 0xdf); dsa_public->Set(287, 0xe1); dsa_public->Set(288, 0x5a);
    dsa_public->Set(289, 0xe5); dsa_public->Set(290, 0x9f); dsa_public->Set(291, 0x6); dsa_public->Set(292, 0x92);
    dsa_public->Set(293, 0x8b); dsa_public->Set(294, 0x66); dsa_public->Set(295, 0x5e); dsa_public->Set(296, 0x80);
    dsa_public->Set(297, 0x7b); dsa_public->Set(298, 0x55); dsa_public->Set(299, 0x25); dsa_public->Set(300, 0x64);
    dsa_public->Set(301, 0x1); dsa_public->Set(302, 0x4c); dsa_public->Set(303, 0x3b); dsa_public->Set(304, 0xfe);
    dsa_public->Set(305, 0xcf); dsa_public->Set(306, 0x49); dsa_public->Set(307, 0x2a); dsa_public->Set(308, 0x3);
    dsa_public->Set(309, 0x81); dsa_public->Set(310, 0x85); dsa_public->Set(311, 0x0); dsa_public->Set(312, 0x2);
    dsa_public->Set(313, 0x81); dsa_public->Set(314, 0x81); dsa_public->Set(315, 0x0); dsa_public->Set(316, 0x81);
    dsa_public->Set(317, 0x2a); dsa_public->Set(318, 0x82); dsa_public->Set(319, 0xf8); dsa_public->Set(320, 0x31);
    dsa_public->Set(321, 0x88); dsa_public->Set(322, 0x90); dsa_public->Set(323, 0x85); dsa_public->Set(324, 0x73);
    dsa_public->Set(325, 0x56); dsa_public->Set(326, 0x64); dsa_public->Set(327, 0xb4); dsa_public->Set(328, 0xd8);
    dsa_public->Set(329, 0x9d); dsa_public->Set(330, 0x69); dsa_public->Set(331, 0x0); dsa_public->Set(332, 0x89);
    dsa_public->Set(333, 0x39); dsa_public->Set(334, 0x31); dsa_public->Set(335, 0x45); dsa_public->Set(336, 0x8c);
    dsa_public->Set(337, 0xf); dsa_public->Set(338, 0x41); dsa_public->Set(339, 0x59); dsa_public->Set(340, 0x6a);
    dsa_public->Set(341, 0x8d); dsa_public->Set(342, 0xf); dsa_public->Set(343, 0xf1); dsa_public->Set(344, 0xf9);
    dsa_public->Set(345, 0x4f); dsa_public->Set(346, 0x77); dsa_public->Set(347, 0xfb); dsa_public->Set(348, 0xa4);
    dsa_public->Set(349, 0x3e); dsa_public->Set(350, 0x3e); dsa_public->Set(351, 0xa2); dsa_public->Set(352, 0xfb);
    dsa_public->Set(353, 0x2e); dsa_public->Set(354, 0x9b); dsa_public->Set(355, 0x9); dsa_public->Set(356, 0xea);
    dsa_public->Set(357, 0xc); dsa_public->Set(358, 0xd); dsa_public->Set(359, 0x60); dsa_public->Set(360, 0x46);
    dsa_public->Set(361, 0xe0); dsa_public->Set(362, 0xaf); dsa_public->Set(363, 0xd6); dsa_public->Set(364, 0x8e);
    dsa_public->Set(365, 0xf6); dsa_public->Set(366, 0xd9); dsa_public->Set(367, 0xb5); dsa_public->Set(368, 0xc6);
    dsa_public->Set(369, 0xe2); dsa_public->Set(370, 0x32); dsa_public->Set(371, 0xaf); dsa_public->Set(372, 0xfb);
    dsa_public->Set(373, 0xa2); dsa_public->Set(374, 0xb5); dsa_public->Set(375, 0x80); dsa_public->Set(376, 0xb6);
    dsa_public->Set(377, 0xe8); dsa_public->Set(378, 0x5c); dsa_public->Set(379, 0x50); dsa_public->Set(380, 0xe1);
    dsa_public->Set(381, 0x57); dsa_public->Set(382, 0x6); dsa_public->Set(383, 0xb5); dsa_public->Set(384, 0x1f);
    dsa_public->Set(385, 0xca); dsa_public->Set(386, 0x9b); dsa_public->Set(387, 0x4e); dsa_public->Set(388, 0x1d);
    dsa_public->Set(389, 0x66); dsa_public->Set(390, 0x2a); dsa_public->Set(391, 0x56); dsa_public->Set(392, 0x5a);
    dsa_public->Set(393, 0x72); dsa_public->Set(394, 0x50); dsa_public->Set(395, 0xa5); dsa_public->Set(396, 0x10);
    dsa_public->Set(397, 0xb2); dsa_public->Set(398, 0xdb); dsa_public->Set(399, 0x12); dsa_public->Set(400, 0x91);
    dsa_public->Set(401, 0xa8); dsa_public->Set(402, 0xc0); dsa_public->Set(403, 0xd4); dsa_public->Set(404, 0x27);
    dsa_public->Set(405, 0x4a); dsa_public->Set(406, 0x27); dsa_public->Set(407, 0xf5); dsa_public->Set(408, 0x48);
    dsa_public->Set(409, 0x12); dsa_public->Set(410, 0xbe); dsa_public->Set(411, 0x44); dsa_public->Set(412, 0x1d);
    dsa_public->Set(413, 0x98); dsa_public->Set(414, 0x6b); dsa_public->Set(415, 0x8f); dsa_public->Set(416, 0x68);
    dsa_public->Set(417, 0x8); dsa_public->Set(418, 0x8d); dsa_public->Set(419, 0x13); dsa_public->Set(420, 0xc8);
    dsa_public->Set(421, 0xb8); dsa_public->Set(422, 0xb4); dsa_public->Set(423, 0x15); dsa_public->Set(424, 0x8b);
    dsa_public->Set(425, 0x25); dsa_public->Set(426, 0xcb); dsa_public->Set(427, 0x88); dsa_public->Set(428, 0x9b);
    dsa_public->Set(429, 0xa7); dsa_public->Set(430, 0x78); dsa_public->Set(431, 0xd); dsa_public->Set(432, 0xaf);
    dsa_public->Set(433, 0xc6); dsa_public->Set(434, 0x14); dsa_public->Set(435, 0x23); dsa_public->Set(436, 0x6e);
    dsa_public->Set(437, 0xa3); dsa_public->Set(438, 0x19); dsa_public->Set(439, 0x6c); dsa_public->Set(440, 0x2);
    dsa_public->Set(441, 0x33); dsa_public->Set(442, 0xf1); dsa_public->Set(443, 0x30);
    return dsa_public;
}

static AutoPtr<ArrayOf<Byte> > GetPrivateKeyEncodedByteArray()
{
    AutoPtr<ArrayOf<Byte> > dsa_private = ArrayOf<Byte>::Alloc(336);
    dsa_private->Set(0, 0x30); dsa_private->Set(1, 0x82); dsa_private->Set(2, 0x1); dsa_private->Set(3, 0x4c);
    dsa_private->Set(4, 0x2); dsa_private->Set(5, 0x1); dsa_private->Set(6, 0x0); dsa_private->Set(7, 0x30);
    dsa_private->Set(8, 0x82); dsa_private->Set(9, 0x1); dsa_private->Set(10, 0x2c); dsa_private->Set(11, 0x6);
    dsa_private->Set(12, 0x7); dsa_private->Set(13, 0x2a); dsa_private->Set(14, 0x86); dsa_private->Set(15, 0x48);
    dsa_private->Set(16, 0xce); dsa_private->Set(17, 0x38); dsa_private->Set(18, 0x4); dsa_private->Set(19, 0x1);
    dsa_private->Set(20, 0x30); dsa_private->Set(21, 0x82); dsa_private->Set(22, 0x1); dsa_private->Set(23, 0x1f);
    dsa_private->Set(24, 0x2); dsa_private->Set(25, 0x81); dsa_private->Set(26, 0x81); dsa_private->Set(27, 0x0);
    dsa_private->Set(28, 0xfd); dsa_private->Set(29, 0x7f); dsa_private->Set(30, 0x53); dsa_private->Set(31, 0x81);
    dsa_private->Set(32, 0x1d); dsa_private->Set(33, 0x75); dsa_private->Set(34, 0x12); dsa_private->Set(35, 0x29);
    dsa_private->Set(36, 0x52); dsa_private->Set(37, 0xdf); dsa_private->Set(38, 0x4a); dsa_private->Set(39, 0x9c);
    dsa_private->Set(40, 0x2e); dsa_private->Set(41, 0xec); dsa_private->Set(42, 0xe4); dsa_private->Set(43, 0xe7);
    dsa_private->Set(44, 0xf6); dsa_private->Set(45, 0x11); dsa_private->Set(46, 0xb7); dsa_private->Set(47, 0x52);
    dsa_private->Set(48, 0x3c); dsa_private->Set(49, 0xef); dsa_private->Set(50, 0x44); dsa_private->Set(51, 0x0);
    dsa_private->Set(52, 0xc3); dsa_private->Set(53, 0x1e); dsa_private->Set(54, 0x3f); dsa_private->Set(55, 0x80);
    dsa_private->Set(56, 0xb6); dsa_private->Set(57, 0x51); dsa_private->Set(58, 0x26); dsa_private->Set(59, 0x69);
    dsa_private->Set(60, 0x45); dsa_private->Set(61, 0x5d); dsa_private->Set(62, 0x40); dsa_private->Set(63, 0x22);
    dsa_private->Set(64, 0x51); dsa_private->Set(65, 0xfb); dsa_private->Set(66, 0x59); dsa_private->Set(67, 0x3d);
    dsa_private->Set(68, 0x8d); dsa_private->Set(69, 0x58); dsa_private->Set(70, 0xfa); dsa_private->Set(71, 0xbf);
    dsa_private->Set(72, 0xc5); dsa_private->Set(73, 0xf5); dsa_private->Set(74, 0xba); dsa_private->Set(75, 0x30);
    dsa_private->Set(76, 0xf6); dsa_private->Set(77, 0xcb); dsa_private->Set(78, 0x9b); dsa_private->Set(79, 0x55);
    dsa_private->Set(80, 0x6c); dsa_private->Set(81, 0xd7); dsa_private->Set(82, 0x81); dsa_private->Set(83, 0x3b);
    dsa_private->Set(84, 0x80); dsa_private->Set(85, 0x1d); dsa_private->Set(86, 0x34); dsa_private->Set(87, 0x6f);
    dsa_private->Set(88, 0xf2); dsa_private->Set(89, 0x66); dsa_private->Set(90, 0x60); dsa_private->Set(91, 0xb7);
    dsa_private->Set(92, 0x6b); dsa_private->Set(93, 0x99); dsa_private->Set(94, 0x50); dsa_private->Set(95, 0xa5);
    dsa_private->Set(96, 0xa4); dsa_private->Set(97, 0x9f); dsa_private->Set(98, 0x9f); dsa_private->Set(99, 0xe8);
    dsa_private->Set(100, 0x4); dsa_private->Set(101, 0x7b); dsa_private->Set(102, 0x10); dsa_private->Set(103, 0x22);
    dsa_private->Set(104, 0xc2); dsa_private->Set(105, 0x4f); dsa_private->Set(106, 0xbb); dsa_private->Set(107, 0xa9);
    dsa_private->Set(108, 0xd7); dsa_private->Set(109, 0xfe); dsa_private->Set(110, 0xb7); dsa_private->Set(111, 0xc6);
    dsa_private->Set(112, 0x1b); dsa_private->Set(113, 0xf8); dsa_private->Set(114, 0x3b); dsa_private->Set(115, 0x57);
    dsa_private->Set(116, 0xe7); dsa_private->Set(117, 0xc6); dsa_private->Set(118, 0xa8); dsa_private->Set(119, 0xa6);
    dsa_private->Set(120, 0x15); dsa_private->Set(121, 0xf); dsa_private->Set(122, 0x4); dsa_private->Set(123, 0xfb);
    dsa_private->Set(124, 0x83); dsa_private->Set(125, 0xf6); dsa_private->Set(126, 0xd3); dsa_private->Set(127, 0xc5);
    dsa_private->Set(128, 0x1e); dsa_private->Set(129, 0xc3); dsa_private->Set(130, 0x2); dsa_private->Set(131, 0x35);
    dsa_private->Set(132, 0x54); dsa_private->Set(133, 0x13); dsa_private->Set(134, 0x5a); dsa_private->Set(135, 0x16);
    dsa_private->Set(136, 0x91); dsa_private->Set(137, 0x32); dsa_private->Set(138, 0xf6); dsa_private->Set(139, 0x75);
    dsa_private->Set(140, 0xf3); dsa_private->Set(141, 0xae); dsa_private->Set(142, 0x2b); dsa_private->Set(143, 0x61);
    dsa_private->Set(144, 0xd7); dsa_private->Set(145, 0x2a); dsa_private->Set(146, 0xef); dsa_private->Set(147, 0xf2);
    dsa_private->Set(148, 0x22); dsa_private->Set(149, 0x3); dsa_private->Set(150, 0x19); dsa_private->Set(151, 0x9d);
    dsa_private->Set(152, 0xd1); dsa_private->Set(153, 0x48); dsa_private->Set(154, 0x1); dsa_private->Set(155, 0xc7);
    dsa_private->Set(156, 0x2); dsa_private->Set(157, 0x15); dsa_private->Set(158, 0x0); dsa_private->Set(159, 0x97);
    dsa_private->Set(160, 0x60); dsa_private->Set(161, 0x50); dsa_private->Set(162, 0x8f); dsa_private->Set(163, 0x15);
    dsa_private->Set(164, 0x23); dsa_private->Set(165, 0xb); dsa_private->Set(166, 0xcc); dsa_private->Set(167, 0xb2);
    dsa_private->Set(168, 0x92); dsa_private->Set(169, 0xb9); dsa_private->Set(170, 0x82); dsa_private->Set(171, 0xa2);
    dsa_private->Set(172, 0xeb); dsa_private->Set(173, 0x84); dsa_private->Set(174, 0xb); dsa_private->Set(175, 0xf0);
    dsa_private->Set(176, 0x58); dsa_private->Set(177, 0x1c); dsa_private->Set(178, 0xf5); dsa_private->Set(179, 0x2);
    dsa_private->Set(180, 0x81); dsa_private->Set(181, 0x81); dsa_private->Set(182, 0x0); dsa_private->Set(183, 0xf7);
    dsa_private->Set(184, 0xe1); dsa_private->Set(185, 0xa0); dsa_private->Set(186, 0x85); dsa_private->Set(187, 0xd6);
    dsa_private->Set(188, 0x9b); dsa_private->Set(189, 0x3d); dsa_private->Set(190, 0xde); dsa_private->Set(191, 0xcb);
    dsa_private->Set(192, 0xbc); dsa_private->Set(193, 0xab); dsa_private->Set(194, 0x5c); dsa_private->Set(195, 0x36);
    dsa_private->Set(196, 0xb8); dsa_private->Set(197, 0x57); dsa_private->Set(198, 0xb9); dsa_private->Set(199, 0x79);
    dsa_private->Set(200, 0x94); dsa_private->Set(201, 0xaf); dsa_private->Set(202, 0xbb); dsa_private->Set(203, 0xfa);
    dsa_private->Set(204, 0x3a); dsa_private->Set(205, 0xea); dsa_private->Set(206, 0x82); dsa_private->Set(207, 0xf9);
    dsa_private->Set(208, 0x57); dsa_private->Set(209, 0x4c); dsa_private->Set(210, 0xb); dsa_private->Set(211, 0x3d);
    dsa_private->Set(212, 0x7); dsa_private->Set(213, 0x82); dsa_private->Set(214, 0x67); dsa_private->Set(215, 0x51);
    dsa_private->Set(216, 0x59); dsa_private->Set(217, 0x57); dsa_private->Set(218, 0x8e); dsa_private->Set(219, 0xba);
    dsa_private->Set(220, 0xd4); dsa_private->Set(221, 0x59); dsa_private->Set(222, 0x4f); dsa_private->Set(223, 0xe6);
    dsa_private->Set(224, 0x71); dsa_private->Set(225, 0x7); dsa_private->Set(226, 0x10); dsa_private->Set(227, 0x81);
    dsa_private->Set(228, 0x80); dsa_private->Set(229, 0xb4); dsa_private->Set(230, 0x49); dsa_private->Set(231, 0x16);
    dsa_private->Set(232, 0x71); dsa_private->Set(233, 0x23); dsa_private->Set(234, 0xe8); dsa_private->Set(235, 0x4c);
    dsa_private->Set(236, 0x28); dsa_private->Set(237, 0x16); dsa_private->Set(238, 0x13); dsa_private->Set(239, 0xb7);
    dsa_private->Set(240, 0xcf); dsa_private->Set(241, 0x9); dsa_private->Set(242, 0x32); dsa_private->Set(243, 0x8c);
    dsa_private->Set(244, 0xc8); dsa_private->Set(245, 0xa6); dsa_private->Set(246, 0xe1); dsa_private->Set(247, 0x3c);
    dsa_private->Set(248, 0x16); dsa_private->Set(249, 0x7a); dsa_private->Set(250, 0x8b); dsa_private->Set(251, 0x54);
    dsa_private->Set(252, 0x7c); dsa_private->Set(253, 0x8d); dsa_private->Set(254, 0x28); dsa_private->Set(255, 0xe0);
    dsa_private->Set(256, 0xa3); dsa_private->Set(257, 0xae); dsa_private->Set(258, 0x1e); dsa_private->Set(259, 0x2b);
    dsa_private->Set(260, 0xb3); dsa_private->Set(261, 0xa6); dsa_private->Set(262, 0x75); dsa_private->Set(263, 0x91);
    dsa_private->Set(264, 0x6e); dsa_private->Set(265, 0xa3); dsa_private->Set(266, 0x7f); dsa_private->Set(267, 0xb);
    dsa_private->Set(268, 0xfa); dsa_private->Set(269, 0x21); dsa_private->Set(270, 0x35); dsa_private->Set(271, 0x62);
    dsa_private->Set(272, 0xf1); dsa_private->Set(273, 0xfb); dsa_private->Set(274, 0x62); dsa_private->Set(275, 0x7a);
    dsa_private->Set(276, 0x1); dsa_private->Set(277, 0x24); dsa_private->Set(278, 0x3b); dsa_private->Set(279, 0xcc);
    dsa_private->Set(280, 0xa4); dsa_private->Set(281, 0xf1); dsa_private->Set(282, 0xbe); dsa_private->Set(283, 0xa8);
    dsa_private->Set(284, 0x51); dsa_private->Set(285, 0x90); dsa_private->Set(286, 0x89); dsa_private->Set(287, 0xa8);
    dsa_private->Set(288, 0x83); dsa_private->Set(289, 0xdf); dsa_private->Set(290, 0xe1); dsa_private->Set(291, 0x5a);
    dsa_private->Set(292, 0xe5); dsa_private->Set(293, 0x9f); dsa_private->Set(294, 0x6); dsa_private->Set(295, 0x92);
    dsa_private->Set(296, 0x8b); dsa_private->Set(297, 0x66); dsa_private->Set(298, 0x5e); dsa_private->Set(299, 0x80);
    dsa_private->Set(300, 0x7b); dsa_private->Set(301, 0x55); dsa_private->Set(302, 0x25); dsa_private->Set(303, 0x64);
    dsa_private->Set(304, 0x1); dsa_private->Set(305, 0x4c); dsa_private->Set(306, 0x3b); dsa_private->Set(307, 0xfe);
    dsa_private->Set(308, 0xcf); dsa_private->Set(309, 0x49); dsa_private->Set(310, 0x2a); dsa_private->Set(311, 0x4);
    dsa_private->Set(312, 0x17); dsa_private->Set(313, 0x2); dsa_private->Set(314, 0x15); dsa_private->Set(315, 0x0);
    dsa_private->Set(316, 0x91); dsa_private->Set(317, 0x7d); dsa_private->Set(318, 0xca); dsa_private->Set(319, 0x84);
    dsa_private->Set(320, 0x8e); dsa_private->Set(321, 0xc4); dsa_private->Set(322, 0x5c); dsa_private->Set(323, 0xd9);
    dsa_private->Set(324, 0x5f); dsa_private->Set(325, 0xa6); dsa_private->Set(326, 0x63); dsa_private->Set(327, 0xb1);
    dsa_private->Set(328, 0xf9); dsa_private->Set(329, 0x7d); dsa_private->Set(330, 0xe8); dsa_private->Set(331, 0xb5);
    dsa_private->Set(332, 0xe2); dsa_private->Set(333, 0x98); dsa_private->Set(334, 0xaf); dsa_private->Set(335, 0x55);

    return dsa_private;
}

ECode SignatureTest::SignatureWithDSA()
{
    ECode ec = NOERROR;
    AutoPtr<IKeyFactoryHelper> helper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
    AutoPtr<IKeyFactory> kf;
    ec = helper->GetInstance(String("DSA"), (IKeyFactory**)&kf);
    ASSERT_SUCCEEDED(ec);

    //Sign
    AutoPtr<ArrayOf<Byte> > dsa_private = GetPrivateKeyEncodedByteArray();
    AutoPtr<IPKCS8EncodedKeySpec> dsaPrivatekeySpec;
    CPKCS8EncodedKeySpec::New(dsa_private, (IPKCS8EncodedKeySpec**)&dsaPrivatekeySpec);
    AutoPtr<IPrivateKey> privKey;
    kf->GeneratePrivate(IKeySpec::Probe(dsaPrivatekeySpec), (IPrivateKey**)&privKey);

    AutoPtr<ISignatureHelper> sh;
    CSignatureHelper::AcquireSingleton((ISignatureHelper**)&sh);
    //test history:
    String algorithm("SHA1withDSA");
    // SHA1withDSA

    Logger::D("SignatureTest", "[TODO] algorithm:%s=====", algorithm.string());
    AutoPtr<ISignature> sig;
    ec = sh->GetInstance(algorithm, (ISignature**)&sig);
    ASSERT_SUCCEEDED(ec);

    sig->InitSign(privKey);
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::AllocInplace(Vector2Data
            , sizeof(Vector2Data)/sizeof(Vector2Data[0]));
    ec = sig->Update(bytes);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > signature;
    ec = sig->Sign((ArrayOf<Byte>**)&signature);
    ASSERT_SUCCEEDED(ec);
    assert(signature != NULL && "Signature must not be null");

    Logger::D("SignatureTest", "Signature match not be tested");

    //Verify
    AutoPtr<ArrayOf<Byte> > dsa_public = GetPublicKeyEncodedByteArray();
    AutoPtr<IX509EncodedKeySpec> x509EncodedKeySpec;
    CX509EncodedKeySpec::New(dsa_public, (IX509EncodedKeySpec**)&x509EncodedKeySpec);
    IKeySpec* keySpec = IKeySpec::Probe(x509EncodedKeySpec);

    AutoPtr<IPublicKey> pubKey;
    ec = kf->GeneratePublic(keySpec, (IPublicKey**)&pubKey);
    ASSERT_SUCCEEDED(ec);
    assert(pubKey != NULL);

    ec = sig->InitVerify(pubKey);
    ASSERT_SUCCEEDED(ec);
    ec = sig->Update(bytes);

    Boolean isVerified = FALSE;
    ec = sig->Verify(signature, &isVerified);
    ASSERT_SUCCEEDED(ec);
    assert("Signature must match expected signature" && isVerified);
    return NOERROR;
}

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos
