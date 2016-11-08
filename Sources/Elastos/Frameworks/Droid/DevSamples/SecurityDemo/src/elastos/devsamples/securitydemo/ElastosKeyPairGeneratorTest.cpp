
#include "elastos/devsamples/securitydemo/ElastosKeyPairGeneratorTest.h"
#include "Elastos.CoreLibrary.Core.h"
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Text.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.KeyStore.h"
#include "elastos/utility/logging/Logger.h"
#include "elastos/security/KeyPairGenerator.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Droid::KeyStore::Security::ICredentials;
using Elastos::Droid::KeyStore::Security::IKeyPairGeneratorSpec;
using Elastos::Droid::KeyStore::Security::IKeyPairGeneratorSpecBuilder;
using Elastos::Droid::KeyStore::Security::CKeyPairGeneratorSpecBuilder;
using Elastos::IO::IInputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Math::CBigInteger;
using Elastos::Math::IBigInteger;
using Elastos::Math::IBigIntegerHelper;
using Elastos::Math::CBigIntegerHelper;
using Elastos::Security::IKey;
using Elastos::Security::IPrincipal;
using Elastos::Security::IPrivateKey;
using Elastos::Security::ISecureRandom;
using Elastos::Security::CSecureRandom;
using Elastos::Security::KeyPairGenerator;
using Elastos::Security::CKeyFactoryHelper;
using Elastos::Security::CSignatureHelper;
using Elastos::Security::IKeyFactory;
using Elastos::Security::IKeyFactoryHelper;
using Elastos::Security::IPublicKey;
using Elastos::Security::ISignature;
using Elastos::Security::ISignatureHelper;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Security::Cert::ICertificateFactoryHelper;
using Elastos::Security::Interfaces::IECKey;
using Elastos::Security::Interfaces::IDSAKey;
using Elastos::Security::Interfaces::IRSAKey;
using Elastos::Security::Interfaces::IRSAPublicKey;
using Elastos::Security::Interfaces::IDSAParams;
using Elastos::Security::Interfaces::IDSAPublicKey;
using Elastos::Security::Spec::CRSAPublicKeySpec;
using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::IECField;
using Elastos::Security::Spec::IDSAParameterSpec;
using Elastos::Security::Spec::CDSAParameterSpec;
using Elastos::Security::Spec::IEllipticCurve;
using Elastos::Security::Spec::IECParameterSpec;
using Elastos::Security::Spec::IRSAPublicKeySpec;
using Elastos::Security::Spec::IDSAParameterSpec;
using Elastos::Security::Spec::IRSAKeyGenParameterSpec;
using Elastos::Security::Spec::CRSAKeyGenParameterSpec;
using Elastos::Text::IDateFormat;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::CDate;
using Elastos::Utility::Logging::Logger;
using Elastosx::Security::Auth::X500::CX500Principal;

namespace Elastos {
namespace DevSamples {
namespace SecurityDemo {

const String ElastosKeyPairGeneratorTest::TEST_ALIAS_1("test1");

const String ElastosKeyPairGeneratorTest::TEST_ALIAS_2("test2");

static AutoPtr<IX500Principal> initTEST_DN_1()
{
    AutoPtr<IX500Principal> tmp;
    CX500Principal::New(String("CN=test1"), (IX500Principal**)&tmp);
    return tmp;
}

AutoPtr<IX500Principal> ElastosKeyPairGeneratorTest::TEST_DN_1 = initTEST_DN_1();

static AutoPtr<IX500Principal> initTEST_DN_2()
{
    AutoPtr<IX500Principal> tmp;
    CX500Principal::New(String("CN=test2"), (IX500Principal**)&tmp);
    return tmp;
}

AutoPtr<IX500Principal> ElastosKeyPairGeneratorTest::TEST_DN_2 = initTEST_DN_2();

static AutoPtr<IBigInteger> initTEST_SERIAL_1()
{
    AutoPtr<IBigIntegerHelper> helper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&helper);
    AutoPtr<IBigInteger> tmp;
    helper->GetONE((IBigInteger**)&tmp);
    return tmp;
}

AutoPtr<IBigInteger> ElastosKeyPairGeneratorTest::TEST_SERIAL_1 = initTEST_SERIAL_1();

static AutoPtr<IBigInteger> initTEST_SERIAL_2()
{
    AutoPtr<IBigIntegerHelper> helper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&helper);
    AutoPtr<IBigInteger> tmp;
    helper->ValueOf(2L, (IBigInteger**)&tmp);
    return tmp;
}

AutoPtr<IBigInteger> ElastosKeyPairGeneratorTest::TEST_SERIAL_2 = initTEST_SERIAL_2();

static Int64 initNOW_MILLIS()
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 m;
    system->GetCurrentTimeMillis(&m);
    return m;
}

Int64 ElastosKeyPairGeneratorTest::NOW_MILLIS = initNOW_MILLIS();

AutoPtr<IDate> ElastosKeyPairGeneratorTest::initNOW()
{
    AutoPtr<IDate> tmp;
    CDate::New(NOW_MILLIS - (NOW_MILLIS % 1000L), (IDate**)&tmp);
    return tmp;
}

AutoPtr<IDate> ElastosKeyPairGeneratorTest::NOW = initNOW();

AutoPtr<IDate> ElastosKeyPairGeneratorTest::initNOW_PLUS_10_YEARS()
{
    Int32 year;
    NOW->GetYear(&year);
    AutoPtr<IDate> tmp;
    CDate::New(year + 10, 0, 1, (IDate**)&tmp);
    return tmp;
}

AutoPtr<IDate> ElastosKeyPairGeneratorTest::NOW_PLUS_10_YEARS = initNOW_PLUS_10_YEARS();

ElastosKeyPairGeneratorTest::ElastosKeyPairGeneratorTest(
    /* [in] */ IContext* c)
    : mContext(c)
{
    AutoPtr<Elastos::Droid::KeyStore::Security::IKeyStoreHelper> helpre;
    Elastos::Droid::KeyStore::Security::CKeyStoreHelper::AcquireSingleton((Elastos::Droid::KeyStore::Security::IKeyStoreHelper**)&helpre);
    helpre->GetInstance((Elastos::Droid::KeyStore::Security::IKeyStore**)&mAndroidKeyStore);

    Boolean res;
    mAndroidKeyStore->Reset(&res);
    assert(res == TRUE);

    mAndroidKeyStore->IsUnlocked(&res);
    assert(res == FALSE);

    KeyPairGenerator::GetInstance(String("RSA"), String("ElastosKeyStore"), (IKeyPairGenerator**)&mGenerator);
}

