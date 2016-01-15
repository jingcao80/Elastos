
#include "org/apache/http/message/BasicHttpRequest.h"
#include "org/apache/http/message/CBasicRequestLine.h"
#include "org/apache/http/params/HttpProtocolParams.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Params::HttpProtocolParams;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_INTERFACE_IMPL(BasicHttpRequest, AbstractHttpMessage, IHttpRequest)

ECode BasicHttpRequest::GetProtocolVersion(
    /* [out] */ IProtocolVersion** protocolVersion)
{
    VALIDATE_NOT_NULL(protocolVersion)
    if (mRequestLine != NULL) {
        return mRequestLine->GetProtocolVersion(protocolVersion);
    }
    else {
        AutoPtr<IHttpParams> params;
        GetParams((IHttpParams**)&params);
        return HttpProtocolParams::GetVersion(params, protocolVersion);
    }
}

ECode BasicHttpRequest::GetRequestLine(
    /* [out] */ IRequestLine** requestLine)
{
    VALIDATE_NOT_NULL(requestLine)
    if (mRequestLine != NULL) {
        *requestLine = mRequestLine;
        REFCOUNT_ADD(*requestLine)
        return NOERROR;
    }
    else {
        AutoPtr<IHttpParams> params;
        GetParams((IHttpParams**)&params);
        AutoPtr<IProtocolVersion> ver;
        HttpProtocolParams::GetVersion(params, (IProtocolVersion**)&ver);
        return CBasicRequestLine::New(mMethod, mUri, ver, requestLine);
    }
}

ECode BasicHttpRequest::Init(
    /* [in] */ const String& method,
    /* [in] */ const String& uri)
{
    AbstractHttpMessage::Init();
    if (method.IsNull()) {
        Logger::E("BasicHttpRequest", "Method name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (uri.IsNull()) {
        Logger::E("BasicHttpRequest", "Request URI may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMethod = method;
    mUri = uri;
    mRequestLine = NULL;
    return NOERROR;
}

ECode BasicHttpRequest::Init(
    /* [in] */ const String& method,
    /* [in] */ const String& uri,
    /* [in] */ IProtocolVersion* ver)
{
    AutoPtr<IRequestLine> line;
    CBasicRequestLine::New(method, uri, ver, (IRequestLine**)&line);
    return Init(line);
}

ECode BasicHttpRequest::Init(
    /* [in] */ IRequestLine* requestline)
{
    AbstractHttpMessage::Init();
    if (requestline == NULL) {
        Logger::E("BasicHttpRequest", "Request line may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mRequestLine = requestline;
    requestline->GetMethod(&mMethod);
    requestline->GetUri(&mUri);
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
