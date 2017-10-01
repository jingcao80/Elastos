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

#ifndef __ELASTOS_NET_SOCKETIMPL_H__
#define __ELASTOS_NET_SOCKETIMPL_H__

#include "Elastos.CoreLibrary.Net.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Net {

class SocketImpl
    : public Object
    , public ISocketImpl
    , public ISocketOptions
{
public:
    CAR_INTERFACE_DECL()

    SocketImpl();

    virtual CARAPI GetFD(
        /* [out] */ IFileDescriptor** fileDescriptor);

    virtual CARAPI ToString(
        /* [out] */ String* str);

    virtual CARAPI Accept(
        /* [in] */ ISocketImpl* newSocket) = 0;

    virtual CARAPI Available(
        /* [out] */ Int32* num) = 0;

    virtual CARAPI Bind(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port) = 0;

    virtual CARAPI Close() = 0;

    virtual CARAPI Connect(
        /* [in] */ const String& host,
        /* [in] */ Int32 port) = 0;

    virtual CARAPI Connect(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port) = 0;

    virtual CARAPI Create(
        /* [in] */ Boolean isStreaming) = 0;

    virtual CARAPI GetFileDescriptor(
        /* [out] */ IFileDescriptor** fileDescriptor);

    virtual CARAPI GetInetAddress(
        /* [out] */ IInetAddress** netAddress);

    virtual CARAPI GetInputStream(
        /* [out] */ IInputStream** inputStream) = 0;

    virtual CARAPI GetLocalPort(
        /* [out] */ Int32* num);

    virtual CARAPI GetOutputStream(
        /* [out] */ IOutputStream** outputStream) = 0;

    virtual CARAPI GetPort(
        /* [out] */ Int32* port);

    virtual CARAPI Listen(
        /* [in] */ Int32 backlog) = 0;

    virtual CARAPI ShutdownInput();

    virtual CARAPI ShutdownOutput();

    virtual CARAPI Connect(
        /* [in] */ ISocketAddress* remoteAddr,
        /* [in] */ Int32 timeout) = 0;

    virtual CARAPI SupportsUrgentData(
        /* [out] */ Boolean* isSupports);

    virtual CARAPI SendUrgentData(
        /* [in] */ Int32 value) = 0;

    virtual CARAPI SetPerformancePreferences(
        /* [in] */ Int32 connectionTime,
        /* [in] */ Int32 latency,
        /* [in] */ Int32 bandwidth);

    /**
     * Initialize the bind() state.
     * @hide used in java.nio.
     */
    CARAPI OnBind(
        /* [in] */ IInetAddress * localAddress,
        /* [in] */ Int32 localPort) {
        // Do not add any code to these methods. They are concrete only to preserve API
        // compatibility.
        return NOERROR;
    }

    /**
     * Initialize the connect() state.
     * @hide used in java.nio.
     */
    CARAPI OnConnect(
        /* [in] */ IInetAddress * remoteAddress,
        /* [in] */ Int32 remotePort) {
        // Do not add any code to these methods. They are concrete only to preserve API
        // compatibility.
        return NOERROR;
    }

    /**
     * Initialize the close() state.
     * @hide used in java.nio.
     */
    CARAPI OnClose() {
        // Do not add any code to these methods. They are concrete only to preserve API
        // compatibility.
        return NOERROR;
    }

public:
    Boolean mStreaming;

protected:
    AutoPtr<IInetAddress> mAddress;

    Int32 mPort;

    AutoPtr<IFileDescriptor> mFd;

    Int32 mLocalport;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_SOCKETIMPL_H__
