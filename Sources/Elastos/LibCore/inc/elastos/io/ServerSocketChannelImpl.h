#ifndef __ELASTOS_IO_SERVERSOCKETCHANNELIMPL_H__
#define __ELASTOS_IO_SERVERSOCKETCHANNELIMPL_H__

#include "ServerSocketChannel.h"
#include "SocketChannelImpl.h"
#include "SelectorProvider.h"
#include "ServerSocket.h"

using Elastos::IO::Channels::IServerSocketChannel;
using Elastos::IO::Channels::ServerSocketChannel;
using Elastos::IO::Channels::ISocketChannel;
using Elastos::IO::Channels::Spi::SelectorProvider;
using Elastos::Net::ISocket;
using Elastos::Net::ServerSocket;

namespace Elastos {
namespace IO {

class ServerSocketChannelImpl
    : public ServerSocketChannel
    , public IFileDescriptorChannel
{
private:
    class ServerSocketAdapter : public ServerSocket
    {
    public:
        ServerSocketAdapter(
            /* [in] */ ServerSocketChannelImpl* aChannelImpl);

        CARAPI Accept(
            /* [out] */ ISocket** socket);

        CARAPI ImplAccept(
            /* [in] */ SocketChannelImpl* clientSocketChannel,
            /* [out] */ ISocket** aSocket);

        CARAPI GetChannel(
            /* [out] */ IServerSocketChannel** channel);

        CARAPI Close();

        CARAPI_(AutoPtr<IFileDescriptor>) GetFD();

    private:
        ServerSocketChannelImpl* mChannelImpl;
    };

public:
    CAR_INTERFACE_DECL()

    ServerSocketChannelImpl(
        /* [in] */ ISelectorProvider* sp);

    CARAPI GetSocket(
        /* [out] */ IServerSocket** outsock);

    CARAPI Accept(
        /* [out] */ ISocketChannel** channel);

    CARAPI GetFD(
        /* [out] */ IFileDescriptor** outfd);

private:
    CARAPI_(Boolean) ShouldThrowSocketTimeoutExceptionFromAccept(
        /* [in] */ ECode e);

protected:
    CARAPI ImplConfigureBlocking(
        /* [in] */ Boolean blocking);

    CARAPI ImplCloseSelectableChannel();

private:
    AutoPtr<ServerSocketAdapter> mSocket;
    Object mAcceptLock;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_SERVERSOCKETCHANNELIMPL_H__
