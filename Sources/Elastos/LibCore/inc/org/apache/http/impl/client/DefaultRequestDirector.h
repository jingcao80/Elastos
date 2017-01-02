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

#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTREQUESTDIRECTOR_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTREQUESTDIRECTOR_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/http/impl/client/RequestWrapper.h"
#include "org/apache/http/impl/client/RoutedRequest.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IMap;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IConnectionReuseStrategy;
using Org::Apache::Http::Auth::IAuthState;
using Org::Apache::Http::Client::IRequestDirector;
using Org::Apache::Http::Client::IHttpRequestRetryHandler;
using Org::Apache::Http::Client::IRedirectHandler;
using Org::Apache::Http::Client::IAuthenticationHandler;
using Org::Apache::Http::Client::IUserTokenHandler;
using Org::Apache::Http::Client::ICredentialsProvider;
using Org::Apache::Http::Conn::IClientConnectionManager;
using Org::Apache::Http::Conn::IConnectionKeepAliveStrategy;
using Org::Apache::Http::Conn::IManagedClientConnection;
using Org::Apache::Http::Conn::Routing::IHttpRoutePlanner;
using Org::Apache::Http::Conn::Routing::IHttpRoute;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Protocol::IHttpContext;
using Org::Apache::Http::Protocol::IHttpRequestExecutor;
using Org::Apache::Http::Protocol::IHttpProcessor;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * Default implementation of {@link RequestDirector}.
 * <br/>
 * This class replaces the <code>HttpMethodDirector</code> in HttpClient 3.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version $Revision: 676023 $
 *
 * @since 4.0
 */
