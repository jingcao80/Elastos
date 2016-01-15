
#include "elastos/droid/net/http/HttpConnection.h"
#include "elastos/droid/net/http/CElastosHttpClientConnection.h"
#include "elastos/droid/net/http/Connection.h"
#include "elastos/droid/net/http/ElastosHttpClient.h"
#include "elastos/droid/net/http/ElastosHttpClientConnection.h"
#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/net/http/Request.h"
#include "elastos/droid/net/ReturnOutValue.h"
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
    ECode ec = NOERROR;
    if (mHttpClientConnection != NULL) {
        if (Ptr(mHttpClientConnection)->Func(mHttpClientConnection->IsOpen)) {
            ec = mHttpClientConnection->Close();
            if (FAILED(ec)) {
                if (HttpLog::LOGV) {
                    String shost;
                    IObject::Probe(mHost)->ToString(&shost);
                    HttpLog::V("closeConnection(): failed closing connection %s", shost.string());
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
