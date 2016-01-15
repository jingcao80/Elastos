
#include "org/apache/http/conn/params/CConnManagerParams.h"
#include "org/apache/http/conn/params/ConnManagerParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

CAR_INTERFACE_IMPL_2(CConnManagerParams, Object, IConnManagerParams, IConnManagerPNames)

CAR_SINGLETON_IMPL(CConnManagerParams)

ECode CConnManagerParams::GetTimeout(
    /* [in] */ IHttpParams* params,
    /* [out] */ Int64* timeout)
{
    return ConnManagerParams::GetTimeout(params, timeout);
}

ECode CConnManagerParams::SetTimeout(
    /* [in] */ IHttpParams* params,
    /* [in] */ Int64 timeout)
{
    return ConnManagerParams::SetTimeout(params, timeout);
}

ECode CConnManagerParams::SetMaxConnectionsPerRoute(
    /* [in] */ IHttpParams* params,
    /* [in] */ IConnPerRoute* connPerRoute)
{
    return ConnManagerParams::SetMaxConnectionsPerRoute(params, connPerRoute);
}

ECode CConnManagerParams::GetMaxConnectionsPerRoute(
    /* [in] */ IHttpParams* params,
    /* [out] */ IConnPerRoute** route)
{
    return ConnManagerParams::GetMaxConnectionsPerRoute(params, route);
}

ECode CConnManagerParams::SetMaxTotalConnections(
    /* [in] */ IHttpParams* params,
    /* [in] */ Int32 maxTotalConnections)
{
    return ConnManagerParams::SetMaxTotalConnections(params, maxTotalConnections);
}

ECode CConnManagerParams::GetMaxTotalConnections(
    /* [in] */ IHttpParams* params,
    /* [out] */ Int32* number)
{
    return ConnManagerParams::GetMaxTotalConnections(params, number);
}

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org