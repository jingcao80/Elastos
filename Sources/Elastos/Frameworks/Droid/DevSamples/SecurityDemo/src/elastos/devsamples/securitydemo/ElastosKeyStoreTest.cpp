
#include "elastos/devsamples/securitydemo/ElastosKeyStoreTest.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/CoreUtils.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.KeyStore.h"
#include "elastos/utility/logging/Logger.h"
#include <elastos/utility/Arrays.h>
#include "elastos/security/KeyPairGenerator.h"
#include "_Org.Conscrypt.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CoreUtils;
using Elastos::Droid::KeyStore::Security::ICredentials;
using Elastos::Droid::KeyStore::Security::IKeyPairGeneratorSpec;
using Elastos::Droid::KeyStore::Security::IKeyPairGeneratorSpecBuilder;
using Elastos::Droid::KeyStore::Security::CKeyPairGeneratorSpecBuilder;
using Elastos::Droid::KeyStore::Security::IElastosKeyStore;
using Elastos::Droid::KeyStore::Security::IKeyStoreParameter;
using Elastos::Droid::KeyStore::Security::IElastosKeyStoreProvider;
using Elastos::Droid::KeyStore::Security::IKeyStoreParameterBuilder;
using Elastos::Droid::KeyStore::Security::CKeyStoreParameterBuilder;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::Math::CBigInteger;
using Elastos::Math::IBigInteger;
using Elastos::Math::IBigIntegerHelper;
using Elastos::Math::CBigIntegerHelper;
using Elastos::Security::IKey;
using Elastos::Security::IProvider;
using Elastos::Security::IKeyStoreEntry;
using Elastos::Security::IKeyStorePrivateKeyEntry;
using Elastos::Security::CKeyStorePrivateKeyEntry;
using Elastos::Security::KeyPairGenerator;
using Elastos::Security::CSignatureHelper;
using Elastos::Security::IKeyFactory;
using Elastos::Security::CKeyFactoryHelper;
using Elastos::Security::IKeyFactoryHelper;
using Elastos::Security::IPublicKey;
using Elastos::Security::ISignature;
using Elastos::Security::ISignatureHelper;
using Elastos::Security::IKeyStoreProtectionParameter;
using Elastos::Security::IKeyStoreTrustedCertificateEntry;
using Elastos::Security::CKeyStoreTrustedCertificateEntry;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::ICertificateFactoryHelper;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Security::Interfaces::IDSAKey;
using Elastos::Security::Interfaces::IECKey;
using Elastos::Security::Interfaces::IRSAKey;
using Elastos::Security::Interfaces::IDSAParams;
using Elastos::Security::Interfaces::IRSAPrivateKey;
using Elastos::Security::Interfaces::IDSAPrivateKey;
using Elastos::Security::Interfaces::IECPrivateKey;
using Elastos::Security::Spec::CRSAPublicKeySpec;
using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::IEllipticCurve;
using Elastos::Security::Spec::IRSAPublicKeySpec;
using Elastos::Security::Spec::IPKCS8EncodedKeySpec;
using Elastos::Security::Spec::CPKCS8EncodedKeySpec;
using Elastos::Security::Spec::IECParameterSpec;
using Elastos::Security::Spec::IX509EncodedKeySpec;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Elastos::Utility::IDate;
using Elastos::Utility::CDate;
using Elastos::Utility::IList;
using Elastos::Utility::Arrays;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashSet;
using Elastos::Utility::ICollection;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::Logging::Logger;
using Elastosx::Crypto::ICipher;
using Elastosx::Crypto::ISecretKey;
using Elastosx::Crypto::ICipherHelper;
using Elastosx::Crypto::CCipherHelper;
using Elastosx::Crypto::Spec::CSecretKeySpec;
using Elastosx::Crypto::Spec::ISecretKeySpec;
using Elastosx::Security::Auth::X500::CX500Principal;
using Org::Conscrypt::INativeCrypto;
using Org::Conscrypt::IOpenSSLEngine;
using Org::Conscrypt::IOpenSSLEngineHelper;
using Org::Conscrypt::COpenSSLEngineHelper;

