
#ifndef __ELASTOS_DROID_NET_LOCALSOCKETIMPL_H__
#define __ELASTOS_DROID_NET_LOCALSOCKETIMPL_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/io/InputStream.h>
#include <elastos/io/OutputStream.h>

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::InputStream;
using Elastos::IO::OutputStream;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Socket implementation used for android.net.LocalSocket and
 * android.net.LocalServerSocket. Supports only AF_LOCAL sockets.
 */
class LocalSocketImpl
    : public Object
    , public ILocalSocketImpl
{
public:
    /**
     * An input stream for local sockets. Needed because we may
     * need to read ancillary data.
     */
    class SocketInputStream
        : public InputStream
    {
    public:
        SocketInputStream(
            /* [in] */ LocalSocketImpl* host);

        /** {@inheritDoc} */
        // @Override
        CARAPI Available(
            /* [out] */ Int32* result);

        /** {@inheritDoc} */
        // @Override
        CARAPI Close();

        /** {@inheritDoc} */
        // @Override
        CARAPI Read(
            /* [out] */ Int32* result);

        /** {@inheritDoc} */
        // @Override
        CARAPI Read(
            /* [in] */ ArrayOf<Byte>* b,
            /* [out] */ Int32* result);

        /** {@inheritDoc} */
        // @Override
        CARAPI Read(
            /* [in] */ ArrayOf<Byte>* b,
            /* [in] */ Int32 off,
            /* [in] */ Int32 len,
            /* [out] */ Int32* result);

    private:
        LocalSocketImpl* mHost;
    };

    /**
     * An output stream for local sockets. Needed because we may
     * need to read ancillary data.
     */
    class SocketOutputStream
        : public OutputStream
    {
    public:
        SocketOutputStream(
            /* [in] */ LocalSocketImpl* host);

        /** {@inheritDoc} */
        // @Override
        CARAPI Close();

        /** {@inheritDoc} */
        // @Override
        CARAPI Write(
            /* [in] */ ArrayOf<Byte>* b);

        /** {@inheritDoc} */
        // @Override
        CARAPI Write(
            /* [in] */ ArrayOf<Byte>* b,
            /* [in] */ Int32 off,
            /* [in] */ Int32 len);

        /** {@inheritDoc} */
        // @Override
        CARAPI Write(
            /* [in] */ Int32 b);

        /**
         * Wait until the data in sending queue is emptied. A polling version
         * for flush implementation.
         * @throws IOException
         *             if an i/o error occurs.
         */
        // @Override
        CARAPI Flush();

    private:
        LocalSocketImpl* mHost;
    };

    CAR_INTERFACE_DECL()

    virtual ~LocalSocketImpl();

    /*package*/
    CARAPI constructor();

    /*package*/
    CARAPI constructor(
        /* [in] */ IFileDescriptor* fd);

    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Creates a socket in the underlying OS.
     *
     * @param sockType either {@link LocalSocket#SOCKET_DGRAM}, {@link LocalSocket#SOCKET_STREAM}
     * or {@link LocalSocket#SOCKET_SEQPACKET}
     * @throws IOException
     */
    CARAPI Create(
        /* [in] */ Int32 sockType);

    /**
     * Closes the socket.
     *
     * @throws IOException
     */
    CARAPI Close();

    /** note timeout presently ignored */
    CARAPI Connect(
        /* [in] */ ILocalSocketAddress* address,
        /* [in] */ Int32 timeout);

    /**
     * Binds this socket to an endpoint name. May only be called on an instance
     * that has not yet been bound.
     *
     * @param endpoint endpoint address
     * @throws IOException
     */
    CARAPI Bind(
        /* [in] */ ILocalSocketAddress* endpoint);

    CARAPI Listen(
        /* [in] */ Int32 backlog);

    /**
     * Accepts a new connection to the socket. Blocks until a new
     * connection arrives.
     *
     * @param s a socket that will be used to represent the new connection.
     * @throws IOException
     */
    CARAPI Accept(
        /* [in] */ ILocalSocketImpl* s);

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
     * Returns the number of bytes available for reading without blocking.
     *
     * @return >= 0 count bytes available
     * @throws IOException
     */
    CARAPI Available(
        /* [out] */ Int32* result);

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

    CARAPI GetFileDescriptor(
        /* [out] */ IFileDescriptor** result);

    CARAPI SupportsUrgentData(
        /* [out] */ Boolean* result);

    CARAPI SendUrgentData(
        /* [in] */ Int32 data);

    CARAPI GetOption(
        /* [in] */ Int32 optID,
        /* [out] */ IInterface** result);

    CARAPI SetOption(
        /* [in] */ Int32 optID,
        /* [in] */ IInterface* value);

    /**
     * Enqueues a set of file descriptors to send to the peer. The queue
     * is one deep. The file descriptors will be sent with the next write
     * of normal data, and will be delivered in a single ancillary message.
     * See "man 7 unix" SCM_RIGHTS on a desktop Linux machine.
     *
     * @param fds non-null; file descriptors to send.
     * @throws IOException
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
     * Retrieves the socket name from the OS.
     *
     * @return non-null; socket name
     * @throws IOException on failure
     */
    CARAPI GetSockAddress(
        /* [out] */ ILocalSocketAddress** result);

private:
    CARAPI NativePending(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Int32* result);

    CARAPI NativeAvailable(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Int32* result);

    CARAPI NativeRead(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Int32* result);

    CARAPI NativeReadba(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Int32* result);

    CARAPI NativeWriteba(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [in] */ IFileDescriptor* fd);

    CARAPI NativeWrite(
        /* [in] */ Int32 b,
        /* [in] */ IFileDescriptor* fd);

    CARAPI ConnectLocal(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ const String& name,
        /* [in] */ Int32 ns);

    CARAPI BindLocal(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ const String& name,
        /* [in] */ Int32 ns);

    CARAPI NativeListen(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 backlog);

    // native function
    CARAPI Shutdown(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Boolean shutdownInput);

    CARAPI NativeGetPeerCredentials(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ ICredentials** result);

    CARAPI NativeGetOption(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 optID,
        /* [out] */ Int32* result);

    CARAPI NativeSetOption(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 optID,
        /* [in] */ Int32 b,
        /* [in] */ Int32 value);

    /**
     * Accepts a connection on a server socket.
     *
     * @param fd file descriptor of server socket
     * @param s socket implementation that will become the new socket
     * @return file descriptor of new socket
     */
    // native function
    CARAPI Accept(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ILocalSocketImpl* s,
        /* [out] */ IFileDescriptor** result);

public:
    /** file descriptor array received during a previous read */
    AutoPtr<ArrayOf<IFileDescriptor*> > mInboundFileDescriptors;

    /** file descriptor array that should be written during next write */
    AutoPtr<ArrayOf<IFileDescriptor*> > mOutboundFileDescriptors;

private:
    Object mReadMonitor;

    Object mWriteMonitor;

    AutoPtr<SocketInputStream> mFis;

    AutoPtr<SocketOutputStream> mFos;

    /** null if closed or not yet created */
    AutoPtr<IFileDescriptor> mFd;

    /** whether fd is created internally */
    Boolean mFdCreatedInternally;

    friend class SocketInputStream;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_LOCALSOCKETIMPL_H__
