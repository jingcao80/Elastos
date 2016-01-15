
#include "org/apache/http/impl/auth/AuthSchemeBase.h"
#include "org/apache/http/protocol/HTTP.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Auth::IAUTH;
using Org::Apache::Http::Protocol::HTTP;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {

AuthSchemeBase::AuthSchemeBase()
    : mProxy(FALSE)
{}

CAR_INTERFACE_IMPL(AuthSchemeBase, Object, IConnectionReuseStrategy)

ECode AuthSchemeBase::ProcessChallenge(
    /* [in] */ IHeader* header)
{
    if (header == NULL) {
        Logger::E("AuthSchemeBase", "Header may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String authheader;
    header->GetName(&authheader);
    if (authheader.EqualsIgnoreCase(IAUTH::WWW_AUTH)) {
        mProxy = FALSE;
    }
    else if (authheader.EqualsIgnoreCase(IAUTH::PROXY_AUTH)) {
        mProxy = TRUE;
    }
    else {
        Logger::E("AuthSchemeBase", "Unexpected header name: %s", authheader.string());
        return E_MALFORMED_CHALLENGE_EXCEPTION;
    }

    AutoPtr<ICharArrayBuffer> buffer;
    Int32 pos;
    AutoPtr<IFormattedHeader> formattedHeader = IFormattedHeader::Probe(header);
    if (formattedHeader != NULL) {
        formattedHeader->GetBuffer((ICharArrayBuffer**)&buffer);
        formattedHeader->GetValuePos(&pos);
    }
    else {
        String s;
        header->GetValue(&s);
        if (s.IsNull()) {
            Logger::E("AuthSchemeBase", "Header value is null");
            return E_MALFORMED_CHALLENGE_EXCEPTION;
        }
        CCharArrayBuffer::New(s.GetLength(), (ICharArrayBuffer**)&buffer);
        buffer->Append(s);
        pos = 0;
    }
    Int32 len;
    buffer->GetLength(&len);
    Char32 c;
    buffer->CharAt(pos, &c);
    while (pos < len && HTTP::IsWhitespace(c)) {
        pos++;
    }
    Int32 beginIndex = pos;
    buffer->CharAt(pos, &c);
    while (pos < len && !HTTP::IsWhitespace(c)) {
        pos++;
    }
    Int32 endIndex = pos;
    String s;
    buffer->Substring(beginIndex, endIndex, &s);
    String name;
    GetSchemeName(&name);
    if (!s.EqualsIgnoreCase(name)) {
        Logger::E("AuthSchemeBase", "Invalid scheme identifier: %s", s.string());
        return E_MALFORMED_CHALLENGE_EXCEPTION;
    }

    return ParseChallenge(buffer, pos, len);
}

Boolean AuthSchemeBase::IsProxy()
{
    return mProxy;
}

} // namespace Auth
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org