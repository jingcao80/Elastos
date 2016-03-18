
#include "org/apache/http/message/BasicLineParser.h"
#include "org/apache/http/message/CBasicLineParser.h"
#include "org/apache/http/message/CBasicRequestLine.h"
#include "org/apache/http/message/CBasicStatusLine.h"
#include "org/apache/http/message/CBufferedHeader.h"
#include "org/apache/http/message/CParserCursor.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "org/apache/http/protocol/HTTP.h"
#include "org/apache/http/CHttpVersion.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::CHttpVersion;
using Org::Apache::Http::Utility::CCharArrayBuffer;
using Org::Apache::Http::Protocol::HTTP;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

static AutoPtr<IBasicLineParser> InitDefault()
{
    AutoPtr<CBasicLineParser> parser;
    CBasicLineParser::NewByFriend((CBasicLineParser**)&parser);
    return (IBasicLineParser*)parser.Get();
}
const AutoPtr<IBasicLineParser> BasicLineParser::DEFAULT = InitDefault();

CAR_INTERFACE_IMPL_2(BasicLineParser, Object, IBasicLineParser, ILineParser)

ECode BasicLineParser::ParseProtocolVersion(
    /* [in] */ const String& value,
    /* [in] */ ILineParser* parser,
    /* [out] */ IProtocolVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = NULL;
    if (value.IsNull()) {
        Logger::E("BasicLineParser", "Value to parse may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (parser == NULL) parser = ILineParser::Probe(DEFAULT);

    AutoPtr<CCharArrayBuffer> buffer;
    CCharArrayBuffer::NewByFriend(value.GetLength(), (CCharArrayBuffer**)&buffer);
    buffer->Append(value);
    AutoPtr<CParserCursor> cursor;
    CParserCursor::NewByFriend(0, value.GetLength(), (CParserCursor**)&cursor);
    return parser->ParseProtocolVersion(buffer, cursor, ver);
}

ECode BasicLineParser::ParseProtocolVersion(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ IParserCursor* cursor,
    /* [out] */ IProtocolVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = NULL;

    if (buffer == NULL) {
        Logger::E("BasicLineParser", "Char array buffer may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (cursor == NULL) {
        Logger::E("BasicLineParser", "Parser cursor may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String protoname;
    mProtocol->GetProtocol(&protoname);
    Int32 protolength = protoname.GetLength();

    Int32 indexFrom, indexTo;
    cursor->GetPos(&indexFrom);
    cursor->GetUpperBound(&indexTo);

    SkipWhitespace(buffer, cursor);

    Int32 i;
    cursor->GetPos(&i);

    // long enough for "HTTP/1.1"?
    if (i + protolength + 4 > indexTo) {
        String s;
        buffer->Substring(indexFrom, indexTo, &s);
        Logger::E("BasicLineParser", "Not a valid protocol version: %s", s.string());
        return E_PARSE_EXCEPTION;
    }

    // check the protocol name and slash
    Boolean ok = TRUE;
    for (Int32 j = 0; ok && (j < protolength); j++) {
        Char32 ch;
        buffer->CharAt(i + j, &ch);
        ok = (ch == protoname.GetChar(j));
    }
    if (ok) {
        Char32 ch;
        buffer->CharAt(i + protolength, &ch);
        ok = (ch == '/');
    }
    if (!ok) {
        String s;
        buffer->Substring(indexFrom, indexTo, &s);
        Logger::E("BasicLineParser", "Not a valid protocol version: %s", s.string());
        return E_PARSE_EXCEPTION;
    }

    i += protolength + 1;

    Int32 period;
    buffer->IndexOf('.', i, indexTo, &period);
    if (period == -1) {
        String s;
        buffer->Substring(indexFrom, indexTo, &s);
        Logger::E("BasicLineParser", "Invalid protocol version number: %s", s.string());
        return E_PARSE_EXCEPTION;
    }
    Int32 major;
    // try {
    String s;
    buffer->SubstringTrimmed(i, period, &s);
    major = StringUtils::ParseInt32(s);
    // } catch (NumberFormatException e) {
    //     throw new ParseException
    //         ("Invalid protocol major version number: " +
    //          buffer.substring(indexFrom, indexTo));
    // }
    i = period + 1;

    Int32 blank;
    buffer->IndexOf(' ', i, indexTo, &blank);
    if (blank == -1) {
        blank = indexTo;
    }
    Int32 minor;
    // try {
    buffer->SubstringTrimmed(i, blank, &s);
    minor = StringUtils::ParseInt32(s);
    // } catch (NumberFormatException e) {
    //     throw new ParseException(
    //         "Invalid protocol minor version number: " +
    //         buffer.substring(indexFrom, indexTo));
    // }

    cursor->UpdatePos(blank);

    return CreateProtocolVersion(major, minor, ver);
}

ECode BasicLineParser::CreateProtocolVersion(
    /* [in] */ Int32 major,
    /* [in] */ Int32 minor,
    /* [out] */ IProtocolVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = NULL;
    return mProtocol->ForVersion(major, minor, ver);
}

ECode BasicLineParser::HasProtocolVersion(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ IParserCursor* cursor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (buffer == NULL) {
        Logger::E("BasicLineParser", "Char array buffer may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (cursor == NULL) {
        Logger::E("BasicLineParser", "Parser cursor may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 index;
    cursor->GetPos(&index);

    String protoname;
    mProtocol->GetProtocol(&protoname);
    Int32 protolength = protoname.GetLength();

    Int32 len;
    buffer->GetLength(&len);
    if (len < protolength + 4) {
        *result = FALSE;
        return NOERROR;
    }
    if (index < 0) {
        // end of line, no tolerance for trailing whitespace
        // this works only for single-digit major and minor version
        buffer->GetLength(&len);
        index = len - 4 - protolength;
    }
    else if (index == 0) {
        // beginning of line, tolerate leading whitespace
        Char32 ch;
        buffer->GetLength(&len);
        while ((index < len) && (buffer->CharAt(index, &ch), HTTP::IsWhitespace(ch))) {
             index++;
         }
    } // else within line, don't tolerate whitespace

    buffer->GetLength(&len);
    if (index + protolength + 4 > len) {
        *result = FALSE;
        return NOERROR;
    }

    // just check protocol name and slash, no need to analyse the version
    Boolean ok = TRUE;
    for (Int32 j = 0; ok && (j < protolength); j++) {
        Char32 ch;
        buffer->CharAt(index + j, &ch);
        ok = (ch == protoname.GetChar(j));
    }
    if (ok) {
        Char32 ch;
        buffer->CharAt(index + protolength, &ch);
        ok = (ch == '/');
    }

    *result = ok;
    return NOERROR;
}

ECode BasicLineParser::ParseRequestLine(
    /* [in] */ const String& value,
    /* [in] */ ILineParser* parser,
    /* [out] */ IRequestLine** requestLine)
{
    VALIDATE_NOT_NULL(requestLine)
    *requestLine = NULL;

    if (value.IsNull()) {
        Logger::E("BasicLineParser", "Value to parse may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (parser == NULL) parser = ILineParser::Probe(DEFAULT);

    AutoPtr<CCharArrayBuffer> buffer;
    CCharArrayBuffer::NewByFriend(value.GetLength(), (CCharArrayBuffer**)&buffer);
    buffer->Append(value);
    AutoPtr<CParserCursor> cursor;
    CParserCursor::NewByFriend(0, value.GetLength(), (CParserCursor**)&cursor);
    return parser->ParseRequestLine(buffer, cursor, requestLine);
}

ECode BasicLineParser::ParseRequestLine(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ IParserCursor* cursor,
    /* [out] */ IRequestLine** requestLine)
{
    VALIDATE_NOT_NULL(requestLine)
    *requestLine = NULL;

    if (buffer == NULL) {
        Logger::E("BasicLineParser", "Char array buffer may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (cursor == NULL) {
        Logger::E("BasicLineParser", "Parser cursor may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 indexFrom, indexTo;
    cursor->GetPos(&indexFrom);
    cursor->GetUpperBound(&indexTo);

    // try {
    SkipWhitespace(buffer, cursor);
    Int32 i;
    cursor->GetPos(&i);

    Int32 blank;
    buffer->IndexOf(' ', i, indexTo, &blank);
    if (blank < 0) {
        String s;
        buffer->Substring(indexFrom, indexTo, &s);
        Logger::E("BasicLineParser", "Invalid request line: %s", s.string());
        return E_PARSE_EXCEPTION;
    }
    String method;
    buffer->SubstringTrimmed(i, blank, &method);
    cursor->UpdatePos(blank);

    SkipWhitespace(buffer, cursor);
    cursor->GetPos(&i);

    buffer->IndexOf(' ', i, indexTo, &blank);
    if (blank < 0) {
        String s;
        buffer->Substring(indexFrom, indexTo, &s);
        Logger::E("BasicLineParser", "Invalid request line: %s", s.string());
        return E_PARSE_EXCEPTION;
    }
    String uri;
    buffer->SubstringTrimmed(i, blank, &uri);
    cursor->UpdatePos(blank);

    AutoPtr<IProtocolVersion> ver;
    ParseProtocolVersion(buffer, cursor, (IProtocolVersion**)&ver);

    SkipWhitespace(buffer, cursor);
    Boolean atEnd;
    if (cursor->AtEnd(&atEnd), !atEnd) {
        String s;
        buffer->Substring(indexFrom, indexTo, &s);
        Logger::E("BasicLineParser", "Invalid request line: %s", s.string());
        return E_PARSE_EXCEPTION;
    }

    return CreateRequestLine(method, uri, ver, requestLine);
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ParseException("Invalid request line: " +
    //                              buffer.substring(indexFrom, indexTo));
    // }
}

ECode BasicLineParser::CreateRequestLine(
    /* [in] */ const String& method,
    /* [in] */ const String& uri,
    /* [in] */ IProtocolVersion* ver,
    /* [out] */ IRequestLine** requestLine)
{
    VALIDATE_NOT_NULL(requestLine)
    return CBasicRequestLine::New(method, uri, ver, requestLine);
}

ECode BasicLineParser::ParseStatusLine(
    /* [in] */ const String& value,
    /* [in] */ ILineParser* parser,
    /* [out] */ IStatusLine** statusLine)
{
    VALIDATE_NOT_NULL(statusLine)
    *statusLine = NULL;

    if (value.IsNull()) {
        Logger::E("BasicLineParser", "Value to parse may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (parser == NULL) parser = ILineParser::Probe(DEFAULT);

    AutoPtr<CCharArrayBuffer> buffer;
    CCharArrayBuffer::NewByFriend(value.GetLength(), (CCharArrayBuffer**)&buffer);
    buffer->Append(value);
    AutoPtr<CParserCursor> cursor;
    CParserCursor::NewByFriend(0, value.GetLength(), (CParserCursor**)&cursor);
    return parser->ParseStatusLine(buffer, cursor, statusLine);
}

ECode BasicLineParser::ParseStatusLine(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ IParserCursor* cursor,
    /* [out] */ IStatusLine** statusLine)
{
    VALIDATE_NOT_NULL(statusLine)
    *statusLine = NULL;

    if (buffer == NULL) {
        Logger::E("BasicLineParser", "Char array buffer may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (cursor == NULL) {
        Logger::E("BasicLineParser", "Parser cursor may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 indexFrom, indexTo;
    cursor->GetPos(&indexFrom);
    cursor->GetUpperBound(&indexTo);

    // try {
    // handle the HTTP-Version
    AutoPtr<IProtocolVersion> ver;
    ParseProtocolVersion(buffer, cursor, (IProtocolVersion**)&ver);

    // handle the Status-Code
    SkipWhitespace(buffer, cursor);
    Int32 i;
    cursor->GetPos(&i);

    Int32 blank;
    buffer->IndexOf(' ', i, indexTo, &blank);
    if (blank < 0) {
        blank = indexTo;
    }
    Int32 statusCode = 0;
    // try {
    String s;
    buffer->SubstringTrimmed(i, blank, &s);
    statusCode = StringUtils::ParseInt32(s);
    // } catch (NumberFormatException e) {
    //     throw new ParseException(
    //         "Unable to parse status code from status line: "
    //         + buffer.substring(indexFrom, indexTo));
    // }
    //handle the Reason-Phrase
    i = blank;
    String reasonPhrase = String(NULL);
    if (i < indexTo) {
        buffer->SubstringTrimmed(i, indexTo, &reasonPhrase);
    }
    else {
        reasonPhrase = String("");
    }
    return CreateStatusLine(ver, statusCode, reasonPhrase, statusLine);

    // } catch (IndexOutOfBoundsException e) {
    //     throw new ParseException("Invalid status line: " +
    //                              buffer.substring(indexFrom, indexTo));
    // }
}

ECode BasicLineParser::CreateStatusLine(
    /* [in] */ IProtocolVersion* ver,
    /* [in] */ Int32 status,
    /* [in] */ const String& reason,
    /* [out] */ IStatusLine** statusLine)
{
    VALIDATE_NOT_NULL(statusLine)
    return CBasicStatusLine::New(ver, status, reason, statusLine);
}

ECode BasicLineParser::ParseHeader(
    /* [in] */ const String& value,
    /* [in] */ ILineParser* parser,
    /* [out] */ IHeader** header)
{
    VALIDATE_NOT_NULL(header)
    *header = NULL;

    if (value.IsNull()) {
        Logger::E("BasicLineParser", "Value to parse may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (parser == NULL) parser = ILineParser::Probe(DEFAULT);

    AutoPtr<CCharArrayBuffer> buffer;
    CCharArrayBuffer::NewByFriend(value.GetLength(), (CCharArrayBuffer**)&buffer);
    buffer->Append(value);
    return parser->ParseHeader(buffer, header);
}

ECode BasicLineParser::ParseHeader(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [out] */ IHeader** header)
{
    VALIDATE_NOT_NULL(header)
    // the actual parser code is in the constructor of BufferedHeader
    return CBufferedHeader::New(buffer, header);
}

ECode BasicLineParser::SkipWhitespace(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ IParserCursor* cursor)
{
    Int32 pos, indexTo;
    cursor->GetPos(&pos);
    cursor->GetUpperBound(&indexTo);
    Char32 ch;
    while ((pos < indexTo) && (buffer->CharAt(pos, &ch), HTTP::IsWhitespace(ch))) {
        pos++;
    }
    return cursor->UpdatePos(pos);
}

ECode BasicLineParser::constructor(
    /* [in] */ IProtocolVersion* proto)
{
    if (proto == NULL) {
        proto = IProtocolVersion::Probe(CHttpVersion::HTTP_1_1);
    }
    mProtocol = proto;
    return NOERROR;
}

ECode BasicLineParser::constructor()
{
    return constructor(NULL);
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
