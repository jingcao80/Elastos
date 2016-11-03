
#include "elastos/devsamples/securitydemo/ElastosKeyStoreTest.h"
#include "Elastos.CoreLibrary.Core.h"
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
using Elastos::Math::CBigInteger;
using Elastos::Math::IBigInteger;
using Elastos::Math::IBigIntegerHelper;
using Elastos::Math::CBigIntegerHelper;
using Elastos::Security::KeyPairGenerator;
using Elastos::Security::CKeyFactoryHelper;
using Elastos::Security::CSignatureHelper;
using Elastos::Security::IKeyFactory;
using Elastos::Security::IKeyFactoryHelper;
using Elastos::Security::IPublicKey;
using Elastos::Security::ISignature;
using Elastos::Security::ISignatureHelper;
using Elastos::Security::Spec::CRSAPublicKeySpec;
using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::IRSAPublicKeySpec;
using Elastos::Utility::CDate;
using Elastos::Utility::IList;
using Elastos::Utility::Arrays;
using Elastos::Utility::ISet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::ICollection;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::Logging::Logger;
using Elastosx::Security::Auth::X500::CX500Principal;
using Org::Conscrypt::INativeCrypto;

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
    static Byte FAKE_RSA_CA_1[] = {
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

    AutoPtr<ArrayOf<Byte> > array3 = ArrayOf<Byte>::Alloc(FAKE_RSA_CA_1
            , sizeof(FAKE_RSA_CA_1)/sizeof(FAKE_RSA_CA_1[0]));

    mAndroidKeyStore->Put(ICredentials::CA_CERTIFICATE + TEST_ALIAS_2, array3,
            Elastos::Droid::KeyStore::Security::IKeyStore::UID_SELF,
            Elastos::Droid::KeyStore::Security::IKeyStore::FLAG_ENCRYPTED, &result);
    assert(result == TRUE);

    AutoPtr<ArrayOf<String> > array4 = ArrayOf<String>::Alloc(2);
    array4->Set(0, TEST_ALIAS_1);
    array4->Set(1, TEST_ALIAS_2);
    AssertAliases(array4);
    Logger::E("ElastosKeyStoreTest", "=====[snow=====TestKeyStore_Aliases_Encrypted_Success return");
    return NOERROR;
}

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos
