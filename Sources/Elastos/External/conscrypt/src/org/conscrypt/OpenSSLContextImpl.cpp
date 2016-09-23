#include "org/conscrypt/OpenSSLContextImpl.h"
#include <elastos/core/AutoLock.h>

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(OpenSSLContextImpl, SSLContextSpi, IOpenSSLContextImpl)

AutoPtr<IDefaultSSLContextImpl> OpenSSLContextImpl::DEFAULT_SSL_CONTEXT_IMPL;

ECode OpenSSLContextImpl::constructor()
{
// TODO: Need CClientSessionContext
    // CClientSessionContext::New((IClientSessionContext**)&clientSessionContext);
// TODO: Need CServerSessionContext
    // CServerSessionContext::New((IServerSessionContext**)&serverSessionContext);
    return NOERROR;
}

ECode OpenSSLContextImpl::constructor(
    /* [in] */ IDefaultSSLContextImpl* dummy)
{
    AutoLock lock(DEFAULT_SSL_CONTEXT_IMPL);    // synchronized (DefaultSSLContextImpl.class);
    if (DEFAULT_SSL_CONTEXT_IMPL == NULL) {
// TODO: Need CClientSessionContext
        // CClientSessionContext::New((IClientSessionContext**)&clientSessionContext);
// TODO: Need CServerSessionContext
        // CServerSessionContext::New((IServerSessionContext**)&serverSessionContext);
        DEFAULT_SSL_CONTEXT_IMPL = IDefaultSSLContextImpl::Probe(this);
    }
    else {
        ISSLContextSpi::Probe(DEFAULT_SSL_CONTEXT_IMPL)->EngineGetClientSessionContext(
            (ISSLSessionContext**)&clientSessionContext);
        ISSLContextSpi::Probe(DEFAULT_SSL_CONTEXT_IMPL)->EngineGetServerSessionContext(
            (ISSLSessionContext**)&serverSessionContext);
    }

    AutoPtr<ArrayOf<IKeyManager*> > keyManager;
    DEFAULT_SSL_CONTEXT_IMPL->GetKeyManagers(
            (ArrayOf<IKeyManager*>**)&keyManager);
    AutoPtr<ArrayOf<ITrustManager*> > trustManager;
    DEFAULT_SSL_CONTEXT_IMPL->GetTrustManagers(
            (ArrayOf<ITrustManager*>**)&trustManager);

// TODO: Need CSSLParametersImpl
    // CSSLParametersImpl::New(
    //         keyManager,
    //         trustManager,
    //         NULL,
    //         clientSessionContext,
    //         serverSessionContext,
    //         (ISSLParametersImpl**)&sslParameters);
    return NOERROR;
}

ECode OpenSSLContextImpl::EngineInit(
    /* [in] */ ArrayOf<IKeyManager*>* kms,
    /* [in] */ ArrayOf<ITrustManager*>* tms,
    /* [in] */ ISecureRandom* sr)
{
// TODO: Need CSSLParametersImpl
    // return CSSLParametersImpl::New(
    //         kms, tms, sr, clientSessionContext, serverSessionContext,
    //         (ISSLParametersImpl**)&sslParameters);
    return NOERROR;
}

ECode OpenSSLContextImpl::EngineGetSocketFactory(
    /* [out] */ ISSLSocketFactory** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (sslParameters == NULL) {
        // throw new IllegalStateException("SSLContext is not initialized.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
// TODO: Need COpenSSLSocketFactoryImpl
    // return COpenSSLSocketFactoryImpl::New(sslParameters, result);
    return NOERROR;
}

ECode OpenSSLContextImpl::EngineGetServerSocketFactory(
    /* [out] */ ISSLServerSocketFactory** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (sslParameters == NULL) {
        // throw new IllegalStateException("SSLContext is not initialized.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
// TODO: Need COpenSSLServerSocketFactoryImpl
    // return COpenSSLServerSocketFactoryImpl::New(sslParameters, result);
    return NOERROR;
}

ECode OpenSSLContextImpl::EngineCreateSSLEngine(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ ISSLEngine** result)
{
    VALIDATE_NOT_NULL(result)
    if (sslParameters == NULL) {
        // throw new IllegalStateException("SSLContext is not initialized.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    // AutoPtr<ISSLParametersImpl> p = (SSLParametersImpl) sslParameters.clone();
    AutoPtr<ISSLParametersImpl> p = ISSLParametersImpl::Probe(sslParameters);
// TODO: Need SetUseClientMode
    // p->SetUseClientMode(FALSE);
// TODO: Need COpenSSLEngineImpl
    // COpenSSLEngineImpl::New(host, port, p, result);
    return NOERROR;
}

ECode OpenSSLContextImpl::EngineCreateSSLEngine(
    /* [out] */ ISSLEngine** result)
{
    VALIDATE_NOT_NULL(result)
    if (sslParameters == NULL) {
        // throw new IllegalStateException("SSLContext is not initialized.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    // AutoPtr<ISSLParametersImpl> p = (SSLParametersImpl) sslParameters.clone();
    AutoPtr<ISSLParametersImpl> p = ISSLParametersImpl::Probe(sslParameters);
// TODO: Need SetUseClientMode
    // p->SetUseClientMode(FALSE);
// TODO: Need COpenSSLEngineImpl
    // COpenSSLEngineImpl::New(p, result);
    return NOERROR;
}

ECode OpenSSLContextImpl::EngineGetServerSessionContext(
    /* [out] */ ISSLSessionContext** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ISSLSessionContext::Probe(serverSessionContext);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLContextImpl::EngineGetClientSessionContext(
    /* [out] */ ISSLSessionContext** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ISSLSessionContext::Probe(clientSessionContext);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
