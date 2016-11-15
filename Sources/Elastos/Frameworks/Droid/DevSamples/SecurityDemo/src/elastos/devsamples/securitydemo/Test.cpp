
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
using Elastos::Security::IKeyFactory;
using Elastos::Security::IKeyFactoryHelper;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::ISignature;
using Elastos::Security::ISignatureHelper;
using Elastos::Security::Spec::CRSAPrivateKeySpec;
using Elastos::Security::Spec::CRSAPublicKeySpec;
using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::IRSAPrivateKeySpec;
using Elastos::Security::Spec::IRSAPublicKeySpec;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;
using Elastosx::Crypto::Cipher;
using Elastosx::Crypto::ICipher;
using Elastosx::Crypto::ISecretKey;
using Elastosx::Crypto::Spec::CSecretKeySpec;

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

ECode SignatureTest::MD5WithRSA()
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
    AutoPtr<ISignature> sig;
    ec = sh->GetInstance(String("MD5withRSA"), (ISignature**)&sig);
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

    AutoPtr<ArrayOf<Byte> > bytes2 = ArrayOf<Byte>::AllocInplace(MD5withRSA_Vector2Signature
            , sizeof(MD5withRSA_Vector2Signature)/sizeof(MD5withRSA_Vector2Signature[0]));
    Boolean e = Arrays::Equals(signature, bytes2);
    assert("Signature should match expected" && e);

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
    ec = sig->Verify(bytes2, &isVerified);
    ASSERT_SUCCEEDED(ec);
    assert("Signature must match expected signature" && isVerified);
    return NOERROR;
}

