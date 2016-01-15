
#include "org/apache/http/impl/io/HttpRequestParser.h"
#include "org/apache/http/message/CParserCursor.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::Message::IParserCursor;
using Org::Apache::Http::Message::CParserCursor;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

HttpRequestParser::HttpRequestParser(
    /* [in] */ ISessionInputBuffer* buffer,
    /* [in] */ ILineParser* parser,
    /* [in] */ IHttpRequestFactory* requestFactory,
    /* [in] */ IHttpParams* params)
    : AbstractMessageParser(buffer, parser, params)
{
    if (requestFactory == NULL) {
        Logger::E("HttpRequestParser", "Request factory may not be null");
        assert(0);
        // throw new IllegalArgumentException("Request factory may not be null");
    }
    mRequestFactory = requestFactory;
    CCharArrayBuffer::New(128, (ICharArrayBuffer**)&mLineBuf);
}

ECode HttpRequestParser::ParseHead(
    /* [in] */ ISessionInputBuffer* sessionBuffer,
    /* [out] */ IHttpMessage** message)
{
    VALIDATE_NOT_NULL(message)
    *message = NULL;
    mLineBuf->Clear();
    Int32 i;
    sessionBuffer->ReadLine(mLineBuf, &i);
    if (i == -1) {
        Logger::E("HttpRequestParser", "Client closed connection");
        return E_CONNECTION_CLOSED_EXCEPTION;
    }
    Int32 len;
    mLineBuf->GetLength(&len);
    AutoPtr<IParserCursor> cursor;
    CParserCursor::New(0, len, (IParserCursor**)&cursor);
    AutoPtr<IRequestLine> requestline;
    mLineParser->ParseRequestLine(mLineBuf, cursor, (IRequestLine**)&requestline);
    AutoPtr<IHttpRequest> request;
    mRequestFactory->NewHttpRequest(requestline, (IHttpRequest**)&request);
    *message = IHttpMessage::Probe(request);
    REFCOUNT_ADD(*message)
    return NOERROR;
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org