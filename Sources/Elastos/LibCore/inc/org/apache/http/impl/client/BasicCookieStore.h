
#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_BASICCOOKIESTORE_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_BASICCOOKIESTORE_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Core::IComparator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IDate;
using Elastos::Utility::IList;
using Org::Apache::Http::Client::ICookieStore;
using Org::Apache::Http::Cookie::ICookie;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * Default implementation of {@link CookieStore}
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
class BasicCookieStore
    : public Object
    , public ICookieStore
{
public:
    BasicCookieStore();

    CAR_INTERFACE_DECL()

    /**
     * Adds an {@link Cookie HTTP cookie}, replacing any existing equivalent cookies.
     * If the given cookie has already expired it will not be added, but existing
     * values will still be removed.
     *
     * @param cookie the {@link Cookie cookie} to be added
     *
     * @see #addCookies(Cookie[])
     *
     */
    CARAPI AddCookie(
        /* [in] */ ICookie* cookie);

    /**
     * Adds an array of {@link Cookie HTTP cookies}. Cookies are added individually and
     * in the given array order. If any of the given cookies has already expired it will
     * not be added, but existing values will still be removed.
     *
     * @param cookies the {@link Cookie cookies} to be added
     *
     * @see #addCookie(Cookie)
     *
     */
    CARAPI_(void) AddCookies(
        /* [in] */ ArrayOf<ICookie*>* cookies);

    /**
     * Returns an immutable array of {@link Cookie cookies} that this HTTP
     * state currently contains.
     *
     * @return an array of {@link Cookie cookies}.
     */
    CARAPI GetCookies(
        /* [out] */ IList** cookies);

    /**
     * Removes all of {@link Cookie cookies} in this HTTP state
     * that have expired by the specified {@link java.util.Date date}.
     *
     * @return true if any cookies were purged.
     *
     * @see Cookie#isExpired(Date)
     */
    CARAPI ClearExpired(
        /* [in] */ IDate* date,
        /* [out] */ Boolean* purged);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Clears all cookies.
     */
    CARAPI Clear();

private:
    AutoPtr<IArrayList> mCookies;

    AutoPtr<IComparator> mCookieComparator;
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_BASICCOOKIESTORE_H__
