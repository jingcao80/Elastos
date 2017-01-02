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

#include "Elastos.CoreLibrary.Net.h"
#include "org/apache/http/impl/client/AbstractHttpClient.h"
#include "org/apache/http/impl/client/DefaultRequestDirector.h"
#include "org/apache/http/impl/client/ClientParamsStack.h"
#include "org/apache/http/CHttpHost.h"
#include "org/apache/http/protocol/CDefaultedHttpContext.h"
#include "elastos/core/AutoLock.h"
#include "elastos/utility/logging/Logger.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;
using Elastos::Net::IURI;
using Org::Apache::Http::Client::EIID_IHttpClient;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::CHttpHost;
using Org::Apache::Http::Protocol::IDefaultedHttpContext;
using Org::Apache::Http::Protocol::CDefaultedHttpContext;
using Org::Apache::Http::Protocol::IHttpRequestInterceptorList;
using Org::Apache::Http::Protocol::IHttpResponseInterceptorList;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

AbstractHttpClient::AbstractHttpClient(
    /* [in] */ IClientConnectionManager* conman,
    /* [in] */ IHttpParams* params)
    : mDefaultParams(params)
    , mConnManager(conman)
{}

CAR_INTERFACE_IMPL(AbstractHttpClient, Object, IHttpClient)

ECode AbstractHttpClient::GetParams(
    /* [out] */ IHttpParams** params)
{
    VALIDATE_NOT_NULL(params)
    {    AutoLock syncLock(this);
        if (mDefaultParams == NULL) {
            CreateHttpParams((IHttpParams**)&mDefaultParams);
        }
        *params = mDefaultParams;
        REFCOUNT_ADD(*params)
    }
    return NOERROR;
}

void AbstractHttpClient::SetParams(
    /* [in] */ IHttpParams* params)
{
    {    AutoLock syncLock(this);
        mDefaultParams = params;
    }
}

ECode AbstractHttpClient::GetConnectionManager(
    /* [out] */ IClientConnectionManager** manager)
{
    VALIDATE_NOT_NULL(manager)
    {    AutoLock syncLock(this);
        if (mConnManager == NULL) {
            CreateClientConnectionManager((IClientConnectionManager**)&mConnManager);
        }
        *manager = mConnManager;
        REFCOUNT_ADD(*manager)
    }
    return NOERROR;
}

ECode AbstractHttpClient::GetRequestExecutor(
    /* [out] */ IHttpRequestExecutor** executor)
{
    VALIDATE_NOT_NULL(executor)
    {    AutoLock syncLock(this);
        if (mRequestExec == NULL) {
            CreateRequestExecutor((IHttpRequestExecutor**)&mRequestExec);
        }
        *executor = mRequestExec;
        REFCOUNT_ADD(*executor)
    }
    return NOERROR;
}

ECode AbstractHttpClient::GetAuthSchemes(
    /* [out] */ IAuthSchemeRegistry** registry)
{
    VALIDATE_NOT_NULL(registry)
    {    AutoLock syncLock(this);
        if (mSupportedAuthSchemes == NULL) {
            CreateAuthSchemeRegistry((IAuthSchemeRegistry**)&mSupportedAuthSchemes);
        }
        *registry = mSupportedAuthSchemes;
        REFCOUNT_ADD(*registry)
    }
    return NOERROR;
}

void AbstractHttpClient::SetAuthSchemes(
    /* [in] */ IAuthSchemeRegistry* authSchemeRegistry)
{
    {    AutoLock syncLock(this);
        mSupportedAuthSchemes = authSchemeRegistry;
    }
}

ECode AbstractHttpClient::GetCookieSpecs(
    /* [out] */ ICookieSpecRegistry** registry)
{
    VALIDATE_NOT_NULL(registry)
    {    AutoLock syncLock(this);
        if (mSupportedCookieSpecs == NULL) {
            CreateCookieSpecRegistry((ICookieSpecRegistry**)&mSupportedCookieSpecs);
        }
        *registry = mSupportedCookieSpecs;
        REFCOUNT_ADD(*registry)
    }
    return NOERROR;
}

void AbstractHttpClient::SetCookieSpecs(
    /* [in] */ ICookieSpecRegistry* cookieSpecRegistry)
{
    {    AutoLock syncLock(this);
        mSupportedCookieSpecs = cookieSpecRegistry;
    }
}

ECode AbstractHttpClient::GetConnectionReuseStrategy(
    /* [out] */ IConnectionReuseStrategy** strategy)
{
    VALIDATE_NOT_NULL(strategy)
    {    AutoLock syncLock(this);
        if (mReuseStrategy == NULL) {
            CreateConnectionReuseStrategy((IConnectionReuseStrategy**)&mReuseStrategy);
        }
        *strategy = mReuseStrategy;
        REFCOUNT_ADD(*strategy)
    }
    return NOERROR;
}

void AbstractHttpClient::SetReuseStrategy(
    /* [in] */ IConnectionReuseStrategy* reuseStrategy)
{
    {    AutoLock syncLock(this);
        mReuseStrategy = reuseStrategy;
    }
}

ECode AbstractHttpClient::GetConnectionKeepAliveStrategy(
    /* [out] */ IConnectionKeepAliveStrategy** strategy)
{
    VALIDATE_NOT_NULL(strategy)
    {    AutoLock syncLock(this);
        if (mKeepAliveStrategy == NULL) {
            CreateConnectionKeepAliveStrategy((IConnectionKeepAliveStrategy**)&mKeepAliveStrategy);
        }
        *strategy = mKeepAliveStrategy;
        REFCOUNT_ADD(*strategy)
    }
    return NOERROR;
}

void AbstractHttpClient::SetKeepAliveStrategy(
    /* [in] */ IConnectionKeepAliveStrategy* keepAliveStrategy)
{
    {    AutoLock syncLock(this);
        mKeepAliveStrategy = keepAliveStrategy;
    }
}

ECode AbstractHttpClient::GetHttpRequestRetryHandler(
    /* [out] */ IHttpRequestRetryHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    {    AutoLock syncLock(this);
        if (mRetryHandler == NULL) {
            CreateHttpRequestRetryHandler((IHttpRequestRetryHandler**)&mRetryHandler);
        }
        *handler = mRetryHandler;
        REFCOUNT_ADD(*handler)
    }
    return NOERROR;
}

void AbstractHttpClient::SetHttpRequestRetryHandler(
    /* [in] */ IHttpRequestRetryHandler* retryHandler)
{
    {    AutoLock syncLock(this);
        mRetryHandler = retryHandler;
    }
}

ECode AbstractHttpClient::GetRedirectHandler(
    /* [out] */ IRedirectHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    {    AutoLock syncLock(this);
        if (mRedirectHandler == NULL) {
            CreateRedirectHandler((IRedirectHandler**)&mRedirectHandler);
        }
        *handler = mRedirectHandler;
        REFCOUNT_ADD(*handler)
    }
    return NOERROR;
}

ECode AbstractHttpClient::SetRedirectHandler(
    /* [in] */ IRedirectHandler* redirectHandler)
{
    {    AutoLock syncLock(this);
        mRedirectHandler = redirectHandler;
    }
    return NOERROR;
}

ECode AbstractHttpClient::GetTargetAuthenticationHandler(
    /* [out] */ IAuthenticationHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    {    AutoLock syncLock(this);
        if (mTargetAuthHandler == NULL) {
            CreateTargetAuthenticationHandler((IAuthenticationHandler**)&mTargetAuthHandler);
        }
        *handler = mTargetAuthHandler;
        REFCOUNT_ADD(*handler)
    }
    return NOERROR;
}

void AbstractHttpClient::SetTargetAuthenticationHandler(
    /* [in] */ IAuthenticationHandler* targetAuthHandler)
{
    {    AutoLock syncLock(this);
        mTargetAuthHandler = targetAuthHandler;
    }
}

ECode AbstractHttpClient::GetProxyAuthenticationHandler(
    /* [out] */ IAuthenticationHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    {    AutoLock syncLock(this);
        if (mProxyAuthHandler == NULL) {
            CreateProxyAuthenticationHandler((IAuthenticationHandler**)&mProxyAuthHandler);
        }
        *handler = mProxyAuthHandler;
        REFCOUNT_ADD(*handler)
    }
    return NOERROR;
}

void AbstractHttpClient::SetProxyAuthenticationHandler(
    /* [in] */ IAuthenticationHandler* proxyAuthHandler)
{
    {    AutoLock syncLock(this);
        mProxyAuthHandler = proxyAuthHandler;
    }
}

ECode AbstractHttpClient::GetCookieStore(
    /* [out] */ ICookieStore** store)
{
    VALIDATE_NOT_NULL(store)
    {    AutoLock syncLock(this);
        if (mCookieStore == NULL) {
            CreateCookieStore((ICookieStore**)&mCookieStore);
        }
        *store = mCookieStore;
        REFCOUNT_ADD(*store)
    }
    return NOERROR;
}

void AbstractHttpClient::SetCookieStore(
    /* [in] */ ICookieStore* cookieStore)
{
    {    AutoLock syncLock(this);
        mCookieStore = cookieStore;
    }
}

ECode AbstractHttpClient::GetCredentialsProvider(
    /* [out] */ ICredentialsProvider** provider)
{
    VALIDATE_NOT_NULL(provider)
    {    AutoLock syncLock(this);
        if (mCredsProvider == NULL) {
            CreateCredentialsProvider((ICredentialsProvider**)&mCredsProvider);
        }
        *provider = mCredsProvider;
        REFCOUNT_ADD(*provider)
    }
    return NOERROR;
}

void AbstractHttpClient::SetCredentialsProvider(
    /* [in] */ ICredentialsProvider* credsProvider)
{
    {    AutoLock syncLock(this);
        mCredsProvider = credsProvider;
    }
}

ECode AbstractHttpClient::GetRoutePlanner(
    /* [out] */ IHttpRoutePlanner** planner)
{
    VALIDATE_NOT_NULL(planner)
    {    AutoLock syncLock(this);
        if (mRoutePlanner == NULL) {
            CreateHttpRoutePlanner((IHttpRoutePlanner**)&mRoutePlanner);
        }
        *planner = mRoutePlanner;
        REFCOUNT_ADD(*planner)
    }
    return NOERROR;
}

void AbstractHttpClient::SetRoutePlanner(
    /* [in] */ IHttpRoutePlanner* routePlanner)
{
    {    AutoLock syncLock(this);
        mRoutePlanner = routePlanner;
    }
}

ECode AbstractHttpClient::GetUserTokenHandler(
    /* [out] */ IUserTokenHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    {    AutoLock syncLock(this);
        if (mUserTokenHandler == NULL) {
            CreateUserTokenHandler((IUserTokenHandler**)&mUserTokenHandler);
        }
        *handler = mUserTokenHandler;
        REFCOUNT_ADD(*handler)
    }
    return NOERROR;
}

void AbstractHttpClient::SetUserTokenHandler(
    /* [in] */ IUserTokenHandler* userTokenHandler)
{
    {    AutoLock syncLock(this);
        mUserTokenHandler = userTokenHandler;
    }
}

ECode AbstractHttpClient::GetHttpProcessor(
    /* [out] */ IBasicHttpProcessor** processor)
{
    VALIDATE_NOT_NULL(processor)
    {    AutoLock syncLock(this);
        if (mHttpProcessor == NULL) {
            CreateHttpProcessor((IBasicHttpProcessor**)&mHttpProcessor);
        }
        *processor = mHttpProcessor;
        REFCOUNT_ADD(*processor)
    }
    return NOERROR;
}

ECode AbstractHttpClient::AddResponseInterceptor(
    /* [in] */ IHttpResponseInterceptor* itcp)
{
    {    AutoLock syncLock(this);
        AutoPtr<IBasicHttpProcessor> processor;
        GetHttpProcessor((IBasicHttpProcessor**)&processor);
        processor->AddInterceptor(itcp);
    }
    return NOERROR;
}

ECode AbstractHttpClient::AddResponseInterceptor(
    /* [in] */ IHttpResponseInterceptor* itcp,
    /* [in] */ Int32 index)
{
    {    AutoLock syncLock(this);
        AutoPtr<IBasicHttpProcessor> processor;
        GetHttpProcessor((IBasicHttpProcessor**)&processor);
        processor->AddInterceptor(itcp, index);
    }
    return NOERROR;
}

ECode AbstractHttpClient::GetResponseInterceptor(
    /* [in] */ Int32 index,
    /* [out] */ IHttpResponseInterceptor** interceptor)
{
    VALIDATE_NOT_NULL(interceptor)
    {    AutoLock syncLock(this);
        AutoPtr<IBasicHttpProcessor> processor;
        GetHttpProcessor((IBasicHttpProcessor**)&processor);
        IHttpResponseInterceptorList::Probe(processor)->GetResponseInterceptor(index, interceptor);
    }
    return NOERROR;
}

ECode AbstractHttpClient::GetResponseInterceptorCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    {    AutoLock syncLock(this);
        AutoPtr<IBasicHttpProcessor> processor;
        GetHttpProcessor((IBasicHttpProcessor**)&processor);
        IHttpResponseInterceptorList::Probe(processor)->GetResponseInterceptorCount(count);
    }
    return NOERROR;
}

ECode AbstractHttpClient::ClearResponseInterceptors()
{
    {    AutoLock syncLock(this);
        AutoPtr<IBasicHttpProcessor> processor;
        GetHttpProcessor((IBasicHttpProcessor**)&processor);
        IHttpResponseInterceptorList::Probe(processor)->ClearResponseInterceptors();
    }
    return NOERROR;
}

ECode AbstractHttpClient::RemoveResponseInterceptorByClass(
    /* [in] */ IHttpResponseInterceptor* clazz)
{
    AutoPtr<IBasicHttpProcessor> processor;
    GetHttpProcessor((IBasicHttpProcessor**)&processor);
    return IHttpResponseInterceptorList::Probe(processor)->RemoveResponseInterceptorByClass(clazz);
}

ECode AbstractHttpClient::AddRequestInterceptor(
    /* [in] */ IHttpRequestInterceptor* itcp)
{
    {    AutoLock syncLock(this);
        AutoPtr<IBasicHttpProcessor> processor;
        GetHttpProcessor((IBasicHttpProcessor**)&processor);
        IHttpRequestInterceptorList::Probe(processor)->AddRequestInterceptor(itcp);
    }
    return NOERROR;
}

ECode AbstractHttpClient::AddRequestInterceptor(
    /* [in] */ IHttpRequestInterceptor* itcp,
    /* [in] */ Int32 index)
{
    {    AutoLock syncLock(this);
        AutoPtr<IBasicHttpProcessor> processor;
        GetHttpProcessor((IBasicHttpProcessor**)&processor);
        IHttpRequestInterceptorList::Probe(processor)->AddRequestInterceptor(itcp, index);
    }
    return NOERROR;
}

ECode AbstractHttpClient::GetRequestInterceptor(
    /* [in] */ Int32 index,
    /* [out] */ IHttpRequestInterceptor** interceptor)
{
    VALIDATE_NOT_NULL(interceptor)
    {    AutoLock syncLock(this);
        AutoPtr<IBasicHttpProcessor> processor;
        GetHttpProcessor((IBasicHttpProcessor**)&processor);
        IHttpRequestInterceptorList::Probe(processor)->GetRequestInterceptor(index, interceptor);
    }
    return NOERROR;
}

ECode AbstractHttpClient::GetRequestInterceptorCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    {    AutoLock syncLock(this);
        AutoPtr<IBasicHttpProcessor> processor;
        GetHttpProcessor((IBasicHttpProcessor**)&processor);
        IHttpRequestInterceptorList::Probe(processor)->GetRequestInterceptorCount(count);
    }
    return NOERROR;
}

ECode AbstractHttpClient::ClearRequestInterceptors()
{
    {    AutoLock syncLock(this);
        AutoPtr<IBasicHttpProcessor> processor;
        GetHttpProcessor((IBasicHttpProcessor**)&processor);
        IHttpRequestInterceptorList::Probe(processor)->ClearRequestInterceptors();
    }
    return NOERROR;
}

ECode AbstractHttpClient::RemoveRequestInterceptorByClass(
    /* [in] */ IHttpRequestInterceptor* clazz)
{
    AutoPtr<IBasicHttpProcessor> processor;
    GetHttpProcessor((IBasicHttpProcessor**)&processor);
    return IHttpRequestInterceptorList::Probe(processor)->RemoveRequestInterceptorByClass(clazz);
}

ECode AbstractHttpClient::Execute(
    /* [in] */ IHttpUriRequest* request,
    /* [out] */ IHttpResponse** response)
{
    VALIDATE_NOT_NULL(response)
    return Execute(request, NULL, response);
}

ECode AbstractHttpClient::Execute(
    /* [in] */ IHttpUriRequest* request,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpResponse** response)
{
    VALIDATE_NOT_NULL(response)
    *response = NULL;
    if (request == NULL) {
        Logger::E("AbstractHttpClient", "Request must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IHttpHost> host = DetermineTarget(request);
    return Execute(host, IHttpRequest::Probe(request), context, response);
}

AutoPtr<IHttpHost> AbstractHttpClient::DetermineTarget(
    /* [in] */ IHttpUriRequest* request)
{
    // A null target may be acceptable if there is a default target.
    // Otherwise, the null target is detected in the director.
    AutoPtr<IHttpHost> target;

    AutoPtr<IURI> requestURI;
    request->GetURI((IURI**)&requestURI);
    Boolean isAbsolute;
    if (requestURI->IsAbsolute(&isAbsolute), isAbsolute) {
        String host, scheme;
        requestURI->GetHost(&host);
        requestURI->GetScheme(&scheme);
        Int32 port;
        requestURI->GetPort(&port);
        CHttpHost::New(host, port, scheme, (IHttpHost**)&target);
    }
    return target;
}

ECode AbstractHttpClient::Execute(
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpRequest* request,
    /* [out] */ IHttpResponse** response)
{
    VALIDATE_NOT_NULL(response)
    return Execute(target, request, NULL, response);
}

ECode AbstractHttpClient::Execute(
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpResponse** response)
{
    VALIDATE_NOT_NULL(response)
    *response = NULL;

    if (request == NULL) {
        Logger::E("AbstractHttpClient", "Request must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // a null target may be acceptable, this depends on the route planner
    // a null context is acceptable, default context created below

    AutoPtr<IHttpContext> execContext;
    AutoPtr<IRequestDirector> director;

    // Initialize the request execution context making copies of
    // all shared objects that are potentially threading unsafe.
    {    AutoLock syncLock(this);

        AutoPtr<IHttpContext> defaultContext;
        CreateHttpContext((IHttpContext**)&defaultContext);
        if (context == NULL) {
            execContext = defaultContext;
        }
        else {
            AutoPtr<IDefaultedHttpContext> dhc;
            CDefaultedHttpContext::New(context, defaultContext, (IDefaultedHttpContext**)&dhc);
            execContext = IHttpContext::Probe(dhc);
        }
        // Create a director for this request
        AutoPtr<IHttpRequestExecutor> executor;
        GetRequestExecutor((IHttpRequestExecutor**)&executor);
        AutoPtr<IClientConnectionManager> man;
        GetConnectionManager((IClientConnectionManager**)&man);
        AutoPtr<IConnectionReuseStrategy> strategy;
        GetConnectionReuseStrategy((IConnectionReuseStrategy**)&strategy);
        AutoPtr<IConnectionKeepAliveStrategy> keepAliveStrategy;
        GetConnectionKeepAliveStrategy((IConnectionKeepAliveStrategy**)&keepAliveStrategy);
        AutoPtr<IHttpRoutePlanner> planner;
        GetRoutePlanner((IHttpRoutePlanner**)&planner);
        AutoPtr<IBasicHttpProcessor> processor;
        GetHttpProcessor((IBasicHttpProcessor**)&processor);
        AutoPtr<IBasicHttpProcessor> copy;
        processor->Copy((IBasicHttpProcessor**)&copy);
        AutoPtr<IHttpRequestRetryHandler> requestRetryHandler;
        GetHttpRequestRetryHandler((IHttpRequestRetryHandler**)&requestRetryHandler);
        AutoPtr<IRedirectHandler> redirectHandler;
        GetRedirectHandler((IRedirectHandler**)&redirectHandler);
        AutoPtr<IAuthenticationHandler> authenticationHandler;
        GetTargetAuthenticationHandler((IAuthenticationHandler**)&authenticationHandler);
        AutoPtr<IAuthenticationHandler> proxyHandler;
        GetProxyAuthenticationHandler((IAuthenticationHandler**)&proxyHandler);
        AutoPtr<IUserTokenHandler> userTokenHandler;
        GetUserTokenHandler((IUserTokenHandler**)&userTokenHandler);
        AutoPtr<IHttpParams> params = DetermineParams(request);
        director = CreateClientRequestDirector(executor, man, strategy, keepAliveStrategy,
                planner, IHttpProcessor::Probe(copy), requestRetryHandler, redirectHandler, authenticationHandler,
                proxyHandler, userTokenHandler, params);
    }

    // try {
    ECode ec = director->Execute(target, request, execContext, response);
    if (ec == (ECode)E_HTTP_EXCEPTION) {
        *response = NULL;
        return E_CLIENT_PROTOCOL_EXCEPTION;
    }
    // } catch(HttpException httpException) {
    //     throw new ClientProtocolException(httpException);
    // }
    return NOERROR;
}

AutoPtr<IRequestDirector> AbstractHttpClient::CreateClientRequestDirector(
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
    /* [in] */ IHttpParams* params)
{
    AutoPtr<IRequestDirector> director = (IRequestDirector*)new DefaultRequestDirector(
            requestExec, conman, reustrat, kastrat,
            rouplan, httpProcessor, retryHandler, redirectHandler, targetAuthHandler,
            proxyAuthHandler, stateHandler, params);
    return director;
}

AutoPtr<IHttpParams> AbstractHttpClient::DetermineParams(
    /* [in] */ IHttpRequest* req)
{
    AutoPtr<IHttpParams> params, reqParams;
    GetParams((IHttpParams**)&params);
    IHttpMessage::Probe(req)->GetParams((IHttpParams**)&reqParams);
    AutoPtr<IHttpParams> stack = (IHttpParams*)new ClientParamsStack(NULL, params, reqParams, NULL);
    return stack;
}

ECode AbstractHttpClient::Execute(
    /* [in] */ IHttpUriRequest* request,
    /* [in] */ IResponseHandler* responseHandler,
    /* [out] */ IInterface** response)
{
    VALIDATE_NOT_NULL(response)
    return Execute(request, responseHandler, NULL, response);
}

ECode AbstractHttpClient::Execute(
    /* [in] */ IHttpUriRequest* request,
    /* [in] */ IResponseHandler* responseHandler,
    /* [in] */ IHttpContext* context,
    /* [out] */ IInterface** response)
{
    VALIDATE_NOT_NULL(response)
    AutoPtr<IHttpHost> target = DetermineTarget(request);
    return Execute(target, IHttpRequest::Probe(request), responseHandler, context, response);
}

ECode AbstractHttpClient::Execute(
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpRequest* request,
    /* [in] */ IResponseHandler* responseHandler,
    /* [out] */ IInterface** response)
{
    VALIDATE_NOT_NULL(response)
    return Execute(target, request, responseHandler, NULL, response);
}

ECode AbstractHttpClient::Execute(
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpRequest* request,
    /* [in] */ IResponseHandler* responseHandler,
    /* [in] */ IHttpContext* context,
    /* [out] */ IInterface** _response)
{
    VALIDATE_NOT_NULL(_response)
    *_response = NULL;

    if (responseHandler == NULL) {
        Logger::E("AbstractHttpClient", "Response handler must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IHttpResponse> response;
    Execute(target, request, context, (IHttpResponse**)&response);

    AutoPtr<IInterface> result;
    // try {
    ECode ec = responseHandler->HandleResponse(response, (IInterface**)&result);
    if (FAILED(ec)) {
        AutoPtr<IHttpEntity> entity;
        response->GetEntity((IHttpEntity**)&entity);
        if (entity != NULL) {
            // try {
            ECode ec1 = entity->ConsumeContent();
            if (FAILED(ec1)) {
                Logger::E("AbstractHttpClient", "Error consuming content after an exception. 0x%08x", ec1);
            }
            // } catch (Throwable t2) {
            //     // Log this exception. The original exception is more
            //     // important and will be thrown to the caller.
            //     this.log.warn("Error consuming content after an exception.", t2);
            // }
        }

        // if (t instanceof Error) {
        //     throw (Error) t;
        // }

        // if (t instanceof RuntimeException) {
        //     throw (RuntimeException) t;
        // }

        // if (t instanceof IOException) {
        //     throw (IOException) t;
        // }

        // throw new UndeclaredThrowableException(t);
        return ec;
    }
    // } catch (Throwable t) {
    //     HttpEntity entity = response.getEntity();
    //     if (entity != null) {
    //         try {
    //             entity.consumeContent();
    //         } catch (Throwable t2) {
    //             // Log this exception. The original exception is more
    //             // important and will be thrown to the caller.
    //             this.log.warn("Error consuming content after an exception.", t2);
    //         }
    //     }

    //     if (t instanceof Error) {
    //         throw (Error) t;
    //     }

    //     if (t instanceof RuntimeException) {
    //         throw (RuntimeException) t;
    //     }

    //     if (t instanceof IOException) {
    //         throw (IOException) t;
    //     }

    //     throw new UndeclaredThrowableException(t);
    // }

    // Handling the response was successful. Ensure that the content has
    // been fully consumed.
    AutoPtr<IHttpEntity> entity;
    response->GetEntity((IHttpEntity**)&entity);
    if (entity != NULL) {
        // Let this exception go to the caller.
        entity->ConsumeContent();
    }

    *_response = result;
    REFCOUNT_ADD(*_response)
    return NOERROR;
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org