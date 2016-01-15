#ifndef __ELASTOS_NET_SOCKETUTILS_H__
#define __ELASTOS_NET_SOCKETUTILS_H__

#include "Socket.h"

namespace Elastos {
namespace Net {

/**
 * @hide internal use only
 */
class SocketUtils
{
public:
    /**
     * Helps us reuse more of Socket's implementation in SocketChannelImpl.SocketAdapter.
     * It's not the case that we should set isCreated in the Socket(SocketImpl) constructor;
     * SocketImpl.create should be called in general. But for SocketChannelImpl.SocketAdapter's
     * SocketImpl, we don't want that behavior.
     */
    static CARAPI_(void) SetCreated(
        /* [in] */ ISocket* s);

private:
    SocketUtils();
    SocketUtils(const SocketUtils&);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_SOCKETUTILS_H__