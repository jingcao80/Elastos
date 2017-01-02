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

#ifndef __ELASTOSX_NET_CDEFAULTSOCKETFACTORY_H__
#define __ELASTOSX_NET_CDEFAULTSOCKETFACTORY_H__

#include "_Elastosx_Net_CDefaultSocketFactory.h"
#include "SocketFactory.h"

using Elastos::Net::ISocket;
using Elastos::Net::IInetAddress;

namespace Elastosx {
namespace Net {

/**
 * Default implementation of {@link javax.net.SocketFactory}
 */
CarClass(CDefaultSocketFactory) , public SocketFactory
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI CreateSocket(
        /* [out] */ ISocket** sock);

    /**
     * Creates a new socket which is connected to the remote host specified by
     * the parameters {@code host} and {@code port}. The socket is bound to any
     * available local address and port.
     *
     * @param host
     *            the remote host address the socket has to be connected to.
     * @param port
     *            the port number of the remote host at which the socket is
     *            connected.
     * @return the created connected socket.
     * @throws IOException
     *             if an error occurs while creating a new socket.
     * @throws UnknownHostException
     *             if the specified host is unknown or the IP address could not
     *             be resolved.
     */
    CARAPI CreateSocket(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [out] */ ISocket** sock);

    /**
     * Creates a new socket which is connected to the remote host specified by
     * the parameters {@code host} and {@code port}. The socket is bound to the
     * local network interface specified by the InetAddress {@code localHost} on
     * port {@code localPort}.
     *
     * @param host
     *            the remote host address the socket has to be connected to.
     * @param port
     *            the port number of the remote host at which the socket is
     *            connected.
     * @param localHost
     *            the local host address the socket is bound to.
     * @param localPort
     *            the port number of the local host at which the socket is
     *            bound.
     * @return the created connected socket.
     * @throws IOException
     *             if an error occurs while creating a new socket.
     * @throws UnknownHostException
     *             if the specified host is unknown or the IP address could not
     *             be resolved.
     */
    CARAPI CreateSocket(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* localHost,
        /* [in] */ Int32 localPort,
        /* [out] */ ISocket** sock);

    /**
     * Creates a new socket which is connected to the remote host specified by
     * the InetAddress {@code host}. The socket is bound to any available local
     * address and port.
     *
     * @param host
     *            the host address the socket has to be connected to.
     * @param port
     *            the port number of the remote host at which the socket is
     *            connected.
     * @return the created connected socket.
     * @throws IOException
     *             if an error occurs while creating a new socket.
     */
    CARAPI CreateSocket(
        /* [in] */ IInetAddress* host,
        /* [in] */ Int32 port,
        /* [out] */ ISocket** sock);

    /**
     * Creates a new socket which is connected to the remote host specified by
     * the InetAddress {@code address}. The socket is bound to the local network
     * interface specified by the InetAddress {@code localHost} on port {@code
     * localPort}.
     *
     * @param address
     *            the remote host address the socket has to be connected to.
     * @param port
     *            the port number of the remote host at which the socket is
     *            connected.
     * @param localAddress
     *            the local host address the socket is bound to.
     * @param localPort
     *            the port number of the local host at which the socket is
     *            bound.
     * @return the created connected socket.
     * @throws IOException
     *             if an error occurs while creating a new socket.
     */
    CARAPI CreateSocket(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* localAddress,
        /* [in] */ Int32 localPort,
        /* [out] */ ISocket** sock);
};

} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_CDEFAULTSOCKETFACTORY_H__
