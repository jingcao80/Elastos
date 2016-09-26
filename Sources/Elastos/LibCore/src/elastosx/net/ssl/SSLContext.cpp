
#include "Elastos.CoreLibrary.Security.h"
#include "AutoLock.h"
#include "SSLContext.h"
#include "CSSLContext.h"
#include "CSecurity.h"
#include "org/apache/harmony/security/fortress/CEngine.h"

using Elastos::Core::AutoLock;
using Elastos::Security::CSecurity;
using Elastos::Security::ISecurity;
using Org::Apache::Harmony::Security::Fortress::CEngine;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;

namespace Elastosx {
namespace Net {
namespace Ssl {

static AutoPtr<IEngine> InitEngine()
{
    AutoPtr<CEngine> e;
    CEngine::NewByFriend(String("SSLContext")/*SERVICE*/, (CEngine**)&e);
    return e;
}

INIT_PROI_5 const String SSLContext::sSERVICE = String("SSLContext");
INIT_PROI_5 const AutoPtr<IEngine> SSLContext::ENGINE = InitEngine();
INIT_PROI_5 AutoPtr<ISSLContext> SSLContext::sDEFAULT;
INIT_PROI_5 Object SSLContext::mLock;

CAR_INTERFACE_IMPL(SSLContext, Object, ISSLContext)

ECode SSLContext::GetDefault(
    /* [out] */ ISSLContext** context)
{
    VALIDATE_NOT_NULL(context)

    {    AutoLock syncLock(mLock);
        if (sDEFAULT == NULL) {
            FAIL_RETURN(SSLContext::GetInstance(String("Default"), (ISSLContext**)&sDEFAULT))
        }
        *context = sDEFAULT;
        REFCOUNT_ADD(*context)
        return NOERROR;
    }
    return NOERROR;
}

ECode SSLContext::SetDefault(
    /* [in] */ ISSLContext* sslContext)
{
    if (sslContext == NULL) {
        //throw new NullPointerException("sslContext == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    {    AutoLock syncLock(mLock);
        sDEFAULT = sslContext;
    }
    return NOERROR;
}

ECode SSLContext::GetInstance(
    /* [in] */ const String& protocol,
    /* [out] */ ISSLContext** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = NULL;

    if (protocol == NULL) {
        //throw new NullPointerException("protocol == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ISpiAndProvider> sap;
    FAIL_RETURN(ENGINE->GetInstance(protocol, NULL, (ISpiAndProvider**)&sap));
    AutoPtr<IInterface> spi;
    FAIL_RETURN(sap->GetSpi((IInterface**)&spi));
    AutoPtr<IProvider> provider;
    FAIL_RETURN(sap->GetProvider((IProvider**)&provider));
    return CSSLContext::New(ISSLContextSpi::Probe(spi), provider, protocol, instance);
}

ECode SSLContext::GetInstance(
    /* [in] */ const String& protocol,
    /* [in] */ const String& provider,
    /* [out] */ ISSLContext** instance)
{
    VALIDATE_NOT_NULL(instance)

    if (provider.IsNull()) {
        //throw new IllegalArgumentException("Provider is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (provider.GetLength() == 0) {
        //throw new IllegalArgumentException("Provider is empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IProvider> impProvider;
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProvider(provider, (IProvider**)&impProvider);
    if (impProvider == NULL) {
        //throw new NoSuchProviderException(provider);
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(protocol, impProvider, instance);
}

ECode SSLContext::GetInstance(
  /* [in] */ const String& protocol,
  /* [in] */ IProvider* provider,
  /* [out] */ ISSLContext** context)
{
    VALIDATE_NOT_NULL(context)

    if (provider == NULL) {
        //throw new IllegalArgumentException("provider is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (protocol.IsNull()) {
        //throw new NullPointerException("protocol == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IInterface> spi;
    ENGINE->GetInstance(protocol, provider, NULL, (IInterface**)&spi);
    return CSSLContext::New(ISSLContextSpi::Probe(spi), provider, protocol, context);
}

ECode SSLContext::constructor(
    /* [in] */ ISSLContextSpi* contextSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& protocol)
{
    mProvider = provider;
    mProtocol = protocol;
    mSpiImpl = contextSpi;
    return NOERROR;
}

ECode SSLContext::GetProtocol(
    /* [out] */ String* protocol)
{
    VALIDATE_NOT_NULL(protocol)

    *protocol = mProtocol;
    return NOERROR;
}

ECode SSLContext::GetProvider(
    /* [out] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider)

    *provider = mProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode SSLContext::Init(
    /* [in] */ ArrayOf<IKeyManager*>* km,
    /* [in] */ ArrayOf<ITrustManager*>* tm,
    /* [in] */ ISecureRandom* sr)
{
    return mSpiImpl->EngineInit(km, tm, sr);
}

ECode SSLContext::GetSocketFactory(
    /* [out] */ ISSLSocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return mSpiImpl->EngineGetSocketFactory(factory);
}

ECode SSLContext::GetServerSocketFactory(
    /* [out] */ ISSLServerSocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return mSpiImpl->EngineGetServerSocketFactory(factory);
}

ECode SSLContext::CreateSSLEngine(
    /* [out] */ ISSLEngine** engine)
{
    VALIDATE_NOT_NULL(engine)

    return mSpiImpl->EngineCreateSSLEngine(engine);
}

ECode SSLContext::CreateSSLEngine(
    /* [in] */ const String& peerHost,
    /* [in] */ Int32 peerPort,
    /* [out] */ ISSLEngine** engine)
{
    VALIDATE_NOT_NULL(engine)

    return mSpiImpl->EngineCreateSSLEngine(peerHost, peerPort, engine);
}

ECode SSLContext::GetServerSessionContext(
    /* [out] */ ISSLSessionContext** context)
{
    VALIDATE_NOT_NULL(context)

    return mSpiImpl->EngineGetServerSessionContext(context);
}

ECode SSLContext::GetClientSessionContext(
    /* [out] */ ISSLSessionContext** context)
{
    VALIDATE_NOT_NULL(context)

    return mSpiImpl->EngineGetClientSessionContext(context);
}

ECode SSLContext::GetDefaultSSLParameters(
    /* [out] */ ISSLParameters** param)
{
    VALIDATE_NOT_NULL(param)

    return mSpiImpl->EngineGetDefaultSSLParameters(param);
}

ECode SSLContext::GetSupportedSSLParameters(
    /* [out] */ ISSLParameters** param)
{
    VALIDATE_NOT_NULL(param)

    return mSpiImpl->EngineGetSupportedSSLParameters(param);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
