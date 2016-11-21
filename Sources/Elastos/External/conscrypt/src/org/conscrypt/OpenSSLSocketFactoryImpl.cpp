
#include "Elastos.CoreLibrary.Net.h"
#include "OpenSSLSocketFactoryImpl.h"
#include "SSLParametersImpl.h"
#include "NativeCrypto.h"
#include "COpenSSLSocketImpl.h"
#include "COpenSSLSocketImplWrapper.h"

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLSocketFactoryImpl::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSocketFactoryImpl, SSLSocketFactory, IOpenSSLSocketFactoryImpl)

ECode OpenSSLSocketFactoryImpl::constructor()
{
    AutoPtr<ISSLParametersImpl> sslParametersLocal;
    // AutoPtr<IIOException> instantiationExceptionLocal;
    // try {
        SSLParametersImpl::GetDefault((ISSLParametersImpl**)&sslParametersLocal);
    // } catch (KeyManagementException e) {
    //     instantiationExceptionLocal = new IOException("Delayed instantiation exception:");
    //     instantiationExceptionLocal.initCause(e);
    // }
    mSslParameters = sslParametersLocal;
    // mInstantiationException = instantiationExceptionLocal;
    return NOERROR;
}

ECode OpenSSLSocketFactoryImpl::constructor(
    /* [in] */ ISSLParametersImpl* sslParameters)
{
    mSslParameters = sslParameters;
    // mInstantiationException = NULL;
    return NOERROR;
}

ECode OpenSSLSocketFactoryImpl::GetDefaultCipherSuites(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetEnabledCipherSuites(result);
}

ECode OpenSSLSocketFactoryImpl::GetSupportedCipherSuites(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::GetSupportedCipherSuites(result);
}

ECode OpenSSLSocketFactoryImpl::CreateSocket(
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result)
    // if (mInstantiationException != NULL) {
    //     // throw instantiationException;
    // }
    AutoPtr<IOpenSSLSocketImpl> res;
    AutoPtr<IInterface> p;
    ICloneable::Probe(mSslParameters)->Clone((IInterface**)&p);
    COpenSSLSocketImpl::New(ISSLParametersImpl::Probe(p), (IOpenSSLSocketImpl**)&res);
    *result = ISocket::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLSocketFactoryImpl::CreateSocket(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> p;
    ICloneable::Probe(mSslParameters)->Clone((IInterface**)&p);
    AutoPtr<IOpenSSLSocketImpl> res;
    COpenSSLSocketImpl::New(host, port, ISSLParametersImpl::Probe(p), (IOpenSSLSocketImpl**)&res);
    *result = ISocket::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLSocketFactoryImpl::CreateSocket(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* localHost,
    /* [in] */ Int32 localPort,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> p;
    ICloneable::Probe(mSslParameters)->Clone((IInterface**)&p);
    AutoPtr<IOpenSSLSocketImpl> res;
    COpenSSLSocketImpl::New(host,
                        port,
                        localHost,
                        localPort,
                        ISSLParametersImpl::Probe(p),
                        (IOpenSSLSocketImpl**)&res);
    *result = ISocket::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLSocketFactoryImpl::CreateSocket(
    /* [in] */ IInetAddress* host,
    /* [in] */ Int32 port,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> p;
    ICloneable::Probe(mSslParameters)->Clone((IInterface**)&p);
    AutoPtr<IOpenSSLSocketImpl> res;
    COpenSSLSocketImpl::New(host, port, ISSLParametersImpl::Probe(p), (IOpenSSLSocketImpl**)&res);
    *result = ISocket::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLSocketFactoryImpl::CreateSocket(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* localAddress,
    /* [in] */ Int32 localPort,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> p;
    ICloneable::Probe(mSslParameters)->Clone((IInterface**)&p);
    AutoPtr<IOpenSSLSocketImpl> res;
    COpenSSLSocketImpl::New(address,
                        port,
                        localAddress,
                        localPort,
                        ISSLParametersImpl::Probe(p),
                        (IOpenSSLSocketImpl**)&res);
    *result = ISocket::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLSocketFactoryImpl::CreateSocket(
    /* [in] */ ISocket* s,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ Boolean autoClose,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> p;
    ICloneable::Probe(mSslParameters)->Clone((IInterface**)&p);
    AutoPtr<IOpenSSLSocketImpl> res;
    COpenSSLSocketImplWrapper::New(s,
                                host,
                                port,
                                autoClose,
                                ISSLParametersImpl::Probe(p),
                                (IOpenSSLSocketImpl**)&res);
    *result = ISocket::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
