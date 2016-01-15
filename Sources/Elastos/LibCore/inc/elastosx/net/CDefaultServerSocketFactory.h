
#ifndef __ELASTOSX_NET_CDEFAULTSERVERSOCKETFACTORY_H__
#define __ELASTOSX_NET_CDEFAULTSERVERSOCKETFACTORY_H__

#include "_Elastosx_Net_CDefaultServerSocketFactory.h"
#include "ServerSocketFactory.h"

using Elastos::Net::IInetAddress;
using Elastos::Net::IServerSocket;

namespace Elastosx {
namespace Net {

CarClass(CDefaultServerSocketFactory) , public ServerSocketFactory
{
public:
    CAR_OBJECT_DECL()

    CARAPI CreateServerSocket(
        /* [out] */ IServerSocket** sock);

    /**
     * Creates a new server socket which is bound to the given port with a
     * maximum backlog of 50 unaccepted connections.
     *
     * @param port the port on which the created socket has to listen.
     * @return the created bound server socket.
     * @throws IOException
     *             if an error occurs while creating a new server socket.
     */
    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [out] */ IServerSocket** sock);

    /**
     * Creates a new server socket which is bound to the given port and
     * configures its maximum of queued connections.
     *
     * @param port the port on which the created socket has to listen.
     * @param backlog the maximum number of unaccepted connections. Passing 0 or
     *     a negative value yields the default backlog of 50.
     * @return the created bound server socket.
     * @throws IOException if an error occurs while creating a new server socket.
     */
    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [out] */ IServerSocket** sock);

    /**
     * Creates a new server socket which is bound to the given address on the
     * specified port and configures its maximum of queued connections.
     *
     * @param port the port on which the created socket has to listen.
     * @param backlog the maximum number of unaccepted connections. Passing 0 or
     *     a negative value yields the default backlog of 50.
     * @param iAddress the address of the network interface which is used by the
     *     created socket.
     * @return the created bound server socket.
     * @throws IOException if an error occurs while creating a new server socket.
     */
    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [in] */ IInetAddress* iAddress,
        /* [out] */ IServerSocket** sock);
};

} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_CDEFAULTSERVERSOCKETFACTORY_H__