class DefaultRequestDirector
    : public Object
    , public IRequestDirector
{
public:
    DefaultRequestDirector(
        /* [in] */ IHttpRequestExecutor* requestExec,
        /* [in] */ IClientConnectionManager* conman,
        /* [in] */ IConnectionReuseStrategy* reustrat,
        /* [in] */ IConnectionKeepAliveStrategy* kastrat,
        /* [in] */ IHttpRoutePlanner* rouplan,
        /* [in] */ IHttpProcessor* httpProcessor,
        /* [in] */ IHttpRequestRetryHandler* retryHandler,
        /* [in] */ IRedirectHandler* redirectHandler,
        /* [in] */ IAuthenticationHandler* targetAuthHandler,
        /* [in] */ IAuthenticationHandler* proxyAuthHandler,
        /* [in] */ IUserTokenHandler* userTokenHandler,
        /* [in] */ IHttpParams* params);

    CAR_INTERFACE_DECL()

    CARAPI Execute(
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpResponse** response);

protected:
    CARAPI RewriteRequestURI(
        /* [in] */ RequestWrapper* request,
        /* [in] */ IHttpRoute* route);

    /**
     * Returns the connection back to the connection manager
     * and prepares for retrieving a new connection during
     * the next request.
     */
    CARAPI ReleaseConnection();

    /**
     * Determines the route for a request.
     * Called by {@link #execute}
     * to determine the route for either the original or a followup request.
     *
     * @param target    the target host for the request.
     *                  Implementations may accept <code>null</code>
     *                  if they can still determine a route, for example
     *                  to a default target or by inspecting the request.
     * @param request   the request to execute
     * @param context   the context to use for the execution,
     *                  never <code>null</code>
     *
     * @return  the route the request should take
     *
     * @throws HttpException    in case of a problem
     */
    CARAPI DetermineRoute(
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpRoute** route);

    /**
     * Establishes the target route.
     *
     * @param route     the route to establish
     * @param context   the context for the request execution
     *
     * @throws HttpException    in case of a problem
     * @throws IOException      in case of an IO problem
     */
    CARAPI EstablishRoute(
        /* [in] */ IHttpRoute* route,
        /* [in] */ IHttpContext* context);

    /**
     * Creates a tunnel to the target server.
     * The connection must be established to the (last) proxy.
     * A CONNECT request for tunnelling through the proxy will
     * be created and sent, the response received and checked.
     * This method does <i>not</i> update the connection with
     * information about the tunnel, that is left to the caller.
     *
     * @param route     the route to establish
     * @param context   the context for request execution
     *
     * @return  <code>true</code> if the tunnelled route is secure,
     *          <code>false</code> otherwise.
     *          The implementation here always returns <code>false</code>,
     *          but derived classes may override.
     *
     * @throws HttpException    in case of a problem
     * @throws IOException      in case of an IO problem
     */
    CARAPI CreateTunnelToTarget(
        /* [in] */ IHttpRoute* route,
        /* [in] */ IHttpContext* context,
        /* [out] */ Boolean* result);

    /**
     * Creates a tunnel to an intermediate proxy.
     * This method is <i>not</i> implemented in this class.
     * It just throws an exception here.
     *
     * @param route     the route to establish
     * @param hop       the hop in the route to establish now.
     *                  <code>route.getHopTarget(hop)</code>
     *                  will return the proxy to tunnel to.
     * @param context   the context for request execution
     *
     * @return  <code>true</code> if the partially tunnelled connection
     *          is secure, <code>false</code> otherwise.
     *
     * @throws HttpException    in case of a problem
     * @throws IOException      in case of an IO problem
     */
    CARAPI CreateTunnelToProxy(
        /* [in] */ IHttpRoute* route,
        /* [in] */ Int32 hop,
        /* [in] */ IHttpContext* context,
        /* [out] */ Boolean* result);

    /**
     * Creates the CONNECT request for tunnelling.
     * Called by {@link #createTunnelToTarget createTunnelToTarget}.
     *
     * @param route     the route to establish
     * @param context   the context for request execution
     *
     * @return  the CONNECT request for tunnelling
     */
    CARAPI CreateConnectRequest(
        /* [in] */ IHttpRoute* route,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpRequest** request);

    /**
     * Analyzes a response to check need for a followup.
     *
     * @param roureq    the request and route.
     * @param response  the response to analayze
     * @param context   the context used for the current request execution
     *
     * @return  the followup request and route if there is a followup, or
     *          <code>null</code> if the response should be returned as is
     *
     * @throws HttpException    in case of a problem
     * @throws IOException      in case of an IO problem
     */
    CARAPI HandleResponse(
        /* [in] */ RoutedRequest* roureq,
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpContext* context,
        /* [out] */ RoutedRequest** request);

private:
    CARAPI_(AutoPtr<RequestWrapper>) WrapRequest(
        /* [in] */ IHttpRequest* request);

    /**
     * Shuts down the connection.
     * This method is called from a <code>catch</code> block in
     * {@link #execute execute} during exception handling.
     */
    CARAPI_(void) AbortConnection();

    CARAPI ProcessChallenges(
        /* [in] */ IMap* challenges,
        /* [in] */ IAuthState* authState,
        /* [in] */ IAuthenticationHandler* authHandler,
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpContext* context);

    CARAPI_(void) UpdateAuthState(
        /* [in] */ IAuthState* authState,
        /* [in] */ IHttpHost* host,
        /* [in] */ ICredentialsProvider* credsProvider);

protected:
    /** The connection manager. */
    AutoPtr<IClientConnectionManager> mConnManager;

    /** The route planner. */
    AutoPtr<IHttpRoutePlanner> mRoutePlanner;

    /** The connection re-use strategy. */
    AutoPtr<IConnectionReuseStrategy> mReuseStrategy;

    /** The keep-alive duration strategy. */
    AutoPtr<IConnectionKeepAliveStrategy> mKeepAliveStrategy;

    /** The request executor. */
    AutoPtr<IHttpRequestExecutor> mRequestExec;

    /** The HTTP protocol processor. */
    AutoPtr<IHttpProcessor> mHttpProcessor;

    /** The request retry handler. */
    AutoPtr<IHttpRequestRetryHandler> mRetryHandler;

    /** The redirect handler. */
    AutoPtr<IRedirectHandler> mRedirectHandler;

    /** The currently allocated connection. */
    AutoPtr<IManagedClientConnection> mManagedConn;

private:
    /** The target authentication handler. */
    AutoPtr<IAuthenticationHandler> mTargetAuthHandler;

    /** The proxy authentication handler. */
    AutoPtr<IAuthenticationHandler> mProxyAuthHandler;

    /** The user token handler. */
    AutoPtr<IUserTokenHandler> mUserTokenHandler;

    /** The HTTP parameters. */
    AutoPtr<IHttpParams> mParams;

    Int32 mRedirectCount;

    Int32 mMaxRedirects;

    AutoPtr<IAuthState> mTargetAuthState;

    AutoPtr<IAuthState> mProxyAuthState;
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTREQUESTDIRECTOR_H__
