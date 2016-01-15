
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/impl/auth/BasicScheme.h"
#include "org/apache/http/utility/EncodingUtils.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "org/apache/http/message/CBufferedHeader.h"
#include "org/apache/http/auth/params/AuthParams.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringBuilder;
using Elastos::Security::IPrincipal;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Auth::IAUTH;
using Org::Apache::Http::Auth::Params::AuthParams;
using Org::Apache::Http::Message::CBufferedHeader;
using Org::Apache::Http::Utility::ICharArrayBuffer;
using Org::Apache::Http::Utility::CCharArrayBuffer;
using Org::Apache::Http::Utility::EncodingUtils;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {

BasicScheme::BasicScheme()
    : RFC2617Scheme()
    , mComplete(FALSE)
{}

ECode BasicScheme::GetSchemeName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = String("basic");
    return NOERROR;
}

ECode BasicScheme::ProcessChallenge(
    /* [in] */ IHeader* header)
{
    RFC2617Scheme::ProcessChallenge(header);
    mComplete = TRUE;
    return NOERROR;
}

ECode BasicScheme::IsComplete(
    /* [out] */ Boolean* isComplete)
{
    VALIDATE_NOT_NULL(isComplete)
    *isComplete = mComplete;
    return NOERROR;
}

ECode BasicScheme::IsConnectionBased(
    /* [out] */ Boolean* connectionBased)
{
    VALIDATE_NOT_NULL(connectionBased)
    *connectionBased = FALSE;
    return NOERROR;
}

ECode BasicScheme::Authenticate(
    /* [in] */ ICredentials* credentials,
    /* [in] */ IHttpRequest* request,
    /* [out] */ IHeader** header)
{
    VALIDATE_NOT_NULL(header)
    *header = NULL;

    if (credentials == NULL) {
        Logger::E("BasicScheme", "Credentials may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (request == NULL) {
        Logger::E("BasicScheme", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IHttpParams> params;
    IHttpMessage::Probe(request)->GetParams((IHttpParams**)&params);
    String charset;
    AuthParams::GetCredentialCharset(params, &charset);
    return Authenticate(credentials, charset, IsProxy(), header);
}

ECode BasicScheme::Authenticate(
    /* [in] */ ICredentials* credentials,
    /* [in] */ const String& charset,
    /* [in] */ Boolean proxy,
    /* [out] */ IHeader** header)
{
    VALIDATE_NOT_NULL(header)
    *header = NULL;

    if (credentials == NULL) {
        Logger::E("BasicScheme", "Credentials may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (charset.IsNull()) {
        Logger::E("BasicScheme", "charset may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    StringBuilder tmp;
    AutoPtr<IPrincipal> principal;
    credentials->GetUserPrincipal((IPrincipal**)&principal);
    String name;
    principal->GetName(&name);
    tmp.Append(name);
    tmp.Append(":");
    String password;
    credentials->GetPassword(&password);
    tmp.Append(password.IsNull() ? String("null") : password);

    AutoPtr< ArrayOf<Byte> > bytes;
    EncodingUtils::GetBytes(tmp.ToString(), charset, (ArrayOf<Byte>**)&bytes);
    AutoPtr< ArrayOf<Byte> > base64password;
    assert(0);
    Logger::E("BasicScheme", "Base64 has not been implemented!");
    // AutoPtr< ArrayOf<Byte> > base64password = Base64::EncodeBase64(bytes);

    AutoPtr<ICharArrayBuffer> buffer;
    CCharArrayBuffer::New(32, (ICharArrayBuffer**)&buffer);
    if (proxy) {
        buffer->Append(IAUTH::PROXY_AUTH_RESP);
    }
    else {
        buffer->Append(IAUTH::WWW_AUTH_RESP);
    }
    buffer->Append(String(": Basic "));
    buffer->Append(base64password, 0, base64password->GetLength());

    return CBufferedHeader::New(buffer, header);
}

} // namespace Auth
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org