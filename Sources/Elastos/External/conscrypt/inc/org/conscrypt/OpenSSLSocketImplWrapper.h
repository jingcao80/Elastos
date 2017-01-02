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

#ifndef __ORG_CONSCRYPT_OPENSSLSOCKETIMPLWRAPPER_H__
#define __ORG_CONSCRYPT_OPENSSLSOCKETIMPLWRAPPER_H__

#include "_Org.Conscrypt.h"
#include "OpenSSLSocketImpl.h"

using Elastos::Net::ISocketAddress;

namespace Org {
namespace Conscrypt {

/**
 * This class wraps the SSL functionality over an existing connected socket.
 */
class OpenSSLSocketImplWrapper
    : public OpenSSLSocketImpl
    , public IOpenSSLSocketImplWrapper
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ISocket* socket,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ Boolean autoClose,
        /* [in] */ ISSLParametersImpl* sslParameters);

    CARAPI Connect(
        /* [in] */ ISocketAddress* sockaddr,
        /* [in] */ Int32 timeout);

    CARAPI Connect(
        /* [in] */ ISocketAddress* sockaddr);

    CARAPI Bind(
        /* [in] */ ISocketAddress* sockaddr);

    CARAPI GetRemoteSocketAddress(
        /* [out] */ ISocketAddress** result);

    CARAPI GetLocalSocketAddress(
        /* [out] */ ISocketAddress** result);

    CARAPI GetLocalAddress(
        /* [out] */ IInetAddress** result);

    CARAPI GetInetAddress(
        /* [out] */ IInetAddress** result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI SetSoLinger(
        /* [in] */ Boolean on,
        /* [in] */ Int32 linger);

    CARAPI SetTcpNoDelay(
        /* [in] */ Boolean on);

    CARAPI SetReuseAddress(
        /* [in] */ Boolean on);

    CARAPI SetKeepAlive(
        /* [in] */ Boolean on);

    CARAPI SetTrafficClass(
        /* [in] */ Int32 tos);

    CARAPI SetSoTimeout(
        /* [in] */ Int32 to);

    CARAPI SetSendBufferSize(
        /* [in] */ Int32 size);

    CARAPI SetReceiveBufferSize(
        /* [in] */ Int32 size);

    CARAPI GetTcpNoDelay(
        /* [out] */ Boolean* result);

    CARAPI GetReuseAddress(
        /* [out] */ Boolean* result);

    CARAPI GetOOBInline(
        /* [out] */ Boolean* result);

    CARAPI GetKeepAlive(
        /* [out] */ Boolean* result);

    CARAPI GetTrafficClass(
        /* [out] */ Int32* result);

    CARAPI GetSoTimeout(
        /* [out] */ Int32* result);

    CARAPI GetSoLinger(
        /* [out] */ Int32* result);

    CARAPI GetSendBufferSize(
        /* [out] */ Int32* result);

    CARAPI GetReceiveBufferSize(
        /* [out] */ Int32* result);

    CARAPI IsConnected(
        /* [out] */ Boolean* result);

    CARAPI IsClosed(
        /* [out] */ Boolean* result);

    CARAPI IsBound(
        /* [out] */ Boolean* result);

    CARAPI IsOutputShutdown(
        /* [out] */ Boolean* result);

    CARAPI IsInputShutdown(
        /* [out] */ Boolean* result);

    CARAPI GetPort(
        /* [out] */ Int32* result);

    CARAPI GetLocalPort(
        /* [out] */ Int32* result);

public:
    AutoPtr<ISocket> mSocket;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLSOCKETIMPLWRAPPER_H__
