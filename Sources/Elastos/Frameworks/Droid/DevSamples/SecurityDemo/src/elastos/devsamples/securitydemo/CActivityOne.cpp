
#include "elastos/devsamples/securitydemo/CActivityOne.h"
#include "org/apache/harmony/security/fortress/Services.h"
#include "elastos/devsamples/securitydemo/CipherTest.h"
#include "elastos/devsamples/securitydemo/Test.h"
#include "elastos/devsamples/securitydemo/ElastosKeyStoreTest.h"
#include "elastos/devsamples/securitydemo/ElastosKeyPairGeneratorTest.h"
#include "R.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.KeyStore.h"
#include "elastos/security/KeyPairGenerator.h"
#include <elastos/core/StringUtils.h>
#include "_Org.Conscrypt.h"
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Utility.h"

using Elastos::Core::StringUtils;
using Elastos::Droid::Utility::CBase64;
using Elastos::Droid::Utility::IBase64;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IStringBufferInputStream;
using Elastos::IO::CStringBufferInputStream;
using Elastos::IO::IInputStream;
using Elastos::Security::CSecureRandom;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::CKeyFactoryHelper;
using Elastos::Security::CKeyPairGeneratorHelper;
using Elastos::Security::CKeyStoreHelper;
using Elastos::Security::CMessageDigestHelper;
using Elastos::Security::CSignatureHelper;
using Elastos::Security::IKey;
using Elastos::Security::IKeyFactory;
using Elastos::Security::IKeyFactoryHelper;
using Elastos::Security::IKeyPairGenerator;
using Elastos::Security::IKeyPairGeneratorHelper;
using Elastos::Security::IKeyStore;
using Elastos::Security::IKeyStoreEntry;
using Elastos::Security::IKeyStoreHelper;
using Elastos::Security::IKeyFactoryHelper;
using Elastos::Security::CKeyFactoryHelper;
using Elastos::Security::IKeyFactory;
using Elastos::Security::IKeyStorePrivateKeyEntry;
using Elastos::Security::IMessageDigest;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Security::IPrincipal;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::ISignature;
using Elastos::Security::ISignatureHelper;
using Elastos::Security::ISecureRandom;
using Elastos::Security::CSecureRandomHelper;
using Elastos::Security::ISecureRandomHelper;
using Elastos::Security::Cert::ICertificateFactoryHelper;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::Spec::IRSAPrivateKeySpec;
using Elastos::Security::Spec::CRSAPrivateKeySpec;
using Elastos::Security::Spec::IX509EncodedKeySpec;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::IECPrivateKeySpec;
using Elastos::Security::Spec::CECPrivateKeySpec;
using Elastos::Security::Spec::IECParameterSpec;
using Elastos::Security::Spec::CECParameterSpec;
using Elastos::Security::Spec::IEllipticCurve;
using Elastos::Security::Spec::CEllipticCurve;
using Elastos::Security::Spec::IECField;
using Elastos::Security::Spec::CECFieldFp;
using Elastos::Security::Spec::IECPoint;
using Elastos::Security::Spec::CECPoint;
using Elastos::Security::Spec::IPKCS8EncodedKeySpec;
using Elastos::Security::Spec::CPKCS8EncodedKeySpec;
using Elastos::Security::Spec::IX509EncodedKeySpec;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Harmony::Security::Fortress::Services;

using Elastosx::Crypto::IKeyAgreement;
using Elastosx::Crypto::IKeyAgreementHelper;
using Elastosx::Crypto::CKeyAgreementHelper;
using Elastosx::Crypto::IKeyGeneratorHelper;
using Elastosx::Crypto::CKeyGeneratorHelper;
using Elastosx::Crypto::IKeyGenerator;
using Elastosx::Crypto::ISecretKey;
using Elastosx::Crypto::IMacHelper;
using Elastosx::Crypto::CMacHelper;
using Elastosx::Crypto::IMac;

using Elastos::Droid::KeyStore::Security::IKeyPairGeneratorSpecBuilder;
using Elastos::Droid::KeyStore::Security::CKeyPairGeneratorSpecBuilder;
using Elastosx::Security::Auth::X500::IX500Principal;
using Elastosx::Security::Auth::X500::CX500Principal;
using Elastos::Math::IBigIntegerHelper;
using Elastos::Math::CBigIntegerHelper;
using Elastos::Math::IBigInteger;
using Elastos::Utility::IDate;
using Elastos::Utility::CDate;
using Elastos::Utility::IRandom;
using Elastos::Droid::KeyStore::Security::IKeyPairGeneratorSpec;
using Elastos::Security::IKeyPairGenerator;
using Elastos::Security::KeyPairGenerator;
using Elastos::Security::IKeyPair;

using Org::Conscrypt::IOpenSSLX509V3CertificateGeneratorHelper;
using Org::Conscrypt::COpenSSLX509V3CertificateGeneratorHelper;

namespace Elastos {
namespace DevSamples {
namespace SecurityDemo {

static const String TAG("SecurityDemo::CActivityOne");

//=======================================================================
// MyListener
//=======================================================================

CAR_INTERFACE_IMPL(CActivityOne::MyListener, Object, IViewOnClickListener)

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 id;
    v->GetId(&id);

    Logger::I(TAG, "SecurityDemo::MyListener::OnClick on viewID:%d", id);

    if (id == R::id::Button2) {
        return mHost->Button2Function();
    }
    else if (id == R::id::Button3) {
        return mHost->Button3Function();
    }
    else if (id == R::id::Button4) {
        return mHost->Button4Function();
    }
    else if (id == R::id::Button5) {
        return mHost->Button5Function();
    }
    else if (id == R::id::Button6) {
        return mHost->Button6Function();
    }
    else if (id == R::id::Button7) {
        return mHost->Button7Function();
    }
    else if (id == R::id::Button12) {
        return mHost->ButtonCSecureRandom();
    }
    else if (id == R::id::Button13) {
        return mHost->ButtonElastosKeyPairGenerator();
    }
    else if (id == R::id::Button14) {
        return mHost->ButtonElastosKeyStore();
    }
    else if (id == R::id::signaturetest) {
        return mHost->SignatureTest();
    }
    else if (id == R::id::ciphertest) {
        return mHost->CipherTest();
    }
    else if (id == R::id::selfsigned) {
        return mHost->SelfSignTest();
    }
    else if (id == R::id::elastoskeypair) {
        return mHost->ElastosKeyPairZLL();
    }
    else {
        return mHost->KeyStoreTest();
    }

    String data = String("testtesttest");
    AutoPtr<ArrayOf<Byte> > signatureData;

    String jksFile = String("doc/keytool/kortide.jks");
    String storepassword = String("elastos123");
    String privatekeypassword = String("elastos");
    String alias = String("kortide");

    Logger::I(TAG, String("jksFile:") + jksFile);
    Logger::I(TAG, String("storepassword:") + storepassword);
    Logger::I(TAG, String("privatekeypassword:") + privatekeypassword);
    Logger::I(TAG, String("alias:") + alias);
    Logger::I(TAG, String("data:") + data);

    //==============================
    AutoPtr<IKeyPairGeneratorHelper> kpgHelper;
    CKeyPairGeneratorHelper::AcquireSingleton((IKeyPairGeneratorHelper**)&kpgHelper);
    AutoPtr<IKeyPairGenerator> kpg;
    kpgHelper->GetInstance(String("RSA"), String("ElastosKeyStore"), (IKeyPairGenerator**)&kpg);

    AutoPtr<IContext> ctx;
    mHost->GetApplicationContext((IContext**)&ctx);
    AutoPtr<ICalendarHelper> calHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calHelper);
    AutoPtr<ICalendar> cal;
    calHelper->GetInstance((ICalendar**)&cal);
    AutoPtr<IDate> now;
    cal->GetTime((IDate**)&now);
    AutoPtr<IDate> end;
    cal->Add(ICalendar::YEAR, 1);
    cal->GetTime((IDate**)&end);
    AutoPtr<IBigIntegerHelper> biHelper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&biHelper);
    AutoPtr<IBigInteger> bi;
    biHelper->ValueOf(1, (IBigInteger**)&bi);
    AutoPtr<IX500Principal> x500p;
    CX500Principal::New(String("CN=test1"), (IX500Principal**)&x500p);

    AutoPtr<IKeyPairGeneratorSpecBuilder> kpgBuilder;
    CKeyPairGeneratorSpecBuilder::New(ctx, (IKeyPairGeneratorSpecBuilder**)&kpgBuilder);
    kpgBuilder->SetAlias(alias);
    kpgBuilder->SetStartDate(now);
    kpgBuilder->SetEndDate(end);
    kpgBuilder->SetSerialNumber(bi);
    kpgBuilder->SetSubject(x500p);
    AutoPtr<IKeyPairGeneratorSpec> spec;
    kpgBuilder->Build((IKeyPairGeneratorSpec**)&spec);

    kpg->Initialize(IAlgorithmParameterSpec::Probe(spec));
    AutoPtr<IKeyPair> kp;
    kpg->GenerateKeyPair((IKeyPair**)&kp);
    //==============================

    // create inputstream for the jks file
    AutoPtr<IInputStream> inputStream;
    // try {
    AutoPtr<IFileInputStream> fileInputStream;
    CFileInputStream::New(jksFile, (IFileInputStream**)&fileInputStream);
    inputStream = IInputStream::Probe(fileInputStream);
    // } catch (FileNotFoundException e) {
    //     e.printStackTrace();
    // }
    // load keystore
    AutoPtr<IKeyStore> keyStore;
    // try {
    AutoPtr<IKeyStoreHelper> ksHelper;
    CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&ksHelper);
    ksHelper->GetInstance(String("ElastosKeyStore"), (IKeyStore**)&keyStore);
    // keyStore->Load(inputStream, storepassword.GetChars());
    keyStore->Load(NULL, NULL);
    // } catch (KeyStoreException e) {
    //     e.printStackTrace();
    // } catch (NoSuchAlgorithmException e) {
    //     e.printStackTrace();
    // } catch (CertificateException e) {
    //     e.printStackTrace();
    // } catch (IOException e) {
    //     e.printStackTrace();
    // }

    //get private from the keystore
    AutoPtr<IPrivateKey> privatekey;
    // try {
    AutoPtr<IKey> key;
    keyStore->GetKey(alias,
            privatekeypassword.GetChars(), (IKey**)&key);
    privatekey = IPrivateKey::Probe(key);
    // } catch (UnrecoverableKeyException | KeyStoreException
    //         | NoSuchAlgorithmException e) {
    //     e.printStackTrace();
    // }
    //try to signature
    AutoPtr<ISignature> signature;
    // try {
    AutoPtr<ISignatureHelper> sHelper;
    CSignatureHelper::AcquireSingleton((ISignatureHelper**)&sHelper);
    sHelper->GetInstance(String("MD5withRSA"), (ISignature**)&signature);
    signature->InitSign(privatekey);
    signature->Update(data.GetBytes());
    signature->Sign((ArrayOf<Byte>**)&signatureData);
    // } catch (NoSuchAlgorithmException | InvalidKeyException | SignatureException e) {
    //     e.printStackTrace();
    // }

    //verify begin
    String jksPubFile = String("doc/keytool/kortide_pub.jks");
    // create inputstream for the jks file
    AutoPtr<IInputStream> inputStreamPub;
    // try {
    AutoPtr<IFileInputStream> fileInputStreamPub;
    CFileInputStream::New(jksPubFile, (IFileInputStream**)&fileInputStreamPub);
    inputStreamPub = IInputStream::Probe(fileInputStreamPub);
    // } catch (FileNotFoundException e) {
    //     e.printStackTrace();
    // }
    // load keystore
    AutoPtr<IKeyStore> keyStorePub;
    // try {
    ksHelper->GetInstance(String("ElastosKeyStore"), (IKeyStore**)&keyStorePub);
    // keyStorePub->Load(inputStreamPub, storepassword.GetChars());
    keyStore->Load(NULL, NULL);
    // } catch (KeyStoreException e) {
    //     e.printStackTrace();
    // } catch (NoSuchAlgorithmException e) {
    //     e.printStackTrace();
    // } catch (CertificateException e) {
    //     e.printStackTrace();
    // } catch (IOException e) {
    //     e.printStackTrace();
    // }
    AutoPtr<IPublicKey> publicKey;
    // try {
    AutoPtr<ICertificate> cert;
    keyStorePub->GetCertificate(alias, (ICertificate**)&cert);
    cert->GetPublicKey((IPublicKey**)&publicKey);

    signature->InitVerify(publicKey);

    AutoPtr<ArrayOf<String> > testData = ArrayOf<String>::Alloc(2);
    (*testData)[0] = data;
    (*testData)[0] = data + "0";

    for (Int32 i = 0; i < testData->GetLength(); i++) {
        String d = (*testData)[i];
        signature->Update(d.GetBytes());
        Boolean result;
        signature->Verify(signatureData, &result);
        if (result) {
            Logger::I(TAG, String("verify success for data:") + d);
        }
        else {
            Logger::I(TAG, String("verify fail for data:") + d);
        }
    }
    // } catch (KeyStoreException e) {
    //     e.printStackTrace();
    // } catch (InvalidKeyException e) {
    //     e.printStackTrace();
    // } catch (SignatureException e) {
    //     e.printStackTrace();
    // }

    return NOERROR;
}

ECode CActivityOne::KeyStoreTest()
{
    Logger::I(TAG, "===== CActivityOne::KeyStoreTest =====");
    String alias = String("kortide");
    String data = String("testtesttest");

    AutoPtr<IContext> ctx;
    GetApplicationContext((IContext**)&ctx);
    AutoPtr<ICalendarHelper> calHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calHelper);
    AutoPtr<ICalendar> cal;
    calHelper->GetInstance((ICalendar**)&cal);
    AutoPtr<IDate> now;
    cal->GetTime((IDate**)&now);
    AutoPtr<IDate> end;
    cal->Add(ICalendar::YEAR, 1);
    cal->GetTime((IDate**)&end);
    AutoPtr<IBigIntegerHelper> biHelper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&biHelper);
    AutoPtr<IBigInteger> bi;
    biHelper->ValueOf(1, (IBigInteger**)&bi);
    AutoPtr<IX500Principal> x500p;
    CX500Principal::New(String("CN=test1"), (IX500Principal**)&x500p);

    AutoPtr<IKeyStoreHelper> ksHelper;
    CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&ksHelper);
    AutoPtr<IKeyStore> keyStore;
    ksHelper->GetInstance(String("ElastosKeyStore"), (IKeyStore**)&keyStore);
    keyStore->Load(NULL, NULL);

    AutoPtr<IKeyStoreEntry> entry;
    keyStore->GetEntry(alias, NULL, (IKeyStoreEntry**)&entry);
    Logger::D(TAG, "===== entry = %p =====", entry.Get());
    AutoPtr<IKeyStorePrivateKeyEntry> pke = IKeyStorePrivateKeyEntry::Probe(entry);
    if (pke == NULL) {
        Logger::E(TAG, "===== Not an instance of a PrivateKeyEntry =====");
        return NOERROR;
    }

    //try to signature
    AutoPtr<ISignatureHelper> sHelper;
    CSignatureHelper::AcquireSingleton((ISignatureHelper**)&sHelper);
    AutoPtr<ISignature> signature;
    sHelper->GetInstance(String("MD5withRSA"), (ISignature**)&signature);
    AutoPtr<IPrivateKey> privatekey;
    pke->GetPrivateKey((IPrivateKey**)&privatekey);
    signature->InitSign(privatekey);
    signature->Update(data.GetBytes());
    AutoPtr<ArrayOf<Byte> > signatureData;
    signature->Sign((ArrayOf<Byte>**)&signatureData);
    AutoPtr<IBase64> base64;
    CBase64::AcquireSingleton((IBase64**)&base64);
    String base64Data;
    base64->EncodeToString(signatureData, IBase64::DEFAULT, &base64Data);
    Logger::I(TAG, "===== CActivityOne::KeyStoreTest, base64Data = %s =====", base64Data.string());

    //Verify
    AutoPtr<ArrayOf<Byte> > dataBytes = data.GetBytes();
    AutoPtr<ArrayOf<Byte> > signatureBytes;
    // try {
    base64->Decode(base64Data, IBase64::DEFAULT, (ArrayOf<Byte>**)&signatureBytes);
    // } catch (IllegalArgumentException e) {
    //     signature = new byte[0];
    // }

    keyStore = NULL;
    ksHelper->GetInstance(String("ElastosKeyStore"), (IKeyStore**)&keyStore);
    keyStore->Load(NULL, NULL);
    entry = NULL;
    keyStore->GetEntry(alias, NULL, (IKeyStoreEntry**)&entry);
    AutoPtr<IKeyStorePrivateKeyEntry> privateKeyEntry = IKeyStorePrivateKeyEntry::Probe(entry);
    if (privateKeyEntry == NULL) {
        Logger::E(TAG, "===== Not an instance of a PrivateKeyEntry =====");
        return NOERROR;
    }

    AutoPtr<ISignature> s;
    sHelper->GetInstance(String("MD5withRSA"), (ISignature**)&s);
    AutoPtr<ICertificate> cert;
    privateKeyEntry->GetCertificate((ICertificate**)&cert);
    s->InitVerify(cert);
    s->Update(dataBytes);
    Boolean b;
    s->Verify(signatureBytes, &b);
    Logger::I(TAG, "===== CActivityOne::KeyStoreTest, Verify = %d =====", b);
    return NOERROR;
}

//=======================================================================
// CActivityOne
//=======================================================================

CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    Logger::I(TAG, " >> constructor()");
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> OnCreate()");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<IView> view = FindViewById(R::id::Button1);
    AutoPtr<MyListener> l = new MyListener(this);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::Button2);
    view->SetOnClickListener(l.Get());

    view = NULL;
    view = FindViewById(R::id::Button3);
    view->SetOnClickListener(l.Get());

    view = NULL;
    view = FindViewById(R::id::Button4);
    view->SetOnClickListener(l.Get());

    view = NULL;
    view = FindViewById(R::id::Button5);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::Button6);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::Button7);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::Button12);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::Button13);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::Button14);
    view->SetOnClickListener(l.Get());

    view = NULL;
    view = FindViewById(R::id::signaturetest);
    view->SetOnClickListener(l.Get());

    view = NULL;
    view = FindViewById(R::id::ciphertest);
    view->SetOnClickListener(l.Get());

    view = NULL;
    view = FindViewById(R::id::selfsigned);
    view->SetOnClickListener(l.Get());

    view = NULL;
    view = FindViewById(R::id::elastoskeypair);
    view->SetOnClickListener(l.Get());

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Logger::I(TAG, " >> OnStart()");
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    Logger::I(TAG, " >> OnResume()");
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    Logger::I(TAG, " >> OnPause()");
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    Logger::I(TAG, " >> OnStop()");
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    Logger::I(TAG, " >> OnDestroy()");
    return Activity::OnDestroy();
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Logger::I(TAG, " >> OnActivityResult()");
    return Activity::OnActivityResult(requestCode, resultCode, data);
}

ECode CActivityOne::Button2Function()
{
    Logger::E(TAG, "leliang begin Button2Function");
    /*
    //test keypairgenerator
    AutoPtr<IKeyPairGeneratorSpecBuilder> kpgSpecBuilder;
    CKeyPairGeneratorSpecBuilder::New(IContext::Probe(this), (IKeyPairGeneratorSpecBuilder**)&kpgSpecBuilder);
    // You'll use the alias later to retrieve the key.  It's a key for the key!
    kpgSpecBuilder->SetAlias(String("leliang"));
    AutoPtr<IX500Principal> x500Principal;
    CX500Principal::New(String("CN=leliang"), (IX500Principal**)&x500Principal);
    // The subject used for the self-signed certificate of the generated pair
    kpgSpecBuilder->SetSubject(x500Principal);
    AutoPtr<IBigIntegerHelper> biHelper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&biHelper);
    AutoPtr<IBigInteger> bigInt;
    biHelper->ValueOf(1337, (IBigInteger**)&bigInt);
    // The serial number used for the self-signed certificate of the
    // generated pair.
    kpgSpecBuilder->SetSerialNumber(bigInt);
    // Date range of validity for the generated pair.
    AutoPtr<IDate> startDate;
    CDate::New(116, 0, 1, (IDate**)&startDate);
    kpgSpecBuilder->SetStartDate(startDate);
    AutoPtr<IDate> endDate;
    CDate::New(126, 0, 1, (IDate**)&endDate);
    kpgSpecBuilder->SetEndDate(endDate);
    AutoPtr<IKeyPairGeneratorSpec> spec;
    kpgSpecBuilder->Build((IKeyPairGeneratorSpec**)&spec);
    // END_INCLUDE(create_spec)

    // BEGIN_INCLUDE(create_keypair)
    // Initialize a KeyPair generator using the the intended algorithm (in this example, RSA
    // and the KeyStore.  This example uses the AndroidKeyStore.
    AutoPtr<IKeyPairGenerator> kpGenerator;
    KeyPairGenerator::GetInstance(String("RSA")//, String("AndroidKeyStore")
                                    , (IKeyPairGenerator**)&kpGenerator);
    kpGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec));
    AutoPtr<IKeyPair> kp;
    kpGenerator->GenerateKeyPair((IKeyPair**)&kp);
    */
    // test mac
    // Generate secret key for HMAC-MD5
    AutoPtr<IKeyGeneratorHelper> kgHelper;
    CKeyGeneratorHelper::AcquireSingleton((IKeyGeneratorHelper**)&kgHelper);
    AutoPtr<IKeyGenerator> kg;
    kgHelper->GetInstance(String("HMACMD5"), (IKeyGenerator**)&kg);
    AutoPtr<ISecretKey> sk;
    kg->GenerateKey((ISecretKey**)&sk);
    Logger::E(TAG, "leliang Button2Function, secretkey:%p", sk.Get());

    // Get instance of Mac object implementing HMAC-MD5, and
    // initialize it with the above secret key
    AutoPtr<IMacHelper> macHelper;
    CMacHelper::AcquireSingleton((IMacHelper**)&macHelper);
    AutoPtr<IMac> mac;
    //Logger::E(TAG, "leliang Button2Function, before macHelper->GetInstance");
    //test history:
    String algorithm("HmacSHA512");
    // HMACMD5      ok
    // HmacSHA224   ok
    // HmacSHA256   ok
    // HmacSHA384   ok
    // HmacSHA512   ok
    macHelper->GetInstance(algorithm, (IMac**)&mac);
    //Logger::E(TAG, "leliang Button2Function, after macHelper->GetInstance");
    //Logger::E(TAG, "leliang Button2Function, before mac->Init");
    mac->Init(IKey::Probe(sk));
    //Logger::E(TAG, "leliang Button2Function, after mac->Init");
    String test("Hi There");
    AutoPtr<ArrayOf<Byte> > input = test.GetBytes();
    AutoPtr<ArrayOf<Byte> > result;
    //Logger::E(TAG, "leliang Button2Function, before mac->DoFinal");
    mac->DoFinal(input, (ArrayOf<Byte>**)&result);
    //Logger::E(TAG, "leliang Button2Function, after mac->DoFinal");
    Logger::E(TAG, "leliang Button2Function, algorithm:%s, test:%s", algorithm.string(), test.string());
    if (result != NULL) {
        for (Int32 i = 0; i < result->GetLength(); ++i) {
            Logger::E(TAG, "result[%d]: 0x%x", i, (*result)[i]);
        }
    } else {
        Logger::E(TAG, "leliang result is NULL");
    }

    test = String("hi there");
    input = test.GetBytes();
    mac->Init(IKey::Probe(sk));
    result = NULL;
    mac->Update(input);
    mac->DoFinal((ArrayOf<Byte>**)&result);
    if (result != NULL) {
        for (Int32 i = 0; i < result->GetLength(); ++i) {
            Logger::E(TAG, "result[%d]: 0x%x", i, (*result)[i]);
        }
    } else {
        Logger::E(TAG, "leliang result is NULL");
    }

    Logger::E(TAG, "leliang end Button2Function");
    return NOERROR;
}

ECode CActivityOne::Button3Function()   //MessageDigest
{
    Logger::D(TAG, "begin MessageDigest");
    Services::Initialize();

    AutoPtr<IMessageDigestHelper> mdHelper;
    CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&mdHelper);
    Logger::D(TAG, "begin GetInstance");
    AutoPtr<ArrayOf<String> > allAlgorithms = ArrayOf<String>::Alloc(6);
    allAlgorithms->Set(0, String("SHA-1"));
    allAlgorithms->Set(1, String("SHA-224"));
    allAlgorithms->Set(2, String("SHA-256"));
    allAlgorithms->Set(3, String("SHA-384"));
    allAlgorithms->Set(4, String("SHA-512"));
    allAlgorithms->Set(5, String("MD5"));
    for (Int32 i = 0; i < allAlgorithms->GetLength(); ++i) {
        String algorithm = (*allAlgorithms)[i];
        AutoPtr<IMessageDigest> md;
        mdHelper->GetInstance(algorithm, (IMessageDigest**)&md);

        String text("abc");
        //SHA-1: A9:99:3E:36:47:06:81:6A:BA:3E:25:71:78:50:C2:6C:9C:D0:D8:9D
        AutoPtr<ArrayOf<Byte> > data = text.GetBytes();
        md->Update(data);
        AutoPtr<ArrayOf<Byte> > resultData;
        md->Digest((ArrayOf<Byte>**)&resultData);
        //for (Int32 i = 0; i < resultData->GetLength(); i++) {
        //    Logger::D(TAG, "resultData %d is : %x", i, (*resultData)[i]);
        //}
        String resultStr = StringUtils::ToHexString(*resultData);
        Logger::D(TAG, "result string for al:%s, is : %s", algorithm.string(), resultStr.string());
    }
    Logger::D(TAG, "=====end MessageDigest===== ");
    return NOERROR;
}

ECode CActivityOne::Button4Function()   //KeyPairGenerator
{
    Logger::D(TAG, "begin Button4Function");
    Services::Initialize();

    AutoPtr<IKeyPairGeneratorHelper> kpgHelper;
    CKeyPairGeneratorHelper::AcquireSingleton((IKeyPairGeneratorHelper**)&kpgHelper);
    AutoPtr<IKeyPairGenerator> keyPairGenerator;
    Logger::D(TAG, "begin GetInstance");
    kpgHelper->GetInstance(String("RSA"), String("ElastosOpenSSL"), (IKeyPairGenerator**)&keyPairGenerator);

    keyPairGenerator->Initialize(1024);

    AutoPtr<IKeyPair> keyPair;
    keyPairGenerator->GenerateKeyPair((IKeyPair**)&keyPair);

    AutoPtr<IPublicKey> publicKey;
    keyPair->GetPublic((IPublicKey**)&publicKey);

    AutoPtr<IPrivateKey> privateKey;
    keyPair->GetPrivate((IPrivateKey**)&privateKey);

    AutoPtr<ArrayOf<Byte> > publicEncoded;
    IKey::Probe(publicKey)->GetEncoded((ArrayOf<Byte>**)&publicEncoded);
    String publicStr = StringUtils::ToHexString(*publicEncoded);
    mRSAPublicEncoded = publicEncoded;

    AutoPtr<ArrayOf<Byte> > privateEncoded;
    IKey::Probe(privateKey)->GetEncoded((ArrayOf<Byte>**)&privateEncoded);
    String privateStr = StringUtils::ToHexString(*privateEncoded);
    mRSAPrivateEncoded = privateEncoded;

    Logger::D(TAG, "RSA publicKey string is : %s, \nprivateKey string is : %s", publicStr.string(), privateStr.string());
    AutoPtr<ArrayOf<String> > allAlgorithms = ArrayOf<String>::Alloc(3);
    allAlgorithms->Set(0, String("DH"));
    allAlgorithms->Set(1, String("DSA"));
    allAlgorithms->Set(2, String("EC"));
    for (Int32 i = 0; i < allAlgorithms->GetLength(); ++i) {
        String algorithm = (*allAlgorithms)[i];
        AutoPtr<IKeyPairGenerator> keyPairGenerator;
        kpgHelper->GetInstance(algorithm, String("ElastosOpenSSL"), (IKeyPairGenerator**)&keyPairGenerator);

        AutoPtr<IKeyPair> keyPair;
        keyPairGenerator->GenerateKeyPair((IKeyPair**)&keyPair);

        AutoPtr<IPublicKey> publicKey;
        keyPair->GetPublic((IPublicKey**)&publicKey);

        AutoPtr<IPrivateKey> privateKey;
        keyPair->GetPrivate((IPrivateKey**)&privateKey);

        Logger::E("leliang", "publicKey:%p, privateKey:%p", publicKey.Get(), privateKey.Get());
        AutoPtr<ArrayOf<Byte> > publicEncoded;
        IKey::Probe(publicKey)->GetEncoded((ArrayOf<Byte>**)&publicEncoded);
        String publicStr = StringUtils::ToHexString(*publicEncoded);

        AutoPtr<ArrayOf<Byte> > privateEncoded;
        IKey::Probe(privateKey)->GetEncoded((ArrayOf<Byte>**)&privateEncoded);
        String privateStr = StringUtils::ToHexString(*privateEncoded);
        if (algorithm.Equals("DH")) {
            mDHPublicEncoded = publicEncoded;
            mDHPrivateEncoded = privateEncoded;
        }

        Logger::D(TAG, "%s publicKey string is : %s, \nprivateKey string is : %s", algorithm.string(), publicStr.string(), privateStr.string());
    }

    Logger::D(TAG, "=====end Button4Function=====");
    return NOERROR;
}

ECode CActivityOne::Button5Function()   //KeyFactory
{
    Logger::D(TAG, "begin Button5Function");
    Services::Initialize();

    AutoPtr<IKeyFactoryHelper> kfHelper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&kfHelper);
    AutoPtr<IKeyFactory> keyFactory;
    Logger::D(TAG, "begin GetInstance");
    kfHelper->GetInstance(String("RSA"), (IKeyFactory**)&keyFactory);

    if (mRSAPublicEncoded == NULL || mRSAPrivateEncoded == NULL) {
        Logger::D(TAG, "\n=====Please run KeyPairGenerator first=====\n");
        return NOERROR;
    }
    AutoPtr<IX509EncodedKeySpec> x509Spec;
    CX509EncodedKeySpec::New(mRSAPublicEncoded, (IX509EncodedKeySpec**)&x509Spec);
    AutoPtr<IPublicKey> publicKey;
    keyFactory->GeneratePublic(IKeySpec::Probe(x509Spec), (IPublicKey**)&publicKey);
    assert("RSA publicKey should not be NULL" && publicKey != NULL);

    AutoPtr<IPKCS8EncodedKeySpec> pkcs8Spec;
    CPKCS8EncodedKeySpec::New(mRSAPrivateEncoded, (IPKCS8EncodedKeySpec**)&pkcs8Spec);
    AutoPtr<IPrivateKey> privateKey;
    keyFactory->GeneratePrivate(IKeySpec::Probe(pkcs8Spec), (IPrivateKey**)&privateKey);
    assert("RSA privateKey should not be NULL" && privateKey != NULL);
    Logger::D(TAG, "\nafter test KeyFactory for RSA");

    kfHelper->GetInstance(String("DH"), (IKeyFactory**)&keyFactory);

    if (mDHPublicEncoded == NULL || mDHPrivateEncoded == NULL) {
        Logger::D(TAG, "\n=====Please run KeyPairGenerator first=====\n");
        return NOERROR;
    }
    AutoPtr<IX509EncodedKeySpec> x509SpecDH;
    CX509EncodedKeySpec::New(mDHPublicEncoded, (IX509EncodedKeySpec**)&x509SpecDH);
    AutoPtr<IPublicKey> publicKeyDH;
    keyFactory->GeneratePublic(IKeySpec::Probe(x509SpecDH), (IPublicKey**)&publicKeyDH);
    assert("DH publicKey should not be NULL" && publicKeyDH != NULL);

    AutoPtr<IPKCS8EncodedKeySpec> pkcs8SpecDH;
    CPKCS8EncodedKeySpec::New(mDHPrivateEncoded, (IPKCS8EncodedKeySpec**)&pkcs8SpecDH);
    AutoPtr<IPrivateKey> privateKeyDH;
    keyFactory->GeneratePrivate(IKeySpec::Probe(pkcs8SpecDH), (IPrivateKey**)&privateKeyDH);
    assert("DH privateKey should not be NULL" && privateKeyDH != NULL);
    Logger::D(TAG, "\nafter test KeyFactory for DH");

    Logger::D(TAG, "=====end Button5Function=====");
    return NOERROR;
}

ECode CActivityOne::Button6Function()// KeyAgreement
{
    Logger::E(TAG, "leliang begin Button6Function ");
    //Services::Initialize();
    AutoPtr<IKeyAgreementHelper> kaHelper;
    CKeyAgreementHelper::AcquireSingleton((IKeyAgreementHelper**)&kaHelper);
    AutoPtr<IKeyAgreement> ka;
    kaHelper->GetInstance(String("ECDH"), (IKeyAgreement**)&ka);
    Logger::E(TAG, "leliang after get IKeyAgreement ka:%p", ka.Get());

    AutoPtr<IKeyFactoryHelper> kfHelper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&kfHelper);
    AutoPtr<IKeyFactory> kf;
    kfHelper->GetInstance(String("EC"), (IKeyFactory**)&kf);

    /*
    AutoPtr<ArrayOf<Byte> > der_public = ArrayOf<Byte>::Alloc(162);
    der_public->Set(0, 0x30); der_public->Set(1, 0x81); der_public->Set(2, 0x9f); der_public->Set(3, 0x30);
    der_public->Set(4, 0xd); der_public->Set(5, 0x6); der_public->Set(6, 0x9); der_public->Set(7, 0x2a);
    der_public->Set(8, 0x86); der_public->Set(9, 0x48); der_public->Set(10, 0x86); der_public->Set(11, 0xf7);
    der_public->Set(12, 0xd); der_public->Set(13, 0x1); der_public->Set(14, 0x1); der_public->Set(15, 0x1);
    der_public->Set(16, 0x5); der_public->Set(17, 0x0); der_public->Set(18, 0x3); der_public->Set(19, 0x81);
    der_public->Set(20, 0x8d); der_public->Set(21, 0x0); der_public->Set(22, 0x30); der_public->Set(23, 0x81);
    der_public->Set(24, 0x89); der_public->Set(25, 0x2); der_public->Set(26, 0x81); der_public->Set(27, 0x81);
    der_public->Set(28, 0x0); der_public->Set(29, 0xe9); der_public->Set(30, 0x9b); der_public->Set(31, 0x82);
    der_public->Set(32, 0x9a); der_public->Set(33, 0x9b); der_public->Set(34, 0x1b); der_public->Set(35, 0xa5);
    der_public->Set(36, 0x26); der_public->Set(37, 0x7c); der_public->Set(38, 0x9c); der_public->Set(39, 0x13);
    der_public->Set(40, 0x33); der_public->Set(41, 0x4e); der_public->Set(42, 0x34); der_public->Set(43, 0x63);
    der_public->Set(44, 0x3b); der_public->Set(45, 0xa7); der_public->Set(46, 0x40); der_public->Set(47, 0xaf);
    der_public->Set(48, 0x1); der_public->Set(49, 0xaa); der_public->Set(50, 0x19); der_public->Set(51, 0xc4);
    der_public->Set(52, 0x91); der_public->Set(53, 0x1d); der_public->Set(54, 0xe4); der_public->Set(55, 0x3b);
    der_public->Set(56, 0xe2); der_public->Set(57, 0xa3); der_public->Set(58, 0x96); der_public->Set(59, 0xfa);
    der_public->Set(60, 0x5); der_public->Set(61, 0x13); der_public->Set(62, 0x9d); der_public->Set(63, 0xa);
    der_public->Set(64, 0xd4); der_public->Set(65, 0x4d); der_public->Set(66, 0x94); der_public->Set(67, 0x4b);
    der_public->Set(68, 0x1); der_public->Set(69, 0xfb); der_public->Set(70, 0xdb); der_public->Set(71, 0xed);
    der_public->Set(72, 0x7); der_public->Set(73, 0x9b); der_public->Set(74, 0x61); der_public->Set(75, 0x93);
    der_public->Set(76, 0x20); der_public->Set(77, 0xd5); der_public->Set(78, 0xb); der_public->Set(79, 0xb3);
    der_public->Set(80, 0x7e); der_public->Set(81, 0x21); der_public->Set(82, 0x6d); der_public->Set(83, 0x0);
    der_public->Set(84, 0x8e); der_public->Set(85, 0xdc); der_public->Set(86, 0x83); der_public->Set(87, 0x13);
    der_public->Set(88, 0xa8); der_public->Set(89, 0x76); der_public->Set(90, 0x40); der_public->Set(91, 0xff);
    der_public->Set(92, 0x80); der_public->Set(93, 0x59); der_public->Set(94, 0xaf); der_public->Set(95, 0x9c);
    der_public->Set(96, 0xe1); der_public->Set(97, 0xd8); der_public->Set(98, 0x74); der_public->Set(99, 0x44);
    der_public->Set(100, 0xcd); der_public->Set(101, 0xae); der_public->Set(102, 0x35); der_public->Set(103, 0x55);
    der_public->Set(104, 0xe4); der_public->Set(105, 0x51); der_public->Set(106, 0x63); der_public->Set(107, 0xeb);
    der_public->Set(108, 0x3); der_public->Set(109, 0xc0); der_public->Set(110, 0xbe); der_public->Set(111, 0xf8);
    der_public->Set(112, 0x61); der_public->Set(113, 0xef); der_public->Set(114, 0x10); der_public->Set(115, 0x39);
    der_public->Set(116, 0x71); der_public->Set(117, 0x67); der_public->Set(118, 0x34); der_public->Set(119, 0x46);
    der_public->Set(120, 0x89); der_public->Set(121, 0x1); der_public->Set(122, 0xc); der_public->Set(123, 0xba);
    der_public->Set(124, 0x2a); der_public->Set(125, 0x69); der_public->Set(126, 0x61); der_public->Set(127, 0xf2);
    der_public->Set(128, 0x32); der_public->Set(129, 0x5); der_public->Set(130, 0x59); der_public->Set(131, 0xe);
    der_public->Set(132, 0xfb); der_public->Set(133, 0x8f); der_public->Set(134, 0x86); der_public->Set(135, 0x46);
    der_public->Set(136, 0xf9); der_public->Set(137, 0xe4); der_public->Set(138, 0x1b); der_public->Set(139, 0x87);
    der_public->Set(140, 0x4a); der_public->Set(141, 0x8f); der_public->Set(142, 0x3c); der_public->Set(143, 0x6b);
    der_public->Set(144, 0x41); der_public->Set(145, 0x83); der_public->Set(146, 0xea); der_public->Set(147, 0x29);
    der_public->Set(148, 0x42); der_public->Set(149, 0xb7); der_public->Set(150, 0xb); der_public->Set(151, 0x83);
    der_public->Set(152, 0xe0); der_public->Set(153, 0x15); der_public->Set(154, 0x6d); der_public->Set(155, 0x85);
    der_public->Set(156, 0x7b); der_public->Set(157, 0x2); der_public->Set(158, 0x3); der_public->Set(159, 0x1);
    der_public->Set(160, 0x0); der_public->Set(161, 0x1);
    */
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
    Logger::E(TAG, "leliang x509EncodedKeySpec: %p", x509EncodedKeySpec.Get());
    IKeySpec* keySpec = IKeySpec::Probe(x509EncodedKeySpec);
    Logger::E(TAG, "leliang keySpec: %p", keySpec);

    AutoPtr<IPublicKey> pubKey;
    kf->GeneratePublic(keySpec, (IPublicKey**)&pubKey);
    Logger::E(TAG, "leliang pubKey: %p", pubKey.Get());

    AutoPtr<IBigIntegerHelper> biHelper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&biHelper);
    /*
    AutoPtr<IRSAPrivateKeySpec> rsaPrivateKeySpec;
    AutoPtr<IBigInteger> mod;
    biHelper->ValueOf(10000, (IBigInteger**)&mod);
    AutoPtr<IBigInteger> priExp;
    biHelper->ValueOf(20000, (IBigInteger**)&priExp);
    CRSAPrivateKeySpec::New(mod, priExp, (IRSAPrivateKeySpec**)&rsaPrivateKeySpec);
    Logger::E(TAG, "leliang rsaPrivateKeySpec: %p", rsaPrivateKeySpec.Get());
    */
    /*
    AutoPtr<IECPrivateKeySpec> ecPrivatekeySpec;
    AutoPtr<IBigInteger> bi;
    biHelper->ValueOf(10000, (IBigInteger**)&bi);
    AutoPtr<IECParameterSpec> params;
    AutoPtr<IEllipticCurve> ec;
    AutoPtr<IECField> ecField;
    AutoPtr<IBigInteger> p;
    biHelper->ValueOf(10001, (IBigInteger**)&p);
    CECFieldFp::New(p, (IECField**)&ecField);
    AutoPtr<IBigInteger> a;
    biHelper->ValueOf(20000, (IBigInteger**)&a);
    AutoPtr<IBigInteger> b;
    biHelper->ValueOf(30000, (IBigInteger**)&b);
    AutoPtr<ArrayOf<Byte> > seed = ArrayOf<Byte>::Alloc(5);
    seed->Set(0, 0x10); seed->Set(1, 0x20); seed->Set(2, 0x30); seed->Set(3, 0x40); seed->Set(4, 0x50);
    CEllipticCurve::New(ecField, a, b, seed, (IEllipticCurve**)&ec);
    AutoPtr<IECPoint> ecPoint;
    AutoPtr<IBigInteger> affineX;
    biHelper->ValueOf(40000, (IBigInteger**)&affineX);
    AutoPtr<IBigInteger> affineY;
    biHelper->ValueOf(50000, (IBigInteger**)&affineY);
    CECPoint::New(affineX, affineY, (IECPoint**)&ecPoint);
    AutoPtr<IBigInteger> order;
    biHelper->ValueOf(40000, (IBigInteger**)&order);
    Int32 cofactor = 100;
    CECParameterSpec::New(ec, ecPoint, order, cofactor, (IECParameterSpec**)&params);
    CECPrivateKeySpec::New(bi, params, (IECPrivateKeySpec**)&ecPrivatekeySpec);
    */

    AutoPtr<IPKCS8EncodedKeySpec> ecPrivatekeySpec;
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

    CPKCS8EncodedKeySpec::New(ec_private, (IPKCS8EncodedKeySpec**)&ecPrivatekeySpec);
    AutoPtr<IPrivateKey> priKey;
    kf->GeneratePrivate(IKeySpec::Probe(ecPrivatekeySpec), (IPrivateKey**)&priKey);
    Logger::E(TAG, "leliang priKey: %p", priKey.Get());

    AutoPtr<IKey> tmpResult;
    ka->Init(IKey::Probe(priKey));
    ka->DoPhase(IKey::Probe(pubKey), TRUE, (IKey**)&tmpResult);

    AutoPtr<ArrayOf<Byte> > secret;
    ka->GenerateSecret((ArrayOf<Byte>**)&secret);

    Logger::E(TAG, "leliang secret begin");
    if (secret != NULL) {
        for (Int32 i = 0; i < secret->GetLength(); i++) {
            Logger::E(TAG, "secret[%d]=%d", i, (*secret)[i]);
        }
    }
    else {
        Logger::E(TAG, "leliang secret is NULL");
    }
    Logger::E(TAG, "leliang secret end");
    Logger::E(TAG, "leliang end Button6Function ");
    return NOERROR;
}

