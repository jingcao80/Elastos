
#ifndef __ORG_APACHE_HTTP_COOKIE_CCOOKIEIDENTITYCOMPARATOR_H_
#define __ORG_APACHE_HTTP_COOKIE_CCOOKIEIDENTITYCOMPARATOR_H_

#include "_Org_Apache_Http_Cookie_CCookieIdentityComparator.h"
#include "elastos/core/Object.h"

using Elastos::Core::IComparator;
using Elastos::IO::ISerializable;

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {

/**
 * This cookie comparator can be used to compare identity of cookies.
 *
 * <p>
 *  Cookies are considered identical if their names are equal and
 *  their domain attributes match ignoring case.
 * </p>
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 */
CarClass(CCookieIdentityComparator)
    , public Object
    , public IComparator
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Compare(
        /* [in] */ IInterface* lhs,
        /* [in] */ IInterface* rhs,
        /* [out] */ Int32* result);

private:
    static const Int64 serialVersionUID = 4466565437490631532L;

};

} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_COOKIE_CCOOKIEIDENTITYCOMPARATOR_H_
