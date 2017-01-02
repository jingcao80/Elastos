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

#ifndef __ELASTOS_IO_CHANNELS_SOCKET_CHANNEL_H__
#define __ELASTOS_IO_CHANNELS_SOCKET_CHANNEL_H__

#include "AbstractSelectableChannel.h"

using Elastos::IO::IByteBuffer;
using Elastos::IO::Channels::IScatteringByteChannel;
using Elastos::IO::Channels::IGatheringByteChannel;
using Elastos::IO::Channels::Spi::AbstractSelectableChannel;
using Elastos::Net::ISocket;
using Elastos::Net::ISocketAddress;

namespace Elastos {
namespace IO {
namespace Channels {

class SocketChannel
    : public AbstractSelectableChannel
    , public IByteChannel
    , public IScatteringByteChannel
    , public IGatheringByteChannel
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates an open and unconnected socket channel.
     * <p>
     * This channel is created by calling {@code openSocketChannel()} of the
     * default {@link SelectorProvider} instance.
     *
     * @return the new channel which is open but unconnected.
     * @throws IOException
     *             if an I/O error occurs.
     */
    CARAPI static Open(
        /* [out] */ ISocketChannel** channel);

    /**
     * Creates a socket channel and connects it to a socket address.
     * <p>
     * This method performs a call to {@code open()} followed by a call to
     * {@code connect(SocketAddress)}.
     *
     * @param address
     *            the socket address to be connected to.
     * @return the new connected channel.
     * @throws AsynchronousCloseException
     *             if this channel is closed by another thread while this method
     *             is executing.
     * @throws ClosedByInterruptException
     *             if another thread interrupts the calling thread while this
     *             operation is executing. The calling thread will have the
     *             interrupt state set and the channel will be closed.
     * @throws UnresolvedAddressException
     *             if the address is not resolved.
     * @throws UnsupportedAddressTypeException
     *             if the address type is not supported.
     * @throws IOException
     *             if an I/O error occurs.
     */
    CARAPI static Open(
        /* [in] */ ISocketAddress* addr,
        /* [out] */ ISocketChannel** channel);

    /**
     * Gets the valid operations of this channel. Socket channels support
     * connect, read and write operation, so this method returns
     * {@code SelectionKey.OP_CONNECT | SelectionKey.OP_READ | SelectionKey.OP_WRITE}.
     *
     * @return the operations supported by this channel.
     * @see java.nio.channels.SelectableChannel#validOps()
     */
    CARAPI GetValidOps(
        /* [out] */ Int32* ret);

    /**
     * Returns the socket assigned to this channel, which does not declare any public
     * methods that are not declared in {@code Socket}.
     *
     * @return the socket assigned to this channel.
     */
    virtual CARAPI GetSocket(
        /* [out] */ ISocket** serverSocket) = 0;


    /**
     * Indicates whether this channel's socket is connected.
     *
     * @return {true} if this channel's socket is connected, {false}
     *         otherwise.
     */
    virtual CARAPI IsConnected(
        /* [out] */ Boolean* isConnected) = 0;

    /**
     * Indicates whether this channel's socket is still trying to connect.
     *
     * @return {true} if the connection is initiated but not finished;
     *         {false} otherwise.
     */
    virtual CARAPI IsConnectionPending(
        /* [out] */ Boolean* isConnectionPending) = 0;

    /**
     * Connects this channel's socket with a remote address.
     * <p>
     * If this channel is blocking, this method will suspend until connecting is
     * finished or an I/O exception occurs. If the channel is non-blocking,
     * this method will return {true} if the connection is finished at
     * once or return {false} when the connection must be finished later
     * by calling {finishConnect()}.
     * <p>
     * This method can be called at any moment and can block other read and
     * write operations while connecting. It executes the same security checks
     * as the connect method of the {Socket} class.
     *
     * @param address
     *            the address to connect with.
     * @return {true} if the connection is finished, {false}
     *         otherwise.
     * @return CARAPI = NOERROR(0) if success, elas as follows:
     * Value | Meaning |
     * :-|:------------|
     * AlreadyConnectedException | if the channel is already connected.
     * ConnectionPendingException
     *             a non-blocking connecting operation is already executing on
     *             this channel.
     * @return CARAPI = NOERROR(0) if success, elas as follows:
     * Value | Meaning |
     * :-|:------------|
     * ClosedChannelException | if this channel is closed.
     * AsynchronousCloseException | if this channel is closed by another thread while this method is executing.
     * ClosedByInterruptException | if another thread interrupts the calling thread while this operation is in progress. The calling thread will have the interrupt state set and this channel will be closed.
     * UnresolvedAddressException | if the address is not resolved.
     * UnsupportedAddressTypeException | if the address type is not supported.
     * IOException | if an I/O error occurs.
     */
    virtual CARAPI Connect(
        /* [in] */ ISocketAddress* address,
        /* [out] */ Boolean* result) = 0;

    /**
     * Completes the connection process initiated by a call of {
     * connect(SocketAddress)}.
     * <p>
     * This method returns {true} if the connection is finished already
     * and returns {false} if the channel is non-blocking and the
     * connection is not finished yet.
     * <p>
     * If this channel is in blocking mode, this method will suspend and return
     * {true} when the connection is finished. It closes this channel and
     * throws an exception if the connection fails.
     * <p>
     * This method can be called at any moment and it can block other {
     * read} and {write} operations while connecting.
     *
     * @return {true} if the connection is successfully finished, {
     *         false} otherwise.
     * @return CARAPI = NOERROR(0) if success, elas as follows:
     * Value | Meaning |
     * :-|:------------|
     * NoConnectionPendingException | if the channel is not connected and the connection process has not been initiated.
     * ClosedChannelException | if this channel is closed.
     * AsynchronousCloseException | if this channel is closed by another thread while this method is executing.
     * ClosedByInterruptException | if another thread interrupts the calling thread while this operation is in progress. The calling thread has the interrupt state set, and this channel is closed.
     * IOException | if an I/O error occurs.
     */
    virtual CARAPI FinishConnect(
        /* [out] */ Boolean* result) = 0;

    /**
     * Reads bytes from this socket channel into the given buffer.
     * <p>
     * The maximum number of bytes that will be read is the remaining number of
     * bytes in the buffer when the method is invoked. The bytes will be copied
     * into the buffer starting at the buffer's current position.
     * <p>
     * The call may block if other threads are also attempting to read from this
     * channel.
     * <p>
     * Upon completion, the buffer's position is set to the end of the bytes
     * that have been read. The buffer's limit is not changed.
     *
     * @param target
     *            the byte buffer to receive the bytes.
     * @return the number of bytes actually read.
     * @return CARAPI = NOERROR(0) if success, elas as follows:
     * Value | Meaning |
     * :-|:------------|
     * AsynchronousCloseException | if another thread closes the channel during the read.
     * NotYetConnectedException | if this channel is not yet connected.
     * ClosedByInterruptException | if another thread interrupts the calling thread while this operation is in progress. The interrupt state of the calling thread is set and the channel is closed.
     * ClosedChannelException | if this channel is closed.
     * IOException | if another I/O error occurs.
     * @see java.nio.channels.ReadableByteChannel#read(java.nio.ByteBuffer)
     */
    virtual CARAPI Read(
        /* [in] */ IByteBuffer* target,
        /* [out] */ Int32* number) = 0;

    /**
     * Reads bytes from this socket channel into a subset of the given buffers.
     * This method attempts to read all {remaining()} bytes from {
     * length} byte buffers, in order, starting at {targets[offset]}. The
     * number of bytes actually read is returned.
     * <p>
     * If a read operation is in progress, subsequent threads will block until
     * the read is completed and will then contend for the ability to read.
     *
     * @param targets
     *            the array of byte buffers into which the bytes will be copied.
     * @param offset
     *            the index of the first buffer to store bytes in.
     * @param length
     *            the maximum number of buffers to store bytes in.
     * @return the number of bytes actually read.
     * @return CARAPI = NOERROR(0) if success, elas as follows:
     * Value | Meaning |
     * :-|:------------|
     * AsynchronousCloseException | if this channel is closed by another thread during this read operation.
     * ClosedByInterruptException | if another thread interrupts the calling thread while this operation is in progress. The interrupt state of the calling thread is set and the channel is closed.
     * ClosedChannelException | if this channel is closed.
     * IndexOutOfBoundsException | if {offset < 0} or {length < 0}, or if {offset + length} is greater than the size of {targets}.
     * IOException | if another I/O error occurs.
     * NotYetConnectedException | if this channel is not yet connected.
     * @see java.nio.channels.ScatteringByteChannel#read(java.nio.ByteBuffer[],
     *      int, int)
     */
    virtual CARAPI Read(
        /* [in] */ ArrayOf<IByteBuffer*>* targets,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int64* number) = 0;

