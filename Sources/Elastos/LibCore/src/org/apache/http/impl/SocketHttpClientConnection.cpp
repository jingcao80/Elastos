
#include "org/apache/http/impl/SocketHttpClientConnection.h"
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

SocketHttpClientConnection::SocketHttpClientConnection()
    : AbstractHttpClientConnection()
    , mOpen(FALSE)
{}

CAR_INTERFACE_IMPL(SocketHttpClientConnection, AbstractHttpClientConnection, IHttpInetConnection)

ECode SocketHttpClientConnection::AssertNotOpen()
{
    if (mOpen) {
        Logger::E("SocketHttpClientConnection", "Connection is already open");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode SocketHttpClientConnection::AssertOpen()
{
    if (!mOpen) {
        Logger::E("SocketHttpClientConnection", "Connection is not open");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode SocketHttpClientConnection::CreateSessionInputBuffer(
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

ECode SocketHttpClientConnection::CreateSessionOutputBuffer(
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

ECode SocketHttpClientConnection::Bind(
    /* [in] */ ISocket* socket,
    /* [in] */ IHttpParams* params)
{
    if (socket == NULL) {
        Logger::E("SocketHttpClientConnection", "Socket may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        Logger::E("SocketHttpClientConnection", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSocket = socket;

    Int32 buffersize;
    HttpConnectionParams::GetSocketBufferSize(params, &buffersize);

    AutoPtr<ISessionInputBuffer> in;
    CreateSessionInputBuffer(socket, buffersize, params, (ISessionInputBuffer**)&in);
    AutoPtr<ISessionOutputBuffer> out;
    CreateSessionOutputBuffer(socket, buffersize, params, (ISessionOutputBuffer**)&out);
    Init(in, out, params);

    mOpen = TRUE;
    return NOERROR;
}

ECode SocketHttpClientConnection::IsOpen(
    /* [out] */ Boolean* isOpen)
{
    VALIDATE_NOT_NULL(isOpen)
    *isOpen = mOpen;
    return NOERROR;
}

AutoPtr<ISocket> SocketHttpClientConnection::GetSocket()
{
    return mSocket;
}

ECode SocketHttpClientConnection::GetLocalAddress(
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

ECode SocketHttpClientConnection::GetLocalPort(
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

ECode SocketHttpClientConnection::GetRemoteAddress(
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

ECode SocketHttpClientConnection::GetRemotePort(
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

ECode SocketHttpClientConnection::SetSocketTimeout(
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

ECode SocketHttpClientConnection::GetSocketTimeout(
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

ECode SocketHttpClientConnection::Shutdown()
{
    mOpen = FALSE;
    AutoPtr<ISocket> tmpsocket = mSocket;
    if (tmpsocket != NULL) {
        tmpsocket->Close();
    }
    return NOERROR;
}

ECode SocketHttpClientConnection::Close()
{
    if (!mOpen) {
        return NOERROR;
    }
    mOpen = FALSE;
    DoFlush();
    // try {
        // try {
    mSocket->ShutdownOutput();
    // } catch (IOException ignore) {
    // }
    // try {
    mSocket->ShutdownInput();
    // } catch (IOException ignore) {
    // }
    // } catch (UnsupportedOperationException ignore) {
    //     // if one isn't supported, the other one isn't either
    // }
    mSocket->Close();
    return NOERROR;
}

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org