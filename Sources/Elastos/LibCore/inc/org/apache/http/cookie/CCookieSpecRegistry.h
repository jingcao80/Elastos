
#ifndef __ORG_APACHE_HTTP_COOKIE_CCOOKIESPECREGISTRY_H_
#define __ORG_APACHE_HTTP_COOKIE_CCOOKIESPECREGISTRY_H_

#include "Elastos.CoreLibrary.Utility.h"
#include "_Org_Apache_Http_Cookie_CCookieSpecRegistry.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IMap;
using Elastos::Utility::IList;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {

/**
 * Cookie specification registry that can be used to obtain the corresponding
 * cookie specification implementation for a given type of type or version of
 * cookie.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 *
 * @since 4.0
 */
CarClass(CCookieSpecRegistry)
    , public Object
    , public ICookieSpecRegistry
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Registers a {@link CookieSpecFactory} with the given identifier.
     * If a specification with the given name already exists it will be overridden.
     * This nameis the same one used to retrieve the {@link CookieSpecFactory}
     * from {@link #getCookieSpec(String)}.
     *
     * @param name the identifier for this specification
     * @param factory the {@link CookieSpecFactory} class to register
     *
     * @see #getCookieSpec(String)
     */
    CARAPI Register(
        /* [in] */ const String& name,
        /* [in] */ ICookieSpecFactory* factory);

    /**
     * Unregisters the {@link CookieSpecFactory} with the given ID.
     *
     * @param id the identifier of the {@link CookieSpec cookie specification} to unregister
     */
    CARAPI Unregister(
        /* [in] */ const String& id);

    /**
     * Gets the {@link CookieSpec cookie specification} with the given ID.
     *
     * @param name the {@link CookieSpec cookie specification} identifier
     * @param params the {@link HttpParams HTTP parameters} for the cookie
     *  specification.
     *
     * @return {@link CookieSpec cookie specification}
     *
     * @throws IllegalStateException if a policy with the given name cannot be found
     */
    CARAPI GetCookieSpec(
        /* [in] */ const String& name,
        /* [in] */ IHttpParams* params,
        /* [out] */ ICookieSpec** spec);

    /**
     * Gets the {@link CookieSpec cookie specification} with the given name.
     *
     * @param name the {@link CookieSpec cookie specification} identifier
     *
     * @return {@link CookieSpec cookie specification}
     *
     * @throws IllegalStateException if a policy with the given name cannot be found
     */
    CARAPI GetCookieSpec(
        /* [in] */ const String& name,
        /* [out] */ ICookieSpec** spec);

    /**
     * Obtains a list containing names of all registered {@link CookieSpec cookie
     * specs} in their default order.
     *
     * Note that the DEFAULT policy (if present) is likely to be the same
     * as one of the other policies, but does not have to be.
     *
     * @return list of registered cookie spec names
     */
    CARAPI GetSpecNames(
        /* [out] */ IList** names);

    /**
     * Populates the internal collection of registered {@link CookieSpec cookie
     * specs} with the content of the map passed as a parameter.
     *
     * @param map cookie specs
     */
    CARAPI SetItems(
        /* [in] */ IMap* map);

    CARAPI constructor();

private:
    AutoPtr<IMap> mRegisteredSpecs;
};

} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_COOKIE_CCOOKIESPECREGISTRY_H_