namespace Elastos {
namespace DevSamples {
namespace SecurityDemo {

/*
 * The keys and certificates below are generated with:
 *
 * openssl req -new -x509 -days 3650 -extensions v3_ca -keyout cakey.pem -out cacert.pem
 * openssl req -newkey rsa:1024 -keyout userkey.pem -nodes -days 3650 -out userkey.req
 * mkdir -p demoCA/newcerts
 * touch demoCA/index.txt
 * echo "01" > demoCA/serial
 * openssl ca -out usercert.pem -in userkey.req -cert cacert.pem -keyfile cakey.pem -days 3650
 */

/**
 * Generated from above and converted with:
 *
 * openssl x509 -outform d -in cacert.pem | xxd -i | sed 's/0x/(Byte) 0x/g'
 */
static Byte _FAKE_RSA_CA_1[] = {
    (Byte) 0x30, (Byte) 0x82, (Byte) 0x02, (Byte) 0xce, (Byte) 0x30, (Byte) 0x82,
    (Byte) 0x02, (Byte) 0x37, (Byte) 0xa0, (Byte) 0x03, (Byte) 0x02, (Byte) 0x01,
    (Byte) 0x02, (Byte) 0x02, (Byte) 0x09, (Byte) 0x00, (Byte) 0xe1, (Byte) 0x6a,
    (Byte) 0xa2, (Byte) 0xf4, (Byte) 0x2e, (Byte) 0x55, (Byte) 0x48, (Byte) 0x0a,
    (Byte) 0x30, (Byte) 0x0d, (Byte) 0x06, (Byte) 0x09, (Byte) 0x2a, (Byte) 0x86,
    (Byte) 0x48, (Byte) 0x86, (Byte) 0xf7, (Byte) 0x0d, (Byte) 0x01, (Byte) 0x01,
    (Byte) 0x05, (Byte) 0x05, (Byte) 0x00, (Byte) 0x30, (Byte) 0x4f, (Byte) 0x31,
    (Byte) 0x0b, (Byte) 0x30, (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x04, (Byte) 0x06, (Byte) 0x13, (Byte) 0x02, (Byte) 0x55, (Byte) 0x53,
    (Byte) 0x31, (Byte) 0x0b, (Byte) 0x30, (Byte) 0x09, (Byte) 0x06, (Byte) 0x03,
    (Byte) 0x55, (Byte) 0x04, (Byte) 0x08, (Byte) 0x13, (Byte) 0x02, (Byte) 0x43,
    (Byte) 0x41, (Byte) 0x31, (Byte) 0x16, (Byte) 0x30, (Byte) 0x14, (Byte) 0x06,
    (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x07, (Byte) 0x13, (Byte) 0x0d,
    (Byte) 0x4d, (Byte) 0x6f, (Byte) 0x75, (Byte) 0x6e, (Byte) 0x74, (Byte) 0x61,
    (Byte) 0x69, (Byte) 0x6e, (Byte) 0x20, (Byte) 0x56, (Byte) 0x69, (Byte) 0x65,
    (Byte) 0x77, (Byte) 0x31, (Byte) 0x1b, (Byte) 0x30, (Byte) 0x19, (Byte) 0x06,
    (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x0a, (Byte) 0x13, (Byte) 0x12,
    (Byte) 0x41, (Byte) 0x6e, (Byte) 0x64, (Byte) 0x72, (Byte) 0x6f, (Byte) 0x69,
    (Byte) 0x64, (Byte) 0x20, (Byte) 0x54, (Byte) 0x65, (Byte) 0x73, (Byte) 0x74,
    (Byte) 0x20, (Byte) 0x43, (Byte) 0x61, (Byte) 0x73, (Byte) 0x65, (Byte) 0x73,
    (Byte) 0x30, (Byte) 0x1e, (Byte) 0x17, (Byte) 0x0d, (Byte) 0x31, (Byte) 0x32,
    (Byte) 0x30, (Byte) 0x38, (Byte) 0x31, (Byte) 0x34, (Byte) 0x31, (Byte) 0x36,
    (Byte) 0x35, (Byte) 0x35, (Byte) 0x34, (Byte) 0x34, (Byte) 0x5a, (Byte) 0x17,
    (Byte) 0x0d, (Byte) 0x32, (Byte) 0x32, (Byte) 0x30, (Byte) 0x38, (Byte) 0x31,
    (Byte) 0x32, (Byte) 0x31, (Byte) 0x36, (Byte) 0x35, (Byte) 0x35, (Byte) 0x34,
    (Byte) 0x34, (Byte) 0x5a, (Byte) 0x30, (Byte) 0x4f, (Byte) 0x31, (Byte) 0x0b,
    (Byte) 0x30, (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04,
    (Byte) 0x06, (Byte) 0x13, (Byte) 0x02, (Byte) 0x55, (Byte) 0x53, (Byte) 0x31,
    (Byte) 0x0b, (Byte) 0x30, (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x04, (Byte) 0x08, (Byte) 0x13, (Byte) 0x02, (Byte) 0x43, (Byte) 0x41,
    (Byte) 0x31, (Byte) 0x16, (Byte) 0x30, (Byte) 0x14, (Byte) 0x06, (Byte) 0x03,
    (Byte) 0x55, (Byte) 0x04, (Byte) 0x07, (Byte) 0x13, (Byte) 0x0d, (Byte) 0x4d,
    (Byte) 0x6f, (Byte) 0x75, (Byte) 0x6e, (Byte) 0x74, (Byte) 0x61, (Byte) 0x69,
    (Byte) 0x6e, (Byte) 0x20, (Byte) 0x56, (Byte) 0x69, (Byte) 0x65, (Byte) 0x77,
    (Byte) 0x31, (Byte) 0x1b, (Byte) 0x30, (Byte) 0x19, (Byte) 0x06, (Byte) 0x03,
    (Byte) 0x55, (Byte) 0x04, (Byte) 0x0a, (Byte) 0x13, (Byte) 0x12, (Byte) 0x41,
    (Byte) 0x6e, (Byte) 0x64, (Byte) 0x72, (Byte) 0x6f, (Byte) 0x69, (Byte) 0x64,
    (Byte) 0x20, (Byte) 0x54, (Byte) 0x65, (Byte) 0x73, (Byte) 0x74, (Byte) 0x20,
    (Byte) 0x43, (Byte) 0x61, (Byte) 0x73, (Byte) 0x65, (Byte) 0x73, (Byte) 0x30,
    (Byte) 0x81, (Byte) 0x9f, (Byte) 0x30, (Byte) 0x0d, (Byte) 0x06, (Byte) 0x09,
    (Byte) 0x2a, (Byte) 0x86, (Byte) 0x48, (Byte) 0x86, (Byte) 0xf7, (Byte) 0x0d,
    (Byte) 0x01, (Byte) 0x01, (Byte) 0x01, (Byte) 0x05, (Byte) 0x00, (Byte) 0x03,
    (Byte) 0x81, (Byte) 0x8d, (Byte) 0x00, (Byte) 0x30, (Byte) 0x81, (Byte) 0x89,
    (Byte) 0x02, (Byte) 0x81, (Byte) 0x81, (Byte) 0x00, (Byte) 0xa3, (Byte) 0x72,
    (Byte) 0xab, (Byte) 0xd0, (Byte) 0xe4, (Byte) 0xad, (Byte) 0x2f, (Byte) 0xe7,
    (Byte) 0xe2, (Byte) 0x79, (Byte) 0x07, (Byte) 0x36, (Byte) 0x3d, (Byte) 0x0c,
    (Byte) 0x8d, (Byte) 0x42, (Byte) 0x9a, (Byte) 0x0a, (Byte) 0x33, (Byte) 0x64,
    (Byte) 0xb3, (Byte) 0xcd, (Byte) 0xb2, (Byte) 0xd7, (Byte) 0x3a, (Byte) 0x42,
    (Byte) 0x06, (Byte) 0x77, (Byte) 0x45, (Byte) 0x29, (Byte) 0xe9, (Byte) 0xcb,
    (Byte) 0xb7, (Byte) 0x4a, (Byte) 0xd6, (Byte) 0xee, (Byte) 0xad, (Byte) 0x01,
    (Byte) 0x91, (Byte) 0x9b, (Byte) 0x0c, (Byte) 0x59, (Byte) 0xa1, (Byte) 0x03,
    (Byte) 0xfa, (Byte) 0xf0, (Byte) 0x5a, (Byte) 0x7c, (Byte) 0x4f, (Byte) 0xf7,
    (Byte) 0x8d, (Byte) 0x36, (Byte) 0x0f, (Byte) 0x1f, (Byte) 0x45, (Byte) 0x7d,
    (Byte) 0x1b, (Byte) 0x31, (Byte) 0xa1, (Byte) 0x35, (Byte) 0x0b, (Byte) 0x00,
    (Byte) 0xed, (Byte) 0x7a, (Byte) 0xb6, (Byte) 0xc8, (Byte) 0x4e, (Byte) 0xa9,
    (Byte) 0x86, (Byte) 0x4c, (Byte) 0x7b, (Byte) 0x99, (Byte) 0x57, (Byte) 0x41,
    (Byte) 0x12, (Byte) 0xef, (Byte) 0x6b, (Byte) 0xbc, (Byte) 0x3d, (Byte) 0x60,
    (Byte) 0xf2, (Byte) 0x99, (Byte) 0x1a, (Byte) 0xcd, (Byte) 0xed, (Byte) 0x56,
    (Byte) 0xa4, (Byte) 0xe5, (Byte) 0x36, (Byte) 0x9f, (Byte) 0x24, (Byte) 0x1f,
    (Byte) 0xdc, (Byte) 0x89, (Byte) 0x40, (Byte) 0xc8, (Byte) 0x99, (Byte) 0x92,
    (Byte) 0xab, (Byte) 0x4a, (Byte) 0xb5, (Byte) 0x61, (Byte) 0x45, (Byte) 0x62,
    (Byte) 0xff, (Byte) 0xa3, (Byte) 0x45, (Byte) 0x65, (Byte) 0xaf, (Byte) 0xf6,
    (Byte) 0x27, (Byte) 0x30, (Byte) 0x51, (Byte) 0x0e, (Byte) 0x0e, (Byte) 0xeb,
    (Byte) 0x79, (Byte) 0x0c, (Byte) 0xbe, (Byte) 0xb3, (Byte) 0x0a, (Byte) 0x6f,
    (Byte) 0x29, (Byte) 0x06, (Byte) 0xdc, (Byte) 0x2f, (Byte) 0x6b, (Byte) 0x51,
    (Byte) 0x02, (Byte) 0x03, (Byte) 0x01, (Byte) 0x00, (Byte) 0x01, (Byte) 0xa3,
    (Byte) 0x81, (Byte) 0xb1, (Byte) 0x30, (Byte) 0x81, (Byte) 0xae, (Byte) 0x30,
    (Byte) 0x1d, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x1d, (Byte) 0x0e,
    (Byte) 0x04, (Byte) 0x16, (Byte) 0x04, (Byte) 0x14, (Byte) 0x33, (Byte) 0x05,
    (Byte) 0xee, (Byte) 0xfe, (Byte) 0x6f, (Byte) 0x60, (Byte) 0xc7, (Byte) 0xf9,
    (Byte) 0xa9, (Byte) 0xd2, (Byte) 0x73, (Byte) 0x5c, (Byte) 0x8f, (Byte) 0x6d,
    (Byte) 0xa2, (Byte) 0x2f, (Byte) 0x97, (Byte) 0x8e, (Byte) 0x5d, (Byte) 0x51,
    (Byte) 0x30, (Byte) 0x7f, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x1d,
    (Byte) 0x23, (Byte) 0x04, (Byte) 0x78, (Byte) 0x30, (Byte) 0x76, (Byte) 0x80,
    (Byte) 0x14, (Byte) 0x33, (Byte) 0x05, (Byte) 0xee, (Byte) 0xfe, (Byte) 0x6f,
    (Byte) 0x60, (Byte) 0xc7, (Byte) 0xf9, (Byte) 0xa9, (Byte) 0xd2, (Byte) 0x73,
    (Byte) 0x5c, (Byte) 0x8f, (Byte) 0x6d, (Byte) 0xa2, (Byte) 0x2f, (Byte) 0x97,
    (Byte) 0x8e, (Byte) 0x5d, (Byte) 0x51, (Byte) 0xa1, (Byte) 0x53, (Byte) 0xa4,
    (Byte) 0x51, (Byte) 0x30, (Byte) 0x4f, (Byte) 0x31, (Byte) 0x0b, (Byte) 0x30,
    (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x06,
    (Byte) 0x13, (Byte) 0x02, (Byte) 0x55, (Byte) 0x53, (Byte) 0x31, (Byte) 0x0b,
    (Byte) 0x30, (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04,
    (Byte) 0x08, (Byte) 0x13, (Byte) 0x02, (Byte) 0x43, (Byte) 0x41, (Byte) 0x31,
    (Byte) 0x16, (Byte) 0x30, (Byte) 0x14, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x04, (Byte) 0x07, (Byte) 0x13, (Byte) 0x0d, (Byte) 0x4d, (Byte) 0x6f,
    (Byte) 0x75, (Byte) 0x6e, (Byte) 0x74, (Byte) 0x61, (Byte) 0x69, (Byte) 0x6e,
    (Byte) 0x20, (Byte) 0x56, (Byte) 0x69, (Byte) 0x65, (Byte) 0x77, (Byte) 0x31,
    (Byte) 0x1b, (Byte) 0x30, (Byte) 0x19, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x04, (Byte) 0x0a, (Byte) 0x13, (Byte) 0x12, (Byte) 0x41, (Byte) 0x6e,
    (Byte) 0x64, (Byte) 0x72, (Byte) 0x6f, (Byte) 0x69, (Byte) 0x64, (Byte) 0x20,
    (Byte) 0x54, (Byte) 0x65, (Byte) 0x73, (Byte) 0x74, (Byte) 0x20, (Byte) 0x43,
    (Byte) 0x61, (Byte) 0x73, (Byte) 0x65, (Byte) 0x73, (Byte) 0x82, (Byte) 0x09,
    (Byte) 0x00, (Byte) 0xe1, (Byte) 0x6a, (Byte) 0xa2, (Byte) 0xf4, (Byte) 0x2e,
    (Byte) 0x55, (Byte) 0x48, (Byte) 0x0a, (Byte) 0x30, (Byte) 0x0c, (Byte) 0x06,
    (Byte) 0x03, (Byte) 0x55, (Byte) 0x1d, (Byte) 0x13, (Byte) 0x04, (Byte) 0x05,
    (Byte) 0x30, (Byte) 0x03, (Byte) 0x01, (Byte) 0x01, (Byte) 0xff, (Byte) 0x30,
    (Byte) 0x0d, (Byte) 0x06, (Byte) 0x09, (Byte) 0x2a, (Byte) 0x86, (Byte) 0x48,
    (Byte) 0x86, (Byte) 0xf7, (Byte) 0x0d, (Byte) 0x01, (Byte) 0x01, (Byte) 0x05,
    (Byte) 0x05, (Byte) 0x00, (Byte) 0x03, (Byte) 0x81, (Byte) 0x81, (Byte) 0x00,
    (Byte) 0x8c, (Byte) 0x30, (Byte) 0x42, (Byte) 0xfa, (Byte) 0xeb, (Byte) 0x1a,
    (Byte) 0x26, (Byte) 0xeb, (Byte) 0xda, (Byte) 0x56, (Byte) 0x32, (Byte) 0xf2,
    (Byte) 0x9d, (Byte) 0xa5, (Byte) 0x24, (Byte) 0xd8, (Byte) 0x3a, (Byte) 0xda,
    (Byte) 0x30, (Byte) 0xa6, (Byte) 0x8b, (Byte) 0x46, (Byte) 0xfe, (Byte) 0xfe,
    (Byte) 0xdb, (Byte) 0xf1, (Byte) 0xe6, (Byte) 0xe1, (Byte) 0x7c, (Byte) 0x1b,
    (Byte) 0xe7, (Byte) 0x77, (Byte) 0x00, (Byte) 0xa1, (Byte) 0x1c, (Byte) 0x19,
    (Byte) 0x17, (Byte) 0x73, (Byte) 0xb0, (Byte) 0xf0, (Byte) 0x9d, (Byte) 0xf3,
    (Byte) 0x4f, (Byte) 0xb6, (Byte) 0xbc, (Byte) 0xc7, (Byte) 0x47, (Byte) 0x85,
    (Byte) 0x2a, (Byte) 0x4a, (Byte) 0xa1, (Byte) 0xa5, (Byte) 0x58, (Byte) 0xf5,
    (Byte) 0xc5, (Byte) 0x1a, (Byte) 0x51, (Byte) 0xb1, (Byte) 0x04, (Byte) 0x80,
    (Byte) 0xee, (Byte) 0x3a, (Byte) 0xec, (Byte) 0x2f, (Byte) 0xe1, (Byte) 0xfd,
    (Byte) 0x58, (Byte) 0xeb, (Byte) 0xed, (Byte) 0x82, (Byte) 0x9e, (Byte) 0x38,
    (Byte) 0xa3, (Byte) 0x24, (Byte) 0x75, (Byte) 0xf7, (Byte) 0x3e, (Byte) 0xc2,
    (Byte) 0xc5, (Byte) 0x27, (Byte) 0xeb, (Byte) 0x6f, (Byte) 0x7b, (Byte) 0x50,
    (Byte) 0xda, (Byte) 0x43, (Byte) 0xdc, (Byte) 0x3b, (Byte) 0x0b, (Byte) 0x6f,
    (Byte) 0x78, (Byte) 0x8f, (Byte) 0xb0, (Byte) 0x66, (Byte) 0xe1, (Byte) 0x12,
    (Byte) 0x87, (Byte) 0x5f, (Byte) 0x97, (Byte) 0x7b, (Byte) 0xca, (Byte) 0x14,
    (Byte) 0x79, (Byte) 0xf7, (Byte) 0xe8, (Byte) 0x6c, (Byte) 0x72, (Byte) 0xdb,
    (Byte) 0x91, (Byte) 0x65, (Byte) 0x17, (Byte) 0x54, (Byte) 0xe0, (Byte) 0x74,
    (Byte) 0x1d, (Byte) 0xac, (Byte) 0x47, (Byte) 0x04, (Byte) 0x12, (Byte) 0xe0,
    (Byte) 0xc3, (Byte) 0x66, (Byte) 0x19, (Byte) 0x05, (Byte) 0x2e, (Byte) 0x7e,
    (Byte) 0xf1, (Byte) 0x61
};

static AutoPtr<ArrayOf<Byte> > FAKE_RSA_CA_1 = ArrayOf<Byte>::AllocInplace(_FAKE_RSA_CA_1
        , ArraySize(_FAKE_RSA_CA_1));

/**
 * Generated from above and converted with:
 *
 * openssl pkcs8 -topk8 -outform d -in userkey.pem -nocrypt | xxd -i | sed 's/0x/(Byte) 0x/g'
 */
static Byte _FAKE_RSA_KEY_1[] = {
    (Byte) 0x30, (Byte) 0x82, (Byte) 0x02, (Byte) 0x78, (Byte) 0x02, (Byte) 0x01,
    (Byte) 0x00, (Byte) 0x30, (Byte) 0x0d, (Byte) 0x06, (Byte) 0x09, (Byte) 0x2a,
    (Byte) 0x86, (Byte) 0x48, (Byte) 0x86, (Byte) 0xf7, (Byte) 0x0d, (Byte) 0x01,
    (Byte) 0x01, (Byte) 0x01, (Byte) 0x05, (Byte) 0x00, (Byte) 0x04, (Byte) 0x82,
    (Byte) 0x02, (Byte) 0x62, (Byte) 0x30, (Byte) 0x82, (Byte) 0x02, (Byte) 0x5e,
    (Byte) 0x02, (Byte) 0x01, (Byte) 0x00, (Byte) 0x02, (Byte) 0x81, (Byte) 0x81,
    (Byte) 0x00, (Byte) 0xce, (Byte) 0x29, (Byte) 0xeb, (Byte) 0xf6, (Byte) 0x5b,
    (Byte) 0x25, (Byte) 0xdc, (Byte) 0xa1, (Byte) 0xa6, (Byte) 0x2c, (Byte) 0x66,
    (Byte) 0xcb, (Byte) 0x20, (Byte) 0x90, (Byte) 0x27, (Byte) 0x86, (Byte) 0x8a,
    (Byte) 0x44, (Byte) 0x71, (Byte) 0x50, (Byte) 0xda, (Byte) 0xd3, (Byte) 0x02,
    (Byte) 0x77, (Byte) 0x55, (Byte) 0xe9, (Byte) 0xe8, (Byte) 0x08, (Byte) 0xf3,
    (Byte) 0x36, (Byte) 0x9a, (Byte) 0xae, (Byte) 0xab, (Byte) 0x04, (Byte) 0x6d,
    (Byte) 0x00, (Byte) 0x99, (Byte) 0xbf, (Byte) 0x7d, (Byte) 0x0f, (Byte) 0x67,
    (Byte) 0x8b, (Byte) 0x1d, (Byte) 0xd4, (Byte) 0x2b, (Byte) 0x7c, (Byte) 0xcb,
    (Byte) 0xcd, (Byte) 0x33, (Byte) 0xc7, (Byte) 0x84, (Byte) 0x30, (Byte) 0xe2,
    (Byte) 0x45, (Byte) 0x21, (Byte) 0xb3, (Byte) 0x75, (Byte) 0xf5, (Byte) 0x79,
    (Byte) 0x02, (Byte) 0xda, (Byte) 0x50, (Byte) 0xa3, (Byte) 0x8b, (Byte) 0xce,
    (Byte) 0xc3, (Byte) 0x8e, (Byte) 0x0f, (Byte) 0x25, (Byte) 0xeb, (Byte) 0x08,
    (Byte) 0x2c, (Byte) 0xdd, (Byte) 0x1c, (Byte) 0xcf, (Byte) 0xff, (Byte) 0x3b,
    (Byte) 0xde, (Byte) 0xb6, (Byte) 0xaa, (Byte) 0x2a, (Byte) 0xa9, (Byte) 0xc4,
    (Byte) 0x8a, (Byte) 0x24, (Byte) 0x24, (Byte) 0xe6, (Byte) 0x29, (Byte) 0x0d,
    (Byte) 0x98, (Byte) 0x4c, (Byte) 0x32, (Byte) 0xa1, (Byte) 0x7b, (Byte) 0x23,
    (Byte) 0x2b, (Byte) 0x42, (Byte) 0x30, (Byte) 0xee, (Byte) 0x78, (Byte) 0x08,
    (Byte) 0x47, (Byte) 0xad, (Byte) 0xf2, (Byte) 0x96, (Byte) 0xd5, (Byte) 0xf1,
    (Byte) 0x62, (Byte) 0x42, (Byte) 0x2d, (Byte) 0x35, (Byte) 0x19, (Byte) 0xb4,
    (Byte) 0x3c, (Byte) 0xc9, (Byte) 0xc3, (Byte) 0x5f, (Byte) 0x03, (Byte) 0x16,
    (Byte) 0x3a, (Byte) 0x23, (Byte) 0xac, (Byte) 0xcb, (Byte) 0xce, (Byte) 0x9e,
    (Byte) 0x51, (Byte) 0x2e, (Byte) 0x6d, (Byte) 0x02, (Byte) 0x03, (Byte) 0x01,
    (Byte) 0x00, (Byte) 0x01, (Byte) 0x02, (Byte) 0x81, (Byte) 0x80, (Byte) 0x16,
    (Byte) 0x59, (Byte) 0xc3, (Byte) 0x24, (Byte) 0x1d, (Byte) 0x33, (Byte) 0x98,
    (Byte) 0x9c, (Byte) 0xc9, (Byte) 0xc8, (Byte) 0x2c, (Byte) 0x88, (Byte) 0xbf,
    (Byte) 0x0a, (Byte) 0x01, (Byte) 0xce, (Byte) 0xfb, (Byte) 0x34, (Byte) 0x7a,
    (Byte) 0x58, (Byte) 0x7a, (Byte) 0xb0, (Byte) 0xbf, (Byte) 0xa6, (Byte) 0xb2,
    (Byte) 0x60, (Byte) 0xbe, (Byte) 0x70, (Byte) 0x21, (Byte) 0xf5, (Byte) 0xfc,
    (Byte) 0x85, (Byte) 0x0d, (Byte) 0x33, (Byte) 0x58, (Byte) 0xa1, (Byte) 0xe5,
    (Byte) 0x09, (Byte) 0x36, (Byte) 0x84, (Byte) 0xb2, (Byte) 0x04, (Byte) 0x0a,
    (Byte) 0x02, (Byte) 0xd3, (Byte) 0x88, (Byte) 0x1f, (Byte) 0x0c, (Byte) 0x2b,
    (Byte) 0x1d, (Byte) 0xe9, (Byte) 0x3d, (Byte) 0xe7, (Byte) 0x79, (Byte) 0xf9,
    (Byte) 0x32, (Byte) 0x5c, (Byte) 0x8a, (Byte) 0x75, (Byte) 0x49, (Byte) 0x12,
    (Byte) 0xe4, (Byte) 0x05, (Byte) 0x26, (Byte) 0xd4, (Byte) 0x2e, (Byte) 0x9e,
    (Byte) 0x1f, (Byte) 0xcc, (Byte) 0x54, (Byte) 0xad, (Byte) 0x33, (Byte) 0x8d,
    (Byte) 0x99, (Byte) 0x00, (Byte) 0xdc, (Byte) 0xf5, (Byte) 0xb4, (Byte) 0xa2,
    (Byte) 0x2f, (Byte) 0xba, (Byte) 0xe5, (Byte) 0x62, (Byte) 0x30, (Byte) 0x6d,
    (Byte) 0xe6, (Byte) 0x3d, (Byte) 0xeb, (Byte) 0x24, (Byte) 0xc2, (Byte) 0xdc,
    (Byte) 0x5f, (Byte) 0xb7, (Byte) 0x16, (Byte) 0x35, (Byte) 0xa3, (Byte) 0x98,
    (Byte) 0x98, (Byte) 0xa8, (Byte) 0xef, (Byte) 0xe8, (Byte) 0xc4, (Byte) 0x96,
    (Byte) 0x6d, (Byte) 0x38, (Byte) 0xab, (Byte) 0x26, (Byte) 0x6d, (Byte) 0x30,
    (Byte) 0xc2, (Byte) 0xa0, (Byte) 0x44, (Byte) 0xe4, (Byte) 0xff, (Byte) 0x7e,
    (Byte) 0xbe, (Byte) 0x7c, (Byte) 0x33, (Byte) 0xa5, (Byte) 0x10, (Byte) 0xad,
    (Byte) 0xd7, (Byte) 0x1e, (Byte) 0x13, (Byte) 0x20, (Byte) 0xb3, (Byte) 0x1f,
    (Byte) 0x41, (Byte) 0x02, (Byte) 0x41, (Byte) 0x00, (Byte) 0xf1, (Byte) 0x89,
    (Byte) 0x07, (Byte) 0x0f, (Byte) 0xe8, (Byte) 0xcf, (Byte) 0xab, (Byte) 0x13,
    (Byte) 0x2a, (Byte) 0x8f, (Byte) 0x88, (Byte) 0x80, (Byte) 0x11, (Byte) 0x9a,
    (Byte) 0x79, (Byte) 0xb6, (Byte) 0x59, (Byte) 0x3a, (Byte) 0x50, (Byte) 0x6e,
    (Byte) 0x57, (Byte) 0x37, (Byte) 0xab, (Byte) 0x2a, (Byte) 0xd2, (Byte) 0xaa,
    (Byte) 0xd9, (Byte) 0x72, (Byte) 0x73, (Byte) 0xff, (Byte) 0x8b, (Byte) 0x47,
    (Byte) 0x76, (Byte) 0xdd, (Byte) 0xdc, (Byte) 0xf5, (Byte) 0x97, (Byte) 0x44,
    (Byte) 0x3a, (Byte) 0x78, (Byte) 0xbe, (Byte) 0x17, (Byte) 0xb4, (Byte) 0x22,
    (Byte) 0x6f, (Byte) 0xe5, (Byte) 0x23, (Byte) 0x70, (Byte) 0x1d, (Byte) 0x10,
    (Byte) 0x5d, (Byte) 0xba, (Byte) 0x16, (Byte) 0x81, (Byte) 0xf1, (Byte) 0x45,
    (Byte) 0xce, (Byte) 0x30, (Byte) 0xb4, (Byte) 0xab, (Byte) 0x80, (Byte) 0xe4,
    (Byte) 0x98, (Byte) 0x31, (Byte) 0x02, (Byte) 0x41, (Byte) 0x00, (Byte) 0xda,
    (Byte) 0x82, (Byte) 0x9d, (Byte) 0x3f, (Byte) 0xca, (Byte) 0x2f, (Byte) 0xe1,
    (Byte) 0xd4, (Byte) 0x86, (Byte) 0x77, (Byte) 0x48, (Byte) 0xa6, (Byte) 0xab,
    (Byte) 0xab, (Byte) 0x1c, (Byte) 0x42, (Byte) 0x5c, (Byte) 0xd5, (Byte) 0xc7,
    (Byte) 0x46, (Byte) 0x59, (Byte) 0x91, (Byte) 0x3f, (Byte) 0xfc, (Byte) 0xcc,
    (Byte) 0xec, (Byte) 0xc2, (Byte) 0x40, (Byte) 0x12, (Byte) 0x2c, (Byte) 0x8d,
    (Byte) 0x1f, (Byte) 0xa2, (Byte) 0x18, (Byte) 0x88, (Byte) 0xee, (Byte) 0x82,
    (Byte) 0x4a, (Byte) 0x5a, (Byte) 0x5e, (Byte) 0x88, (Byte) 0x20, (Byte) 0xe3,
    (Byte) 0x7b, (Byte) 0xe0, (Byte) 0xd8, (Byte) 0x3a, (Byte) 0x52, (Byte) 0x9a,
    (Byte) 0x26, (Byte) 0x6a, (Byte) 0x04, (Byte) 0xec, (Byte) 0xe8, (Byte) 0xb9,
    (Byte) 0x48, (Byte) 0x40, (Byte) 0xe1, (Byte) 0xe1, (Byte) 0x83, (Byte) 0xa6,
    (Byte) 0x67, (Byte) 0xa6, (Byte) 0xfd, (Byte) 0x02, (Byte) 0x41, (Byte) 0x00,
    (Byte) 0x89, (Byte) 0x72, (Byte) 0x3e, (Byte) 0xb0, (Byte) 0x90, (Byte) 0xfd,
    (Byte) 0x4c, (Byte) 0x0e, (Byte) 0xd6, (Byte) 0x13, (Byte) 0x63, (Byte) 0xcb,
    (Byte) 0xed, (Byte) 0x38, (Byte) 0x88, (Byte) 0xb6, (Byte) 0x79, (Byte) 0xc4,
    (Byte) 0x33, (Byte) 0x6c, (Byte) 0xf6, (Byte) 0xf8, (Byte) 0xd8, (Byte) 0xd0,
    (Byte) 0xbf, (Byte) 0x9d, (Byte) 0x35, (Byte) 0xac, (Byte) 0x69, (Byte) 0xd2,
    (Byte) 0x2b, (Byte) 0xc1, (Byte) 0xf9, (Byte) 0x24, (Byte) 0x7b, (Byte) 0xce,
    (Byte) 0xcd, (Byte) 0xcb, (Byte) 0xa7, (Byte) 0xb2, (Byte) 0x7a, (Byte) 0x0a,
    (Byte) 0x27, (Byte) 0x19, (Byte) 0xc9, (Byte) 0xaf, (Byte) 0x0d, (Byte) 0x21,
    (Byte) 0x89, (Byte) 0x88, (Byte) 0x7c, (Byte) 0xad, (Byte) 0x9e, (Byte) 0x8d,
    (Byte) 0x47, (Byte) 0x6d, (Byte) 0x3f, (Byte) 0xce, (Byte) 0x7b, (Byte) 0xa1,
    (Byte) 0x74, (Byte) 0xf1, (Byte) 0xa0, (Byte) 0xa1, (Byte) 0x02, (Byte) 0x41,
    (Byte) 0x00, (Byte) 0xd9, (Byte) 0xa8, (Byte) 0xf5, (Byte) 0xfe, (Byte) 0xce,
    (Byte) 0xe6, (Byte) 0x77, (Byte) 0x6b, (Byte) 0xfe, (Byte) 0x2d, (Byte) 0xe0,
    (Byte) 0x1e, (Byte) 0xb6, (Byte) 0x2e, (Byte) 0x12, (Byte) 0x4e, (Byte) 0x40,
    (Byte) 0xaf, (Byte) 0x6a, (Byte) 0x7b, (Byte) 0x37, (Byte) 0x49, (Byte) 0x2a,
    (Byte) 0x96, (Byte) 0x25, (Byte) 0x83, (Byte) 0x49, (Byte) 0xd4, (Byte) 0x0c,
    (Byte) 0xc6, (Byte) 0x78, (Byte) 0x25, (Byte) 0x24, (Byte) 0x90, (Byte) 0x90,
    (Byte) 0x06, (Byte) 0x15, (Byte) 0x9e, (Byte) 0xfe, (Byte) 0xf9, (Byte) 0xdf,
    (Byte) 0x5b, (Byte) 0xf3, (Byte) 0x7e, (Byte) 0x38, (Byte) 0x70, (Byte) 0xeb,
    (Byte) 0x57, (Byte) 0xd0, (Byte) 0xd9, (Byte) 0xa7, (Byte) 0x0e, (Byte) 0x14,
    (Byte) 0xf7, (Byte) 0x95, (Byte) 0x68, (Byte) 0xd5, (Byte) 0xc8, (Byte) 0xab,
    (Byte) 0x9d, (Byte) 0x3a, (Byte) 0x2b, (Byte) 0x51, (Byte) 0xf9, (Byte) 0x02,
    (Byte) 0x41, (Byte) 0x00, (Byte) 0x96, (Byte) 0xdf, (Byte) 0xe9, (Byte) 0x67,
    (Byte) 0x6c, (Byte) 0xdc, (Byte) 0x90, (Byte) 0x14, (Byte) 0xb4, (Byte) 0x1d,
    (Byte) 0x22, (Byte) 0x33, (Byte) 0x4a, (Byte) 0x31, (Byte) 0xc1, (Byte) 0x9d,
    (Byte) 0x2e, (Byte) 0xff, (Byte) 0x9a, (Byte) 0x2a, (Byte) 0x95, (Byte) 0x4b,
    (Byte) 0x27, (Byte) 0x74, (Byte) 0xcb, (Byte) 0x21, (Byte) 0xc3, (Byte) 0xd2,
    (Byte) 0x0b, (Byte) 0xb2, (Byte) 0x46, (Byte) 0x87, (Byte) 0xf8, (Byte) 0x28,
    (Byte) 0x01, (Byte) 0x8b, (Byte) 0xd8, (Byte) 0xb9, (Byte) 0x4b, (Byte) 0xcd,
    (Byte) 0x9a, (Byte) 0x96, (Byte) 0x41, (Byte) 0x0e, (Byte) 0x36, (Byte) 0x6d,
    (Byte) 0x40, (Byte) 0x42, (Byte) 0xbc, (Byte) 0xd9, (Byte) 0xd3, (Byte) 0x7b,
    (Byte) 0xbc, (Byte) 0xa7, (Byte) 0x92, (Byte) 0x90, (Byte) 0xdd, (Byte) 0xa1,
    (Byte) 0x9c, (Byte) 0xce, (Byte) 0xa1, (Byte) 0x87, (Byte) 0x11, (Byte) 0x51
};

static AutoPtr<ArrayOf<Byte> > FAKE_RSA_KEY_1 = ArrayOf<Byte>::AllocInplace(_FAKE_RSA_KEY_1
        , ArraySize(_FAKE_RSA_KEY_1));

/**
 * Generated from above and converted with:
 *
 * openssl x509 -outform d -in usercert.pem | xxd -i | sed 's/0x/(Byte) 0x/g'
 */
static Byte _FAKE_RSA_USER_1[] = {
    (Byte) 0x30, (Byte) 0x82, (Byte) 0x02, (Byte) 0x95, (Byte) 0x30, (Byte) 0x82,
    (Byte) 0x01, (Byte) 0xfe, (Byte) 0xa0, (Byte) 0x03, (Byte) 0x02, (Byte) 0x01,
    (Byte) 0x02, (Byte) 0x02, (Byte) 0x01, (Byte) 0x01, (Byte) 0x30, (Byte) 0x0d,
    (Byte) 0x06, (Byte) 0x09, (Byte) 0x2a, (Byte) 0x86, (Byte) 0x48, (Byte) 0x86,
    (Byte) 0xf7, (Byte) 0x0d, (Byte) 0x01, (Byte) 0x01, (Byte) 0x05, (Byte) 0x05,
    (Byte) 0x00, (Byte) 0x30, (Byte) 0x4f, (Byte) 0x31, (Byte) 0x0b, (Byte) 0x30,
    (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x06,
    (Byte) 0x13, (Byte) 0x02, (Byte) 0x55, (Byte) 0x53, (Byte) 0x31, (Byte) 0x0b,
    (Byte) 0x30, (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04,
    (Byte) 0x08, (Byte) 0x13, (Byte) 0x02, (Byte) 0x43, (Byte) 0x41, (Byte) 0x31,
    (Byte) 0x16, (Byte) 0x30, (Byte) 0x14, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x04, (Byte) 0x07, (Byte) 0x13, (Byte) 0x0d, (Byte) 0x4d, (Byte) 0x6f,
    (Byte) 0x75, (Byte) 0x6e, (Byte) 0x74, (Byte) 0x61, (Byte) 0x69, (Byte) 0x6e,
    (Byte) 0x20, (Byte) 0x56, (Byte) 0x69, (Byte) 0x65, (Byte) 0x77, (Byte) 0x31,
    (Byte) 0x1b, (Byte) 0x30, (Byte) 0x19, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x04, (Byte) 0x0a, (Byte) 0x13, (Byte) 0x12, (Byte) 0x41, (Byte) 0x6e,
    (Byte) 0x64, (Byte) 0x72, (Byte) 0x6f, (Byte) 0x69, (Byte) 0x64, (Byte) 0x20,
    (Byte) 0x54, (Byte) 0x65, (Byte) 0x73, (Byte) 0x74, (Byte) 0x20, (Byte) 0x43,
    (Byte) 0x61, (Byte) 0x73, (Byte) 0x65, (Byte) 0x73, (Byte) 0x30, (Byte) 0x1e,
    (Byte) 0x17, (Byte) 0x0d, (Byte) 0x31, (Byte) 0x32, (Byte) 0x30, (Byte) 0x38,
    (Byte) 0x31, (Byte) 0x34, (Byte) 0x32, (Byte) 0x33, (Byte) 0x32, (Byte) 0x35,
    (Byte) 0x34, (Byte) 0x38, (Byte) 0x5a, (Byte) 0x17, (Byte) 0x0d, (Byte) 0x32,
    (Byte) 0x32, (Byte) 0x30, (Byte) 0x38, (Byte) 0x31, (Byte) 0x32, (Byte) 0x32,
    (Byte) 0x33, (Byte) 0x32, (Byte) 0x35, (Byte) 0x34, (Byte) 0x38, (Byte) 0x5a,
    (Byte) 0x30, (Byte) 0x55, (Byte) 0x31, (Byte) 0x0b, (Byte) 0x30, (Byte) 0x09,
    (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x06, (Byte) 0x13,
    (Byte) 0x02, (Byte) 0x55, (Byte) 0x53, (Byte) 0x31, (Byte) 0x0b, (Byte) 0x30,
    (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x08,
    (Byte) 0x13, (Byte) 0x02, (Byte) 0x43, (Byte) 0x41, (Byte) 0x31, (Byte) 0x1b,
    (Byte) 0x30, (Byte) 0x19, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04,
    (Byte) 0x0a, (Byte) 0x13, (Byte) 0x12, (Byte) 0x41, (Byte) 0x6e, (Byte) 0x64,
    (Byte) 0x72, (Byte) 0x6f, (Byte) 0x69, (Byte) 0x64, (Byte) 0x20, (Byte) 0x54,
    (Byte) 0x65, (Byte) 0x73, (Byte) 0x74, (Byte) 0x20, (Byte) 0x43, (Byte) 0x61,
    (Byte) 0x73, (Byte) 0x65, (Byte) 0x73, (Byte) 0x31, (Byte) 0x1c, (Byte) 0x30,
    (Byte) 0x1a, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x03,
    (Byte) 0x13, (Byte) 0x13, (Byte) 0x73, (Byte) 0x65, (Byte) 0x72, (Byte) 0x76,
    (Byte) 0x65, (Byte) 0x72, (Byte) 0x31, (Byte) 0x2e, (Byte) 0x65, (Byte) 0x78,
    (Byte) 0x61, (Byte) 0x6d, (Byte) 0x70, (Byte) 0x6c, (Byte) 0x65, (Byte) 0x2e,
    (Byte) 0x63, (Byte) 0x6f, (Byte) 0x6d, (Byte) 0x30, (Byte) 0x81, (Byte) 0x9f,
    (Byte) 0x30, (Byte) 0x0d, (Byte) 0x06, (Byte) 0x09, (Byte) 0x2a, (Byte) 0x86,
    (Byte) 0x48, (Byte) 0x86, (Byte) 0xf7, (Byte) 0x0d, (Byte) 0x01, (Byte) 0x01,
    (Byte) 0x01, (Byte) 0x05, (Byte) 0x00, (Byte) 0x03, (Byte) 0x81, (Byte) 0x8d,
    (Byte) 0x00, (Byte) 0x30, (Byte) 0x81, (Byte) 0x89, (Byte) 0x02, (Byte) 0x81,
    (Byte) 0x81, (Byte) 0x00, (Byte) 0xce, (Byte) 0x29, (Byte) 0xeb, (Byte) 0xf6,
    (Byte) 0x5b, (Byte) 0x25, (Byte) 0xdc, (Byte) 0xa1, (Byte) 0xa6, (Byte) 0x2c,
    (Byte) 0x66, (Byte) 0xcb, (Byte) 0x20, (Byte) 0x90, (Byte) 0x27, (Byte) 0x86,
    (Byte) 0x8a, (Byte) 0x44, (Byte) 0x71, (Byte) 0x50, (Byte) 0xda, (Byte) 0xd3,
    (Byte) 0x02, (Byte) 0x77, (Byte) 0x55, (Byte) 0xe9, (Byte) 0xe8, (Byte) 0x08,
    (Byte) 0xf3, (Byte) 0x36, (Byte) 0x9a, (Byte) 0xae, (Byte) 0xab, (Byte) 0x04,
    (Byte) 0x6d, (Byte) 0x00, (Byte) 0x99, (Byte) 0xbf, (Byte) 0x7d, (Byte) 0x0f,
    (Byte) 0x67, (Byte) 0x8b, (Byte) 0x1d, (Byte) 0xd4, (Byte) 0x2b, (Byte) 0x7c,
    (Byte) 0xcb, (Byte) 0xcd, (Byte) 0x33, (Byte) 0xc7, (Byte) 0x84, (Byte) 0x30,
    (Byte) 0xe2, (Byte) 0x45, (Byte) 0x21, (Byte) 0xb3, (Byte) 0x75, (Byte) 0xf5,
    (Byte) 0x79, (Byte) 0x02, (Byte) 0xda, (Byte) 0x50, (Byte) 0xa3, (Byte) 0x8b,
    (Byte) 0xce, (Byte) 0xc3, (Byte) 0x8e, (Byte) 0x0f, (Byte) 0x25, (Byte) 0xeb,
    (Byte) 0x08, (Byte) 0x2c, (Byte) 0xdd, (Byte) 0x1c, (Byte) 0xcf, (Byte) 0xff,
    (Byte) 0x3b, (Byte) 0xde, (Byte) 0xb6, (Byte) 0xaa, (Byte) 0x2a, (Byte) 0xa9,
    (Byte) 0xc4, (Byte) 0x8a, (Byte) 0x24, (Byte) 0x24, (Byte) 0xe6, (Byte) 0x29,
    (Byte) 0x0d, (Byte) 0x98, (Byte) 0x4c, (Byte) 0x32, (Byte) 0xa1, (Byte) 0x7b,
    (Byte) 0x23, (Byte) 0x2b, (Byte) 0x42, (Byte) 0x30, (Byte) 0xee, (Byte) 0x78,
    (Byte) 0x08, (Byte) 0x47, (Byte) 0xad, (Byte) 0xf2, (Byte) 0x96, (Byte) 0xd5,
    (Byte) 0xf1, (Byte) 0x62, (Byte) 0x42, (Byte) 0x2d, (Byte) 0x35, (Byte) 0x19,
    (Byte) 0xb4, (Byte) 0x3c, (Byte) 0xc9, (Byte) 0xc3, (Byte) 0x5f, (Byte) 0x03,
    (Byte) 0x16, (Byte) 0x3a, (Byte) 0x23, (Byte) 0xac, (Byte) 0xcb, (Byte) 0xce,
    (Byte) 0x9e, (Byte) 0x51, (Byte) 0x2e, (Byte) 0x6d, (Byte) 0x02, (Byte) 0x03,
    (Byte) 0x01, (Byte) 0x00, (Byte) 0x01, (Byte) 0xa3, (Byte) 0x7b, (Byte) 0x30,
    (Byte) 0x79, (Byte) 0x30, (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x1d, (Byte) 0x13, (Byte) 0x04, (Byte) 0x02, (Byte) 0x30, (Byte) 0x00,
    (Byte) 0x30, (Byte) 0x2c, (Byte) 0x06, (Byte) 0x09, (Byte) 0x60, (Byte) 0x86,
    (Byte) 0x48, (Byte) 0x01, (Byte) 0x86, (Byte) 0xf8, (Byte) 0x42, (Byte) 0x01,
    (Byte) 0x0d, (Byte) 0x04, (Byte) 0x1f, (Byte) 0x16, (Byte) 0x1d, (Byte) 0x4f,
    (Byte) 0x70, (Byte) 0x65, (Byte) 0x6e, (Byte) 0x53, (Byte) 0x53, (Byte) 0x4c,
    (Byte) 0x20, (Byte) 0x47, (Byte) 0x65, (Byte) 0x6e, (Byte) 0x65, (Byte) 0x72,
    (Byte) 0x61, (Byte) 0x74, (Byte) 0x65, (Byte) 0x64, (Byte) 0x20, (Byte) 0x43,
    (Byte) 0x65, (Byte) 0x72, (Byte) 0x74, (Byte) 0x69, (Byte) 0x66, (Byte) 0x69,
    (Byte) 0x63, (Byte) 0x61, (Byte) 0x74, (Byte) 0x65, (Byte) 0x30, (Byte) 0x1d,
    (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x1d, (Byte) 0x0e, (Byte) 0x04,
    (Byte) 0x16, (Byte) 0x04, (Byte) 0x14, (Byte) 0x32, (Byte) 0xa1, (Byte) 0x1e,
    (Byte) 0x6b, (Byte) 0x69, (Byte) 0x04, (Byte) 0xfe, (Byte) 0xb3, (Byte) 0xcd,
    (Byte) 0xf8, (Byte) 0xbb, (Byte) 0x14, (Byte) 0xcd, (Byte) 0xff, (Byte) 0xd4,
    (Byte) 0x16, (Byte) 0xc3, (Byte) 0xab, (Byte) 0x44, (Byte) 0x2f, (Byte) 0x30,
    (Byte) 0x1f, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x1d, (Byte) 0x23,
    (Byte) 0x04, (Byte) 0x18, (Byte) 0x30, (Byte) 0x16, (Byte) 0x80, (Byte) 0x14,
    (Byte) 0x33, (Byte) 0x05, (Byte) 0xee, (Byte) 0xfe, (Byte) 0x6f, (Byte) 0x60,
    (Byte) 0xc7, (Byte) 0xf9, (Byte) 0xa9, (Byte) 0xd2, (Byte) 0x73, (Byte) 0x5c,
    (Byte) 0x8f, (Byte) 0x6d, (Byte) 0xa2, (Byte) 0x2f, (Byte) 0x97, (Byte) 0x8e,
    (Byte) 0x5d, (Byte) 0x51, (Byte) 0x30, (Byte) 0x0d, (Byte) 0x06, (Byte) 0x09,
    (Byte) 0x2a, (Byte) 0x86, (Byte) 0x48, (Byte) 0x86, (Byte) 0xf7, (Byte) 0x0d,
    (Byte) 0x01, (Byte) 0x01, (Byte) 0x05, (Byte) 0x05, (Byte) 0x00, (Byte) 0x03,
    (Byte) 0x81, (Byte) 0x81, (Byte) 0x00, (Byte) 0x46, (Byte) 0x42, (Byte) 0xef,
    (Byte) 0x56, (Byte) 0x89, (Byte) 0x78, (Byte) 0x90, (Byte) 0x38, (Byte) 0x24,
    (Byte) 0x9f, (Byte) 0x8c, (Byte) 0x7a, (Byte) 0xce, (Byte) 0x7a, (Byte) 0xa5,
    (Byte) 0xb5, (Byte) 0x1e, (Byte) 0x74, (Byte) 0x96, (Byte) 0x34, (Byte) 0x49,
    (Byte) 0x8b, (Byte) 0xed, (Byte) 0x44, (Byte) 0xb3, (Byte) 0xc9, (Byte) 0x05,
    (Byte) 0xd7, (Byte) 0x48, (Byte) 0x55, (Byte) 0x52, (Byte) 0x59, (Byte) 0x15,
    (Byte) 0x0b, (Byte) 0xaa, (Byte) 0x16, (Byte) 0x86, (Byte) 0xd2, (Byte) 0x8e,
    (Byte) 0x16, (Byte) 0x99, (Byte) 0xe8, (Byte) 0x5f, (Byte) 0x11, (Byte) 0x71,
    (Byte) 0x42, (Byte) 0x55, (Byte) 0xd1, (Byte) 0xc4, (Byte) 0x6f, (Byte) 0x2e,
    (Byte) 0xa9, (Byte) 0x64, (Byte) 0x6f, (Byte) 0xd8, (Byte) 0xfd, (Byte) 0x43,
    (Byte) 0x13, (Byte) 0x24, (Byte) 0xaa, (Byte) 0x67, (Byte) 0xe6, (Byte) 0xf5,
    (Byte) 0xca, (Byte) 0x80, (Byte) 0x5e, (Byte) 0x3a, (Byte) 0x3e, (Byte) 0xcc,
    (Byte) 0x4f, (Byte) 0xba, (Byte) 0x87, (Byte) 0xe6, (Byte) 0xae, (Byte) 0xbf,
    (Byte) 0x8f, (Byte) 0xd5, (Byte) 0x28, (Byte) 0x38, (Byte) 0x58, (Byte) 0x30,
    (Byte) 0x24, (Byte) 0xf6, (Byte) 0x53, (Byte) 0x5b, (Byte) 0x41, (Byte) 0x53,
    (Byte) 0xe6, (Byte) 0x45, (Byte) 0xbc, (Byte) 0xbe, (Byte) 0xe6, (Byte) 0xbb,
    (Byte) 0x5d, (Byte) 0xd8, (Byte) 0xa7, (Byte) 0xf9, (Byte) 0x64, (Byte) 0x99,
    (Byte) 0x04, (Byte) 0x43, (Byte) 0x75, (Byte) 0xd7, (Byte) 0x2d, (Byte) 0x32,
    (Byte) 0x0a, (Byte) 0x94, (Byte) 0xaf, (Byte) 0x06, (Byte) 0x34, (Byte) 0xae,
    (Byte) 0x46, (Byte) 0xbd, (Byte) 0xda, (Byte) 0x00, (Byte) 0x0e, (Byte) 0x25,
    (Byte) 0xc2, (Byte) 0xf7, (Byte) 0xc9, (Byte) 0xc3, (Byte) 0x65, (Byte) 0xd2,
    (Byte) 0x08, (Byte) 0x41, (Byte) 0x0a, (Byte) 0xf3, (Byte) 0x72
};

static AutoPtr<ArrayOf<Byte> > FAKE_RSA_USER_1 = ArrayOf<Byte>::AllocInplace(_FAKE_RSA_USER_1
        , ArraySize(_FAKE_RSA_USER_1));

/*
 * The keys and certificates below are generated with:
 *
 * openssl req -new -x509 -days 3650 -extensions v3_ca -keyout cakey.pem -out cacert.pem
 * openssl ecparam -name prime256v1 -out ecparam.pem
 * openssl req -newkey ec:ecparam.pem -keyout userkey.pem -nodes -days 3650 -out userkey.req
 * mkdir -p demoCA/newcerts
 * touch demoCA/index.txt
 * echo "01" > demoCA/serial
 * openssl ca -out usercert.pem -in userkey.req -cert cacert.pem -keyfile cakey.pem -days 3650
 */

/**
 * Generated from above and converted with:
 *
 * openssl x509 -outform d -in cacert.pem | xxd -i | sed 's/0x/(Byte) 0x/g'
 */
static Byte _FAKE_EC_CA_1[] = {
    (Byte) 0x30, (Byte) 0x82, (Byte) 0x02, (Byte) 0x58, (Byte) 0x30, (Byte) 0x82,
    (Byte) 0x01, (Byte) 0xc1, (Byte) 0xa0, (Byte) 0x03, (Byte) 0x02, (Byte) 0x01,
    (Byte) 0x02, (Byte) 0x02, (Byte) 0x09, (Byte) 0x00, (Byte) 0xe1, (Byte) 0xb2,
    (Byte) 0x8c, (Byte) 0x04, (Byte) 0x95, (Byte) 0xeb, (Byte) 0x10, (Byte) 0xcb,
    (Byte) 0x30, (Byte) 0x0d, (Byte) 0x06, (Byte) 0x09, (Byte) 0x2a, (Byte) 0x86,
    (Byte) 0x48, (Byte) 0x86, (Byte) 0xf7, (Byte) 0x0d, (Byte) 0x01, (Byte) 0x01,
    (Byte) 0x05, (Byte) 0x05, (Byte) 0x00, (Byte) 0x30, (Byte) 0x45, (Byte) 0x31,
    (Byte) 0x0b, (Byte) 0x30, (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x04, (Byte) 0x06, (Byte) 0x13, (Byte) 0x02, (Byte) 0x41, (Byte) 0x55,
    (Byte) 0x31, (Byte) 0x13, (Byte) 0x30, (Byte) 0x11, (Byte) 0x06, (Byte) 0x03,
    (Byte) 0x55, (Byte) 0x04, (Byte) 0x08, (Byte) 0x0c, (Byte) 0x0a, (Byte) 0x53,
    (Byte) 0x6f, (Byte) 0x6d, (Byte) 0x65, (Byte) 0x2d, (Byte) 0x53, (Byte) 0x74,
    (Byte) 0x61, (Byte) 0x74, (Byte) 0x65, (Byte) 0x31, (Byte) 0x21, (Byte) 0x30,
    (Byte) 0x1f, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x0a,
    (Byte) 0x0c, (Byte) 0x18, (Byte) 0x49, (Byte) 0x6e, (Byte) 0x74, (Byte) 0x65,
    (Byte) 0x72, (Byte) 0x6e, (Byte) 0x65, (Byte) 0x74, (Byte) 0x20, (Byte) 0x57,
    (Byte) 0x69, (Byte) 0x64, (Byte) 0x67, (Byte) 0x69, (Byte) 0x74, (Byte) 0x73,
    (Byte) 0x20, (Byte) 0x50, (Byte) 0x74, (Byte) 0x79, (Byte) 0x20, (Byte) 0x4c,
    (Byte) 0x74, (Byte) 0x64, (Byte) 0x30, (Byte) 0x1e, (Byte) 0x17, (Byte) 0x0d,
    (Byte) 0x31, (Byte) 0x33, (Byte) 0x30, (Byte) 0x38, (Byte) 0x32, (Byte) 0x37,
    (Byte) 0x31, (Byte) 0x36, (Byte) 0x32, (Byte) 0x38, (Byte) 0x32, (Byte) 0x38,
    (Byte) 0x5a, (Byte) 0x17, (Byte) 0x0d, (Byte) 0x32, (Byte) 0x33, (Byte) 0x30,
    (Byte) 0x38, (Byte) 0x32, (Byte) 0x35, (Byte) 0x31, (Byte) 0x36, (Byte) 0x32,
    (Byte) 0x38, (Byte) 0x32, (Byte) 0x38, (Byte) 0x5a, (Byte) 0x30, (Byte) 0x45,
    (Byte) 0x31, (Byte) 0x0b, (Byte) 0x30, (Byte) 0x09, (Byte) 0x06, (Byte) 0x03,
    (Byte) 0x55, (Byte) 0x04, (Byte) 0x06, (Byte) 0x13, (Byte) 0x02, (Byte) 0x41,
    (Byte) 0x55, (Byte) 0x31, (Byte) 0x13, (Byte) 0x30, (Byte) 0x11, (Byte) 0x06,
    (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x08, (Byte) 0x0c, (Byte) 0x0a,
    (Byte) 0x53, (Byte) 0x6f, (Byte) 0x6d, (Byte) 0x65, (Byte) 0x2d, (Byte) 0x53,
    (Byte) 0x74, (Byte) 0x61, (Byte) 0x74, (Byte) 0x65, (Byte) 0x31, (Byte) 0x21,
    (Byte) 0x30, (Byte) 0x1f, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04,
    (Byte) 0x0a, (Byte) 0x0c, (Byte) 0x18, (Byte) 0x49, (Byte) 0x6e, (Byte) 0x74,
    (Byte) 0x65, (Byte) 0x72, (Byte) 0x6e, (Byte) 0x65, (Byte) 0x74, (Byte) 0x20,
    (Byte) 0x57, (Byte) 0x69, (Byte) 0x64, (Byte) 0x67, (Byte) 0x69, (Byte) 0x74,
    (Byte) 0x73, (Byte) 0x20, (Byte) 0x50, (Byte) 0x74, (Byte) 0x79, (Byte) 0x20,
    (Byte) 0x4c, (Byte) 0x74, (Byte) 0x64, (Byte) 0x30, (Byte) 0x81, (Byte) 0x9f,
    (Byte) 0x30, (Byte) 0x0d, (Byte) 0x06, (Byte) 0x09, (Byte) 0x2a, (Byte) 0x86,
    (Byte) 0x48, (Byte) 0x86, (Byte) 0xf7, (Byte) 0x0d, (Byte) 0x01, (Byte) 0x01,
    (Byte) 0x01, (Byte) 0x05, (Byte) 0x00, (Byte) 0x03, (Byte) 0x81, (Byte) 0x8d,
    (Byte) 0x00, (Byte) 0x30, (Byte) 0x81, (Byte) 0x89, (Byte) 0x02, (Byte) 0x81,
    (Byte) 0x81, (Byte) 0x00, (Byte) 0xb5, (Byte) 0xf6, (Byte) 0x08, (Byte) 0x0f,
    (Byte) 0xc4, (Byte) 0x4d, (Byte) 0xe4, (Byte) 0x0d, (Byte) 0x34, (Byte) 0x1d,
    (Byte) 0xe2, (Byte) 0x23, (Byte) 0x18, (Byte) 0x63, (Byte) 0x03, (Byte) 0xf7,
    (Byte) 0x14, (Byte) 0x0e, (Byte) 0x98, (Byte) 0xcd, (Byte) 0x45, (Byte) 0x1f,
    (Byte) 0xfe, (Byte) 0xfb, (Byte) 0x09, (Byte) 0x3f, (Byte) 0x5d, (Byte) 0x36,
    (Byte) 0x3b, (Byte) 0x0f, (Byte) 0xf9, (Byte) 0x5e, (Byte) 0x86, (Byte) 0x56,
    (Byte) 0x64, (Byte) 0xd7, (Byte) 0x3f, (Byte) 0xae, (Byte) 0x33, (Byte) 0x09,
    (Byte) 0xd3, (Byte) 0xdd, (Byte) 0x06, (Byte) 0x17, (Byte) 0x26, (Byte) 0xdc,
    (Byte) 0xa2, (Byte) 0x8c, (Byte) 0x3c, (Byte) 0x65, (Byte) 0xed, (Byte) 0x03,
    (Byte) 0x82, (Byte) 0x78, (Byte) 0x9b, (Byte) 0xee, (Byte) 0xe3, (Byte) 0x98,
    (Byte) 0x58, (Byte) 0xe1, (Byte) 0xf1, (Byte) 0xa0, (Byte) 0x85, (Byte) 0xae,
    (Byte) 0x63, (Byte) 0x84, (Byte) 0x41, (Byte) 0x46, (Byte) 0xa7, (Byte) 0x4f,
    (Byte) 0xdc, (Byte) 0xbb, (Byte) 0x1c, (Byte) 0x6e, (Byte) 0xec, (Byte) 0x7b,
    (Byte) 0xd5, (Byte) 0xab, (Byte) 0x3d, (Byte) 0x6a, (Byte) 0x05, (Byte) 0x58,
    (Byte) 0x0f, (Byte) 0x9b, (Byte) 0x6a, (Byte) 0x67, (Byte) 0x4b, (Byte) 0xe9,
    (Byte) 0x2a, (Byte) 0x6d, (Byte) 0x96, (Byte) 0x11, (Byte) 0x53, (Byte) 0x95,
    (Byte) 0x78, (Byte) 0xaa, (Byte) 0xd1, (Byte) 0x91, (Byte) 0x4a, (Byte) 0xf8,
    (Byte) 0x54, (Byte) 0x52, (Byte) 0x6d, (Byte) 0xb9, (Byte) 0xca, (Byte) 0x74,
    (Byte) 0x81, (Byte) 0xf8, (Byte) 0x99, (Byte) 0x64, (Byte) 0xd1, (Byte) 0x4f,
    (Byte) 0x01, (Byte) 0x38, (Byte) 0x4f, (Byte) 0x08, (Byte) 0x5c, (Byte) 0x31,
    (Byte) 0xcb, (Byte) 0x7c, (Byte) 0x5c, (Byte) 0x78, (Byte) 0x5d, (Byte) 0x47,
    (Byte) 0xd9, (Byte) 0xf0, (Byte) 0x1a, (Byte) 0xeb, (Byte) 0x02, (Byte) 0x03,
    (Byte) 0x01, (Byte) 0x00, (Byte) 0x01, (Byte) 0xa3, (Byte) 0x50, (Byte) 0x30,
    (Byte) 0x4e, (Byte) 0x30, (Byte) 0x1d, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x1d, (Byte) 0x0e, (Byte) 0x04, (Byte) 0x16, (Byte) 0x04, (Byte) 0x14,
    (Byte) 0x5f, (Byte) 0x5b, (Byte) 0x5e, (Byte) 0xac, (Byte) 0x29, (Byte) 0xfa,
    (Byte) 0xa1, (Byte) 0x9f, (Byte) 0x9e, (Byte) 0xad, (Byte) 0x46, (Byte) 0xe1,
    (Byte) 0xbc, (Byte) 0x20, (Byte) 0x72, (Byte) 0xcf, (Byte) 0x4a, (Byte) 0xd4,
    (Byte) 0xfa, (Byte) 0xe3, (Byte) 0x30, (Byte) 0x1f, (Byte) 0x06, (Byte) 0x03,
    (Byte) 0x55, (Byte) 0x1d, (Byte) 0x23, (Byte) 0x04, (Byte) 0x18, (Byte) 0x30,
    (Byte) 0x16, (Byte) 0x80, (Byte) 0x14, (Byte) 0x5f, (Byte) 0x5b, (Byte) 0x5e,
    (Byte) 0xac, (Byte) 0x29, (Byte) 0xfa, (Byte) 0xa1, (Byte) 0x9f, (Byte) 0x9e,
    (Byte) 0xad, (Byte) 0x46, (Byte) 0xe1, (Byte) 0xbc, (Byte) 0x20, (Byte) 0x72,
    (Byte) 0xcf, (Byte) 0x4a, (Byte) 0xd4, (Byte) 0xfa, (Byte) 0xe3, (Byte) 0x30,
    (Byte) 0x0c, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x1d, (Byte) 0x13,
    (Byte) 0x04, (Byte) 0x05, (Byte) 0x30, (Byte) 0x03, (Byte) 0x01, (Byte) 0x01,
    (Byte) 0xff, (Byte) 0x30, (Byte) 0x0d, (Byte) 0x06, (Byte) 0x09, (Byte) 0x2a,
    (Byte) 0x86, (Byte) 0x48, (Byte) 0x86, (Byte) 0xf7, (Byte) 0x0d, (Byte) 0x01,
    (Byte) 0x01, (Byte) 0x05, (Byte) 0x05, (Byte) 0x00, (Byte) 0x03, (Byte) 0x81,
    (Byte) 0x81, (Byte) 0x00, (Byte) 0xa1, (Byte) 0x4a, (Byte) 0xe6, (Byte) 0xfc,
    (Byte) 0x7f, (Byte) 0x17, (Byte) 0xaa, (Byte) 0x65, (Byte) 0x4a, (Byte) 0x34,
    (Byte) 0xde, (Byte) 0x69, (Byte) 0x67, (Byte) 0x54, (Byte) 0x4d, (Byte) 0xa2,
    (Byte) 0xc2, (Byte) 0x98, (Byte) 0x02, (Byte) 0x43, (Byte) 0x6a, (Byte) 0x0e,
    (Byte) 0x0b, (Byte) 0x7f, (Byte) 0xa4, (Byte) 0x46, (Byte) 0xaf, (Byte) 0xa4,
    (Byte) 0x65, (Byte) 0xa0, (Byte) 0xdb, (Byte) 0xf1, (Byte) 0x5b, (Byte) 0xd5,
    (Byte) 0x09, (Byte) 0xbc, (Byte) 0xee, (Byte) 0x37, (Byte) 0x51, (Byte) 0x19,
    (Byte) 0x36, (Byte) 0xc0, (Byte) 0x90, (Byte) 0xd3, (Byte) 0x5f, (Byte) 0xf3,
    (Byte) 0x4f, (Byte) 0xb9, (Byte) 0x08, (Byte) 0x45, (Byte) 0x0e, (Byte) 0x01,
    (Byte) 0x8a, (Byte) 0x95, (Byte) 0xef, (Byte) 0x92, (Byte) 0x95, (Byte) 0x33,
    (Byte) 0x78, (Byte) 0xdd, (Byte) 0x90, (Byte) 0xbb, (Byte) 0xf3, (Byte) 0x06,
    (Byte) 0x75, (Byte) 0xd0, (Byte) 0x66, (Byte) 0xe6, (Byte) 0xd0, (Byte) 0x18,
    (Byte) 0x6e, (Byte) 0xeb, (Byte) 0x1c, (Byte) 0x52, (Byte) 0xc3, (Byte) 0x2e,
    (Byte) 0x57, (Byte) 0x7d, (Byte) 0xa9, (Byte) 0x03, (Byte) 0xdb, (Byte) 0xf4,
    (Byte) 0x57, (Byte) 0x5f, (Byte) 0x6c, (Byte) 0x7e, (Byte) 0x00, (Byte) 0x0d,
    (Byte) 0x8f, (Byte) 0xe8, (Byte) 0x91, (Byte) 0xf7, (Byte) 0xae, (Byte) 0x24,
    (Byte) 0x35, (Byte) 0x07, (Byte) 0xb5, (Byte) 0x48, (Byte) 0x2d, (Byte) 0x36,
    (Byte) 0x30, (Byte) 0x5d, (Byte) 0xe9, (Byte) 0x49, (Byte) 0x2d, (Byte) 0xd1,
    (Byte) 0x5d, (Byte) 0xc5, (Byte) 0xf4, (Byte) 0x33, (Byte) 0x77, (Byte) 0x3c,
    (Byte) 0x71, (Byte) 0xad, (Byte) 0x90, (Byte) 0x65, (Byte) 0xa9, (Byte) 0xc1,
    (Byte) 0x0b, (Byte) 0x5c, (Byte) 0x62, (Byte) 0x55, (Byte) 0x50, (Byte) 0x6f,
    (Byte) 0x9b, (Byte) 0xc9, (Byte) 0x0d, (Byte) 0xee
};

static AutoPtr<ArrayOf<Byte> > FAKE_EC_CA_1 = ArrayOf<Byte>::AllocInplace(_FAKE_EC_CA_1
        , ArraySize(_FAKE_EC_CA_1));

/**
 * Generated from above and converted with:
 *
 * openssl pkcs8 -topk8 -outform d -in userkey.pem -nocrypt | xxd -i | sed 's/0x/(Byte) 0x/g'
 */
static Byte _FAKE_EC_KEY_1[] = {
    (Byte) 0x30, (Byte) 0x81, (Byte) 0x87, (Byte) 0x02, (Byte) 0x01, (Byte) 0x00,
    (Byte) 0x30, (Byte) 0x13, (Byte) 0x06, (Byte) 0x07, (Byte) 0x2a, (Byte) 0x86,
    (Byte) 0x48, (Byte) 0xce, (Byte) 0x3d, (Byte) 0x02, (Byte) 0x01, (Byte) 0x06,
    (Byte) 0x08, (Byte) 0x2a, (Byte) 0x86, (Byte) 0x48, (Byte) 0xce, (Byte) 0x3d,
    (Byte) 0x03, (Byte) 0x01, (Byte) 0x07, (Byte) 0x04, (Byte) 0x6d, (Byte) 0x30,
    (Byte) 0x6b, (Byte) 0x02, (Byte) 0x01, (Byte) 0x01, (Byte) 0x04, (Byte) 0x20,
    (Byte) 0x3a, (Byte) 0x8a, (Byte) 0x02, (Byte) 0xdc, (Byte) 0xde, (Byte) 0x70,
    (Byte) 0x84, (Byte) 0x45, (Byte) 0x34, (Byte) 0xaf, (Byte) 0xbd, (Byte) 0xd5,
    (Byte) 0x02, (Byte) 0x17, (Byte) 0x69, (Byte) 0x90, (Byte) 0x65, (Byte) 0x1e,
    (Byte) 0x87, (Byte) 0xf1, (Byte) 0x3d, (Byte) 0x17, (Byte) 0xb6, (Byte) 0xf4,
    (Byte) 0x31, (Byte) 0x94, (Byte) 0x86, (Byte) 0x76, (Byte) 0x55, (Byte) 0xf7,
    (Byte) 0xcc, (Byte) 0xba, (Byte) 0xa1, (Byte) 0x44, (Byte) 0x03, (Byte) 0x42,
    (Byte) 0x00, (Byte) 0x04, (Byte) 0xd9, (Byte) 0xcf, (Byte) 0xe7, (Byte) 0x9b,
    (Byte) 0x23, (Byte) 0xc8, (Byte) 0xa3, (Byte) 0xb8, (Byte) 0x33, (Byte) 0x14,
    (Byte) 0xa4, (Byte) 0x4d, (Byte) 0x75, (Byte) 0x90, (Byte) 0xf3, (Byte) 0xcd,
    (Byte) 0x43, (Byte) 0xe5, (Byte) 0x1b, (Byte) 0x05, (Byte) 0x1d, (Byte) 0xf3,
    (Byte) 0xd0, (Byte) 0xa3, (Byte) 0xb7, (Byte) 0x32, (Byte) 0x5f, (Byte) 0x79,
    (Byte) 0xdc, (Byte) 0x88, (Byte) 0xb8, (Byte) 0x4d, (Byte) 0xb3, (Byte) 0xd1,
    (Byte) 0x6d, (Byte) 0xf7, (Byte) 0x75, (Byte) 0xf3, (Byte) 0xbf, (Byte) 0x50,
    (Byte) 0xa1, (Byte) 0xbc, (Byte) 0x03, (Byte) 0x64, (Byte) 0x22, (Byte) 0xe6,
    (Byte) 0x1a, (Byte) 0xa1, (Byte) 0xe1, (Byte) 0x06, (Byte) 0x68, (Byte) 0x3b,
    (Byte) 0xbc, (Byte) 0x9f, (Byte) 0xd3, (Byte) 0xae, (Byte) 0x77, (Byte) 0x5e,
    (Byte) 0x88, (Byte) 0x0c, (Byte) 0x5e, (Byte) 0x0c, (Byte) 0xb2, (Byte) 0x38
};

static AutoPtr<ArrayOf<Byte> > FAKE_EC_KEY_1 = ArrayOf<Byte>::AllocInplace(_FAKE_EC_KEY_1
        , ArraySize(_FAKE_EC_KEY_1));

/**
 * Generated from above and converted with:
 *
 * openssl x509 -outform d -in usercert.pem | xxd -i | sed 's/0x/(Byte) 0x/g'
 */
static Byte _FAKE_EC_USER_1[] = {
    (Byte) 0x30, (Byte) 0x82, (Byte) 0x02, (Byte) 0x51, (Byte) 0x30, (Byte) 0x82,
    (Byte) 0x01, (Byte) 0xba, (Byte) 0xa0, (Byte) 0x03, (Byte) 0x02, (Byte) 0x01,
    (Byte) 0x02, (Byte) 0x02, (Byte) 0x01, (Byte) 0x01, (Byte) 0x30, (Byte) 0x0d,
    (Byte) 0x06, (Byte) 0x09, (Byte) 0x2a, (Byte) 0x86, (Byte) 0x48, (Byte) 0x86,
    (Byte) 0xf7, (Byte) 0x0d, (Byte) 0x01, (Byte) 0x01, (Byte) 0x05, (Byte) 0x05,
    (Byte) 0x00, (Byte) 0x30, (Byte) 0x45, (Byte) 0x31, (Byte) 0x0b, (Byte) 0x30,
    (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x06,
    (Byte) 0x13, (Byte) 0x02, (Byte) 0x41, (Byte) 0x55, (Byte) 0x31, (Byte) 0x13,
    (Byte) 0x30, (Byte) 0x11, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04,
    (Byte) 0x08, (Byte) 0x0c, (Byte) 0x0a, (Byte) 0x53, (Byte) 0x6f, (Byte) 0x6d,
    (Byte) 0x65, (Byte) 0x2d, (Byte) 0x53, (Byte) 0x74, (Byte) 0x61, (Byte) 0x74,
    (Byte) 0x65, (Byte) 0x31, (Byte) 0x21, (Byte) 0x30, (Byte) 0x1f, (Byte) 0x06,
    (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x0a, (Byte) 0x0c, (Byte) 0x18,
    (Byte) 0x49, (Byte) 0x6e, (Byte) 0x74, (Byte) 0x65, (Byte) 0x72, (Byte) 0x6e,
    (Byte) 0x65, (Byte) 0x74, (Byte) 0x20, (Byte) 0x57, (Byte) 0x69, (Byte) 0x64,
    (Byte) 0x67, (Byte) 0x69, (Byte) 0x74, (Byte) 0x73, (Byte) 0x20, (Byte) 0x50,
    (Byte) 0x74, (Byte) 0x79, (Byte) 0x20, (Byte) 0x4c, (Byte) 0x74, (Byte) 0x64,
    (Byte) 0x30, (Byte) 0x1e, (Byte) 0x17, (Byte) 0x0d, (Byte) 0x31, (Byte) 0x33,
    (Byte) 0x30, (Byte) 0x38, (Byte) 0x32, (Byte) 0x37, (Byte) 0x31, (Byte) 0x36,
    (Byte) 0x33, (Byte) 0x30, (Byte) 0x30, (Byte) 0x38, (Byte) 0x5a, (Byte) 0x17,
    (Byte) 0x0d, (Byte) 0x32, (Byte) 0x33, (Byte) 0x30, (Byte) 0x38, (Byte) 0x32,
    (Byte) 0x35, (Byte) 0x31, (Byte) 0x36, (Byte) 0x33, (Byte) 0x30, (Byte) 0x30,
    (Byte) 0x38, (Byte) 0x5a, (Byte) 0x30, (Byte) 0x62, (Byte) 0x31, (Byte) 0x0b,
    (Byte) 0x30, (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04,
    (Byte) 0x06, (Byte) 0x13, (Byte) 0x02, (Byte) 0x41, (Byte) 0x55, (Byte) 0x31,
    (Byte) 0x13, (Byte) 0x30, (Byte) 0x11, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x04, (Byte) 0x08, (Byte) 0x0c, (Byte) 0x0a, (Byte) 0x53, (Byte) 0x6f,
    (Byte) 0x6d, (Byte) 0x65, (Byte) 0x2d, (Byte) 0x53, (Byte) 0x74, (Byte) 0x61,
    (Byte) 0x74, (Byte) 0x65, (Byte) 0x31, (Byte) 0x21, (Byte) 0x30, (Byte) 0x1f,
    (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x0a, (Byte) 0x0c,
    (Byte) 0x18, (Byte) 0x49, (Byte) 0x6e, (Byte) 0x74, (Byte) 0x65, (Byte) 0x72,
    (Byte) 0x6e, (Byte) 0x65, (Byte) 0x74, (Byte) 0x20, (Byte) 0x57, (Byte) 0x69,
    (Byte) 0x64, (Byte) 0x67, (Byte) 0x69, (Byte) 0x74, (Byte) 0x73, (Byte) 0x20,
    (Byte) 0x50, (Byte) 0x74, (Byte) 0x79, (Byte) 0x20, (Byte) 0x4c, (Byte) 0x74,
    (Byte) 0x64, (Byte) 0x31, (Byte) 0x1b, (Byte) 0x30, (Byte) 0x19, (Byte) 0x06,
    (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x03, (Byte) 0x0c, (Byte) 0x12,
    (Byte) 0x73, (Byte) 0x65, (Byte) 0x72, (Byte) 0x76, (Byte) 0x65, (Byte) 0x72,
    (Byte) 0x2e, (Byte) 0x65, (Byte) 0x78, (Byte) 0x61, (Byte) 0x6d, (Byte) 0x70,
    (Byte) 0x6c, (Byte) 0x65, (Byte) 0x2e, (Byte) 0x63, (Byte) 0x6f, (Byte) 0x6d,
    (Byte) 0x30, (Byte) 0x59, (Byte) 0x30, (Byte) 0x13, (Byte) 0x06, (Byte) 0x07,
    (Byte) 0x2a, (Byte) 0x86, (Byte) 0x48, (Byte) 0xce, (Byte) 0x3d, (Byte) 0x02,
    (Byte) 0x01, (Byte) 0x06, (Byte) 0x08, (Byte) 0x2a, (Byte) 0x86, (Byte) 0x48,
    (Byte) 0xce, (Byte) 0x3d, (Byte) 0x03, (Byte) 0x01, (Byte) 0x07, (Byte) 0x03,
    (Byte) 0x42, (Byte) 0x00, (Byte) 0x04, (Byte) 0xd9, (Byte) 0xcf, (Byte) 0xe7,
    (Byte) 0x9b, (Byte) 0x23, (Byte) 0xc8, (Byte) 0xa3, (Byte) 0xb8, (Byte) 0x33,
    (Byte) 0x14, (Byte) 0xa4, (Byte) 0x4d, (Byte) 0x75, (Byte) 0x90, (Byte) 0xf3,
    (Byte) 0xcd, (Byte) 0x43, (Byte) 0xe5, (Byte) 0x1b, (Byte) 0x05, (Byte) 0x1d,
    (Byte) 0xf3, (Byte) 0xd0, (Byte) 0xa3, (Byte) 0xb7, (Byte) 0x32, (Byte) 0x5f,
    (Byte) 0x79, (Byte) 0xdc, (Byte) 0x88, (Byte) 0xb8, (Byte) 0x4d, (Byte) 0xb3,
    (Byte) 0xd1, (Byte) 0x6d, (Byte) 0xf7, (Byte) 0x75, (Byte) 0xf3, (Byte) 0xbf,
    (Byte) 0x50, (Byte) 0xa1, (Byte) 0xbc, (Byte) 0x03, (Byte) 0x64, (Byte) 0x22,
    (Byte) 0xe6, (Byte) 0x1a, (Byte) 0xa1, (Byte) 0xe1, (Byte) 0x06, (Byte) 0x68,
    (Byte) 0x3b, (Byte) 0xbc, (Byte) 0x9f, (Byte) 0xd3, (Byte) 0xae, (Byte) 0x77,
    (Byte) 0x5e, (Byte) 0x88, (Byte) 0x0c, (Byte) 0x5e, (Byte) 0x0c, (Byte) 0xb2,
    (Byte) 0x38, (Byte) 0xa3, (Byte) 0x7b, (Byte) 0x30, (Byte) 0x79, (Byte) 0x30,
    (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x1d, (Byte) 0x13,
    (Byte) 0x04, (Byte) 0x02, (Byte) 0x30, (Byte) 0x00, (Byte) 0x30, (Byte) 0x2c,
    (Byte) 0x06, (Byte) 0x09, (Byte) 0x60, (Byte) 0x86, (Byte) 0x48, (Byte) 0x01,
    (Byte) 0x86, (Byte) 0xf8, (Byte) 0x42, (Byte) 0x01, (Byte) 0x0d, (Byte) 0x04,
    (Byte) 0x1f, (Byte) 0x16, (Byte) 0x1d, (Byte) 0x4f, (Byte) 0x70, (Byte) 0x65,
    (Byte) 0x6e, (Byte) 0x53, (Byte) 0x53, (Byte) 0x4c, (Byte) 0x20, (Byte) 0x47,
    (Byte) 0x65, (Byte) 0x6e, (Byte) 0x65, (Byte) 0x72, (Byte) 0x61, (Byte) 0x74,
    (Byte) 0x65, (Byte) 0x64, (Byte) 0x20, (Byte) 0x43, (Byte) 0x65, (Byte) 0x72,
    (Byte) 0x74, (Byte) 0x69, (Byte) 0x66, (Byte) 0x69, (Byte) 0x63, (Byte) 0x61,
    (Byte) 0x74, (Byte) 0x65, (Byte) 0x30, (Byte) 0x1d, (Byte) 0x06, (Byte) 0x03,
    (Byte) 0x55, (Byte) 0x1d, (Byte) 0x0e, (Byte) 0x04, (Byte) 0x16, (Byte) 0x04,
    (Byte) 0x14, (Byte) 0xd5, (Byte) 0xc4, (Byte) 0x72, (Byte) 0xbd, (Byte) 0xd2,
    (Byte) 0x4e, (Byte) 0x90, (Byte) 0x1b, (Byte) 0x14, (Byte) 0x32, (Byte) 0xdb,
    (Byte) 0x03, (Byte) 0xae, (Byte) 0xfa, (Byte) 0x27, (Byte) 0x7d, (Byte) 0x8d,
    (Byte) 0xe4, (Byte) 0x80, (Byte) 0x58, (Byte) 0x30, (Byte) 0x1f, (Byte) 0x06,
    (Byte) 0x03, (Byte) 0x55, (Byte) 0x1d, (Byte) 0x23, (Byte) 0x04, (Byte) 0x18,
    (Byte) 0x30, (Byte) 0x16, (Byte) 0x80, (Byte) 0x14, (Byte) 0x5f, (Byte) 0x5b,
    (Byte) 0x5e, (Byte) 0xac, (Byte) 0x29, (Byte) 0xfa, (Byte) 0xa1, (Byte) 0x9f,
    (Byte) 0x9e, (Byte) 0xad, (Byte) 0x46, (Byte) 0xe1, (Byte) 0xbc, (Byte) 0x20,
    (Byte) 0x72, (Byte) 0xcf, (Byte) 0x4a, (Byte) 0xd4, (Byte) 0xfa, (Byte) 0xe3,
    (Byte) 0x30, (Byte) 0x0d, (Byte) 0x06, (Byte) 0x09, (Byte) 0x2a, (Byte) 0x86,
    (Byte) 0x48, (Byte) 0x86, (Byte) 0xf7, (Byte) 0x0d, (Byte) 0x01, (Byte) 0x01,
    (Byte) 0x05, (Byte) 0x05, (Byte) 0x00, (Byte) 0x03, (Byte) 0x81, (Byte) 0x81,
    (Byte) 0x00, (Byte) 0x43, (Byte) 0x99, (Byte) 0x9f, (Byte) 0x67, (Byte) 0x08,
    (Byte) 0x43, (Byte) 0xd5, (Byte) 0x6b, (Byte) 0x6f, (Byte) 0xd7, (Byte) 0x05,
    (Byte) 0xd6, (Byte) 0x75, (Byte) 0x34, (Byte) 0x30, (Byte) 0xca, (Byte) 0x20,
    (Byte) 0x47, (Byte) 0x61, (Byte) 0xa1, (Byte) 0x89, (Byte) 0xb6, (Byte) 0xf1,
    (Byte) 0x49, (Byte) 0x7b, (Byte) 0xd9, (Byte) 0xb9, (Byte) 0xe8, (Byte) 0x1e,
    (Byte) 0x29, (Byte) 0x74, (Byte) 0x0a, (Byte) 0x67, (Byte) 0xc0, (Byte) 0x7d,
    (Byte) 0xb8, (Byte) 0xe6, (Byte) 0x39, (Byte) 0xa8, (Byte) 0x5e, (Byte) 0xc3,
    (Byte) 0xb0, (Byte) 0xa1, (Byte) 0x30, (Byte) 0x6a, (Byte) 0x1f, (Byte) 0x1d,
    (Byte) 0xfc, (Byte) 0x11, (Byte) 0x59, (Byte) 0x0b, (Byte) 0xb9, (Byte) 0xad,
    (Byte) 0x3a, (Byte) 0x4e, (Byte) 0x50, (Byte) 0x0a, (Byte) 0x61, (Byte) 0xdb,
    (Byte) 0x75, (Byte) 0x6b, (Byte) 0xe5, (Byte) 0x3f, (Byte) 0x8d, (Byte) 0xde,
    (Byte) 0x28, (Byte) 0x68, (Byte) 0xb1, (Byte) 0x29, (Byte) 0x9a, (Byte) 0x18,
    (Byte) 0x8a, (Byte) 0xfc, (Byte) 0x3f, (Byte) 0x13, (Byte) 0x93, (Byte) 0x29,
    (Byte) 0xed, (Byte) 0x22, (Byte) 0x7c, (Byte) 0xb4, (Byte) 0x50, (Byte) 0xd5,
    (Byte) 0x4d, (Byte) 0x32, (Byte) 0x4d, (Byte) 0x42, (Byte) 0x2b, (Byte) 0x29,
    (Byte) 0x97, (Byte) 0x86, (Byte) 0xc0, (Byte) 0x01, (Byte) 0x00, (Byte) 0x25,
    (Byte) 0xf6, (Byte) 0xd3, (Byte) 0x2a, (Byte) 0xd8, (Byte) 0xda, (Byte) 0x13,
    (Byte) 0x94, (Byte) 0x12, (Byte) 0x78, (Byte) 0x14, (Byte) 0x0b, (Byte) 0x51,
    (Byte) 0xc0, (Byte) 0x45, (Byte) 0xb4, (Byte) 0x02, (Byte) 0x37, (Byte) 0x98,
    (Byte) 0x42, (Byte) 0x3c, (Byte) 0xcb, (Byte) 0x2e, (Byte) 0xe4, (Byte) 0x38,
    (Byte) 0x69, (Byte) 0x1b, (Byte) 0x72, (Byte) 0xf0, (Byte) 0xaa, (Byte) 0x89,
    (Byte) 0x7e, (Byte) 0xde, (Byte) 0xb2
};

static AutoPtr<ArrayOf<Byte> > FAKE_EC_USER_1 = ArrayOf<Byte>::AllocInplace(_FAKE_EC_USER_1
        , ArraySize(_FAKE_EC_USER_1));

/*
 * The keys and certificates below are generated with:
 *
 * openssl req -new -x509 -days 3650 -extensions v3_ca -keyout cakey.pem -out cacert.pem
 * openssl dsaparam -out dsaparam.pem 1024
 * openssl req -newkey dsa:dsaparam.pem -keyout userkey.pem -nodes -days 3650 -out userkey.req
 * mkdir -p demoCA/newcerts
 * touch demoCA/index.txt
 * echo "01" > demoCA/serial
 * openssl ca -out usercert.pem -in userkey.req -cert cacert.pem -keyfile cakey.pem -days 3650
 */

/**
 * Generated from above and converted with:
 *
 * openssl x509 -outform d -in cacert.pem | xxd -i | sed 's/0x/(Byte) 0x/g'
 */
static Byte _FAKE_DSA_CA_1[] = {
    (Byte) 0x30, (Byte) 0x82, (Byte) 0x02, (Byte) 0x8a, (Byte) 0x30, (Byte) 0x82,
    (Byte) 0x01, (Byte) 0xf3, (Byte) 0xa0, (Byte) 0x03, (Byte) 0x02, (Byte) 0x01,
    (Byte) 0x02, (Byte) 0x02, (Byte) 0x09, (Byte) 0x00, (Byte) 0x87, (Byte) 0xc0,
    (Byte) 0x68, (Byte) 0x7f, (Byte) 0x42, (Byte) 0x92, (Byte) 0x0b, (Byte) 0x7a,
    (Byte) 0x30, (Byte) 0x0d, (Byte) 0x06, (Byte) 0x09, (Byte) 0x2a, (Byte) 0x86,
    (Byte) 0x48, (Byte) 0x86, (Byte) 0xf7, (Byte) 0x0d, (Byte) 0x01, (Byte) 0x01,
    (Byte) 0x05, (Byte) 0x05, (Byte) 0x00, (Byte) 0x30, (Byte) 0x5e, (Byte) 0x31,
    (Byte) 0x0b, (Byte) 0x30, (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x04, (Byte) 0x06, (Byte) 0x13, (Byte) 0x02, (Byte) 0x41, (Byte) 0x55,
    (Byte) 0x31, (Byte) 0x13, (Byte) 0x30, (Byte) 0x11, (Byte) 0x06, (Byte) 0x03,
    (Byte) 0x55, (Byte) 0x04, (Byte) 0x08, (Byte) 0x0c, (Byte) 0x0a, (Byte) 0x53,
    (Byte) 0x6f, (Byte) 0x6d, (Byte) 0x65, (Byte) 0x2d, (Byte) 0x53, (Byte) 0x74,
    (Byte) 0x61, (Byte) 0x74, (Byte) 0x65, (Byte) 0x31, (Byte) 0x21, (Byte) 0x30,
    (Byte) 0x1f, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x0a,
    (Byte) 0x0c, (Byte) 0x18, (Byte) 0x49, (Byte) 0x6e, (Byte) 0x74, (Byte) 0x65,
    (Byte) 0x72, (Byte) 0x6e, (Byte) 0x65, (Byte) 0x74, (Byte) 0x20, (Byte) 0x57,
    (Byte) 0x69, (Byte) 0x64, (Byte) 0x67, (Byte) 0x69, (Byte) 0x74, (Byte) 0x73,
    (Byte) 0x20, (Byte) 0x50, (Byte) 0x74, (Byte) 0x79, (Byte) 0x20, (Byte) 0x4c,
    (Byte) 0x74, (Byte) 0x64, (Byte) 0x31, (Byte) 0x17, (Byte) 0x30, (Byte) 0x15,
    (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x03, (Byte) 0x0c,
    (Byte) 0x0e, (Byte) 0x63, (Byte) 0x61, (Byte) 0x2e, (Byte) 0x65, (Byte) 0x78,
    (Byte) 0x61, (Byte) 0x6d, (Byte) 0x70, (Byte) 0x6c, (Byte) 0x65, (Byte) 0x2e,
    (Byte) 0x63, (Byte) 0x6f, (Byte) 0x6d, (Byte) 0x30, (Byte) 0x1e, (Byte) 0x17,
    (Byte) 0x0d, (Byte) 0x31, (Byte) 0x33, (Byte) 0x30, (Byte) 0x38, (Byte) 0x32,
    (Byte) 0x37, (Byte) 0x32, (Byte) 0x33, (Byte) 0x33, (Byte) 0x31, (Byte) 0x32,
    (Byte) 0x39, (Byte) 0x5a, (Byte) 0x17, (Byte) 0x0d, (Byte) 0x32, (Byte) 0x33,
    (Byte) 0x30, (Byte) 0x38, (Byte) 0x32, (Byte) 0x35, (Byte) 0x32, (Byte) 0x33,
    (Byte) 0x33, (Byte) 0x31, (Byte) 0x32, (Byte) 0x39, (Byte) 0x5a, (Byte) 0x30,
    (Byte) 0x5e, (Byte) 0x31, (Byte) 0x0b, (Byte) 0x30, (Byte) 0x09, (Byte) 0x06,
    (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x06, (Byte) 0x13, (Byte) 0x02,
    (Byte) 0x41, (Byte) 0x55, (Byte) 0x31, (Byte) 0x13, (Byte) 0x30, (Byte) 0x11,
    (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x08, (Byte) 0x0c,
    (Byte) 0x0a, (Byte) 0x53, (Byte) 0x6f, (Byte) 0x6d, (Byte) 0x65, (Byte) 0x2d,
    (Byte) 0x53, (Byte) 0x74, (Byte) 0x61, (Byte) 0x74, (Byte) 0x65, (Byte) 0x31,
    (Byte) 0x21, (Byte) 0x30, (Byte) 0x1f, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x04, (Byte) 0x0a, (Byte) 0x0c, (Byte) 0x18, (Byte) 0x49, (Byte) 0x6e,
    (Byte) 0x74, (Byte) 0x65, (Byte) 0x72, (Byte) 0x6e, (Byte) 0x65, (Byte) 0x74,
    (Byte) 0x20, (Byte) 0x57, (Byte) 0x69, (Byte) 0x64, (Byte) 0x67, (Byte) 0x69,
    (Byte) 0x74, (Byte) 0x73, (Byte) 0x20, (Byte) 0x50, (Byte) 0x74, (Byte) 0x79,
    (Byte) 0x20, (Byte) 0x4c, (Byte) 0x74, (Byte) 0x64, (Byte) 0x31, (Byte) 0x17,
    (Byte) 0x30, (Byte) 0x15, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04,
    (Byte) 0x03, (Byte) 0x0c, (Byte) 0x0e, (Byte) 0x63, (Byte) 0x61, (Byte) 0x2e,
    (Byte) 0x65, (Byte) 0x78, (Byte) 0x61, (Byte) 0x6d, (Byte) 0x70, (Byte) 0x6c,
    (Byte) 0x65, (Byte) 0x2e, (Byte) 0x63, (Byte) 0x6f, (Byte) 0x6d, (Byte) 0x30,
    (Byte) 0x81, (Byte) 0x9f, (Byte) 0x30, (Byte) 0x0d, (Byte) 0x06, (Byte) 0x09,
    (Byte) 0x2a, (Byte) 0x86, (Byte) 0x48, (Byte) 0x86, (Byte) 0xf7, (Byte) 0x0d,
    (Byte) 0x01, (Byte) 0x01, (Byte) 0x01, (Byte) 0x05, (Byte) 0x00, (Byte) 0x03,
    (Byte) 0x81, (Byte) 0x8d, (Byte) 0x00, (Byte) 0x30, (Byte) 0x81, (Byte) 0x89,
    (Byte) 0x02, (Byte) 0x81, (Byte) 0x81, (Byte) 0x00, (Byte) 0xa4, (Byte) 0xc7,
    (Byte) 0x06, (Byte) 0xba, (Byte) 0xdf, (Byte) 0x2b, (Byte) 0xee, (Byte) 0xd2,
    (Byte) 0xb9, (Byte) 0xe4, (Byte) 0x52, (Byte) 0x21, (Byte) 0x68, (Byte) 0x2b,
    (Byte) 0x83, (Byte) 0xdf, (Byte) 0xe3, (Byte) 0x9c, (Byte) 0x08, (Byte) 0x73,
    (Byte) 0xdd, (Byte) 0x90, (Byte) 0xea, (Byte) 0x97, (Byte) 0x0c, (Byte) 0x96,
    (Byte) 0x20, (Byte) 0xb1, (Byte) 0xee, (Byte) 0x11, (Byte) 0xd5, (Byte) 0xd4,
    (Byte) 0x7c, (Byte) 0x44, (Byte) 0x96, (Byte) 0x2e, (Byte) 0x6e, (Byte) 0xa2,
    (Byte) 0xb2, (Byte) 0xa3, (Byte) 0x4b, (Byte) 0x0f, (Byte) 0x32, (Byte) 0x90,
    (Byte) 0xaf, (Byte) 0x5c, (Byte) 0x6f, (Byte) 0x00, (Byte) 0x88, (Byte) 0x45,
    (Byte) 0x4e, (Byte) 0x9b, (Byte) 0x26, (Byte) 0xc1, (Byte) 0x94, (Byte) 0x3c,
    (Byte) 0xfe, (Byte) 0x10, (Byte) 0xbd, (Byte) 0xda, (Byte) 0xf2, (Byte) 0x8d,
    (Byte) 0x03, (Byte) 0x52, (Byte) 0x32, (Byte) 0x11, (Byte) 0xff, (Byte) 0xf6,
    (Byte) 0xf9, (Byte) 0x6e, (Byte) 0x8f, (Byte) 0x0f, (Byte) 0xc8, (Byte) 0x0a,
    (Byte) 0x48, (Byte) 0x39, (Byte) 0x33, (Byte) 0xb9, (Byte) 0x0c, (Byte) 0xb3,
    (Byte) 0x2b, (Byte) 0xab, (Byte) 0x7d, (Byte) 0x79, (Byte) 0x6f, (Byte) 0x57,
    (Byte) 0x5b, (Byte) 0xb8, (Byte) 0x84, (Byte) 0xb6, (Byte) 0xcc, (Byte) 0xe8,
    (Byte) 0x30, (Byte) 0x78, (Byte) 0xff, (Byte) 0x92, (Byte) 0xe5, (Byte) 0x43,
    (Byte) 0x2e, (Byte) 0xef, (Byte) 0x66, (Byte) 0x98, (Byte) 0xb4, (Byte) 0xfe,
    (Byte) 0xa2, (Byte) 0x40, (Byte) 0xf2, (Byte) 0x1f, (Byte) 0xd0, (Byte) 0x86,
    (Byte) 0x16, (Byte) 0xc8, (Byte) 0x45, (Byte) 0xc4, (Byte) 0x52, (Byte) 0xcb,
    (Byte) 0x31, (Byte) 0x5c, (Byte) 0x9f, (Byte) 0x32, (Byte) 0x3b, (Byte) 0xf7,
    (Byte) 0x19, (Byte) 0x08, (Byte) 0xc7, (Byte) 0x00, (Byte) 0x21, (Byte) 0x7d,
    (Byte) 0x02, (Byte) 0x03, (Byte) 0x01, (Byte) 0x00, (Byte) 0x01, (Byte) 0xa3,
    (Byte) 0x50, (Byte) 0x30, (Byte) 0x4e, (Byte) 0x30, (Byte) 0x1d, (Byte) 0x06,
    (Byte) 0x03, (Byte) 0x55, (Byte) 0x1d, (Byte) 0x0e, (Byte) 0x04, (Byte) 0x16,
    (Byte) 0x04, (Byte) 0x14, (Byte) 0x47, (Byte) 0x82, (Byte) 0xa3, (Byte) 0xf1,
    (Byte) 0xc2, (Byte) 0x7e, (Byte) 0x3a, (Byte) 0xde, (Byte) 0x4f, (Byte) 0x30,
    (Byte) 0x4c, (Byte) 0x7f, (Byte) 0x72, (Byte) 0x81, (Byte) 0x15, (Byte) 0x32,
    (Byte) 0xda, (Byte) 0x7f, (Byte) 0x58, (Byte) 0x18, (Byte) 0x30, (Byte) 0x1f,
    (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x1d, (Byte) 0x23, (Byte) 0x04,
    (Byte) 0x18, (Byte) 0x30, (Byte) 0x16, (Byte) 0x80, (Byte) 0x14, (Byte) 0x47,
    (Byte) 0x82, (Byte) 0xa3, (Byte) 0xf1, (Byte) 0xc2, (Byte) 0x7e, (Byte) 0x3a,
    (Byte) 0xde, (Byte) 0x4f, (Byte) 0x30, (Byte) 0x4c, (Byte) 0x7f, (Byte) 0x72,
    (Byte) 0x81, (Byte) 0x15, (Byte) 0x32, (Byte) 0xda, (Byte) 0x7f, (Byte) 0x58,
    (Byte) 0x18, (Byte) 0x30, (Byte) 0x0c, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x1d, (Byte) 0x13, (Byte) 0x04, (Byte) 0x05, (Byte) 0x30, (Byte) 0x03,
    (Byte) 0x01, (Byte) 0x01, (Byte) 0xff, (Byte) 0x30, (Byte) 0x0d, (Byte) 0x06,
    (Byte) 0x09, (Byte) 0x2a, (Byte) 0x86, (Byte) 0x48, (Byte) 0x86, (Byte) 0xf7,
    (Byte) 0x0d, (Byte) 0x01, (Byte) 0x01, (Byte) 0x05, (Byte) 0x05, (Byte) 0x00,
    (Byte) 0x03, (Byte) 0x81, (Byte) 0x81, (Byte) 0x00, (Byte) 0x08, (Byte) 0x7f,
    (Byte) 0x6a, (Byte) 0x48, (Byte) 0x90, (Byte) 0x7b, (Byte) 0x9b, (Byte) 0x72,
    (Byte) 0x13, (Byte) 0xa7, (Byte) 0xef, (Byte) 0x6b, (Byte) 0x0b, (Byte) 0x59,
    (Byte) 0xe5, (Byte) 0x49, (Byte) 0x72, (Byte) 0x3a, (Byte) 0xc8, (Byte) 0x84,
    (Byte) 0xcc, (Byte) 0x23, (Byte) 0x18, (Byte) 0x4c, (Byte) 0xec, (Byte) 0xc7,
    (Byte) 0xef, (Byte) 0xcb, (Byte) 0xa7, (Byte) 0xbe, (Byte) 0xe4, (Byte) 0xef,
    (Byte) 0x8f, (Byte) 0xc6, (Byte) 0x06, (Byte) 0x8c, (Byte) 0xc0, (Byte) 0xe4,
    (Byte) 0x2f, (Byte) 0x2a, (Byte) 0xc0, (Byte) 0x35, (Byte) 0x7d, (Byte) 0x5e,
    (Byte) 0x19, (Byte) 0x29, (Byte) 0x8c, (Byte) 0xb9, (Byte) 0xf1, (Byte) 0x1e,
    (Byte) 0xaf, (Byte) 0x82, (Byte) 0xd8, (Byte) 0xe3, (Byte) 0x88, (Byte) 0xe1,
    (Byte) 0x31, (Byte) 0xc8, (Byte) 0x82, (Byte) 0x1f, (Byte) 0x83, (Byte) 0xa9,
    (Byte) 0xde, (Byte) 0xfe, (Byte) 0x4b, (Byte) 0xe2, (Byte) 0x78, (Byte) 0x64,
    (Byte) 0xed, (Byte) 0xa4, (Byte) 0x7b, (Byte) 0xee, (Byte) 0x8d, (Byte) 0x71,
    (Byte) 0x1b, (Byte) 0x44, (Byte) 0xe6, (Byte) 0xb7, (Byte) 0xe8, (Byte) 0xc5,
    (Byte) 0x9a, (Byte) 0x93, (Byte) 0x92, (Byte) 0x6f, (Byte) 0x6f, (Byte) 0xdb,
    (Byte) 0xbd, (Byte) 0xd7, (Byte) 0x03, (Byte) 0x85, (Byte) 0xa9, (Byte) 0x5f,
    (Byte) 0x53, (Byte) 0x5f, (Byte) 0x5d, (Byte) 0x30, (Byte) 0xc6, (Byte) 0xd9,
    (Byte) 0xce, (Byte) 0x34, (Byte) 0xa8, (Byte) 0xbe, (Byte) 0x31, (Byte) 0x47,
    (Byte) 0x1c, (Byte) 0xa4, (Byte) 0x7f, (Byte) 0xc0, (Byte) 0x2c, (Byte) 0xbc,
    (Byte) 0xfe, (Byte) 0x1a, (Byte) 0x31, (Byte) 0xd8, (Byte) 0x77, (Byte) 0x4d,
    (Byte) 0xfc, (Byte) 0x45, (Byte) 0x84, (Byte) 0xfc, (Byte) 0x45, (Byte) 0x12,
    (Byte) 0xab, (Byte) 0x50, (Byte) 0xe4, (Byte) 0x45, (Byte) 0xe5, (Byte) 0x11
};

static AutoPtr<ArrayOf<Byte> > FAKE_DSA_CA_1 = ArrayOf<Byte>::AllocInplace(_FAKE_DSA_CA_1
        , ArraySize(_FAKE_DSA_CA_1));

/**
 * Generated from above and converted with: openssl pkcs8 -topk8 -outform d
 * -in userkey.pem -nocrypt | xxd -i | sed 's/0x/(Byte) 0x/g'
 */
static Byte _FAKE_DSA_KEY_1[] = {
    (Byte) 0x30, (Byte) 0x82, (Byte) 0x01, (Byte) 0x4c, (Byte) 0x02, (Byte) 0x01,
    (Byte) 0x00, (Byte) 0x30, (Byte) 0x82, (Byte) 0x01, (Byte) 0x2c, (Byte) 0x06,
    (Byte) 0x07, (Byte) 0x2a, (Byte) 0x86, (Byte) 0x48, (Byte) 0xce, (Byte) 0x38,
    (Byte) 0x04, (Byte) 0x01, (Byte) 0x30, (Byte) 0x82, (Byte) 0x01, (Byte) 0x1f,
    (Byte) 0x02, (Byte) 0x81, (Byte) 0x81, (Byte) 0x00, (Byte) 0xb3, (Byte) 0x23,
    (Byte) 0xf7, (Byte) 0x86, (Byte) 0xbd, (Byte) 0x3b, (Byte) 0x86, (Byte) 0xcc,
    (Byte) 0xc3, (Byte) 0x91, (Byte) 0xc0, (Byte) 0x30, (Byte) 0x32, (Byte) 0x02,
    (Byte) 0x47, (Byte) 0x35, (Byte) 0x01, (Byte) 0xef, (Byte) 0xee, (Byte) 0x98,
    (Byte) 0x13, (Byte) 0x56, (Byte) 0x49, (Byte) 0x47, (Byte) 0xb5, (Byte) 0x20,
    (Byte) 0xa8, (Byte) 0x60, (Byte) 0xcb, (Byte) 0xc0, (Byte) 0xd5, (Byte) 0x77,
    (Byte) 0xc1, (Byte) 0x69, (Byte) 0xcd, (Byte) 0x18, (Byte) 0x34, (Byte) 0x92,
    (Byte) 0xf2, (Byte) 0x6a, (Byte) 0x2a, (Byte) 0x10, (Byte) 0x59, (Byte) 0x1c,
    (Byte) 0x91, (Byte) 0x20, (Byte) 0x51, (Byte) 0xca, (Byte) 0x37, (Byte) 0xb2,
    (Byte) 0x87, (Byte) 0xa6, (Byte) 0x8a, (Byte) 0x02, (Byte) 0xfd, (Byte) 0x45,
    (Byte) 0x46, (Byte) 0xf9, (Byte) 0x76, (Byte) 0xb1, (Byte) 0x35, (Byte) 0x38,
    (Byte) 0x8d, (Byte) 0xff, (Byte) 0x4c, (Byte) 0x5d, (Byte) 0x75, (Byte) 0x8f,
    (Byte) 0x66, (Byte) 0x15, (Byte) 0x7d, (Byte) 0x7b, (Byte) 0xda, (Byte) 0xdb,
    (Byte) 0x57, (Byte) 0x39, (Byte) 0xff, (Byte) 0x91, (Byte) 0x3f, (Byte) 0xdd,
    (Byte) 0xe2, (Byte) 0xb4, (Byte) 0x22, (Byte) 0x60, (Byte) 0x4c, (Byte) 0x32,
    (Byte) 0x3b, (Byte) 0x9d, (Byte) 0x34, (Byte) 0x9f, (Byte) 0xb9, (Byte) 0x5d,
    (Byte) 0x75, (Byte) 0xb9, (Byte) 0xd3, (Byte) 0x7f, (Byte) 0x11, (Byte) 0xba,
    (Byte) 0xb7, (Byte) 0xc8, (Byte) 0x32, (Byte) 0xc6, (Byte) 0xce, (Byte) 0x71,
    (Byte) 0x91, (Byte) 0xd3, (Byte) 0x32, (Byte) 0xaf, (Byte) 0x4d, (Byte) 0x7e,
    (Byte) 0x7c, (Byte) 0x15, (Byte) 0xf7, (Byte) 0x71, (Byte) 0x2c, (Byte) 0x52,
    (Byte) 0x65, (Byte) 0x4d, (Byte) 0xa9, (Byte) 0x81, (Byte) 0x25, (Byte) 0x35,
    (Byte) 0xce, (Byte) 0x0b, (Byte) 0x5b, (Byte) 0x56, (Byte) 0xfe, (Byte) 0xf1,
    (Byte) 0x02, (Byte) 0x15, (Byte) 0x00, (Byte) 0xeb, (Byte) 0x4e, (Byte) 0x7f,
    (Byte) 0x7a, (Byte) 0x31, (Byte) 0xb3, (Byte) 0x7d, (Byte) 0x8d, (Byte) 0xb2,
    (Byte) 0xf7, (Byte) 0xaf, (Byte) 0xad, (Byte) 0xb1, (Byte) 0x42, (Byte) 0x92,
    (Byte) 0xf3, (Byte) 0x6c, (Byte) 0xe4, (Byte) 0xed, (Byte) 0x8b, (Byte) 0x02,
    (Byte) 0x81, (Byte) 0x81, (Byte) 0x00, (Byte) 0x81, (Byte) 0xc8, (Byte) 0x36,
    (Byte) 0x48, (Byte) 0xdb, (Byte) 0x71, (Byte) 0x2b, (Byte) 0x91, (Byte) 0xce,
    (Byte) 0x6d, (Byte) 0xbc, (Byte) 0xb8, (Byte) 0xf9, (Byte) 0xcb, (Byte) 0x50,
    (Byte) 0x91, (Byte) 0x10, (Byte) 0x8a, (Byte) 0xf8, (Byte) 0x37, (Byte) 0x50,
    (Byte) 0xda, (Byte) 0x4f, (Byte) 0xc8, (Byte) 0x4d, (Byte) 0x73, (Byte) 0xcb,
    (Byte) 0x4d, (Byte) 0xb0, (Byte) 0x19, (Byte) 0x54, (Byte) 0x5a, (Byte) 0xf3,
    (Byte) 0x6c, (Byte) 0xc9, (Byte) 0xd8, (Byte) 0x96, (Byte) 0xd9, (Byte) 0xb0,
    (Byte) 0x54, (Byte) 0x7e, (Byte) 0x7d, (Byte) 0xe2, (Byte) 0x58, (Byte) 0x0e,
    (Byte) 0x5f, (Byte) 0xc0, (Byte) 0xce, (Byte) 0xb9, (Byte) 0x5c, (Byte) 0xe3,
    (Byte) 0xd3, (Byte) 0xdf, (Byte) 0xcf, (Byte) 0x45, (Byte) 0x74, (Byte) 0xfb,
    (Byte) 0xe6, (Byte) 0x20, (Byte) 0xe7, (Byte) 0xfc, (Byte) 0x0f, (Byte) 0xca,
    (Byte) 0xdb, (Byte) 0xc0, (Byte) 0x0b, (Byte) 0xe1, (Byte) 0x5a, (Byte) 0x16,
    (Byte) 0x1d, (Byte) 0xb3, (Byte) 0x2e, (Byte) 0xe5, (Byte) 0x5f, (Byte) 0x89,
    (Byte) 0x17, (Byte) 0x73, (Byte) 0x50, (Byte) 0xd1, (Byte) 0x4a, (Byte) 0x60,
    (Byte) 0xb7, (Byte) 0xaa, (Byte) 0xf0, (Byte) 0xc7, (Byte) 0xc5, (Byte) 0x03,
    (Byte) 0x4e, (Byte) 0x36, (Byte) 0x51, (Byte) 0x9e, (Byte) 0x2f, (Byte) 0xfa,
    (Byte) 0xf3, (Byte) 0xd6, (Byte) 0x58, (Byte) 0x14, (Byte) 0x02, (Byte) 0xb4,
    (Byte) 0x41, (Byte) 0xd6, (Byte) 0x72, (Byte) 0x6f, (Byte) 0x58, (Byte) 0x5b,
    (Byte) 0x2d, (Byte) 0x23, (Byte) 0xc0, (Byte) 0x75, (Byte) 0x4f, (Byte) 0x39,
    (Byte) 0xa8, (Byte) 0x6a, (Byte) 0xdf, (Byte) 0x79, (Byte) 0x21, (Byte) 0xf2,
    (Byte) 0x77, (Byte) 0x91, (Byte) 0x3f, (Byte) 0x1c, (Byte) 0x4d, (Byte) 0x48,
    (Byte) 0x78, (Byte) 0xcd, (Byte) 0xed, (Byte) 0x79, (Byte) 0x23, (Byte) 0x04,
    (Byte) 0x17, (Byte) 0x02, (Byte) 0x15, (Byte) 0x00, (Byte) 0xc7, (Byte) 0xe7,
    (Byte) 0xe2, (Byte) 0x6b, (Byte) 0x14, (Byte) 0xe6, (Byte) 0x31, (Byte) 0x12,
    (Byte) 0xb2, (Byte) 0x1e, (Byte) 0xd4, (Byte) 0xf2, (Byte) 0x9b, (Byte) 0x2c,
    (Byte) 0xf6, (Byte) 0x54, (Byte) 0x4c, (Byte) 0x12, (Byte) 0xe8, (Byte) 0x22
};

static AutoPtr<ArrayOf<Byte> > FAKE_DSA_KEY_1 = ArrayOf<Byte>::AllocInplace(_FAKE_DSA_KEY_1
        , ArraySize(_FAKE_DSA_KEY_1));

/**
 * Generated from above and converted with: openssl x509 -outform d -in
 * usercert.pem | xxd -i | sed 's/0x/(Byte) 0x/g'
 */
static Byte _FAKE_DSA_USER_1[] = {
    (Byte) 0x30, (Byte) 0x82, (Byte) 0x03, (Byte) 0xca, (Byte) 0x30, (Byte) 0x82,
    (Byte) 0x03, (Byte) 0x33, (Byte) 0xa0, (Byte) 0x03, (Byte) 0x02, (Byte) 0x01,
    (Byte) 0x02, (Byte) 0x02, (Byte) 0x01, (Byte) 0x01, (Byte) 0x30, (Byte) 0x0d,
    (Byte) 0x06, (Byte) 0x09, (Byte) 0x2a, (Byte) 0x86, (Byte) 0x48, (Byte) 0x86,
    (Byte) 0xf7, (Byte) 0x0d, (Byte) 0x01, (Byte) 0x01, (Byte) 0x05, (Byte) 0x05,
    (Byte) 0x00, (Byte) 0x30, (Byte) 0x5e, (Byte) 0x31, (Byte) 0x0b, (Byte) 0x30,
    (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x06,
    (Byte) 0x13, (Byte) 0x02, (Byte) 0x41, (Byte) 0x55, (Byte) 0x31, (Byte) 0x13,
    (Byte) 0x30, (Byte) 0x11, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04,
    (Byte) 0x08, (Byte) 0x0c, (Byte) 0x0a, (Byte) 0x53, (Byte) 0x6f, (Byte) 0x6d,
    (Byte) 0x65, (Byte) 0x2d, (Byte) 0x53, (Byte) 0x74, (Byte) 0x61, (Byte) 0x74,
    (Byte) 0x65, (Byte) 0x31, (Byte) 0x21, (Byte) 0x30, (Byte) 0x1f, (Byte) 0x06,
    (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x0a, (Byte) 0x0c, (Byte) 0x18,
    (Byte) 0x49, (Byte) 0x6e, (Byte) 0x74, (Byte) 0x65, (Byte) 0x72, (Byte) 0x6e,
    (Byte) 0x65, (Byte) 0x74, (Byte) 0x20, (Byte) 0x57, (Byte) 0x69, (Byte) 0x64,
    (Byte) 0x67, (Byte) 0x69, (Byte) 0x74, (Byte) 0x73, (Byte) 0x20, (Byte) 0x50,
    (Byte) 0x74, (Byte) 0x79, (Byte) 0x20, (Byte) 0x4c, (Byte) 0x74, (Byte) 0x64,
    (Byte) 0x31, (Byte) 0x17, (Byte) 0x30, (Byte) 0x15, (Byte) 0x06, (Byte) 0x03,
    (Byte) 0x55, (Byte) 0x04, (Byte) 0x03, (Byte) 0x0c, (Byte) 0x0e, (Byte) 0x63,
    (Byte) 0x61, (Byte) 0x2e, (Byte) 0x65, (Byte) 0x78, (Byte) 0x61, (Byte) 0x6d,
    (Byte) 0x70, (Byte) 0x6c, (Byte) 0x65, (Byte) 0x2e, (Byte) 0x63, (Byte) 0x6f,
    (Byte) 0x6d, (Byte) 0x30, (Byte) 0x1e, (Byte) 0x17, (Byte) 0x0d, (Byte) 0x31,
    (Byte) 0x33, (Byte) 0x30, (Byte) 0x38, (Byte) 0x32, (Byte) 0x37, (Byte) 0x32,
    (Byte) 0x33, (Byte) 0x33, (Byte) 0x34, (Byte) 0x32, (Byte) 0x32, (Byte) 0x5a,
    (Byte) 0x17, (Byte) 0x0d, (Byte) 0x32, (Byte) 0x33, (Byte) 0x30, (Byte) 0x38,
    (Byte) 0x32, (Byte) 0x35, (Byte) 0x32, (Byte) 0x33, (Byte) 0x33, (Byte) 0x34,
    (Byte) 0x32, (Byte) 0x32, (Byte) 0x5a, (Byte) 0x30, (Byte) 0x62, (Byte) 0x31,
    (Byte) 0x0b, (Byte) 0x30, (Byte) 0x09, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x04, (Byte) 0x06, (Byte) 0x13, (Byte) 0x02, (Byte) 0x41, (Byte) 0x55,
    (Byte) 0x31, (Byte) 0x13, (Byte) 0x30, (Byte) 0x11, (Byte) 0x06, (Byte) 0x03,
    (Byte) 0x55, (Byte) 0x04, (Byte) 0x08, (Byte) 0x0c, (Byte) 0x0a, (Byte) 0x53,
    (Byte) 0x6f, (Byte) 0x6d, (Byte) 0x65, (Byte) 0x2d, (Byte) 0x53, (Byte) 0x74,
    (Byte) 0x61, (Byte) 0x74, (Byte) 0x65, (Byte) 0x31, (Byte) 0x21, (Byte) 0x30,
    (Byte) 0x1f, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x0a,
    (Byte) 0x0c, (Byte) 0x18, (Byte) 0x49, (Byte) 0x6e, (Byte) 0x74, (Byte) 0x65,
    (Byte) 0x72, (Byte) 0x6e, (Byte) 0x65, (Byte) 0x74, (Byte) 0x20, (Byte) 0x57,
    (Byte) 0x69, (Byte) 0x64, (Byte) 0x67, (Byte) 0x69, (Byte) 0x74, (Byte) 0x73,
    (Byte) 0x20, (Byte) 0x50, (Byte) 0x74, (Byte) 0x79, (Byte) 0x20, (Byte) 0x4c,
    (Byte) 0x74, (Byte) 0x64, (Byte) 0x31, (Byte) 0x1b, (Byte) 0x30, (Byte) 0x19,
    (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x04, (Byte) 0x03, (Byte) 0x0c,
    (Byte) 0x12, (Byte) 0x73, (Byte) 0x65, (Byte) 0x72, (Byte) 0x76, (Byte) 0x65,
    (Byte) 0x72, (Byte) 0x2e, (Byte) 0x65, (Byte) 0x78, (Byte) 0x61, (Byte) 0x6d,
    (Byte) 0x70, (Byte) 0x6c, (Byte) 0x65, (Byte) 0x2e, (Byte) 0x63, (Byte) 0x6f,
    (Byte) 0x6d, (Byte) 0x30, (Byte) 0x82, (Byte) 0x01, (Byte) 0xb7, (Byte) 0x30,
    (Byte) 0x82, (Byte) 0x01, (Byte) 0x2c, (Byte) 0x06, (Byte) 0x07, (Byte) 0x2a,
    (Byte) 0x86, (Byte) 0x48, (Byte) 0xce, (Byte) 0x38, (Byte) 0x04, (Byte) 0x01,
    (Byte) 0x30, (Byte) 0x82, (Byte) 0x01, (Byte) 0x1f, (Byte) 0x02, (Byte) 0x81,
    (Byte) 0x81, (Byte) 0x00, (Byte) 0xb3, (Byte) 0x23, (Byte) 0xf7, (Byte) 0x86,
    (Byte) 0xbd, (Byte) 0x3b, (Byte) 0x86, (Byte) 0xcc, (Byte) 0xc3, (Byte) 0x91,
    (Byte) 0xc0, (Byte) 0x30, (Byte) 0x32, (Byte) 0x02, (Byte) 0x47, (Byte) 0x35,
    (Byte) 0x01, (Byte) 0xef, (Byte) 0xee, (Byte) 0x98, (Byte) 0x13, (Byte) 0x56,
    (Byte) 0x49, (Byte) 0x47, (Byte) 0xb5, (Byte) 0x20, (Byte) 0xa8, (Byte) 0x60,
    (Byte) 0xcb, (Byte) 0xc0, (Byte) 0xd5, (Byte) 0x77, (Byte) 0xc1, (Byte) 0x69,
    (Byte) 0xcd, (Byte) 0x18, (Byte) 0x34, (Byte) 0x92, (Byte) 0xf2, (Byte) 0x6a,
    (Byte) 0x2a, (Byte) 0x10, (Byte) 0x59, (Byte) 0x1c, (Byte) 0x91, (Byte) 0x20,
    (Byte) 0x51, (Byte) 0xca, (Byte) 0x37, (Byte) 0xb2, (Byte) 0x87, (Byte) 0xa6,
    (Byte) 0x8a, (Byte) 0x02, (Byte) 0xfd, (Byte) 0x45, (Byte) 0x46, (Byte) 0xf9,
    (Byte) 0x76, (Byte) 0xb1, (Byte) 0x35, (Byte) 0x38, (Byte) 0x8d, (Byte) 0xff,
    (Byte) 0x4c, (Byte) 0x5d, (Byte) 0x75, (Byte) 0x8f, (Byte) 0x66, (Byte) 0x15,
    (Byte) 0x7d, (Byte) 0x7b, (Byte) 0xda, (Byte) 0xdb, (Byte) 0x57, (Byte) 0x39,
    (Byte) 0xff, (Byte) 0x91, (Byte) 0x3f, (Byte) 0xdd, (Byte) 0xe2, (Byte) 0xb4,
    (Byte) 0x22, (Byte) 0x60, (Byte) 0x4c, (Byte) 0x32, (Byte) 0x3b, (Byte) 0x9d,
    (Byte) 0x34, (Byte) 0x9f, (Byte) 0xb9, (Byte) 0x5d, (Byte) 0x75, (Byte) 0xb9,
    (Byte) 0xd3, (Byte) 0x7f, (Byte) 0x11, (Byte) 0xba, (Byte) 0xb7, (Byte) 0xc8,
    (Byte) 0x32, (Byte) 0xc6, (Byte) 0xce, (Byte) 0x71, (Byte) 0x91, (Byte) 0xd3,
    (Byte) 0x32, (Byte) 0xaf, (Byte) 0x4d, (Byte) 0x7e, (Byte) 0x7c, (Byte) 0x15,
    (Byte) 0xf7, (Byte) 0x71, (Byte) 0x2c, (Byte) 0x52, (Byte) 0x65, (Byte) 0x4d,
    (Byte) 0xa9, (Byte) 0x81, (Byte) 0x25, (Byte) 0x35, (Byte) 0xce, (Byte) 0x0b,
    (Byte) 0x5b, (Byte) 0x56, (Byte) 0xfe, (Byte) 0xf1, (Byte) 0x02, (Byte) 0x15,
    (Byte) 0x00, (Byte) 0xeb, (Byte) 0x4e, (Byte) 0x7f, (Byte) 0x7a, (Byte) 0x31,
    (Byte) 0xb3, (Byte) 0x7d, (Byte) 0x8d, (Byte) 0xb2, (Byte) 0xf7, (Byte) 0xaf,
    (Byte) 0xad, (Byte) 0xb1, (Byte) 0x42, (Byte) 0x92, (Byte) 0xf3, (Byte) 0x6c,
    (Byte) 0xe4, (Byte) 0xed, (Byte) 0x8b, (Byte) 0x02, (Byte) 0x81, (Byte) 0x81,
    (Byte) 0x00, (Byte) 0x81, (Byte) 0xc8, (Byte) 0x36, (Byte) 0x48, (Byte) 0xdb,
    (Byte) 0x71, (Byte) 0x2b, (Byte) 0x91, (Byte) 0xce, (Byte) 0x6d, (Byte) 0xbc,
    (Byte) 0xb8, (Byte) 0xf9, (Byte) 0xcb, (Byte) 0x50, (Byte) 0x91, (Byte) 0x10,
    (Byte) 0x8a, (Byte) 0xf8, (Byte) 0x37, (Byte) 0x50, (Byte) 0xda, (Byte) 0x4f,
    (Byte) 0xc8, (Byte) 0x4d, (Byte) 0x73, (Byte) 0xcb, (Byte) 0x4d, (Byte) 0xb0,
    (Byte) 0x19, (Byte) 0x54, (Byte) 0x5a, (Byte) 0xf3, (Byte) 0x6c, (Byte) 0xc9,
    (Byte) 0xd8, (Byte) 0x96, (Byte) 0xd9, (Byte) 0xb0, (Byte) 0x54, (Byte) 0x7e,
    (Byte) 0x7d, (Byte) 0xe2, (Byte) 0x58, (Byte) 0x0e, (Byte) 0x5f, (Byte) 0xc0,
    (Byte) 0xce, (Byte) 0xb9, (Byte) 0x5c, (Byte) 0xe3, (Byte) 0xd3, (Byte) 0xdf,
    (Byte) 0xcf, (Byte) 0x45, (Byte) 0x74, (Byte) 0xfb, (Byte) 0xe6, (Byte) 0x20,
    (Byte) 0xe7, (Byte) 0xfc, (Byte) 0x0f, (Byte) 0xca, (Byte) 0xdb, (Byte) 0xc0,
    (Byte) 0x0b, (Byte) 0xe1, (Byte) 0x5a, (Byte) 0x16, (Byte) 0x1d, (Byte) 0xb3,
    (Byte) 0x2e, (Byte) 0xe5, (Byte) 0x5f, (Byte) 0x89, (Byte) 0x17, (Byte) 0x73,
    (Byte) 0x50, (Byte) 0xd1, (Byte) 0x4a, (Byte) 0x60, (Byte) 0xb7, (Byte) 0xaa,
    (Byte) 0xf0, (Byte) 0xc7, (Byte) 0xc5, (Byte) 0x03, (Byte) 0x4e, (Byte) 0x36,
    (Byte) 0x51, (Byte) 0x9e, (Byte) 0x2f, (Byte) 0xfa, (Byte) 0xf3, (Byte) 0xd6,
    (Byte) 0x58, (Byte) 0x14, (Byte) 0x02, (Byte) 0xb4, (Byte) 0x41, (Byte) 0xd6,
    (Byte) 0x72, (Byte) 0x6f, (Byte) 0x58, (Byte) 0x5b, (Byte) 0x2d, (Byte) 0x23,
    (Byte) 0xc0, (Byte) 0x75, (Byte) 0x4f, (Byte) 0x39, (Byte) 0xa8, (Byte) 0x6a,
    (Byte) 0xdf, (Byte) 0x79, (Byte) 0x21, (Byte) 0xf2, (Byte) 0x77, (Byte) 0x91,
    (Byte) 0x3f, (Byte) 0x1c, (Byte) 0x4d, (Byte) 0x48, (Byte) 0x78, (Byte) 0xcd,
    (Byte) 0xed, (Byte) 0x79, (Byte) 0x23, (Byte) 0x03, (Byte) 0x81, (Byte) 0x84,
    (Byte) 0x00, (Byte) 0x02, (Byte) 0x81, (Byte) 0x80, (Byte) 0x1a, (Byte) 0x50,
    (Byte) 0x9d, (Byte) 0x3e, (Byte) 0xa1, (Byte) 0x6c, (Byte) 0x99, (Byte) 0x35,
    (Byte) 0x36, (Byte) 0x26, (Byte) 0x22, (Byte) 0x6b, (Byte) 0x47, (Byte) 0x45,
    (Byte) 0x80, (Byte) 0x5b, (Byte) 0xd5, (Byte) 0xc1, (Byte) 0xc5, (Byte) 0x70,
    (Byte) 0x75, (Byte) 0x55, (Byte) 0x66, (Byte) 0x33, (Byte) 0x1d, (Byte) 0xae,
    (Byte) 0xd0, (Byte) 0x01, (Byte) 0x64, (Byte) 0x8b, (Byte) 0xae, (Byte) 0x9d,
    (Byte) 0x66, (Byte) 0x58, (Byte) 0xf9, (Byte) 0x42, (Byte) 0x74, (Byte) 0x3a,
    (Byte) 0x32, (Byte) 0xc7, (Byte) 0x7f, (Byte) 0x25, (Byte) 0x64, (Byte) 0x7d,
    (Byte) 0x08, (Byte) 0x26, (Byte) 0xbf, (Byte) 0x21, (Byte) 0x3a, (Byte) 0x84,
    (Byte) 0xcc, (Byte) 0x2c, (Byte) 0x66, (Byte) 0x7d, (Byte) 0xc7, (Byte) 0xd6,
    (Byte) 0xb1, (Byte) 0x69, (Byte) 0x57, (Byte) 0x67, (Byte) 0x52, (Byte) 0x73,
    (Byte) 0x3f, (Byte) 0x79, (Byte) 0x60, (Byte) 0xaa, (Byte) 0xf4, (Byte) 0x8a,
    (Byte) 0x48, (Byte) 0x42, (Byte) 0x46, (Byte) 0x41, (Byte) 0xd0, (Byte) 0x50,
    (Byte) 0x9b, (Byte) 0xa2, (Byte) 0x4e, (Byte) 0xa5, (Byte) 0x88, (Byte) 0x10,
    (Byte) 0xf7, (Byte) 0x61, (Byte) 0xa2, (Byte) 0xfa, (Byte) 0x8d, (Byte) 0xa6,
    (Byte) 0x13, (Byte) 0x9e, (Byte) 0x36, (Byte) 0x86, (Byte) 0x62, (Byte) 0xf0,
    (Byte) 0x97, (Byte) 0xef, (Byte) 0x11, (Byte) 0xc6, (Byte) 0x35, (Byte) 0xd3,
    (Byte) 0x79, (Byte) 0x30, (Byte) 0xde, (Byte) 0xf2, (Byte) 0x7f, (Byte) 0x7a,
    (Byte) 0x3c, (Byte) 0x03, (Byte) 0xa3, (Byte) 0xc5, (Byte) 0xbc, (Byte) 0xb1,
    (Byte) 0xbc, (Byte) 0x2f, (Byte) 0x10, (Byte) 0xf4, (Byte) 0x51, (Byte) 0x89,
    (Byte) 0xe2, (Byte) 0xaf, (Byte) 0xf7, (Byte) 0x61, (Byte) 0x1a, (Byte) 0xf0,
    (Byte) 0x87, (Byte) 0x5e, (Byte) 0xa5, (Byte) 0x02, (Byte) 0xd2, (Byte) 0xe4,
    (Byte) 0xa3, (Byte) 0x7b, (Byte) 0x30, (Byte) 0x79, (Byte) 0x30, (Byte) 0x09,
    (Byte) 0x06, (Byte) 0x03, (Byte) 0x55, (Byte) 0x1d, (Byte) 0x13, (Byte) 0x04,
    (Byte) 0x02, (Byte) 0x30, (Byte) 0x00, (Byte) 0x30, (Byte) 0x2c, (Byte) 0x06,
    (Byte) 0x09, (Byte) 0x60, (Byte) 0x86, (Byte) 0x48, (Byte) 0x01, (Byte) 0x86,
    (Byte) 0xf8, (Byte) 0x42, (Byte) 0x01, (Byte) 0x0d, (Byte) 0x04, (Byte) 0x1f,
    (Byte) 0x16, (Byte) 0x1d, (Byte) 0x4f, (Byte) 0x70, (Byte) 0x65, (Byte) 0x6e,
    (Byte) 0x53, (Byte) 0x53, (Byte) 0x4c, (Byte) 0x20, (Byte) 0x47, (Byte) 0x65,
    (Byte) 0x6e, (Byte) 0x65, (Byte) 0x72, (Byte) 0x61, (Byte) 0x74, (Byte) 0x65,
    (Byte) 0x64, (Byte) 0x20, (Byte) 0x43, (Byte) 0x65, (Byte) 0x72, (Byte) 0x74,
    (Byte) 0x69, (Byte) 0x66, (Byte) 0x69, (Byte) 0x63, (Byte) 0x61, (Byte) 0x74,
    (Byte) 0x65, (Byte) 0x30, (Byte) 0x1d, (Byte) 0x06, (Byte) 0x03, (Byte) 0x55,
    (Byte) 0x1d, (Byte) 0x0e, (Byte) 0x04, (Byte) 0x16, (Byte) 0x04, (Byte) 0x14,
    (Byte) 0xd1, (Byte) 0x6c, (Byte) 0x36, (Byte) 0x36, (Byte) 0x61, (Byte) 0x6c,
    (Byte) 0xf6, (Byte) 0x90, (Byte) 0x82, (Byte) 0x82, (Byte) 0x87, (Byte) 0x93,
    (Byte) 0xbe, (Byte) 0x99, (Byte) 0x60, (Byte) 0x1b, (Byte) 0x03, (Byte) 0x58,
    (Byte) 0x36, (Byte) 0x63, (Byte) 0x30, (Byte) 0x1f, (Byte) 0x06, (Byte) 0x03,
    (Byte) 0x55, (Byte) 0x1d, (Byte) 0x23, (Byte) 0x04, (Byte) 0x18, (Byte) 0x30,
    (Byte) 0x16, (Byte) 0x80, (Byte) 0x14, (Byte) 0x47, (Byte) 0x82, (Byte) 0xa3,
    (Byte) 0xf1, (Byte) 0xc2, (Byte) 0x7e, (Byte) 0x3a, (Byte) 0xde, (Byte) 0x4f,
    (Byte) 0x30, (Byte) 0x4c, (Byte) 0x7f, (Byte) 0x72, (Byte) 0x81, (Byte) 0x15,
    (Byte) 0x32, (Byte) 0xda, (Byte) 0x7f, (Byte) 0x58, (Byte) 0x18, (Byte) 0x30,
    (Byte) 0x0d, (Byte) 0x06, (Byte) 0x09, (Byte) 0x2a, (Byte) 0x86, (Byte) 0x48,
    (Byte) 0x86, (Byte) 0xf7, (Byte) 0x0d, (Byte) 0x01, (Byte) 0x01, (Byte) 0x05,
    (Byte) 0x05, (Byte) 0x00, (Byte) 0x03, (Byte) 0x81, (Byte) 0x81, (Byte) 0x00,
    (Byte) 0x81, (Byte) 0xde, (Byte) 0x20, (Byte) 0xa1, (Byte) 0xb2, (Byte) 0x50,
    (Byte) 0x03, (Byte) 0xcd, (Byte) 0x90, (Byte) 0x4f, (Byte) 0x2b, (Byte) 0x47,
    (Byte) 0x1d, (Byte) 0xac, (Byte) 0x6e, (Byte) 0xb4, (Byte) 0xc7, (Byte) 0x14,
    (Byte) 0xc6, (Byte) 0x4f, (Byte) 0x45, (Byte) 0xaf, (Byte) 0x81, (Byte) 0x5d,
    (Byte) 0x5a, (Byte) 0x31, (Byte) 0xff, (Byte) 0x9c, (Byte) 0x4d, (Byte) 0xdc,
    (Byte) 0x9e, (Byte) 0x36, (Byte) 0x9f, (Byte) 0x9b, (Byte) 0xb1, (Byte) 0xc9,
    (Byte) 0x50, (Byte) 0xa3, (Byte) 0xf6, (Byte) 0x9c, (Byte) 0x68, (Byte) 0x6f,
    (Byte) 0x68, (Byte) 0xd9, (Byte) 0x56, (Byte) 0x1b, (Byte) 0xe5, (Byte) 0x1b,
    (Byte) 0x41, (Byte) 0xd4, (Byte) 0xcc, (Byte) 0xb6, (Byte) 0x37, (Byte) 0xd5,
    (Byte) 0x69, (Byte) 0x6b, (Byte) 0x39, (Byte) 0xaf, (Byte) 0xc6, (Byte) 0xb8,
    (Byte) 0x39, (Byte) 0x76, (Byte) 0xe3, (Byte) 0xf7, (Byte) 0x97, (Byte) 0x74,
    (Byte) 0x31, (Byte) 0xc4, (Byte) 0x2d, (Byte) 0xb7, (Byte) 0x9a, (Byte) 0xa4,
    (Byte) 0xfa, (Byte) 0x9f, (Byte) 0xa8, (Byte) 0xe3, (Byte) 0x41, (Byte) 0xda,
    (Byte) 0x2f, (Byte) 0x0c, (Byte) 0x9d, (Byte) 0x83, (Byte) 0xdc, (Byte) 0x86,
    (Byte) 0x1f, (Byte) 0x5c, (Byte) 0x0f, (Byte) 0x87, (Byte) 0x05, (Byte) 0xc9,
    (Byte) 0xb0, (Byte) 0x63, (Byte) 0xca, (Byte) 0x9b, (Byte) 0xdb, (Byte) 0xe6,
    (Byte) 0x3c, (Byte) 0xe9, (Byte) 0x23, (Byte) 0x9e, (Byte) 0x23, (Byte) 0x44,
    (Byte) 0x1d, (Byte) 0x5b, (Byte) 0x60, (Byte) 0x66, (Byte) 0xb6, (Byte) 0x72,
    (Byte) 0x8c, (Byte) 0x87, (Byte) 0x86, (Byte) 0xe8, (Byte) 0xdb, (Byte) 0x29,
    (Byte) 0x67, (Byte) 0x9c, (Byte) 0x33, (Byte) 0x5c, (Byte) 0x39, (Byte) 0xf1,
    (Byte) 0xb5, (Byte) 0x9b, (Byte) 0xb8, (Byte) 0xe1, (Byte) 0x42, (Byte) 0x51,
    (Byte) 0xed, (Byte) 0x2c
};

static AutoPtr<ArrayOf<Byte> > FAKE_DSA_USER_1 = ArrayOf<Byte>::AllocInplace(_FAKE_DSA_USER_1
        , ArraySize(_FAKE_DSA_USER_1));

const String ElastosKeyStoreTest::TEST_ALIAS_1("test1");

const String ElastosKeyStoreTest::TEST_ALIAS_2("test2");

const String ElastosKeyStoreTest::TEST_ALIAS_3("test3");

static AutoPtr<IX500Principal> initTEST_DN_1()
{
    AutoPtr<IX500Principal> tmp;
    CX500Principal::New(String("CN=test1"), (IX500Principal**)&tmp);
    return tmp;
}

AutoPtr<IX500Principal> ElastosKeyStoreTest::TEST_DN_1 = initTEST_DN_1();

static AutoPtr<IX500Principal> initTEST_DN_2()
{
    AutoPtr<IX500Principal> tmp;
    CX500Principal::New(String("CN=test2"), (IX500Principal**)&tmp);
    return tmp;
}

AutoPtr<IX500Principal> ElastosKeyStoreTest::TEST_DN_2 = initTEST_DN_2();

static AutoPtr<IBigInteger> initTEST_SERIAL_1()
{
    AutoPtr<IBigIntegerHelper> helper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&helper);
    AutoPtr<IBigInteger> tmp;
    helper->GetONE((IBigInteger**)&tmp);
    return tmp;
}

AutoPtr<IBigInteger> ElastosKeyStoreTest::TEST_SERIAL_1 = initTEST_SERIAL_1();

static AutoPtr<IBigInteger> initTEST_SERIAL_2()
{
    AutoPtr<IBigIntegerHelper> helper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&helper);
    AutoPtr<IBigInteger> tmp;
    helper->ValueOf(2L, (IBigInteger**)&tmp);
    return tmp;
}

AutoPtr<IBigInteger> ElastosKeyStoreTest::TEST_SERIAL_2 = initTEST_SERIAL_2();

static Int64 initNOW_MILLIS()
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 m;
    system->GetCurrentTimeMillis(&m);
    return m;
}

Int64 ElastosKeyStoreTest::NOW_MILLIS = initNOW_MILLIS();

AutoPtr<IDate> ElastosKeyStoreTest::initNOW()
{
    AutoPtr<IDate> tmp;
    CDate::New(NOW_MILLIS - (NOW_MILLIS % 1000L), (IDate**)&tmp);
    return tmp;
}

AutoPtr<IDate> ElastosKeyStoreTest::NOW = initNOW();

AutoPtr<IDate> ElastosKeyStoreTest::initNOW_PLUS_10_YEARS()
{
    Int32 year;
    NOW->GetYear(&year);
    AutoPtr<IDate> tmp;
    CDate::New(year + 10, 0, 1, (IDate**)&tmp);
    return tmp;
}

AutoPtr<IDate> ElastosKeyStoreTest::NOW_PLUS_10_YEARS = initNOW_PLUS_10_YEARS();

const Int32 ElastosKeyStoreTest::SLOP_TIME_MILLIS = 15000L;

ElastosKeyStoreTest::ElastosKeyStoreTest()
{
    AutoPtr<Elastos::Droid::KeyStore::Security::IKeyStoreHelper> helper;
    Elastos::Droid::KeyStore::Security::CKeyStoreHelper::AcquireSingleton(
            (Elastos::Droid::KeyStore::Security::IKeyStoreHelper**)&helper);
    helper->GetInstance((Elastos::Droid::KeyStore::Security::IKeyStore**)&mAndroidKeyStore);
    assert(mAndroidKeyStore != NULL);

    Boolean res;
    mAndroidKeyStore->Reset(&res);
    assert(res == TRUE);

    mAndroidKeyStore->IsUnlocked(&res);
    assert(res == FALSE);

    AutoPtr<Elastos::Security::IKeyStoreHelper> helper2;
    Elastos::Security::CKeyStoreHelper::AcquireSingleton((Elastos::Security::IKeyStoreHelper**)&helper2);
    helper2->GetInstance(String("ElastosKeyStore"), (Elastos::Security::IKeyStore**)&mKeyStore);
}

ECode ElastosKeyStoreTest::SetupPassword()
{
    Boolean res;
    mAndroidKeyStore->Password(String("1111"), &res);
    assert(res == TRUE);

    mAndroidKeyStore->IsUnlocked(&res);
    assert(res == TRUE);

    AutoPtr<ArrayOf<String> > aliases;
    mAndroidKeyStore->Saw(String(""), (ArrayOf<String>**)&aliases);
    assert(aliases != NULL);
    assert(0 == aliases->GetLength());

    return NOERROR;
}

ECode ElastosKeyStoreTest::AssertAliases(
    /* [in] */ ArrayOf<String>* expectedAliases)
{
    AutoPtr<IEnumeration> aliases;
    mKeyStore->Aliases((IEnumeration**)&aliases);
    Int32 count = 0;

    AutoPtr<IList> outList;
    Arrays::AsList(expectedAliases, (IList**)&outList);
    AutoPtr<ISet> expectedSet;
    CHashSet::New((ISet**)&expectedSet);
    Boolean res;
    expectedSet->AddAll(ICollection::Probe(outList), &res);

    while (aliases->HasMoreElements(&res), res) {
        count++;
        AutoPtr<IInterface> obj;
        aliases->GetNextElement((IInterface**)&obj);
        AutoPtr<ICharSequence> c = ICharSequence::Probe(obj);
        String alias;
        c->ToString(&alias);

        Boolean tmp;
        expectedSet->Contains(TO_IINTERFACE(CoreUtils::Convert(alias)), &tmp);
        assert(TRUE == tmp);
        expectedSet->Remove(TO_IINTERFACE(CoreUtils::Convert(alias)));
    }

    expectedSet->IsEmpty(&res);
    assert(TRUE == res);

    assert(expectedAliases->GetLength() == count);
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_Aliases_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(0);
    AssertAliases(array);

    Boolean result;
    mAndroidKeyStore->Generate(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF, INativeCrypto::EVP_PKEY_RSA, 1024,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED,
            NULL, &result);
    assert(result == TRUE);

    AutoPtr<ArrayOf<String> > array2 = ArrayOf<String>::Alloc(1);
    array2->Set(0, TEST_ALIAS_1);
    AssertAliases(array2);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_2, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &result);
    assert(result == TRUE);

    AutoPtr<ArrayOf<String> > array4 = ArrayOf<String>::Alloc(2);
    array4->Set(0, TEST_ALIAS_1);
    array4->Set(1, TEST_ALIAS_2);
    AssertAliases(array4);
    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_Aliases_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_Aliases_NotInitialized_Encrypted_Failure()
{
    SetupPassword();

    //try {
    AutoPtr<IEnumeration> enumeration;
    ECode ec = mKeyStore->Aliases((IEnumeration**)&enumeration);
    assert(ec != NOERROR);
    //fail("KeyStore should throw exception when not initialized");
    //} catch (KeyStoreException success) {
    //}
    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_Aliases_NotInitialized_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_ContainsAliases_PrivateAndCA_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(0);
    AssertAliases(array);

    Boolean res;
    mAndroidKeyStore->Generate(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF, INativeCrypto::EVP_PKEY_RSA, 1024,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED,
            NULL, &res);
    assert(TRUE == res);

    mKeyStore->ContainsAlias(TEST_ALIAS_1, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_2, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mKeyStore->ContainsAlias(TEST_ALIAS_2, &res);
    assert(TRUE == res);

    mKeyStore->ContainsAlias(TEST_ALIAS_3, &res);
    assert(FALSE == res);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_ContainsAliases_PrivateAndCA_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_ContainsAliases_CAOnly_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_2, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF, Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mKeyStore->ContainsAlias(TEST_ALIAS_2, &res);
    assert(TRUE == res);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_ContainsAliases_CAOnly_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_ContainsAliases_NonExistent_Encrypted_Failure()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mKeyStore->ContainsAlias(TEST_ALIAS_1, &res);
    assert(FALSE == res);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_ContainsAliases_NonExistent_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_DeleteEntry_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    // TEST_ALIAS_1
    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_RSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_USER_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);


    // TEST_ALIAS_2
    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_2, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    // TEST_ALIAS_3
    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_3, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(3);
    array->Set(0, TEST_ALIAS_1);
    array->Set(1, TEST_ALIAS_2);
    array->Set(2, TEST_ALIAS_3);
    AssertAliases(array);

    mKeyStore->DeleteEntry(TEST_ALIAS_1);

    AutoPtr<ArrayOf<String> > array2 = ArrayOf<String>::Alloc(2);
    array2->Set(0, TEST_ALIAS_2);
    array2->Set(1, TEST_ALIAS_3);
    AssertAliases(array2);

    mKeyStore->DeleteEntry(TEST_ALIAS_3);

    AutoPtr<ArrayOf<String> > array3 = ArrayOf<String>::Alloc(1);
    array3->Set(0, TEST_ALIAS_2);
    AssertAliases(array3);

    mKeyStore->DeleteEntry(TEST_ALIAS_2);

    AutoPtr<ArrayOf<String> > array4 = ArrayOf<String>::Alloc(0);
    AssertAliases(array4);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_DeleteEntry_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_DeleteEntry_EmptyStore_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    // Should not throw when a non-existent entry is requested for delete.
    mKeyStore->DeleteEntry(TEST_ALIAS_1);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_DeleteEntry_EmptyStore_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_DeleteEntry_NonExistent_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    // TEST_ALIAS_1
    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_RSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_USER_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    // Should not throw when a non-existent entry is requested for delete.
    mKeyStore->DeleteEntry(TEST_ALIAS_2);
    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_DeleteEntry_NonExistent_Encrypted_Success return");
    return NOERROR;
}

Boolean ElastosKeyStoreTest::IsEquals(
    /* [in] */ IInterface* expected,
    /* [in] */ IInterface* actual)
{
    Boolean res;
    IObject::Probe(expected)->Equals(actual, &res);
    return res;
}

ECode ElastosKeyStoreTest::AssertEquals(
    /* [in] */ IInterface* expected,
    /* [in] */ IInterface* actual)
{
    if (expected == NULL && actual == NULL){
        return NOERROR;
    }
    if (expected != NULL && IsEquals(expected, actual)){
        return NOERROR;
    }
    // else if (expected instanceof String && actual instanceof String) {
    //     // String cleanMessage= message == null ? "" : message;
    //     // throw new ComparisonFailure(cleanMessage, (String) expected,
    //     //         (String) actual);
    // }
    // else
    //     failNotEquals(message, expected, actual);
    assert(0);
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetCertificate_Single_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    array->Set(0, TEST_ALIAS_1);
    AssertAliases(array);

    AutoPtr<ICertificate> c;
    mKeyStore->GetCertificate(TEST_ALIAS_2, (ICertificate**)&c);
    assert(NULL == c);

    AutoPtr<ICertificate> retrieved;
    mKeyStore->GetCertificate(TEST_ALIAS_1, (ICertificate**)&retrieved);
    assert(NULL != retrieved);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> actual;
    f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&actual);

    AssertEquals(actual, retrieved);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetCertificate_Single_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetCertificate_NonExist_Encrypted_Failure()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    AutoPtr<ICertificate> c;
    mKeyStore->GetCertificate(TEST_ALIAS_1, (ICertificate**)&c);
    assert(NULL == c);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetCertificate_NonExist_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetCertificateAlias_CAEntry_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> actual;
    f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&actual);

    String alias;
    mKeyStore->GetCertificateAlias(actual, &alias);
    assert(TRUE == TEST_ALIAS_1.Equals(alias));

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetCertificateAlias_CAEntry_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetCertificateAlias_PrivateKeyEntry_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_RSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_USER_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> actual;
    f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&actual);

    String alias;
    mKeyStore->GetCertificateAlias(actual, &alias);
    assert(TRUE == TEST_ALIAS_1.Equals(alias));

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetCertificateAlias_PrivateKeyEntry_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetCertificateAlias_CAEntry_WithPrivateKeyUsingCA_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    // Insert TrustedCertificateEntry with CA name
    Boolean res;
    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_2, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    // Insert PrivateKeyEntry that uses the same CA
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_RSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_USER_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> actual;
    f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&actual);

    String alias;
    mKeyStore->GetCertificateAlias(actual, &alias);
    assert(TRUE == TEST_ALIAS_2.Equals(alias));

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetCertificateAlias_CAEntry_WithPrivateKeyUsingCA_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetCertificateAlias_NonExist_Empty_Encrypted_Failure()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> actual;
    f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&actual);

    String alias;
    mKeyStore->GetCertificateAlias(actual, &alias);
    assert(alias.IsNull());

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetCertificateAlias_NonExist_Empty_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetCertificateAlias_NonExist_Encrypted_Failure()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> userCert;
    f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&userCert);

    String alias;
    mKeyStore->GetCertificateAlias(userCert, &alias);
    assert(alias.IsNull());

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetCertificateAlias_NonExist_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetCertificateChain_SingleLength_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_RSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_USER_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> cf;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&cf);

    AutoPtr<ArrayOf<ICertificate*> > expected = ArrayOf<ICertificate*>::Alloc(2);

    AutoPtr<IByteArrayInputStream> stream1;
    CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream1);
    AutoPtr<IByteArrayInputStream> stream2;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream2);

    AutoPtr<ICertificate> c1;
    cf->GenerateCertificate(IInputStream::Probe(stream1), (ICertificate**)&c1);
    AutoPtr<ICertificate> c2;
    cf->GenerateCertificate(IInputStream::Probe(stream2), (ICertificate**)&c2);

    expected->Set(0, c1);
    expected->Set(1, c2);

    AutoPtr<ArrayOf<ICertificate*> > actual;
    mKeyStore->GetCertificateChain(TEST_ALIAS_1, (ArrayOf<ICertificate*>**)&actual);

    assert(NULL != actual);
    assert(expected->GetLength() == actual->GetLength());

    AssertEquals((*expected)[0], (*actual)[0]);
    AssertEquals((*expected)[1], (*actual)[1]);

    // Negative test when keystore is populated.
    AutoPtr<ArrayOf<ICertificate*> > tmp;
    mKeyStore->GetCertificateChain(TEST_ALIAS_2, (ArrayOf<ICertificate*>**)&tmp);
    assert(tmp == NULL);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetCertificateChain_SingleLength_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetCertificateChain_NonExist_Encrypted_Failure()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    AutoPtr<ArrayOf<ICertificate*> > tmp;
    mKeyStore->GetCertificateChain(TEST_ALIAS_1, (ArrayOf<ICertificate*>**)&tmp);
    assert(tmp == NULL);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetCertificateChain_NonExist_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetCreationDate_PrivateKeyEntry_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_RSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_USER_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    AutoPtr<IDate> now;
    CDate::New((IDate**)&now);

    AutoPtr<IDate> actual;
    mKeyStore->GetCreationDate(TEST_ALIAS_1, (IDate**)&actual);

    AutoPtr<IDate> expectedAfter;
    Int64 time;
    now->GetTime(&time);
    CDate::New(time - SLOP_TIME_MILLIS, (IDate**)&expectedAfter);
    AutoPtr<IDate> expectedBefore;
    now->GetTime(&time);
    CDate::New(time + SLOP_TIME_MILLIS, (IDate**)&expectedBefore);

    actual->IsBefore(expectedBefore, &res);
    assert(TRUE == res);
    actual->IsAfter(expectedAfter, &res);
    assert(TRUE == res);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetCreationDate_PrivateKeyEntry_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetCreationDate_PrivateKeyEntry_Unencrypted_Success()
{
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_RSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_USER_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    AutoPtr<IDate> now;
    CDate::New((IDate**)&now);
    AutoPtr<IDate> actual;
    mKeyStore->GetCreationDate(TEST_ALIAS_1, (IDate**)&actual);

    AutoPtr<IDate> expectedAfter;
    Int64 time;
    now->GetTime(&time);
    CDate::New(time - SLOP_TIME_MILLIS, (IDate**)&expectedAfter);
    AutoPtr<IDate> expectedBefore;
    now->GetTime(&time);
    CDate::New(time + SLOP_TIME_MILLIS, (IDate**)&expectedBefore);

    actual->IsBefore(expectedBefore, &res);
    assert(TRUE == res);
    actual->IsAfter(expectedAfter, &res);
    assert(TRUE == res);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetCreationDate_PrivateKeyEntry_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetCreationDate_CAEntry_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    AutoPtr<IDate> now;
    CDate::New((IDate**)&now);
    AutoPtr<IDate> actual;
    mKeyStore->GetCreationDate(TEST_ALIAS_1, (IDate**)&actual);
    assert(NULL != actual);

    AutoPtr<IDate> expectedAfter;
    Int64 time;
    now->GetTime(&time);
    CDate::New(time - SLOP_TIME_MILLIS, (IDate**)&expectedAfter);
    AutoPtr<IDate> expectedBefore;
    now->GetTime(&time);
    CDate::New(time + SLOP_TIME_MILLIS, (IDate**)&expectedBefore);

    actual->IsBefore(expectedBefore, &res);
    assert(TRUE == res);
    actual->IsAfter(expectedAfter, &res);
    assert(TRUE == res);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetCreationDate_CAEntry_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetEntry_NullParams_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_RSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_USER_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    AutoPtr<IKeyStoreEntry> entry;
    mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&entry);
    assert(NULL != entry);

    assert(IKeyStorePrivateKeyEntry::Probe(entry) != NULL);

    AutoPtr<IKeyStorePrivateKeyEntry> keyEntry = IKeyStorePrivateKeyEntry::Probe(entry);

    AssertPrivateKeyEntryEquals(keyEntry, String("RSA"), FAKE_RSA_KEY_1, FAKE_RSA_USER_1,
            FAKE_RSA_CA_1);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetEntry_NullParams_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetEntry_DSA_NullParams_Unencrypted_Success()
{
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_DSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1,
            FAKE_DSA_USER_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_DSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    AutoPtr<IKeyStoreEntry> entry;
    mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&entry);
    assert(NULL != entry);

    assert(IKeyStorePrivateKeyEntry::Probe(entry) != NULL);

    AutoPtr<IKeyStorePrivateKeyEntry> keyEntry = IKeyStorePrivateKeyEntry::Probe(entry);

    AssertPrivateKeyEntryEquals(keyEntry, String("DSA"), FAKE_DSA_KEY_1, FAKE_DSA_USER_1, FAKE_DSA_CA_1);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetEntry_DSA_NullParams_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetEntry_EC_NullParams_Unencrypted_Success()
{
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_EC_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1,
            FAKE_EC_USER_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_EC_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    AutoPtr<IKeyStoreEntry> entry;
    mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&entry);
    assert(NULL != entry);

    assert(IKeyStorePrivateKeyEntry::Probe(entry) != NULL);

    AutoPtr<IKeyStorePrivateKeyEntry> keyEntry = IKeyStorePrivateKeyEntry::Probe(entry);

    AssertPrivateKeyEntryEquals(keyEntry, String("EC"), FAKE_EC_KEY_1, FAKE_EC_USER_1, FAKE_EC_CA_1);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetEntry_EC_NullParams_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetEntry_RSA_NullParams_Unencrypted_Success()
{
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_RSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1,
            FAKE_RSA_USER_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    AutoPtr<IKeyStoreEntry> entry;
    mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&entry);
    assert(NULL != entry);

    assert(IKeyStorePrivateKeyEntry::Probe(entry) != NULL);

    AutoPtr<IKeyStorePrivateKeyEntry> keyEntry = IKeyStorePrivateKeyEntry::Probe(entry);

    AssertPrivateKeyEntryEquals(keyEntry, String("RSA"), FAKE_RSA_KEY_1, FAKE_RSA_USER_1,
            FAKE_RSA_CA_1);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetEntry_RSA_NullParams_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::AssertPrivateKeyEntryEquals(
    /* [in] */ IKeyStorePrivateKeyEntry* keyEntry,
    /* [in] */ const String& keyType,
    /* [in] */ ArrayOf<Byte>* key,
    /* [in] */ ArrayOf<Byte>* cert,
    /* [in] */ ArrayOf<Byte>* ca)
{
    AutoPtr<IKeyFactoryHelper> helper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
    AutoPtr<IKeyFactory> keyFact;
    helper->GetInstance(keyType, (IKeyFactory**)&keyFact);

    AutoPtr<IPKCS8EncodedKeySpec> spec;
    CPKCS8EncodedKeySpec::New(key, (IPKCS8EncodedKeySpec**)&spec);
    AutoPtr<IPrivateKey> expectedKey;
    keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&expectedKey);

    AutoPtr<ICertificateFactoryHelper> helper2;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper2);
    AutoPtr<ICertificateFactory> certFact;
    helper2->GetInstance(String("X.509"), (ICertificateFactory**)&certFact);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(cert, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> expectedCert;
    certFact->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&expectedCert);

    AutoPtr<ICollection> expectedChain;
    if (ca != NULL) {
        AutoPtr<IByteArrayInputStream> stream;
        CByteArrayInputStream:: New(ca, (IByteArrayInputStream**)&stream);
        certFact->GenerateCertificates(IInputStream::Probe(stream), (ICollection**)&expectedChain);
    }
    else {
        expectedChain = NULL;
    }

    return AssertPrivateKeyEntryEquals(keyEntry, expectedKey, expectedCert, expectedChain);
}

ECode ElastosKeyStoreTest::AssertPrivateKeyEntryEquals(
    /* [in] */ IKeyStorePrivateKeyEntry* keyEntry,
    /* [in] */ IPrivateKey* expectedKey,
    /* [in] */ ICertificate* expectedCert,
    /* [in] */ ICollection* expectedChain)
{
    if (IDSAPrivateKey::Probe(expectedKey) != NULL) {
        AutoPtr<IDSAParams> parameters;
        IDSAKey::Probe(expectedKey)->GetParams((IDSAParams**)&parameters);

        AutoPtr<Elastos::Security::Cert::ICertificate> cert;
        keyEntry->GetCertificate((Elastos::Security::Cert::ICertificate**)&cert);
        AutoPtr<IPublicKey> key;
        cert->GetPublicKey((IPublicKey**)&key);
        AutoPtr<IDSAParams> parameters2;
        IDSAKey::Probe(key)->GetParams((IDSAParams**)&parameters2);

        AssertEquals(parameters, parameters2);
    }
    else if (IECPrivateKey::Probe(expectedKey) != NULL) {
        AutoPtr<IECParameterSpec> params;
        IECKey::Probe(expectedKey)->GetParams((IECParameterSpec**)&params);
        AutoPtr<IEllipticCurve> curve;
        params->GetCurve((IEllipticCurve**)&curve);

        AutoPtr<Elastos::Security::Cert::ICertificate> cert;
        keyEntry->GetCertificate((Elastos::Security::Cert::ICertificate**)&cert);
        AutoPtr<IPublicKey> key;
        cert->GetPublicKey((IPublicKey**)&key);
        AutoPtr<IECParameterSpec> params2;
        IECKey::Probe(key)->GetParams((IECParameterSpec**)&params2);
        AutoPtr<IEllipticCurve> curve2;
        params2->GetCurve((IEllipticCurve**)&curve2);

        AssertEquals(curve, curve2);
    }
    else if (IRSAPrivateKey::Probe(expectedKey) != NULL) {
        AutoPtr<IBigInteger> modulus;
        IRSAKey::Probe(expectedKey)->GetModulus((IBigInteger**)&modulus);

        AutoPtr<IPrivateKey> key;
        keyEntry->GetPrivateKey((IPrivateKey**)&key);
        AutoPtr<IBigInteger> modulus2;
        IRSAKey::Probe(key)->GetModulus((IBigInteger**)&modulus2);

        AssertEquals(modulus, modulus2);
    }

    AutoPtr<Elastos::Security::Cert::ICertificate> cert;
    keyEntry->GetCertificate((Elastos::Security::Cert::ICertificate**)&cert);
    AssertEquals(expectedCert, cert);

    AutoPtr<ArrayOf<ICertificate*> > actualChain;
    keyEntry->GetCertificateChain((ArrayOf<ICertificate*>**)&actualChain);

    AssertEquals(expectedCert, (*actualChain)[0]);

    if (expectedChain == NULL) {
        assert(1 == actualChain->GetLength());
    }
    else {
        Int32 i = 1;
        AutoPtr<IIterator> it;
        expectedChain->GetIterator((IIterator**)&it);
        Boolean res;
        while (it->HasNext(&res), res) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICertificate> c = ICertificate::Probe(obj);
            AssertEquals(c, (*actualChain)[i++]);
        }
    }
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetEntry_Nonexistent_NullParams_Encrypted_Failure()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    AutoPtr<IKeyStoreEntry> entry;
    mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&entry);
    assert(NULL == entry);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetEntry_Nonexistent_NullParams_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetEntry_Nonexistent_NullParams_Unencrypted_Failure()
{
    mKeyStore->Load(NULL, NULL);

    AutoPtr<IKeyStoreEntry> entry;
    mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&entry);
    assert(NULL == entry);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetEntry_Nonexistent_NullParams_Unencrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetKey_NoPassword_Encrypted_Success()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_RSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_USER_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    AutoPtr<IKey> key;
    mKeyStore->GetKey(TEST_ALIAS_1, NULL, (IKey**)&key);
    assert(NULL != key);

    assert(IRSAPrivateKey::Probe(key) != NULL);

    AutoPtr<IRSAPrivateKey> actualKey = IRSAPrivateKey::Probe(key);

    AutoPtr<IKeyFactoryHelper> helper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
    AutoPtr<IKeyFactory> keyFact;
    helper->GetInstance(String("RSA"), (IKeyFactory**)&keyFact);

    AutoPtr<IPKCS8EncodedKeySpec> spec;
    CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
    AutoPtr<IPrivateKey> expectedKey;
    keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&expectedKey);

    AutoPtr<IBigInteger> modulus;
    IRSAKey::Probe(expectedKey)->GetModulus((IBigInteger**)&modulus);
    AutoPtr<IBigInteger> modulus2;
    IRSAKey::Probe(actualKey)->GetModulus((IBigInteger**)&modulus2);
    AssertEquals(modulus, modulus2);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetKey_NoPassword_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetKey_NoPassword_Unencrypted_Success()
{
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_RSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_USER_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
    assert(TRUE == res);

    AutoPtr<IKey> key;
    mKeyStore->GetKey(TEST_ALIAS_1, NULL, (IKey**)&key);
    assert(NULL != key);

    assert(IRSAPrivateKey::Probe(key) != NULL);

    AutoPtr<IRSAPrivateKey> actualKey = IRSAPrivateKey::Probe(key);

    AutoPtr<IKeyFactoryHelper> helper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
    AutoPtr<IKeyFactory> keyFact;
    helper->GetInstance(String("RSA"), (IKeyFactory**)&keyFact);

    AutoPtr<IPKCS8EncodedKeySpec> spec;
    CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
    AutoPtr<IPrivateKey> expectedKey;
    keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&expectedKey);


    AutoPtr<IBigInteger> modulus;
    IRSAKey::Probe(expectedKey)->GetModulus((IBigInteger**)&modulus);
    AutoPtr<IBigInteger> modulus2;
    IRSAKey::Probe(actualKey)->GetModulus((IBigInteger**)&modulus2);
    AssertEquals(modulus, modulus2);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetKey_NoPassword_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetKey_Certificate_Encrypted_Failure()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    AutoPtr<IKey> key;
    mKeyStore->GetKey(TEST_ALIAS_1, NULL, (IKey**)&key);
    assert(NULL == key);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetKey_Certificate_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetKey_NonExistent_Encrypted_Failure()
{
    SetupPassword();

    mKeyStore->Load(NULL, NULL);

    AutoPtr<IKey> key;
    mKeyStore->GetKey(TEST_ALIAS_1, NULL, (IKey**)&key);
    assert(NULL == key);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetKey_NonExistent_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetProvider_Encrypted_Success()
{

    AutoPtr<IProvider> provider;
    mKeyStore->GetProvider((IProvider**)&provider);
    String name;
    provider->GetName(&name);
    assert(TRUE == IElastosKeyStoreProvider::PROVIDER_NAME.Equals(name));

    SetupPassword();

    AutoPtr<IProvider> provider2;
    mKeyStore->GetProvider((IProvider**)&provider2);
    String name2;
    provider2->GetName(&name2);
    assert(TRUE == IElastosKeyStoreProvider::PROVIDER_NAME.Equals(name2));

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetProvider_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_GetType_Encrypted_Success()
{
    String type;
    mKeyStore->GetType(&type);
    assert(TRUE == IElastosKeyStore::NAME.Equals(type));

    SetupPassword();

    String type2;
    mKeyStore->GetType(&type2);
    assert(TRUE == IElastosKeyStore::NAME.Equals(type2));

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_GetType_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_IsCertificateEntry_CA_Encrypted_Success()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mKeyStore->IsCertificateEntry(TEST_ALIAS_1, &res);
    assert(TRUE == res);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_IsCertificateEntry_CA_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_IsCertificateEntry_PrivateKey_Encrypted_Failure()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_RSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_USER_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mKeyStore->IsCertificateEntry(TEST_ALIAS_1, &res);
    assert(FALSE == res);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_IsCertificateEntry_PrivateKey_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_IsCertificateEntry_NonExist_Encrypted_Failure()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mKeyStore->IsCertificateEntry(TEST_ALIAS_1, &res);
    assert(FALSE == res);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_IsCertificateEntry_NonExist_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_IsCertificateEntry_NonExist_Unencrypted_Failure()
{
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mKeyStore->IsCertificateEntry(TEST_ALIAS_1, &res);
    assert(FALSE == res);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_IsCertificateEntry_NonExist_Unencrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_IsKeyEntry_PrivateKey_Encrypted_Success()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_RSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_USER_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mKeyStore->IsKeyEntry(TEST_ALIAS_1, &res);
    assert(TRUE == res);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_IsKeyEntry_PrivateKey_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_IsKeyEntry_CA_Encrypted_Failure()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mKeyStore->IsKeyEntry(TEST_ALIAS_1, &res);
    assert(FALSE == res);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_IsKeyEntry_CA_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_IsKeyEntry_NonExist_Encrypted_Failure()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mKeyStore->IsKeyEntry(TEST_ALIAS_1, &res);
    assert(FALSE == res);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_IsKeyEntry_NonExist_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetCertificate_CA_Encrypted_Success()
{
    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> actual;
    f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&actual);

    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    mKeyStore->SetCertificateEntry(TEST_ALIAS_1, actual);

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    array->Set(0, TEST_ALIAS_1);
    AssertAliases(array);

    AutoPtr<ICertificate> retrieved;
    mKeyStore->GetCertificate(TEST_ALIAS_1, (ICertificate**)&retrieved);

    AssertEquals(actual, retrieved);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetCertificate_CA_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetCertificate_CAExists_Overwrite_Encrypted_Success()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    array->Set(0, TEST_ALIAS_1);
    AssertAliases(array);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> cert;
    f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&cert);

    // TODO have separate FAKE_CA for second test
    mKeyStore->SetCertificateEntry(TEST_ALIAS_1, cert);

    AutoPtr<ArrayOf<String> > array1 = ArrayOf<String>::Alloc(1);
    array1->Set(0, TEST_ALIAS_1);
    AssertAliases(array);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetCertificate_CAExists_Overwrite_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetCertificate_PrivateKeyExists_Encrypted_Failure()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->ImportKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1,
            FAKE_RSA_KEY_1, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_USER_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    array->Set(0, TEST_ALIAS_1);
    AssertAliases(array);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> cert;
    f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&cert);

    //try {
    ECode ec = mKeyStore->SetCertificateEntry(TEST_ALIAS_1, cert);
    //fail("Should throw when trying to overwrite a PrivateKey entry with a Certificate");
    assert(ec != NOERROR);
    //} catch (KeyStoreException success) {
    //}

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetCertificate_PrivateKeyExists_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetEntry_PrivateKeyEntry_Encrypted_Success()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    AutoPtr<IKeyFactoryHelper> helper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
    AutoPtr<IKeyFactory> keyFact;
    helper->GetInstance(String("RSA"), (IKeyFactory**)&keyFact);

    AutoPtr<IPKCS8EncodedKeySpec> spec;
    CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
    AutoPtr<IPrivateKey> expectedKey;
    keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&expectedKey);

    AutoPtr<ICertificateFactoryHelper> helper2;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper2);
    AutoPtr<ICertificateFactory> f;
    helper2->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<ArrayOf<ICertificate*> > expectedChain = ArrayOf<ICertificate*>::Alloc(2);

    AutoPtr<IByteArrayInputStream> stream1;
    CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream1);
    AutoPtr<ICertificate> c1;
    f->GenerateCertificate(IInputStream::Probe(stream1), (ICertificate**)&c1);
    expectedChain->Set(0, c1);

    AutoPtr<IByteArrayInputStream> stream2;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream2);
    AutoPtr<ICertificate> c2;
    f->GenerateCertificate(IInputStream::Probe(stream2), (ICertificate**)&c2);
    expectedChain->Set(1, c2);

    AutoPtr<IKeyStorePrivateKeyEntry> expected;
    CKeyStorePrivateKeyEntry::New(expectedKey, expectedChain, (IKeyStorePrivateKeyEntry**)&expected);

    mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(expected), NULL);

    AutoPtr<IKeyStoreEntry> actualEntry;
    mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
    assert(NULL != actualEntry);

    assert(IKeyStorePrivateKeyEntry::Probe(actualEntry) != NULL);

    AutoPtr<IKeyStorePrivateKeyEntry> actual = IKeyStorePrivateKeyEntry::Probe(actualEntry);

    AssertPrivateKeyEntryEquals(actual, String("RSA"), FAKE_RSA_KEY_1, FAKE_RSA_USER_1, FAKE_RSA_CA_1);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetEntry_PrivateKeyEntry_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetEntry_PrivateKeyEntry_DSA_Unencrypted_Success()
{
    mKeyStore->Load(NULL, NULL);

    AutoPtr<IKeyFactoryHelper> helper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
    AutoPtr<IKeyFactory> keyFact;
    helper->GetInstance(String("DSA"), (IKeyFactory**)&keyFact);

    AutoPtr<IPKCS8EncodedKeySpec> spex;
    CPKCS8EncodedKeySpec::New(FAKE_DSA_KEY_1, (IPKCS8EncodedKeySpec**)&spex);
    AutoPtr<IPrivateKey> expectedKey;
    keyFact->GeneratePrivate(IKeySpec::Probe(spex), (IPrivateKey**)&expectedKey);

    AutoPtr<ICertificateFactoryHelper> helper2;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper2);
    AutoPtr<ICertificateFactory> f;
    helper2->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<ArrayOf<ICertificate*> > expectedChain = ArrayOf<ICertificate*>::Alloc(2);

    AutoPtr<IByteArrayInputStream> stream1;
    CByteArrayInputStream::New(FAKE_DSA_USER_1, (IByteArrayInputStream**)&stream1);
    AutoPtr<ICertificate> c1;
    f->GenerateCertificate(IInputStream::Probe(stream1), (ICertificate**)&c1);
    expectedChain->Set(0, c1);

    AutoPtr<IByteArrayInputStream> stream2;
    CByteArrayInputStream::New(FAKE_DSA_CA_1, (IByteArrayInputStream**)&stream2);
    AutoPtr<ICertificate> c2;
    f->GenerateCertificate(IInputStream::Probe(stream2), (ICertificate**)&c2);
    expectedChain->Set(1, c2);

    AutoPtr<IKeyStorePrivateKeyEntry> expected;
    CKeyStorePrivateKeyEntry::New(expectedKey, expectedChain, (IKeyStorePrivateKeyEntry**)&expected);

    mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(expected), NULL);

    AutoPtr<IKeyStoreEntry> actualEntry;
    mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
    assert(NULL != actualEntry);

    assert(IKeyStorePrivateKeyEntry::Probe(actualEntry) != NULL);

    AutoPtr<IKeyStorePrivateKeyEntry> actual = IKeyStorePrivateKeyEntry::Probe(actualEntry);

    AssertPrivateKeyEntryEquals(actual, String("DSA"), FAKE_DSA_KEY_1, FAKE_DSA_USER_1, FAKE_DSA_CA_1);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetEntry_PrivateKeyEntry_DSA_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetEntry_PrivateKeyEntry_EC_Unencrypted_Success()
{
    mKeyStore->Load(NULL, NULL);

    AutoPtr<IKeyFactoryHelper> helper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
    AutoPtr<IKeyFactory> keyFact;
    helper->GetInstance(String("EC"), (IKeyFactory**)&keyFact);

    AutoPtr<IPKCS8EncodedKeySpec> spec;
    CPKCS8EncodedKeySpec::New(FAKE_EC_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
    AutoPtr<IPrivateKey> expectedKey;
    keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&expectedKey);

    AutoPtr<ICertificateFactoryHelper> helper2;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper2);
    AutoPtr<ICertificateFactory> f;
    helper2->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<ArrayOf<ICertificate*> > expectedChain = ArrayOf<ICertificate*>::Alloc(2);

    AutoPtr<IByteArrayInputStream> stream1;
    CByteArrayInputStream::New(FAKE_EC_USER_1, (IByteArrayInputStream**)&stream1);
    AutoPtr<ICertificate> c1;
    f->GenerateCertificate(IInputStream::Probe(stream1), (ICertificate**)&c1);
    expectedChain->Set(0, c1);

    AutoPtr<IByteArrayInputStream> stream2;
    CByteArrayInputStream::New(FAKE_EC_CA_1, (IByteArrayInputStream**)&stream2);
    AutoPtr<ICertificate> c2;
    f->GenerateCertificate(IInputStream::Probe(stream2), (ICertificate**)&c2);
    expectedChain->Set(1, c2);

    AutoPtr<IKeyStorePrivateKeyEntry> expected;
    CKeyStorePrivateKeyEntry::New(expectedKey, expectedChain, (IKeyStorePrivateKeyEntry**)&expected);

    mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(expected), NULL);

    AutoPtr<IKeyStoreEntry> actualEntry;
    mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
    assert(NULL != actualEntry);

    assert(IKeyStorePrivateKeyEntry::Probe(actualEntry) != NULL);

    AutoPtr<IKeyStorePrivateKeyEntry> actual = IKeyStorePrivateKeyEntry::Probe(actualEntry);

    AssertPrivateKeyEntryEquals(actual, String("EC"), FAKE_EC_KEY_1, FAKE_EC_USER_1, FAKE_EC_CA_1);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetEntry_PrivateKeyEntry_EC_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetEntry_PrivateKeyEntry_RSA_Unencrypted_Success()
{
    mKeyStore->Load(NULL, NULL);

    AutoPtr<IKeyFactoryHelper> helper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
    AutoPtr<IKeyFactory> keyFact;
    helper->GetInstance(String("RSA"), (IKeyFactory**)&keyFact);

    AutoPtr<IPKCS8EncodedKeySpec> spec;
    CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
    AutoPtr<IPrivateKey> expectedKey;
    keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&expectedKey);

    AutoPtr<ICertificateFactoryHelper> helper2;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper2);
    AutoPtr<ICertificateFactory> f;
    helper2->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<ArrayOf<ICertificate*> > expectedChain = ArrayOf<ICertificate*>::Alloc(2);

    AutoPtr<IByteArrayInputStream> stream1;
    CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream1);
    AutoPtr<ICertificate> c1;
    f->GenerateCertificate(IInputStream::Probe(stream1), (ICertificate**)&c1);
    expectedChain->Set(0, c1);

    AutoPtr<IByteArrayInputStream> stream2;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream2);
    AutoPtr<ICertificate> c2;
    f->GenerateCertificate(IInputStream::Probe(stream2), (ICertificate**)&c2);
    expectedChain->Set(1, c2);

    AutoPtr<IKeyStorePrivateKeyEntry> expected;
    CKeyStorePrivateKeyEntry::New(expectedKey, expectedChain, (IKeyStorePrivateKeyEntry**)&expected);

    mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(expected), NULL);

    AutoPtr<IKeyStoreEntry> actualEntry;
    mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
    assert(NULL != actualEntry);

    assert(IKeyStorePrivateKeyEntry::Probe(actualEntry) != NULL);

    AutoPtr<IKeyStorePrivateKeyEntry> actual = IKeyStorePrivateKeyEntry::Probe(actualEntry);

    AssertPrivateKeyEntryEquals(actual, String("RSA"), FAKE_RSA_KEY_1, FAKE_RSA_USER_1, FAKE_RSA_CA_1);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetEntry_PrivateKeyEntry_RSA_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetEntry_PrivateKeyEntry_Params_Unencrypted_Failure()
{
    mKeyStore->Load(NULL, NULL);

    AutoPtr<IKeyFactoryHelper> helper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
    AutoPtr<IKeyFactory> keyFact;
    helper->GetInstance(String("RSA"), (IKeyFactory**)&keyFact);

    AutoPtr<IPKCS8EncodedKeySpec> spec;
    CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
    AutoPtr<IPrivateKey> expectedKey;
    keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&expectedKey);

    AutoPtr<ICertificateFactoryHelper> helper2;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper2);
    AutoPtr<ICertificateFactory> f;
    helper2->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<ArrayOf<ICertificate*> > expectedChain = ArrayOf<ICertificate*>::Alloc(2);

    AutoPtr<IByteArrayInputStream> stream1;
    CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream1);
    AutoPtr<ICertificate> c1;
    f->GenerateCertificate(IInputStream::Probe(stream1), (ICertificate**)&c1);
    expectedChain->Set(0, c1);

    AutoPtr<IByteArrayInputStream> stream2;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream2);
    AutoPtr<ICertificate> c2;
    f->GenerateCertificate(IInputStream::Probe(stream2), (ICertificate**)&c2);
    expectedChain->Set(1, c2);

    AutoPtr<IKeyStorePrivateKeyEntry> entry;
    CKeyStorePrivateKeyEntry::New(expectedKey, expectedChain, (IKeyStorePrivateKeyEntry**)&entry);

    //try {
    AutoPtr<IKeyStoreParameterBuilder> builder;
    CKeyStoreParameterBuilder::New(mContext, (IKeyStoreParameterBuilder**)&builder);
    builder->SetEncryptionRequired(TRUE);
    AutoPtr<IKeyStoreParameter> para;
    builder->Build((IKeyStoreParameter**)&para);
    ECode ec = mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(entry), IKeyStoreProtectionParameter::Probe(para));
    assert(ec != NOERROR);
    //fail("Shouldn't be able to insert encrypted entry when KeyStore uninitialized");
    //} catch (KeyStoreException expected) {
    //}

    AutoPtr<IKeyStoreEntry> e;
    mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&e);
    assert(e == NULL);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetEntry_PrivateKeyEntry_Params_Unencrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetEntry_PrivateKeyEntry_Overwrites_PrivateKeyEntry_Encrypted_Success()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    AutoPtr<IKeyFactoryHelper> helper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
    AutoPtr<IKeyFactory> keyFact;
    helper->GetInstance(String("RSA"), (IKeyFactory**)&keyFact);

