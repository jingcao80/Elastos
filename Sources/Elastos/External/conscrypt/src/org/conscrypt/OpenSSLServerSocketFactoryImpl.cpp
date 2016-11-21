
#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "OpenSSLServerSocketFactoryImpl.h"
#include "SSLParametersImpl.h"
#include "NativeCrypto.h"

using Elastosx::Net::Ssl::EIID_ISSLServerSocketFactory;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLServerSocketFactoryImpl::
//=========================================
CAR_INTERFACE_IMPL_2(OpenSSLServerSocketFactoryImpl, Object, ISSLServerSocketFactory, IOpenSSLServerSocketFactoryImpl)

ECode OpenSSLServerSocketFactoryImpl::constructor()
{
    SSLParametersImpl::GetDefault((ISSLParametersImpl**)&mSslParameters);
    mSslParameters->SetUseClientMode(FALSE);
    return NOERROR;
}

ECode OpenSSLServerSocketFactoryImpl::constructor(
    /* [in] */ ISSLParametersImpl* sslParameters)
{
    AutoPtr<IInterface> p;
    ICloneable::Probe(sslParameters)->Clone((IInterface**)&p);
    mSslParameters = ISSLParametersImpl::Probe(p);
    mSslParameters->SetUseClientMode(FALSE);
    return NOERROR;
}

ECode OpenSSLServerSocketFactoryImpl::GetDefaultCipherSuites(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetEnabledCipherSuites(result);
}

ECode OpenSSLServerSocketFactoryImpl::GetSupportedCipherSuites(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::GetSupportedCipherSuites(result);
}

ECode OpenSSLServerSocketFactoryImpl::CreateServerSocket(
    /* [out] */ IServerSocket** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLServerSocketImpl> res;
    assert(0 && "TODO");
    // new OpenSSLServerSocketImpl((SSLParametersImpl) sslParameters.clone());
    *result = IServerSocket::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLServerSocketFactoryImpl::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [out] */ IServerSocket** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLServerSocketImpl> res;
    assert(0 && "TODO");
    // new OpenSSLServerSocketImpl(port, (SSLParametersImpl) sslParameters.clone());
    *result = IServerSocket::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLServerSocketFactoryImpl::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [in] */ Int32 backlog,
    /* [out] */ IServerSocket** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLServerSocketImpl> res;
    assert(0 && "TODO");
    // new OpenSSLServerSocketImpl(port,
    //                            backlog,
    //                            (SSLParametersImpl) sslParameters.clone());
    *result = IServerSocket::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLServerSocketFactoryImpl::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [in] */ Int32 backlog,
    /* [in] */ IInetAddress* iAddress,
    /* [out] */ IServerSocket** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLServerSocketImpl> res;
    assert(0 && "TODO");
    // new OpenSSLServerSocketImpl(port,
    //                            backlog,
    //                            iAddress,
    //                            (SSLParametersImpl) sslParameters.clone());
    *result = IServerSocket::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
