
#include "org/apache/http/impl/io/HttpResponseParser.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "org/apache/http/message/CParserCursor.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::Message::IParserCursor;
using Org::Apache::Http::Message::CParserCursor;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

HttpResponseParser::HttpResponseParser(
    /* [in] */ ISessionInputBuffer* buffer,
    /* [in] */ ILineParser* parser,
    /* [in] */ IHttpResponseFactory* responseFactory,
    /* [in] */ IHttpParams* params)
    : AbstractMessageParser(buffer, parser, params)
{
    if (responseFactory == NULL) {
        Logger::E("HttpResponseParser", "Response factory may not be null");
        assert(0);
        // throw new IllegalArgumentException("Response factory may not be null");
    }
    mResponseFactory = responseFactory;
    CCharArrayBuffer::New(128, (ICharArrayBuffer**)&mLineBuf);
}

ECode HttpResponseParser::ParseHead(
    /* [in] */ ISessionInputBuffer* sessionBuffer,
    /* [out] */ IHttpMessage** message)
{
    VALIDATE_NOT_NULL(message)
    *message = NULL;
    mLineBuf->Clear();
    Int32 i;
    sessionBuffer->ReadLine(mLineBuf, &i);
    if (i == -1) {
        Logger::E("HttpResponseParser", "The target server failed to respond");
        return E_NO_HTTP_RESPONSE_EXCEPTION;
    }
    Int32 len;
    mLineBuf->GetLength(&len);
    AutoPtr<IParserCursor> cursor;
    CParserCursor::New(0, len, (IParserCursor**)&cursor);
    AutoPtr<IStatusLine> statusline;
    mLineParser->ParseStatusLine(mLineBuf, cursor, (IStatusLine**)&statusline);
    AutoPtr<IHttpResponse> response;
    mResponseFactory->NewHttpResponse(statusline, NULL, (IHttpResponse**)&response);
    *message = IHttpMessage::Probe(response);
    REFCOUNT_ADD(*message)
    return NOERROR;
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org