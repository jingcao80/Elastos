
#ifndef __ORG_APACHE_HTTP_CONN_SCHEME_CSCHEME_H_
#define __ORG_APACHE_HTTP_CONN_SCHEME_CSCHEME_H_

#include "_Org_Apache_Http_Conn_Scheme_CScheme.h"
#include "elastos/core/Object.h"

using Elastos::Net::IInetAddress;
using Elastos::Net::ISocket;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Scheme {

/**
 * Encapsulates specifics of a protocol scheme such as "http" or "https".
 * Schemes are identified by lowercase names.
 * Supported schemes are typically collected in a
 * {@link SchemeRegistry SchemeRegistry}.
 *
 * <p>
 * For example, to configure support for "https://" URLs,
 * you could write code like the following:
 * </p>
 * <pre>
 * Scheme https = new Scheme("https", new MySecureSocketFactory(), 443);
 * SchemeRegistry.DEFAULT.register(https);
 * </pre>
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 * @author Michael Becke
 * @author Jeff Dever
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 */
CarClass(CScheme)
    , public Object
    , public IScheme
{
public:
    CScheme();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Obtains the default port.
     *
     * @return  the default port for this scheme
     */
    CARAPI GetDefaultPort(
        /* [out] */ Int32* port);

    /**
     * Obtains the socket factory.
     * If this scheme is {@link #isLayered layered}, the factory implements
     * {@link LayeredSocketFactory LayeredSocketFactory}.
     *
     * @return  the socket factory for this scheme
     */
    CARAPI GetSocketFactory(
        /* [out] */ ISocketFactory** socketFactory);

    /**
     * Obtains the scheme name.
     *
     * @return  the name of this scheme, in lowercase
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Indicates whether this scheme allows for layered connections.
     *
     * @return <code>true</code> if layered connections are possible,
     *         <code>false</code> otherwise
     */
    CARAPI IsLayered(
        /* [out] */ Boolean* isLayered);

    /**
     * Resolves the correct port for this scheme.
     * Returns the given port if it is valid, the default port otherwise.
     *
     * @param port      the port to be resolved,
     *                  a negative number to obtain the default port
     *
     * @return the given port or the defaultPort
     */
    CARAPI ResolvePort(
        /* [in] */ Int32 port,
        /* [out] */ Int32* resolvedPort);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    /**
     * Creates a new scheme.
     * Whether the created scheme allows for layered connections
     * depends on the class of <code>factory</code>.
     *
     * @param name      the scheme name, for example "http".
     *                  The name will be converted to lowercase.
     * @param factory   the factory for creating sockets for communication
     *                  with this scheme
     * @param port      the default port for this scheme
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ ISocketFactory* factory,
        /* [in] */ Int32 port);

private:
    /** The name of this scheme, in lowercase. (e.g. http, https) */
    String mName;

    /** The socket factory for this scheme */
    AutoPtr<ISocketFactory> mSocketFactory;

    /** The default port for this scheme */
    Int32 mDefaultPort;

    /** Indicates whether this scheme allows for layered connections */
    Boolean mLayered;


    /** A string representation, for {@link #toString toString}. */
    String mStringRep;

};

} // namespace Scheme
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_SCHEME_CSCHEME_H_
