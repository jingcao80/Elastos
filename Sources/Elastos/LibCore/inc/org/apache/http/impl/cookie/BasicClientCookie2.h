
#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_BASICCLIENTCOOKIE2_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_BASICCLIENTCOOKIE2_H__

#include "org/apache/http/impl/cookie/BasicClientCookie.h"

using Org::Apache::Http::Cookie::ISetCookie2;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

/**
 * HTTP "magic-cookie" represents a piece of state information
 * that the HTTP agent and the target server can exchange to maintain
 * a session as specified by RFC2965.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 */
class BasicClientCookie2
    : public BasicClientCookie
    , public ISetCookie2
{
public:
    BasicClientCookie2();

    /**
     * Default Constructor taking a name and a value. The value may be null.
     *
     * @param name The name.
     * @param value The value.
     */
    BasicClientCookie2(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CAR_INTERFACE_DECL()

    CARAPI GetPorts(
        /* [out, callee] */ ArrayOf<Int32>** ports);

    CARAPI SetPorts(
        /* [in] */ ArrayOf<Int32>* ports);

    CARAPI GetCommentURL(
        /* [out] */ String* commentURL);

    CARAPI SetCommentURL(
        /* [in] */ const String& commentURL);

    CARAPI SetDiscard(
        /* [in] */ Boolean discard);

    CARAPI IsPersistent(
        /* [out] */ Boolean* isPersistent);

    CARAPI IsExpired(
        /* [in] */ IDate* date,
        /* [out] */ Boolean* isExpired);

    CARAPI Clone(
        /* [out] */ IInterface** object);

protected:
    CARAPI CloneImpl(
        /* [in] */ BasicClientCookie2* cookie);

private:
    String mCommentURL;
    AutoPtr< ArrayOf<Int32> > mPorts;
    Boolean mDiscard;
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_BASICCLIENTCOOKIE2_H__
