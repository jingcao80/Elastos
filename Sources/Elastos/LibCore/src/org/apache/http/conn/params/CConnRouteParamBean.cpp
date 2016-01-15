
#include "Elastos.CoreLibrary.Net.h"
#include "org/apache/http/conn/params/CConnRouteParamBean.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

CAR_INTERFACE_IMPL(CConnRouteParamBean, Object, IConnRouteParamBean)

CAR_OBJECT_IMPL(CConnRouteParamBean)

ECode CConnRouteParamBean::SetDefaultProxy(
    /* [in] */ IHttpHost* defaultProxy)
{
    AutoPtr<IHttpParams> p;
    return mParams->SetParameter(IConnRoutePNames::DEFAULT_PROXY, defaultProxy,
            (IHttpParams**)&p);
}

ECode CConnRouteParamBean::SetLocalAddress(
    /* [in] */ IInetAddress* address)
{
    AutoPtr<IHttpParams> p;
    return mParams->SetParameter(IConnRoutePNames::LOCAL_ADDRESS, address,
            (IHttpParams**)&p);
}

ECode CConnRouteParamBean::SetForcedRoute(
    /* [in] */ IHttpRoute* route)
{
    AutoPtr<IHttpParams> p;
    return mParams->SetParameter(IConnRoutePNames::FORCED_ROUTE, route,
            (IHttpParams**)&p);
}

ECode CConnRouteParamBean::constructor(
    /* [in] */ IHttpParams* params)
{
    return Init(params);
}

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org
