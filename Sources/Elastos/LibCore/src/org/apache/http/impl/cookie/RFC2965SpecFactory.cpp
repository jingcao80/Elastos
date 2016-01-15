
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/impl/cookie/RFC2965SpecFactory.h"
#include "org/apache/http/impl/cookie/RFC2965Spec.h"

using Elastos::Core::ICharSequence;
using Org::Apache::Http::Cookie::EIID_ICookieSpecFactory;
using Org::Apache::Http::Cookie::Params::ICookieSpecPNames;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

CAR_INTERFACE_IMPL(RFC2965SpecFactory, Object, ICookieSpecFactory)

ECode RFC2965SpecFactory::NewInstance(
    /* [in] */ IHttpParams* params,
    /* [out] */ ICookieSpec** spec)
{
    VALIDATE_NOT_NULL(spec)
    if (params != NULL) {
        AutoPtr<IInterface> param;
        params->GetParameter(ICookieSpecPNames::DATE_PATTERNS, (IInterface**)&param);
        AutoPtr<ICollection> collection = ICollection::Probe(param);
        AutoPtr< ArrayOf<IInterface*> > paramArray;
        collection->ToArray((ArrayOf<IInterface*>**)&paramArray);
        AutoPtr< ArrayOf<String> > strs = ArrayOf<String>::Alloc(paramArray->GetLength());
        for (Int32 i = 0; i < paramArray->GetLength(); i++) {
            AutoPtr<ICharSequence> cs = ICharSequence::Probe((*paramArray)[i]);
            cs->ToString(&(*strs)[i]);
        }
        Boolean value;
        params->GetBooleanParameter(ICookieSpecPNames::SINGLE_COOKIE_HEADER, FALSE, &value);
        *spec = (ICookieSpec*)new RFC2965Spec(strs, value);
    }
    else {
        *spec = (ICookieSpec*)new RFC2965Spec();
    }
    REFCOUNT_ADD(*spec)
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org