ECode CActivityOne::Button7Function()// Certificate
{
    Logger::E(TAG, "leliang begin Button7Function");
    //Services::Initialize();
    AutoPtr<ICertificateFactoryHelper> cfHelper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&cfHelper);
    AutoPtr<ICertificateFactory> cf;
    cfHelper->GetInstance(String("X509"), (ICertificateFactory**)&cf);
    Logger::E(TAG, "leliang after get ICertificateFactory cf:%p", cf.Get());
    //AutoPtr<IFileInputStream> fis;
    //CFileInputStream::New(String("/data/security/test.crt"), (IFileInputStream**)&fis);
    //Logger::E(TAG, "leliang after create CFileInputStream, fis:%p", fis.Get());
    //if (fis == NULL) {
    //    Logger::E(TAG, "leliang /data/security/test.crt is not found");
    //    return NOERROR;
    //}
    String certStr("-----BEGIN CERTIFICATE-----\n\
MIIDTjCCAjagAwIBAgIBATANBgkqhkiG9w0BAQsFADB5MQswCQYDVQQGEwJDTjEL\n\
MAkGA1UECAwCU0gxCzAJBgNVBAcMAlNIMQswCQYDVQQKDAJPUzELMAkGA1UECwwC\n\
T1MxEDAOBgNVBAMMB2VsYXN0b3MxJDAiBgkqhkiG9w0BCQEWFXpoYW5nLmxlbGlh\n\
bmdAMTYzLmNvbTAeFw0xNjEwMjUwMzE0MDBaFw0xNzEwMjUwMzE0MDBaMGMxCzAJ\n\
BgNVBAYTAkNOMQswCQYDVQQIDAJTSDELMAkGA1UECwwCT1MxEDAOBgNVBAMMB2Vs\n\
YXN0b3MxKDAmBgkqhkiG9w0BCQEWGXpoYW5nLmxlbGlhbmdAa29ydGlkZS5jb20w\n\
gZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAJ/YepjuVlTa2lvvLqUguq/StGH5\n\
hJoqE9JvLVLfWCd5VsDweQH/1iUJejsQr1ZHD9rSrX0oVbRTXV/2fx+iow/8n/xO\n\
RUCxdhxoxz1oXEWLWugEWRXqt2t//vTiy16h1wtwWwKpKb9d01yZ+3d0QHaWYPpJ\n\
CH+gD3AUvpPjskwFAgMBAAGjezB5MAkGA1UdEwQCMAAwLAYJYIZIAYb4QgENBB8W\n\
HU9wZW5TU0wgR2VuZXJhdGVkIENlcnRpZmljYXRlMB0GA1UdDgQWBBTw6tsyRxFp\n\
3Yev2r+GEqDiCz1hazAfBgNVHSMEGDAWgBTlQcWf03XjZNX7oN74cHMffC15xjAN\n\
BgkqhkiG9w0BAQsFAAOCAQEAHA8t28IV79t+Pl/vs8/Gozmz+bRw4Af7PsTU8mHJ\n\
SXTpKX7pGTaLmkCrGIvzUnIxbFdhi0cYWIIIxsWgBQmxIwOVP1/941K5/y7WdijN\n\
t1mU9NhXrxa77BwJNUH6+JrILUkpWFmrN2n4tWm7sQe4F5Tno68XqL6CvgNrkHft\n\
dcHFRXtSMQSUvrItZcZlT5KHRHqD42Rahq15pcuY1uQTOmJK0sqAG5VEGVgLCs6w\n\
+5UY4DnYz5YBpqodYy8O/kMksl96q75zOaqWo2mfDhn5nt3/AbDrwvCYZ4Q1XGTm\n\
Oij8xXDCkImcYXmmKDUOt5VNwLZL3vvqyz3AJm70sPo1bA==\n\
-----END CERTIFICATE-----");
    AutoPtr<IStringBufferInputStream> sbis;
    CStringBufferInputStream::New(certStr, (IStringBufferInputStream**)&sbis);
    Logger::E(TAG, "leliang after create CFileInputStream, sbis:%p", sbis.Get());

    AutoPtr<ICertificate> certificate;
    cf->GenerateCertificate(IInputStream::Probe(sbis), (ICertificate**)&certificate);
    Logger::E(TAG, "leliang after get ICertificate certificate:%p", certificate.Get());
    assert("certificate should not be NULL" && certificate != NULL);

    String type;
    certificate->GetType(&type);
    Logger::E(TAG, "certificate type:%s", type.string());
    IX509Certificate* x509Certificate = IX509Certificate::Probe(certificate);
    Logger::E(TAG, "leliang after get IX509Certificate:%p", x509Certificate);
    Int32 version;
    x509Certificate->GetVersion(&version);
    Logger::E(TAG, "x509 version:%d", version);
    AutoPtr<IBigInteger> sn;
    x509Certificate->GetSerialNumber((IBigInteger**)&sn);
    String serialNum;
    sn->ToString(10, &serialNum);
    Logger::E(TAG, "x509 serial number:%s", serialNum.string());
    AutoPtr<IPrincipal> issueDN;
    x509Certificate->GetIssuerDN((IPrincipal**)&issueDN);
    String issueDNName;
    issueDN->GetName(&issueDNName);
    Logger::E(TAG, "x509 issueDN:%s", issueDNName.string());
    AutoPtr<IPrincipal> subjectDN;
    x509Certificate->GetSubjectDN((IPrincipal**)&subjectDN);
    String subjectDNName;
    subjectDN->GetName(&subjectDNName);
    Logger::E(TAG, "x509 subjectDNName:%s", subjectDNName.string());
    String sigAlgName;
    x509Certificate->GetSigAlgName(&sigAlgName);
    Logger::E(TAG, "x509 sigAlgName:%s", sigAlgName.string());
    AutoPtr<IDate> notBefore;
    x509Certificate->GetNotBefore((IDate**)&notBefore);
    String nbString;
    notBefore->ToString(&nbString);
    AutoPtr<IDate> notAfter;
    x509Certificate->GetNotAfter((IDate**)&notAfter);
    String naString;
    notAfter->ToString(&naString);
    Logger::E(TAG, "x509 naString:%s, nbString:%s", naString.string(), nbString.string());



    Logger::E(TAG, "leliang end Button7Function");
    return NOERROR;
}

ECode CActivityOne::ButtonCSecureRandom()
{
    //setprop elastos.class.path /system/lib/Elastos.Droid.Core.eco:/system/lib/Org.Conscrypt.eco
    Logger::E(TAG, "snow begin ButtonCSecureRandom");
    Services::Initialize();

    const String algorithm("SHA1PRNG"); // algorithm's name

    AutoPtr<ISecureRandomHelper> helper;
    CSecureRandomHelper::AcquireSingleton((ISecureRandomHelper**)&helper);

    AutoPtr<ISecureRandom> sr;
    helper->GetInstance(algorithm, (ISecureRandom**)&sr);
    // AutoPtr<ISecureRandom> sr2;
    // helper->GetInstance(algorithm, (ISecureRandom**)&sr2);

    AutoPtr<ArrayOf<Byte> > randomData1 = ArrayOf<Byte>::Alloc(64);
    //AutoPtr<ArrayOf<Byte> > randomData2 = ArrayOf<Byte>::Alloc(64);

    IRandom::Probe(sr)->NextBytes(randomData1);
    //IRandom::Probe(sr2)->NextBytes(randomData2);

    Logger::E(TAG, "randomData1 begin==========");
    for (Int32 i = 0; i < randomData1->GetLength(); i++) {
        Logger::E(TAG, "randomData1[%d]=%d", i, (*randomData1)[i]);
    }
    Logger::E(TAG, "randomData1 end==========");

    // for (Int32 i = 0; i < randomData2->GetLength(); i++) {
    //     Logger::E(TAG, "randomData2[%d]=%d", i, (*randomData2)[i]);
    // }

    return NOERROR;
}

ECode CActivityOne::ButtonElastosKeyPairGenerator()
{
    //setprop elastos.class.path /system/lib/Elastos.Droid.Core.eco:/system/lib/Org.Conscrypt.eco
    //Services::Initialize();

    AutoPtr<ElastosKeyPairGeneratorTest> text = new ElastosKeyPairGeneratorTest(this);

    //return text->TestKeyPairGenerator_Initialize_Params_Encrypted_Success();                                 //OK

    //return text->TestKeyPairGenerator_Initialize_KeySize_Encrypted_Failure();                                //OK

    //return text->TestKeyPairGenerator_Initialize_KeySizeAndSecureRandom_Encrypted_Failure();                 //OK

    //return text->TestKeyPairGenerator_Initialize_ParamsAndSecureRandom_Encrypted_Failure();                  //OK

    //return text->TestKeyPairGenerator_GenerateKeyPair_Encrypted_Success();                                   //OK

    //return text->TestKeyPairGenerator_GenerateKeyPair_DSA_Unencrypted_Success();                             //OK

    //return text->TestKeyPairGenerator_GenerateKeyPair_DSA_2048_Unencrypted_Success();                        //OK

    // return text->TestKeyPairGenerator_GenerateKeyPair_DSA_SpecifiedParams_Unencrypted_Success();            //OK

    //return text->TestKeyPairGenerator_GenerateKeyPair_EC_Unencrypted_Success();                              //OK

    //return text->TestKeyPairGenerator_GenerateKeyPair_EC_P521_Unencrypted_Success();                         //OK

    //return text->TestKeyPairGenerator_GenerateKeyPair_RSA_Unencrypted_Success();                             //OK

    return text->TestKeyPairGenerator_GenerateKeyPair_RSA_WithParams_Unencrypted_Success();                    //Ok

    //return text->TestKeyPairGenerator_GenerateKeyPair_Replaced_Success();                                    //OK

    //return text->TestKeyPairGenerator_GenerateKeyPair_Replaced_UnencryptedToEncrypted_Success();             //OK
}

