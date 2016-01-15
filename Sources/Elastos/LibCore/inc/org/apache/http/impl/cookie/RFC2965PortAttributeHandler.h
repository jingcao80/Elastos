
#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2965PORTATTRIBUTEHANDLER_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2965PORTATTRIBUTEHANDLER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::Cookie::ICookie;
using Org::Apache::Http::Cookie::ICookieAttributeHandler;
using Org::Apache::Http::Cookie::ICookieOrigin;
using Org::Apache::Http::Cookie::ISetCookie;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

/**
 * <tt>"Port"</tt> cookie attribute handler for RFC 2965 cookie spec.
 */
class RFC2965PortAttributeHandler
    : public Object
    , public ICookieAttributeHandler
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Parse cookie port attribute.
     */
    CARAPI Parse(
        /* [in] */ ISetCookie* cookie,
        /* [in] */ const String& portValue);

    /**
     * Validate cookie port attribute. If the Port attribute was specified
     * in header, the request port must be in cookie's port list.
     */
    CARAPI Validate(
        /* [in] */ ICookie* cookie,
        /* [in] */ ICookieOrigin* origin);

    /**
     * Match cookie port attribute. If the Port attribute is not specified
     * in header, the cookie can be sent to any port. Otherwise, the request port
     * must be in the cookie's port list.
     */
    CARAPI Match(
        /* [in] */ ICookie* cookie,
        /* [in] */ ICookieOrigin* origin,
        /* [out] */ Boolean* result);

private:
     /**
     * Parses the given Port attribute value (e.g. "8000,8001,8002")
     * into an array of ports.
     *
     * @param portValue port attribute value
     * @return parsed array of ports
     * @throws MalformedCookieException if there is a problem in
     *          parsing due to invalid portValue.
     */
    static CARAPI ParsePortAttribute(
        /* [in] */ const String& portValue,
        /* [out] */ ArrayOf<Int32>** ports);

    /**
     * Returns <tt>true</tt> if the given port exists in the given
     * ports list.
     *
     * @param port port of host where cookie was received from or being sent to.
     * @param ports port list
     * @return true returns <tt>true</tt> if the given port exists in
     *         the given ports list; <tt>false</tt> otherwise.
     */
    static CARAPI_(Boolean) PortMatch(
        /* [in] */ Int32 port,
        /* [in] */ ArrayOf<Int32>* ports);
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2965PORTATTRIBUTEHANDLER_H__
