
#include "elastos/devsamples/securitydemo/CActivityOne.h"
#include "R.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include "Elastos.Droid.Widget.h"
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
    Logger::I(TAG, "SecurityDemo::MyListener::OnClick");

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

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos
