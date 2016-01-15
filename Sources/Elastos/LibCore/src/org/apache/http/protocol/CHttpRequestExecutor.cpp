
#include "org/apache/http/protocol/CHttpRequestExecutor.h"
#include "org/apache/http/CHttpVersion.h"
#include "elastos/core/CBoolean.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::IHttpStatus;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHttpEntityEnclosingRequest;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::CHttpVersion;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Params::ICoreProtocolPNames;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL(CHttpRequestExecutor, Object, IHttpRequestExecutor)

CAR_OBJECT_IMPL(CHttpRequestExecutor)

ECode CHttpRequestExecutor::CanResponseHaveBody(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpResponse* response,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IRequestLine> rl;
    request->GetRequestLine((IRequestLine**)&rl);
    String method;
    rl->GetMethod(&method);
    if (String("HEAD").EqualsIgnoreCase(method)) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IStatusLine> sl;
    response->GetStatusLine((IStatusLine**)&sl);
    Int32 status;
    sl->GetStatusCode(&status);
    *result = status >= IHttpStatus::SC_OK
            && status != IHttpStatus::SC_NO_CONTENT
            && status != IHttpStatus::SC_NOT_MODIFIED
            && status != IHttpStatus::SC_RESET_CONTENT;
    return NOERROR;
}

ECode CHttpRequestExecutor::Execute(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpClientConnection* conn,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpResponse** response)
{
    VALIDATE_NOT_NULL(response)
    *response = NULL;

    if (request == NULL) {
        Logger::E("CHttpRequestExecutor", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (conn == NULL) {
        Logger::E("CHttpRequestExecutor", "Client connection may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL) {
        Logger::E("CHttpRequestExecutor", "HTTP context may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    AutoPtr<IHttpResponse> resp;
    ECode ec = DoSendRequest(request, conn, context, (IHttpResponse**)&resp);
    if (FAILED(ec)) {
        *response = NULL;
        IHttpConnection::Probe(conn)->Close();
        return ec;
    }
    if (response == NULL) {
        ec = DoReceiveResponse(request, conn, context, (IHttpResponse**)&resp);
        if (FAILED(ec)) {
            *response = NULL;
            IHttpConnection::Probe(conn)->Close();
            return ec;
        }
    }
    *response = resp;
    REFCOUNT_ADD(*response)
    return NOERROR;
}

ECode CHttpRequestExecutor::PreProcess(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpProcessor* processor,
    /* [in] */ IHttpContext* context)
{
    if (request == NULL) {
        Logger::E("CHttpRequestExecutor", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (processor == NULL) {
        Logger::E("CHttpRequestExecutor", "HTTP processor may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL) {
        Logger::E("CHttpRequestExecutor", "HTTP context may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return IHttpRequestInterceptor::Probe(processor)->Process(request, context);
}

ECode CHttpRequestExecutor::DoSendRequest(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpClientConnection* conn,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpResponse** _response)
{
    VALIDATE_NOT_NULL(_response)
    *_response = NULL;

    if (request == NULL) {
        Logger::E("CHttpRequestExecutor", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (conn == NULL) {
        Logger::E("CHttpRequestExecutor", "HTTP connection may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL) {
        Logger::E("CHttpRequestExecutor", "HTTP context may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IHttpResponse> response;
    AutoPtr<IBoolean> b;
    CBoolean::New(FALSE, (IBoolean**)&b);
    context->SetAttribute(IExecutionContext::HTTP_REQ_SENT, b);

    conn->SendRequestHeader(request);
    AutoPtr<IHttpEntityEnclosingRequest> enclosingRequest = IHttpEntityEnclosingRequest::Probe(request);
    if (enclosingRequest != NULL) {
        // Check for expect-continue handshake. We have to flush the
        // headers and wait for an 100-continue response to handle it.
        // If we get a different response, we must not send the entity.
        Boolean sendentity = TRUE;
        AutoPtr<IRequestLine> rl;
        request->GetRequestLine((IRequestLine**)&rl);
        AutoPtr<IProtocolVersion> ver;
        rl->GetProtocolVersion((IProtocolVersion**)&ver);
        Boolean result, lessEquals;
        if ((enclosingRequest->ExpectContinue(&result), result) &&
                (ver->LessEquals(IProtocolVersion::Probe(CHttpVersion::HTTP_1_0), &lessEquals), !lessEquals)) {
            conn->Flush();
            // As suggested by RFC 2616 section 8.2.3, we don't wait for a
            // 100-continue response forever. On timeout, send the entity.
            AutoPtr<IHttpParams> params;
            IHttpMessage::Probe(request)->GetParams((IHttpParams**)&params);
            Int32 tms;
            params->GetInt32Parameter(ICoreProtocolPNames::WAIT_FOR_CONTINUE, 2000, &tms);
            Boolean isAvailable;
            if (conn->IsResponseAvailable(tms, &isAvailable), isAvailable) {
                response = NULL;
                conn->ReceiveResponseHeader((IHttpResponse**)&response);
                Boolean haveBody;
                if (CanResponseHaveBody(request, response, &haveBody), haveBody) {
                    conn->ReceiveResponseEntity(response);
                }
                AutoPtr<IStatusLine> sl;
                response->GetStatusLine((IStatusLine**)&sl);
                Int32 status;
                sl->GetStatusCode(&status);
                if (status < 200) {
                    if (status != IHttpStatus::SC_CONTINUE) {
                        Logger::E("CHttpRequestExecutor", "Unexpected response: %p", sl.Get());
                        return E_PROTOCOL_EXCEPTION;
                    }
                    // discard 100-continue
                    response = NULL;
                }
                else {
                    sendentity = FALSE;
                }
            }
        }
        if (sendentity) {
            conn->SendRequestEntity(enclosingRequest);
        }
    }
    conn->Flush();
    AutoPtr<IBoolean> tb;
    CBoolean::New(TRUE, (IBoolean**)&tb);
    context->SetAttribute(IExecutionContext::HTTP_REQ_SENT, tb);
    *_response = response;
    REFCOUNT_ADD(*_response)
    return NOERROR;
}

ECode CHttpRequestExecutor::DoReceiveResponse(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpClientConnection* conn,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpResponse** _response)
{
    VALIDATE_NOT_NULL(_response)
    *_response = NULL;

    if (request == NULL) {
        Logger::E("CHttpRequestExecutor", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (conn == NULL) {
        Logger::E("CHttpRequestExecutor", "HTTP connection may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL) {
        Logger::E("CHttpRequestExecutor", "HTTP context may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IHttpResponse> response;
    Int32 statuscode = 0;

    while (response == NULL || statuscode < IHttpStatus::SC_OK) {
        response = NULL;
        conn->ReceiveResponseHeader((IHttpResponse**)&response);
        Boolean haveBody;
        if (CanResponseHaveBody(request, response, &haveBody), haveBody) {
            conn->ReceiveResponseEntity(response);
        }
        AutoPtr<IStatusLine> sl;
        response->GetStatusLine((IStatusLine**)&sl);
        sl->GetStatusCode(&statuscode);

    } // while intermediate response

    *_response = response;
    REFCOUNT_ADD(*_response)
    return NOERROR;
}

ECode CHttpRequestExecutor::PostProcess(
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpProcessor* processor,
    /* [in] */ IHttpContext* context)
{
    if (response == NULL) {
        Logger::E("CHttpRequestExecutor", "HTTP response may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (processor == NULL) {
        Logger::E("CHttpRequestExecutor", "HTTP processor may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL) {
        Logger::E("CHttpRequestExecutor", "HTTP context may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return IHttpResponseInterceptor::Probe(processor)->Process(response, context);
}

ECode CHttpRequestExecutor::constructor()
{
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