ECode ElastosKeyPairGeneratorTest::SetupPassword()
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

ECode ElastosKeyPairGeneratorTest::TestKeyPairGenerator_Initialize_Params_Encrypted_Success()
{
    SetupPassword();

    AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
    CKeyPairGeneratorSpecBuilder::New(mContext, (IKeyPairGeneratorSpecBuilder**)&builder);
    builder->SetAlias(TEST_ALIAS_1);
    builder->SetSubject(TEST_DN_1);
    builder->SetSerialNumber(TEST_SERIAL_1);
    builder->SetStartDate(NOW);
    builder->SetEndDate(NOW_PLUS_10_YEARS);
    builder->SetEncryptionRequired();
    AutoPtr<IKeyPairGeneratorSpec> spec;
    builder->Build((IKeyPairGeneratorSpec**)&spec);

    mGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec));
    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow]=====TestKeyPairGenerator_Initialize_Params_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyPairGeneratorTest::TestKeyPairGenerator_Initialize_KeySize_Encrypted_Failure()
{
    SetupPassword();

    //try {
    mGenerator->Initialize(1024);
    //fail("KeyPairGenerator should not support setting the key size");
    //} catch (IllegalArgumentException success) {
    //}
    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow]=====TestKeyPairGenerator_Initialize_KeySize_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyPairGeneratorTest::TestKeyPairGenerator_Initialize_KeySizeAndSecureRandom_Encrypted_Failure()
{
    SetupPassword();

    //try {
    AutoPtr<ISecureRandom> r;
    CSecureRandom::New((ISecureRandom**)&r);
    mGenerator->Initialize(1024, r);
    //fail("KeyPairGenerator should not support setting the key size");
    //} catch (IllegalArgumentException success) {
    //}
    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow]=====testKeyPairGenerator_Initialize_KeySizeAndSecureRandom_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyPairGeneratorTest::TestKeyPairGenerator_Initialize_ParamsAndSecureRandom_Encrypted_Failure()
{
    SetupPassword();

    AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
    CKeyPairGeneratorSpecBuilder::New(mContext, (IKeyPairGeneratorSpecBuilder**)&builder);
    builder->SetAlias(TEST_ALIAS_1);
    builder->SetKeyType(String("RSA"));
    builder->SetKeySize(1024);
    builder->SetSubject(TEST_DN_1);
    builder->SetSerialNumber(TEST_SERIAL_1);
    builder->SetStartDate(NOW);
    builder->SetEndDate(NOW_PLUS_10_YEARS);
    builder->SetEncryptionRequired();
    AutoPtr<IKeyPairGeneratorSpec> spec;
    builder->Build((IKeyPairGeneratorSpec**)&spec);

    AutoPtr<ISecureRandom> r;
    CSecureRandom::New((ISecureRandom**)&r);

    mGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec), r);

    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow]=====TestKeyPairGenerator_Initialize_ParamsAndSecureRandom_Encrypted_Failure return");
    return NOERROR;
}

