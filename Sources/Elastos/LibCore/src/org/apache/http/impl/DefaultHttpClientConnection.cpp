
#include "org/apache/http/impl/DefaultHttpClientConnection.h"
#include "org/apache/http/params/HttpConnectionParams.h"
#include "elastos/utility/logging/Logger.h"
#include "elastos/core/StringBuilder.h"

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Params::HttpConnectionParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

DefaultHttpClientConnection::DefaultHttpClientConnection()
    : SocketHttpClientConnection()
{}

ECode DefaultHttpClientConnection::Bind(
    /* [in] */ ISocket* socket,
    /* [in] */ IHttpParams* params)
{
    if (socket == NULL) {
        Logger::E("DefaultHttpClientConnection", "Socket may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        Logger::E("DefaultHttpClientConnection", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(AssertNotOpen())
    Boolean result;
    HttpConnectionParams::GetTcpNoDelay(params, &result);
    socket->SetTcpNoDelay(result);
    Int32 timeout;
    HttpConnectionParams::GetSoTimeout(params, &timeout);
    socket->SetSoTimeout(timeout);

    Int32 linger;
    HttpConnectionParams::GetLinger(params, &linger);
    if (linger >= 0) {
        socket->SetSoLinger(linger > 0, linger);
    }
    return SocketHttpClientConnection::Bind(socket, params);
}

ECode DefaultHttpClientConnection::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder b;
    b.Append("[");
    Boolean isOpen;
    if (IsOpen(&isOpen), isOpen) {
        Int32 port;
        GetRemotePort(&port);
        b.Append(port);
    }
    else {
        b.Append("closed");
    }
    b.Append("]");
    *str = b.ToString();
    return NOERROR;
}

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org