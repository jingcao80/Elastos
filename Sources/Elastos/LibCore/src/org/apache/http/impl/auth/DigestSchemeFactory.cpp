
#include "org/apache/http/impl/auth/DigestSchemeFactory.h"
#include "org/apache/http/impl/auth/DigestScheme.h"

using Org::Apache::Http::Auth::EIID_IAuthSchemeFactory;
using Org::Apache::Http::Auth::EIID_IAuthScheme;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {

CAR_INTERFACE_IMPL(DigestSchemeFactory, Object, IAuthSchemeFactory)

ECode DigestSchemeFactory::NewInstance(
    /* [in] */ IHttpParams* params,
    /* [out] */ IAuthScheme** scheme)
{
    VALIDATE_NOT_NULL(scheme)
    AutoPtr<DigestScheme> s = new DigestScheme();
    *scheme = (IAuthScheme*)s->Probe(EIID_IAuthScheme);
    REFCOUNT_ADD(*scheme)
    return NOERROR;
}

} // namespace Auth
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org