ECode ElastosKeyPairGeneratorTest::TestKeyPairGenerator_GenerateKeyPair_Encrypted_Success()
{
    SetupPassword();

    AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
    CKeyPairGeneratorSpecBuilder::New(mContext, (IKeyPairGeneratorSpecBuilder**)&builder);
    builder->SetAlias(TEST_ALIAS_1);
    builder->SetSubject(TEST_DN_1);
    builder->SetSerialNumber(TEST_SERIAL_1);
    builder->SetStartDate(NOW);
    builder->SetEndDate(NOW_PLUS_10_YEARS);
    builder->SetEncryptionRequired();
    AutoPtr<IKeyPairGeneratorSpec> spec;
    builder->Build((IKeyPairGeneratorSpec**)&spec);

    mGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec));

    AutoPtr<IKeyPair> pair;
    mGenerator->GenerateKeyPair((IKeyPair**)&pair);
    assert(pair != NULL);

    AssertKeyPairCorrect(pair, TEST_ALIAS_1, String("RSA"), 2048, NULL, TEST_DN_1, TEST_SERIAL_1, NOW,
            NOW_PLUS_10_YEARS);
    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow]=====TestKeyPairGenerator_GenerateKeyPair_Encrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyPairGeneratorTest::TestKeyPairGenerator_GenerateKeyPair_DSA_Unencrypted_Success()
{
    AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
    CKeyPairGeneratorSpecBuilder::New(mContext, (IKeyPairGeneratorSpecBuilder**)&builder);
    builder->SetAlias(TEST_ALIAS_1);
    builder->SetKeyType(String("DSA"));
    builder->SetSubject(TEST_DN_1);
    builder->SetSerialNumber(TEST_SERIAL_1);
    builder->SetStartDate(NOW);
    builder->SetEndDate(NOW_PLUS_10_YEARS);
    AutoPtr<IKeyPairGeneratorSpec> spec;
    builder->Build((IKeyPairGeneratorSpec**)&spec);

    mGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec));

    AutoPtr<IKeyPair> pair;
    mGenerator->GenerateKeyPair((IKeyPair**)&pair);
    assert(NULL != pair);

    AssertKeyPairCorrect(pair, TEST_ALIAS_1, String("DSA"), 1024, NULL, TEST_DN_1, TEST_SERIAL_1, NOW,
            NOW_PLUS_10_YEARS);
    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow]=====TestKeyPairGenerator_GenerateKeyPair_DSA_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyPairGeneratorTest::TestKeyPairGenerator_GenerateKeyPair_DSA_2048_Unencrypted_Success()
{
    AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
    CKeyPairGeneratorSpecBuilder::New(mContext, (IKeyPairGeneratorSpecBuilder**)&builder);
    builder->SetAlias(TEST_ALIAS_1);
    builder->SetKeyType(String("DSA"));
    builder->SetKeySize(2048);
    builder->SetSubject(TEST_DN_1);
    builder->SetSerialNumber(TEST_SERIAL_1);
    builder->SetStartDate(NOW);
    builder->SetEndDate(NOW_PLUS_10_YEARS);
    AutoPtr<IKeyPairGeneratorSpec> spec;
    builder->Build((IKeyPairGeneratorSpec**)&spec);

    mGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec));

    AutoPtr<IKeyPair> pair;
    mGenerator->GenerateKeyPair((IKeyPair**)&pair);
    assert(NULL != pair);

    AssertKeyPairCorrect(pair, TEST_ALIAS_1, String("DSA"), 2048, NULL, TEST_DN_1, TEST_SERIAL_1, NOW,
            NOW_PLUS_10_YEARS);
    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow]=====TestKeyPairGenerator_GenerateKeyPair_DSA_2048_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyPairGeneratorTest::TestKeyPairGenerator_GenerateKeyPair_DSA_SpecifiedParams_Unencrypted_Success()
{
    /*
     * generated using: openssl dsaparam -C 2048
     */
    Byte barray[] =  {
        (Byte) 0xC0, (Byte) 0x3D, (Byte) 0x86, (Byte) 0x09, (Byte) 0xCA, (Byte) 0x8C,
        (Byte) 0x37, (Byte) 0xCA, (Byte) 0xCC, (Byte) 0x4A, (Byte) 0x81, (Byte) 0xBD,
        (Byte) 0xD8, (Byte) 0x50, (Byte) 0x77, (Byte) 0xCD, (Byte) 0xDD, (Byte) 0x32,
        (Byte) 0x0B, (Byte) 0x43, (Byte) 0xBF, (Byte) 0x42, (Byte) 0x06, (Byte) 0x5A,
        (Byte) 0x3D, (Byte) 0x18, (Byte) 0x50, (Byte) 0x47, (Byte) 0x79, (Byte) 0xE1,
        (Byte) 0x5B, (Byte) 0x86, (Byte) 0x03, (Byte) 0xB9, (Byte) 0x28, (Byte) 0x9C,
        (Byte) 0x18, (Byte) 0xA9, (Byte) 0xF5, (Byte) 0xD6, (Byte) 0xF4, (Byte) 0x94,
        (Byte) 0x5B, (Byte) 0x87, (Byte) 0x58, (Byte) 0xCA, (Byte) 0xB2, (Byte) 0x1E,
        (Byte) 0xFC, (Byte) 0xED, (Byte) 0x37, (Byte) 0xC3, (Byte) 0x49, (Byte) 0xAC,
        (Byte) 0xFA, (Byte) 0x46, (Byte) 0xDB, (Byte) 0x7A, (Byte) 0x50, (Byte) 0x96,
        (Byte) 0xCF, (Byte) 0x52, (Byte) 0xD7, (Byte) 0x4E, (Byte) 0xEB, (Byte) 0x26,
        (Byte) 0x41, (Byte) 0xA2, (Byte) 0x6F, (Byte) 0x99, (Byte) 0x80, (Byte) 0x9F,
        (Byte) 0x0F, (Byte) 0x0A, (Byte) 0xA8, (Byte) 0x0D, (Byte) 0xAC, (Byte) 0xAB,
        (Byte) 0xEF, (Byte) 0x7D, (Byte) 0xE7, (Byte) 0x4C, (Byte) 0xF1, (Byte) 0x88,
        (Byte) 0x44, (Byte) 0xC9, (Byte) 0x17, (Byte) 0xD0, (Byte) 0xBB, (Byte) 0xE2,
        (Byte) 0x01, (Byte) 0x8C, (Byte) 0xC1, (Byte) 0x02, (Byte) 0x1D, (Byte) 0x3C,
        (Byte) 0x15, (Byte) 0xB7, (Byte) 0x41, (Byte) 0x30, (Byte) 0xD8, (Byte) 0x11,
        (Byte) 0xBD, (Byte) 0x6A, (Byte) 0x2A, (Byte) 0x0D, (Byte) 0x36, (Byte) 0x44,
        (Byte) 0x9C, (Byte) 0x3F, (Byte) 0x32, (Byte) 0xE2, (Byte) 0x1C, (Byte) 0xFB,
        (Byte) 0xE3, (Byte) 0xFF, (Byte) 0xCC, (Byte) 0x1A, (Byte) 0x72, (Byte) 0x38,
        (Byte) 0x37, (Byte) 0x69, (Byte) 0x5E, (Byte) 0x35, (Byte) 0x73, (Byte) 0xE1,
        (Byte) 0x1E, (Byte) 0x74, (Byte) 0x35, (Byte) 0x44, (Byte) 0x07, (Byte) 0xB5,
        (Byte) 0x2F, (Byte) 0x0B, (Byte) 0x60, (Byte) 0xF4, (Byte) 0xA9, (Byte) 0xE0,
        (Byte) 0x81, (Byte) 0xB2, (Byte) 0xCD, (Byte) 0x8B, (Byte) 0x82, (Byte) 0x76,
        (Byte) 0x7F, (Byte) 0xD4, (Byte) 0x17, (Byte) 0x32, (Byte) 0x86, (Byte) 0x98,
        (Byte) 0x7C, (Byte) 0x85, (Byte) 0x66, (Byte) 0xF6, (Byte) 0x77, (Byte) 0xED,
        (Byte) 0x8B, (Byte) 0x1A, (Byte) 0x52, (Byte) 0x16, (Byte) 0xDA, (Byte) 0x1C,
        (Byte) 0xA7, (Byte) 0x16, (Byte) 0x79, (Byte) 0x20, (Byte) 0x1C, (Byte) 0x99,
        (Byte) 0x5F, (Byte) 0x12, (Byte) 0x66, (Byte) 0x15, (Byte) 0x9F, (Byte) 0xE5,
        (Byte) 0x73, (Byte) 0xA9, (Byte) 0x61, (Byte) 0xBA, (Byte) 0xA7, (Byte) 0x23,
        (Byte) 0x93, (Byte) 0x77, (Byte) 0xB5, (Byte) 0xF6, (Byte) 0xEC, (Byte) 0x13,
        (Byte) 0xBF, (Byte) 0x95, (Byte) 0x60, (Byte) 0x78, (Byte) 0x84, (Byte) 0xE3,
        (Byte) 0x44, (Byte) 0xEC, (Byte) 0x74, (Byte) 0xC2, (Byte) 0xCB, (Byte) 0xD4,
        (Byte) 0x70, (Byte) 0xC5, (Byte) 0x7B, (Byte) 0xF8, (Byte) 0x07, (Byte) 0x3B,
        (Byte) 0xEB, (Byte) 0x9F, (Byte) 0xC9, (Byte) 0x7D, (Byte) 0xE0, (Byte) 0xA5,
        (Byte) 0xBA, (Byte) 0x68, (Byte) 0x7B, (Byte) 0xF4, (Byte) 0x70, (Byte) 0x40,
        (Byte) 0xAE, (Byte) 0xE9, (Byte) 0x65, (Byte) 0xEE, (Byte) 0x5B, (Byte) 0x71,
        (Byte) 0x36, (Byte) 0x0B, (Byte) 0xB0, (Byte) 0xA2, (Byte) 0x98, (Byte) 0x7D,
        (Byte) 0xE3, (Byte) 0x24, (Byte) 0x95, (Byte) 0x2B, (Byte) 0xC2, (Byte) 0x0A,
        (Byte) 0x78, (Byte) 0x3D, (Byte) 0xCC, (Byte) 0x3A, (Byte) 0xEE, (Byte) 0xED,
        (Byte) 0x48, (Byte) 0xEB, (Byte) 0xA3, (Byte) 0x78, (Byte) 0xA8, (Byte) 0x9D,
        (Byte) 0x0A, (Byte) 0x8F, (Byte) 0x9E, (Byte) 0x59, (Byte) 0x2C, (Byte) 0x44,
        (Byte) 0xB5, (Byte) 0xF9, (Byte) 0x53, (Byte) 0x43,
    };
    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(barray, sizeof(barray)/sizeof(barray[0]));
    AutoPtr<IBigInteger> p;
    CBigInteger::New(1, *array, (IBigInteger**)&p);

    Byte barray2[] = {
        (Byte) 0xA1, (Byte) 0x9B, (Byte) 0x1D, (Byte) 0xC0, (Byte) 0xE3, (Byte) 0xF6,
        (Byte) 0x4A, (Byte) 0x35, (Byte) 0xE1, (Byte) 0x8A, (Byte) 0x43, (Byte) 0xC2,
        (Byte) 0x9C, (Byte) 0xF9, (Byte) 0x52, (Byte) 0x8F, (Byte) 0x94, (Byte) 0xA1,
        (Byte) 0x12, (Byte) 0x11, (Byte) 0xDB, (Byte) 0x9A, (Byte) 0xB6, (Byte) 0x35,
        (Byte) 0x56, (Byte) 0x26, (Byte) 0x60, (Byte) 0x89, (Byte) 0x11, (Byte) 0xAC,
        (Byte) 0xA8, (Byte) 0xE5,
    };
    AutoPtr<ArrayOf<Byte> > array2 = ArrayOf<Byte>::Alloc(barray2, sizeof(barray2)/sizeof(barray2[0]));
    AutoPtr<IBigInteger> q;
    CBigInteger::New(1, *array2, (IBigInteger**)&q);

    Byte barray3[] = {
        (Byte) 0xA1, (Byte) 0x5C, (Byte) 0x57, (Byte) 0x15, (Byte) 0xC3, (Byte) 0xD9,
        (Byte) 0xD7, (Byte) 0x41, (Byte) 0x89, (Byte) 0xD6, (Byte) 0xB8, (Byte) 0x7B,
        (Byte) 0xF3, (Byte) 0xE0, (Byte) 0xB3, (Byte) 0xC5, (Byte) 0xD1, (Byte) 0xAA,
        (Byte) 0xF9, (Byte) 0x55, (Byte) 0x48, (Byte) 0xF1, (Byte) 0xDA, (Byte) 0xE8,
        (Byte) 0x6F, (Byte) 0x51, (Byte) 0x05, (Byte) 0xB2, (Byte) 0xC9, (Byte) 0x64,
        (Byte) 0xDA, (Byte) 0x5F, (Byte) 0xD4, (Byte) 0xAA, (Byte) 0xFD, (Byte) 0x67,
        (Byte) 0xE0, (Byte) 0x10, (Byte) 0x2C, (Byte) 0x1F, (Byte) 0x03, (Byte) 0x10,
        (Byte) 0xD4, (Byte) 0x4B, (Byte) 0x20, (Byte) 0x82, (Byte) 0x2B, (Byte) 0x04,
        (Byte) 0xF9, (Byte) 0x09, (Byte) 0xAE, (Byte) 0x28, (Byte) 0x3D, (Byte) 0x9B,
        (Byte) 0xFF, (Byte) 0x87, (Byte) 0x76, (Byte) 0xCD, (Byte) 0xF0, (Byte) 0x11,
        (Byte) 0xB7, (Byte) 0xEA, (Byte) 0xE6, (Byte) 0xCD, (Byte) 0x60, (Byte) 0xD3,
        (Byte) 0x8C, (Byte) 0x74, (Byte) 0xD3, (Byte) 0x45, (Byte) 0x63, (Byte) 0x69,
        (Byte) 0x3F, (Byte) 0x1D, (Byte) 0x31, (Byte) 0x25, (Byte) 0x49, (Byte) 0x97,
        (Byte) 0x4B, (Byte) 0x73, (Byte) 0x34, (Byte) 0x12, (Byte) 0x73, (Byte) 0x27,
        (Byte) 0x4C, (Byte) 0xDA, (Byte) 0xF3, (Byte) 0x08, (Byte) 0xA8, (Byte) 0xA9,
        (Byte) 0x27, (Byte) 0xE4, (Byte) 0xB8, (Byte) 0xD6, (Byte) 0xB5, (Byte) 0xC4,
        (Byte) 0x18, (Byte) 0xED, (Byte) 0xBD, (Byte) 0x6F, (Byte) 0xA2, (Byte) 0x36,
        (Byte) 0xA2, (Byte) 0x9C, (Byte) 0x27, (Byte) 0x62, (Byte) 0x7F, (Byte) 0x93,
        (Byte) 0xD7, (Byte) 0x52, (Byte) 0xA9, (Byte) 0x76, (Byte) 0x55, (Byte) 0x99,
        (Byte) 0x00, (Byte) 0x5B, (Byte) 0xC2, (Byte) 0xB9, (Byte) 0x18, (Byte) 0xAC,
        (Byte) 0x6B, (Byte) 0x83, (Byte) 0x0D, (Byte) 0xA1, (Byte) 0xC5, (Byte) 0x01,
        (Byte) 0x1A, (Byte) 0xE5, (Byte) 0x4D, (Byte) 0x2F, (Byte) 0xCF, (Byte) 0x5D,
        (Byte) 0xB2, (Byte) 0xE7, (Byte) 0xC7, (Byte) 0xCB, (Byte) 0x2C, (Byte) 0xFF,
        (Byte) 0x51, (Byte) 0x1B, (Byte) 0x9D, (Byte) 0xA4, (Byte) 0x05, (Byte) 0xEB,
        (Byte) 0x17, (Byte) 0xD8, (Byte) 0x97, (Byte) 0x9D, (Byte) 0x0C, (Byte) 0x59,
        (Byte) 0x92, (Byte) 0x8A, (Byte) 0x03, (Byte) 0x34, (Byte) 0xFD, (Byte) 0x16,
        (Byte) 0x0F, (Byte) 0x2A, (Byte) 0xF9, (Byte) 0x7D, (Byte) 0xC3, (Byte) 0x41,
        (Byte) 0x0D, (Byte) 0x06, (Byte) 0x5A, (Byte) 0x4B, (Byte) 0x34, (Byte) 0xD5,
        (Byte) 0xF5, (Byte) 0x09, (Byte) 0x1C, (Byte) 0xCE, (Byte) 0xA7, (Byte) 0x19,
        (Byte) 0x6D, (Byte) 0x04, (Byte) 0x53, (Byte) 0x71, (Byte) 0xCC, (Byte) 0x84,
        (Byte) 0xA0, (Byte) 0xB2, (Byte) 0xA0, (Byte) 0x68, (Byte) 0xA3, (Byte) 0x40,
        (Byte) 0xC0, (Byte) 0x67, (Byte) 0x38, (Byte) 0x96, (Byte) 0x73, (Byte) 0x2E,
        (Byte) 0x8E, (Byte) 0x2A, (Byte) 0x9D, (Byte) 0x56, (Byte) 0xE9, (Byte) 0xAC,
        (Byte) 0xC7, (Byte) 0xEC, (Byte) 0x84, (Byte) 0x7F, (Byte) 0xFC, (Byte) 0xE0,
        (Byte) 0x69, (Byte) 0x03, (Byte) 0x8B, (Byte) 0x48, (Byte) 0x64, (Byte) 0x76,
        (Byte) 0x85, (Byte) 0xA5, (Byte) 0x10, (Byte) 0xD9, (Byte) 0x31, (Byte) 0xC3,
        (Byte) 0x8B, (Byte) 0x07, (Byte) 0x48, (Byte) 0x62, (Byte) 0xF6, (Byte) 0x68,
        (Byte) 0xF2, (Byte) 0x96, (Byte) 0xB2, (Byte) 0x18, (Byte) 0x5B, (Byte) 0xFF,
        (Byte) 0x6D, (Byte) 0xD1, (Byte) 0x6B, (Byte) 0xF5, (Byte) 0xFD, (Byte) 0x81,
        (Byte) 0xF1, (Byte) 0xFD, (Byte) 0x04, (Byte) 0xF0, (Byte) 0x9F, (Byte) 0xB7,
        (Byte) 0x08, (Byte) 0x95, (Byte) 0x57, (Byte) 0x48, (Byte) 0x07, (Byte) 0x00,
        (Byte) 0x52, (Byte) 0xEC, (Byte) 0x75, (Byte) 0x91, (Byte) 0x02, (Byte) 0x11,
        (Byte) 0xA3, (Byte) 0x64, (Byte) 0x26, (Byte) 0xCA,
    };
    AutoPtr<ArrayOf<Byte> > array3 = ArrayOf<Byte>::Alloc(barray3, sizeof(barray3)/sizeof(barray3[0]));
    AutoPtr<IBigInteger> g;
    CBigInteger::New(1, *array3, (IBigInteger**)&g);

    AutoPtr<IAlgorithmParameterSpec> spec;
    CDSAParameterSpec::New(p, q, g, (IAlgorithmParameterSpec**)&spec);

    AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
    CKeyPairGeneratorSpecBuilder::New(mContext, (IKeyPairGeneratorSpecBuilder**)&builder);
    builder->SetAlias(TEST_ALIAS_1);
    builder->SetKeyType(String("DSA"));
    builder->SetKeySize(2048);
    builder->SetAlgorithmParameterSpec(spec);
    builder->SetSubject(TEST_DN_1);
    builder->SetSerialNumber(TEST_SERIAL_1);
    builder->SetStartDate(NOW);
    builder->SetEndDate(NOW_PLUS_10_YEARS);
    AutoPtr<IKeyPairGeneratorSpec> _spec;
    builder->Build((IKeyPairGeneratorSpec**)&_spec);

    mGenerator->Initialize(IAlgorithmParameterSpec::Probe(_spec));

    AutoPtr<IKeyPair> pair;
    mGenerator->GenerateKeyPair((IKeyPair**)&pair);
    assert(NULL != pair);

    AssertKeyPairCorrect(pair, TEST_ALIAS_1, String("DSA"), 2048, spec, TEST_DN_1, TEST_SERIAL_1, NOW,
            NOW_PLUS_10_YEARS);

    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow]=====TestKeyPairGenerator_GenerateKeyPair_DSA_SpecifiedParams_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyPairGeneratorTest::TestKeyPairGenerator_GenerateKeyPair_EC_Unencrypted_Success()
{
    AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
    CKeyPairGeneratorSpecBuilder::New(mContext, (IKeyPairGeneratorSpecBuilder**)&builder);
    builder->SetAlias(TEST_ALIAS_1);
    builder->SetKeyType(String("EC"));
    builder->SetSubject(TEST_DN_1);
    builder->SetSerialNumber(TEST_SERIAL_1);
    builder->SetStartDate(NOW);
    builder->SetEndDate(NOW_PLUS_10_YEARS);
    AutoPtr<IKeyPairGeneratorSpec> spec;
    builder->Build((IKeyPairGeneratorSpec**)&spec);

    mGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec));

    AutoPtr<IKeyPair> pair;
    mGenerator->GenerateKeyPair((IKeyPair**)&pair);
    assert(NULL != pair);

    AssertKeyPairCorrect(pair, TEST_ALIAS_1, String("EC"), 256, NULL, TEST_DN_1, TEST_SERIAL_1, NOW,
            NOW_PLUS_10_YEARS);

    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow]=====TestKeyPairGenerator_GenerateKeyPair_EC_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyPairGeneratorTest::TestKeyPairGenerator_GenerateKeyPair_EC_P521_Unencrypted_Success()
{
    AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
    CKeyPairGeneratorSpecBuilder::New(mContext, (IKeyPairGeneratorSpecBuilder**)&builder);
    builder->SetAlias(TEST_ALIAS_1);
    builder->SetKeyType(String("EC"));
    builder->SetKeySize(521);
    builder->SetSubject(TEST_DN_1);
    builder->SetSerialNumber(TEST_SERIAL_1);
    builder->SetStartDate(NOW);
    builder->SetEndDate(NOW_PLUS_10_YEARS);
    AutoPtr<IKeyPairGeneratorSpec> spec;
    builder->Build((IKeyPairGeneratorSpec**)&spec);

    mGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec));

    AutoPtr<IKeyPair> pair;
    mGenerator->GenerateKeyPair((IKeyPair**)&pair);
    assert(NULL != pair);

    AssertKeyPairCorrect(pair, TEST_ALIAS_1, String("EC"), 521, NULL, TEST_DN_1, TEST_SERIAL_1, NOW,
            NOW_PLUS_10_YEARS);
    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow]=====TestKeyPairGenerator_GenerateKeyPair_EC_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyPairGeneratorTest::TestKeyPairGenerator_GenerateKeyPair_RSA_Unencrypted_Success()
{
    AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
    CKeyPairGeneratorSpecBuilder::New(mContext, (IKeyPairGeneratorSpecBuilder**)&builder);
    builder->SetAlias(TEST_ALIAS_1);
    builder->SetSubject(TEST_DN_1);
    builder->SetSerialNumber(TEST_SERIAL_1);
    builder->SetStartDate(NOW);
    builder->SetEndDate(NOW_PLUS_10_YEARS);
    AutoPtr<IKeyPairGeneratorSpec> spec;
    builder->Build((IKeyPairGeneratorSpec**)&spec);

    mGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec));

    AutoPtr<IKeyPair> pair;
    mGenerator->GenerateKeyPair((IKeyPair**)&pair);
    assert(NULL != pair);

    AssertKeyPairCorrect(pair, TEST_ALIAS_1, String("RSA"), 2048, NULL, TEST_DN_1, TEST_SERIAL_1, NOW,
            NOW_PLUS_10_YEARS);
    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow]=====TestKeyPairGenerator_GenerateKeyPair_RSA_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyPairGeneratorTest::TestKeyPairGenerator_GenerateKeyPair_RSA_WithParams_Unencrypted_Success()
{
    AutoPtr<IBigIntegerHelper> helper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&helper);
    AutoPtr<IBigInteger> b;
    helper->ValueOf(3L, (IBigInteger**)&b);

    AutoPtr<IAlgorithmParameterSpec> spec;
    CRSAKeyGenParameterSpec::New(1024, b, (IAlgorithmParameterSpec**)&spec);

    AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
    CKeyPairGeneratorSpecBuilder::New(mContext, (IKeyPairGeneratorSpecBuilder**)&builder);
    builder->SetAlias(TEST_ALIAS_1);
    builder->SetKeySize(1024);
    builder->SetAlgorithmParameterSpec(spec);
    builder->SetSubject(TEST_DN_1);
    builder->SetSerialNumber(TEST_SERIAL_1);
    builder->SetStartDate(NOW);
    builder->SetEndDate(NOW_PLUS_10_YEARS);
    AutoPtr<IKeyPairGeneratorSpec> _spec;
    builder->Build((IKeyPairGeneratorSpec**)&_spec);

    mGenerator->Initialize(IAlgorithmParameterSpec::Probe(_spec));

    AutoPtr<IKeyPair> pair;
    mGenerator->GenerateKeyPair((IKeyPair**)&pair);
    assert(NULL != pair);

    AssertKeyPairCorrect(pair, TEST_ALIAS_1, String("RSA"), 1024, spec, TEST_DN_1, TEST_SERIAL_1, NOW,
            NOW_PLUS_10_YEARS);
    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow]=====TestKeyPairGenerator_GenerateKeyPair_RSA_WithParams_Unencrypted_Success return");
    return NOERROR;
}

