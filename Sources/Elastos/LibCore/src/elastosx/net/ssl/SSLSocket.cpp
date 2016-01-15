
#include "SSLSocket.h"
#include "CSSLParameters.h"

using Elastosx::Net::Ssl::CSSLParameters;

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(SSLSocket, Socket, ISSLSocket)

CARAPI SSLSocket::constructor()
{
    return NOERROR;
}

CARAPI SSLSocket::constructor(
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
{
    return Socket::constructor(host, port);
}

CARAPI SSLSocket::constructor(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port)
{
    return Socket::constructor(address, port);
}

CARAPI SSLSocket::constructor(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* clientAddress,
    /* [in] */ Int32 clientPort)
{
    return Socket::constructor(host, port, clientAddress, clientPort);
}

CARAPI SSLSocket::constructor(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* clientAddress,
    /* [in] */ Int32 clientPort)
{
    return Socket::constructor(address, port, clientAddress, clientPort);
}

CARAPI SSLSocket::ShutdownInput()
{
    //throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

CARAPI SSLSocket::ShutdownOutput()
{
    //throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

CARAPI SSLSocket::GetSSLParameters(
    /* [out] */ ISSLParameters** param)
{
    VALIDATE_NOT_NULL(param)

    AutoPtr<ISSLParameters> p;
    CSSLParameters::New((ISSLParameters**)&p);
    AutoPtr<ArrayOf<String> > suites;
    FAIL_RETURN(GetEnabledCipherSuites((ArrayOf<String>**)&suites))
    FAIL_RETURN(p->SetCipherSuites(suites))
    AutoPtr<ArrayOf<String> > protocols;
    FAIL_RETURN(GetEnabledProtocols((ArrayOf<String>**)&protocols))
    FAIL_RETURN(p->SetProtocols(protocols))
    Boolean auth;
    FAIL_RETURN(GetNeedClientAuth(&auth))
    FAIL_RETURN(p->SetNeedClientAuth(auth))
    FAIL_RETURN(GetWantClientAuth(&auth))
    FAIL_RETURN(p->SetWantClientAuth(auth))
    *param = p;
    REFCOUNT_ADD(*param)
    return NOERROR;
}

CARAPI SSLSocket::SetSSLParameters(
    /* [in] */ ISSLParameters* p)
{
    AutoPtr<ArrayOf<String> > cipherSuites;
    FAIL_RETURN(p->GetCipherSuites((ArrayOf<String>**)&cipherSuites))
    if (cipherSuites != NULL) {
        FAIL_RETURN(SetEnabledCipherSuites(cipherSuites))
    }
    AutoPtr<ArrayOf<String> > protocols;
    FAIL_RETURN(p->GetProtocols((ArrayOf<String>**)&protocols))
    if (protocols != NULL) {
        FAIL_RETURN(SetEnabledProtocols(protocols))
    }
    Boolean auth;
    if (p->GetNeedClientAuth(&auth), auth) {
        FAIL_RETURN(SetNeedClientAuth(TRUE))
    } else if (p->GetWantClientAuth(&auth), auth) {
        FAIL_RETURN(SetWantClientAuth(TRUE))
    } else {
        FAIL_RETURN(SetWantClientAuth(FALSE))
    }
    return NOERROR;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
