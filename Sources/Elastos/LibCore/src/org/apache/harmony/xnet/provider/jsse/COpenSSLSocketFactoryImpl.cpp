
#include "COpenSSLSocketFactoryImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(COpenSSLSocketFactoryImpl)

ECode COpenSSLSocketFactoryImpl::CreateSocket(
    /* [out] */ Elastos::Net::ISocket ** ppSock)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSocketFactoryImpl::CreateSocketEx(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ Elastos::Net::ISocket ** ppSock)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSocketFactoryImpl::CreateSocketEx2(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ Elastos::Net::IInetAddress * pLocalHost,
    /* [in] */ Int32 localPort,
    /* [out] */ Elastos::Net::ISocket ** ppSock)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSocketFactoryImpl::CreateSocketEx3(
    /* [in] */ Elastos::Net::IInetAddress * pHost,
    /* [in] */ Int32 port,
    /* [out] */ Elastos::Net::ISocket ** ppSock)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSocketFactoryImpl::CreateSocketEx4(
    /* [in] */ Elastos::Net::IInetAddress * pAddress,
    /* [in] */ Int32 port,
    /* [in] */ Elastos::Net::IInetAddress * pLocalAddress,
    /* [in] */ Int32 localPort,
    /* [out] */ Elastos::Net::ISocket ** ppSock)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSocketFactoryImpl::GetDefaultCipherSuites(
    /* [out, callee] */ ArrayOf<String> ** ppSuites)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSocketFactoryImpl::GetSupportedCipherSuites(
    /* [out, callee] */ ArrayOf<String> ** ppSuites)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSocketFactoryImpl::CreateSocketEx5(
    /* [in] */ Elastos::Net::ISocket * pS,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ Boolean autoClose,
    /* [out] */ Elastos::Net::ISocket ** ppSock)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSocketFactoryImpl::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSocketFactoryImpl::constructor(
    /* [in] */ Elastosx::Net::Ssl::ISSLParameters * pSslParameters)
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

