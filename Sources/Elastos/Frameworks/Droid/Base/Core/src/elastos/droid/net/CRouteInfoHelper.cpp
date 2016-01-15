
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CRouteInfoHelper.h"
#include "elastos/droid/net/RouteInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CRouteInfoHelper, Singleton, IRouteInfoHelper)

CAR_SINGLETON_IMPL(CRouteInfoHelper)

ECode CRouteInfoHelper::MakeHostRoute(
    /* [in] */ IInetAddress* host,
    /* [in] */ const String& iface,
    /* [out] */ IRouteInfo** result)
{
    return RouteInfo::MakeHostRoute(host, iface, result);
}

ECode CRouteInfoHelper::MakeHostRoute(
    /* [in] */ IInetAddress* host,
    /* [in] */ IInetAddress* gateway,
    /* [in] */ const String& iface,
    /* [out] */ IRouteInfo** result)
{
    return RouteInfo::MakeHostRoute(host, gateway, iface, result);
}

ECode CRouteInfoHelper::SelectBestRoute(
    /* [in] */ ICollection* routes,
    /* [in] */ IInetAddress* dest,
    /* [out] */ IRouteInfo** info)
{
    return RouteInfo::SelectBestRoute(routes, dest, info);
}

} //namespace Net
} //namespace Droid
} //namespace Elastos
