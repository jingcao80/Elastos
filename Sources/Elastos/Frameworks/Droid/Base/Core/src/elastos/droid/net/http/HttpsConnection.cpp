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

#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include "_Org.Conscrypt.h"
#include "elastos/droid/net/http/HttpsConnection.h"
#include "elastos/droid/net/Proxy.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/net/http/CCertificateChainValidator.h"
#include "elastos/droid/net/http/CCertificateChainValidatorHelper.h"
#include "elastos/droid/net/http/CElastosHttpClient.h"
#include "elastos/droid/net/http/CElastosHttpClientConnection.h"
#include "elastos/droid/net/http/CHeaders.h"
#include "elastos/droid/net/http/Connection.h"
#include "elastos/droid/net/http/HttpConnection.h"
#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/net/http/Request.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Utility::ILog;

using Elastos::Core::CObject;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Net::CSocket;
using Elastos::Net::ISocket;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::ILocale;
using Elastos::Utility::Logging::Logger;
using Elastosx::Net::ISocketFactory;
using Elastosx::Net::Ssl::EIID_ITrustManager;
using Elastosx::Net::Ssl::EIID_IX509TrustManager;
using Elastosx::Net::Ssl::ISSLContextSpi;
using Elastosx::Net::Ssl::ISSLSessionContext;
using Elastosx::Net::Ssl::ISSLSocket;
using Elastosx::Net::Ssl::ISSLSocketFactory;
using Elastosx::Net::Ssl::ITrustManager;
using Elastosx::Net::Ssl::IX509TrustManager;

using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHttpConnection;
using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IHttpStatus;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::Message::CBasicHttpRequest;
using Org::Apache::Http::Message::IBasicHttpRequest;
using Org::Apache::Http::Params::CBasicHttpParams;
using Org::Apache::Http::Params::CHttpConnectionParams;
using Org::Apache::Http::Params::IBasicHttpParams;
using Org::Apache::Http::Params::ICoreConnectionPNames;
using Org::Apache::Http::Params::IHttpConnectionParams;
using Org::Apache::Http::Params::IHttpParams;
using Org::Conscrypt::CFileClientSessionCache;
using Org::Conscrypt::COpenSSLContextImpl;
using Org::Conscrypt::IClientSessionContext;
using Org::Conscrypt::IFileClientSessionCache;
using Org::Conscrypt::IOpenSSLContextImpl;
using Org::Conscrypt::ISSLClientSessionCache;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

//===========================================================
// HttpsConnection::InnerSub_X509TrustManager
//===========================================================
CAR_INTERFACE_IMPL(HttpsConnection::InnerSub_X509TrustManager, Object, IX509TrustManager)

ECode HttpsConnection::InnerSub_X509TrustManager::GetAcceptedIssuers(
    /* [out, callee] */ ArrayOf<IX509Certificate*>** result)
{
    *result = NULL;
    return NOERROR;
}

ECode HttpsConnection::InnerSub_X509TrustManager::CheckClientTrusted(
    /* [in] */ ArrayOf<IX509Certificate*>* chain,
    /* [in] */ const String& authType)
{
    return NOERROR;
}

ECode HttpsConnection::InnerSub_X509TrustManager::CheckServerTrusted(
    /* [in] */ ArrayOf<ICertificate*>* chain,
    /* [in] */ const String& authType)
{
    return NOERROR;
}


//===========================================================
// HttpsConnection::StaticInitializer
//===========================================================
HttpsConnection::StaticInitializer::StaticInitializer()
{
    // TODO:
    // This initialization happens in the zygote. It triggers some
    // lazy initialization that can will benefit later invocations of
    // initializeEngine().
    // InitializeEngine(NULL);
}

//===========================================================
// HttpsConnection
//===========================================================
AutoPtr<ISSLSocketFactory> HttpsConnection::sSslSocketFactory;
Object HttpsConnection::sLock;
const HttpsConnection::StaticInitializer HttpsConnection::sInitializer;

CAR_INTERFACE_IMPL(HttpsConnection, Connection, IHttpsConnection)

HttpsConnection::HttpsConnection()
    : mSuspended(FALSE)
    , mAborted(FALSE)
{
    Elastos::Core::CObject::New((IObject**)&mSuspendLock);
}

ECode HttpsConnection::InitializeEngine(
    /* [in] */ IFile* sessionDir)
{
    AutoPtr<ISSLClientSessionCache> cache;
    if (sessionDir != NULL) {
        String sSessionDir;
        IObject::Probe(sessionDir)->ToString(&sSessionDir);
        Logger::D(String("HttpsConnection"), String("Caching SSL sessions in ")
                + sSessionDir + String("."));
        AutoPtr<IFileClientSessionCache> helper;

        CFileClientSessionCache::AcquireSingleton((IFileClientSessionCache**)&helper);
        FAIL_RETURN(helper->UsingDirectory(sessionDir, (ISSLClientSessionCache**)&cache));
    }

    AutoPtr<IOpenSSLContextImpl> sslContext;
    COpenSSLContextImpl::New((IOpenSSLContextImpl**)&sslContext);

    // here, trust managers is a single trust-all manager
    AutoPtr<ArrayOf<ITrustManager*> > trustManagers = ArrayOf<ITrustManager*>::Alloc(1);
    AutoPtr<ITrustManager> manager;
    InnerSub_X509TrustManager* cmanager = new InnerSub_X509TrustManager();
    manager = ITrustManager::Probe(cmanager);
    (*trustManagers)[0] = manager;

    FAIL_RETURN(ISSLContextSpi::Probe(sslContext)->EngineInit(NULL, trustManagers, NULL));

    AutoPtr<ISSLSessionContext> context;
    FAIL_RETURN(ISSLContextSpi::Probe(sslContext)->EngineGetClientSessionContext((ISSLSessionContext**)&context));
    FAIL_RETURN(IClientSessionContext::Probe(context)->SetPersistentCache(cache));

    {
        AutoLock syncLock(sLock);
        return ISSLContextSpi::Probe(sslContext)->EngineGetSocketFactory((ISSLSocketFactory**)&sSslSocketFactory);
    }
}

AutoPtr<ISSLSocketFactory> HttpsConnection::GetSocketFactory()
{
    return sSslSocketFactory;
}

ECode HttpsConnection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHttpHost* host,
    /* [in] */ IHttpHost* proxy,
    /* [in] */ IRequestFeeder* requestFeeder)
{
    Connection::constructor(context, host, requestFeeder);
    mProxyHost = proxy;
    return NOERROR;
}

ECode HttpsConnection::SetCertificate(
    /* [in] */ ISslCertificate* certificate)
{
    mCertificate = certificate;
    return NOERROR;
}

ECode HttpsConnection::OpenConnection(
    /* [in] */ IRequest* req,
    /* [out] */ IElastosHttpClientConnection** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISSLSocket> sslSock;

    if (mProxyHost != NULL) {
        // If we have a proxy set, we first send a CONNECT request
        // to the proxy; if the proxy returns 200 OK, we negotiate
        // a secure connection to the target server via the proxy.
        // If the request fails, we drop it, but provide the event
        // handler with the response status and headers. The event
        // handler is then responsible for cancelling the load or
        // issueing a new request.
        AutoPtr<IElastosHttpClientConnection> proxyConnection;
        AutoPtr<ISocket> proxySock;

        String name;
        Int32 port;
        mProxyHost->GetHostName(&name);
        mProxyHost->GetPort(&port);
        CSocket::New(name, port, (ISocket**)&proxySock);

        ECode ec;
        ec = proxySock->SetSoTimeout(60 * 1000);

        CElastosHttpClientConnection::New((IElastosHttpClientConnection**)&proxyConnection);
        AutoPtr<IHttpParams> params;
        CBasicHttpParams::New((IHttpParams**)&params);
        AutoPtr<IHttpConnectionParams> helper;
        CHttpConnectionParams::AcquireSingleton((IHttpConnectionParams**)&helper);
        ec = helper->SetSocketBufferSize(params, 8192);

        ec = proxyConnection->Bind(proxySock, params);

        AutoPtr<IStatusLine> statusLine;
        Int32 statusCode = 0;
        AutoPtr<IHeaders> headers;
        CHeaders::New((IHeaders**)&headers);

        AutoPtr<IBasicHttpRequest> proxyReq;
        String shost;
        mHost->ToHostString(&shost);
        // TODO: Waiting for CBasicHttpRequest
        assert(0);
        // CBasicHttpRequest::New(String("CONNECT"), shost, (IBasicHttpRequest**)&proxyReq);

        // add all 'proxy' headers from the original request, we also need
        // to add 'host' header unless we want proxy to answer us with a
        // 400 Bad Request
        AutoPtr<ArrayOf<IHeader*> > reqHeaders;
        // TODO: Waiting for IBasicHttpRequest
        assert(0);
        // ((Request*)req)->mHttpRequest->GetAllHeaders((ArrayOf<IHeader*>**)&reqHeaders);
        if (reqHeaders != NULL) {
            for (Int32 i = 0; i < reqHeaders->GetLength(); ++i) {
                AutoPtr<IHeader> h;
                h = (*reqHeaders)[i];
                String headerName;
                h->GetName(&headerName);
                headerName = headerName.ToLowerCase(/* ILocale::ROOT */);

                if (headerName.StartWith("proxy")
                        || headerName.Equals("keep-alive")
                        || headerName.Equals("host")) {
                    // TODO: Waiting for BasicHttpRequest
                    assert(0);
                    // proxyReq->AddHeader(h);
                }
            }
        }

        AutoPtr<IHttpRequest> httpReq = IHttpRequest::Probe(proxyReq);
        ec = proxyConnection->SendRequestHeader(httpReq);
        if (ec == (ECode)E_PARSE_EXCEPTION
                || ec == (ECode)E_HTTP_EXCEPTION
                || ec == (ECode)E_IO_EXCEPTION ) {
            Logger::E("HttpsConnection", String("failed to send a CONNECT request"));
            return ec;
        }
        ec = proxyConnection->Flush();
        if (ec == (ECode)E_PARSE_EXCEPTION
                || ec == (ECode)E_HTTP_EXCEPTION
                || ec == (ECode)E_IO_EXCEPTION ) {
            Logger::E("HttpsConnection", String("failed to send a CONNECT request"));
            return ec;
        }

        // it is possible to receive informational status
        // codes prior to receiving actual headers;
        // all those status codes are smaller than OK 200
        // a loop is a standard way of dealing with them
        do {
            statusLine = NULL;
            ec = proxyConnection->ParseResponseHeader(headers, (IStatusLine**)&statusLine);
            if (ec == (ECode)E_PARSE_EXCEPTION
                    || ec == (ECode)E_HTTP_EXCEPTION
                    || ec == (ECode)E_IO_EXCEPTION ) {
                Logger::E("HttpsConnection", String("failed to send a CONNECT request"));
                return ec;
            }
            statusLine->GetStatusCode(&statusCode);
        } while (statusCode < IHttpStatus::SC_OK);


        if (statusCode == IHttpStatus::SC_OK) {
            String hName;
            Int32 hPort;
            mHost->GetHostName(&hName);
            mHost->GetPort(&hPort);
            AutoPtr<ISocket> socket;
            ec = GetSocketFactory()->CreateSocket(proxySock, hName, hPort, TRUE, (ISocket**)&socket);
            sslSock = ISSLSocket::Probe(socket);
            if (FAILED(ec)) {
                if (sslSock != NULL) {
                    ISocket::Probe(sslSock)->Close();
                }

                Logger::E("HttpsConnection", String("failed to create an SSL socket"));
            }
        } else {
            // if the code is not OK, inform the event handler
            AutoPtr<IProtocolVersion> version;
            statusLine->GetProtocolVersion((IProtocolVersion**)&version);

            Int32 major;
            Int32 minor;
            version->GetMajor(&major);
            version->GetMinor(&minor);
            String phrase;
            statusLine->GetReasonPhrase(&phrase);
            ((Request*)req)->mEventHandler->Status(major, minor, statusCode, phrase);
            ((Request*)req)->mEventHandler->Headers(headers);
            ((Request*)req)->mEventHandler->EndData();

            proxyConnection->Close();

            // here, we return null to indicate that the original
            // request needs to be dropped
            *result = NULL;
            return NOERROR;
        }
    } else {
        // if we do not have a proxy, we simply connect to the host
        String hName;
        Int32 hPort;
        mHost->GetHostName(&hName);
        mHost->GetPort(&hPort);
        ECode ec;
        AutoPtr<ISocket> socket;
        ec = ISocketFactory::Probe(GetSocketFactory())->CreateSocket(hName, hPort, (ISocket**)&socket);
        sslSock = ISSLSocket::Probe(socket);
        if (FAILED(ec)) {
            if (sslSock != NULL) {
                ISocket::Probe(sslSock)->Close();
            }

            Logger::E("HttpsConnection", String("failed to create an SSL socket"));
        }
        ec = ISocket::Probe(sslSock)->SetSoTimeout(SOCKET_TIMEOUT);
    }

    // do handshake and validate server certificates
    AutoPtr<ICertificateChainValidatorHelper> helper;
    CCertificateChainValidatorHelper::AcquireSingleton((ICertificateChainValidatorHelper**)&helper);
    AutoPtr<ICertificateChainValidator> validator;
    helper->GetInstance((ICertificateChainValidator**)&validator);
    String hName;
    mHost->GetHostName(&hName);
    AutoPtr<ISslError> error;
    AutoPtr<IHttpsConnection> iThis = this;
    validator->DoHandshakeAndValidateServerCertificates(iThis, sslSock, hName, (ISslError**)&error);

    // Inform the user if there is a problem
    if (error != NULL) {
        // handleSslErrorRequest may immediately unsuspend if it wants to
        // allow the certificate anyway.
        // So we mark the connection as suspended, call handleSslErrorRequest
        // then check if we're still suspended and only wait if we actually
        // need to.
        {
            {    AutoLock syncLock(mSuspendLock);
                mSuspended = TRUE;
            }
        }
        // don't hold the lock while calling out to the event handler
        Boolean canHandle;
        AutoPtr<IEventHandler> handler;
        ((Request*)req)->GetEventHandler((IEventHandler**)&handler);
        handler->HandleSslErrorRequest(error, &canHandle);
        if(!canHandle) {
            String serr;
            IObject::Probe(error)->ToString(&serr);
            Logger::E("HttpsConnection", String("failed to handle ") + serr);
            return E_IO_EXCEPTION;
        }

        {
            {    AutoLock syncLock(mSuspendLock);
                if (mSuspended) {
                    // Put a limit on how long we are waiting; if the timeout
                    // expires (which should never happen unless you choose
                    // to ignore the SSL error dialog for a very long time),
                    // we wake up the thread and abort the request. This is
                    // to prevent us from stalling the network if things go
                    // very bad.
                    // TODO:
                    // mSuspendLock.Wait(10 * 60 * 1000);
                    if (mSuspended) {
                        // mSuspended is true if we have not had a chance to
                        // restart the connection yet (ie, the wait timeout
                        // has expired)
                        mSuspended = FALSE;
                        mAborted = TRUE;
                        if (HttpLog::LOGV) {
                            HttpLog::V(String("HttpsConnection.openConnection(): SSL timeout expired and request was cancelled!!!"));
                        }
                    }
                }
                if (mAborted) {
                    // The user decided not to use this unverified connection
                    // so close it immediately.
                    ISocket::Probe(sslSock)->Close();
                    Logger::E("HttpsConnection", String("connection closed by the user"));
                    return E_SSL_CONNECTION_CLOSED_BY_USER_EXCEPTION;
                }
            }
        }
    }

    // All went well, we have an open, verified connection.
    AutoPtr<IElastosHttpClientConnection> conn;
    CElastosHttpClientConnection::New((IElastosHttpClientConnection**)&conn);
    AutoPtr<IBasicHttpParams> params;
    CBasicHttpParams::New((IBasicHttpParams**)&params);
    AutoPtr<IHttpParams> httpParams;
    IHttpParams::Probe(params)->SetInt32Parameter(/* HttpConnectionParams.SOCKET_BUFFER_SIZE */ICoreConnectionPNames::SOCKET_BUFFER_SIZE, 8192, (IHttpParams**)&httpParams);
    AutoPtr<ISocket> sslBaseSock = ISocket::Probe(sslSock);
    AutoPtr<IHttpParams> baseParams = Org::Apache::Http::Params::IHttpParams::Probe(sslSock);
    conn->Bind(sslBaseSock, baseParams);

    *result = conn;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode HttpsConnection::CloseConnection()
{
    // if the connection has been suspended due to an SSL error
    if (mSuspended) {
        // wake up the network thread
        RestartConnection(FALSE);
    }

    ECode ec = NOERROR;
    Boolean isOpen;
    if (mHttpClientConnection != NULL && (mHttpClientConnection->IsOpen(&isOpen), isOpen)) {
        ec = mHttpClientConnection->Close();
    }
    if (FAILED(ec)) {
        if (HttpLog::LOGV) {
            String smHost;
            IObject::Probe(mHost)->ToString(&smHost);
            HttpLog::V(String("HttpsConnection.closeConnection(): failed closing connection ") + smHost);
        }
    }
    return ec;
}

ECode HttpsConnection::RestartConnection(
    /* [in] */ Boolean proceed)
{
    if (HttpLog::LOGV) {
        HttpLog::V(String("HttpsConnection.restartConnection(): proceed: ") + StringUtils::BooleanToString(proceed));
    }

    {    AutoLock syncLock(mSuspendLock);
        if (mSuspended) {
            mSuspended = FALSE;
            mAborted = !proceed;
            ISynchronize::Probe(mSuspendLock)->Notify();
        }
    }
    return NOERROR;
}

ECode HttpsConnection::GetScheme(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = String("https");
    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
