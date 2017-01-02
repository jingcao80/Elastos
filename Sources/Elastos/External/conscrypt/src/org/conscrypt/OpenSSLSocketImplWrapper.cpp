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

#include "Elastos.CoreLibrary.Utility.h"
#include "OpenSSLSocketImplWrapper.h"

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLSocketImplWrapper::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSocketImplWrapper, OpenSSLSocketImpl, IOpenSSLSocketImplWrapper)

ECode OpenSSLSocketImplWrapper::constructor(
    /* [in] */ ISocket* socket,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ Boolean autoClose,
    /* [in] */ ISSLParametersImpl* sslParameters)
{
    OpenSSLSocketImpl::constructor(socket, host, port, autoClose, sslParameters);
    Boolean bCont = FALSE;
    socket->IsConnected(&bCont);
    if (!bCont) {
        // throw new SocketException("Socket is not connected.");
    }
    mSocket = socket;
    return NOERROR;
}

ECode OpenSSLSocketImplWrapper::Connect(
    /* [in] */ ISocketAddress* sockaddr,
    /* [in] */ Int32 timeout)
{
    // throw new IOException("Underlying socket is already connected.");
    return E_IO_EXCEPTION;
}

ECode OpenSSLSocketImplWrapper::Connect(
    /* [in] */ ISocketAddress* sockaddr)
{
    // throw new IOException("Underlying socket is already connected.");
    return E_IO_EXCEPTION;
}

ECode OpenSSLSocketImplWrapper::Bind(
    /* [in] */ ISocketAddress* sockaddr)
{
    // throw new IOException("Underlying socket is already connected.");
    return E_IO_EXCEPTION;
}

ECode OpenSSLSocketImplWrapper::GetRemoteSocketAddress(
    /* [out] */ ISocketAddress** result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetRemoteSocketAddress(result);
}

ECode OpenSSLSocketImplWrapper::GetLocalSocketAddress(
    /* [out] */ ISocketAddress** result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetLocalSocketAddress(result);
}

ECode OpenSSLSocketImplWrapper::GetLocalAddress(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetLocalAddress(result);
}

ECode OpenSSLSocketImplWrapper::GetInetAddress(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetInetAddress(result);
}

ECode OpenSSLSocketImplWrapper::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String str;
    IObject::Probe(mSocket)->ToString(&str);
    *result = String("SSL socket over ") + str;
    return NOERROR;
}

ECode OpenSSLSocketImplWrapper::SetSoLinger(
    /* [in] */ Boolean on,
    /* [in] */ Int32 linger)
{
    mSocket->SetSoLinger(on, linger);
    return NOERROR;
}

ECode OpenSSLSocketImplWrapper::SetTcpNoDelay(
    /* [in] */ Boolean on)
{
    mSocket->SetTcpNoDelay(on);
    return NOERROR;
}

ECode OpenSSLSocketImplWrapper::SetReuseAddress(
    /* [in] */ Boolean on)
{
    mSocket->SetReuseAddress(on);
    return NOERROR;
}

ECode OpenSSLSocketImplWrapper::SetKeepAlive(
    /* [in] */ Boolean on)
{
    mSocket->SetKeepAlive(on);
    return NOERROR;
}

ECode OpenSSLSocketImplWrapper::SetTrafficClass(
    /* [in] */ Int32 tos)
{
    mSocket->SetTrafficClass(tos);
    return NOERROR;
}

ECode OpenSSLSocketImplWrapper::SetSoTimeout(
    /* [in] */ Int32 to)
{
    mSocket->SetSoTimeout(to);
    OpenSSLSocketImpl::SetSoTimeout(to);
    return NOERROR;
}

ECode OpenSSLSocketImplWrapper::SetSendBufferSize(
    /* [in] */ Int32 size)
{
    mSocket->SetSendBufferSize(size);
    return NOERROR;
}

ECode OpenSSLSocketImplWrapper::SetReceiveBufferSize(
    /* [in] */ Int32 size)
{
    mSocket->SetReceiveBufferSize(size);
    return NOERROR;
}

ECode OpenSSLSocketImplWrapper::GetTcpNoDelay(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetTcpNoDelay(result);
}

ECode OpenSSLSocketImplWrapper::GetReuseAddress(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetReuseAddress(result);
}

ECode OpenSSLSocketImplWrapper::GetOOBInline(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetOOBInline(result);
}

ECode OpenSSLSocketImplWrapper::GetKeepAlive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetKeepAlive(result);
}

ECode OpenSSLSocketImplWrapper::GetTrafficClass(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetTrafficClass(result);
}

ECode OpenSSLSocketImplWrapper::GetSoTimeout(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetSoTimeout(result);
}

ECode OpenSSLSocketImplWrapper::GetSoLinger(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetSoLinger(result);
}

ECode OpenSSLSocketImplWrapper::GetSendBufferSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetSendBufferSize(result);
}

ECode OpenSSLSocketImplWrapper::GetReceiveBufferSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetReceiveBufferSize(result);
}

ECode OpenSSLSocketImplWrapper::IsConnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->IsConnected(result);
}

ECode OpenSSLSocketImplWrapper::IsClosed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->IsClosed(result);
}

ECode OpenSSLSocketImplWrapper::IsBound(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->IsBound(result);
}

ECode OpenSSLSocketImplWrapper::IsOutputShutdown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->IsOutputShutdown(result);
}

ECode OpenSSLSocketImplWrapper::IsInputShutdown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->IsInputShutdown(result);
}

ECode OpenSSLSocketImplWrapper::GetPort(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetPort(result);
}

ECode OpenSSLSocketImplWrapper::GetLocalPort(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mSocket->GetLocalPort(result);
}

} // namespace Conscrypt
} // namespace Org
