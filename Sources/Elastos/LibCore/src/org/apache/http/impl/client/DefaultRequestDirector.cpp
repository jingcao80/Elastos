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

#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/impl/client/DefaultRequestDirector.h"
#include "org/apache/http/impl/client/EntityEnclosingRequestWrapper.h"
#include "org/apache/http/CHttpHost.h"
#include "org/apache/http/auth/CAuthState.h"
#include "org/apache/http/auth/CAuthScope.h"
#include "org/apache/http/client/methods/CHttpGet.h"
#include "org/apache/http/client/params/HttpClientParams.h"
#include "org/apache/http/client/utils/URIUtils.h"
#include "org/apache/http/conn/CBasicManagedEntity.h"
#include "org/apache/http/conn/params/ConnManagerParams.h"
#include "org/apache/http/conn/routing/CBasicRouteDirector.h"
#include "org/apache/http/params/HttpProtocolParams.h"
#include "org/apache/http/params/HttpConnectionParams.h"
#include "org/apache/http/entity/CBufferedHttpEntity.h"
#include "org/apache/http/message/CBasicHttpRequest.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/CString.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/concurrent/CTimeUnitHelper.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::ILocale;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpEntityEnclosingRequest;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::CHttpHost;
using Org::Apache::Http::Auth::CAuthState;
using Org::Apache::Http::Auth::CAuthScope;
using Org::Apache::Http::Auth::IAuthScheme;
using Org::Apache::Http::Auth::IAuthScope;
using Org::Apache::Http::Auth::ICredentials;
using Org::Apache::Http::Client::EIID_IRequestDirector;
using Org::Apache::Http::Client::ICredentialsProvider;
using Org::Apache::Http::Client::Methods::IAbortableHttpRequest;
using Org::Apache::Http::Client::Methods::IHttpUriRequest;
using Org::Apache::Http::Client::Methods::IHttpGet;
using Org::Apache::Http::Client::Methods::CHttpGet;
using Org::Apache::Http::Client::Params::IClientPNames;
using Org::Apache::Http::Client::Params::HttpClientParams;
using Org::Apache::Http::Client::Protocol::IClientContext;
using Org::Apache::Http::Client::Utils::URIUtils;
using Org::Apache::Http::Conn::CBasicManagedEntity;
using Org::Apache::Http::Conn::IClientConnectionRequest;
using Org::Apache::Http::Conn::Params::ConnManagerParams;
using Org::Apache::Http::Conn::Routing::IHttpRoute;
using Org::Apache::Http::Conn::Routing::IHttpRouteDirector;
using Org::Apache::Http::Conn::Routing::IBasicRouteDirector;
using Org::Apache::Http::Conn::Routing::CBasicRouteDirector;
using Org::Apache::Http::Conn::Routing::IRouteInfo;
using Org::Apache::Http::Conn::Scheme::IScheme;
using Org::Apache::Http::Conn::Scheme::ISchemeRegistry;
using Org::Apache::Http::Entity::CBufferedHttpEntity;
using Org::Apache::Http::Message::CBasicHttpRequest;
using Org::Apache::Http::Params::HttpConnectionParams;
using Org::Apache::Http::Params::HttpProtocolParams;
using Org::Apache::Http::Protocol::IHTTP;
using Org::Apache::Http::Protocol::IExecutionContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

DefaultRequestDirector::DefaultRequestDirector(
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
    /* [in] */ IHttpParams* params)
    : mConnManager(conman)
    , mRoutePlanner(rouplan)
    , mReuseStrategy(reustrat)
    , mKeepAliveStrategy(kastrat)
    , mRequestExec(requestExec)
    , mHttpProcessor(httpProcessor)
    , mRetryHandler(retryHandler)
    , mRedirectHandler(redirectHandler)
    , mUserTokenHandler(userTokenHandler)
    , mParams(params)
    , mRedirectCount(0)
    , mMaxRedirects(0)
{
    if (requestExec == NULL) {
        Logger::E("DefaultRequestDirector", "Request executor may not be null.");
        assert(0); // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (conman == NULL) {
        Logger::E("DefaultRequestDirector", "Client connection manager may not be null.");
        assert(0); // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (reustrat == NULL) {
        Logger::E("DefaultRequestDirector", "Connection reuse strategy may not be null.");
        assert(0); // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (kastrat == NULL) {
        Logger::E("DefaultRequestDirector", "Connection keep alive strategy may not be null.");
        assert(0); // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (rouplan == NULL) {
        Logger::E("DefaultRequestDirector", "Route planner may not be null.");
        assert(0); // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (httpProcessor == NULL) {
        Logger::E("DefaultRequestDirector", "HTTP protocol processor may not be null.");
        assert(0); // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (retryHandler == NULL) {
        Logger::E("DefaultRequestDirector", "HTTP request retry handler may not be null.");
        assert(0); // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (redirectHandler == NULL) {
        Logger::E("DefaultRequestDirector", "Redirect handler may not be null.");
        assert(0); // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (targetAuthHandler == NULL) {
        Logger::E("DefaultRequestDirector", "Target authentication handler may not be null.");
        assert(0); // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (proxyAuthHandler == NULL) {
        Logger::E("DefaultRequestDirector", "Proxy authentication handler may not be null.");
        assert(0); // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (userTokenHandler == NULL) {
        Logger::E("DefaultRequestDirector", "User token handler may not be null.");
        assert(0); // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        Logger::E("DefaultRequestDirector", "HTTP parameters may not be null");
        assert(0); // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mParams->GetInt32Parameter(IClientPNames::MAX_REDIRECTS, 100, &mMaxRedirects);
    CAuthState::New((IAuthState**)&mTargetAuthState);
    CAuthState::New((IAuthState**)&mProxyAuthState);
}

CAR_INTERFACE_IMPL(DefaultRequestDirector, Object, IRequestDirector)

AutoPtr<RequestWrapper> DefaultRequestDirector::WrapRequest(
    /* [in] */ IHttpRequest* request)
{
    AutoPtr<RequestWrapper> wrapper;
    AutoPtr<IHttpEntityEnclosingRequest> enclosingRequest = IHttpEntityEnclosingRequest::Probe(request);
    if (enclosingRequest != NULL) {
        wrapper = (RequestWrapper*)new EntityEnclosingRequestWrapper(enclosingRequest);
    }
    else {
        wrapper = new RequestWrapper(request);
    }
    return wrapper;
}

ECode DefaultRequestDirector::RewriteRequestURI(
    /* [in] */ RequestWrapper* request,
    /* [in] */ IHttpRoute* route)
{
    // try {
    AutoPtr<IURI> uri;
    request->GetURI((IURI**)&uri);
    AutoPtr<IHttpHost> host;
    Boolean isTunnelled;
    AutoPtr<IRouteInfo> ri = IRouteInfo::Probe(route);
    if ((ri->GetProxyHost((IHttpHost**)&host), host != NULL)
            && (ri->IsTunnelled(&isTunnelled), !isTunnelled)) {
        // Make sure the request URI is absolute
        Boolean isAbsolute;
        if (uri->IsAbsolute(&isAbsolute), !isAbsolute) {
            AutoPtr<IHttpHost> target;
            ri->GetTargetHost((IHttpHost**)&target);
            AutoPtr<IURI> temp = uri;
            uri = NULL;
            ECode ec;
            FAIL_GOTO(ec = URIUtils::RewriteURI(temp, target, (IURI**)&uri), failed)
            request->SetURI(uri);
            return NOERROR;
        }
    }
    else {
        // Make sure the request URI is relative
        Boolean isAbsolute;
        if (uri->IsAbsolute(&isAbsolute), isAbsolute) {
            AutoPtr<IURI> temp = uri;
            uri = NULL;
            ECode ec;
            FAIL_GOTO(ec = URIUtils::RewriteURI(temp, NULL, (IURI**)&uri), failed)
            request->SetURI(uri);
            return NOERROR;
        }
    }
failed:
    AutoPtr<IRequestLine> line;
    request->GetRequestLine((IRequestLine**)&line);
    String u;
    line->GetUri(&u);
    Logger::E("DefaultRequestDirector", "Invalid URI: %s", u.string());
    return E_PROTOCOL_EXCEPTION;
    // } catch (URISyntaxException ex) {
    //     throw new ProtocolException("Invalid URI: " +
    //             request.getRequestLine().getUri(), ex);
    // }
}

ECode DefaultRequestDirector::Execute(
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpResponse** _response)
{
    VALIDATE_NOT_NULL(_response)
    *_response = NULL;

    AutoPtr<IHttpRequest> orig = request;
    AutoPtr<RequestWrapper> origWrapper = WrapRequest(orig);
    origWrapper->SetParams(mParams);
    AutoPtr<IHttpRoute> origRoute;
    DetermineRoute(target, origWrapper, context, (IHttpRoute**)&origRoute);

    AutoPtr<RoutedRequest> roureq = new RoutedRequest(origWrapper, origRoute);

    Int64 timeout;
    ConnManagerParams::GetTimeout(mParams, &timeout);

    Int32 execCount = 0;

    Boolean reuse = FALSE;
    AutoPtr<IHttpResponse> response;
    Boolean done = FALSE;
    // try {
    while (!done) {
        // In this loop, the RoutedRequest may be replaced by a
        // followup request and route. The request and route passed
        // in the method arguments will be replaced. The original
        // request is still available in 'orig'.

        AutoPtr<RequestWrapper> wrapper = roureq->GetRequest();
        AutoPtr<IHttpRoute> route = roureq->GetRoute();

        // See if we have a user token bound to the execution context
        AutoPtr<IInterface> attr;
        context->GetAttribute(IClientContext::USER_TOKEN, (IInterface**)&attr);
        AutoPtr<IObject> userToken = IObject::Probe(attr);

        // Allocate connection if needed
        if (mManagedConn == NULL) {
            AutoPtr<IClientConnectionRequest> connRequest;
            mConnManager->RequestConnection(route, userToken, (IClientConnectionRequest**)&connRequest);
            AutoPtr<IAbortableHttpRequest> abortableRequest = IAbortableHttpRequest::Probe(orig);
            if (abortableRequest != NULL) {
                abortableRequest->SetConnectionRequest(connRequest);
            }

            // try {
            AutoPtr<ITimeUnitHelper> helper;
            CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&helper);
            AutoPtr<ITimeUnit> milliseconds;
            helper->GetMILLISECONDS((ITimeUnit**)&milliseconds);
            FAIL_RETURN(connRequest->GetConnection(timeout, milliseconds, (IManagedClientConnection**)&mManagedConn))
            // } catch(InterruptedException interrupted) {
            //     InterruptedIOException iox = new InterruptedIOException();
            //     iox.initCause(interrupted);
            //     throw iox;
            //Org::Apache::Http::Conn::IManagedClientConnection }
            Boolean isEnabled;
            if (HttpConnectionParams::IsStaleCheckingEnabled(mParams, &isEnabled), isEnabled) {
                // validate connection
                Logger::D("DefaultRequestDirector", "Stale connection check");
                Boolean isStale;
                if (IHttpConnection::Probe(mManagedConn)->IsStale(&isStale), isStale) {
                    Logger::D("DefaultRequestDirector", "Stale connection detected");
                    // BEGIN android-changed
                    // try {
                    IHttpConnection::Probe(mManagedConn)->Close();
                    // } catch (IOException ignored) {
                    //     // SSLSocket's will throw IOException
                    //     // because they can't send a "close
                    //     // notify" protocol message to the
                    //     // server. Just supresss any
                    //     // exceptions related to closing the
                    //     // stale connection.
                    // }
                    // END android-changed
                }
            }
        }

        AutoPtr<IAbortableHttpRequest> abortableRequest = IAbortableHttpRequest::Probe(orig);
        if (abortableRequest != NULL) {
            abortableRequest->SetReleaseTrigger(IConnectionReleaseTrigger::Probe(mManagedConn));
        }

        // Reopen connection if needed
        Boolean isOpen;
        if (IHttpConnection::Probe(mManagedConn)->IsOpen(&isOpen), !isOpen) {
            mManagedConn->Open(route, context, mParams);
        }
        // BEGIN android-added
        else {
            // b/3241899 set the per request timeout parameter on reused connections
            Int32 timeout;
            HttpConnectionParams::GetSoTimeout(mParams, &timeout);
            IHttpConnection::Probe(mManagedConn)->SetSocketTimeout(timeout);
        }
        // END android-added

        // try {
        ECode ec = EstablishRoute(route, context);
        if (ec == (ECode)E_TUNNEL_REFUSED_EXCEPTION) {
            break;
        }
        // } catch (TunnelRefusedException ex) {
        //     if (this.log.isDebugEnabled()) {
        //         this.log.debug(ex.getMessage());
        //     }
        //     response = ex.getResponse();
        //     break;
        // }

        // Reset headers on the request wrapper
        wrapper->ResetHeaders();

        // Re-write request URI if needed
        RewriteRequestURI(wrapper, route);

        // Use virtual host if set
        AutoPtr<IHttpParams> params;
        wrapper->GetParams((IHttpParams**)&params);
        AutoPtr<IInterface> param;
        params->GetParameter(IClientPNames::VIRTUAL_HOST, (IInterface**)&param);
        target = IHttpHost::Probe(param);

        if (target == NULL) {
            IRouteInfo::Probe(route)->GetTargetHost((IHttpHost**)&target);
        }

        AutoPtr<IHttpHost> proxy;
        IRouteInfo::Probe(route)->GetProxyHost((IHttpHost**)&proxy);

        // Populate the execution context
        context->SetAttribute(IExecutionContext::HTTP_TARGET_HOST, (IInterface*)target);
        context->SetAttribute(IExecutionContext::HTTP_PROXY_HOST, (IInterface*)proxy.Get());
        context->SetAttribute(IExecutionContext::HTTP_CONNECTION, (IInterface*)mManagedConn.Get());
        context->SetAttribute(IClientContext::TARGET_AUTH_STATE, (IInterface*)mTargetAuthState.Get());
        context->SetAttribute(IClientContext::PROXY_AUTH_STATE, (IInterface*)mProxyAuthState.Get());

        // Run request protocol interceptors
        mRequestExec->PreProcess(wrapper, mHttpProcessor, context);

        context->SetAttribute(IExecutionContext::HTTP_REQUEST, wrapper->Probe(EIID_IInterface));

        Boolean retrying = TRUE;
        while (retrying) {
            // Increment total exec count (with redirects)
            execCount++;
            // Increment exec count for this particular request
            wrapper->IncrementExecCount();
            if (wrapper->GetExecCount() > 1 && !wrapper->IsRepeatable()) {
                Logger::E("DefaultRequestDirector", "Cannot retry request with a non-repeatable request entity");
                return E_NON_REPEATABLE_REQUEST_EXCEPTION;
            }

            // try {
            // if (this.log.isDebugEnabled()) {
            //     this.log.debug("Attempt " + execCount + " to execute request");
            // }
            response = NULL;
            ec = mRequestExec->Execute((IHttpRequest*)wrapper,
                    IHttpClientConnection::Probe(mManagedConn), context, (IHttpResponse**)&response);
            if (ec == (ECode)E_IO_EXCEPTION) {
                // this.log.debug("Closing the connection.");
                IHttpConnection::Probe(mManagedConn)->Close();
                Boolean result;
                if (mRetryHandler->RetryRequest(ec, execCount, context, &result), result) {
                    // if (this.log.isInfoEnabled()) {
                    //     this.log.info("I/O exception ("+ ex.getClass().getName() +
                    //             ") caught when processing request: "
                    //             + ex.getMessage());
                    // }
                    // if (this.log.isDebugEnabled()) {
                    //     this.log.debug(ex.getMessage(), ex);
                    // }
                    // this.log.info("Retrying request");
                }
                else {
                    return ec;
                }

                // If we have a direct route to the target host
                // just re-open connection and re-try the request
                Int32 count;
                if (IRouteInfo::Probe(route)->GetHopCount(&count), count == 1) {
                    Logger::D("DefaultRequestDirector", "Reopening the direct connection.");
                    mManagedConn->Open(route, context, params);
                }
                else {
                    // otherwise give up
                    return ec;
                }
            }
            retrying = FALSE;

            // } catch (IOException ex) {
            //     this.log.debug("Closing the connection.");
            //     managedConn.close();
            //     if (retryHandler.retryRequest(ex, execCount, context)) {
            //         if (this.log.isInfoEnabled()) {
            //             this.log.info("I/O exception ("+ ex.getClass().getName() +
            //                     ") caught when processing request: "
            //                     + ex.getMessage());
            //         }
            //         if (this.log.isDebugEnabled()) {
            //             this.log.debug(ex.getMessage(), ex);
            //         }
            //         this.log.info("Retrying request");
            //     } else {
            //         throw ex;
            //     }

            //     // If we have a direct route to the target host
            //     // just re-open connection and re-try the request
            //     if (route.getHopCount() == 1) {
            //         this.log.debug("Reopening the direct connection.");
            //         managedConn.open(route, context, params);
            //     } else {
            //         // otherwise give up
            //         throw ex;
            //     }

            // }

        }

        // Run response protocol interceptors
        IHttpMessage::Probe(response)->SetParams(params);
        mRequestExec->PostProcess(response, mHttpProcessor, context);


        // The connection is in or can be brought to a re-usable state.
        if(mReuseStrategy->KeepAlive(response, context, &reuse), reuse) {
            // Set the idle duration of this connection
            Int64 duration;
            mKeepAliveStrategy->GetKeepAliveDuration(response, context, &duration);
            AutoPtr<ITimeUnitHelper> helper;
            CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&helper);
            AutoPtr<ITimeUnit> milliseconds;
            helper->GetMILLISECONDS((ITimeUnit**)&milliseconds);
            mManagedConn->SetIdleDuration(duration, milliseconds);
        }

        AutoPtr<RoutedRequest> followup;
        HandleResponse(roureq, response, context, (RoutedRequest**)&followup);
        if (followup == NULL) {
            done = TRUE;
        }
        else {
            if (reuse) {
                Logger::D("DefaultRequestDirector", "Connection kept alive");
                // Make sure the response body is fully consumed, if present
                AutoPtr<IHttpEntity> entity;
                response->GetEntity((IHttpEntity**)&entity);
                if (entity != NULL) {
                    entity->ConsumeContent();
                }
                // entity consumed above is not an auto-release entity,
                // need to mark the connection re-usable explicitly
                mManagedConn->MarkReusable();
            }
            else {
                IHttpConnection::Probe(mManagedConn)->Close();
            }
            // check if we can use the same connection for the followup
            Boolean equals;
            AutoPtr<IObject> o = IObject::Probe(followup->GetRoute());
            if (o->Equals(roureq->GetRoute(), &equals), !equals) {
                ReleaseConnection();
            }
            roureq = followup;
        }

        AutoPtr<IInterface> ut;
        mUserTokenHandler->GetUserToken(context, (IInterface**)&ut);
        userToken = IObject::Probe(ut);
        context->SetAttribute(IClientContext::USER_TOKEN, (IInterface*)userToken.Get());
        if (mManagedConn != NULL) {
            mManagedConn->SetState(userToken);
        }
    } // while not done


    // check for entity, release connection if possible
    AutoPtr<IHttpEntity> entity;
    Boolean isStreaming;
    if ((response == NULL) || (response->GetEntity((IHttpEntity**)&entity), entity == NULL) ||
        (entity->IsStreaming(&isStreaming), !isStreaming)) {
        // connection not needed and (assumed to be) in re-usable state
        if (reuse) mManagedConn->MarkReusable();
        ReleaseConnection();
    }
    else {
        // install an auto-release entity
        AutoPtr<IHttpEntity> e;
        response->GetEntity((IHttpEntity**)&e);
        CBasicManagedEntity::New(e, mManagedConn, reuse, (IHttpEntity**)&entity);
        response->SetEntity(entity);
    }

    *_response = response;
    REFCOUNT_ADD(*_response)
    return NOERROR;

    // } catch (HttpException ex) {
    //     abortConnection();
    //     throw ex;
    // } catch (IOException ex) {
    //     abortConnection();
    //     throw ex;
    // } catch (RuntimeException ex) {
    //     abortConnection();
    //     throw ex;
    // }
}

ECode DefaultRequestDirector::ReleaseConnection()
{
    // Release the connection through the ManagedConnection instead of the
    // ConnectionManager directly.  This lets the connection control how
    // it is released.
    // try {
    IConnectionReleaseTrigger::Probe(mManagedConn)->ReleaseConnection();
    // } catch(IOException ignored) {
    //     this.log.debug("IOException releasing connection", ignored);
    // }
    mManagedConn = NULL;
    return NOERROR;
}

ECode DefaultRequestDirector::DetermineRoute(
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpRoute** route)
{
    VALIDATE_NOT_NULL(route)

    if (target == NULL) {
        AutoPtr<IHttpParams> params;
        IHttpMessage::Probe(request)->GetParams((IHttpParams**)&params);
        AutoPtr<IInterface> param;
        params->GetParameter(IClientPNames::DEFAULT_HOST, (IInterface**)&param);
        target = IHttpHost::Probe(param);
    }
    if (target == NULL) {
        // BEGIN android-changed
        //     If the URI was malformed, make it obvious where there's no host component
        String scheme = String(NULL);
        String host = String(NULL);
        String path = String(NULL);
        AutoPtr<IURI> uri;
        AutoPtr<IHttpUriRequest> uriRequest = IHttpUriRequest::Probe(request);
        if (uriRequest != NULL && (uriRequest->GetURI((IURI**)&uri), uri != NULL)) {
            uri->GetScheme(&scheme);
            uri->GetHost(&host);
            uri->GetPath(&path);
        }
        Logger::E("DefaultRequestDirector", "Target host must not be null, or set in parameters. scheme=%s, host=%s, path=%s"
                , scheme.string(), host.string(), path.string());
        return E_ILLEGAL_STATE_EXCEPTION;
        // END android-changed
    }

    return mRoutePlanner->DetermineRoute(target, request, context, route);
}

ECode DefaultRequestDirector::EstablishRoute(
    /* [in] */ IHttpRoute* route,
    /* [in] */ IHttpContext* context)
{
    //@@@ how to handle CONNECT requests for tunnelling?
    //@@@ refuse to send external CONNECT via director? special handling?

    //@@@ should the request parameters already be used below?
    //@@@ probably yes, but they're not linked yet
    //@@@ will linking above cause problems with linking in reqExec?
    //@@@ probably not, because the parent is replaced
    //@@@ just make sure we don't link parameters to themselves
    AutoPtr<IBasicRouteDirector> basicrowdy;
    CBasicRouteDirector::New((IBasicRouteDirector**)&basicrowdy);
    AutoPtr<IHttpRouteDirector> rowdy = IHttpRouteDirector::Probe(basicrowdy);
    Int32 step;
    do {
        AutoPtr<IHttpRoute> fact;
        mManagedConn->GetRoute((IHttpRoute**)&fact);
        rowdy->NextStep(IRouteInfo::Probe(route), IRouteInfo::Probe(fact), &step);

        switch (step) {

            case IHttpRouteDirector::CONNECT_TARGET:
            case IHttpRouteDirector::CONNECT_PROXY:
                mManagedConn->Open(route, context, mParams);
                break;

            case IHttpRouteDirector::TUNNEL_TARGET:
            {
                Boolean secure;
                CreateTunnelToTarget(route, context, &secure);
                Logger::D("DefaultRequestDirector", "Tunnel to target created.");
                mManagedConn->TunnelTarget(secure, mParams);
                break;
            }
            case IHttpRouteDirector::TUNNEL_PROXY:
            {
                // The most simple example for this case is a proxy chain
                // of two proxies, where P1 must be tunnelled to P2.
                // route: Source -> P1 -> P2 -> Target (3 hops)
                // fact:  Source -> P1 -> Target       (2 hops)
                AutoPtr<IRouteInfo> ri = IRouteInfo::Probe(fact);
                Int32 count;
                ri->GetHopCount(&count);
                Int32 hop = count - 1; // the hop to establish
                Boolean secure;
                CreateTunnelToProxy(route, hop, context, &secure);
                Logger::D("DefaultRequestDirector", "Tunnel to proxy created.");
                AutoPtr<IHttpHost> target;
                ri->GetHopTarget(hop, (IHttpHost**)&target);
                mManagedConn->TunnelProxy(target, secure, mParams);
                break;
            }
            case IHttpRouteDirector::LAYER_PROTOCOL:
                mManagedConn->LayerProtocol(context, mParams);
                break;
            case IHttpRouteDirector::UNREACHABLE:
                Logger::E("DefaultRequestDirector", "Unable to establish route.\nplanned = %p\ncurrent = %p"
                        , route, fact.Get());
                return E_ILLEGAL_STATE_EXCEPTION;

            case IHttpRouteDirector::COMPLETE:
                // do nothing
                break;

            default:
                Logger::E("DefaultRequestDirector", "Unknown step indicator %d from RouteDirector.", step);
                return E_ILLEGAL_STATE_EXCEPTION;
        } // switch

    } while (step > IHttpRouteDirector::COMPLETE);
    return NOERROR;
}

ECode DefaultRequestDirector::CreateTunnelToTarget(
    /* [in] */ IHttpRoute* route,
    /* [in] */ IHttpContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IRouteInfo> ri = IRouteInfo::Probe(route);
    AutoPtr<IHttpHost> proxy;
    ri->GetProxyHost((IHttpHost**)&proxy);
    AutoPtr<IHttpHost> target;
    ri->GetTargetHost((IHttpHost**)&target);
    AutoPtr<IHttpResponse> response;

    Boolean done = FALSE;
    while (!done) {

        done = TRUE;

        Boolean isOpen;
        if (IHttpConnection::Probe(mManagedConn)->IsOpen(&isOpen), !isOpen) {
            mManagedConn->Open(route, context, mParams);
        }

        AutoPtr<IHttpRequest> connect;
        AutoPtr<IHttpMessage> hm = IHttpMessage::Probe(connect);
        CreateConnectRequest(route, context, (IHttpRequest**)&connect);

        String agent;
        HttpProtocolParams::GetUserAgent(mParams, &agent);
        if (!agent.IsNull()) {
            hm->AddHeader(IHTTP::USER_AGENT, agent);
        }
        String hostString;
        target->ToHostString(&hostString);
        hm->AddHeader(IHTTP::TARGET_HOST, hostString);

        AutoPtr<IAuthScheme> authScheme;
        mProxyAuthState->GetAuthScheme((IAuthScheme**)&authScheme);
        AutoPtr<IAuthScope> authScope;
        mProxyAuthState->GetAuthScope((IAuthScope**)&authScope);
        AutoPtr<ICredentials> creds;
        mProxyAuthState->GetCredentials((ICredentials**)&creds);
        if (creds != NULL) {
            Boolean isConnectionBased;
            if (authScope != NULL || (authScheme->IsConnectionBased(&isConnectionBased), !isConnectionBased)) {
                // try {
                AutoPtr<IHeader> h;
                authScheme->Authenticate(creds, connect, (IHeader**)&h);
                hm->AddHeader(h);
                // } catch (AuthenticationException ex) {
                //     if (this.log.isErrorEnabled()) {
                //         this.log.error("Proxy authentication error: " + ex.getMessage());
                //     }
                // }
            }
        }

        response = NULL;
        mRequestExec->Execute(connect, IHttpClientConnection::Probe(mManagedConn), context, (IHttpResponse**)&response);

        AutoPtr<IStatusLine> statusLine;
        response->GetStatusLine((IStatusLine**)&statusLine);
        Int32 status;
        statusLine->GetStatusCode(&status);
        if (status < 200) {
            Logger::E("DefaultRequestDirector", "Unexpected response to CONNECT request: %p", statusLine.Get());
            return E_HTTP_EXCEPTION;
        }

        AutoPtr<IInterface> attr;
        context->GetAttribute(IClientContext::CREDS_PROVIDER, (IInterface**)&attr);
        AutoPtr<ICredentialsProvider> credsProvider = ICredentialsProvider::Probe(attr);

        Boolean isAuthenticating;
        if (credsProvider != NULL && (HttpClientParams::IsAuthenticating(mParams, &isAuthenticating), isAuthenticating)) {
            Boolean isRequested;
            if (mProxyAuthHandler->IsAuthenticationRequested(response, context, &isRequested), isRequested) {

                Logger::D("DefaultRequestDirector", "Proxy requested authentication");
                AutoPtr<IMap> challenges;
                mProxyAuthHandler->GetChallenges(response, context, (IMap**)&challenges);
                // try {
                ProcessChallenges(challenges, mProxyAuthState, mProxyAuthHandler, response, context);
                // } catch (AuthenticationException ex) {
                //     if (this.log.isWarnEnabled()) {
                //         this.log.warn("Authentication error: " +  ex.getMessage());
                //         break;
                //     }
                // }
                UpdateAuthState(mProxyAuthState, proxy, credsProvider);

                AutoPtr<ICredentials> c;
                if (mProxyAuthState->GetCredentials((ICredentials**)&c), c != NULL) {
                    done = FALSE;

                    // Retry request
                    Boolean isAlive;
                    if (mReuseStrategy->KeepAlive(response, context, &isAlive), isAlive) {
                        Logger::D("DefaultRequestDirector", "Connection kept alive");
                        // Consume response content
                        AutoPtr<IHttpEntity> entity;
                        response->GetEntity((IHttpEntity**)&entity);
                        if (entity != NULL) {
                            entity->ConsumeContent();
                        }
                    }
                    else {
                        IHttpConnection::Probe(mManagedConn)->Close();
                    }

                }

            }
            else {
                // Reset proxy auth scope
                mProxyAuthState->SetAuthScope(NULL);
            }
        }
    }

    AutoPtr<IStatusLine> statusLine;
    response->GetStatusLine((IStatusLine**)&statusLine);
    Int32 status;
    statusLine->GetStatusCode(&status);

    if (status > 299) {

        // Buffer response content
        AutoPtr<IHttpEntity> entity;
        response->GetEntity((IHttpEntity**)&entity);
        if (entity != NULL) {
            AutoPtr<IHttpEntity> bufferedEntity;
            CBufferedHttpEntity::New(entity, (IHttpEntity**)&bufferedEntity);
            response->SetEntity(bufferedEntity);
        }

        IHttpConnection::Probe(mManagedConn)->Close();
        Logger::E("DefaultRequestDirector", "CONNECT refused by proxy: %p, %p", statusLine.Get(), response.Get());
        return E_TUNNEL_REFUSED_EXCEPTION;
    }

    mManagedConn->MarkReusable();

    // How to decide on security of the tunnelled connection?
    // The socket factory knows only about the segment to the proxy.
    // Even if that is secure, the hop to the target may be insecure.
    // Leave it to derived classes, consider insecure by default here.
    *result = FALSE;
    return NOERROR;
}

ECode DefaultRequestDirector::CreateTunnelToProxy(
    /* [in] */ IHttpRoute* route,
    /* [in] */ Int32 hop,
    /* [in] */ IHttpContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    // Have a look at createTunnelToTarget and replicate the parts
    // you need in a custom derived class. If your proxies don't require
    // authentication, it is not too hard. But for the stock version of
    // HttpClient, we cannot make such simplifying assumptions and would
    // have to include proxy authentication code. The HttpComponents team
    // is currently not in a position to support rarely used code of this
    // complexity. Feel free to submit patches that refactor the code in
    // createTunnelToTarget to facilitate re-use for proxy tunnelling.
    Logger::E("DefaultRequestDirector", "Proxy chains are not supported.");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode DefaultRequestDirector::CreateConnectRequest(
    /* [in] */ IHttpRoute* route,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpRequest** request)
{
    VALIDATE_NOT_NULL(request)

    // see RFC 2817, section 5.2 and
    // INTERNET-DRAFT: Tunneling TCP based protocols through
    // Web proxy servers

    AutoPtr<IHttpHost> target;
    IRouteInfo::Probe(route)->GetTargetHost((IHttpHost**)&target);

    String host;
    target->GetHostName(&host);
    Int32 port;
    if (target->GetPort(&port), port < 0) {
        AutoPtr<ISchemeRegistry> registry;
        mConnManager->GetSchemeRegistry((ISchemeRegistry**)&registry);
        String schemeName;
        target->GetSchemeName(&schemeName);
        AutoPtr<IScheme> scheme;
        registry->GetScheme(schemeName, (IScheme**)&scheme);
        scheme->GetDefaultPort(&port);
    }

    StringBuilder buffer(host.GetLength() + 6);
    buffer.Append(host);
    buffer.AppendChar(':');
    buffer.Append(StringUtils::ToString(port));

    String authority = buffer.ToString();
    AutoPtr<IProtocolVersion> ver;
    HttpProtocolParams::GetVersion(mParams, (IProtocolVersion**)&ver);
    return CBasicHttpRequest::New(String("CONNECT"), authority, ver, request);
}

ECode DefaultRequestDirector::HandleResponse(
    /* [in] */ RoutedRequest* roureq,
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context,
    /* [out] */ RoutedRequest** request)
{
    VALIDATE_NOT_NULL(request)
    *request = NULL;

    AutoPtr<IHttpRoute> route = roureq->GetRoute();
    AutoPtr<IHttpHost> proxy;
    IRouteInfo::Probe(route)->GetProxyHost((IHttpHost**)&proxy);
    AutoPtr<RequestWrapper> requestWrapper = roureq->GetRequest();

    AutoPtr<IHttpParams> params;
    requestWrapper->GetParams((IHttpParams**)&params);
    Boolean isRedirecting, isRedirectRequested;
    if ((HttpClientParams::IsRedirecting(params, &isRedirecting), isRedirecting) &&
            (mRedirectHandler->IsRedirectRequested(response, context, &isRedirectRequested), isRedirectRequested)) {
        if (mRedirectCount >= mMaxRedirects) {
            Logger::E("DefaultRequestDirector", "Maximum redirects (%d) exceeded", mMaxRedirects);
            return E_REDIRECT_EXCEPTION;
        }
        mRedirectCount++;

        AutoPtr<IURI> uri;
        mRedirectHandler->GetLocationURI(response, context, (IURI**)&uri);

        String host, scheme;
        Int32 port;
        uri->GetHost(&host);
        uri->GetScheme(&scheme);
        uri->GetPort(&port);
        AutoPtr<IHttpHost> newTarget;
        CHttpHost::New(host, port, scheme, (IHttpHost**)&newTarget);

        AutoPtr<IHttpGet> redirect;
        CHttpGet::New(uri, (IHttpGet**)&redirect);

        AutoPtr<IHttpRequest> orig = requestWrapper->GetOriginal();
        AutoPtr< ArrayOf<IHeader*> > headers;
        IHttpMessage::Probe(orig)->GetAllHeaders((ArrayOf<IHeader*>**)&headers);
        IHttpMessage::Probe(redirect)->SetHeaders(headers);

        AutoPtr<RequestWrapper> wrapper = new RequestWrapper(IHttpRequest::Probe(redirect));
        wrapper->SetParams(params);

        AutoPtr<IHttpRoute> newRoute;
        DetermineRoute(newTarget, wrapper, context, (IHttpRoute**)&newRoute);
        AutoPtr<RoutedRequest> newRequest = new RoutedRequest(wrapper, newRoute);

        // if (this.log.isDebugEnabled()) {
        //     this.log.debug("Redirecting to '" + uri + "' via " + newRoute);
        // }

        *request = newRequest;
        REFCOUNT_ADD(*request)
        return NOERROR;
    }

    AutoPtr<IInterface> attr;
    context->GetAttribute(IClientContext::CREDS_PROVIDER, (IInterface**)&attr);
    AutoPtr<ICredentialsProvider> credsProvider = ICredentialsProvider::Probe(attr);
    Boolean isAuthenticating;
    if (credsProvider != NULL && (HttpClientParams::IsAuthenticating(params, &isAuthenticating), isAuthenticating)) {
        Boolean isAuthenticationRequested;
        if (mTargetAuthHandler->IsAuthenticationRequested(response, context, &isAuthenticationRequested), isAuthenticationRequested) {
            AutoPtr<IInterface> targetAttr;
            context->GetAttribute(IExecutionContext::HTTP_TARGET_HOST, (IInterface**)&targetAttr);
            AutoPtr<IHttpHost> target = IHttpHost::Probe(targetAttr);
            if (target == NULL) {
                IRouteInfo::Probe(route)->GetTargetHost((IHttpHost**)&target);
            }

            Logger::D("DefaultRequestDirector", "Target requested authentication");
            AutoPtr<IMap> challenges;
            mTargetAuthHandler->GetChallenges(response, context, (IMap**)&challenges);
            // try {
            ECode ec = ProcessChallenges(challenges, mTargetAuthState, mTargetAuthHandler,
                    response, context);
            if (ec == (ECode)E_AUTHENTICATION_EXCEPTION) {
                *request = NULL;
                return ec;
            }
            // } catch (AuthenticationException ex) {
            //     if (this.log.isWarnEnabled()) {
            //         this.log.warn("Authentication error: " +  ex.getMessage());
            //         return null;
            //     }
            // }
            UpdateAuthState(mTargetAuthState, target, credsProvider);

            AutoPtr<ICredentials> cred;
            if (mTargetAuthState->GetCredentials((ICredentials**)&cred), cred != NULL) {
                // Re-try the same request via the same route
                *request = roureq;
                REFCOUNT_ADD(*request)
                return NOERROR;
            }
            else {
                *request = NULL;
                return NOERROR;
            }
        }
        else {
            // Reset target auth scope
            mTargetAuthState->SetAuthScope(NULL);
        }

        if (mProxyAuthHandler->IsAuthenticationRequested(response, context, &isAuthenticationRequested), isAuthenticationRequested) {
            Logger::D("DefaultRequestDirector", "Proxy requested authentication");
            AutoPtr<IMap> challenges;
            mProxyAuthHandler->GetChallenges(response, context, (IMap**)&challenges);
            // try {
            ECode ec = ProcessChallenges(challenges, mProxyAuthState, mProxyAuthHandler,
                    response, context);
            if (ec == (ECode)E_AUTHENTICATION_EXCEPTION) {
                *request = NULL;
                return ec;
            }
            // } catch (AuthenticationException ex) {
            //     if (this.log.isWarnEnabled()) {
            //         this.log.warn("Authentication error: " +  ex.getMessage());
            //         return null;
            //     }
            // }
            UpdateAuthState(mProxyAuthState, proxy, credsProvider);

            AutoPtr<ICredentials> cred;
            if (mProxyAuthState->GetCredentials((ICredentials**)&cred), cred != NULL) {
                // Re-try the same request via the same route
                *request = roureq;
                REFCOUNT_ADD(*request)
                return NOERROR;
            }
            else {
                *request = NULL;
                return NOERROR;
            }
        }
        else {
            // Reset proxy auth scope
            mProxyAuthState->SetAuthScope(NULL);
        }
    }
    *request = NULL;
    return NOERROR;
}

void DefaultRequestDirector::AbortConnection()
{
    AutoPtr<IManagedClientConnection> mcc = mManagedConn;
    if (mcc != NULL) {
        // we got here as the result of an exception
        // no response will be returned, release the connection
        mManagedConn = NULL;
        AutoPtr<IConnectionReleaseTrigger> crt = IConnectionReleaseTrigger::Probe(mcc);
        // try {
        crt->AbortConnection();
        // } catch (IOException ex) {
        //     if (this.log.isDebugEnabled()) {
        //         this.log.debug(ex.getMessage(), ex);
        //     }
        // }
        // ensure the connection manager properly releases this connection
        // try {
        crt->ReleaseConnection();
        // } catch(IOException ignored) {
        //     this.log.debug("Error releasing connection", ignored);
        // }
    }
}

ECode DefaultRequestDirector::ProcessChallenges(
    /* [in] */ IMap* challenges,
    /* [in] */ IAuthState* authState,
    /* [in] */ IAuthenticationHandler* authHandler,
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context)
{
    AutoPtr<IAuthScheme> authScheme;
    authState->GetAuthScheme((IAuthScheme**)&authScheme);
    if (authScheme == NULL) {
        // Authentication not attempted before
        authHandler->SelectScheme(challenges, response, context, (IAuthScheme**)&authScheme);
        authState->SetAuthScheme(authScheme);
    }
    String id;
    authScheme->GetSchemeName(&id);

    AutoPtr<ICharSequence> cs;
    CString::New(id.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    challenges->Get(cs, (IInterface**)&value);
    AutoPtr<IHeader> challenge = IHeader::Probe(value);
    if (challenge == NULL) {
        Logger::E("DefaultRequestDirector", "%s authorization challenge expected, but not found", id.string());
        return E_AUTHENTICATION_EXCEPTION;
    }
    authScheme->ProcessChallenge(challenge);
    Logger::D("DefaultRequestDirector", "Authorization challenge processed");
    return NOERROR;
}

void DefaultRequestDirector::UpdateAuthState(
    /* [in] */ IAuthState* authState,
    /* [in] */ IHttpHost* host,
    /* [in] */ ICredentialsProvider* credsProvider)
{
    Boolean isValid;
    if (authState->IsValid(&isValid), !isValid) {
        return;
    }

    String hostname;
    host->GetHostName(&hostname);
    Int32 port;
    host->GetPort(&port);
    if (port < 0) {
        AutoPtr<IScheme> scheme;
        AutoPtr<ISchemeRegistry> registry;
        mConnManager->GetSchemeRegistry((ISchemeRegistry**)&registry);
        registry->GetScheme(host, (IScheme**)&scheme);
        scheme->GetDefaultPort(&port);
    }

    AutoPtr<IAuthScheme> authScheme;
    authState->GetAuthScheme((IAuthScheme**)&authScheme);
    AutoPtr<IAuthScope> authScope;
    String realm, schemeName;
    authScheme->GetRealm(&realm);
    authScheme->GetSchemeName(&schemeName);
    CAuthScope::New(hostname, port, realm, schemeName, (IAuthScope**)&authScope);

    // if (this.log.isDebugEnabled()) {
    //     this.log.debug("Authentication scope: " + authScope);
    // }
    AutoPtr<ICredentials> creds;
    authState->GetCredentials((ICredentials**)&creds);
    if (creds == NULL) {
        credsProvider->GetCredentials(authScope, (ICredentials**)&creds);
        // if (this.log.isDebugEnabled()) {
        //     if (creds != null) {
        //         this.log.debug("Found credentials");
        //     } else {
        //         this.log.debug("Credentials not found");
        //     }
        // }
    }
    else {
        Boolean isComplete;
        if (authScheme->IsComplete(&isComplete), isComplete) {
            Logger::D("DefaultRequestDirector", "Authentication failed");
            creds = NULL;
        }
    }
    authState->SetAuthScope(authScope);
    authState->SetCredentials(creds);
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org