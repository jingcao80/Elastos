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

#include "org/apache/http/impl/SocketHttpServerConnection.h"
#include "org/apache/http/params/HttpConnectionParams.h"
#include "org/apache/http/impl/io/SocketInputBuffer.h"
#include "org/apache/http/impl/io/SocketOutputBuffer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Impl::IO::SocketInputBuffer;
using Org::Apache::Http::Impl::IO::SocketOutputBuffer;
using Org::Apache::Http::Params::HttpConnectionParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

SocketHttpServerConnection::SocketHttpServerConnection()
    : AbstractHttpServerConnection()
    , mOpen(FALSE)
{}

CAR_INTERFACE_IMPL(SocketHttpServerConnection, AbstractHttpServerConnection, IHttpInetConnection)

ECode SocketHttpServerConnection::AssertNotOpen()
{
    if (mOpen) {
        Logger::E("SocketHttpServerConnection", "Connection is already open");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode SocketHttpServerConnection::AssertOpen()
{
    if (!mOpen) {
        Logger::E("SocketHttpServerConnection", "Connection is not open");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode SocketHttpServerConnection::CreateHttpDataReceiver(
    /* [in] */ ISocket* socket,
    /* [in] */ Int32 buffersize,
    /* [in] */ IHttpParams* params,
    /* [out] */ ISessionInputBuffer** inputBuffer)
{
    VALIDATE_NOT_NULL(inputBuffer)
    AutoPtr<ISessionInputBuffer> buf = (ISessionInputBuffer*)new SocketInputBuffer(
            socket, buffersize, params);
    *inputBuffer = buf;
    REFCOUNT_ADD(*inputBuffer)
    return NOERROR;
}

ECode SocketHttpServerConnection::CreateHttpDataTransmitter(
    /* [in] */ ISocket* socket,
    /* [in] */ Int32 buffersize,
    /* [in] */ IHttpParams* params,
    /* [out] */ ISessionOutputBuffer** outputBuffer)
{
    VALIDATE_NOT_NULL(outputBuffer)
    AutoPtr<ISessionOutputBuffer> buf = (ISessionOutputBuffer*)new SocketOutputBuffer(
            socket, buffersize, params);
    *outputBuffer = buf;
    REFCOUNT_ADD(*outputBuffer)
    return NOERROR;
}

ECode SocketHttpServerConnection::Bind(
    /* [in] */ ISocket* socket,
    /* [in] */ IHttpParams* params)
{
    if (socket == NULL) {
        Logger::E("SocketHttpServerConnection", "Socket may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        Logger::E("SocketHttpServerConnection", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSocket = socket;

    Int32 buffersize;
    HttpConnectionParams::GetSocketBufferSize(params, &buffersize);

    AutoPtr<ISessionInputBuffer> in;
    CreateHttpDataReceiver(socket, buffersize, params, (ISessionInputBuffer**)&in);
    AutoPtr<ISessionOutputBuffer> out;
    CreateHttpDataTransmitter(socket, buffersize, params, (ISessionOutputBuffer**)&out);
    Init(in, out, params);

    mOpen = TRUE;
    return NOERROR;
}

AutoPtr<ISocket> SocketHttpServerConnection::GetSocket()
{
    return mSocket;
}

ECode SocketHttpServerConnection::IsOpen(
    /* [out] */ Boolean* isOpen)
{
    VALIDATE_NOT_NULL(isOpen)
    *isOpen = mOpen;
    return NOERROR;
}

ECode SocketHttpServerConnection::GetLocalAddress(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address)
    if (mSocket != NULL) {
        return mSocket->GetLocalAddress(address);
    }
    else {
        *address = NULL;
        return NOERROR;
    }
}

ECode SocketHttpServerConnection::GetLocalPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port)
    if (mSocket != NULL) {
        return mSocket->GetLocalPort(port);
    }
    else {
        *port = -1;
        return NOERROR;
    }
}

ECode SocketHttpServerConnection::GetRemoteAddress(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address)
    if (mSocket != NULL) {
        return mSocket->GetInetAddress(address);
    }
    else {
        *address = NULL;
        return NOERROR;
    }
}

ECode SocketHttpServerConnection::GetRemotePort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port)
    if (mSocket != NULL) {
        return mSocket->GetPort(port);
    }
    else {
        *port = -1;
        return NOERROR;
    }
}

ECode SocketHttpServerConnection::SetSocketTimeout(
    /* [in] */ Int32 timeout)
{
    FAIL_RETURN(AssertOpen())
    if (mSocket != NULL) {
        // try {
        return mSocket->SetSoTimeout(timeout);
        // } catch (SocketException ignore) {
        //     // It is not quite clear from the Sun's documentation if there are any
        //     // other legitimate cases for a socket exception to be thrown when setting
        //     // SO_TIMEOUT besides the socket being already closed
        // }
    }
    return NOERROR;
}

ECode SocketHttpServerConnection::GetSocketTimeout(
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout)
    if (mSocket != NULL) {
        // try {
        Int32 result;
        ECode ec = mSocket->GetSoTimeout(&result);
        if (ec == (ECode)E_SOCKET_EXCEPTION) {
            *timeout = -1;
            return NOERROR;
        }
        *timeout = result;
        // } catch (SocketException ignore) {
        //     return -1;
        // }
    }
    else {
        *timeout = -1;
    }
    return NOERROR;
}

ECode SocketHttpServerConnection::Shutdown()
{
    mOpen = FALSE;
    AutoPtr<ISocket> tmpsocket = mSocket;
    if (tmpsocket != NULL) {
        tmpsocket->Close();
    }
    return NOERROR;
}

ECode SocketHttpServerConnection::Close()
{
    if (!mOpen) {
        return NOERROR;
    }
    mOpen = FALSE;
    DoFlush();
    // try {
    mSocket->ShutdownOutput();
    // } catch (IOException ignore) {
    // }
    // try {
    mSocket->ShutdownInput();
    // } catch (IOException ignore) {
    // }
    mSocket->Close();
    return NOERROR;
}

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org