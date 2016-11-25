#include "org/conscrypt/CDefaultSSLContextImpl.h"

using Elastos::Security::CKeyStoreHelper;
using Elastos::Security::IKeyStore;
using Elastos::Security::IKeyStoreHelper;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IInputStream;
using Elastosx::Net::Ssl::CKeyManagerFactoryHelper;
using Elastosx::Net::Ssl::CTrustManagerFactoryHelper;
using Elastosx::Net::Ssl::IKeyManagerFactory;
using Elastosx::Net::Ssl::IKeyManagerFactoryHelper;
using Elastosx::Net::Ssl::ITrustManagerFactory;
using Elastosx::Net::Ssl::ITrustManagerFactoryHelper;

namespace Org {
namespace Conscrypt {

AutoPtr<ArrayOf<IKeyManager*> > CDefaultSSLContextImpl::KEY_MANAGERS;
AutoPtr<ArrayOf<ITrustManager*> > CDefaultSSLContextImpl::TRUST_MANAGERS;

CAR_OBJECT_IMPL(CDefaultSSLContextImpl)

CAR_INTERFACE_IMPL(CDefaultSSLContextImpl, OpenSSLContextImpl,
        IDefaultSSLContextImpl)

ECode CDefaultSSLContextImpl::constructor()
{
    return OpenSSLContextImpl::constructor(NULL);
}

ECode CDefaultSSLContextImpl::GetKeyManagers(
    /* [out, callee] */ ArrayOf<IKeyManager*>** result)
{
    VALIDATE_NOT_NULL(result)
    if (KEY_MANAGERS != NULL) {
        *result = KEY_MANAGERS;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    // find KeyStore, KeyManagers
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String keystore;
    system->GetProperty(String("javax.net.ssl.keyStore"), &keystore);
    if (keystore == NULL) {
        *result = NULL;
        return NOERROR;
    }
    String keystorepwd;
    system->GetProperty(String("javax.net.ssl.keyStorePassword"), &keystorepwd);

    AutoPtr<ArrayOf<Char32> > pwd = (keystorepwd == NULL) ? NULL : keystorepwd.GetChars();

    AutoPtr<IKeyStoreHelper> ksHelper;
    CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&ksHelper);
    String type;
    ksHelper->GetDefaultType(&type);
    AutoPtr<IKeyStore> ks;
    ksHelper->GetInstance(type, (IKeyStore**)&ks);

    AutoPtr<IInputStream> is;
    // try {
    AutoPtr<IInputStream> fis;
    CFileInputStream::New(keystore, (IInputStream**)&fis);
    CBufferedInputStream::New(fis, (IInputStream**)&is);

    ks->Load(is, pwd);
    // } finally {
    if (is != NULL) {
        ICloseable::Probe(is)->Close();
    }
    // }

    AutoPtr<IKeyManagerFactoryHelper> kmfHelper;
    CKeyManagerFactoryHelper::AcquireSingleton((IKeyManagerFactoryHelper**)&kmfHelper);
    String kmfAlg;
    kmfHelper->GetDefaultAlgorithm(&kmfAlg);
    AutoPtr<IKeyManagerFactory> kmf;
    kmfHelper->GetInstance(kmfAlg, (IKeyManagerFactory**)&kmf);
    kmf->Init(ks, pwd);
    kmf->GetKeyManagers((ArrayOf<IKeyManager*>**)&KEY_MANAGERS);
    *result = KEY_MANAGERS;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDefaultSSLContextImpl::GetTrustManagers(
    /* [out, callee] */ ArrayOf<ITrustManager*>** result)
{
    VALIDATE_NOT_NULL(result)
    if (TRUST_MANAGERS != NULL) {
        *result = TRUST_MANAGERS;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    // find TrustStore, TrustManagers
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String keystore;
    system->GetProperty(String("javax.net.ssl.trustStore"), &keystore);
    if (keystore == NULL) {
        *result = NULL;
        return NOERROR;
    }
    String keystorepwd;
    system->GetProperty(String("javax.net.ssl.trustStorePassword"), &keystorepwd);

    AutoPtr<ArrayOf<Char32> > pwd = (keystorepwd == NULL) ? NULL : keystorepwd.GetChars();

    // TODO Defaults: jssecacerts; cacerts
    AutoPtr<IKeyStoreHelper> ksHelper;
    CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&ksHelper);
    String type;
    ksHelper->GetDefaultType(&type);
    AutoPtr<IKeyStore> ks;
    ksHelper->GetInstance(type, (IKeyStore**)&ks);

    AutoPtr<IInputStream> is;
    // try {
    AutoPtr<IInputStream> fis;
    CFileInputStream::New(keystore, (IInputStream**)&fis);
    CBufferedInputStream::New(fis, (IInputStream**)&is);

    ks->Load(is, pwd);
    // } finally {
    if (is != NULL) {
        ICloseable::Probe(is)->Close();
    }
    // }

    AutoPtr<ITrustManagerFactoryHelper> tmfHelper;
    CTrustManagerFactoryHelper::AcquireSingleton((ITrustManagerFactoryHelper**)&tmfHelper);
    String tmfAlg;
    tmfHelper->GetDefaultAlgorithm(&tmfAlg);
    AutoPtr<ITrustManagerFactory> tmf;
    tmfHelper->GetInstance(tmfAlg, (ITrustManagerFactory**)&tmf);

    tmf->Init(ks);
    tmf->GetTrustManagers((ArrayOf<ITrustManager*>**)&TRUST_MANAGERS);
    *result = TRUST_MANAGERS;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDefaultSSLContextImpl::EngineInit(
    /* [in] */ ArrayOf<IKeyManager*>* km,
    /* [in] */ ArrayOf<ITrustManager*>* tm,
    /* [in] */ ISecureRandom* sr)
{
    // throw new KeyManagementException("Do not init() the default SSLContext ");
    return E_KEY_MANAGEMENT_EXCEPTION;
}


} // namespace Conscrypt
} // namespace Org
