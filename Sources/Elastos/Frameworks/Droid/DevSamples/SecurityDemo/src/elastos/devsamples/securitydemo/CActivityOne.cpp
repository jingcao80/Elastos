
#include "elastos/devsamples/securitydemo/CActivityOne.h"
#include "org/apache/harmony/security/fortress/Services.h"
#include "elastos/devsamples/securitydemo/Signaturetest.h"
#include "R.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.KeyStore.h"
#include "elastos/security/KeyPairGenerator.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IInputStream;
using Elastos::Security::CKeyStoreHelper;
using Elastos::Security::CSignatureHelper;
using Elastos::Security::IKey;
using Elastos::Security::IKeyStore;
using Elastos::Security::IKeyStoreHelper;
using Elastos::Security::IKeyFactoryHelper;
using Elastos::Security::CKeyFactoryHelper;
using Elastos::Security::IKeyFactory;
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
using Elastos::Security::Spec::IX509EncodedKeySpec;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Elastos::Security::Spec::IKeySpec;
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
    else if (id == R::id::Button6) {
        return mHost->Button6Function();
    }
    else if (id == R::id::Button7) {
        return mHost->Button7Function();
    }
    else if (id == R::id::Button12) {
        return mHost->ButtonCSecureRandom();
    }
    else if (id == R::id::signaturetest) {
        return mHost->SignatureTest();
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
    ksHelper->GetInstance(String("JKS"), (IKeyStore**)&keyStore);
    keyStore->Load(inputStream, storepassword.GetChars());
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
    ksHelper->GetInstance(String("JKS"), (IKeyStore**)&keyStorePub);
    keyStorePub->Load(inputStreamPub, storepassword.GetChars());
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

    // AutoPtr<IView> view = FindViewById(R::id::ImageView);
    // IImageView* imageView = IImageView::Probe(view);
    // imageView->SetImageResource(R::drawable::earth);
    // imageView->SetScaleType(Elastos::Droid::Widget::ImageViewScaleType_FIT_CENTER);

    AutoPtr<IView> view = FindViewById(R::id::Button1);
    AutoPtr<MyListener> l = new MyListener(this);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::Button2);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::Button6);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::Button7);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::Button12);
    view->SetOnClickListener(l.Get());

    view = NULL;
    view = FindViewById(R::id::signaturetest);
    view->SetOnClickListener(l.Get());

    //TODO : To be deleted.
    Services::Initialize();

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
    macHelper->GetInstance(String("HMACMD5"), (IMac**)&mac);
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

ECode CActivityOne::Button6Function()// KeyAgreement
{
    Logger::E(TAG, "leliang begin Button6Function ");
    Services::Initialize();
    AutoPtr<IKeyAgreementHelper> kaHelper;
    CKeyAgreementHelper::AcquireSingleton((IKeyAgreementHelper**)&kaHelper);
    AutoPtr<IKeyAgreement> ka;
    kaHelper->GetInstance(String("ECDH"), (IKeyAgreement**)&ka);
    Logger::E(TAG, "leliang after get IKeyAgreement ka:%p", ka.Get());

    AutoPtr<IKeyFactoryHelper> kfHelper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&kfHelper);
    AutoPtr<IKeyFactory> kf;
    kfHelper->GetInstance(String("RSA"), (IKeyFactory**)&kf);

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

    AutoPtr<IX509EncodedKeySpec> x509EncodedKeySpec;
    CX509EncodedKeySpec::New(der_public, (IX509EncodedKeySpec**)&x509EncodedKeySpec);
    IKeySpec* keySpec = IKeySpec::Probe(x509EncodedKeySpec);
    Logger::E(TAG, "leliang keySpec: %p", keySpec);

    AutoPtr<IPublicKey> pubKey;
    kf->GeneratePublic(keySpec, (IPublicKey**)&pubKey);
    Logger::E(TAG, "leliang pubKey: %p", pubKey.Get());
    AutoPtr<IPrivateKey> priKey;
    kf->GeneratePrivate(keySpec, (IPrivateKey**)&priKey);
    Logger::E(TAG, "leliang priKey: %p", priKey.Get());

    AutoPtr<IKey> tmpResult;
    ka->Init(IKey::Probe(priKey));
    ka->DoPhase(IKey::Probe(pubKey), TRUE, (IKey**)&tmpResult);

    AutoPtr<ArrayOf<Byte> > secret;
    ka->GenerateSecret((ArrayOf<Byte>**)&secret);

    Logger::E(TAG, "leliang secret begin");
    for (Int32 i = 0; i < secret->GetLength(); i++) {
        Logger::E(TAG, "secret[%d]=%d", i, (*secret)[i]);
    }
    Logger::E(TAG, "leliang secret end");
    Logger::E(TAG, "leliang end Button6Function ");
    return NOERROR;
}

ECode CActivityOne::Button7Function()// Certificate
{
    Logger::E(TAG, "leliang begin Button7Function");
    Services::Initialize();
    AutoPtr<ICertificateFactoryHelper> cfHelper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&cfHelper);
    AutoPtr<ICertificateFactory> cf;
    cfHelper->GetInstance(String("X509"), (ICertificateFactory**)&cf);
    Logger::E(TAG, "leliang after get ICertificateFactory cf:%p", cf.Get());
    AutoPtr<IFileInputStream> fis;
    CFileInputStream::New(String("/data/security/test.crt"), (IFileInputStream**)&fis);
    Logger::E(TAG, "leliang after create CFileInputStream, fis:%p", fis.Get());
    if (fis == NULL) {
        Logger::E(TAG, "leliang /data/security/test.crt is not found");
        return NOERROR;
    }
    AutoPtr<ICertificate> certificate;
    cf->GenerateCertificate(IInputStream::Probe(fis), (ICertificate**)&certificate);
    Logger::E(TAG, "leliang after get ICertificate certificate:%p", certificate.Get());

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

ECode CActivityOne::SignatureTest()
{
    return SignatureTest::MD5WithRSA();
}

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos
