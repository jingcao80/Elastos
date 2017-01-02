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

#include "org/apache/http/impl/conn/DefaultResponseParser.h"
#include "org/apache/http/message/CParserCursor.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/core/Math.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::Math;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::Conn::Params::IConnConnectionPNames;
using Org::Apache::Http::Message::IParserCursor;
using Org::Apache::Http::Message::CParserCursor;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

DefaultResponseParser::DefaultResponseParser(
    /* [in] */ ISessionInputBuffer* buffer,
    /* [in] */ ILineParser* parser,
    /* [in] */ IHttpResponseFactory* responseFactory,
    /* [in] */ IHttpParams* params)
    : AbstractMessageParser(buffer, parser, params)
    , mMaxGarbageLines(0)
{
    if (responseFactory == NULL) {
        Logger::E("DefaultResponseParser", "Response factory may not be null");
        assert(0);
        // throw new IllegalArgumentException
        //     ("Response factory may not be null");
    }
    mResponseFactory = responseFactory;
    CCharArrayBuffer::New(128, (ICharArrayBuffer**)&mLineBuf);
    params->GetInt32Parameter(IConnConnectionPNames::MAX_STATUS_LINE_GARBAGE, Elastos::Core::Math::INT64_MAX_VALUE, &mMaxGarbageLines);
}

ECode DefaultResponseParser::ParseHead(
    /* [in] */ ISessionInputBuffer* sessionBuffer,
    /* [out] */ IHttpMessage** msg)
{
    VALIDATE_NOT_NULL(msg)
    *msg = NULL;

    // clear the buffer
    mLineBuf->Clear();
    //read out the HTTP status string
    Int32 count = 0;
    AutoPtr<IParserCursor> cursor;
    do {
        Int32 i;
        sessionBuffer->ReadLine(mLineBuf, &i);
        if (i == -1 && count == 0) {
            // The server just dropped connection on us
            Logger::E("DefaultResponseParser", "The target server failed to respond");
            return E_NO_HTTP_RESPONSE_EXCEPTION;
        }
        Int32 len;
        mLineBuf->GetLength(&len);
        CParserCursor::New(0, len, (IParserCursor**)&cursor);
        Boolean has;
        if (mLineParser->HasProtocolVersion(mLineBuf, cursor, &has), has) {
            // Got one
            break;
        }
        else if (i == -1 || count >= mMaxGarbageLines) {
            // Giving up
            Logger::E("DefaultResponseParser", "The server failed to respond with a valid HTTP response");
            return E_PROTOCOL_EXCEPTION;
        }
        count++;
    } while(TRUE);
    //create the status line from the status string
    AutoPtr<IStatusLine> statusline;
    mLineParser->ParseStatusLine(mLineBuf, cursor, (IStatusLine**)&statusline);
    AutoPtr<IHttpResponse> hr;
    mResponseFactory->NewHttpResponse(statusline, NULL, (IHttpResponse**)&hr);
    *msg = IHttpMessage::Probe(hr);
    REFCOUNT_ADD(*msg)
    return NOERROR;
}

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org