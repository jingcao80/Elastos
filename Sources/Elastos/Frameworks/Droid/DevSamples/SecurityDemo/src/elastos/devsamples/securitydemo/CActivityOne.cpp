
#include "elastos/devsamples/securitydemo/CActivityOne.h"
#include "org/apache/harmony/security/fortress/Services.h"
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
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::CKeyStoreHelper;
using Elastos::Security::CSignatureHelper;
using Elastos::Security::IKey;
using Elastos::Security::IKeyStore;
using Elastos::Security::IKeyStoreHelper;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::ISignature;
using Elastos::Security::ISignatureHelper;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Harmony::Security::Fortress::Services;

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
    Services::Initialize();
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

    // Get instance of Mac object implementing HMAC-MD5, and
    // initialize it with the above secret key
    AutoPtr<IMacHelper> macHelper;
    CMacHelper::AcquireSingleton((IMacHelper**)&macHelper);
    AutoPtr<IMac> mac;
    macHelper->GetInstance(String("HMACMD5"), (IMac**)&mac);
    mac->Init(IKey::Probe(sk));
    String test("Hi There");
    AutoPtr<ArrayOf<Byte> > input = test.GetBytes();
    AutoPtr<ArrayOf<Byte> > result;
    mac->DoFinal(input, (ArrayOf<Byte>**)&result);

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

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos
