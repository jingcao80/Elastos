
#ifndef __ORG_APACHE_HTTP_AUTH_CAUTHSCHEMEREGISTRY_H_
#define __ORG_APACHE_HTTP_AUTH_CAUTHSCHEMEREGISTRY_H_

#include "Elastos.CoreLibrary.Utility.h"
#include "_Org_Apache_Http_Auth_CAuthSchemeRegistry.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IMap;
using Elastos::Utility::IList;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {

/**
 * Authentication scheme registry that can be used to obtain the corresponding
 * authentication scheme implementation for a given type of authorization challenge.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 *
 * @version $Revision: 652950 $
 * @since 4.0
 */
CarClass(CAuthSchemeRegistry)
    , public Object
    , public IAuthSchemeRegistry
{
public:
    CAuthSchemeRegistry();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Registers a {@link AuthSchemeFactory} with  the given identifier. If a factory with the
     * given name already exists it will be overridden. This name is the same one used to
     * retrieve the {@link AuthScheme authentication scheme} from {@link #getAuthScheme}.
     *
     * <p>
     * Please note that custom authentication preferences, if used, need to be updated accordingly
     * for the new {@link AuthScheme authentication scheme} to take effect.
     * </p>
     *
     * @param name the identifier for this scheme
     * @param factory the {@link AuthSchemeFactory} class to register
     *
     * @see #getAuthScheme
     */
    CARAPI Register(
        /* [in] */ const String& name,
        /* [in] */ IAuthSchemeFactory* factory);

    /**
     * Unregisters the class implementing an {@link AuthScheme authentication scheme} with
     * the given name.
     *
     * @param name the identifier of the class to unregister
     */
    CARAPI Unregister(
        /* [in] */ const String& name);

    /**
     * Gets the {@link AuthScheme authentication scheme} with the given name.
     *
     * @param name the {@link AuthScheme authentication scheme} identifier
     * @param params the {@link HttpParams HTTP parameters} for the authentication
     *  scheme.
     *
     * @return {@link AuthScheme authentication scheme}
     *
     * @throws IllegalStateException if a scheme with the given name cannot be found
     */
    CARAPI GetAuthScheme(
        /* [in] */ const String& name,
        /* [in] */ IHttpParams* params,
        /* [out] */ IAuthScheme** scheme);

    /**
     * Obtains a list containing names of all registered {@link AuthScheme authentication
     * schemes} in their default order.
     *
     * @return list of registered scheme names
     */
    CARAPI GetSchemeNames(
        /* [out] */ IList** names);

    /**
     * Populates the internal collection of registered {@link AuthScheme authentication schemes}
     * with the content of the map passed as a parameter.
     *
     * @param map authentication schemes
     */
    CARAPI SetItems(
        /* [in] */ IMap* map);

    CARAPI constructor();

private:
    AutoPtr<IMap> mRegisteredSchemes;
};

} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_AUTH_CAUTHSCHEMEREGISTRY_H_
