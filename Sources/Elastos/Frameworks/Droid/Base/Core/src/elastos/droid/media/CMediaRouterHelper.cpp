#include "elastos/droid/media/CMediaRouter.h"
#include "elastos/droid/media/CMediaRouterHelper.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CMediaRouterHelper)

CAR_INTERFACE_IMPL(CMediaRouterHelper, Singleton, IMediaRouterHelper)

ECode CMediaRouterHelper::UpdateRoute(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CMediaRouter::UpdateRoute(info);
}

ECode CMediaRouterHelper::GetRouteCountStatic(
    /* [out] */ Int32* result)
{
    return CMediaRouter::GetRouteCountStatic(result);
}

ECode CMediaRouterHelper::GetRouteAtStatic(
    /* [in] */ Int32 index,
    /* [out] */ IMediaRouterRouteInfo** result)
{
    return CMediaRouter::GetRouteAtStatic(index, result);
}

ECode CMediaRouterHelper::DispatchRouteVolumeChanged(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CMediaRouter::DispatchRouteVolumeChanged(info);
}

ECode CMediaRouterHelper::DispatchRouteGrouped(
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ IMediaRouterRouteGroup* group,
    /* [in] */ Int32 index)
{
    return CMediaRouter::DispatchRouteGrouped(info, group, index);
}

ECode CMediaRouterHelper::DispatchRouteUngrouped(
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ IMediaRouterRouteGroup* group)
{
    return CMediaRouter::DispatchRouteUngrouped(info, group);
}

ECode CMediaRouterHelper::RemoveRouteStatic(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CMediaRouter::RemoveRouteStatic(info);
}

ECode CMediaRouterHelper::SelectRouteStatic(
    /* [in] */ Int32 types,
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [in] */ Boolean isexplicit)
{
    return CMediaRouter::SelectRouteStatic(types, route, isexplicit);
}

ECode CMediaRouterHelper::SelectDefaultRouteStatic()
{
    return CMediaRouter::SelectDefaultRouteStatic();
}

ECode CMediaRouterHelper::MatchesDeviceAddress(
    /* [in] */ IWifiDisplay* display,
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [out] */ Boolean* result)
{
    return CMediaRouter::MatchesDeviceAddress(display, info, result);
}

ECode CMediaRouterHelper::AddRouteStatic(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CMediaRouter::AddRouteStatic(info);
}

ECode CMediaRouterHelper::DispatchRouteSelected(
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CMediaRouter::DispatchRouteSelected(type, info);
}

ECode CMediaRouterHelper::DispatchRouteUnselected(
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CMediaRouter::DispatchRouteUnselected(type, info);
}

ECode CMediaRouterHelper::DispatchRouteChanged(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CMediaRouter::DispatchRouteChanged(info);
}

ECode CMediaRouterHelper::DispatchRouteChanged(
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ Int32 oldSupportedTypes)
{
    return CMediaRouter::DispatchRouteChanged(info, oldSupportedTypes);
}

ECode CMediaRouterHelper::DispatchRouteAdded(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CMediaRouter::DispatchRouteAdded(info);
}

ECode CMediaRouterHelper::DispatchRouteRemoved(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CMediaRouter::DispatchRouteRemoved(info);
}

ECode CMediaRouterHelper::DispatchRoutePresentationDisplayChanged(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CMediaRouter::DispatchRoutePresentationDisplayChanged(info);
}

ECode CMediaRouterHelper::SystemVolumeChanged(
    /* [in] */ Int32 newValue)
{
    return CMediaRouter::SystemVolumeChanged(newValue);
}

ECode CMediaRouterHelper::UpdateWifiDisplayStatus(
    /* [in] */ IWifiDisplayStatus* status)
{
    return CMediaRouter::UpdateWifiDisplayStatus(status);
}

ECode CMediaRouterHelper::GetWifiDisplayStatusCode(
    /* [in] */ IWifiDisplay* d,
    /* [in] */ IWifiDisplayStatus* wfdStatus,
    /* [out] */ Int32* result)
{
    return CMediaRouter::GetWifiDisplayStatusCode(d, wfdStatus, result);
}

ECode CMediaRouterHelper::IsWifiDisplayEnabled(
    /* [in] */ IWifiDisplay* d,
    /* [in] */ IWifiDisplayStatus* wfdStatus,
    /* [out] */ Boolean* result)
{
    return CMediaRouter::IsWifiDisplayEnabled(d, wfdStatus, result);
}

ECode CMediaRouterHelper::MakeWifiDisplayRoute(
    /* [in] */ IWifiDisplay* display,
    /* [in] */ IWifiDisplayStatus* wfdStatus,
    /* [out] */ IMediaRouterRouteInfo** result)
{
    return CMediaRouter::MakeWifiDisplayRoute(display, wfdStatus, result);
}


} // namespace Media
} // namepsace Droid
} // namespace Elastos