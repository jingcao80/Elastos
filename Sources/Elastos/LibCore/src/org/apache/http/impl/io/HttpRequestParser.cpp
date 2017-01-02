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