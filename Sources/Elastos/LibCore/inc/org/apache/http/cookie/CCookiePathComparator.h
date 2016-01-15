
#ifndef __ORG_APACHE_HTTP_COOKIE_CCOOKIEPATHCOMPARATOR_H_
#define __ORG_APACHE_HTTP_COOKIE_CCOOKIEPATHCOMPARATOR_H_

#include "_Org_Apache_Http_Cookie_CCookiePathComparator.h"
#include "elastos/core/Object.h"

using Elastos::Core::IComparator;
using Elastos::IO::ISerializable;

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {

/**
 * This cookie comparator ensures that multiple cookies satisfying
 * a common criteria are ordered in the <tt>Cookie</tt> header such
 * that those with more specific Path attributes precede those with
 * less specific.
 *
 * <p>
 * This comparator assumes that Path attributes of two cookies
 * path-match a commmon request-URI. Otherwise, the result of the
 * comparison is undefined.
 * </p>
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 */
CarClass(CCookiePathComparator)
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
    CARAPI_(String) NormalizePath(
        /* [in] */ ICookie* cookie);

private:
    static const Int64 serialVersionUID = 7523645369616405818L;

};

} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_COOKIE_CCOOKIEPATHCOMPARATOR_H_