ECode ElastosKeyPairGeneratorTest::TestKeyPairGenerator_GenerateKeyPair_Replaced_Success()
{
    // Generate the first key
    {
        AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
        CKeyPairGeneratorSpecBuilder::New(mContext, (IKeyPairGeneratorSpecBuilder**)&builder);
        builder->SetAlias(TEST_ALIAS_1);
        builder->SetSubject(TEST_DN_1);
        builder->SetSerialNumber(TEST_SERIAL_1);
        builder->SetStartDate(NOW);
        builder->SetEndDate(NOW_PLUS_10_YEARS);
        AutoPtr<IKeyPairGeneratorSpec> spec;
        builder->Build((IKeyPairGeneratorSpec**)&spec);

        mGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec));

        AutoPtr<IKeyPair> pair1;
        mGenerator->GenerateKeyPair((IKeyPair**)&pair1);
        assert(NULL != pair1);

        AssertKeyPairCorrect(pair1, TEST_ALIAS_1, String("RSA"), 2048, NULL, TEST_DN_1, TEST_SERIAL_1,
                NOW, NOW_PLUS_10_YEARS);
    }

    // Replace the original key
    {
        AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
        CKeyPairGeneratorSpecBuilder::New(mContext, (IKeyPairGeneratorSpecBuilder**)&builder);
        builder->SetAlias(TEST_ALIAS_2);
        builder->SetSubject(TEST_DN_2);
        builder->SetSerialNumber(TEST_SERIAL_2);
        builder->SetStartDate(NOW);
        builder->SetEndDate(NOW_PLUS_10_YEARS);
        AutoPtr<IKeyPairGeneratorSpec> spec;
        builder->Build((IKeyPairGeneratorSpec**)&spec);

        mGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec));

        AutoPtr<IKeyPair> pair2;
        mGenerator->GenerateKeyPair((IKeyPair**)&pair2);
        assert(NULL != pair2);

        AssertKeyPairCorrect(pair2, TEST_ALIAS_2, String("RSA"), 2048, NULL, TEST_DN_2, TEST_SERIAL_2,
                NOW, NOW_PLUS_10_YEARS);
    }
    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow]=====TestKeyPairGenerator_GenerateKeyPair_Replaced_Success return");
    return NOERROR;
}

