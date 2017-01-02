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

#include "org/apache/http/impl/DefaultHttpServerConnection.h"
#include "org/apache/http/params/HttpConnectionParams.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Params::HttpConnectionParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

DefaultHttpServerConnection::DefaultHttpServerConnection()
    : SocketHttpServerConnection()
{}

ECode DefaultHttpServerConnection::Bind(
    /* [in] */ ISocket* socket,
    /* [in] */ IHttpParams* params)
{
    if (socket == NULL) {
        Logger::E("DefaultHttpServerConnection", "Socket may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        Logger::E("DefaultHttpServerConnection", "HTTP parameters may not be null");
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
    return SocketHttpServerConnection::Bind(socket, params);
}

ECode DefaultHttpServerConnection::ToString(
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