    AutoPtr<ICertificateFactoryHelper> helper2;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper2);
    AutoPtr<ICertificateFactory> f;
    helper2->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    // Start with PrivateKeyEntry
    {
        AutoPtr<IPKCS8EncodedKeySpec> spec;
        CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
        AutoPtr<IPrivateKey> expectedKey;
        keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&expectedKey);

        AutoPtr<ArrayOf<ICertificate*> > expectedChain = ArrayOf<ICertificate*>::Alloc(2);

        AutoPtr<IByteArrayInputStream> stream1;
        CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream1);
        AutoPtr<ICertificate> c1;
        f->GenerateCertificate(IInputStream::Probe(stream1), (ICertificate**)&c1);
        expectedChain->Set(0, c1);

        AutoPtr<IByteArrayInputStream> stream2;
        CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream2);
        AutoPtr<ICertificate> c2;
        f->GenerateCertificate(IInputStream::Probe(stream2), (ICertificate**)&c2);
        expectedChain->Set(1, c2);

        AutoPtr<IKeyStorePrivateKeyEntry> expected;
        CKeyStorePrivateKeyEntry::New(expectedKey, expectedChain, (IKeyStorePrivateKeyEntry**)&expected);

        mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(expected), NULL);

        AutoPtr<IKeyStoreEntry> actualEntry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
        assert(NULL != actualEntry);

        assert(IKeyStorePrivateKeyEntry::Probe(actualEntry) != NULL);

        AutoPtr<IKeyStorePrivateKeyEntry> actual = IKeyStorePrivateKeyEntry::Probe(actualEntry);

        AssertPrivateKeyEntryEquals(actual, String("RSA"), FAKE_RSA_KEY_1, FAKE_RSA_USER_1,
                FAKE_RSA_CA_1);
    }

    // TODO make entirely new test vector for the overwrite
    // Replace with PrivateKeyEntry
    {
        AutoPtr<IPKCS8EncodedKeySpec> spec;
        CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
        AutoPtr<IPrivateKey> expectedKey;
        keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&expectedKey);

        AutoPtr<ArrayOf<ICertificate*> > expectedChain = ArrayOf<ICertificate*>::Alloc(2);

        AutoPtr<IByteArrayInputStream> stream1;
        CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream1);
        AutoPtr<ICertificate> c1;
        f->GenerateCertificate(IInputStream::Probe(stream1), (ICertificate**)&c1);
        expectedChain->Set(0, c1);

        AutoPtr<IByteArrayInputStream> stream2;
        CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream2);
        AutoPtr<ICertificate> c2;
        f->GenerateCertificate(IInputStream::Probe(stream2), (ICertificate**)&c2);
        expectedChain->Set(1, c2);

        AutoPtr<IKeyStorePrivateKeyEntry> expected;
        CKeyStorePrivateKeyEntry::New(expectedKey, expectedChain, (IKeyStorePrivateKeyEntry**)&expected);

        mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(expected), NULL);

        AutoPtr<IKeyStoreEntry> actualEntry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
        assert(NULL != actualEntry);

        assert(IKeyStorePrivateKeyEntry::Probe(actualEntry) != NULL);

        AutoPtr<IKeyStorePrivateKeyEntry> actual = IKeyStorePrivateKeyEntry::Probe(actualEntry);

        AssertPrivateKeyEntryEquals(actual, String("RSA"), FAKE_RSA_KEY_1, FAKE_RSA_USER_1,
                FAKE_RSA_CA_1);
    }

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetEntry_PrivateKeyEntry_Overwrites_PrivateKeyEntry_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetEntry_CAEntry_Overwrites_PrivateKeyEntry_Encrypted_Success()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    // Start with TrustedCertificateEntry
    {
        AutoPtr<IByteArrayInputStream> stream;
        CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream);
        AutoPtr<ICertificate> caCert;
        f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&caCert);

        AutoPtr<IKeyStoreTrustedCertificateEntry> expectedCertEntry;
        CKeyStoreTrustedCertificateEntry::New(caCert, (IKeyStoreTrustedCertificateEntry**)&expectedCertEntry);
        mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(expectedCertEntry), NULL);

        AutoPtr<IKeyStoreEntry> actualEntry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
        assert(NULL != actualEntry);
        assert(IKeyStoreTrustedCertificateEntry::Probe(actualEntry) != NULL);
        AutoPtr<IKeyStoreTrustedCertificateEntry> actualCertEntry = IKeyStoreTrustedCertificateEntry::Probe(actualEntry);

        AutoPtr<Elastos::Security::Cert::ICertificate> cert1;
        expectedCertEntry->GetTrustedCertificate((Elastos::Security::Cert::ICertificate**)&cert1);
        AutoPtr<Elastos::Security::Cert::ICertificate> cert2;
        actualCertEntry->GetTrustedCertificate((Elastos::Security::Cert::ICertificate**)&cert2);
        AssertEquals(cert1, cert2);
    }

    // Replace with PrivateKeyEntry
    {
        AutoPtr<IKeyFactoryHelper> helper;
        CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
        AutoPtr<IKeyFactory> keyFact;
        helper->GetInstance(String("RSA"), (IKeyFactory**)&keyFact);

        AutoPtr<IPKCS8EncodedKeySpec> spec;
        CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
        AutoPtr<IPrivateKey> expectedKey;
        keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&expectedKey);

        AutoPtr<ArrayOf<ICertificate*> > expectedChain = ArrayOf<ICertificate*>::Alloc(2);

        AutoPtr<IByteArrayInputStream> stream1;
        CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream1);
        AutoPtr<ICertificate> c1;
        f->GenerateCertificate(IInputStream::Probe(stream1), (ICertificate**)&c1);
        expectedChain->Set(0, c1);

        AutoPtr<IByteArrayInputStream> stream2;
        CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream2);
        AutoPtr<ICertificate> c2;
        f->GenerateCertificate(IInputStream::Probe(stream2), (ICertificate**)&c2);
        expectedChain->Set(1, c2);

        AutoPtr<IKeyStorePrivateKeyEntry> expectedPrivEntry;
        CKeyStorePrivateKeyEntry::New(expectedKey, expectedChain, (IKeyStorePrivateKeyEntry**)&expectedPrivEntry);

        mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(expectedPrivEntry), NULL);

        AutoPtr<IKeyStoreEntry> actualEntry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
        assert(NULL != actualEntry);
        assert(IKeyStorePrivateKeyEntry::Probe(actualEntry) != NULL);

        AutoPtr<IKeyStorePrivateKeyEntry> actualPrivEntry = IKeyStorePrivateKeyEntry::Probe(actualEntry);
        AssertPrivateKeyEntryEquals(actualPrivEntry, String("RSA"), FAKE_RSA_KEY_1, FAKE_RSA_USER_1,
                FAKE_RSA_CA_1);
    }

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetEntry_CAEntry_Overwrites_PrivateKeyEntry_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetEntry_PrivateKeyEntry_Overwrites_CAEntry_Encrypted_Success()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> caCert;
    f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&caCert);

    // Start with PrivateKeyEntry
    {
        AutoPtr<IKeyFactoryHelper> helper;
        CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
        AutoPtr<IKeyFactory> keyFact;
        helper->GetInstance(String("RSA"), (IKeyFactory**)&keyFact);

        AutoPtr<IPKCS8EncodedKeySpec> spec;
        CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
        AutoPtr<IPrivateKey> expectedKey;
        keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&expectedKey);

        AutoPtr<ArrayOf<ICertificate*> > expectedChain = ArrayOf<ICertificate*>::Alloc(2);
        AutoPtr<IByteArrayInputStream> stream;
        CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream);
        AutoPtr<ICertificate> c;
        f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&c);
        expectedChain->Set(0, c);
        expectedChain->Set(1, caCert);

        AutoPtr<IKeyStorePrivateKeyEntry> expectedPrivEntry;
        CKeyStorePrivateKeyEntry::New(expectedKey, expectedChain, (IKeyStorePrivateKeyEntry**)&expectedPrivEntry);

        mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(expectedPrivEntry), NULL);

        AutoPtr<IKeyStoreEntry> actualEntry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
        assert(NULL != actualEntry);
        assert(IKeyStorePrivateKeyEntry::Probe(actualEntry) != NULL);

        AutoPtr<IKeyStorePrivateKeyEntry> actualPrivEntry = IKeyStorePrivateKeyEntry::Probe(actualEntry);
        AssertPrivateKeyEntryEquals(actualPrivEntry, String("RSA"), FAKE_RSA_KEY_1, FAKE_RSA_USER_1,
                FAKE_RSA_CA_1);
    }

    // Replace with TrustedCertificateEntry
    {
        AutoPtr<IKeyStoreTrustedCertificateEntry> expectedCertEntry;
        CKeyStoreTrustedCertificateEntry::New(caCert, (IKeyStoreTrustedCertificateEntry**)&expectedCertEntry);
        mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(expectedCertEntry), NULL);

        AutoPtr<IKeyStoreEntry> actualEntry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
        assert(NULL != actualEntry);
        assert(IKeyStoreTrustedCertificateEntry::Probe(actualEntry) != NULL);

        AutoPtr<IKeyStoreTrustedCertificateEntry> actualCertEntry = IKeyStoreTrustedCertificateEntry::Probe(actualEntry);

        AutoPtr<ICertificate> ec;
        expectedCertEntry->GetTrustedCertificate((ICertificate**)&ec);
        AutoPtr<ICertificate> ac;
        actualCertEntry->GetTrustedCertificate((ICertificate**)&ac);
        AssertEquals(ec, ac);
    }

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetEntry_PrivateKeyEntry_Overwrites_CAEntry_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetEntry_PrivateKeyEntry_Overwrites_ShortPrivateKeyEntry_Encrypted_Success()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> caCert;
    f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&caCert);

    // Start with PrivateKeyEntry
    {
        AutoPtr<IKeyFactoryHelper> helper;
        CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
        AutoPtr<IKeyFactory> keyFact;
        helper->GetInstance(String("RSA"), (IKeyFactory**)&keyFact);
        AutoPtr<IPKCS8EncodedKeySpec> spec;
        CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
        AutoPtr<IPrivateKey> expectedKey;
        keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&expectedKey);


        AutoPtr<ArrayOf<ICertificate*> > expectedChain = ArrayOf<ICertificate*>::Alloc(2);
        AutoPtr<IByteArrayInputStream> stream;
        CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream);
        AutoPtr<ICertificate> c;
        f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&c);
        expectedChain->Set(0, c);
        expectedChain->Set(1, caCert);

        AutoPtr<IKeyStorePrivateKeyEntry> expectedPrivEntry;
        CKeyStorePrivateKeyEntry::New(expectedKey, expectedChain, (IKeyStorePrivateKeyEntry**)&expectedPrivEntry);

        mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(expectedPrivEntry), NULL);

        AutoPtr<IKeyStoreEntry> actualEntry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
        assert(NULL != actualEntry);
        assert(IKeyStorePrivateKeyEntry::Probe(actualEntry) != NULL);

        AutoPtr<IKeyStorePrivateKeyEntry> actualPrivEntry = IKeyStorePrivateKeyEntry::Probe(actualEntry);
        AssertPrivateKeyEntryEquals(actualPrivEntry, String("RSA"), FAKE_RSA_KEY_1, FAKE_RSA_USER_1,
                FAKE_RSA_CA_1);
    }

    // Replace with PrivateKeyEntry that has no chain
    {
        AutoPtr<IKeyFactoryHelper> helper;
        CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
        AutoPtr<IKeyFactory> keyFact;
        helper->GetInstance(String("RSA"), (IKeyFactory**)&keyFact);

        AutoPtr<IPKCS8EncodedKeySpec> spec;
        CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
        AutoPtr<IPrivateKey> expectedKey;
        keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&expectedKey);


        AutoPtr<ArrayOf<ICertificate*> > expectedChain = ArrayOf<ICertificate*>::Alloc(1);
        AutoPtr<IByteArrayInputStream> stream;
        CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream);
        AutoPtr<ICertificate> c;
        f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&c);
        expectedChain->Set(0, c);

        AutoPtr<IKeyStorePrivateKeyEntry> expectedPrivEntry;
        CKeyStorePrivateKeyEntry::New(expectedKey, expectedChain, (IKeyStorePrivateKeyEntry**)&expectedPrivEntry);

        mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(expectedPrivEntry), NULL);

        AutoPtr<IKeyStoreEntry> actualEntry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
        assert(NULL != actualEntry);
        assert(IKeyStorePrivateKeyEntry::Probe(actualEntry) != NULL);

        AutoPtr<IKeyStorePrivateKeyEntry> actualPrivEntry = IKeyStorePrivateKeyEntry::Probe(actualEntry);
        AssertPrivateKeyEntryEquals(actualPrivEntry, String("RSA"), FAKE_RSA_KEY_1, FAKE_RSA_USER_1,
                NULL);
    }

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetEntry_PrivateKeyEntry_Overwrites_ShortPrivateKeyEntry_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetEntry_CAEntry_Overwrites_CAEntry_Encrypted_Success()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    // Insert TrustedCertificateEntry
    {
        AutoPtr<IByteArrayInputStream> stream;
        CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream);
        AutoPtr<ICertificate> caCert;
        f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&caCert);

        AutoPtr<IKeyStoreTrustedCertificateEntry> expectedCertEntry;
        CKeyStoreTrustedCertificateEntry::New(caCert, (IKeyStoreTrustedCertificateEntry**)&expectedCertEntry);
        mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(expectedCertEntry), NULL);

        AutoPtr<IKeyStoreEntry> actualEntry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
        assert(NULL != actualEntry);
        assert(IKeyStoreTrustedCertificateEntry::Probe(actualEntry) != NULL);
        AutoPtr<IKeyStoreTrustedCertificateEntry> actualCertEntry = IKeyStoreTrustedCertificateEntry::Probe(actualEntry);

        AutoPtr<ICertificate> ec;
        expectedCertEntry->GetTrustedCertificate((ICertificate**)&ec);
        AutoPtr<ICertificate> ac;
        actualCertEntry->GetTrustedCertificate((ICertificate**)&ac);
        AssertEquals(ec, ac);
    }

    // Replace with TrustedCertificateEntry of USER
    {
        AutoPtr<IByteArrayInputStream> stream;
        CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream);
        AutoPtr<ICertificate> userCert;
        f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&userCert);

        AutoPtr<IKeyStoreTrustedCertificateEntry> expectedUserEntry;
        CKeyStoreTrustedCertificateEntry::New(userCert, (IKeyStoreTrustedCertificateEntry**)&expectedUserEntry);
        mKeyStore->SetEntry(TEST_ALIAS_1, IKeyStoreEntry::Probe(expectedUserEntry), NULL);

        AutoPtr<IKeyStoreEntry> actualEntry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
        assert(NULL != actualEntry);
        assert(IKeyStoreTrustedCertificateEntry::Probe(actualEntry) != NULL);
        AutoPtr<IKeyStoreTrustedCertificateEntry> actualUserEntry = IKeyStoreTrustedCertificateEntry::Probe(actualEntry);

        AutoPtr<ICertificate> ec;
        expectedUserEntry->GetTrustedCertificate((ICertificate**)&ec);
        AutoPtr<ICertificate> ac;
        actualUserEntry->GetTrustedCertificate((ICertificate**)&ac);
        AssertEquals(ec, ac);
    }

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetEntry_CAEntry_Overwrites_CAEntry_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetKeyEntry_ProtectedKey_Encrypted_Failure()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> caCert;
    f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&caCert);

    AutoPtr<IKeyFactoryHelper> helper2;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper2);
    AutoPtr<IKeyFactory> keyFact;
    helper2->GetInstance(String("RSA"), (IKeyFactory**)&keyFact);

    AutoPtr<IPKCS8EncodedKeySpec> spec;
    CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
    AutoPtr<IPrivateKey> privKey;
    keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&privKey);


    AutoPtr<ArrayOf<ICertificate*> > chain = ArrayOf<ICertificate*>::Alloc(2);
    AutoPtr<IByteArrayInputStream> stream2;
    CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream2);
    AutoPtr<ICertificate> c;
    f->GenerateCertificate(IInputStream::Probe(stream2), (ICertificate**)&c);
    chain->Set(0, c);
    chain->Set(1, caCert);

    //try {
    AutoPtr<ArrayOf<Char32> > array = String("foo").GetChars();
    ECode ec = mKeyStore->SetKeyEntry(TEST_ALIAS_1, IKey::Probe(privKey), array, chain);
    assert(ec != NOERROR);
    //fail("Should fail when a password is specified");
    //} catch (KeyStoreException success) {
    //}

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetKeyEntry_ProtectedKey_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetKeyEntry_Encrypted_Success()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> caCert;
    f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&caCert);

    AutoPtr<IKeyFactoryHelper> helper2;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper2);
    AutoPtr<IKeyFactory> keyFact;
    helper2->GetInstance(String("RSA"), (IKeyFactory**)&keyFact);

    AutoPtr<IPKCS8EncodedKeySpec> spec;
    CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
    AutoPtr<IPrivateKey> privKey;
    keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&privKey);

    AutoPtr<ArrayOf<ICertificate*> > chain = ArrayOf<ICertificate*>::Alloc(2);
    AutoPtr<IByteArrayInputStream> stream2;
    CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream2);
    AutoPtr<ICertificate> c;
    f->GenerateCertificate(IInputStream::Probe(stream2), (ICertificate**)&c);
    chain->Set(0, c);
    chain->Set(1, caCert);

    mKeyStore->SetKeyEntry(TEST_ALIAS_1, IKey::Probe(privKey), NULL, chain);

    AutoPtr<IKeyStoreEntry> actualEntry;
    mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
    assert(NULL != actualEntry);

    assert(IKeyStorePrivateKeyEntry::Probe(actualEntry) != NULL);

    AutoPtr<IKeyStorePrivateKeyEntry> actual = IKeyStorePrivateKeyEntry::Probe(actualEntry);

    AssertPrivateKeyEntryEquals(actual, String("RSA"), FAKE_RSA_KEY_1, FAKE_RSA_USER_1, FAKE_RSA_CA_1);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetKeyEntry_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetKeyEntry_Replaced_Encrypted_Success()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> f;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&f);

    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(FAKE_RSA_CA_1, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> caCert;
    f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&caCert);

    // Insert initial key
    {
        AutoPtr<IKeyFactoryHelper> helper2;
        CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper2);
        AutoPtr<IKeyFactory> keyFact;
        helper2->GetInstance(String("RSA"), (IKeyFactory**)&keyFact);

        AutoPtr<IPKCS8EncodedKeySpec> spec;
        CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
        AutoPtr<IPrivateKey> privKey;
        keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&privKey);

        AutoPtr<ArrayOf<ICertificate*> > chain = ArrayOf<ICertificate*>::Alloc(2);
        AutoPtr<IByteArrayInputStream> stream;
        CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream);
        AutoPtr<ICertificate> c;
        f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&c);
        chain->Set(0, c);
        chain->Set(1, caCert);

        mKeyStore->SetKeyEntry(TEST_ALIAS_1, IKey::Probe(privKey), NULL, chain);

        AutoPtr<IKeyStoreEntry> actualEntry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
        assert(NULL != actualEntry);

        assert(IKeyStorePrivateKeyEntry::Probe(actualEntry) != NULL);

        AutoPtr<IKeyStorePrivateKeyEntry> actual = IKeyStorePrivateKeyEntry::Probe(actualEntry);

        AssertPrivateKeyEntryEquals(actual, String("RSA"), FAKE_RSA_KEY_1, FAKE_RSA_USER_1,
                FAKE_RSA_CA_1);
    }

    // TODO make a separate key
    // Replace key
    {
        AutoPtr<IKeyFactoryHelper> helper2;
        CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper2);
        AutoPtr<IKeyFactory> keyFact;
        helper2->GetInstance(String("RSA"), (IKeyFactory**)&keyFact);

        AutoPtr<IPKCS8EncodedKeySpec> spec;
        CPKCS8EncodedKeySpec::New(FAKE_RSA_KEY_1, (IPKCS8EncodedKeySpec**)&spec);
        AutoPtr<IPrivateKey> privKey;
        keyFact->GeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&privKey);


        AutoPtr<ArrayOf<ICertificate*> > chain = ArrayOf<ICertificate*>::Alloc(2);
        AutoPtr<IByteArrayInputStream> stream;
        CByteArrayInputStream::New(FAKE_RSA_USER_1, (IByteArrayInputStream**)&stream);
        AutoPtr<ICertificate> c;
        f->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&c);
        chain->Set(0, c);
        chain->Set(1, caCert);

        mKeyStore->SetKeyEntry(TEST_ALIAS_1, IKey::Probe(privKey), NULL, chain);

        AutoPtr<IKeyStoreEntry> actualEntry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&actualEntry);
        assert(NULL != actualEntry);

        assert(IKeyStorePrivateKeyEntry::Probe(actualEntry) != NULL);

        AutoPtr<IKeyStorePrivateKeyEntry> actual = IKeyStorePrivateKeyEntry::Probe(actualEntry);

        AssertPrivateKeyEntryEquals(actual, String("RSA"), FAKE_RSA_KEY_1, FAKE_RSA_USER_1,
                FAKE_RSA_CA_1);
    }

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetKeyEntry_Replaced_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::GenerateCertificate(
    /* [in] */ Elastos::Droid::KeyStore::Security::IKeyStore* keyStore,
    /* [in] */ const String& alias,
    /* [in] */ IBigInteger* serialNumber,
    /* [in] */ IX500Principal* subjectDN,
    /* [in] */ IDate* notBefore,
    /* [in] */ IDate* notAfter,
    /* [out] */ IX509Certificate** certificate)
{
    VALIDATE_NOT_NULL(certificate);
    *certificate = NULL;

    String privateKeyAlias(ICredentials::USER_PRIVATE_KEY + alias);

    AutoPtr<IOpenSSLEngineHelper> helper;
    COpenSSLEngineHelper::AcquireSingleton((IOpenSSLEngineHelper**)&helper);
    AutoPtr<IOpenSSLEngine> engine;
    helper->GetInstance(String("keystore"), (IOpenSSLEngine**)&engine);
    //try {
    AutoPtr<IPrivateKey> privKey;
    ECode ec = engine->GetPrivateKeyById(privateKeyAlias, (IPrivateKey**)&privKey);
    //} catch (InvalidKeyException e) {
    if (ec == (ECode)E_INVALID_KEY_EXCEPTION) {
        Logger::E("ElastosKeyStoreTest", "=====[snow]=====RuntimeException: Can't get key");
        //throw new RuntimeException("Can't get key", e);
        return E_RUNTIME_EXCEPTION;
    }
    //}

    AutoPtr<ArrayOf<Byte> > pubKeyBytes;
    keyStore->GetPubkey(privateKeyAlias, (ArrayOf<Byte>**)&pubKeyBytes);

    AutoPtr<IPublicKey> pubKey;
    //try
    {
        AutoPtr<IKeyFactoryHelper> helper;
        CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&helper);
        AutoPtr<IKeyFactory> keyFact;
        FAIL_GOTO(ec = helper->GetInstance(String("RSA"), (IKeyFactory**)&keyFact), ERROR)
        AutoPtr<IX509EncodedKeySpec> spec;
        FAIL_GOTO(ec = CX509EncodedKeySpec::New(pubKeyBytes, (IX509EncodedKeySpec**)&spec), ERROR)
        FAIL_GOTO(ec = keyFact->GeneratePublic(IKeySpec::Probe(spec), (IPublicKey**)&pubKey), ERROR)
    } //catch (NoSuchAlgorithmException e) {
