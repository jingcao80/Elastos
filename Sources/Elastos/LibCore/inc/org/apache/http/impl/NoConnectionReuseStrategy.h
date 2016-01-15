
#ifndef __ORG_APACHE_HTTP_IMPL_NOCONNECTIONREUSESTRATETY_H__
#define __ORG_APACHE_HTTP_IMPL_NOCONNECTIONREUSESTRATETY_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IConnectionReuseStrategy;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

/**
 * A strategy that never re-uses a connection.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 *
 * @version $Revision: 502684 $
 *
 * @since 4.0
 */
class NoConnectionReuseStrategy
    : public Object
    , public IConnectionReuseStrategy
{
public:
    CAR_INTERFACE_DECL()

    CARAPI KeepAlive(
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpContext* context,
        /* [out] */ Boolean* result);
};

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_NOCONNECTIONREUSESTRATETY_H__
