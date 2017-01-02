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

#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_ABSTRACTHTTPCLIENT_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_ABSTRACTHTTPCLIENT_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IMap;
using Elastos::Utility::IList;
using Org::Apache::Http::IConnectionReuseStrategy;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::IHttpResponseInterceptor;
using Org::Apache::Http::IHttpRequestInterceptor;
using Org::Apache::Http::Auth::IAuthSchemeRegistry;
using Org::Apache::Http::Client::IHttpClient;
using Org::Apache::Http::Client::IAuthenticationHandler;
using Org::Apache::Http::Client::ICookieStore;
using Org::Apache::Http::Client::ICredentialsProvider;
using Org::Apache::Http::Client::IHttpRequestRetryHandler;
using Org::Apache::Http::Client::IRedirectHandler;
using Org::Apache::Http::Client::IRequestDirector;
using Org::Apache::Http::Client::IResponseHandler;
using Org::Apache::Http::Client::IUserTokenHandler;
using Org::Apache::Http::Client::Methods::IHttpUriRequest;
using Org::Apache::Http::Conn::IClientConnectionManager;
using Org::Apache::Http::Conn::IConnectionKeepAliveStrategy;
using Org::Apache::Http::Conn::Routing::IHttpRoutePlanner;
using Org::Apache::Http::Cookie::ICookieSpecRegistry;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Protocol::IBasicHttpProcessor;
using Org::Apache::Http::Protocol::IHttpContext;
using Org::Apache::Http::Protocol::IHttpRequestExecutor;
using Org::Apache::Http::Protocol::IHttpProcessor;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * Convenience base class for HTTP client implementations.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version   $Revision: 677250 $
 *
 * @since 4.0
 */
