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
