
#ifndef __ORG_APACHE_HTTP_COOKIE_CCOOKIEORIGIN_H_
#define __ORG_APACHE_HTTP_COOKIE_CCOOKIEORIGIN_H_

#include "_Org_Apache_Http_Cookie_CCookieOrigin.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;

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
CarClass(CCookieOrigin)
    , public Object
    , public ICookieOrigin
{
public:
    CCookieOrigin();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetHost(
        /* [out] */ String* host);

    CARAPI GetPath(
        /* [out] */ String* path);

    CARAPI GetPort(
        /* [out] */ Int32* port);

    CARAPI IsSecure(
        /* [out] */ Boolean* isSecure);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& path,
        /* [in] */ Boolean secure);

private:
    String mHost;
    Int32 mPort;
    String mPath;
    Boolean mSecure;

};

} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_COOKIE_CCOOKIEORIGIN_H_
