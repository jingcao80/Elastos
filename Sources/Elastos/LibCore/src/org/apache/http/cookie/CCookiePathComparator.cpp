
#include "org/apache/http/cookie/CCookiePathComparator.h"

using Elastos::Core::EIID_IComparator;
using Elastos::IO::EIID_ISerializable;

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {

const Int64 CCookiePathComparator::serialVersionUID;

CAR_INTERFACE_IMPL_2(CCookiePathComparator, Object, IComparator, ISerializable)

CAR_OBJECT_IMPL(CCookiePathComparator)

String CCookiePathComparator::NormalizePath(
    /* [in] */ ICookie* cookie)
{
    String path;
    cookie->GetPath(&path);
    if (path.IsNull()) {
        path = String("/");
    }
    if (!path.EndWith("/")) {
        path = path + String("/");
    }
    return path;
}

ECode CCookiePathComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICookie> c1 = ICookie::Probe(lhs);
    AutoPtr<ICookie> c2 = ICookie::Probe(rhs);
    String path1 = NormalizePath(c1);
    String path2 = NormalizePath(c2);
    if (path1.Equals(path2)) {
        *result = 0;
    }
    else if (path1.StartWith(path2)) {
        *result = -1;
    }
    else if (path2.StartWith(path1)) {
        *result = 1;
    }
    else {
        // Does not really matter
        *result = 0;
    }
    return NOERROR;
}

} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org
