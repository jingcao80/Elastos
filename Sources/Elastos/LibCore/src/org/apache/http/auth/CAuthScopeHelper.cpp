
#include "org/apache/http/auth/CAuthScopeHelper.h"
#include "org/apache/http/auth/CAuthScope.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {

CAR_INTERFACE_IMPL(CAuthScopeHelper, Object, IAuthScopeHelper)

CAR_SINGLETON_IMPL(CAuthScopeHelper)

ECode CAuthScopeHelper::GetDefaultScope(
    /* [out] */ IAuthScope** scope)
{
    VALIDATE_NOT_NULL(scope)
    *scope = CAuthScope::ANY;
    REFCOUNT_ADD(*scope)
    return NOERROR;
}

} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org
