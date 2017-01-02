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

#include "org/apache/http/impl/conn/DefaultClientConnection.h"
#include "org/apache/http/impl/conn/DefaultResponseParser.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Conn::EIID_IOperatedClientConnection;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

DefaultClientConnection::DefaultClientConnection()
    : SocketHttpClientConnection()
    , mConnSecure(FALSE)
    , mShutdown(FALSE)
{}

CAR_INTERFACE_IMPL(DefaultClientConnection, SocketHttpClientConnection, IOperatedClientConnection)

ECode DefaultClientConnection::GetTargetHost(
    /* [out] */ IHttpHost** host)
{
    VALIDATE_NOT_NULL(host)
    *host = mTargetHost;
    REFCOUNT_ADD(*host)
    return NOERROR;
}

ECode DefaultClientConnection::IsSecure(
    /* [out] */ Boolean* isSecure)
{
    VALIDATE_NOT_NULL(isSecure)
    *isSecure = mConnSecure;
    return NOERROR;
}

ECode DefaultClientConnection::GetSocket(
    /* [out] */ ISocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    *socket = mSocket;
    REFCOUNT_ADD(*socket)
    return NOERROR;
}

ECode DefaultClientConnection::Opening(
    /* [in] */ ISocket* sock,
    /* [in] */ IHttpHost* target)
{
    FAIL_RETURN(AssertNotOpen())
    mSocket = sock;
    mTargetHost = target;

    // Check for shutdown after assigning socket, so that
    if (mShutdown) {
        sock->Close(); // allow this to throw...
        // ...but if it doesn't, explicitly throw one ourselves.
        Logger::E("DefaultClientConnection", "Connection already shutdown");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode DefaultClientConnection::OpenCompleted(
    /* [in] */ Boolean secure,
    /* [in] */ IHttpParams* params)
{
    FAIL_RETURN(AssertNotOpen())
    if (params == NULL) {
        Logger::E("DefaultClientConnection", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mConnSecure = secure;
    Bind(mSocket, params);
    return NOERROR;
}

ECode DefaultClientConnection::Shutdown()
{
    // log.debug("Connection shut down");
    Logger::D("DefaultClientConnection", "Connection shut down");
    mShutdown = TRUE;

    SocketHttpClientConnection::Shutdown();
    AutoPtr<ISocket> sock = mSocket; // copy volatile attribute
    if (sock != NULL) sock->Close();
    return NOERROR;
}

ECode DefaultClientConnection::Close()
{
    // log.debug("Connection closed");
    Logger::D("DefaultClientConnection", "Connection closed");
    SocketHttpClientConnection::Close();
    return NOERROR;
}

ECode DefaultClientConnection::CreateSessionInputBuffer(
    /* [in] */ ISocket* socket,
    /* [in] */ Int32 buffersize,
    /* [in] */ IHttpParams* params,
    /* [out] */ ISessionInputBuffer** inputBuffer)
{
    VALIDATE_NOT_NULL(inputBuffer)
    AutoPtr<ISessionInputBuffer> inbuffer;
    SocketHttpClientConnection::CreateSessionInputBuffer(socket, buffersize, params,
            (ISessionInputBuffer**)&inbuffer);
    // if (wireLog.isDebugEnabled()) {
    //     inbuffer = new LoggingSessionInputBuffer(inbuffer, new Wire(wireLog));
    // }
    *inputBuffer = inbuffer;
    REFCOUNT_ADD(*inputBuffer)
    return NOERROR;
}

ECode DefaultClientConnection::CreateSessionOutputBuffer(
    /* [in] */ ISocket* socket,
    /* [in] */ Int32 buffersize,
    /* [in] */ IHttpParams* params,
    /* [out] */ ISessionOutputBuffer** outputBuffer)
{
    VALIDATE_NOT_NULL(outputBuffer)
    AutoPtr<ISessionOutputBuffer> outbuffer;
    SocketHttpClientConnection::CreateSessionOutputBuffer(socket, buffersize, params,
            (ISessionOutputBuffer**)&outbuffer);
    // if (wireLog.isDebugEnabled()) {
    //     outbuffer = new LoggingSessionOutputBuffer(outbuffer, new Wire(wireLog));
    // }
    *outputBuffer = outbuffer;
    REFCOUNT_ADD(*outputBuffer)
    return NOERROR;
}

AutoPtr<IHttpMessageParser> DefaultClientConnection::CreateResponseParser(
    /* [in] */ ISessionInputBuffer* buffer,
    /* [in] */ IHttpResponseFactory* responseFactory,
    /* [in] */ IHttpParams* params)
{
    // override in derived class to specify a line parser
    AutoPtr<IHttpMessageParser> parser = (IHttpMessageParser*)new DefaultResponseParser
            (buffer, NULL, responseFactory, params);
    return parser;
}

ECode DefaultClientConnection::Update(
    /* [in] */ ISocket* sock,
    /* [in] */ IHttpHost* target,
    /* [in] */ Boolean secure,
    /* [in] */ IHttpParams* params)
{
    FAIL_RETURN(AssertOpen())
    if (target == NULL) {
        Logger::E("DefaultClientConnection", "Target host must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        Logger::E("DefaultClientConnection", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (sock != NULL) {
        mSocket = sock;
        Bind(sock, params);
    }
    mTargetHost = target;
    mConnSecure = secure;
    return NOERROR;
}

ECode DefaultClientConnection::ReceiveResponseHeader(
    /* [out] */ IHttpResponse** httpResponse)
{
    VALIDATE_NOT_NULL(httpResponse)
    AutoPtr<IHttpResponse> response;
    SocketHttpClientConnection::ReceiveResponseHeader((IHttpResponse**)&response);
    // if (headerLog.isDebugEnabled()) {
    //     headerLog.debug("<< " + response.getStatusLine().toString());
    //     Header[] headers = response.getAllHeaders();
    //     for (Header header : headers) {
    //         headerLog.debug("<< " + header.toString());
    //     }
    // }
    *httpResponse = response;
    REFCOUNT_ADD(*httpResponse)
    return NOERROR;
}

ECode DefaultClientConnection::SendRequestHeader(
    /* [in] */ IHttpRequest* request)
{
    return SocketHttpClientConnection::SendRequestHeader(request);
    // if (headerLog.isDebugEnabled()) {
    //     headerLog.debug(">> " + request.getRequestLine().toString());
    //     Header[] headers = request.getAllHeaders();
    //     for (Header header : headers) {
    //         headerLog.debug(">> " + header.toString());
    //     }
    // }
}

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org