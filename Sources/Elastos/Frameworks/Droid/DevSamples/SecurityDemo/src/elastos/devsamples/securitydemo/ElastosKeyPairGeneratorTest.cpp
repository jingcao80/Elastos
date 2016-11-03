
#include "elastos/devsamples/securitydemo/ElastosKeyPairGeneratorTest.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.KeyStore.h"
#include "elastos/utility/logging/Logger.h"
#include "elastos/security/KeyPairGenerator.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
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
    Logger::E("ElastosKeyPairGeneratorTest", "=====[snow=====TestKeyPairGenerator_Initialize_Params_Encrypted_Success return");
    return NOERROR;
}

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos
