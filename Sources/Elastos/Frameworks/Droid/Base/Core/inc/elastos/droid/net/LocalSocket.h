
#ifndef __ELASTOS_DROID_NET_LOCALSOCKET_H__
#define __ELASTOS_DROID_NET_LOCALSOCKET_H__

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::IO::ICloseable;
using Elastos::IO::IInputStream;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Creates a (non-server) socket in the UNIX-domain namespace. The interface
 * here is not entirely unlike that of java.net.Socket
 */
class LocalSocket
    : public Object
    , public ILocalSocket
    , public ICloseable
{
public:
    CAR_INTERFACE_DECL()

public:
    /**
     * Creates a AF_LOCAL/UNIX domain stream socket.
     */
    CARAPI constructor();

    /**
     * Creates a AF_LOCAL/UNIX domain stream socket with given socket type
     *
     * @param sockType either {@link #SOCKET_DGRAM}, {@link #SOCKET_STREAM}
     * or {@link #SOCKET_SEQPACKET}
     */
    CARAPI constructor(
        /* [in] */ Int32 sockType);

    /**
     * Creates a AF_LOCAL/UNIX domain stream socket with FileDescriptor.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IFileDescriptor* fd);

    /*package*/
    CARAPI constructor(
        /* [in] */ ILocalSocketImpl* impl,
        /* [in] */ Int32 sockType);

    /** {@inheritDoc} */
    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Connects this socket to an endpoint. May only be called on an instance
     * that has not yet been connected.
     *
     * @param endpoint endpoint address
     * @throws IOException if socket is in invalid state or the address does
     * not exist.
     */
    CARAPI Connect(
        /* [in] */ ILocalSocketAddress* endpoint);

    /**
     * Binds this socket to an endpoint name. May only be called on an instance
     * that has not yet been bound.
     *
     * @param bindpoint endpoint address
     * @throws IOException
     */
    CARAPI Bind(
        /* [in] */ ILocalSocketAddress* bindpoint);

    /**
     * Retrieves the name that this socket is bound to, if any.
     *
     * @return Local address or null if anonymous
     */
    CARAPI GetLocalSocketAddress(
        /* [out] */ ILocalSocketAddress** result);

    /**
     * Retrieves the input stream for this instance.
     *
     * @return input stream
     * @throws IOException if socket has been closed or cannot be created.
     */
    CARAPI GetInputStream(
        /* [out] */ IInputStream** result);

    /**
     * Retrieves the output stream for this instance.
     *
     * @return output stream
     * @throws IOException if socket has been closed or cannot be created.
     */
    CARAPI GetOutputStream(
        /* [out] */ IOutputStream** result);

    /**
     * Closes the socket.
     *
     * @throws IOException
     */
    // @Override
    CARAPI Close();

    /**
     * Shuts down the input side of the socket.
     *
     * @throws IOException
     */
    CARAPI ShutdownInput();

    /**
     * Shuts down the output side of the socket.
     *
     * @throws IOException
     */
    CARAPI ShutdownOutput();

    CARAPI SetReceiveBufferSize(
        /* [in] */ Int32 size);

    CARAPI GetReceiveBufferSize(
        /* [out] */ Int32* result);

    CARAPI SetSoTimeout(
        /* [in] */ Int32 n);

    CARAPI GetSoTimeout(
        /* [out] */ Int32* result);

    CARAPI SetSendBufferSize(
        /* [in] */ Int32 n);

    CARAPI GetSendBufferSize(
        /* [out] */ Int32* result);

    //???SEC
    CARAPI GetRemoteSocketAddress(
        /* [out] */ ILocalSocketAddress** result);

    //???SEC
    CARAPI IsConnected(
        /* [out] */ Boolean* result);

    //???SEC
    CARAPI IsClosed(
        /* [out] */ Boolean* result);

    //???SEC
    CARAPI IsBound(
        /* [out] */ Boolean* result);

    //???SEC
    CARAPI IsOutputShutdown(
        /* [out] */ Boolean* result);

    //???SEC
    CARAPI IsInputShutdown(
        /* [out] */ Boolean* result);

    //???SEC
    CARAPI Connect(
        /* [in] */ ILocalSocketAddress* endpoint,
        /* [in] */ Int32 timeout);

    /**
     * Enqueues a set of file descriptors to send to the peer. The queue
     * is one deep. The file descriptors will be sent with the next write
     * of normal data, and will be delivered in a single ancillary message.
     * See "man 7 unix" SCM_RIGHTS on a desktop Linux machine.
     *
     * @param fds non-null; file descriptors to send.
     */
    CARAPI SetFileDescriptorsForSend(
        /* [in] */ ArrayOf<IFileDescriptor*>* fds);

    /**
     * Retrieves a set of file descriptors that a peer has sent through
     * an ancillary message. This method retrieves the most recent set sent,
     * and then returns null until a new set arrives.
     * File descriptors may only be passed along with regular data, so this
     * method can only return a non-null after a read operation.
     *
     * @return null or file descriptor array
     * @throws IOException
     */
    CARAPI GetAncillaryFileDescriptors(
        /* [out, callee] */ ArrayOf<IFileDescriptor*>** result);

    /**
     * Retrieves the credentials of this socket's peer. Only valid on
     * connected sockets.
     *
     * @return non-null; peer credentials
     * @throws IOException
     */
    CARAPI GetPeerCredentials(
        /* [out] */ ICredentials** result);

    /**
     * Returns file descriptor or null if not yet open/already closed
     *
     * @return fd or null
     */
    CARAPI GetFileDescriptor(
        /* [out] */ IFileDescriptor** result);

private:
    /**
     * It's difficult to discern from the spec when impl.create() should be
     * called, but it seems like a reasonable rule is "as soon as possible,
     * but not in a context where IOException cannot be thrown"
     *
     * @throws IOException from SocketImpl.create()
     */
    CARAPI ImplCreateIfNeeded();

public:
    /** unknown socket type (used for constructor with existing file descriptor) */
    /* package */ static const Int32 SOCKET_UNKNOWN;

private:
    AutoPtr<ILocalSocketImpl> mImpl;

    /* volatile */ Boolean mImplCreated;

    AutoPtr<ILocalSocketAddress> mLocalAddress;

    Boolean mIsBound;

    Boolean mIsConnected;

    Int32 mSockType;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_LOCALSOCKET_H__
