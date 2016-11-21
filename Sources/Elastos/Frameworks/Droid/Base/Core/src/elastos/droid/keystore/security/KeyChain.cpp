#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Security.h"
#include "elastos/droid/keystore/security/KeyChain.h"
#include "elastos/droid/keystore/security/KeyStore.h"
#include "elastos/droid/app/CPendingIntentHelper.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/utility/logging/Logger.h>
#include "_Org.Conscrypt.h"

using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::IO::EIID_ICloseable;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::IO::IInputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Security::IPrivateKey;
using Elastos::Security::Cert::ICertificateFactoryHelper;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::ICertificate;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::CLinkedBlockingQueue;
using Elastos::Utility::Logging::Logger;
using Org::Conscrypt::COpenSSLEngineHelper;
using Org::Conscrypt::IOpenSSLEngine;
using Org::Conscrypt::IOpenSSLEngineHelper;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

//=========================================================================
//  KeyChain::AliasResponse
//=========================================================================
CAR_INTERFACE_IMPL_2(KeyChain::AliasResponse, Object, IIKeyChainAliasCallback, IBinder);

KeyChain::AliasResponse::AliasResponse(
    /* [in] */ IKeyChainAliasCallback* keyChainAliasResponse)
    : mKeyChainAliasResponse(keyChainAliasResponse)
{}

ECode KeyChain::AliasResponse::Alias(
    /* [in] */ const String& alias)
{
    return mKeyChainAliasResponse->Alias(alias);
}

ECode KeyChain::AliasResponse::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = "KeyChain::AliasResponse";
    return NOERROR;
}


//=========================================================================
//  KeyChain::KeyChainConnection
//=========================================================================
CAR_INTERFACE_IMPL_2(KeyChain::KeyChainConnection, Object, IKeyChainConnection, ICloseable);

KeyChain::KeyChainConnection::KeyChainConnection(
    /* [in] */ IContext* context,
    /* [in] */ IServiceConnection* serviceConnection,
    /* [in] */ IIKeyChainService* service)
    : mContext(context)
    , mServiceConnection(serviceConnection)
    , mService(service)
{}

ECode KeyChain::KeyChainConnection::Close()
{
    return mContext->UnbindService(mServiceConnection);
}

ECode KeyChain::KeyChainConnection::GetService(
    /* [out] */ IIKeyChainService** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mService;
    REFCOUNT_ADD(*result);
    return NOERROR;
}


//=========================================================================
//  KeyChain::ServiceConnection
//=========================================================================
CAR_INTERFACE_IMPL(KeyChain::ServiceConnection, Object, IServiceConnection);

KeyChain::ServiceConnection::ServiceConnection(
    /* [in] */ KeyChain* host,
    /* [in] */ IBlockingQueue* queue)
    : mHost(host)
    , mQueue(queue)
    , mConnectedAtLeastOnce(FALSE)
{
}

ECode KeyChain::ServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (!mConnectedAtLeastOnce) {
        mConnectedAtLeastOnce = TRUE;
        //try {
        mQueue->Put(IIKeyChainService::Probe(service));
        //} catch (InterruptedException e) {
            // will never happen, since the queue starts with one available slot
        //}
    }
    return NOERROR;
}

ECode KeyChain::ServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return NOERROR;
}


//=========================================================================
//  KeyChain
//=========================================================================
//CAR_INTERFACE_IMPL(KeyChain, Object, IKeyChain);
const String KeyChain::TAG("KeyChain");
const String KeyChain::ACTION_INSTALL("android.credentials.INSTALL");
const String KeyChain::CERT_INSTALLER_PACKAGE("com.android.certinstaller");
const String KeyChain::KEYCHAIN_PACKAGE("com.android.keychain");
const String KeyChain::ACTION_CHOOSER("com.android.keychain.CHOOSER");

ECode KeyChain::CreateInstallIntent(
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIntent> intent;
    CIntent::New(ACTION_INSTALL, (IIntent**)&intent);
    intent->SetClassName(CERT_INSTALLER_PACKAGE, String("com.android.certinstaller.CertInstallerMain"));
    *result = intent;
    REFCOUNT_ADD(*result);;
    return NOERROR;
}

ECode KeyChain::ChoosePrivateKeyAlias(
    /* [in] */ IActivity* activity,
    /* [in] */ IKeyChainAliasCallback* response,
    /* [in] */ ArrayOf<String>* keyTypes,
    /* [in] */ ArrayOf<IPrincipal*>* issuers,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& alias)
{
    if (activity == NULL) {
        //throw new NullPointerException("activity == null");
        Logger::E("KeyChain", "ChoosePrivateKeyAlias activity == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (response == NULL) {
        //throw new NullPointerException("response == null");
        Logger::E("KeyChain", "ChoosePrivateKeyAlias response == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IIntent> intent;
    CIntent::New(ACTION_CHOOSER, (IIntent**)&intent);
    intent->SetPackage(KEYCHAIN_PACKAGE);
    intent->PutExtra(IKeyChain::EXTRA_RESPONSE, new AliasResponse(response));
    intent->PutExtra(IKeyChain::EXTRA_HOST, host);
    intent->PutExtra(IKeyChain::EXTRA_PORT, port);
    intent->PutExtra(IKeyChain::EXTRA_ALIAS, alias);
    // the PendingIntent is used to get calling package name
    AutoPtr<IPendingIntentHelper> piHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);
    AutoPtr<IPendingIntent> pendingIntent;
    AutoPtr<IIntent> tmp;
    CIntent::New(ACTION_CHOOSER, (IIntent**)&tmp);
    piHelper->GetActivity(IContext::Probe(activity), 0, tmp, 0, (IPendingIntent**)&pendingIntent);
    intent->PutExtra(IKeyChain::EXTRA_SENDER, IParcelable::Probe(tmp));
    IContext::Probe(activity)->StartActivity(intent);
    return NOERROR;
}

ECode KeyChain::GetPrivateKey(
    /* [in] */ IContext* context,
    /* [in] */ const String& alias,
    /* [out] */ IPrivateKey** result)
{
    VALIDATE_NOT_NULL(result);

    if (alias.IsNull()) {
        //throw new NullPointerException("alias == null");
        Logger::E("KeyChain", "GetPrivateKey alias == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IKeyChainConnection> keyChainConnection;
    Bind(context, (IKeyChainConnection**)&keyChainConnection);
    //try {
    AutoPtr<IIKeyChainService> keyChainService;
    keyChainConnection->GetService((IIKeyChainService**)&keyChainService);
    String keyId;
    keyChainService->RequestPrivateKey(alias, &keyId);
    if (keyId.IsNull()) {
        //throw new KeyChainException("keystore had a problem");
        Logger::E("KeyChain", "GetPrivateKey keystore had a problem");
        return E_KEY_CHAIN_EXCEPTION;
    }

    AutoPtr<IOpenSSLEngineHelper> oeHelper;
    COpenSSLEngineHelper::AcquireSingleton((IOpenSSLEngineHelper**)&oeHelper);
    AutoPtr<IOpenSSLEngine> engine;
    oeHelper->GetInstance(String("keystore"), (IOpenSSLEngine**)&engine);
    engine->GetPrivateKeyById(keyId, result);
    //} catch (RemoteException e) {
    //    throw new KeyChainException(e);
    //} catch (RuntimeException e) {
    //    // only certain RuntimeExceptions can be propagated across the IKeyChainService call
    //    throw new KeyChainException(e);
    //} catch (InvalidKeyException e) {
    //    throw new KeyChainException(e);
    //} finally {
    ICloseable::Probe(keyChainConnection)->Close();
    //}
    return NOERROR;
}

ECode KeyChain::GetCertificateChain(
    /* [in] */ IContext* context,
    /* [in] */ const String& alias,
    /* [out, callee] */ ArrayOf<IX509Certificate*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    if (alias.IsNull()) {
        //throw new NullPointerException("alias == null");
        Logger::E("KeyChain", "GetCertificateChain alias == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IKeyChainConnection> keyChainConnection;
    Bind(context, (IKeyChainConnection**)&keyChainConnection);
    //try {
    AutoPtr<IIKeyChainService> keyChainService;
    keyChainConnection->GetService((IIKeyChainService**)&keyChainService);

    AutoPtr<ArrayOf<Byte> > certificateBytes;
    keyChainService->GetCertificate(alias, (ArrayOf<Byte>**)&certificateBytes);
    if (certificateBytes == NULL) {
        return NOERROR;
    }

    //TODO AutoPtr<ITrustedCertificateStore> store;
    //TODO CTrustedCertificateStore::New((ITrustedCertificateStore**)&store);
    //List<X509Certificate> chain = store.getCertificateChain(ToCertificate(certificateBytes));
    AutoPtr<IX509Certificate> x509;
    ToCertificate(certificateBytes, (IX509Certificate**)&x509);
    AutoPtr<IList> chain;
    //TODO store->GetCertificateChain(x509, (IList**)&chain);
    Int32 size;
    chain->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > inArray = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr<ArrayOf<IInterface* > > outArray;
    chain->ToArray(inArray, (ArrayOf<IInterface*>**)&outArray);
    size = outArray->GetLength();
    AutoPtr<ArrayOf<IX509Certificate*> > x509s = ArrayOf<IX509Certificate*>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        IInterface* obj = (*outArray)[i];
        IX509Certificate* x509 = IX509Certificate::Probe(obj);
        x509s->Set(i, x509);
    }
    *result = x509s;
    REFCOUNT_ADD(*result);
    //} catch (CertificateException e) {
    //    throw new KeyChainException(e);
    //} catch (RemoteException e) {
    //    throw new KeyChainException(e);
    //} catch (RuntimeException e) {
    //    // only certain RuntimeExceptions can be propagated across the IKeyChainService call
    //    throw new KeyChainException(e);
    //} finally {
    ICloseable::Probe(keyChainConnection)->Close();
    //}
    return NOERROR;
}

ECode KeyChain::IsKeyAlgorithmSupported(
    /* [in] */ const String& algorithm,
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);
    String algUpper = algorithm.ToUpperCase(/*Locale.US*/);
    *supported = String("DSA").Equals(algUpper) || String("EC").Equals(algUpper) || String("RSA").Equals(algUpper);
    return NOERROR;
}

ECode KeyChain::IsBoundKeyAlgorithm(
    /* [in] */ const String& algorithm,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    Boolean bTmp;
    if (!(IsKeyAlgorithmSupported(algorithm, &bTmp), bTmp)) {
        *ret = FALSE;
        return NOERROR;
    }

    AutoPtr<KeyStore> keyStore = KeyStore::GetInstance();
    return keyStore->IsHardwareBacked(algorithm, ret);
}

ECode KeyChain::ToCertificate(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [out] */ IX509Certificate** certificate)
{
    VALIDATE_NOT_NULL(certificate);
    *certificate = NULL;
    if (bytes == NULL) {
        //throw new IllegalArgumentException("bytes == null");
        Logger::E("KeyChain", "ToCertificate, bytes == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //try {
    AutoPtr<ICertificateFactoryHelper> cfHelper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&cfHelper);
    AutoPtr<ICertificateFactory> certFactory;
    cfHelper->GetInstance(String("X.509"), (ICertificateFactory**)&certFactory);
    AutoPtr<IByteArrayInputStream> baInputStream;
    CByteArrayInputStream::New(bytes, (IByteArrayInputStream**)&baInputStream);
    AutoPtr<ICertificate> cert;
    certFactory->GenerateCertificate(IInputStream::Probe(baInputStream), (ICertificate**)&cert);
    *certificate = IX509Certificate::Probe(cert);
    REFCOUNT_ADD(*certificate);
    //} catch (CertificateException e) {
    //    throw new AssertionError(e);
    //}
    return NOERROR;
}

ECode KeyChain::Bind(
    /* [in] */ IContext* context,
    /* [out] */ IKeyChainConnection** connection)
{
    AutoPtr<IUserHandle> uh;
    Process::MyUserHandle((IUserHandle**)&uh);
    return BindAsUser(context, uh, connection);
}

ECode KeyChain::BindAsUser(
    /* [in] */ IContext* context,
    /* [in] */ IUserHandle* user,
    /* [out] */ IKeyChainConnection** connection)
{
    VALIDATE_NOT_NULL(connection);
    if (context == NULL) {
        //throw new NullPointerException("context == null");
        Logger::E("KeyChain", "BindAsUser, context == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    EnsureNotOnMainThread(context);
    //final BlockingQueue<IKeyChainService> q = new LinkedBlockingQueue<IKeyChainService>(1);
    AutoPtr<IBlockingQueue> q;
    CLinkedBlockingQueue::New(1, (IBlockingQueue**)&q);

    AutoPtr<ServiceConnection> keyChainServiceConnection = new ServiceConnection(NULL, q);

    AutoPtr<IIntent> intent;
    CIntent::New(String("android.security.IKeyChainService")/*TODO IKeyChainService.class.getName()*/, (IIntent**)&intent);
    AutoPtr<IComponentName> comp;
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    intent->ResolveSystemService(pm, 0, (IComponentName**)&comp);
    intent->SetComponent(comp);
    Boolean isBound;
    context->BindServiceAsUser(intent, keyChainServiceConnection,
                                                IContext::BIND_AUTO_CREATE,
                                                user, &isBound);
    if (!isBound) {
        //throw new AssertionError("could not bind to KeyChainService");
        Logger::E("KeyChain", "BindAsUser, could not bind to KeyChainService");
        return E_ASSERTION_ERROR;
    }

    AutoPtr<IInterface> obj;
    q->Take((IInterface**)&obj);
    AutoPtr<IKeyChainConnection> kcc = new KeyChainConnection(context, keyChainServiceConnection, IIKeyChainService::Probe(obj));
    *connection = kcc;
    REFCOUNT_ADD(*connection);
    return NOERROR;
}

ECode KeyChain::EnsureNotOnMainThread(
    /* [in] */ IContext* context)
{
    AutoPtr<ILooper> looper = Looper::GetMyLooper();
    AutoPtr<ILooper> mainLooper;
    context->GetMainLooper((ILooper**)&mainLooper);
    if (looper != NULL && looper == mainLooper) {
        //throw new IllegalStateException("calling this from your main thread can lead to deadlock");
        Logger::E("KeyChain", "EnsureNotOnMainThread, calling this from your main thread can lead to deadlock");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security
