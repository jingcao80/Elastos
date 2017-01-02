//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOSX_NET_SERVERSOCKETFACTORY_H__
#define __ELASTOSX_NET_SERVERSOCKETFACTORY_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Net::IInetAddress;
using Elastos::Net::IServerSocket;

namespace Elastosx {
namespace Net {

/**
 * This abstract class defines methods to create server sockets. It can be
 * subclassed to create specific server socket types.
 */
class ServerSocketFactory
    : public Object
    , public IServerSocketFactory
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Gets the default server socket factory of the system which can be used to
     * create new server sockets without creating a subclass of this factory.
     *
     * @return the system default server socket factory.
     */
    static CARAPI GetDefault(
        /* [out] */ IServerSocketFactory** factory);

    /**
     * Creates a new server socket which is not bound to any local address. This
     * method has to be overridden by a subclass otherwise a {@code
     * SocketException} is thrown.
     *
     * @return the created unbound server socket.
     * @throws IOException
     *             if an error occurs while creating a new server socket.
     */
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
    virtual CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [out] */ IServerSocket** sock) = 0;

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
    virtual CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [out] */ IServerSocket** sock) = 0;

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
    virtual CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [in] */ IInetAddress* iAddress,
        /* [out] */ IServerSocket** sock) = 0;

protected:
    /**
     * Creates a new {@code ServerSocketFactory} instance.
     */
    ServerSocketFactory();

private:
    static AutoPtr<IServerSocketFactory> sDefaultFactory;

    static Object sLock;
};

} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SERVERSOCKETFACTORY_H__
