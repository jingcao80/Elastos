
#include "org/apache/http/impl/client/RequestWrapper.h"
#include "org/apache/http/params/HttpProtocolParams.h"
#include "org/apache/http/message/CBasicRequestLine.h"
#include "elastos/net/CURI.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Net::CURI;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::Client::Methods::EIID_IHttpUriRequest;
using Org::Apache::Http::Message::CBasicRequestLine;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Params::HttpProtocolParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

RequestWrapper::RequestWrapper(
    /* [in] */ IHttpRequest* request)
    : AbstractHttpMessage()
    , mOriginal(request)
    , mExecCount(0)
{
    if (request == NULL) {
        Logger::E("RequestWrapper", "HTTP request may not be null");
        assert(0);
        // throw new IllegalArgumentException("HTTP request may not be null");
    }
    AutoPtr<IHttpParams> params;
    IHttpMessage::Probe(request)->GetParams((IHttpParams**)&params);
    SetParams(params);
    // Make a copy of the original URI
    AutoPtr<IHttpUriRequest> uriRequest = IHttpUriRequest::Probe(request);
    if (uriRequest != NULL) {
        uriRequest->GetURI((IURI**)&mUri);
        uriRequest->GetMethod(&mMethod);
        mVersion = NULL;
    }
    else {
        AutoPtr<IRequestLine> requestLine;
        request->GetRequestLine((IRequestLine**)&requestLine);
        // try {
        String uri;
        requestLine->GetUri(&uri);
        if (FAILED(CURI::New(uri, (IURI**)&mUri))) {
            Logger::E("RequestWrapper", "Invalid request URI: %s", uri.string());
            assert(0);
        }
        // } catch (URISyntaxException ex) {
        //     throw new ProtocolException("Invalid request URI: "
        //             + requestLine.getUri(), ex);
        // }
        requestLine->GetMethod(&mMethod);
        IHttpMessage::Probe(request)->GetProtocolVersion((IProtocolVersion**)&mVersion);
    }
}

CAR_INTERFACE_IMPL_2(RequestWrapper, AbstractHttpMessage, IHttpUriRequest, IHttpRequest)

void RequestWrapper::ResetHeaders()
{
    // Make a copy of original headers
    mHeadergroup->Clear();
    AutoPtr< ArrayOf<IHeader*> > headers;
    IHttpMessage::Probe(mOriginal)->GetAllHeaders((ArrayOf<IHeader*>**)&headers);
    SetHeaders(headers);
}

ECode RequestWrapper::GetMethod(
    /* [out] */ String* method)
{
    VALIDATE_NOT_NULL(method)
    *method = mMethod;
    return NOERROR;
}

ECode RequestWrapper::SetMethod(
    /* [in] */ const String& method)
{
    if (method.IsNull()) {
        Logger::E("RequestWrapper", "Method name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMethod = method;
    return NOERROR;
}

ECode RequestWrapper::GetProtocolVersion(
    /* [out] */ IProtocolVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    if (mVersion != NULL) {
        *ver = mVersion;
        REFCOUNT_ADD(*ver)
        return NOERROR;
    }
    else {
        AutoPtr<IHttpParams> params;
        GetParams((IHttpParams**)&params);
        return HttpProtocolParams::GetVersion(params, ver);
    }
}

void RequestWrapper::SetProtocolVersion(
    /* [in] */ IProtocolVersion* ver)
{
    mVersion = ver;
}

ECode RequestWrapper::GetURI(
    /* [out] */ IURI** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = mUri;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

void RequestWrapper::SetURI(
    /* [in] */ IURI* uri)
{
    mUri = uri;
}

ECode RequestWrapper::GetRequestLine(
    /* [out] */ IRequestLine** line)
{
    VALIDATE_NOT_NULL(line)
    String method;
    GetMethod(&method);
    AutoPtr<IProtocolVersion> ver;
    GetProtocolVersion((IProtocolVersion**)&ver);
    String uritext = String(NULL);
    if (mUri != NULL) {
        mUri->ToASCIIString(&uritext);
    }
    if (uritext.IsNull() || uritext.GetLength() == 0) {
        uritext = String("/");
    }
    return CBasicRequestLine::New(method, uritext, ver, line);
}

ECode RequestWrapper::Abort()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RequestWrapper::IsAborted(
    /* [out] */ Boolean* isAborted)
{
    VALIDATE_NOT_NULL(isAborted)
    *isAborted = FALSE;
    return NOERROR;
}

AutoPtr<IHttpRequest> RequestWrapper::GetOriginal()
{
    return mOriginal;
}

Boolean RequestWrapper::IsRepeatable()
{
    return TRUE;
}

Int32 RequestWrapper::GetExecCount()
{
    return mExecCount;
}

void RequestWrapper::IncrementExecCount()
{
    mExecCount++;
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org