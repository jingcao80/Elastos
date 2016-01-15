
#include "DefaultSSLServerSocketFactory.h"
#include "EmptyArray.h"

using Libcore::Utility::EmptyArray;

namespace Elastosx {
namespace Net {
namespace Ssl {

ECode DefaultSSLServerSocketFactory::constructor(
    /* [in] */ const String& mes)
{
    mErrMessage = mes;
    return NOERROR;
}

ECode DefaultSSLServerSocketFactory::GetDefaultCipherSuites(
    /* [out, callee] */ ArrayOf<String>** suites)
{
    VALIDATE_NOT_NULL(suites)

    *suites = EmptyArray::STRING;
    REFCOUNT_ADD(*suites);
    return NOERROR;
}

ECode DefaultSSLServerSocketFactory::GetSupportedCipherSuites(
    /* [out, callee] */ ArrayOf<String>** suites)
{
    VALIDATE_NOT_NULL(suites)

    *suites = EmptyArray::STRING;
    REFCOUNT_ADD(*suites);
    return NOERROR;
}

ECode DefaultSSLServerSocketFactory::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [out] */ IServerSocket** sock)
{
    VALIDATE_NOT_NULL(sock)

    //throw new SocketException(errMessage);
    return E_SOCKET_EXCEPTION;
}

ECode DefaultSSLServerSocketFactory::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [in] */ Int32 backlog,
    /* [out] */ IServerSocket** sock)
{
    VALIDATE_NOT_NULL(sock)

    //throw new SocketException(errMessage);
    return E_SOCKET_EXCEPTION;
}

ECode DefaultSSLServerSocketFactory::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [in] */ Int32 backlog,
    /* [in] */ IInetAddress* iAddress,
    /* [out] */ IServerSocket** sock)
{
    VALIDATE_NOT_NULL(sock)

    //throw new SocketException(errMessage);
    return E_SOCKET_EXCEPTION;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
