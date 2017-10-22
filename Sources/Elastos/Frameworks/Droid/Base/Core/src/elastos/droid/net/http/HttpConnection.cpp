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

#include "elastos/droid/net/http/HttpConnection.h"
#include "elastos/droid/net/http/CElastosHttpClientConnection.h"
#include "elastos/droid/net/http/Connection.h"
#include "elastos/droid/net/http/ElastosHttpClient.h"
#include "elastos/droid/net/http/ElastosHttpClientConnection.h"
#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/net/http/Request.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;

using Elastos::Net::CSocket;
using Elastos::Net::ISocket;

using Org::Apache::Http::IHttpConnection;
using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::Params::CBasicHttpParams;
using Org::Apache::Http::Params::IBasicHttpParams;
using Org::Apache::Http::Params::ICoreConnectionPNames;
using Org::Apache::Http::Params::IHttpConnectionParams;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

HttpConnection::HttpConnection()
{}

ECode HttpConnection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHttpHost* host,
    /* [in] */ IRequestFeeder* requestFeeder)
{
    return Connection::constructor(context, host, requestFeeder);
}

ECode HttpConnection::OpenConnection(
    /* [in] */ IRequest* req,
    /* [out] */ IElastosHttpClientConnection** result)
{
    VALIDATE_NOT_NULL(result)

    // Update the certificate info (connection not secure - set to null)
    AutoPtr<IEventHandler> eventHandler;
    ((Request*)req)->GetEventHandler((IEventHandler**)&eventHandler);
    mCertificate = NULL;
    eventHandler->Certificate(mCertificate);

    AutoPtr<IElastosHttpClientConnection> conn;
    CElastosHttpClientConnection::New((IElastosHttpClientConnection**)&conn);
    AutoPtr<IBasicHttpParams> params;
    CBasicHttpParams::New((IBasicHttpParams**)&params);
    AutoPtr<ISocket> sock;
    String hostName;
    Int32 port;
    mHost->GetHostName(&hostName);
    mHost->GetPort(&port);
    CSocket::New(hostName, port, (ISocket**)&sock);
    AutoPtr<IHttpParams> tmp;
    IHttpParams::Probe(params)->SetInt32Parameter(ICoreConnectionPNames::SOCKET_BUFFER_SIZE, 8192, (IHttpParams**)&tmp);
    conn->Bind(sock, IHttpParams::Probe(params));

    *result = conn;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode HttpConnection::CloseConnection()
{
    if (mHttpClientConnection != NULL) {
        Boolean open;
        if (mHttpClientConnection->IsOpen(&open), open) {
            ECode ec = mHttpClientConnection->Close();
            if (FAILED(ec)) {
                if (HttpLog::LOGV) {
                    HttpLog::V("closeConnection(): failed closing connection %s", TO_CSTR(mHost));
                }
            }
        }
    }

    return NOERROR;
}

ECode HttpConnection::RestartConnection(
    /* [in] */ Boolean abort)
{
    // not required for plain http connections
    return NOERROR;
}

ECode HttpConnection::GetScheme(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = String("http");
    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
