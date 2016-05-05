
#include "org/apache/http/message/BasicLineFormatter.h"
#include "org/apache/http/message/CBasicLineFormatter.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/utility/logging/Logger.h"
#include "elastos/core/StringUtils.h"

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IFormattedHeader;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

static AutoPtr<IBasicLineFormatter> InitDefault()
{
    AutoPtr<IBasicLineFormatter> formatter;
    CBasicLineFormatter::NewByFriend((IBasicLineFormatter**)&formatter);
    return formatter;
}
const AutoPtr<IBasicLineFormatter> BasicLineFormatter::DEFAULT = InitDefault();

CAR_INTERFACE_IMPL_2(BasicLineFormatter, Object, IBasicLineFormatter, ILineFormatter)

ECode BasicLineFormatter::InitBuffer(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [out] */ ICharArrayBuffer** buf)
{
    VALIDATE_NOT_NULL(buf)
    AutoPtr<ICharArrayBuffer> out = buffer;
    if (out != NULL) {
        out->Clear();
    }
    else {
        CCharArrayBuffer::New(64, (ICharArrayBuffer**)&out);
    }
    *buf = out;
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode BasicLineFormatter::FormatProtocolVersion(
    /* [in] */ IProtocolVersion* version,
    /* [in] */ ILineFormatter* formatter,
    /* [out] */ String* formattedStr)
{
    VALIDATE_NOT_NULL(formattedStr)
    if (formatter == NULL)
        formatter = ILineFormatter::Probe(BasicLineFormatter::DEFAULT);

    AutoPtr<ICharArrayBuffer> buf;
    formatter->AppendProtocolVersion(NULL, version, (ICharArrayBuffer**)&buf);
    return IObject::Probe(buf)->ToString(formattedStr);
}

ECode BasicLineFormatter::AppendProtocolVersion(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ IProtocolVersion* version,
    /* [out] */ ICharArrayBuffer** buf)
{
    VALIDATE_NOT_NULL(buf)
    *buf = NULL;

    if (version == NULL) {
        Logger::E("BasicLineFormatter", "Protocol version may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // can't use initBuffer, that would clear the argument!
    AutoPtr<ICharArrayBuffer> result = buffer;
    Int32 len;
    EstimateProtocolVersionLen(version, &len);
    if (result == NULL) {
        CCharArrayBuffer::New(len, (ICharArrayBuffer**)&result);
    }
    else {
        result->EnsureCapacity(len);
    }

    String protocol;
    version->GetProtocol(&protocol);
    result->Append(protocol);
    result->Append('/');
    Int32 major;
    version->GetMajor(&major);
    result->Append(major);
    result->Append('.');
    Int32 minor;
    version->GetMinor(&minor);
    result->Append(minor);

    *buf = result;
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode BasicLineFormatter::EstimateProtocolVersionLen(
    /* [in] */ IProtocolVersion* version,
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len)
    String protocol;
    version->GetProtocol(&protocol);
    *len = protocol.GetLength() + 4; // room for "HTTP/1.1"
    return NOERROR;
}

ECode BasicLineFormatter::FormatRequestLine(
    /* [in] */ IRequestLine* reqline,
    /* [in] */ ILineFormatter* formatter,
    /* [out] */ String* formattedStr)
{
    VALIDATE_NOT_NULL(formattedStr)
    if (formatter == NULL)
        formatter = ILineFormatter::Probe(BasicLineFormatter::DEFAULT);

    AutoPtr<ICharArrayBuffer> buf;
    formatter->FormatRequestLine(NULL, reqline, (ICharArrayBuffer**)&buf);
    return IObject::Probe(buf)->ToString(formattedStr);
}

ECode BasicLineFormatter::FormatRequestLine(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ IRequestLine* reqline,
    /* [out] */ ICharArrayBuffer** buf)
{
    VALIDATE_NOT_NULL(buf)
    *buf = NULL;

    if (reqline == NULL) {
        Logger::E("BasicLineFormatter", "Request line may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ICharArrayBuffer> result;
    InitBuffer(buffer, (ICharArrayBuffer**)&result);
    DoFormatRequestLine(result, reqline);

    *buf = result;
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode BasicLineFormatter::DoFormatRequestLine(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ IRequestLine* reqline)
{
    String method, uri;
    reqline->GetMethod(&method);
    reqline->GetUri(&uri);

    // room for "GET /index.html HTTP/1.1"
    AutoPtr<IProtocolVersion> ver;
    reqline->GetProtocolVersion((IProtocolVersion**)&ver);
    Int32 verLen;
    EstimateProtocolVersionLen(ver, &verLen);
    Int32 len = method.GetLength() + 1 + uri.GetLength() + 1 + verLen;
    buffer->EnsureCapacity(len);

    buffer->Append(method);
    buffer->Append(' ');
    buffer->Append(uri);
    buffer->Append(' ');
    AutoPtr<ICharArrayBuffer> buf;
    AppendProtocolVersion(buffer, ver, (ICharArrayBuffer**)&buf);

    return NOERROR;
}

ECode BasicLineFormatter::FormatStatusLine(
    /* [in] */ IStatusLine* statline,
    /* [in] */ ILineFormatter* formatter,
    /* [out] */ String* formattedStr)
{
    VALIDATE_NOT_NULL(formattedStr)
    if (formatter == NULL)
        formatter = ILineFormatter::Probe(BasicLineFormatter::DEFAULT);

    AutoPtr<ICharArrayBuffer> buf;
    formatter->FormatStatusLine(NULL, statline, (ICharArrayBuffer**)&buf);
    return IObject::Probe(buf)->ToString(formattedStr);
}

ECode BasicLineFormatter::FormatStatusLine(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ IStatusLine* statline,
    /* [out] */ ICharArrayBuffer** buf)
{
    VALIDATE_NOT_NULL(buf)
    *buf = NULL;

    if (statline == NULL) {
        Logger::E("BasicLineFormatter", "Status line may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ICharArrayBuffer> result;
    InitBuffer(buffer, (ICharArrayBuffer**)&result);
    DoFormatStatusLine(result, statline);

    *buf = result;
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode BasicLineFormatter::DoFormatStatusLine(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ IStatusLine* statline)
{
    AutoPtr<IProtocolVersion> ver;
    statline->GetProtocolVersion((IProtocolVersion**)&ver);
    Int32 verLen;
    EstimateProtocolVersionLen(ver, &verLen);
    Int32 len = verLen + 1 + 3 + 1; // room for "HTTP/1.1 200 "
    String reason;
    statline->GetReasonPhrase(&reason);
    if (!reason.IsNull()) {
        len += reason.GetLength();
    }
    buffer->EnsureCapacity(len);

    AutoPtr<ICharArrayBuffer> buf;
    AppendProtocolVersion(buffer, ver, (ICharArrayBuffer**)&buf);
    buffer->Append(' ');
    Int32 code;
    statline->GetStatusCode(&code);
    buffer->Append(code);
    buffer->Append(' '); // keep whitespace even if reason phrase is empty
    if (!reason.IsNull()) {
        buffer->Append(reason);
    }
    return NOERROR;
}

ECode BasicLineFormatter::FormatHeader(
    /* [in] */ IHeader* header,
    /* [in] */ ILineFormatter* formatter,
    /* [out] */ String* formattedStr)
{
    VALIDATE_NOT_NULL(formattedStr)
    if (formatter == NULL)
        formatter = ILineFormatter::Probe(BasicLineFormatter::DEFAULT);

    AutoPtr<ICharArrayBuffer> buf;
    formatter->FormatHeader(NULL, header, (ICharArrayBuffer**)&buf);
    return IObject::Probe(buf)->ToString(formattedStr);
}

ECode BasicLineFormatter::FormatHeader(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ IHeader* header,
    /* [out] */ ICharArrayBuffer** buf)
{
    VALIDATE_NOT_NULL(buf)
    *buf = NULL;

    if (header == NULL) {
        Logger::E("BasicLineFormatter", "Header may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharArrayBuffer> result;

    AutoPtr<IFormattedHeader> fh = IFormattedHeader::Probe(header);
    if (fh != NULL) {
        // If the header is backed by a buffer, re-use the buffer
        fh->GetBuffer((ICharArrayBuffer**)&result);
    }
    else {
        InitBuffer(buffer, (ICharArrayBuffer**)&result);
        DoFormatHeader(result, header);
    }
    *buf = result;
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode BasicLineFormatter::DoFormatHeader(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ IHeader* header)
{
    String name, value;
    header->GetName(&name);
    header->GetValue(&value);

    Int32 len = name.GetLength() + 2;
    if (!value.IsNull()) {
        len += value.GetLength();
    }
    buffer->EnsureCapacity(len);

    buffer->Append(name);
    buffer->Append(String(": "));
    if (!value.IsNull()) {
        buffer->Append(value);
    }
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
