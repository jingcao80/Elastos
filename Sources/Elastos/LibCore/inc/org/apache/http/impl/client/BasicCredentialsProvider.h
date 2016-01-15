
#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_BASICCREDENTIALSPROVIDER_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_BASICCREDENTIALSPROVIDER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IHashMap;
using Org::Apache::Http::Auth::IAuthScope;
using Org::Apache::Http::Auth::ICredentials;
using Org::Apache::Http::Client::ICredentialsProvider;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * Default implementation of {@link CredentialsProvider}
 *
 * @author <a href="mailto:remm@apache.org">Remy Maucherat</a>
 * @author Rodney Waldhoff
 * @author <a href="mailto:jsdever@apache.org">Jeff Dever</a>
 * @author Sean C. Sullivan
 * @author <a href="mailto:becke@u.washington.edu">Michael Becke</a>
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:adrian@intencha.com">Adrian Sutton</a>
 *
 * @since 4.0
 */
class BasicCredentialsProvider
    : public Object
    , public ICredentialsProvider
{
public:
    /**
     * Default constructor.
     */
    BasicCredentialsProvider();

    CAR_INTERFACE_DECL()

    /**
     * Sets the {@link Credentials credentials} for the given authentication
     * scope. Any previous credentials for the given scope will be overwritten.
     *
     * @param authscope the {@link AuthScope authentication scope}
     * @param credentials the authentication {@link Credentials credentials}
     * for the given scope.
     *
     * @see #getCredentials(AuthScope)
     */
    CARAPI SetCredentials(
        /* [in] */ IAuthScope* authscope,
        /* [in] */ ICredentials* credentials);

    /**
     * Get the {@link Credentials credentials} for the given authentication scope.
     *
     * @param authscope the {@link AuthScope authentication scope}
     * @return the credentials
     *
     * @see #setCredentials(AuthScope, Credentials)
     */
    CARAPI GetCredentials(
        /* [in] */ IAuthScope* authscope,
        /* [out] */ ICredentials** credentials);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Clears all credentials.
     */
    CARAPI Clear();

private:
    /**
     * Find matching {@link Credentials credentials} for the given authentication scope.
     *
     * @param map the credentials hash map
     * @param authscope the {@link AuthScope authentication scope}
     * @return the credentials
     *
     */
    static CARAPI_(AutoPtr<ICredentials>) MatchCredentials(
        /* [in] */ IHashMap* map,
        /* [in] */ IAuthScope* authscope);

private:
    AutoPtr<IHashMap> mCredMap;
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_BASICCREDENTIALSPROVIDER_H__
