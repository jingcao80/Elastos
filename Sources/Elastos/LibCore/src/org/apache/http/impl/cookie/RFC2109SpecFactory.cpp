
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/impl/cookie/RFC2109SpecFactory.h"
#include "org/apache/http/impl/cookie/RFC2109Spec.h"

using Elastos::Core::ICharSequence;
using Org::Apache::Http::Cookie::EIID_ICookieSpecFactory;
using Org::Apache::Http::Cookie::Params::ICookieSpecPNames;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

CAR_INTERFACE_IMPL(RFC2109SpecFactory, Object, ICookieSpecFactory)

ECode RFC2109SpecFactory::NewInstance(
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
        *spec = (ICookieSpec*)new RFC2109Spec(strs, FALSE);
    }
    else {
        *spec = (ICookieSpec*)new RFC2109Spec();
    }
    REFCOUNT_ADD(*spec)
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org