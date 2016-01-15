#ifndef __ELASTOS_IO_DATAGRAMCHANNELIMPL_H__
#define __ELASTOS_IO_DATAGRAMCHANNELIMPL_H__

#include "DatagramChannel.h"
#include "DatagramSocket.h"

using Elastos::IO::Channels::DatagramChannel;
using Elastos::IO::IFileDescriptorChannel;
using Elastos::IO::IFileDescriptor;
using Elastos::Net::IDatagramSocket;
using Elastos::Net::ISocketAddress;
using Elastos::Net::IInetSocketAddress;
using Elastos::Net::IDatagramPacket;
using Elastos::Net::DatagramSocket;
using Elastos::Net::IDatagramSocketImpl;
using Elastos::Net::INetworkInterface;

namespace Elastos {
namespace IO {

class DatagramChannelImpl
    : public DatagramChannel
    , public IFileDescriptorChannel
{
private:
    class DatagramSocketAdapter
        : public DatagramSocket
    {
    public:
        DatagramSocketAdapter();

        CARAPI constructor(
            /* [in] */ IDatagramSocketImpl* socketimpl,
            /* [in] */ DatagramChannelImpl* channelimpl);

        CARAPI Bind(
            /* [in] */ ISocketAddress* localAddr);

        CARAPI GetChannel(
            /* [out] */ IDatagramChannel** channel);

        CARAPI Connect(
            /* [in] */ ISocketAddress* peer);

        CARAPI Connect(
            /* [in] */ IInetAddress* address,
            /* [in] */ Int32 aPort);

        CARAPI Receive(
            /* [in] */ IDatagramPacket* pack);

        CARAPI Send(
            /* [in] */ IDatagramPacket* pack);

        CARAPI Close();

        CARAPI Disconnect();

    private:
        AutoPtr<DatagramChannelImpl> mChannelImpl;
    };

public:
    /*
     * Constructor
     */
    DatagramChannelImpl(
        /* [in] */ ISelectorProvider* provider);

    CAR_INTERFACE_DECL()

    DatagramChannelImpl();

    CARAPI constructor();

    /*
     * Constructor
     */
    CARAPI constructor(
        /* [in] */ ISelectorProvider* provider);

    /*
     * Getting the internal DatagramSocket If we have not the socket, we create
     * a new one.
     */
    CARAPI GetSocket(
        /* [out] */ IDatagramSocket** socket);

     /**
      * @see java.nio.channels.DatagramChannel#isConnected()
      */
    CARAPI IsConnected(
        /* [out] */ Boolean* value);

    /**
    * @see java.nio.channels.DatagramChannel#connect(java.net.SocketAddress)
    */
    CARAPI Connect(
        /* [in] */ ISocketAddress* address,
        /* [out] */ IDatagramChannel** channel);

    /**
     * Initialize the state associated with being connected, optionally syncing the socket if there
     * is one.
     * @hide used to sync state, non-private to avoid synthetic method
     */
    CARAPI OnConnect(
        /* [in] */ IInetAddress* remoteAddress,
        /* [in] */ Int32 remotePort,
        /* [in] */ Boolean updateSocketState);

    /**
    * @see java.nio.channels.DatagramChannel#disconnect()
    */
    CARAPI Disconnect();

    virtual CARAPI Receive(
        /* [in] */ IByteBuffer* target,
        /* [in] */ ISocketAddress** address);

    CARAPI Send(
        /* [in] */ IByteBuffer* target,
        /* [in] */ ISocketAddress* address,
        /* [out] */ Int32 * count);

    CARAPI Read(
        /* [in] */ IByteBuffer* target,
        /* [out] */ Int32 * count);

    CARAPI Read(
        /* [in] */ ArrayOf<IByteBuffer*> * targets,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int64* count);

    /*
     * read from channel, and store the result in the target.
     */
    CARAPI ReadImpl(
        /* [in] */ IByteBuffer* target,
        /* [out] */ Int32 * count);

    CARAPI Write(
        /* [in] */ IByteBuffer* src,
        /* [out] */ Int32 * count);

    CARAPI Write(
        /* [in] */ ArrayOf<IByteBuffer*> * sources,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int64* count);

    CARAPI WriteImpl(
        /* [in] */ IByteBuffer* src,
        /* [out] */ Int32 * count);

    CARAPI GetFD(
        /* [out] */ IFileDescriptor** outfd);

protected:
    /**
     * Returns the local address to which the socket is bound.
     */
    CARAPI GetLocalAddress(
        /* [out] */ IInetAddress** addr);

    /**
     * Initialize the state associated with being disconnected, optionally syncing the socket if
     * there is one.
     * @hide used to sync state, non-private to avoid synthetic method
     */
    CARAPI OnDisconnect(
        /* [in] */ Boolean updateSocketState);

    CARAPI ImplCloseSelectableChannel();

    CARAPI ImplConfigureBlocking(
        /* [in] */ Boolean blocking);

    /**
     * Initialise the isBound, localAddress and localPort state from the file descriptor. Used when
     * some or all of the bound state has been left to the OS to decide, or when the Socket handled
     * bind() or connect().
     *
     * @param updateSocketState
     *        if the associated socket (if present) needs to be updated
     * @hide used to sync state, non-private to avoid synthetic method
     */
    CARAPI OnBind(
        /* [in] */ Boolean updateSocketState);

private:

    CARAPI ReceiveImpl(
        /* [in] */ IByteBuffer* target,
        /* [in] */ Boolean loop,
        /* [out] */ ISocketAddress** addr);

    CARAPI ReceiveDirectImpl(
        /* [in] */ IByteBuffer* target,
        /* [in] */ Boolean loop,
        /* [out] */ ISocketAddress** addr);

    CARAPI CheckOpen();

    /*
     * Status check, must be open and connected, for read and write.
     */
    CARAPI CheckOpenConnected();

    /*
     * Buffer check, must not null
     */
    CARAPI CheckNotNull(
        /* [in] */ IByteBuffer* buffer);

protected:
    // The address to be connected.
    AutoPtr<IInetSocketAddress> mConnectAddress;

    // At first, uninitialized.
    Boolean mConnected;

    // whether the socket is bound
    Boolean mIsBound;

    AutoPtr<IInetAddress> mLocalAddress;

private:
    // The fd to interact with native code
    AutoPtr<IFileDescriptor> mFd;

    // Our internal DatagramSocket.
    AutoPtr<IDatagramSocket> mSocket;

    // local port
    Int32 mLocalPort;

    Object mReadLock;
    Object mWriteLock;
    AutoPtr<ISelectorProvider> mSelprovider;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_DATAGRAMCHANNELIMPL_H__
