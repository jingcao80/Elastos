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

#ifndef __ELASTOS_NET_SERVERSOCKET_H__
#define __ELASTOS_NET_SERVERSOCKET_H__

#include "CSocket.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::ICloseable;
using Elastos::IO::Channels::IServerSocketChannel;

namespace Elastos {
namespace Net {

class ServerSocket
    : public Object
    , public IServerSocket
    , public ICloseable
{
public:
    CAR_INTERFACE_DECL()

    ServerSocket();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 aPort);

    CARAPI constructor(
        /* [in] */ Int32 aPort,
        /* [in] */ Int32 backlog);

    CARAPI constructor(
        /* [in] */ Int32 aPort,
        /* [in] */ Int32 backlog,
        /* [in] */ IInetAddress* localAddr);

    CARAPI GetImpl(
        /* [out] */ ISocketImpl** impl);

    CARAPI Accept(
        /* [out] */ ISocket** socket);

    CARAPI CheckListen(
        /* [in] */ Int32 aPort);

    CARAPI Close();

    CARAPI GetInetAddress(
        /* [out] */ IInetAddress** address);

    CARAPI GetLocalPort(
        /* [out] */ Int32* port);

    CARAPI GetSoTimeout(
        /* [out] */ Int32* timeout);

    static CARAPI SetSocketFactory(
        /* [in] */ ISocketImplFactory* aFactory);

    CARAPI SetSoTimeout(
        /* [in] */ Int32 timeout);

    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI Bind(
        /* [in] */ ISocketAddress* localAddr);

    CARAPI Bind(
        /* [in] */ ISocketAddress* localAddr,
        /* [in] */ Int32 backlog);

    CARAPI GetLocalSocketAddress(
        /* [out] */ ISocketAddress** address);

    CARAPI IsBound(
        /* [out] */ Boolean* isBound);

    CARAPI IsClosed(
        /* [out] */ Boolean* isClosed);

    CARAPI SetReuseAddress(
        /* [in] */ Boolean reuse);

    CARAPI GetReuseAddress(
        /* [out] */ Boolean* reuse);

    CARAPI SetReceiveBufferSize(
        /* [in] */ Int32 size);

    CARAPI GetReceiveBufferSize(
        /* [out] */ Int32* size);

    CARAPI GetChannel(
        /* [out] */ IServerSocketChannel** channel);

    CARAPI SetPerformancePreferences(
        /* [in] */ Int32 connectionTime,
        /* [in] */ Int32 latency,
        /* [in] */ Int32 bandwidth);

//protected:
    CARAPI ImplAccept(
        /* [in] */ ISocket* aSocket);

private:
    CARAPI ReadBackBindState();

    CARAPI CheckOpen();

public:
    AutoPtr<ISocketImpl> mImpl;

    static AutoPtr<ISocketImplFactory> sFactory;
    static Object sLock;

private:
    static const Int32 DEFAULT_BACKLOG;// = 50;

    Boolean mIsBound;
    Boolean mIsClosed;

    AutoPtr<IInetAddress> mLocalAddress;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_SERVERSOCKET_H__