class AbstractHttpClient
    : public Object
    , public IHttpClient
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetParams(
        /* [out] */ IHttpParams** params);

    /**
     * Replaces the parameters.
     * The implementation here does not update parameters of dependent objects.
     *
     * @param params    the new default parameters
     */
    CARAPI_(void) SetParams(
        /* [in] */ IHttpParams* params);

    CARAPI GetConnectionManager(
        /* [out] */ IClientConnectionManager** manager);

    CARAPI GetRequestExecutor(
        /* [out] */ IHttpRequestExecutor** executor);

    CARAPI GetAuthSchemes(
        /* [out] */ IAuthSchemeRegistry** registry);

    CARAPI_(void) SetAuthSchemes(
        /* [in] */ IAuthSchemeRegistry* authSchemeRegistry);

    CARAPI GetCookieSpecs(
        /* [out] */ ICookieSpecRegistry** registry);

    CARAPI_(void) SetCookieSpecs(
        /* [in] */ ICookieSpecRegistry* cookieSpecRegistry);

    CARAPI GetConnectionReuseStrategy(
        /* [out] */ IConnectionReuseStrategy** strategy);

    CARAPI_(void) SetReuseStrategy(
        /* [in] */ IConnectionReuseStrategy* reuseStrategy);

    CARAPI GetConnectionKeepAliveStrategy(
        /* [out] */ IConnectionKeepAliveStrategy** strategy);

    CARAPI_(void) SetKeepAliveStrategy(
        /* [in] */ IConnectionKeepAliveStrategy* keepAliveStrategy);

    CARAPI GetHttpRequestRetryHandler(
        /* [out] */ IHttpRequestRetryHandler** handler);

    CARAPI_(void) SetHttpRequestRetryHandler(
        /* [in] */ IHttpRequestRetryHandler* retryHandler);

    CARAPI GetRedirectHandler(
        /* [out] */ IRedirectHandler** handler);

    CARAPI SetRedirectHandler(
        /* [in] */ IRedirectHandler* redirectHandler);

    CARAPI GetTargetAuthenticationHandler(
        /* [out] */ IAuthenticationHandler** handler);

    CARAPI_(void) SetTargetAuthenticationHandler(
        /* [in] */ IAuthenticationHandler* targetAuthHandler);

    CARAPI GetProxyAuthenticationHandler(
        /* [out] */ IAuthenticationHandler** handler);

    CARAPI_(void) SetProxyAuthenticationHandler(
        /* [in] */ IAuthenticationHandler* proxyAuthHandler);

    CARAPI GetCookieStore(
        /* [out] */ ICookieStore** store);

    CARAPI_(void) SetCookieStore(
        /* [in] */ ICookieStore* cookieStore);

    CARAPI GetCredentialsProvider(
        /* [out] */ ICredentialsProvider** provider);

    CARAPI_(void) SetCredentialsProvider(
        /* [in] */ ICredentialsProvider* credsProvider);

    CARAPI GetRoutePlanner(
        /* [out] */ IHttpRoutePlanner** planner);

    CARAPI_(void) SetRoutePlanner(
        /* [in] */ IHttpRoutePlanner* routePlanner);

    CARAPI GetUserTokenHandler(
        /* [out] */ IUserTokenHandler** handler);

    CARAPI_(void) SetUserTokenHandler(
        /* [in] */ IUserTokenHandler* userTokenHandler);

    CARAPI GetHttpProcessor(
        /* [out] */ IBasicHttpProcessor** processor);

    CARAPI AddResponseInterceptor(
        /* [in] */ IHttpResponseInterceptor* itcp);

    CARAPI AddResponseInterceptor(
        /* [in] */ IHttpResponseInterceptor* itcp,
        /* [in] */ Int32 index);

    CARAPI GetResponseInterceptor(
        /* [in] */ Int32 index,
        /* [out] */ IHttpResponseInterceptor** interceptor);

    CARAPI GetResponseInterceptorCount(
        /* [out] */ Int32* count);

    CARAPI ClearResponseInterceptors();

    CARAPI RemoveResponseInterceptorByClass(
        /* [in] */ IHttpResponseInterceptor* clazz);

    CARAPI AddRequestInterceptor(
        /* [in] */ IHttpRequestInterceptor* itcp);

    CARAPI AddRequestInterceptor(
        /* [in] */ IHttpRequestInterceptor* itcp,
        /* [in] */ Int32 index);

    CARAPI GetRequestInterceptor(
        /* [in] */ Int32 index,
        /* [out] */ IHttpRequestInterceptor** interceptor);

    CARAPI GetRequestInterceptorCount(
        /* [out] */ Int32* count);

    CARAPI ClearRequestInterceptors();

    CARAPI RemoveRequestInterceptorByClass(
        /* [in] */ IHttpRequestInterceptor* clazz);

    CARAPI Execute(
        /* [in] */ IHttpUriRequest* request,
        /* [out] */ IHttpResponse** response);

    /**
     * Maps to {@link HttpClient#execute(HttpHost,HttpRequest,HttpContext)
     *                           CARAPI Execute(target, request, context)}.
     * The target is determined from the URI of the request.
     *
     * @param request   the request to execute
     * @param context   the request-specific execution context,
     *                  or <code>null</code> to use a default context
     */
    CARAPI Execute(
        /* [in] */ IHttpUriRequest* request,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpResponse** response);

    CARAPI Execute(
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpRequest* request,
        /* [out] */ IHttpResponse** response);

    CARAPI Execute(
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpResponse** response);

    CARAPI Execute(
        /* [in] */ IHttpUriRequest* request,
        /* [in] */ IResponseHandler* responseHandler,
        /* [out] */ IInterface** response);

    CARAPI Execute(
        /* [in] */ IHttpUriRequest* request,
        /* [in] */ IResponseHandler* responseHandler,
        /* [in] */ IHttpContext* context,
        /* [out] */ IInterface** response);

    CARAPI Execute(
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpRequest* request,
        /* [in] */ IResponseHandler* responseHandler,
        /* [out] */ IInterface** response);

    CARAPI Execute(
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpRequest* request,
        /* [in] */ IResponseHandler* responseHandler,
        /* [in] */ IHttpContext* context,
        /* [out] */ IInterface** response);

