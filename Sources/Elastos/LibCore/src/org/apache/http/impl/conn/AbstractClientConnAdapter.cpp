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

#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Net.h"
#include "org/apache/http/impl/conn/AbstractClientConnAdapter.h"
#include "elastos/core/Thread.h"
#include "elastos/core/Math.h"
#include "elastos/utility/concurrent/CTimeUnitHelper.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::Thread;
using Elastos::Core::Math;
using Elastos::Net::ISocket;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Logging::Logger;
using Elastosx::Net::Ssl::ISSLSocket;
using Org::Apache::Http::Conn::EIID_IManagedClientConnection;
using Org::Apache::Http::Conn::EIID_IConnectionReleaseTrigger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

AbstractClientConnAdapter::AbstractClientConnAdapter(
    /* [in] */ IClientConnectionManager* mgr,
    /* [in] */ IOperatedClientConnection* conn)
    : mConnManager(mgr)
    , mWrappedConnection(conn)
    , mMarkedReusable(FALSE)
    , mAborted(FALSE)
    , mDuration(Elastos::Core::Math::INT64_MAX_VALUE)
{
    mExecutionThread = Thread::GetCurrentThread();
}

CAR_INTERFACE_IMPL_5(AbstractClientConnAdapter, Object
        , IManagedClientConnection
        , IHttpClientConnection
        , IHttpConnection
        , IHttpInetConnection
        , IConnectionReleaseTrigger)

void AbstractClientConnAdapter::Detach()
{
    mWrappedConnection = NULL;
    mConnManager = NULL; // base class attribute
    mDuration = Elastos::Core::Math::INT64_MAX_VALUE;
}

AutoPtr<IOperatedClientConnection> AbstractClientConnAdapter::GetWrappedConnection()
{
    return mWrappedConnection;
}

AutoPtr<IClientConnectionManager> AbstractClientConnAdapter::GetManager()
{
    return mConnManager;
}

ECode AbstractClientConnAdapter::AssertNotAborted()
{
    if (mAborted) {
        Logger::E("AbstractClientConnAdapter", "Connection has been shut down.");
        return E_INTERRUPTED_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode AbstractClientConnAdapter::AssertValid(
    /* [in] */ IOperatedClientConnection* wrappedConn)
{
    if (wrappedConn == NULL) {
        Logger::E("AbstractClientConnAdapter", "No wrapped connection.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode AbstractClientConnAdapter::IsOpen(
    /* [out] */ Boolean* isOpen)
{
    VALIDATE_NOT_NULL(isOpen)
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    if (conn == NULL) {
        *isOpen = FALSE;
        return NOERROR;
    }
    return IHttpConnection::Probe(conn)->IsOpen(isOpen);
}

ECode AbstractClientConnAdapter::IsStale(
    /* [out] */ Boolean* isStale)
{
    VALIDATE_NOT_NULL(isStale)
    if (mAborted) {
        *isStale = TRUE;
        return NOERROR;
    }
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    if (conn == NULL) {
        *isStale = TRUE;
        return NOERROR;
    }
    return IHttpConnection::Probe(conn)->IsStale(isStale);
}

ECode AbstractClientConnAdapter::SetSocketTimeout(
    /* [in] */ Int32 timeout)
{
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))
    return IHttpConnection::Probe(conn)->SetSocketTimeout(timeout);
}

ECode AbstractClientConnAdapter::GetSocketTimeout(
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout)
    *timeout = 0;
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))
    return IHttpConnection::Probe(conn)->GetSocketTimeout(timeout);
}

ECode AbstractClientConnAdapter::GetMetrics(
    /* [out] */ IHttpConnectionMetrics** metrics)
{
    VALIDATE_NOT_NULL(metrics)
    *metrics = NULL;
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))
    return IHttpConnection::Probe(conn)->GetMetrics(metrics);
}

ECode AbstractClientConnAdapter::Flush()
{
    FAIL_RETURN(AssertNotAborted())
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))

    return IHttpClientConnection::Probe(conn)->Flush();
}

ECode AbstractClientConnAdapter::IsResponseAvailable(
    /* [in] */ Int32 timeout,
    /* [out] */ Boolean* isAvailable)
{
    VALIDATE_NOT_NULL(isAvailable)
    *isAvailable = FALSE;
    FAIL_RETURN(AssertNotAborted())
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))

    return IHttpClientConnection::Probe(conn)->IsResponseAvailable(timeout, isAvailable);
}

ECode AbstractClientConnAdapter::ReceiveResponseEntity(
    /* [in] */ IHttpResponse* response)
{
    FAIL_RETURN(AssertNotAborted())
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))

    UnmarkReusable();
    return IHttpClientConnection::Probe(conn)->ReceiveResponseEntity(response);
}

ECode AbstractClientConnAdapter::ReceiveResponseHeader(
    /* [out] */ IHttpResponse** httpResponse)
{
    VALIDATE_NOT_NULL(httpResponse)
    *httpResponse = NULL;
    FAIL_RETURN(AssertNotAborted())
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))

    UnmarkReusable();
    return IHttpClientConnection::Probe(conn)->ReceiveResponseHeader(httpResponse);
}

ECode AbstractClientConnAdapter::SendRequestEntity(
    /* [in] */ IHttpEntityEnclosingRequest* request)
{
    FAIL_RETURN(AssertNotAborted())
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))

    UnmarkReusable();
    return IHttpClientConnection::Probe(conn)->SendRequestEntity(request);
}

ECode AbstractClientConnAdapter::SendRequestHeader(
    /* [in] */ IHttpRequest* request)
{
    FAIL_RETURN(AssertNotAborted())
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))

    UnmarkReusable();
    return IHttpClientConnection::Probe(conn)->SendRequestHeader(request);
}

ECode AbstractClientConnAdapter::GetLocalAddress(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address)
    *address = NULL;
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))
    return IHttpInetConnection::Probe(conn)->GetLocalAddress(address);
}

ECode AbstractClientConnAdapter::GetLocalPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port)
    *port = 0;
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))
    return IHttpInetConnection::Probe(conn)->GetLocalPort(port);
}

ECode AbstractClientConnAdapter::GetRemoteAddress(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address)
    *address = NULL;
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))
    return IHttpInetConnection::Probe(conn)->GetRemoteAddress(address);
}

ECode AbstractClientConnAdapter::GetRemotePort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port)
    *port = 0;
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))
    return IHttpInetConnection::Probe(conn)->GetRemotePort(port);
}

ECode AbstractClientConnAdapter::IsSecure(
    /* [out] */ Boolean* isSecure)
{
    VALIDATE_NOT_NULL(isSecure)
    *isSecure = FALSE;
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))
    return conn->IsSecure(isSecure);
}

ECode AbstractClientConnAdapter::GetSSLSession(
    /* [out] */ ISSLSession** session)
{
    VALIDATE_NOT_NULL(session)
    *session = NULL;
    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    FAIL_RETURN(AssertValid(conn))
    Boolean isOpen;
    if (IsOpen(&isOpen), !isOpen) {
        *session = NULL;
        return NOERROR;
    }

    AutoPtr<ISSLSession> result;
    AutoPtr<ISocket> sock;
    conn->GetSocket((ISocket**)&sock);
    AutoPtr<ISSLSocket> sslSocket = ISSLSocket::Probe(sock);
    if (sslSocket != NULL) {
        sslSocket->GetSession((ISSLSession**)&result);
    }
    *session = result;
    REFCOUNT_ADD(*session)
    return NOERROR;
}

ECode AbstractClientConnAdapter::MarkReusable()
{
    mMarkedReusable = TRUE;
    return NOERROR;
}

ECode AbstractClientConnAdapter::UnmarkReusable()
{
    mMarkedReusable = FALSE;
    return NOERROR;
}

ECode AbstractClientConnAdapter::IsMarkedReusable(
    /* [out] */ Boolean* isMarked)
{
    VALIDATE_NOT_NULL(isMarked)
    *isMarked = mMarkedReusable;
    return NOERROR;
}

ECode AbstractClientConnAdapter::SetIdleDuration(
    /* [in] */ Int64 duration,
    /* [in] */ ITimeUnit* unit)
{
    if(duration > 0) {
        unit->ToMillis(duration, &mDuration);
    }
    else {
        mDuration = -1;
    }
    return NOERROR;
}

ECode AbstractClientConnAdapter::ReleaseConnection()
{
    if (mConnManager != NULL) {
        AutoPtr<ITimeUnitHelper> helper;
        CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&helper);
        AutoPtr<ITimeUnit> milliseconds;
        helper->GetMILLISECONDS((ITimeUnit**)&milliseconds);
        mConnManager->ReleaseConnection(this, mDuration, milliseconds);
    }
    return NOERROR;
}

ECode AbstractClientConnAdapter::AbortConnection()
{
    if (mAborted) {
        return NOERROR;
    }
    mAborted = TRUE;
    UnmarkReusable();
    // try {
    Shutdown();
    // } catch (IOException ignore) {
    // }
    // Usually #abortConnection() is expected to be called from
    // a helper thread in order to unblock the main execution thread
    // blocked in an I/O operation. It may be unsafe to call
    // #releaseConnection() from the helper thread, so we have to rely
    // on an IOException thrown by the closed socket on the main thread
    // to trigger the release of the connection back to the
    // connection manager.
    //
    // However, if this method is called from the main execution thread
    // it should be safe to release the connection immediately. Besides,
    // this also helps ensure the connection gets released back to the
    // manager if #abortConnection() is called from the main execution
    // thread while there is no blocking I/O operation.
    Boolean equals;
    if (IObject::Probe(mExecutionThread)->Equals(Thread::GetCurrentThread(), &equals), equals) {
        ReleaseConnection();
    }
    return NOERROR;
}

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org