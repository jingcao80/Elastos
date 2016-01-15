
#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_REDIRECTLOCATIONS_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_REDIRECTLOCATIONS_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/core/Object.h"

using Elastos::Net::IURI;
using Elastos::Utility::ISet;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * A collection of URIs that were used as redirects.
 */
class RedirectLocations : public Object
{
public:
    RedirectLocations();

    /**
     * Returns true if this collection contains the given URI.
     */
    CARAPI_(Boolean) Contains(
        /* [in] */ IURI* uri);

    /**
     * Adds a new URI to the list of redirects.
     */
    CARAPI_(void) Add(
        /* [in] */ IURI* uri);

    /**
     * Removes a URI from the list of redirects.
     */
    CARAPI_(Boolean) Remove(
        /* [in] */ IURI* uri);

private:
    AutoPtr<ISet> mUris;
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_REDIRECTLOCATIONS_H__
