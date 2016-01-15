
#ifndef __ORG_APACHE_HTTP_AUTH_CAUTHSCOPE_H_
#define __ORG_APACHE_HTTP_AUTH_CAUTHSCOPE_H_

#include "_Org_Apache_Http_Auth_CAuthScope.h"
#include "elastos/core/Object.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {

/**
 * The class represents an authentication scope consisting of a host name,
 * a port number, a realm name and an authentication scheme name which
 * {@link Credentials Credentials} apply to.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 * @author <a href="mailto:adrian@intencha.com">Adrian Sutton</a>
 *
 * @since 4.0
 */
CarClass(CAuthScope)
    , public Object
    , public IAuthScope
{
public:
    CAuthScope()
        : mPort(0)
    {}

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * @return the host
     */
    CARAPI GetHost(
        /* [out] */ String* host);

    /**
     * @return the port
     */
    CARAPI GetPort(
        /* [out] */ Int32* port);

    /**
     * @return the realm name
     */
    CARAPI GetRealm(
        /* [out] */ String* realmName);

    /**
     * @return the scheme type
     */
    CARAPI GetScheme(
        /* [out] */ String* scheme);

    /**
     * Tests if the authentication scopes match.
     *
     * @return the match factor. Negative value signifies no match.
     *    Non-negative signifies a match. The greater the returned value
     *    the closer the match.
     */
    CARAPI Match(
        /* [in] */ IAuthScope* that,
        /* [out] */ Int32* value);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equals);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    /** Creates a new credentials scope for the given
     * <tt>host</tt>, <tt>port</tt>, <tt>realm</tt>, and
     * <tt>authentication scheme</tt>.
     *
     * @param host the host the credentials apply to. May be set
     *   to <tt>null</tt> if credenticals are applicable to
     *   any host.
     * @param port the port the credentials apply to. May be set
     *   to negative value if credenticals are applicable to
     *   any port.
     * @param realm the realm the credentials apply to. May be set
     *   to <tt>null</tt> if credenticals are applicable to
     *   any realm.
     * @param scheme the authentication scheme the credentials apply to.
     *   May be set to <tt>null</tt> if credenticals are applicable to
     *   any authentication scheme.
     */
    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& realm,
        /* [in] */ const String& scheme);

    /** Creates a new credentials scope for the given
     * <tt>host</tt>, <tt>port</tt>, <tt>realm</tt>, and any
     * authentication scheme.
     *
     * @param host the host the credentials apply to. May be set
     *   to <tt>null</tt> if credenticals are applicable to
     *   any host.
     * @param port the port the credentials apply to. May be set
     *   to negative value if credenticals are applicable to
     *   any port.
     * @param realm the realm the credentials apply to. May be set
     *   to <tt>null</tt> if credenticals are applicable to
     *   any realm.
     */
    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& realm);

    /** Creates a new credentials scope for the given
     * <tt>host</tt>, <tt>port</tt>, any realm name, and any
     * authentication scheme.
     *
     * @param host the host the credentials apply to. May be set
     *   to <tt>null</tt> if credenticals are applicable to
     *   any host.
     * @param port the port the credentials apply to. May be set
     *   to negative value if credenticals are applicable to
     *   any port.
     */
    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port);

    /**
     * Creates a copy of the given credentials scope.
     */
    CARAPI constructor(
        /* [in] */ IAuthScope* authscope);

public:
    /**
     * Default scope matching any host, port, realm and authentication scheme.
     * In the future versions of HttpClient the use of this parameter will be
     * discontinued.
     */
    static const AutoPtr<IAuthScope> ANY;

private:
    /** The authentication scheme the credentials apply to. */
    String mScheme;

    /** The realm the credentials apply to. */
    String mRealm;

    /** The host the credentials apply to. */
    String mHost;

    /** The port the credentials apply to. */
    Int32 mPort;

};

} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_AUTH_CAUTHSCOPE_H_
