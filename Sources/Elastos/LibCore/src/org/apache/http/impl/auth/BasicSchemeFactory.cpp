
#include "org/apache/http/impl/auth/BasicSchemeFactory.h"
#include "org/apache/http/impl/auth/BasicScheme.h"

using Org::Apache::Http::Auth::EIID_IAuthSchemeFactory;
using Org::Apache::Http::Auth::EIID_IAuthScheme;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {

CAR_INTERFACE_IMPL(BasicSchemeFactory, Object, IAuthSchemeFactory)

ECode BasicSchemeFactory::NewInstance(
    /* [in] */ IHttpParams* params,
    /* [out] */ IAuthScheme** scheme)
{
    VALIDATE_NOT_NULL(scheme)
    AutoPtr<BasicScheme> s = new BasicScheme();
    *scheme = (IAuthScheme*)s->Probe(EIID_IAuthScheme);
    REFCOUNT_ADD(*scheme)
    return NOERROR;
}

} // namespace Auth
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org