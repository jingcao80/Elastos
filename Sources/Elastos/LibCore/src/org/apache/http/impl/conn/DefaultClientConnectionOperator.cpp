
#include "org/apache/http/impl/conn/DefaultClientConnectionOperator.h"
#include "org/apache/http/impl/conn/DefaultClientConnection.h"
#include "org/apache/http/params/HttpConnectionParams.h"
#include "org/apache/http/conn/scheme/CPlainSocketFactory.h"
#include "elastos/net/CInetAddressHelper.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Net::IInetAddressHelper;
using Elastos::Net::CInetAddressHelper;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Conn::EIID_IClientConnectionOperator;
using Org::Apache::Http::Conn::Scheme::IScheme;
using Org::Apache::Http::Conn::Scheme::ISocketFactory;
using Org::Apache::Http::Conn::Scheme::ILayeredSocketFactory;
using Org::Apache::Http::Conn::Scheme::CPlainSocketFactory;
using Org::Apache::Http::Params:: HttpConnectionParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

static AutoPtr<IPlainSocketFactory> InitPlainSocketFactory()
{
    AutoPtr<CPlainSocketFactory> psf;
    CPlainSocketFactory::NewByFriend((CPlainSocketFactory**)&psf);
    return (IPlainSocketFactory*)psf.Get();
}
const AutoPtr<IPlainSocketFactory> DefaultClientConnectionOperator::sStaticPlainSocketFactory = InitPlainSocketFactory();

DefaultClientConnectionOperator::DefaultClientConnectionOperator(
    /* [in] */ ISchemeRegistry* schemes)
{
    if (schemes == NULL) {
        Logger::E("DefaultClientConnectionOperator", "Scheme registry must not be null.");
        assert(0);
        // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSchemeRegistry = schemes;
}

CAR_INTERFACE_IMPL(DefaultClientConnectionOperator, Object, IClientConnectionOperator)

ECode DefaultClientConnectionOperator::CreateConnection(
    /* [out] */ IOperatedClientConnection** connection)
{
    VALIDATE_NOT_NULL(connection)
    *connection = (IOperatedClientConnection*)new DefaultClientConnection();
    REFCOUNT_ADD(*connection)
    return NOERROR;
}

ECode DefaultClientConnectionOperator::OpenConnection(
    /* [in] */ IOperatedClientConnection* conn,
    /* [in] */ IHttpHost* target,
    /* [in] */ IInetAddress* local,
    /* [in] */ IHttpContext* context,
    /* [in] */ IHttpParams* params)
{
    if (conn == NULL) {
        Logger::E("DefaultClientConnection", "Connection must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (target == NULL) {
        Logger::E("DefaultClientConnection", "Target host must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // local address may be null
    //@@@ is context allowed to be null?
    if (params == NULL) {
        Logger::E("DefaultClientConnection", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean isOpen;
    if (IHttpConnection::Probe(conn)->IsOpen(&isOpen), isOpen) {
        Logger::E("DefaultClientConnection", "Connection must not be open.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String name;
    target->GetSchemeName(&name);
    AutoPtr<IScheme> schm;
    mSchemeRegistry->GetScheme(name, (IScheme**)&schm);
    AutoPtr<ISocketFactory> sf;
    schm->GetSocketFactory((ISocketFactory**)&sf);
    AutoPtr<ISocketFactory> plain_sf;
    AutoPtr<ILayeredSocketFactory> layered_sf;
    if (ILayeredSocketFactory::Probe(sf) != NULL) {
        plain_sf = ISocketFactory::Probe(sStaticPlainSocketFactory);
        layered_sf = ILayeredSocketFactory::Probe(sf);
    }
    else {
        plain_sf = sf;
        layered_sf = NULL;
    }
    AutoPtr<IInetAddressHelper> helper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
    AutoPtr< ArrayOf<IInetAddress*> > addresses;
    helper->GetAllByName(name, (ArrayOf<IInetAddress*>**)&addresses);

    for (Int32 i = 0; i < addresses->GetLength(); ++i) {
        AutoPtr<ISocket> sock;
        plain_sf->CreateSocket((ISocket**)&sock);
        conn->Opening(sock, target);

        // try {
        String addr;
        (*addresses)[i]->GetHostAddress(&addr);
        Int32 port;
        target->GetPort(&port);
        Int32 resolvePort;
        schm->ResolvePort(port, &resolvePort);
        AutoPtr<ISocket> connsock;
        plain_sf->ConnectSocket(sock, addr, resolvePort, local, 0, params, (ISocket**)&connsock);
        if (sock != connsock) {
            sock = connsock;
            conn->Opening(sock, target);
        }
        /*
         * prepareSocket is called on the just connected
         * socket before the creation of the layered socket to
         * ensure that desired socket options such as
         * TCP_NODELAY, SO_RCVTIMEO, SO_LINGER will be set
         * before any I/O is performed on the socket. This
         * happens in the common case as
         * SSLSocketFactory.createSocket performs hostname
         * verification which requires that SSL handshaking be
         * performed.
         */
        PrepareSocket(sock, context, params);
        if (layered_sf != NULL) {
            AutoPtr<ISocket> layeredsock;
            String host;
            target->GetHostName(&host);
            target->GetPort(&port);
            schm->ResolvePort(port, &resolvePort);
            layered_sf->CreateSocket(sock, host, resolvePort, TRUE, (ISocket**)&layeredsock);
            if (layeredsock != sock) {
                conn->Opening(layeredsock, target);
            }
            Boolean isSecure;
            sf->IsSecure(layeredsock, &isSecure);
            conn->OpenCompleted(isSecure, params);
        }
        else {
            Boolean isSecure;
            sf->IsSecure(sock, &isSecure);
            conn->OpenCompleted(isSecure, params);
        }
        break;
        // BEGIN android-changed
        //       catch SocketException to cover any kind of connect failure
        // } catch (SocketException ex) {
        //     if (i == addresses.length - 1) {
        //         ConnectException cause = ex instanceof ConnectException
        //                 ? (ConnectException) ex : new ConnectException(ex.getMessage(), ex);
        //         throw new HttpHostConnectException(target, cause);
        //     }
        // // END android-changed
        // } catch (ConnectTimeoutException ex) {
        //     if (i == addresses.length - 1) {
        //         throw ex;
        //     }
        // }
    }
    return NOERROR;
}

ECode DefaultClientConnectionOperator::UpdateSecureConnection(
    /* [in] */ IOperatedClientConnection* conn,
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpContext* context,
    /* [in] */ IHttpParams* params)
{
    if (conn == NULL) {
        Logger::E("DefaultClientConnection", "Connection must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (target == NULL) {
        Logger::E("DefaultClientConnection", "Target host must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //@@@ is context allowed to be null?
    if (params == NULL) {
        Logger::E("DefaultClientConnection", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean isOpen;
    if (IHttpConnection::Probe(conn)->IsOpen(&isOpen), !isOpen) {
        Logger::E("DefaultClientConnection", "Connection must be open.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String name;
    target->GetSchemeName(&name);
    AutoPtr<IScheme> schm;
    mSchemeRegistry->GetScheme(name, (IScheme**)&schm);
    AutoPtr<ISocketFactory> sf;
    schm->GetSocketFactory((ISocketFactory**)&sf);
    AutoPtr<ILayeredSocketFactory> lsf = ILayeredSocketFactory::Probe(sf);
    if (lsf == NULL) {
        String schemeName;
        schm->GetName(&schemeName);
        Logger::E("DefaultClientConnection", "Target scheme (%s) must have layered socket factory.", schemeName.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ISocket> sock;
    // try {
    AutoPtr<ISocket> connSock;
    conn->GetSocket((ISocket**)&connSock);
    String hostname;
    target->GetHostName(&hostname);
    Int32 port;
    target->GetPort(&port);
    Int32 resolvePort;
    schm->ResolvePort(port, &resolvePort);
    lsf->CreateSocket(connSock, hostname, resolvePort, TRUE, (ISocket**)&sock);
    // } catch (ConnectException ex) {
    //     throw new HttpHostConnectException(target, ex);
    // }
    PrepareSocket(sock, context, params);
    Boolean isSecure;
    ISocketFactory::Probe(lsf)->IsSecure(sock, &isSecure);
    conn->Update(sock, target, isSecure, params);
    //@@@ error handling: close the layered socket in case of exception?
    return NOERROR;
}

ECode DefaultClientConnectionOperator::PrepareSocket(
    /* [in] */ ISocket* sock,
    /* [in] */ IHttpContext* context,
    /* [in] */ IHttpParams* params)
{
    // context currently not used, but derived classes may need it
    //@@@ is context allowed to be null?

    Boolean result;
    HttpConnectionParams::GetTcpNoDelay(params, &result);
    sock->SetTcpNoDelay(result);
    Int32 timeout;
    HttpConnectionParams::GetSoTimeout(params, &timeout);
    sock->SetSoTimeout(timeout);

    Int32 linger;
    HttpConnectionParams::GetLinger(params, &linger);
    if (linger >= 0) {
        sock->SetSoLinger(linger > 0, linger);
    }
    return NOERROR;
}

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org