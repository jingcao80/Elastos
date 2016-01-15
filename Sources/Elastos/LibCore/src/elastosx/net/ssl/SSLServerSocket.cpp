
#include "SSLServerSocket.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(SSLServerSocket, ServerSocket, ISSLServerSocket)

SSLServerSocket::SSLServerSocket()
{}

ECode SSLServerSocket::constructor()
{
    return NOERROR;
}

ECode SSLServerSocket::constructor(
    /* [in] */ Int32 port)
{
    return ServerSocket::constructor(port);
}

ECode SSLServerSocket::constructor(
    /* [in] */ Int32 port,
    /* [in] */ Int32 backlog)
{
    return ServerSocket::constructor(port, backlog);
}

ECode SSLServerSocket::constructor(
    /* [in] */ Int32 port,
    /* [in] */ Int32 backlog,
    /* [in] */ IInetAddress* address)
{
    return ServerSocket::constructor(port, backlog, address);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
