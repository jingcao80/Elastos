
#include "org/apache/http/auth/CAuthState.h"
#include "elastos/core/StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {

CAR_INTERFACE_IMPL(CAuthState, Object, IAuthState)

CAR_OBJECT_IMPL(CAuthState)

ECode CAuthState::Invalidate()
{
    mAuthScheme = NULL;
    mAuthScope = NULL;
    mCredentials = NULL;
    return NOERROR;
}

ECode CAuthState::IsValid(
    /* [out] */ Boolean* isValid)
{
    VALIDATE_NOT_NULL(isValid)
    *isValid = mAuthScheme != NULL;
    return NOERROR;
}

ECode CAuthState::SetAuthScheme(
    /* [in] */ IAuthScheme* authScheme)
{
    if (authScheme == NULL) {
        Invalidate();
        return NOERROR;
    }
    mAuthScheme = authScheme;
    return NOERROR;
}

ECode CAuthState::GetAuthScheme(
    /* [out] */ IAuthScheme** scheme)
{
    VALIDATE_NOT_NULL(scheme)
    *scheme = mAuthScheme;
    REFCOUNT_ADD(*scheme)
    return NOERROR;
}

ECode CAuthState::GetCredentials(
    /* [out] */ ICredentials** credentials)
{
    VALIDATE_NOT_NULL(credentials)
    *credentials = mCredentials;
    REFCOUNT_ADD(*credentials)
    return NOERROR;
}

ECode CAuthState::SetCredentials(
    /* [in] */ ICredentials* credentials)
{
    mCredentials = credentials;
    return NOERROR;
}

ECode CAuthState::GetAuthScope(
    /* [out] */ IAuthScope** authScope)
{
    VALIDATE_NOT_NULL(authScope)
    *authScope = mAuthScope;
    REFCOUNT_ADD(*authScope)
    return NOERROR;
}

ECode CAuthState::SetAuthScope(
    /* [in] */ IAuthScope* authScope)
{
    mAuthScope = authScope;
    return NOERROR;
}

ECode CAuthState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder buffer;
    buffer.Append("auth scope [");
    buffer.Append(mAuthScope);
    buffer.Append("]; credentials set [");
    buffer.Append(mCredentials != NULL ? "true" : "false");
    buffer.Append("]");
    *str = buffer.ToString();
    return NOERROR;
}

ECode CAuthState::constructor()
{
    return NOERROR;
}

} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org
