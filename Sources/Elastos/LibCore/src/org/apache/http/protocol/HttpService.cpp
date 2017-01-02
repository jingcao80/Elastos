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

#include "org/apache/http/protocol/HttpService.h"
#include "org/apache/http/params/CDefaultedHttpParams.h"
#include "org/apache/http/CHttpVersion.h"
#include "org/apache/http/utility/EncodingUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::CHttpVersion;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IConnectionReuseStrategy;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IHttpStatus;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IHttpEntityEnclosingRequest;
using Org::Apache::Http::Params::IDefaultedHttpParams;
using Org::Apache::Http::Params::CDefaultedHttpParams;
using Org::Apache::Http::Utility::EncodingUtils;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL(HttpService, Object, IHttpService)

ECode HttpService::Init(
    /* [in] */ IHttpProcessor* processor,
    /* [in] */ IConnectionReuseStrategy* connStrategy,
    /* [in] */ IHttpResponseFactory* responseFactory)
{
    ECode ec;
    FAIL_RETURN(ec = SetHttpProcessor(processor))
    FAIL_RETURN(ec = SetConnReuseStrategy(connStrategy))
    FAIL_RETURN(ec = SetResponseFactory(responseFactory))
    return NOERROR;
}

ECode HttpService::SetHttpProcessor(
    /* [in] */ IHttpProcessor* processor)
{
    if (processor == NULL) {
        Logger::E("HttpService", "HTTP processor may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mProcessor = processor;
    return NOERROR;
}

ECode HttpService::SetConnReuseStrategy(
    /* [in] */ IConnectionReuseStrategy* connStrategy)
{
    if (connStrategy == NULL) {
        Logger::E("HttpService", "Connection reuse strategy may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mConnStrategy = connStrategy;
    return NOERROR;
}

ECode HttpService::SetResponseFactory(
    /* [in] */ IHttpResponseFactory* responseFactory)
{
    if (responseFactory == NULL) {
        Logger::E("HttpService", "Response factory may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mResponseFactory = responseFactory;
    return NOERROR;
}

ECode HttpService::SetHandlerResolver(
    /* [in] */ IHttpRequestHandlerResolver* handlerResolver)
{
    mHandlerResolver = handlerResolver;
    return NOERROR;
}

ECode HttpService::SetExpectationVerifier(
    /* [in] */ IHttpExpectationVerifier* expectationVerifier)
{
    mExpectationVerifier = expectationVerifier;
    return NOERROR;
}

ECode HttpService::GetParams(
    /* [out] */ IHttpParams** params)
{
    VALIDATE_NOT_NULL(params)
    *params = mParams;
    REFCOUNT_ADD(*params)
    return NOERROR;
}

ECode HttpService::SetParams(
    /* [in] */ IHttpParams* params)
{
    mParams = params;
    return NOERROR;
}

ECode HttpService::HandleRequest(
    /* [in] */ IHttpServerConnection* conn,
    /* [in] */ IHttpContext* context)
{
    context->SetAttribute(IExecutionContext::HTTP_CONNECTION, conn);

    AutoPtr<IHttpResponse> response;

    // try {
    AutoPtr<IHttpRequest> request;
    conn->ReceiveRequestHeader((IHttpRequest**)&request);
    AutoPtr<IHttpParams> requestP;
    IHttpMessage::Probe(request)->GetParams((IHttpParams**)&requestP);
    AutoPtr<IDefaultedHttpParams> defaultParams;
    CDefaultedHttpParams::New(requestP, mParams, (IDefaultedHttpParams**)&defaultParams);
    AutoPtr<IHttpParams> hp = IHttpParams::Probe(defaultParams);
    IHttpMessage::Probe(request)->SetParams(hp);

    AutoPtr<IRequestLine> rl;
    request->GetRequestLine((IRequestLine**)&rl);
    AutoPtr<IProtocolVersion> ver;
    rl->GetProtocolVersion((IProtocolVersion**)&ver);
    Boolean lessEquals;
    if (ver->LessEquals(IProtocolVersion::Probe(CHttpVersion::HTTP_1_1), &lessEquals), !lessEquals) {
        // Downgrade protocol version if greater than HTTP/1.1
        ver = IProtocolVersion::Probe(CHttpVersion::HTTP_1_1);
    }

    AutoPtr<IHttpEntityEnclosingRequest> enclosingRequest = IHttpEntityEnclosingRequest::Probe(request);
    if (enclosingRequest != NULL) {
        Boolean expectContinue;
        if (enclosingRequest->ExpectContinue(&expectContinue), expectContinue) {
            response = NULL;
            mResponseFactory->NewHttpResponse(ver, IHttpStatus::SC_CONTINUE, context, (IHttpResponse**)&response);
            AutoPtr<IDefaultedHttpParams> dhp;
            AutoPtr<IHttpParams> responseP;
            IHttpMessage::Probe(response)->GetParams((IHttpParams**)&responseP);
            CDefaultedHttpParams::New(responseP, mParams, (IDefaultedHttpParams**)&dhp);
            AutoPtr<IHttpParams> responsehp = IHttpParams::Probe(dhp);
            IHttpMessage::Probe(response)->SetParams(responsehp);

            if (mExpectationVerifier != NULL) {
                // try {
                ECode ec = mExpectationVerifier->Verify(request, response, context);
                if (ec == (ECode)E_HTTP_EXCEPTION) {
                    response = NULL;
                    mResponseFactory->NewHttpResponse(
                            IProtocolVersion::Probe(CHttpVersion::HTTP_1_0), IHttpStatus::SC_INTERNAL_SERVER_ERROR
                            ,context, (IHttpResponse**)&response);
                    AutoPtr<IDefaultedHttpParams> exceptionD;
                    AutoPtr<IHttpParams> exceptionP;
                    IHttpMessage::Probe(response)->GetParams((IHttpParams**)&exceptionP);
                    CDefaultedHttpParams::New(exceptionP, mParams, (IDefaultedHttpParams**)&exceptionD);
                    AutoPtr<IHttpParams> exceptionDp = IHttpParams::Probe(exceptionD);
                    IHttpMessage::Probe(response)->SetParams(exceptionDp);
                    HandleException(ec, response);
                }
                // } catch (HttpException ex) {
                //     response = this.responseFactory.newHttpResponse(HttpVersion.HTTP_1_0,
                //             HttpStatus.SC_INTERNAL_SERVER_ERROR, context);
                //     response.setParams(
                //             new DefaultedHttpParams(response.getParams(), this.params));
                //     handleException(ex, response);
                // }
            }
            AutoPtr<IStatusLine> sl;
            response->GetStatusLine((IStatusLine**)&sl);
            Int32 code;
            if (sl->GetStatusCode(&code), code < 200) {
                // Send 1xx response indicating the server expections
                // have been met
                conn->SendResponseHeader(response);
                conn->Flush();
                response = NULL;
                conn->ReceiveRequestEntity(enclosingRequest);
            }
        }
        else {
            conn->ReceiveRequestEntity(enclosingRequest);
        }
    }

    if (response == NULL) {
        mResponseFactory->NewHttpResponse(ver, IHttpStatus::SC_OK, context, (IHttpResponse**)&response);
        AutoPtr<IDefaultedHttpParams> dhp;
        AutoPtr<IHttpParams> responseP;
        IHttpMessage::Probe(response)->GetParams((IHttpParams**)&responseP);
        CDefaultedHttpParams::New(responseP, mParams, (IDefaultedHttpParams**)&dhp);
        AutoPtr<IHttpParams> responsehp = IHttpParams::Probe(dhp);
        IHttpMessage::Probe(response)->SetParams(responsehp);

        context->SetAttribute(IExecutionContext::HTTP_REQUEST, request);
        context->SetAttribute(IExecutionContext::HTTP_RESPONSE, response);

        IHttpRequestInterceptor::Probe(mProcessor)->Process(request, context);
        DoService(request, response, context);
    }

    // Make sure the request content is fully consumed
    if (enclosingRequest != NULL) {
        AutoPtr<IHttpEntity> entity;
        enclosingRequest->GetEntity((IHttpEntity**)&entity);
        if (entity != NULL) {
            entity->ConsumeContent();
        }
    }

    // } catch (HttpException ex) {
    //     response = this.responseFactory.newHttpResponse
    //         (HttpVersion.HTTP_1_0, HttpStatus.SC_INTERNAL_SERVER_ERROR,
    //          context);
    //     response.setParams(
    //             new DefaultedHttpParams(response.getParams(), this.params));
    //     handleException(ex, response);
    // }

    IHttpResponseInterceptor::Probe(mProcessor)->Process(response, context);
    conn->SendResponseHeader(response);
    conn->SendResponseEntity(response);
    conn->Flush();

    Boolean keepAlive;
    if (mConnStrategy->KeepAlive(response, context, &keepAlive), !keepAlive) {
        IHttpConnection::Probe(conn)->Close();
    }
    return NOERROR;
}

ECode HttpService::HandleException(
    /* [in] */ ECode ex,
    /* [in] */ IHttpResponse* response)
{
    if (ex == (ECode)E_METHOD_NOT_SUPPORTED_EXCEPTION) {
        response->SetStatusCode(IHttpStatus::SC_NOT_IMPLEMENTED);
    }
    else if (ex == (ECode)E_UNSUPPORTED_HTTP_VERSION_EXCEPTION) {
        response->SetStatusCode(IHttpStatus::SC_HTTP_VERSION_NOT_SUPPORTED);
    }
    else if (ex == (ECode)E_PROTOCOL_EXCEPTION) {
        response->SetStatusCode(IHttpStatus::SC_BAD_REQUEST);
    }
    else {
        response->SetStatusCode(IHttpStatus::SC_INTERNAL_SERVER_ERROR);
    }
    assert(0);
    // AutoPtr< ArrayOf<Byte> > msg;
    // byte[] msg = EncodingUtils::GetAsciiBytes(ex.getMessage());
    // AutoPtr<IByteArrayEntity> entity; = new ByteArrayEntity(msg);
    // entity.setContentType("text/plain; charset=US-ASCII");
    // response.setEntity(entity);
    return E_NOT_IMPLEMENTED;
}

ECode HttpService::DoService(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context)
{
    AutoPtr<IHttpRequestHandler> handler;
    if (mHandlerResolver != NULL) {
        AutoPtr<IRequestLine> rl;
        request->GetRequestLine((IRequestLine**)&rl);
        String requestURI;
        rl->GetUri(&requestURI);
        mHandlerResolver->Lookup(requestURI, (IHttpRequestHandler**)&handler);
    }
    if (handler != NULL) {
        handler->Handle(request, response, context);
    }
    else {
        response->SetStatusCode(IHttpStatus::SC_NOT_IMPLEMENTED);
    }
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