ECode CActivityOne::ButtonElastosKeyStore()
{
    //setprop elastos.class.path /system/lib/Elastos.Droid.Core.eco:/system/lib/Org.Conscrypt.eco
    //Services::Initialize();

    AutoPtr<ElastosKeyStoreTest> test = new ElastosKeyStoreTest(this);

    //return test->TestKeyStore_Aliases_Encrypted_Success();                                        //OK

    //return test->TestKeyStore_Aliases_NotInitialized_Encrypted_Failure();                         //OK

    //return test->TestKeyStore_ContainsAliases_PrivateAndCA_Encrypted_Success();                   //OK

    //return test->TestKeyStore_ContainsAliases_CAOnly_Encrypted_Success();                         //OK

    //return test->TestKeyStore_ContainsAliases_NonExistent_Encrypted_Failure();                    //OK

    //return test->TestKeyStore_DeleteEntry_Encrypted_Success();                                    //OK

    //return test->TestKeyStore_DeleteEntry_EmptyStore_Encrypted_Success();                         //OK

    //return test->TestKeyStore_DeleteEntry_NonExistent_Encrypted_Success();                        //OK

    //return test->TestKeyStore_GetCertificate_Single_Encrypted_Success();                          //OK

    //return test->TestKeyStore_GetCertificate_NonExist_Encrypted_Failure();                        //OK

    //return test->TestKeyStore_GetCertificateAlias_CAEntry_Encrypted_Success();                    //OK

    //return test->TestKeyStore_GetCertificateAlias_PrivateKeyEntry_Encrypted_Success();            //OK

    //return test->TestKeyStore_GetCertificateAlias_CAEntry_WithPrivateKeyUsingCA_Encrypted_Success(); //OK

    //return test->TestKeyStore_GetCertificateAlias_NonExist_Empty_Encrypted_Failure();             //OK

    //return test->TestKeyStore_GetCertificateAlias_NonExist_Encrypted_Failure();                   //OK

    //return test->TestKeyStore_GetCertificateChain_SingleLength_Encrypted_Success();               //OK

    //return test->TestKeyStore_GetCertificateChain_NonExist_Encrypted_Failure();                   //OK

    //return test->TestKeyStore_GetCreationDate_PrivateKeyEntry_Encrypted_Success();                //OK

    //return test->TestKeyStore_GetCreationDate_PrivateKeyEntry_Unencrypted_Success();              //OK

    //return test->TestKeyStore_GetCreationDate_CAEntry_Encrypted_Success();                        //OK

    //return test->TestKeyStore_GetEntry_NullParams_Encrypted_Success();                            //OK

    //return test->TestKeyStore_GetEntry_DSA_NullParams_Unencrypted_Success();                      //OK

    // return test->TestKeyStore_GetEntry_EC_NullParams_Unencrypted_Success();                      //OK

    //return test->TestKeyStore_GetEntry_RSA_NullParams_Unencrypted_Success();                      //OK

    //return test->TestKeyStore_GetEntry_Nonexistent_NullParams_Encrypted_Failure();                //OK

    //return test->TestKeyStore_GetEntry_Nonexistent_NullParams_Unencrypted_Failure();              //OK

    //return test->TestKeyStore_GetKey_NoPassword_Encrypted_Success();                              //OK

    //return test->TestKeyStore_GetKey_NoPassword_Unencrypted_Success();                            //OK

    //return test->TestKeyStore_GetKey_Certificate_Encrypted_Failure();                             //OK

    //return test->TestKeyStore_GetKey_NonExistent_Encrypted_Failure();                             //OK

    //return test->TestKeyStore_GetProvider_Encrypted_Success();                                    //OK

    //return test->TestKeyStore_GetType_Encrypted_Success();                                        //OK

    //return test->TestKeyStore_IsCertificateEntry_CA_Encrypted_Success();                          //OK

    //return test->TestKeyStore_IsCertificateEntry_PrivateKey_Encrypted_Failure();                  //OK

    //return test->TestKeyStore_IsCertificateEntry_NonExist_Encrypted_Failure();                    //OK

    //return test->TestKeyStore_IsCertificateEntry_NonExist_Unencrypted_Failure();                  //OK

    //return test->TestKeyStore_IsKeyEntry_PrivateKey_Encrypted_Success();                          //OK

    //return test->TestKeyStore_IsKeyEntry_CA_Encrypted_Failure();                                  //OK

    //return test->TestKeyStore_IsKeyEntry_NonExist_Encrypted_Failure();                            //OK

    //return test->TestKeyStore_SetCertificate_CA_Encrypted_Success();                              //OK

    //return test->TestKeyStore_SetCertificate_CAExists_Overwrite_Encrypted_Success();              //OK

    //return test->TestKeyStore_SetCertificate_PrivateKeyExists_Encrypted_Failure();                //OK

    //return test->TestKeyStore_SetEntry_PrivateKeyEntry_Encrypted_Success();                       //OK

    //return test->TestKeyStore_SetEntry_PrivateKeyEntry_DSA_Unencrypted_Success();                 //OK

    // return test->TestKeyStore_SetEntry_PrivateKeyEntry_EC_Unencrypted_Success();                 //OK

    //return test->TestKeyStore_SetEntry_PrivateKeyEntry_RSA_Unencrypted_Success();                 //OK

    return test->TestKeyStore_SetEntry_PrivateKeyEntry_Params_Unencrypted_Failure();                         //OK

    //return test->TestKeyStore_SetEntry_PrivateKeyEntry_Overwrites_PrivateKeyEntry_Encrypted_Success(); //OK

    //return test->TestKeyStore_SetEntry_CAEntry_Overwrites_PrivateKeyEntry_Encrypted_Success();    //OK

    //return test->TestKeyStore_SetEntry_PrivateKeyEntry_Overwrites_CAEntry_Encrypted_Success();    //OK

    //return test->TestKeyStore_SetEntry_PrivateKeyEntry_Overwrites_ShortPrivateKeyEntry_Encrypted_Success(); //OK

    //return test->TestKeyStore_SetEntry_CAEntry_Overwrites_CAEntry_Encrypted_Success();            //OK

    //return test->TestKeyStore_SetKeyEntry_ProtectedKey_Encrypted_Failure();                       //OK

    //return test->TestKeyStore_SetKeyEntry_Encrypted_Success();                                    //OK

    //return test->TestKeyStore_SetKeyEntry_Replaced_Encrypted_Success();                           //OK

    //return test->TestKeyStore_SetKeyEntry_ReplacedChain_Encrypted_Success();                      //OK ElastosOpenSSL replace CX509V3CertificateGenerator

    //return test->TestKeyStore_SetKeyEntry_ReplacedChain_DifferentPrivateKey_Encrypted_Failure();  //OK ElastosOpenSSL replace CX509V3CertificateGenerator

    return test->TestKeyStore_SetKeyEntry_ReplacedChain_UnencryptedToEncrypted_Failure();           //OK ElastosOpenSSL replace CX509V3CertificateGenerator

    //return test->TestKeyStore_Size_Encrypted_Success();                                           //OK

    //return test->TestKeyStore_Store_LoadStoreParam_Encrypted_Failure();                           //OK

    //return test->TestKeyStore_Load_InputStreamSupplied_Encrypted_Failure();                       //OK

    //return test->TestKeyStore_Load_PasswordSupplied_Encrypted_Failure();                          //OK

    //return test->TestKeyStore_Store_OutputStream_Encrypted_Failure();                             //OK

    //return test->TestKeyStore_KeyOperations_Wrap_Encrypted_Success();                             //OK ElastosOpenSSL replace CX509V3CertificateGenerator

    return NOERROR;
}

ECode CActivityOne::SignatureTest()
{
    //SignatureTest::SignatureWithRSA();
    //SignatureTest::SignatureWithEC();
    SignatureTest::SignatureWithDSA();
    return NOERROR;
}

ECode CActivityOne::CipherTest()
{
    return CipherTest::Test();
}

ECode CActivityOne::SelfSignTest()
{
    Logger::I(TAG, " SelfSignTest begin");
    AutoPtr<IOpenSSLX509V3CertificateGeneratorHelper> certGenHelper;
    COpenSSLX509V3CertificateGeneratorHelper::AcquireSingleton((IOpenSSLX509V3CertificateGeneratorHelper**)&certGenHelper);

    AutoPtr<IKeyPairGeneratorHelper> kpgHelper;
    CKeyPairGeneratorHelper::AcquireSingleton((IKeyPairGeneratorHelper**)&kpgHelper);
    AutoPtr<IKeyPairGenerator> keyPairGenerator;
    kpgHelper->GetInstance(String("RSA"), String("ElastosOpenSSL"), (IKeyPairGenerator**)&keyPairGenerator);
    keyPairGenerator->Initialize(1024);
    AutoPtr<IKeyPair> keyPair;
    keyPairGenerator->GenerateKeyPair((IKeyPair**)&keyPair);

    AutoPtr<IPublicKey> pubKey;
    keyPair->GetPublic((IPublicKey**)&pubKey);

    AutoPtr<IPrivateKey> privKey;
    keyPair->GetPrivate((IPrivateKey**)&privKey);

    AutoPtr<IBigIntegerHelper> biHelper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&biHelper);
    AutoPtr<IBigInteger> serialNumber;
    biHelper->ValueOf(12345, (IBigInteger**)&serialNumber);
    AutoPtr<IDate> start;
    CDate::New(116, 11, 23, (IDate**)&start);
    AutoPtr<IDate> end;
    CDate::New(126, 1, 1, (IDate**)&end);
    String subjectDNName("C=CN2, L=SH2, ST=SH2, CN=kortide2, O=kortide2, OU=OS2");

    AutoPtr<IX509Certificate> cert;
    certGenHelper->Generate(
            pubKey, privKey, serialNumber, start, end, subjectDNName, subjectDNName, (IX509Certificate**)&cert);

    Logger::I(TAG, " SelfSignTest end");
    return NOERROR;
}

ECode CActivityOne::ElastosKeyPairZLL()
{
    Logger::D(TAG, "begin ElastosKeyPairZLL");
    //AutoPtr<Elastos::Droid::KeyStore::Security::IKeyStore> androidKeyStore;
    //AutoPtr<Elastos::Droid::KeyStore::Security::IKeyStoreHelper> helpre;
    //Elastos::Droid::KeyStore::Security::CKeyStoreHelper::AcquireSingleton(
    //        (Elastos::Droid::KeyStore::Security::IKeyStoreHelper**)&helpre);
    //helpre->GetInstance((Elastos::Droid::KeyStore::Security::IKeyStore**)&androidKeyStore);

    //Boolean res;
    //androidKeyStore->Reset(&res);
    //assert(res == TRUE);
    //androidKeyStore->IsUnlocked(&res);
    //assert(res == FALSE);

    //androidKeyStore->Password(String("1111"), &res);
    //assert(res == TRUE);

    //androidKeyStore->IsUnlocked(&res);
    //assert(res == TRUE);

    //AutoPtr<ArrayOf<String> > aliases;
    //androidKeyStore->Saw(String(""), (ArrayOf<String>**)&aliases);
    //assert(aliases != NULL);
    //assert(0 == aliases->GetLength());

    AutoPtr<IKeyPairGeneratorHelper> kpgHelper;
    CKeyPairGeneratorHelper::AcquireSingleton((IKeyPairGeneratorHelper**)&kpgHelper);
    AutoPtr<IKeyPairGenerator> keyPairGenerator;
    kpgHelper->GetInstance(String("RSA"), String("ElastosKeyStore"), (IKeyPairGenerator**)&keyPairGenerator);

    AutoPtr<IKeyPairGeneratorSpecBuilder> builder;
    CKeyPairGeneratorSpecBuilder::New(this, (IKeyPairGeneratorSpecBuilder**)&builder);
    builder->SetAlias(String("ZLLTest"));
    //builder->SetKeyType(String("RSA"));
    builder->SetKeyType(String("DSA"));
    //builder->SetKeySize(1024);
    AutoPtr<IX500Principal> dn;
    CX500Principal::New(String("CN=test1"), (IX500Principal**)&dn);
    builder->SetSubject(dn);

    AutoPtr<IBigIntegerHelper> helper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&helper);
    AutoPtr<IBigInteger> serialNumber1;
    helper->ValueOf(2L, (IBigInteger**)&serialNumber1);
    builder->SetSerialNumber(serialNumber1);
    AutoPtr<IDate> start;
    CDate::New(116, 11, 23, (IDate**)&start);
    AutoPtr<IDate> end;
    CDate::New(126, 1, 1, (IDate**)&end);
    builder->SetStartDate(start);
    builder->SetEndDate(end);
    //builder->SetEncryptionRequired();
    AutoPtr<IKeyPairGeneratorSpec> spec;
    builder->Build((IKeyPairGeneratorSpec**)&spec);

    //AutoPtr<ISecureRandom> r;
    //CSecureRandom::New((ISecureRandom**)&r);

    //keyPairGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec), r);
    keyPairGenerator->Initialize(IAlgorithmParameterSpec::Probe(spec));


    AutoPtr<IKeyPair> keyPair;
    keyPairGenerator->GenerateKeyPair((IKeyPair**)&keyPair);

    AutoPtr<IPublicKey> publicKey;
    keyPair->GetPublic((IPublicKey**)&publicKey);

    AutoPtr<IPrivateKey> privateKey;
    keyPair->GetPrivate((IPrivateKey**)&privateKey);

    AutoPtr<ArrayOf<Byte> > publicEncoded;
    IKey::Probe(publicKey)->GetEncoded((ArrayOf<Byte>**)&publicEncoded);
    if (publicEncoded != NULL) {
        String publicStr = StringUtils::ToHexString(*publicEncoded);
        Logger::D(TAG, "RSA publicKey string is : %s, ", publicStr.string());
    }
    else {
        Logger::D(TAG, "RSA publicKey string is : NULL,");
    }

    AutoPtr<ArrayOf<Byte> > privateEncoded;
    IKey::Probe(privateKey)->GetEncoded((ArrayOf<Byte>**)&privateEncoded);
    if (privateEncoded != NULL) {
        String privateStr = StringUtils::ToHexString(*privateEncoded);
        Logger::D(TAG, "DSA privateKey string is : %s,", privateStr.string());
    }
    else {
        // because using an OpenSSL ENGINE, see: OpenSSLRSAPrivateKey::GetEncoded
        Logger::D(TAG, "DSA privateKey string is : NULL,");
    }

#if true
    Logger::D(TAG, "ElastosKeyPairZLL begin signature with the privateKey and publicKey");
    AutoPtr<ISignatureHelper> sh;
    CSignatureHelper::AcquireSingleton((ISignatureHelper**)&sh);
    //String algorithm("MD5withRSA");
    String algorithm("SHA1withDSA");
    AutoPtr<ISignature> sig;
    ECode ec = sh->GetInstance(algorithm, (ISignature**)&sig);
    ASSERT_SUCCEEDED(ec);
    sig->InitSign(privateKey);
    ec = sig->Update(algorithm.GetBytes());
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > signature;
    ec = sig->Sign((ArrayOf<Byte>**)&signature);
    ASSERT_SUCCEEDED(ec);
    assert(signature != NULL && "Signature must not be null");

    ec = sig->InitVerify(publicKey);
    ASSERT_SUCCEEDED(ec);
    //ec = sig->Update(String("MD5withRSA").GetBytes());
    ec = sig->Update(String("SHA1withDSA").GetBytes());

    Boolean isVerified = FALSE;
    ec = sig->Verify(signature, &isVerified);
    ASSERT_SUCCEEDED(ec);
    assert("Signature must match expected signature" && isVerified);

    Logger::D(TAG, "ElastosKeyPairZLL end signature with the privateKey and publicKey");
#endif
    //=====================================================================
#if true
    Logger::I(TAG, "ElastosKeyPairZLL begin selfsigned certificate");
    AutoPtr<IOpenSSLX509V3CertificateGeneratorHelper> certGenHelper;
    COpenSSLX509V3CertificateGeneratorHelper::AcquireSingleton((IOpenSSLX509V3CertificateGeneratorHelper**)&certGenHelper);

    AutoPtr<IBigIntegerHelper> biHelper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&biHelper);
    AutoPtr<IBigInteger> serialNumber;
    biHelper->ValueOf(1234567, (IBigInteger**)&serialNumber);
    AutoPtr<IDate> start2;
    CDate::New(116, 11, 24, (IDate**)&start2);
    AutoPtr<IDate> end2;
    CDate::New(126, 1, 1, (IDate**)&end2);
    String subjectDNName("C=CN, L=SH, ST=SH2, CN=kortide2, O=kortide2, OU=OS2");

    AutoPtr<IX509Certificate> cert;
    certGenHelper->Generate(
                    publicKey, privateKey, serialNumber, start2, end2, subjectDNName, subjectDNName, (IX509Certificate**)&cert);

    Logger::I(TAG, "ElastosKeyPairZLL end selfsigned certificate");
#endif
    Logger::D(TAG, "end ElastosKeyPairZLL");

    return NOERROR;
}

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos
