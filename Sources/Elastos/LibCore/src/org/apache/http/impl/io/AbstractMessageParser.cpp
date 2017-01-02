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

#include "org/apache/http/impl/io/AbstractMessageParser.h"
#include "org/apache/http/message/BasicLineParser.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IO::EIID_IHttpMessageParser;
using Org::Apache::Http::Message::BasicLineParser;
using Org::Apache::Http::Params::ICoreConnectionPNames;
using Org::Apache::Http::Utility::ICharArrayBuffer;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

AbstractMessageParser::AbstractMessageParser(
    /* [in] */ ISessionInputBuffer* buffer,
    /* [in] */ ILineParser* parser,
    /* [in] */ IHttpParams* params)
    : mMaxHeaderCount(0)
    , mMaxLineLen(0)
{
    if (buffer == NULL) {
        Logger::E("AbstractMessageParser", "Session input buffer may not be null");
        assert(0);
        // throw new IllegalArgumentException("Session input buffer may not be null");
    }
    if (params == NULL) {
        Logger::E("AbstractMessageParser", "HTTP parameters may not be null");
        assert(0);
        // throw new IllegalArgumentException("HTTP parameters may not be null");
    }
    mSessionBuffer = buffer;
    params->GetInt32Parameter(ICoreConnectionPNames::MAX_HEADER_COUNT, -1, &mMaxHeaderCount);
    params->GetInt32Parameter(
            ICoreConnectionPNames::MAX_LINE_LENGTH, -1, &mMaxLineLen);
    mLineParser = (parser != NULL) ? parser : ILineParser::Probe(BasicLineParser::DEFAULT);
}

CAR_INTERFACE_IMPL(AbstractMessageParser, Object, IHttpMessageParser)

ECode AbstractMessageParser::ParseHeaders(
    /* [in] */ ISessionInputBuffer* inbuffer,
    /* [in] */ Int32 maxHeaderCount,
    /* [in] */ Int32 maxLineLen,
    /* [in] */ ILineParser* parser,
    /* [out] */ ArrayOf<IHeader*>** _headers)
{
    VALIDATE_NOT_NULL(_headers)
    *_headers = NULL;

    if (inbuffer == NULL) {
        Logger::E("AbstractMessageParser", "Session input buffer may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (parser == NULL) {
        parser = ILineParser::Probe(BasicLineParser::DEFAULT);
    }

    AutoPtr<IArrayList> headerLines;
    CArrayList::New((IArrayList**)&headerLines);

    AutoPtr<ICharArrayBuffer> current, previous;
    for (;;) {
        if (current == NULL) {
            CCharArrayBuffer::New(64, (ICharArrayBuffer**)&current);
        }
        else {
            current->Clear();
        }
        Int32 l;
        inbuffer->ReadLine(current, &l);
        Int32 len;
        if (l == -1 || (current->GetLength(&len), len < 1)) {
            break;
        }
        // Parse the header name and value
        // Check for folded headers first
        // Detect LWS-char see HTTP/1.0 or HTTP/1.1 Section 2.2
        // discussion on folded headers
        Char32 c1, c2;
        if (((current->CharAt(0, &c1), c1 == ' ') || (current->CharAt(0, &c2), c2 == '\t')) && previous != NULL) {
            // we have continuation folded header
            // so append value
            Int32 i = 0;
            Int32 length;
            while (current->GetLength(&length), i < length) {
                Char32 ch;
                current->CharAt(i, &ch);
                if (ch != ' ' && ch != '\t') {
                    break;
                }
                i++;
            }
            if (maxLineLen > 0) {
                Int32 len1, len2;
                previous->GetLength(&len1);
                current->GetLength(&len2);
                if (len1 + 1 + len2 - i > maxLineLen) {
                    Logger::E("AbstractMessageParser", "Maximum line length limit exceeded");
                    return E_IO_EXCEPTION;
                }
            }
            previous->Append(' ');
            current->GetLength(&length);
            previous->Append(current, i, length - i);
        }
        else {
            headerLines->Add(current);
            previous = current;
            current = NULL;
        }
        Int32 size;
        if (maxHeaderCount > 0 && (headerLines->GetSize(&size), size >= maxHeaderCount)) {
            Logger::E("AbstractMessageParser", "Maximum header count exceeded");
            return E_IO_EXCEPTION;
        }
    }
    Int32 size;
    headerLines->GetSize(&size);
    AutoPtr< ArrayOf<IHeader*> > headers = ArrayOf<IHeader*>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> value;
        headerLines->Get(i, (IInterface**)&value);
        AutoPtr<ICharArrayBuffer> buffer = ICharArrayBuffer::Probe(value);
        // try {
        ECode ec = parser->ParseHeader(buffer, (IHeader**)&(*headers)[i]);
        if (ec == (ECode)E_PARSE_EXCEPTION) {
            return E_PROTOCOL_EXCEPTION;
        }
        // } catch (ParseException ex) {
        //     throw new ProtocolException(ex.getMessage());
        // }
    }
    *_headers = headers;
    REFCOUNT_ADD(*_headers)
    return NOERROR;
}

ECode AbstractMessageParser::Parse(
    /* [out] */ IHttpMessage** _message)
{
    VALIDATE_NOT_NULL(_message)
    *_message = NULL;

    AutoPtr<IHttpMessage> message;
    // try {
    ECode ec = ParseHead(mSessionBuffer, (IHttpMessage**)&message);
    if (ec == (ECode)E_PARSE_EXCEPTION) {
        return E_PROTOCOL_EXCEPTION;
    }
    // } catch (ParseException px) {
    //     throw new ProtocolException(px.getMessage(), px);
    // }
    AutoPtr< ArrayOf<IHeader*> > headers;
    ParseHeaders(mSessionBuffer, mMaxHeaderCount, mMaxLineLen, mLineParser, (ArrayOf<IHeader*>**)&headers);
    message->SetHeaders(headers);
    *_message = message;
    REFCOUNT_ADD(*_message)
    return NOERROR;
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org