ECode ElastosKeyPairGeneratorTest::TestKeyPairGenerator_GenerateKeyPair_Replaced_UnencryptedToEncrypted_Success()
{
    // Generate the first key
    {
        AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
        CKeyPairGeneratorSpecBuilder::New(mContext, (IKeyPairGeneratorSpecBuilder**)&builder);
        builder->SetAlias(TEST_ALIAS_1);
        builder->SetSubject(TEST_DN_1);
        builder->SetSerialNumber(TEST_SERIAL_1);
        builder->SetStartDate(NOW);
        builder->SetEndDate(NOW_PLUS_10_YEARS);
        AutoPtr<IKeyPairGeneratorSpec> spec;
        builder->Build((IKeyPairGeneratorSpec**)&spec);

        mGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec));

        AutoPtr<IKeyPair> pair1;
        mGenerator->GenerateKeyPair((IKeyPair**)&pair1);
        assert(NULL != pair1);

        AssertKeyPairCorrect(pair1, TEST_ALIAS_1, String("RSA"), 2048, NULL, TEST_DN_1, TEST_SERIAL_1,
                NOW, NOW_PLUS_10_YEARS);
    }

    // Attempt to replace previous key
    {
        AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
        CKeyPairGeneratorSpecBuilder::New(mContext, (IKeyPairGeneratorSpecBuilder**)&builder);
        builder->SetAlias(TEST_ALIAS_1);
        builder->SetSubject(TEST_DN_2);
        builder->SetSerialNumber(TEST_SERIAL_2);
        builder->SetStartDate(NOW);
        builder->SetEndDate(NOW_PLUS_10_YEARS);
        builder->SetEncryptionRequired();
        AutoPtr<IKeyPairGeneratorSpec> spec;
        builder->Build((IKeyPairGeneratorSpec**)&spec);

        mGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec));

        AutoPtr<IKeyPair> pair2;
        mGenerator->GenerateKeyPair((IKeyPair**)&pair2);
        assert(0 && "TODO: should failed");
        //fail("Should not be able to generate encrypted key while not initialized");

        Boolean res;
        mAndroidKeyStore->Password(String("1111"), &res);
        assert(res == TRUE);

        mAndroidKeyStore->IsUnlocked(&res);
        assert(res == TRUE);

        AutoPtr<IKeyPair> pair3;
        mGenerator->GenerateKeyPair((IKeyPair**)&pair3);
        assert(NULL != pair3);

        AssertKeyPairCorrect(pair3, TEST_ALIAS_1, String("RSA"), 2048, NULL, TEST_DN_2, TEST_SERIAL_2,
                NOW, NOW_PLUS_10_YEARS);
    }
    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow]=====TestKeyPairGenerator_GenerateKeyPair_Replaced_UnencryptedToEncrypted_Success return");
    return NOERROR;
}