protected:
    /**
     * Creates a new HTTP client.
     *
     * @param conman    the connection manager
     * @param params    the parameters
     */
    AbstractHttpClient(
        /* [in] */ IClientConnectionManager* conman,
        /* [in] */ IHttpParams* params);

    virtual CARAPI CreateHttpParams(
        /* [out] */ IHttpParams** params) = 0;

    virtual CARAPI CreateHttpContext(
        /* [out] */ IHttpContext** context) = 0;

    virtual CARAPI CreateRequestExecutor(
        /* [out] */ IHttpRequestExecutor** executor) = 0;

    virtual CARAPI CreateClientConnectionManager(
        /* [out] */ IClientConnectionManager** manager) = 0;

    virtual CARAPI CreateAuthSchemeRegistry(
        /* [out] */ IAuthSchemeRegistry** registry) = 0;

    virtual CARAPI CreateCookieSpecRegistry(
        /* [out] */ ICookieSpecRegistry** registry) = 0;

    virtual CARAPI CreateConnectionReuseStrategy(
        /* [out] */ IConnectionReuseStrategy** strategy) = 0;

    virtual CARAPI CreateConnectionKeepAliveStrategy(
        /* [out] */ IConnectionKeepAliveStrategy** strategy) = 0;

    virtual CARAPI CreateHttpProcessor(
        /* [out] */ IBasicHttpProcessor** processor) = 0;

    virtual CARAPI CreateHttpRequestRetryHandler(
        /* [out] */ IHttpRequestRetryHandler** handler) = 0;

    virtual CARAPI CreateRedirectHandler(
        /* [out] */ IRedirectHandler** handler) = 0;

    virtual CARAPI CreateTargetAuthenticationHandler(
        /* [out] */ IAuthenticationHandler** handler) = 0;

    virtual CARAPI CreateProxyAuthenticationHandler(
        /* [out] */ IAuthenticationHandler** handler) = 0;

    virtual CARAPI CreateCookieStore(
        /* [out] */ ICookieStore** store) = 0;

    virtual CARAPI CreateCredentialsProvider(
        /* [out] */ ICredentialsProvider** provider) = 0;

    virtual CARAPI CreateHttpRoutePlanner(
        /* [out] */ IHttpRoutePlanner** planner) = 0;

    virtual CARAPI CreateUserTokenHandler(
        /* [out] */ IUserTokenHandler** handler) = 0;

    CARAPI_(AutoPtr<IRequestDirector>) CreateClientRequestDirector(
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
        /* [in] */ IUserTokenHandler* stateHandler,
        /* [in] */ IHttpParams* params);

    /**
     * Obtains parameters for executing a request.
     * The default implementation in this class creates a new
     * {@link ClientParamsStack} from the request parameters
     * and the client parameters.
     * <br/>
     * This method is called by the default implementation of
     * {@link #execute(HttpHost,HttpRequest,HttpContext)}
     * to obtain the parameters for the
     * {@link DefaultRequestDirector}.
     *
     * @param req    the request that will be executed
     *
     * @return  the parameters to use
     */
    CARAPI_(AutoPtr<IHttpParams>) DetermineParams(
        /* [in] */ IHttpRequest* req);

private:
    CARAPI_(AutoPtr<IHttpHost>) DetermineTarget(
        /* [in] */ IHttpUriRequest* request);

private:
    /** The parameters. */
    AutoPtr<IHttpParams> mDefaultParams;

    /** The request executor. */
    AutoPtr<IHttpRequestExecutor> mRequestExec;

    /** The connection manager. */
    AutoPtr<IClientConnectionManager> mConnManager;

    /** The connection re-use strategy. */
    AutoPtr<IConnectionReuseStrategy> mReuseStrategy;

    /** The connection keep-alive strategy. */
    AutoPtr<IConnectionKeepAliveStrategy> mKeepAliveStrategy;

    /** The cookie spec registry. */
    AutoPtr<ICookieSpecRegistry> mSupportedCookieSpecs;

    /** The authentication scheme registry. */
    AutoPtr<IAuthSchemeRegistry> mSupportedAuthSchemes;

    /** The HTTP processor. */
    AutoPtr<IBasicHttpProcessor> mHttpProcessor;

    /** The request retry handler. */
    AutoPtr<IHttpRequestRetryHandler> mRetryHandler;

    /** The redirect handler. */
    AutoPtr<IRedirectHandler> mRedirectHandler;

    /** The target authentication handler. */
    AutoPtr<IAuthenticationHandler> mTargetAuthHandler;

    /** The proxy authentication handler. */
    AutoPtr<IAuthenticationHandler> mProxyAuthHandler;

    /** The cookie store. */
    AutoPtr<ICookieStore> mCookieStore;

    /** The credentials provider. */
    AutoPtr<ICredentialsProvider> mCredsProvider;

    /** The route planner. */
    AutoPtr<IHttpRoutePlanner> mRoutePlanner;

    /** The user token handler. */
    AutoPtr<IUserTokenHandler> mUserTokenHandler;
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_ABSTRACTHTTPCLIENT_H__
