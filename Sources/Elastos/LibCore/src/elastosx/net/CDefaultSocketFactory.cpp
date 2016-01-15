
#include "CDefaultSocketFactory.h"
#include "CSocket.h"

using Elastos::Net::CSocket;

namespace Elastosx {
namespace Net {

CAR_OBJECT_IMPL(CDefaultSocketFactory)

ECode CDefaultSocketFactory::constructor()
{
    return NOERROR;
}

ECode CDefaultSocketFactory::CreateSocket(
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CSocket> ss;
    FAIL_RETURN(CSocket::NewByFriend((CSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

ECode CDefaultSocketFactory::CreateSocket(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CSocket> ss;
    FAIL_RETURN(CSocket::NewByFriend(host, port, (CSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

ECode CDefaultSocketFactory::CreateSocket(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* localHost,
    /* [in] */ Int32 localPort,
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CSocket> ss;
    FAIL_RETURN(CSocket::NewByFriend(host, port, localHost, localPort, (CSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

ECode CDefaultSocketFactory::CreateSocket(
    /* [in] */ IInetAddress* host,
    /* [in] */ Int32 port,
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CSocket> ss;
    FAIL_RETURN(CSocket::NewByFriend(host, port, (CSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

ECode CDefaultSocketFactory::CreateSocket(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* localAddress,
    /* [in] */ Int32 localPort,
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CSocket> ss;
    FAIL_RETURN(CSocket::NewByFriend(address, port, localAddress, localPort, (CSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

} // namespace Net
} // namespace Elastosx