Boolean ElastosKeyPairGeneratorTest::IsEquals(
    /* [in] */ IInterface* expected,
    /* [in] */ IInterface* actual)
{
    Boolean res;
    IObject::Probe(expected)->Equals(actual, &res);
    return res;
}

ECode ElastosKeyPairGeneratorTest::AssertEquals(
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

ECode ElastosKeyPairGeneratorTest::AssertDateEquals(
    /* [in] */ IDate* date1,
    /* [in] */ IDate* date2)
{
    AutoPtr<ISimpleDateFormat> formatter;
    CSimpleDateFormat::New(String("dd MMM yyyy HH:mm:ss"), (ISimpleDateFormat**)&formatter);

    String result1;
    IDateFormat::Probe(formatter)->Format(date1, &result1);
    String result2;
    IDateFormat::Probe(formatter)->Format(date2, &result2);

    if (result1.IsNull() && result2.IsNull()) {
        return NOERROR;
    }

    if (!result1.IsNull()) {
        assert(TRUE == result1.Equals(result2));
    }

    return NOERROR;
}

ECode ElastosKeyPairGeneratorTest::AssertKeyPairCorrect(
    /* [in] */ IKeyPair* pair,
    /* [in] */ const String& alias,
    /* [in] */ const String& keyType,
    /* [in] */ Int32 keySize,
    /* [in] */ IAlgorithmParameterSpec* spec,
    /* [in] */ IX500Principal* dn,
    /* [in] */ IBigInteger* serial,
    /* [in] */ IDate* start,
    /* [in] */ IDate* end)
{
    AutoPtr<IPublicKey> pubKey;
    pair->GetPublic((IPublicKey**)&pubKey);
    assert(NULL != pubKey);

    String algorithm;
    IKey::Probe(pubKey)->GetAlgorithm(&algorithm);
    assert(TRUE == keyType.Equals(algorithm));

    if (String("DSA").EqualsIgnoreCase(keyType)) {
        AutoPtr<IDSAPublicKey> dsaPubKey = IDSAPublicKey::Probe(pubKey);
        AutoPtr<IDSAParams> actualParams;
        IDSAKey::Probe(dsaPubKey)->GetParams((IDSAParams**)&actualParams);

        AutoPtr<IBigInteger> bi;
        actualParams->GetP((IBigInteger**)&bi);
        Int32 len;
        bi->BitLength(&len);
        assert(keySize == ((len + 7) & ~7));

        if (spec != NULL) {
            AutoPtr<IDSAParameterSpec> expectedParams = IDSAParameterSpec::Probe(spec);

            AutoPtr<IBigInteger> ep;
            IDSAParams::Probe(expectedParams)->GetP((IBigInteger**)&ep);
            AutoPtr<IBigInteger> eq;
            IDSAParams::Probe(expectedParams)->GetQ((IBigInteger**)&eq);
            AutoPtr<IBigInteger> eg;
            IDSAParams::Probe(expectedParams)->GetG((IBigInteger**)&eg);

            AutoPtr<IBigInteger> ap;
            actualParams->GetP((IBigInteger**)&ap);
            AutoPtr<IBigInteger> aq;
            actualParams->GetQ((IBigInteger**)&aq);
            AutoPtr<IBigInteger> ag;
            actualParams->GetG((IBigInteger**)&ag);

            AssertEquals(TO_IINTERFACE(ep), TO_IINTERFACE(ap));
            AssertEquals(TO_IINTERFACE(eq), TO_IINTERFACE(aq));
            AssertEquals(TO_IINTERFACE(eg), TO_IINTERFACE(ag));
        }
    }
    else if (String("EC").EqualsIgnoreCase(keyType)) {
        AutoPtr<IECParameterSpec> param;
        IECKey::Probe(pubKey)->GetParams((IECParameterSpec**)&param);
        AutoPtr<IEllipticCurve> curve;
        param->GetCurve((IEllipticCurve**)&curve);
        AutoPtr<IECField> field;
        curve->GetField((IECField**)&field);
        Int32 size;
        field->GetFieldSize(&size);

        assert(keySize == size);
    }
    else if (String("RSA").EqualsIgnoreCase(keyType)) {
        AutoPtr<IRSAPublicKey> rsaPubKey = IRSAPublicKey::Probe(pubKey);
        AutoPtr<IBigInteger> modulus;
        IRSAKey::Probe(rsaPubKey)->GetModulus((IBigInteger**)&modulus);
        Int32 len;
        modulus->BitLength(&len);
        assert(((keySize + 7) & ~7) == ((len + 7) & ~7));

        if (spec != NULL) {
            AutoPtr<IRSAKeyGenParameterSpec> params = IRSAKeyGenParameterSpec::Probe(spec);
            Int32 size;
            params->GetKeysize(&size);
            assert(((keySize + 7) & ~7) == ((size + 7) & ~7));

            AutoPtr<IBigInteger> pubExponent;
            params->GetPublicExponent((IBigInteger**)&pubExponent);

            AutoPtr<IBigInteger> publicExponent;
            rsaPubKey->GetPublicExponent((IBigInteger**)&publicExponent);

            AssertEquals(TO_IINTERFACE(pubExponent), TO_IINTERFACE(publicExponent));
        }
    }

    AutoPtr<IPrivateKey> privKey;
    pair->GetPrivate((IPrivateKey**)&privKey);
    assert(NULL != privKey);

    String algorithm2;
    IKey::Probe(privKey)->GetAlgorithm(&algorithm2);
    assert(TRUE == keyType.Equals(algorithm2));

    AutoPtr<ArrayOf<Byte> > userCertBytes;
    mAndroidKeyStore->Get(ICredentials::USER_CERTIFICATE + alias, (ArrayOf<Byte>**)&userCertBytes);
    assert(NULL != userCertBytes);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> cf;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&cf);
    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(userCertBytes, (IByteArrayInputStream**)&stream);
    AutoPtr<ICertificate> userCert;
    cf->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&userCert);
    assert(IX509Certificate::Probe(userCert) != NULL);

    AutoPtr<IX509Certificate> x509userCert = IX509Certificate::Probe(userCert);

    AutoPtr<IPublicKey> x509PublicKey;
    ICertificate::Probe(x509userCert)->GetPublicKey((IPublicKey**)&x509PublicKey);
    AssertEquals(TO_IINTERFACE(pubKey), TO_IINTERFACE(x509PublicKey));

    AutoPtr<IPrincipal> p;
    x509userCert->GetSubjectDN((IPrincipal**)&p);
    AutoPtr<IX500Principal> x509SubjectDn = IX500Principal::Probe(p);
    AssertEquals(TO_IINTERFACE(dn), TO_IINTERFACE(x509SubjectDn));

    AutoPtr<IPrincipal> p2;
    x509userCert->GetIssuerDN((IPrincipal**)&p2);
    AutoPtr<IX500Principal> x509IssuerDn = IX500Principal::Probe(p2);
    AssertEquals(TO_IINTERFACE(dn), TO_IINTERFACE(x509IssuerDn));

    AutoPtr<IBigInteger> x509SerialNumber;
    x509userCert->GetSerialNumber((IBigInteger**)&x509SerialNumber);
    AssertEquals(TO_IINTERFACE(serial), TO_IINTERFACE(x509SerialNumber));

    AutoPtr<IDate> x509NotBefore;
    x509userCert->GetNotBefore((IDate**)&x509NotBefore);
    AssertDateEquals(start, x509NotBefore);

    AutoPtr<IDate> x509NotAfter;
    x509userCert->GetNotAfter((IDate**)&x509NotAfter);
    AssertDateEquals(end, x509NotAfter);

    ICertificate::Probe(x509userCert)->Verify(pubKey);

    AutoPtr<ArrayOf<Byte> > caCerts;
    mAndroidKeyStore->Get(ICredentials::CA_CERTIFICATE + alias, (ArrayOf<Byte>**)&caCerts);
    assert(NULL != caCerts);

    AutoPtr<ArrayOf<Byte> > pubKeyBytes;
    mAndroidKeyStore->GetPubkey(ICredentials::USER_PRIVATE_KEY + alias, (ArrayOf<Byte>**)&pubKeyBytes);
    assert(NULL != pubKeyBytes);

    return NOERROR;
}

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos
