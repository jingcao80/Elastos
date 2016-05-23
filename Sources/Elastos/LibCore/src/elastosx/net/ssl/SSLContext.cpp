
#include "Elastos.CoreLibrary.Security.h"
#include "SSLContext.h"
#include "AutoLock.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;

namespace Elastosx {
namespace Net {
namespace Ssl {

const String SSLContext::sSERVICE = String("SSLContext");
//TODO :const AutoPtr<IEngine> SSLContext::sENGINE;// = new Engine(SERVICE);
AutoPtr<ISSLContext> SSLContext::sDEFAULT;
Object SSLContext::mLock;;

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

    if (protocol == NULL) {
        //throw new NullPointerException("protocol == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    assert(0 && "TODO");
    // Engine.SpiAndProvider sap = ENGINE.getInstance(protocol, null);
    // return new SSLContext((SSLContextSpi) sap.spi, sap.provider, protocol);
    return NOERROR;
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
    assert(0 && "TODO");
    //TODO: Security::GetProvider(provider, (IProvider**)&impProvider);
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
    assert(0 && "TODO");
    // Object spi = ENGINE.getInstance(protocol, provider, null);
    // return new SSLContext((SSLContextSpi) spi, provider, protocol);
    return NOERROR;
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