    /**
     * Reads bytes from this socket channel and stores them in the specified
     * array of buffers. This method attempts to read as many bytes as can be
     * stored in the buffer array from this channel and returns the number of
     * bytes actually read.
     * <p>
     * If a read operation is in progress, subsequent threads will block until
     * the read is completed and will then contend for the ability to read.
     * <p>
     * Calling this method is equivalent to calling {read(targets, 0,
     * targets.length);}
     *
     * @param targets
     *            the array of byte buffers into which the bytes will be copied.
     * @return the number of bytes actually read.
     * @return CARAPI = NOERROR(0) if success, elas as follows:
     * Value | Meaning |
     * :-|:------------|
     * AsynchronousCloseException | if this channel is closed by another thread during this read operation.
     * ClosedByInterruptException | if another thread interrupts the calling thread while this operation is in progress. The interrupt state of the calling thread is set and the channel is closed.
     * ClosedChannelException | if this channel is closed.
     * IOException | if another I/O error occurs.
     * NotYetConnectedException | if this channel is not yet connected.
     */
    CARAPI Read(
        /* [in] */ ArrayOf<IByteBuffer*>* targets,
        /* [out] */ Int64* number);

    /**
     * Writes bytes from the given byte buffer to this socket channel. The
     * maximum number of bytes that are written is the remaining number of bytes
     * in the buffer when this method is invoked. The bytes are taken from the
     * buffer starting at the buffer's position.
     * <p>
     * The call may block if other threads are also attempting to write to the
     * same channel.
     * <p>
     * Upon completion, the buffer's position is updated to the end of the bytes
     * that have been written. The buffer's limit is not changed.
     *
     * @param source
     *            the byte buffer containing the bytes to be written.
     * @return the number of bytes actually written.
     * @return CARAPI = NOERROR(0) if success, elas as follows:
     * Value | Meaning |
     * :-|:------------|
     * AsynchronousCloseException | if another thread closes the channel during the write.
     * ClosedByInterruptException | if another thread interrupts the calling thread while this operation is in progress. The interrupt state of the calling thread is set and the channel is closed.
     * ClosedChannelException | if the channel was already closed.
     * IOException | if another I/O error occurs.
     * NotYetConnectedException | if this channel is not connected yet.
     * @see java.nio.channels.WritableByteChannel#write(java.nio.ByteBuffer)
     */
    virtual CARAPI Write(
        /* [in] */ IByteBuffer* buffer,
        /* [out] */ Int32* number) = 0;

    /**
     * Attempts to write a subset of the given bytes from the buffers to this
     * socket channel. This method attempts to write all {remaining()}
     * bytes from {length} byte buffers, in order, starting at {
     * sources[offset]}. The number of bytes actually written is returned.
     * <p>
     * If a write operation is in progress, subsequent threads will block until
     * the write is completed and then contend for the ability to write.
     *
     * @param sources
     *            the array of byte buffers that is the source for bytes written
     *            to this channel.
     * @param offset
     *            the index of the first buffer in {buffers }to get bytes
     *            from.
     * @param length
     *            the number of buffers to get bytes from.
     * @return the number of bytes actually written to this channel.
     * @return CARAPI = NOERROR(0) if success, elas as follows:
     * Value | Meaning |
     * :-|:------------|
     * AsynchronousCloseException | if this channel is closed by another thread during this write operation.
     * ClosedByInterruptException | if another thread interrupts the calling thread while this operation is in progress. The interrupt state of the calling thread is set and the channel is closed.
     * ClosedChannelException | if this channel is closed.
     * IndexOutOfBoundsException | if {offset < 0} or {length < 0}, or if {offset + length} is greater than the size of {sources}.
     * IOException | if another I/O error occurs.
     * NotYetConnectedException | if this channel is not yet connected.
     * @see java.nio.channels.GatheringByteChannel#write(java.nio.ByteBuffer[],
     *      int, int)
     */
    virtual CARAPI Write(
        /* [in] */ ArrayOf<IByteBuffer*>* sources,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int64* number) = 0;

    /**
     * Writes bytes from all the given byte buffers to this socket channel.
     * <p>
     * Calling this method is equivalent to calling {write(sources, 0,
     * sources.length);}
     *
     * @param sources
     *            the buffers containing bytes to write.
     * @return the number of bytes actually written.
     * AsynchronousCloseException | if this channel is closed by another thread during this write operation.
     * ClosedByInterruptException | if another thread interrupts the calling thread while this operation is in progress. The interrupt state of the calling thread is set and the channel is closed.
     * ClosedChannelException | if this channel is closed.
     * IOException | if another I/O error occurs.
     * NotYetConnectedException | if this channel is not yet connected.
     * @see java.nio.channels.GatheringByteChannel#write(java.nio.ByteBuffer[])
     */
    CARAPI Write(
        /* [in] */ ArrayOf<IByteBuffer*>* buffers,
        /* [out] */ Int64* number);

protected:
    SocketChannel();

    CARAPI constructor(
        /* [in] */ ISelectorProvider* provider);
};

} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHANNELS_SOCKET_CHANNEL_H__
