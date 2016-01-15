
#ifndef __ELASTOS_DROID_NET_LOCALSERVERSOCKET_H__
#define __ELASTOS_DROID_NET_LOCALSERVERSOCKET_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/LocalSocketImpl.h"

using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * non-standard class for creating inbound UNIX-domain socket
 * on the Android platform, this is created in the Linux non-filesystem
 * namespace.
 *
 * On simulator platforms, this may be created in a temporary directory on
 * the filesystem
 */
class LocalServerSocket
    : public Object
    , public ILocalServerSocket
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Crewates a new server socket listening at specified name.
     * On the Android platform, the name is created in the Linux
     * abstract namespace (instead of on the filesystem).
     *
     * @param name address for socket
     * @throws IOException
     */
    CARAPI constructor(
        /* [in] */ const String& name);

    /**
     * Create a LocalServerSocket from a file descriptor that's already
     * been created and bound. listen() will be called immediately on it.
     * Used for cases where file descriptors are passed in via environment
     * variables
     *
     * @param fd bound file descriptor
     * @throws IOException
     */
    CARAPI constructor(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Obtains the socket's local address
     *
     * @return local address
     */
    CARAPI GetLocalSocketAddress(
        /* [out] */ ILocalSocketAddress** result);

    /**
     * Accepts a new connection to the socket. Blocks until a new
     * connection arrives.
     *
     * @return a socket representing the new connection.
     * @throws IOException
     */
    CARAPI Accept(
        /* [out] */ ILocalSocket** result);

    /**
     * Returns file descriptor or null if not yet open/already closed
     *
     * @return fd or null
     */
    CARAPI GetFileDescriptor(
        /* [out] */ IFileDescriptor** result);

    /**
     * Closes server socket.
     *
     * @throws IOException
     */
    CARAPI Close();

private:
    AutoPtr<LocalSocketImpl> mImpl;
    AutoPtr<ILocalSocketAddress> mLocalAddress;

    /** 50 seems a bit much, but it's what was here */
    static const Int32 LISTEN_BACKLOG;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_LOCALSERVERSOCKET_H__
