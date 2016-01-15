
#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_NETSCAPEDRAFTSPEC_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_NETSCAPEDRAFTSPEC_H__

#include "org/apache/http/impl/cookie/CookieSpecBase.h"

using Elastos::Utility::IList;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::Cookie::ICookieSpec;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

class NetScapeDraftSpec : public CookieSpecBase
{
public:
    NetScapeDraftSpec(
        /* [in] */ ArrayOf<String>* datepatterns);

    NetScapeDraftSpec();

    using CookieSpecBase::Parse;

    /**
      * Parses the Set-Cookie value into an array of <tt>Cookie</tt>s.
      *
      * <p>Syntax of the Set-Cookie HTTP Response Header:</p>
      *
      * <p>This is the format a CGI script would use to add to
      * the HTTP headers a new piece of data which is to be stored by
      * the client for later retrieval.</p>
      *
      * <PRE>
      *  Set-Cookie: NAME=VALUE; expires=DATE; path=PATH; domain=DOMAIN_NAME; secure
      * </PRE>
      *
      * <p>Please note that Netscape draft specification does not fully
      * conform to the HTTP header format. Netscape draft does not specify
      * whether multiple cookies may be sent in one header. Hence, comma
      * character may be present in unquoted cookie value or unquoted
      * parameter value.</p>
      *
      * @see <a href="http://wp.netscape.com/newsref/std/cookie_spec.html">
      *  The Cookie Spec.</a>
      *
      * @param header the <tt>Set-Cookie</tt> received from the server
      * @return an array of <tt>Cookie</tt>s parsed from the Set-Cookie value
      * @throws MalformedCookieException if an exception occurs during parsing
      */
    CARAPI Parse(
        /* [in] */ IHeader* header,
        /* [in] */ ICookieOrigin* origin,
        /* [out] */ IList** cookies);

    CARAPI FormatCookies(
        /* [in] */ IList* cookies,
        /* [out] */ IList** headers);

    CARAPI GetVersion(
        /* [out] */ Int32* version);

    CARAPI GetVersionHeader(
        /* [out] */ IHeader** header);

private:
    CARAPI_(void) Init(
        /* [in] */ ArrayOf<String>* datepatterns);

protected:
    static const String EXPIRES_PATTERN;

private:
    AutoPtr< ArrayOf<String> > mDatepatterns;
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_NETSCAPEDRAFTSPEC_H__
