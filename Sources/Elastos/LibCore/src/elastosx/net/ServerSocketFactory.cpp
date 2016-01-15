
#include "ServerSocketFactory.h"
#include "CDefaultServerSocketFactory.h"
#include "AutoLock.h"

using Elastosx::Net::CDefaultServerSocketFactory;

namespace Elastosx {
namespace Net {

AutoPtr<IServerSocketFactory> ServerSocketFactory::sDefaultFactory;
Object ServerSocketFactory::sLock;

CAR_INTERFACE_IMPL(ServerSocketFactory, Object, IServerSocketFactory)

ServerSocketFactory::ServerSocketFactory()
{}

ECode ServerSocketFactory::GetDefault(
    /* [out] */ IServerSocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    synchronized(sLock) {
        if (sDefaultFactory == NULL) {
            CDefaultServerSocketFactory::New((IServerSocketFactory**)&sDefaultFactory);
        }
        *factory = sDefaultFactory;
        REFCOUNT_ADD(*factory)
    }
    return NOERROR;
}

ECode ServerSocketFactory::CreateServerSocket(
    /* [out] */ IServerSocket** sock)
{
    VALIDATE_NOT_NULL(sock)

    // follow RI's behavior
    // throw new SocketException("Unbound server sockets not implemented");
    return E_SOCKET_EXCEPTION;
}

} // namespace Net
} // namespace Elastosx

