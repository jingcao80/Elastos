
#include "Elastos.CoreLibrary.Net.h"
#include "org/apache/http/impl/client/RoutedRequest.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

RoutedRequest::RoutedRequest(
    /* [in] */ RequestWrapper* request,
    /* [in] */ IHttpRoute* route)
    : mRequest(request)
    , mRoute(route)
{}

AutoPtr<RequestWrapper> RoutedRequest::GetRequest()
{
    return mRequest;
}

AutoPtr<IHttpRoute> RoutedRequest::GetRoute()
{
    return mRoute;
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org