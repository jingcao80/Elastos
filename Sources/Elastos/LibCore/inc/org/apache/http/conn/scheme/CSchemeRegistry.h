
#ifndef __ORG_APACHE_HTTP_CONN_SCHEME_CSCHEMEREGISTRY_H_
#define __ORG_APACHE_HTTP_CONN_SCHEME_CSCHEMEREGISTRY_H_

#include "Elastos.CoreLibrary.Utility.h"
#include "_Org_Apache_Http_Conn_Scheme_CSchemeRegistry.h"
#include "elastos/core/Object.h"

using Elastos::Net::IInetAddress;
using Elastos::Net::ISocket;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Scheme {

/**
 * A set of supported protocol {@link Scheme schemes}.
 * Schemes are identified by lowercase names.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 *
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version   $Revision: 648356 $ $Date: 2008-04-15 10:57:53 -0700 (Tue, 15 Apr 2008) $
 *
 * @since 4.0
 */
CarClass(CSchemeRegistry)
    , public Object
    , public ISchemeRegistry
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Obtains a scheme by name.
     *
     * @param name      the name of the scheme to look up (in lowercase)
     *
     * @return  the scheme, never <code>null</code>
     *
     * @throws IllegalStateException
     *          if the scheme with the given name is not registered
     */
    CARAPI GetScheme(
        /* [in] */ const String& name,
        /* [out] */ IScheme** scheme);

    /**
     * Obtains the scheme for a host.
     * Convenience method for <code>getScheme(host.getSchemeName())</pre>
     *
     * @param host      the host for which to obtain the scheme
     *
     * @return  the scheme for the given host, never <code>null</code>
     *
     * @throws IllegalStateException
     *          if a scheme with the respective name is not registered
     */
    CARAPI GetScheme(
        /* [in] */ IHttpHost* host,
        /* [out] */ IScheme** scheme);

    /**
     * Obtains a scheme by name, if registered.
     *
     * @param name      the name of the scheme to look up (in lowercase)
     *
     * @return  the scheme, or
     *          <code>null</code> if there is none by this name
     */
    CARAPI Get(
        /* [in] */ const String& name,
        /* [out] */ IScheme** scheme);

    /**
     * Registers a scheme.
     * The scheme can later be retrieved by its name
     * using {@link #getScheme(String) getScheme} or {@link #get get}.
     *
     * @param sch       the scheme to register
     *
     * @return  the scheme previously registered with that name, or
     *          <code>null</code> if none was registered
     */
    CARAPI Register(
        /* [in] */ IScheme* sch,
        /* [out] */ IScheme** scheme);

    /**
     * Unregisters a scheme.
     *
     * @param name      the name of the scheme to unregister (in lowercase)
     *
     * @return  the unregistered scheme, or
     *          <code>null</code> if there was none
     */
    CARAPI Unregister(
        /* [in] */ const String& name,
        /* [out] */ IScheme** scheme);

    /**
     * Obtains the names of the registered schemes in their default order.
     *
     * @return  List containing registered scheme names.
     */
    CARAPI GetSchemeNames(
        /* [out] */ IList** names);

    /**
     * Populates the internal collection of registered {@link Scheme protocol schemes}
     * with the content of the map passed as a parameter.
     *
     * @param map protocol schemes
     */
    CARAPI SetItems(
        /* [in] */ IMap* map);

    /**
     * Creates a new, empty scheme registry.
     */
    CARAPI constructor();

private:
    /** The available schemes in this registry. */
    AutoPtr<IMap> mRegisteredSchemes;

};

} // namespace Scheme
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_SCHEME_CSCHEMEREGISTRY_H_
