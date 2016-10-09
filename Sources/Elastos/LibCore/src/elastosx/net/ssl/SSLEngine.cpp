
#include "_Elastos.CoreLibrary.IO.h"
#include "SSLEngine.h"
#include "CSSLParameters.h"

using Elastosx::Net::Ssl::CSSLParameters;

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(SSLEngine, Object, ISSLEngine)

SSLEngine::SSLEngine()
    : mPeerHost(String(NULL))
    , mPeerPort(-1)
{}

ECode SSLEngine::constructor(
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
{
    mPeerHost = host;
    mPeerPort = port;
    return NOERROR;
}

ECode SSLEngine::GetPeerHost(
    /* [out] */ String* host)
{
    VALIDATE_NOT_NULL(host)

    *host = mPeerHost;
    return NOERROR;
}

ECode SSLEngine::GetPeerPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port)

    *port = mPeerPort;
    return NOERROR;
}

ECode SSLEngine::Unwrap(
    /* [in] */ IByteBuffer* src,
    /* [in] */ IByteBuffer* dst,
    /* [out] */ ISSLEngineResult** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<IByteBuffer*> > buffer = ArrayOf<IByteBuffer*>::Alloc(1);
    buffer->Set(0, dst);
    return Unwrap(src, (ArrayOf<IByteBuffer*>*)buffer, 0, 1, result);
}

ECode SSLEngine::Unwrap(
    /* [in] */ IByteBuffer* src,
    /* [in] */ ArrayOf<IByteBuffer*>* dsts,
    /* [out] */ ISSLEngineResult** result)
{
    VALIDATE_NOT_NULL(result)

    if (dsts == NULL) {
        //throw new IllegalArgumentException("Byte buffer array dsts is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return Unwrap(src, dsts, 0, dsts->GetLength(), result);
}

ECode SSLEngine::Wrap(
    /* [in] */ ArrayOf<IByteBuffer*>* srcs,
    /* [in] */ IByteBuffer* dst,
    /* [out] */ ISSLEngineResult** result)
{
    VALIDATE_NOT_NULL(result)

    if (srcs == NULL) {
        //throw new IllegalArgumentException("Byte buffer array srcs is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return Wrap(srcs, 0, srcs->GetLength(), dst, result);
}

ECode SSLEngine::Wrap(
    /* [in] */ IByteBuffer* src,
    /* [in] */ IByteBuffer* dst,
    /* [out] */ ISSLEngineResult** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<IByteBuffer*> > buffer = ArrayOf<IByteBuffer*>::Alloc(1);
    buffer->Set(0, src);
    return Wrap((ArrayOf<IByteBuffer*>*)buffer, 0, 1, dst, result);
}

ECode SSLEngine::GetSSLParameters(
    /* [out] */ ISSLParameters** parameters)
{
    VALIDATE_NOT_NULL(parameters)

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
    *parameters = p;
    REFCOUNT_ADD(*parameters);
    return NOERROR;
}

ECode SSLEngine::SetSSLParameters(
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
    }
    else if (p->GetWantClientAuth(&auth), auth) {
        FAIL_RETURN(SetWantClientAuth(TRUE))
    }
    else {
        FAIL_RETURN(SetWantClientAuth(FALSE))
    }
    return NOERROR;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
