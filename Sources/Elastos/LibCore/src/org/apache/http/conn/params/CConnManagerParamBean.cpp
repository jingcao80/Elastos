
#include "org/apache/http/conn/params/CConnManagerParamBean.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

CAR_INTERFACE_IMPL(CConnManagerParamBean, Object, IConnManagerParamBean)

CAR_OBJECT_IMPL(CConnManagerParamBean)

ECode CConnManagerParamBean::SetTimeout(
    /* [in] */ Int64 timeout)
{
    AutoPtr<IHttpParams> p;
    return mParams->SetInt64Parameter(IConnManagerPNames::TIMEOUT, timeout,
            (IHttpParams**)&p);
}

ECode CConnManagerParamBean::SetMaxTotalConnections(
    /* [in] */ Int32 maxConnections)
{
    AutoPtr<IHttpParams> p;
    return mParams->SetInt32Parameter(IConnManagerPNames::MAX_TOTAL_CONNECTIONS, maxConnections,
            (IHttpParams**)&p);
}

ECode CConnManagerParamBean::SetConnectionsPerRoute(
    /* [in] */ IConnPerRouteBean* connPerRoute)
{
    AutoPtr<IHttpParams> p;
    return mParams->SetParameter(IConnManagerPNames::MAX_CONNECTIONS_PER_ROUTE, connPerRoute,
            (IHttpParams**)&p);
}

ECode CConnManagerParamBean::constructor(
    /* [in] */ IHttpParams* params)
{
    return Init(mParams);
}

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org