ECode CipherTest::AesECBNoPadding() /*throws Exception*/
{
    /*
     * Test vector generation:
     * openssl rand -hex 16
     * echo '3d4f8970b1f27537f40a39298a41555f' | sed 's/\(..\)/(Byte) 0x\1, /g'
     */
    Byte AES_128_KEY[] = {
            (Byte) 0x3d, (Byte) 0x4f, (Byte) 0x89, (Byte) 0x70, (Byte) 0xb1, (Byte) 0xf2,
            (Byte) 0x75, (Byte) 0x37, (Byte) 0xf4, (Byte) 0x0a, (Byte) 0x39, (Byte) 0x29,
            (Byte) 0x8a, (Byte) 0x41, (Byte) 0x55, (Byte) 0x5f,
    };

    AutoPtr<ArrayOf<Byte> > bytes1 = ArrayOf<Byte>::AllocInplace(AES_128_KEY
            , sizeof(AES_128_KEY)/sizeof(AES_128_KEY[0]));

    String provider("ElastosOpenSSL");
    AutoPtr<ISecretKey> key;
    CSecretKeySpec::New(bytes1, String("AES"), (ISecretKey**)&key);
    AutoPtr<ICipher> c;

    //1. AES/ECB/NoPadding
    String transformation("AES/ECB/NoPadding");
    //2. AES/ECB/PKCS5Padding
    // transformation = String("AES/ECB/PKCS5Padding");
    // //3. AES/CBC/NoPadding
    // transformation = String("AES/CBC/NoPadding");
    // //4. AES/CBC/PKCS5Padding
    // transformation = String("AES/CBC/PKCS5Padding");
    // //5. AES/CFB/NoPadding
    // transformation = String("AES/CFB/NoPadding");
    // //6. AES/CTR/NoPadding
    // transformation = String("AES/CTR/NoPadding");
    // //7. AES/OFB/NoPadding
    // transformation = String("AES/OFB/NoPadding");
    // //8. DESEDE/ECB/NoPadding
    // transformation = String("DESEDE/ECB/NoPadding");
    // //9. DESEDE/ECB/PKCS5Padding
    // transformation = String("DESEDE/ECB/PKCS5Padding");
    // //10. DESEDE/CBC/NoPadding
    // transformation = String("DESEDE/CBC/NoPadding");
    // //11. DESEDE/CBC/PKCS5Padding
    // transformation = String("DESEDE/CBC/PKCS5Padding");
    // //12. DESEDE/CFB/NoPadding
    // transformation = String("DESEDE/CFB/NoPadding");
    // //13. DESEDE/OFB/NoPadding
    // transformation = String("DESEDE/OFB/NoPadding");
    // //14. ARC4
    // transformation = String("ARC4");

    ECode ec = Cipher::GetInstance(transformation, provider, (ICipher**)&c);
    ASSERT_SUCCEEDED(ec);
    AutoPtr<IProvider> p;
    c->GetProvider((IProvider**)&p);
    String n;
    p->GetName(&n);
    assert(provider == n);
    c->Init(ICipher::ENCRYPT_MODE, IKey::Probe(key));

    /*
     * Test vector creation:
     * openssl enc -aes-128-ecb -K 3d4f8970b1f27537f40a39298a41555f -in blah|openssl enc -aes-128-ecb -K 3d4f8970b1f27537f40a39298a41555f -nopad -d|recode ../x1 | sed 's/0x/(Byte) 0x/g'
     */
    Byte AES_128_ECB_PKCS5Padding_TestVector_1_Plaintext_Padded[] = {
            (Byte) 0x48, (Byte) 0x65, (Byte) 0x6C, (Byte) 0x6C, (Byte) 0x6F, (Byte) 0x2C,
            (Byte) 0x20, (Byte) 0x77, (Byte) 0x6F, (Byte) 0x72, (Byte) 0x6C, (Byte) 0x64,
            (Byte) 0x21, (Byte) 0x03, (Byte) 0x03, (Byte) 0x03
    };
    Int32 length = sizeof(AES_128_ECB_PKCS5Padding_TestVector_1_Plaintext_Padded) /
        sizeof(AES_128_ECB_PKCS5Padding_TestVector_1_Plaintext_Padded[0]);

    AutoPtr<ArrayOf<Byte> > bytes2 = ArrayOf<Byte>::AllocInplace(AES_128_ECB_PKCS5Padding_TestVector_1_Plaintext_Padded
            , length);

    for (Int32 i = 0; i < length - 1; i++) {
        AutoPtr<ArrayOf<Byte> > outputFragment;
        c->Update(bytes2, i, 1 , (ArrayOf<Byte>**)&outputFragment);
        if (outputFragment != NULL) {
            assert(0 == outputFragment->GetLength());
        }
    }

    AutoPtr<ArrayOf<Byte> > output;
    c->DoFinal(bytes2, length - 1, 1, (ArrayOf<Byte>**)&output);
    assert(output);
    assert(length == output->GetLength());

    /*
     * Test vector generation:
     * openssl enc -aes-128-ecb -K 3d4f8970b1f27537f40a39298a41555f -in blah|recode ../x1 | sed 's/0x/(Byte) 0x/g'
     */
    Byte AES_128_ECB_PKCS5Padding_TestVector_1_Encrypted[] = {
            (Byte) 0x65, (Byte) 0x3E, (Byte) 0x86, (Byte) 0xFB, (Byte) 0x05, (Byte) 0x5A,
            (Byte) 0x52, (Byte) 0xEA, (Byte) 0xDD, (Byte) 0x08, (Byte) 0xE7, (Byte) 0x48,
            (Byte) 0x33, (Byte) 0x01, (Byte) 0xFC, (Byte) 0x5A,
    };

    length = sizeof(AES_128_ECB_PKCS5Padding_TestVector_1_Encrypted) /
        sizeof(AES_128_ECB_PKCS5Padding_TestVector_1_Encrypted[0]);

    AutoPtr<ArrayOf<Byte> > bytes3 = ArrayOf<Byte>::AllocInplace(AES_128_ECB_PKCS5Padding_TestVector_1_Encrypted
            , length);

    assert(Arrays::Equals(bytes3, output));
    return NOERROR;
}

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos
