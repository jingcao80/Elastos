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

#ifndef __ORG_APACHE_HTTP_IMPL_CONN_ABSTRACTCLIENTCONNADAPTER_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_ABSTRACTCLIENTCONNADAPTER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Core::IThread;
using Elastos::Net::IInetAddress;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastosx::Net::Ssl::ISSLSession;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IHttpConnection;
using Org::Apache::Http::IHttpInetConnection;
using Org::Apache::Http::IHttpClientConnection;
using Org::Apache::Http::IHttpConnectionMetrics;
using Org::Apache::Http::IHttpEntityEnclosingRequest;
using Org::Apache::Http::Conn::IOperatedClientConnection;
using Org::Apache::Http::Conn::IClientConnectionManager;
using Org::Apache::Http::Conn::IManagedClientConnection;
using Org::Apache::Http::Conn::IConnectionReleaseTrigger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

/**
 * Abstract adapter from {@link OperatedClientConnection operated} to
 * {@link ManagedClientConnection managed} client connections.
 * Read and write methods are delegated to the wrapped connection.
 * Operations affecting the connection state have to be implemented
 * by derived classes. Operations for querying the connection state
 * are delegated to the wrapped connection if there is one, or
 * return a default value if there is none.
 * <br/>
 * This adapter tracks the checkpoints for reusable communication states,
 * as indicated by {@link #markReusable markReusable} and queried by
 * {@link #isMarkedReusable isMarkedReusable}.
 * All send and receive operations will automatically clear the mark.
 * <br/>
 * Connection release calls are delegated to the connection manager,
 * if there is one. {@link #abortConnection abortConnection} will
 * clear the reusability mark first. The connection manager is
 * expected to tolerate multiple calls to the release method.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 *
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version   $Revision: 672969 $ $Date: 2008-06-30 18:09:50 -0700 (Mon, 30 Jun 2008) $
 *
 * @since 4.0
 */
class AbstractClientConnAdapter
    : public Object
    , public IManagedClientConnection
    , public IHttpClientConnection
    , public IHttpConnection
    , public IHttpInetConnection
    , public IConnectionReleaseTrigger
{
public:
    CAR_INTERFACE_DECL()

    CARAPI IsOpen(
        /* [out] */ Boolean* isOpen);

    CARAPI IsStale(
        /* [out] */ Boolean* isStale);

    CARAPI SetSocketTimeout(
        /* [in] */ Int32 timeout);

    CARAPI GetSocketTimeout(
        /* [out] */ Int32* timeout);

    CARAPI GetMetrics(
        /* [out] */ IHttpConnectionMetrics** metrics);

    CARAPI Flush();

    CARAPI IsResponseAvailable(
        /* [in] */ Int32 timeout,
        /* [out] */ Boolean* isAvailable);

    CARAPI ReceiveResponseEntity(
        /* [in] */ IHttpResponse* response);

    CARAPI ReceiveResponseHeader(
        /* [out] */ IHttpResponse** httpResponse);

    CARAPI SendRequestEntity(
        /* [in] */ IHttpEntityEnclosingRequest* request);

    CARAPI SendRequestHeader(
        /* [in] */ IHttpRequest* request);

    CARAPI GetLocalAddress(
        /* [out] */ IInetAddress** address);

    CARAPI GetLocalPort(
        /* [out] */ Int32* port);

    CARAPI GetRemoteAddress(
        /* [out] */ IInetAddress** address);

    CARAPI GetRemotePort(
        /* [out] */ Int32* port);

    CARAPI IsSecure(
        /* [out] */ Boolean* isSecure);

    CARAPI GetSSLSession(
        /* [out] */ ISSLSession** session);

    CARAPI MarkReusable();

    CARAPI UnmarkReusable();

    CARAPI IsMarkedReusable(
        /* [out] */ Boolean* isMarked);

    CARAPI SetIdleDuration(
        /* [in] */ Int64 duration,
        /* [in] */ ITimeUnit* unit);

    CARAPI ReleaseConnection();

    CARAPI AbortConnection();

protected:
    AbstractClientConnAdapter(
        /* [in] */ IClientConnectionManager* mgr,
        /* [in] */ IOperatedClientConnection* conn);

    /**
     * Detaches this adapter from the wrapped connection.
     * This adapter becomes useless.
     */
    CARAPI_(void) Detach();

    CARAPI_(AutoPtr<IOperatedClientConnection>) GetWrappedConnection();

    CARAPI_(AutoPtr<IClientConnectionManager>) GetManager();

    /**
     * Asserts that the connection has not been aborted.
     *
     * @throws InterruptedIOException   if the connection has been aborted
     */
    CARAPI AssertNotAborted();

    /**
     * Asserts that there is a wrapped connection to delegate to.
     *
     * @throws IllegalStateException    if there is no wrapped connection
     *                                  or connection has been aborted
     */
    CARAPI AssertValid(
        /* [in] */ IOperatedClientConnection* wrappedConn);

private:
    /** Thread that requested this connection. */
    AutoPtr<IThread> mExecutionThread;

    /**
     * The connection manager, if any.
     * This attribute MUST NOT be final, so the adapter can be detached
     * from the connection manager without keeping a hard reference there.
     */
    AutoPtr<IClientConnectionManager> mConnManager;

    /** The wrapped connection. */
    AutoPtr<IOperatedClientConnection> mWrappedConnection;

    /** The reusability marker. */
    Boolean mMarkedReusable;

    /** True if the connection has been aborted. */
    Boolean mAborted;

    /** The duration this is valid for while idle (in ms). */
    Int64 mDuration;
};

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_ABSTRACTCLIENTCONNADAPTER_H__
