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

#ifndef __ELASTOS_NET_DATAGRAMSOCKETIMPL_H__
#define __ELASTOS_NET_DATAGRAMSOCKETIMPL_H__

#include "Elastos.CoreLibrary.Net.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Net {

class DatagramSocketImpl
    : public Object
    , public IDatagramSocketImpl
    , public ISocketOptions
{
public:
    CAR_INTERFACE_DECL()

    DatagramSocketImpl();

    /**
     * Binds the datagram socket to the given localhost/port. Sockets must be
     * bound prior to attempting to send or receive data.
     *
     * @param port
     *            the port on the localhost to bind.
     * @param addr
     *            the address on the multihomed localhost to bind.
     * @throws SocketException
     *                if an error occurs while binding, for example, if the port
     *                has been already bound.
     */
    virtual CARAPI Bind(
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* addr) = 0;

    /**
     * Closes this socket.
     */
    virtual CARAPI Close() = 0;

    /**
     * This method allocates the socket descriptor in the underlying operating
     * system.
     *
     * @throws SocketException
     *             if an error occurs while creating the socket.
     */
    virtual CARAPI Create() = 0;

    /**
     * Gets the {@code FileDescriptor} of this datagram socket, which is invalid
     * if the socket is closed or not bound.
     *
     * @return the current file descriptor of this socket.
     */
    virtual CARAPI GetFileDescriptor(
        /* [out] */ IFileDescriptor** fileDescriptor);

    /**
     * Returns the local port to which this socket is bound.
     */
    virtual CARAPI GetLocalPort(
        /* [out] */ Int32* port);

    /**
     * Gets the time-to-live (TTL) for multicast packets sent on this socket.
     *
     * @return the time-to-live option as a byte value.
     * @throws IOException
     *             if an error occurs while getting the time-to-live option
     *             value.
     * @deprecated Use {@link #getTimeToLive} instead.
     * @see #getTimeToLive()
     */
    virtual CARAPI GetTTL(
        /* [out] */ Byte* ttl) = 0;

    /**
     * Gets the time-to-live (TTL) for multicast packets sent on this socket.
     * The TTL option defines how many routers a packet may be pass before it is
     * discarded.
     *
     * @return the time-to-live option as an integer value.
     * @throws IOException
     *             if an error occurs while getting the time-to-live option
     *             value.
     */
    virtual CARAPI GetTimeToLive(
        /* [out] */ Int32* option) = 0;

    /**
     * Adds this socket to the multicast group {@code addr}. A socket must join
     * a group before being able to receive data. Further, a socket may be a
     * member of multiple groups but may join any group only once.
     *
     * @param addr
     *            the multicast group to which this socket has to be joined.
     * @throws IOException
     *             if an error occurs while joining the specified multicast
     *             group.
     */
    virtual CARAPI Join(
        /* [in] */ IInetAddress* addr) = 0;

    /**
     * Adds this socket to the multicast group {@code addr}. A socket must join
     * a group before being able to receive data. Further, a socket may be a
     * member of multiple groups but may join any group only once.
     *
     * @param addr
     *            the multicast group to which this socket has to be joined.
     * @param netInterface
     *            the local network interface which will receive the multicast
     *            datagram packets.
     * @throws IOException
     *             if an error occurs while joining the specified multicast
     *             group.
     */
    virtual CARAPI JoinGroup(
        /* [in] */ ISocketAddress* addr,
        /* [in] */ INetworkInterface* netInterface) = 0;

    /**
     * Removes this socket from the multicast group {@code addr}.
     *
     * @param addr
     *            the multicast group to be left.
     * @throws IOException
     *             if an error occurs while leaving the group or no multicast
     *             address was assigned.
     */
    virtual CARAPI Leave(
        /* [in] */ IInetAddress* addr) = 0;

    /**
     * Removes this socket from the multicast group {@code addr}.
     *
     * @param addr
     *            the multicast group to be left.
     * @param netInterface
     *            the local network interface on which this socket has to be
     *            removed.
     * @throws IOException
     *             if an error occurs while leaving the group.
     */
    virtual CARAPI LeaveGroup(
        /* [in] */ ISocketAddress* addr,
        /* [in] */ INetworkInterface* netInterface) = 0;

    /**
     * Peeks at the incoming packet to this socket and returns the address of
     * the {@code sender}. The method will block until a packet is received or
     * timeout expires.
     *
     * @param sender
     *            the origin address of a packet.
     * @return the address of {@code sender} as an integer value.
     * @throws IOException
     *                if an error or a timeout occurs while reading the address.
     */
    virtual CARAPI Peek(
        /* [in] */ IInetAddress* sender,
        /* [out] */ Int32* addr) = 0;

    /**
     * Receives data and stores it in the supplied datagram packet {@code pack}.
     * This call will block until either data has been received or, if a timeout
     * is set, the timeout has expired. If the timeout expires an {@code
     * InterruptedIOException} is thrown.
     *
     * @param pack
     *            the datagram packet container to fill in the received data.
     * @throws IOException
     *                if an error or timeout occurs while receiving data.
     */
    virtual CARAPI Receive(
        /* [in] */ IDatagramPacket* pack) = 0;

    /**
     * Sends the given datagram packet {@code pack}. The packet contains the
     * data and the address and port information of the target host as well.
     *
     * @param pack
     *            the datagram packet to be sent.
     * @throws IOException
     *                if an error occurs while sending the packet.
     */
    virtual CARAPI Send(
        /* [in] */ IDatagramPacket* pack) = 0;

    /**
     * Sets the time-to-live (TTL) option for multicast packets sent on this
     * socket.
     *
     * @param ttl
     *            the time-to-live option value. Valid values are 0 &lt; ttl
     *            &lt;= 255.
     * @throws IOException
     *             if an error occurs while setting the option.
     */
    virtual CARAPI SetTimeToLive(
        /* [in] */ Int32 ttl) = 0;

    /**
     * Sets the time-to-live (TTL) option for multicast packets sent on this
     * socket.
     *
     * @param ttl
     *            the time-to-live option value. Valid values are 0 &lt; ttl
     *            &lt;= 255.
     * @throws IOException
     *             if an error occurs while setting the option.
     * @deprecated Use {@link #setTimeToLive} instead.
     * @see #setTimeToLive(int)
     */
    virtual CARAPI SetTTL(
        /* [in] */ Byte ttl) = 0;

    /**
     * Connects this socket to the specified remote address and port.
     *
     * @param inetAddr
     *            the address of the target host which has to be connected.
     * @param port
     *            the port on the target host which has to be connected.
     * @throws SocketException
     *                if the datagram socket cannot be connected to the
     *                specified remote address and port.
     */
    virtual CARAPI Connect(
        /* [in] */ IInetAddress* inetAddr,
        /* [in] */ Int32 port);

    /**
     * Disconnects this socket from the remote host.
     */
    virtual CARAPI Disconnect();

    /**
     * Receives data into the supplied datagram packet by peeking. The data is
     * not removed from socket buffer and can be received again by another
     * {@code peekData()} or {@code receive()} call. This call blocks until
     * either data has been received or, if a timeout is set, the timeout has
     * been expired.
     *
     * @param pack
     *            the datagram packet used to store the data.
     * @return the port the packet was received from.
     * @throws IOException
     *                if an error occurs while peeking at the data.
     */
    virtual CARAPI PeekData(
        /* [in] */ IDatagramPacket* pack,
        /* [out] */ Int32* localPort) = 0;

    /**
     * Initialize the bind() state.
     * @hide used in java.nio.
     */
    virtual CARAPI OnBind(
        /* [in] */ IInetAddress* inetAddr,
        /* [in] */ Int32 port)
    {
        // Do not add any code to these methods. They are concrete only to preserve API
        // compatibility.
        return NOERROR;
    }

    /**
     * Initialize the connect() state.
     * @hide used in java.nio.
     */
    virtual CARAPI OnConnect(
        /* [in] */ IInetAddress* inetAddr,
        /* [in] */ Int32 port)
    {
        // Do not add any code to these methods. They are concrete only to preserve API
        // compatibility.
        return NOERROR;
    }

    /**
     * Initialize the disconnected state.
     * @hide used in java.nio.
     */
    virtual CARAPI OnDisConnect()
    {
        // Do not add any code to these methods. They are concrete only to preserve API
        // compatibility.
        return NOERROR;
    }

    /**
     * Initialize the closed state.
     * @hide used in java.nio.
     */
    virtual CARAPI OnClose()
    {
        // Do not add any code to these methods. They are concrete only to preserve API
        // compatibility.
        return NOERROR;
    }

protected:
    /**
     * File descriptor that is used to address this socket.
     */
    AutoPtr<IFileDescriptor> mFd;

    /**
     * The number of the local port to which this socket is bound.
     */
    Int32 mLocalPort;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_DATAGRAMSOCKETIMPL_H__