ERROR:
    if (ec == (ECode)E_NO_SUCH_ALGORITHM_EXCEPTION) {
        Logger::E("ElastosKeyStoreTest", "=====[snow]=====IllegalStateException Can't instantiate RSA key generator");
        //throw new IllegalStateException("Can't instantiate RSA key generator", e);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    //} catch (InvalidKeySpecException e) {
    if (ec == (ECode)E_INVALID_KEY_SPEC_EXCEPTION) {
        Logger::E("ElastosKeyStoreTest", "=====[snow]=====IllegalStateException keystore returned invalid key encoding");
        //throw new IllegalStateException("keystore returned invalid key encoding", e);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    assert(0);
    // AutoPtr<IX509V3CertificateGenerator> certGen;
    // CX509V3CertificateGenerator::New((IX509V3CertificateGenerator**)&certGen);
    // certGen->SetPublicKey(pubKey);
    // certGen->SetSerialNumber(serialNumber);
    // certGen->SetSubjectDN(subjectDN);
    // certGen->SetIssuerDN(subjectDN);
    // certGen->SetNotBefore(notBefore);
    // certGen->SetNotAfter(notAfter);
    // certGen->SetSignatureAlgorithm(String("sha1WithRSA"));

    AutoPtr<IX509Certificate> cert;
    assert(0);
    //certGen->Generate(privKey, (IX509Certificate**)&cert);

    *certificate = cert;
    REFCOUNT_ADD(*certificate);
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetKeyEntry_ReplacedChain_Encrypted_Success()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    // Create key #1
    {
        String privateKeyAlias(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1);
        Boolean res;
        mAndroidKeyStore->Generate(privateKeyAlias, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
                INativeCrypto::EVP_PKEY_RSA, 1024,
                Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, NULL, &res);
        assert(TRUE == res);

        AutoPtr<IKey> key;
        mKeyStore->GetKey(TEST_ALIAS_1, NULL, (IKey**)&key);

        assert(IPrivateKey::Probe(key) != NULL);

        AutoPtr<IPrivateKey> expectedKey = IPrivateKey::Probe(key);

        AutoPtr<IX509Certificate> expectedCert;
        GenerateCertificate(mAndroidKeyStore, TEST_ALIAS_1,
                TEST_SERIAL_1, TEST_DN_1, NOW, NOW_PLUS_10_YEARS, (IX509Certificate**)&expectedCert);

        AutoPtr<ArrayOf<Byte> > array;
        ICertificate::Probe(expectedCert)->GetEncoded((ArrayOf<Byte>**)&array);
        mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1,
                array, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
                Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
        assert(TRUE == res);

        AutoPtr<IKeyStoreEntry> entry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&entry);

        assert(IKeyStorePrivateKeyEntry::Probe(entry) != NULL);

        AutoPtr<IKeyStorePrivateKeyEntry> keyEntry = IKeyStorePrivateKeyEntry::Probe(entry);

        AssertPrivateKeyEntryEquals(keyEntry, expectedKey, ICertificate::Probe(expectedCert), NULL);
    }

    // Replace key #1 with new chain
    {
        AutoPtr<IKey> key;
        mKeyStore->GetKey(TEST_ALIAS_1, NULL, (IKey**)&key);

        assert(IPrivateKey::Probe(key) != NULL);

        AutoPtr<IPrivateKey> expectedKey = IPrivateKey::Probe(key);

        AutoPtr<IX509Certificate> expectedCert;
        GenerateCertificate(mAndroidKeyStore, TEST_ALIAS_1,
                TEST_SERIAL_2, TEST_DN_2, NOW, NOW_PLUS_10_YEARS, (IX509Certificate**)&expectedCert);

        AutoPtr<ArrayOf<ICertificate*> > array = ArrayOf<ICertificate*>::Alloc(1);
        array->Set(0, ICertificate::Probe(expectedCert));

        mKeyStore->SetKeyEntry(TEST_ALIAS_1, IKey::Probe(expectedKey), NULL, array);

        AutoPtr<IKeyStoreEntry> entry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&entry);

        assert(IKeyStorePrivateKeyEntry::Probe(entry) != NULL);

        AutoPtr<IKeyStorePrivateKeyEntry> keyEntry = IKeyStorePrivateKeyEntry::Probe(entry);

        AssertPrivateKeyEntryEquals(keyEntry, expectedKey, ICertificate::Probe(expectedCert), NULL);
    }

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetKeyEntry_ReplacedChain_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetKeyEntry_ReplacedChain_DifferentPrivateKey_Encrypted_Failure()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    // Create key #1
    {
        String privateKeyAlias(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1);

        Boolean res;
        mAndroidKeyStore->Generate(privateKeyAlias, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
                INativeCrypto::EVP_PKEY_RSA, 1024,
                Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, NULL, &res);
        assert(TRUE == res);

        AutoPtr<IX509Certificate> cert;
        GenerateCertificate(mAndroidKeyStore, TEST_ALIAS_1,
                TEST_SERIAL_1, TEST_DN_1, NOW, NOW_PLUS_10_YEARS, (IX509Certificate**)&cert);

        AutoPtr<ArrayOf<Byte> > array;
        ICertificate::Probe(cert)->GetEncoded((ArrayOf<Byte>**)&array);
        mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1,
                array, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
                Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
        assert(TRUE == res);
    }

    // Create key #2
    {
        String privateKeyAlias(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_2);

        Boolean res;
        mAndroidKeyStore->Generate(privateKeyAlias, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
                INativeCrypto::EVP_PKEY_RSA, 1024,
                Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, NULL, &res);
        assert(TRUE == res);

        AutoPtr<IX509Certificate> cert;
        GenerateCertificate(mAndroidKeyStore, TEST_ALIAS_2,
                TEST_SERIAL_2, TEST_DN_2, NOW, NOW_PLUS_10_YEARS, (IX509Certificate**)&cert);

        AutoPtr<ArrayOf<Byte> > array;
        ICertificate::Probe(cert)->GetEncoded((ArrayOf<Byte>**)&array);
        mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_2,
                array, Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
                Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
        assert(TRUE == res);
    }

    // Replace key #1 with key #2
    {
        AutoPtr<IKey> key1;
        mKeyStore->GetKey(TEST_ALIAS_2, NULL, (IKey**)&key1);

        AutoPtr<IX509Certificate> cert;
        GenerateCertificate(mAndroidKeyStore, TEST_ALIAS_2,
                TEST_SERIAL_2, TEST_DN_2, NOW, NOW_PLUS_10_YEARS, (IX509Certificate**)&cert);

        //try {
        AutoPtr<ArrayOf<ICertificate*> > array = ArrayOf<ICertificate*>::Alloc(1);
        array->Set(0, ICertificate::Probe(cert));
        ECode ec = mKeyStore->SetKeyEntry(TEST_ALIAS_1, key1, NULL, array);
        //fail("Should not allow setting of KeyEntry with wrong PrivaetKey");
        assert(ec != NOERROR);
        //} catch (KeyStoreException success) {
        //}
    }

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetKeyEntry_ReplacedChain_DifferentPrivateKey_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_SetKeyEntry_ReplacedChain_UnencryptedToEncrypted_Failure()
{
    mKeyStore->Load(NULL, NULL);

    // Create key #1
    {
        String privateKeyAlias(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1);

        Boolean res;
        mAndroidKeyStore->Generate(privateKeyAlias,
                Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
                INativeCrypto::EVP_PKEY_RSA, 1024,
                Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, NULL, &res);
        assert(TRUE == res);

        AutoPtr<IX509Certificate> cert;
        GenerateCertificate(mAndroidKeyStore, TEST_ALIAS_1, TEST_SERIAL_1, TEST_DN_1,
                        NOW, NOW_PLUS_10_YEARS, (IX509Certificate**)&cert);

        AutoPtr<ArrayOf<Byte> > array;
        ICertificate::Probe(cert)->GetEncoded((ArrayOf<Byte>**)&array);
        mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1,
                array,
                Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
                Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_NONE, &res);
        assert(TRUE == res);
    }

    // Replace with one that requires encryption
    {
        AutoPtr<IKeyStoreEntry> entry;
        mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&entry);

        //try
        {
            AutoPtr<IKeyStoreParameterBuilder> builder;
            CKeyStoreParameterBuilder::New(mContext, (IKeyStoreParameterBuilder**)&builder);
            builder->SetEncryptionRequired(TRUE);
            AutoPtr<IKeyStoreParameter> para;
            builder->Build((IKeyStoreParameter**)&para);
            ECode ec = mKeyStore->SetEntry(TEST_ALIAS_1, entry, IKeyStoreProtectionParameter::Probe(para));
            //fail("Should not allow setting of Entry without unlocked keystore");
            assert(ec != NOERROR);
        } //catch (KeyStoreException success) {
        //}
        Boolean res;
        mAndroidKeyStore->Password(String("1111"), &res);
        assert(TRUE == res);

        mAndroidKeyStore->IsUnlocked(&res);
        assert(TRUE == res);

        AutoPtr<IKeyStoreParameterBuilder> builder;
        CKeyStoreParameterBuilder::New(mContext, (IKeyStoreParameterBuilder**)&builder);
        builder->SetEncryptionRequired(TRUE);
        AutoPtr<IKeyStoreParameter> para;
        builder->Build((IKeyStoreParameter**)&para);
        mKeyStore->SetEntry(TEST_ALIAS_1, entry, IKeyStoreProtectionParameter::Probe(para));
    }

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_SetKeyEntry_ReplacedChain_UnencryptedToEncrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_Size_Encrypted_Success()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    Boolean res;
    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    Int32 size;
    mKeyStore->GetSize(&size);
    assert(1 == size);

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    array->Set(0, TEST_ALIAS_1);
    AssertAliases(array);

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_2, FAKE_RSA_CA_1,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);

    mKeyStore->GetSize(&size);
    assert(2 == size);

    AutoPtr<ArrayOf<String> > array2 = ArrayOf<String>::Alloc(2);
    array2->Set(0, TEST_ALIAS_1);
    array2->Set(1, TEST_ALIAS_2);
    AssertAliases(array2);

    mAndroidKeyStore->Generate(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_3,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            INativeCrypto::EVP_PKEY_RSA, 1024, Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED,
            NULL, &res);
    assert(TRUE == res);

    mKeyStore->GetSize(&size);
    assert(3 == size);

    AutoPtr<ArrayOf<String> > array3 = ArrayOf<String>::Alloc(3);
    array3->Set(0, TEST_ALIAS_1);
    array3->Set(1, TEST_ALIAS_2);
    array3->Set(2, TEST_ALIAS_3);
    AssertAliases(array3);

    mAndroidKeyStore->Delete(ICredentials::CA_CERTIFICATE + TEST_ALIAS_1, &res);
    assert(TRUE == res);

    mKeyStore->GetSize(&size);
    assert(2 == size);

    AutoPtr<ArrayOf<String> > array4 = ArrayOf<String>::Alloc(2);
    array4->Set(0, TEST_ALIAS_2);
    array4->Set(1, TEST_ALIAS_3);
    AssertAliases(array4);

    mAndroidKeyStore->DelKey(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_3, &res);
    assert(TRUE == res);

    mKeyStore->GetSize(&size);
    assert(1 == size);

    AutoPtr<ArrayOf<String> > array5 = ArrayOf<String>::Alloc(1);
    array5->Set(0, TEST_ALIAS_2);
    AssertAliases(array5);

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_Size_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_Store_LoadStoreParam_Encrypted_Failure()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    //try {
    ECode ec = mKeyStore->StoreEx(NULL);
    //fail("Should throw UnsupportedOperationException when trying to store");
    assert(ec != NOERROR);
    //} catch (UnsupportedOperationException success) {
    //}

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_Store_LoadStoreParam_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_Load_InputStreamSupplied_Encrypted_Failure()
{
    AutoPtr<ArrayOf<Byte> > buf = String("FAKE KEYSTORE").GetBytes();

    AutoPtr<IByteArrayInputStream> is;
    CByteArrayInputStream::New(buf, (IByteArrayInputStream**)&is);

    //try {
    mKeyStore->Load(IInputStream::Probe(is), NULL);
    //fail("Should throw IllegalArgumentException when InputStream is supplied");
    assert(0 != NOERROR);
    //} catch (IllegalArgumentException success) {
    //}

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_Load_InputStreamSupplied_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_Load_PasswordSupplied_Encrypted_Failure()
{
    //try {
    AutoPtr<ArrayOf<Char32> > array = String("password").GetChars();
    ECode ec = mKeyStore->Load(NULL, array);
    //fail("Should throw IllegalArgumentException when password is supplied");
    assert(ec != NOERROR);
    //} catch (IllegalArgumentException success) {
    //}

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_Load_PasswordSupplied_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_Store_OutputStream_Encrypted_Failure()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    AutoPtr<IOutputStream> sink;
    CByteArrayOutputStream::New((IOutputStream**)&sink);
    //try {
    ECode ec = mKeyStore->Store(sink, NULL);
    //fail("Should throw UnsupportedOperationException when trying to store");
    assert(ec != NOERROR);
    //} catch (UnsupportedOperationException success) {
    //}

    //try {
    AutoPtr<ArrayOf<Char32> > array = String("blah").GetChars();
    ec = mKeyStore->Store(sink, array);
    //fail("Should throw UnsupportedOperationException when trying to store");
    assert(ec != NOERROR);
    //} catch (UnsupportedOperationException success) {
    //}

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_Store_OutputStream_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyStoreTest::SetupKey()
{
    String privateKeyAlias(ICredentials::USER_PRIVATE_KEY + TEST_ALIAS_1);

    Boolean res;
    mAndroidKeyStore->Generate(privateKeyAlias,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF, INativeCrypto::EVP_PKEY_RSA, 1024,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, NULL, &res);
    assert(TRUE == res);

    AutoPtr<IX509Certificate> cert;
    GenerateCertificate(mAndroidKeyStore, TEST_ALIAS_1, TEST_SERIAL_1,
            TEST_DN_1, NOW, NOW_PLUS_10_YEARS, (IX509Certificate**)&cert);

    AutoPtr<ArrayOf<Byte> > array;
    ICertificate::Probe(cert)->GetEncoded((ArrayOf<Byte>**)&array);
    mAndroidKeyStore->Put(ICredentials::USER_CERTIFICATE + TEST_ALIAS_1,
            array,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &res);
    assert(TRUE == res);
    return NOERROR;
}

ECode ElastosKeyStoreTest::TestKeyStore_KeyOperations_Wrap_Encrypted_Success()
{
    SetupPassword();
    mKeyStore->Load(NULL, NULL);

    SetupKey();

    // Test key usage
    AutoPtr<IKeyStoreEntry> e;
    mKeyStore->GetEntry(TEST_ALIAS_1, NULL, (IKeyStoreEntry**)&e);
    assert(NULL != e);
    assert(IKeyStorePrivateKeyEntry::Probe(e) != NULL);

    AutoPtr<IKeyStorePrivateKeyEntry> privEntry = IKeyStorePrivateKeyEntry::Probe(e);
    AutoPtr<IPrivateKey> privKey;
    privEntry->GetPrivateKey((IPrivateKey**)&privKey);
    assert(NULL != privKey);

    AutoPtr<ICertificate> cert;
    privEntry->GetCertificate((ICertificate**)&cert);
    AutoPtr<IPublicKey> pubKey;
    cert->GetPublicKey((IPublicKey**)&pubKey);

    AutoPtr<ICipherHelper> helper;
    CCipherHelper::AcquireSingleton((ICipherHelper**)&helper);
    AutoPtr<ICipher> c;
    helper->GetInstance(String("RSA/ECB/PKCS1Padding"), (ICipher**)&c);
    c->Init(ICipher::WRAP_MODE, IKey::Probe(pubKey));

    AutoPtr<ArrayOf<Byte> > expectedKey = ArrayOf<Byte>::Alloc(7);
    expectedKey->Set(0, 0x00);
    expectedKey->Set(1, 0x05);
    expectedKey->Set(2, (Byte)0xAA);
    expectedKey->Set(3, (Byte)0x0A5);
    expectedKey->Set(4, (Byte)0xFF);
    expectedKey->Set(5, 0x55);
    expectedKey->Set(6, 0x0A);

    AutoPtr<ISecretKey> expectedSecret;
    CSecretKeySpec::New(expectedKey, String("AES"), (ISecretKey**)&expectedSecret);

    AutoPtr<ArrayOf<Byte> > wrappedExpected;
    c->Wrap(IKey::Probe(expectedSecret), (ArrayOf<Byte>**)&wrappedExpected);

    c->Init(ICipher::UNWRAP_MODE, IKey::Probe(privKey));

    AutoPtr<IKey> key;
    c->Unwrap(wrappedExpected, String("AES"), ICipher::SECRET_KEY, (IKey**)&key);
    AutoPtr<ISecretKey> actualSecret = ISecretKey::Probe(key);

    AutoPtr<ArrayOf<Byte> > value1;
    ISecretKeySpec::Probe(expectedSecret)->GetEncoded((ArrayOf<Byte>**)&value1);
    String str1 = Arrays::ToString(value1);

    AutoPtr<ArrayOf<Byte> > value2;
    ISecretKeySpec::Probe(actualSecret)->GetEncoded((ArrayOf<Byte>**)&value2);
    String str2 = Arrays::ToString(value2);

    assert(str1.Equals(str2));

    Logger::E("ElastosKeyStoreTest", "=====[snow]=====TestKeyStore_KeyOperations_Wrap_Encrypted_Success return");
    return NOERROR;
}

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos
