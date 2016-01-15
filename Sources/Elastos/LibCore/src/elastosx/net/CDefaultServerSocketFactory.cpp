
#include "CDefaultServerSocketFactory.h"
#include "CServerSocket.h"

using Elastos::Net::CServerSocket;

namespace Elastosx {
namespace Net {

CAR_OBJECT_IMPL(CDefaultServerSocketFactory)

ECode CDefaultServerSocketFactory::CreateServerSocket(
    /* [out] */ IServerSocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CServerSocket> ss;
    FAIL_RETURN(CServerSocket::NewByFriend((CServerSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

ECode CDefaultServerSocketFactory::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [out] */ IServerSocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CServerSocket> ss;
    FAIL_RETURN(CServerSocket::NewByFriend(port, (CServerSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

ECode CDefaultServerSocketFactory::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [in] */ Int32 backlog,
    /* [out] */ IServerSocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CServerSocket> ss;
    FAIL_RETURN(CServerSocket::NewByFriend(port, backlog, (CServerSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

ECode CDefaultServerSocketFactory::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [in] */ Int32 backlog,
    /* [in] */ IInetAddress* iAddress,
    /* [out] */ IServerSocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CServerSocket> ss;
    FAIL_RETURN(CServerSocket::NewByFriend(port, backlog, iAddress, (CServerSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

} // namespace Net
} // namespace Elastosx

