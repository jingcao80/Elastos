
#include "org/apache/http/auth/params/CAuthParams.h"
#include "org/apache/http/auth/params/AuthParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {
namespace Params {

CAR_INTERFACE_IMPL(CAuthParams, Object, IAuthParams)

CAR_SINGLETON_IMPL(CAuthParams)

ECode CAuthParams::GetCredentialCharset(
    /* [in] */ IHttpParams* params,
    /* [out] */ String* charset)
{
    return AuthParams::GetCredentialCharset(params, charset);
}

ECode CAuthParams::SetCredentialCharset(
    /* [in] */ IHttpParams* params,
    /* [in] */ const String& charset)
{
    return AuthParams::SetCredentialCharset(params, charset);
}

} // namespace Params
} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org
