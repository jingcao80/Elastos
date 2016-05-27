
#include "CSSLEngineImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(CSSLEngineImpl)

ECode CSSLEngineImpl::GetPeerHost(
    /* [out] */ String * pHost)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::GetPeerPort(
    /* [out] */ Int32 * pPort)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::BeginHandshake()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::CloseInbound()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::CloseOutbound()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::GetDelegatedTask(
    /* [out] */ Elastos::Core::IRunnable ** ppTask)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::GetEnabledCipherSuites(
    /* [out, callee] */ ArrayOf<String> ** ppSuites)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::GetEnabledProtocols(
    /* [out, callee] */ ArrayOf<String> ** ppProtocols)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::GetEnableSessionCreation(
    /* [out] */ Boolean * pEnabled)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::GetHandshakeStatus(
    /* [out] */ Elastosx::Net::Ssl::SSLEngineResult_HandshakeStatus * pStatus)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::GetNeedClientAuth(
    /* [out] */ Boolean * pAuth)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::GetSession(
    /* [out] */ Elastosx::Net::Ssl::ISSLSession ** ppSession)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::GetSupportedCipherSuites(
    /* [out, callee] */ ArrayOf<String> ** ppSuites)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::GetSupportedProtocols(
    /* [out, callee] */ ArrayOf<String> ** ppProtocols)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::GetUseClientMode(
    /* [out] */ Boolean * pMode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::GetWantClientAuth(
    /* [out] */ Boolean * pAuth)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::IsInboundDone(
    /* [out] */ Boolean * pDone)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::IsOutboundDone(
    /* [out] */ Boolean * pDone)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::SetEnabledCipherSuites(
    /* [in] */ ArrayOf<String> * pSuites)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::SetEnabledProtocols(
    /* [in] */ ArrayOf<String> * pProtocols)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::SetEnableSessionCreation(
    /* [in] */ Boolean flag)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::SetNeedClientAuth(
    /* [in] */ Boolean need)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::SetUseClientMode(
    /* [in] */ Boolean mode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::SetWantClientAuth(
    /* [in] */ Boolean want)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::Unwrap(
    /* [in] */ Elastos::IO::IByteBuffer * pSrc,
    /* [in] */ ArrayOf<Elastos::IO::IByteBuffer *> * pDsts,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Elastosx::Net::Ssl::ISSLEngineResult ** ppResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::Wrap(
    /* [in] */ ArrayOf<Elastos::IO::IByteBuffer *> * pSrcs,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ Elastos::IO::IByteBuffer * pDst,
    /* [out] */ Elastosx::Net::Ssl::ISSLEngineResult ** ppResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::UnwrapEx(
    /* [in] */ Elastos::IO::IByteBuffer * pSrc,
    /* [in] */ Elastos::IO::IByteBuffer * pDst,
    /* [out] */ Elastosx::Net::Ssl::ISSLEngineResult ** ppResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::UnwrapEx2(
    /* [in] */ Elastos::IO::IByteBuffer * pSrc,
    /* [in] */ ArrayOf<Elastos::IO::IByteBuffer *> * pDsts,
    /* [out] */ Elastosx::Net::Ssl::ISSLEngineResult ** ppResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::WrapEx(
    /* [in] */ ArrayOf<Elastos::IO::IByteBuffer *> * pSrcs,
    /* [in] */ Elastos::IO::IByteBuffer * pDst,
    /* [out] */ Elastosx::Net::Ssl::ISSLEngineResult ** ppResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::WrapEx2(
    /* [in] */ Elastos::IO::IByteBuffer * pSrc,
    /* [in] */ Elastos::IO::IByteBuffer * pDst,
    /* [out] */ Elastosx::Net::Ssl::ISSLEngineResult ** ppResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::GetSSLParameters(
    /* [out] */ Elastosx::Net::Ssl::ISSLParameters ** ppParameters)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSSLEngineImpl::SetSSLParameters(
    /* [in] */ Elastosx::Net::Ssl::ISSLParameters * pP)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

