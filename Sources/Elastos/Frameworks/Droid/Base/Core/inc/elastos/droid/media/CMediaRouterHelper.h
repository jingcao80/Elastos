#ifndef __ELASTOS_DROID_MEDIA_CMEDIAROUTERHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAROUTERHELPER_H__

#include "_Elastos_Droid_Media_CMediaRouterHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Hardware::Display::IWifiDisplay;
using Elastos::Droid::Hardware::Display::IWifiDisplayStatus;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaRouterHelper)
    , public Singleton
    , public IMediaRouterHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI UpdateRoute(
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI GetRouteCountStatic(
        /* [out] */ Int32* result);

    CARAPI GetRouteAtStatic(
        /* [in] */ Int32 index,
        /* [out] */ IMediaRouterRouteInfo** result);

    CARAPI DispatchRouteVolumeChanged(
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI DispatchRouteGrouped(
        /* [in] */ IMediaRouterRouteInfo* info,
        /* [in] */ IMediaRouterRouteGroup* group,
        /* [in] */ Int32 index);

    CARAPI DispatchRouteUngrouped(
        /* [in] */ IMediaRouterRouteInfo* info,
        /* [in] */ IMediaRouterRouteGroup* group);

    CARAPI RemoveRouteStatic(
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI SelectRouteStatic(
        /* [in] */ Int32 types,
        /* [in] */ IMediaRouterRouteInfo* route,
        /* [in] */ Boolean isexplicit);

    CARAPI SelectDefaultRouteStatic();

    CARAPI MatchesDeviceAddress(
        /* [in] */ IWifiDisplay* display,
        /* [in] */ IMediaRouterRouteInfo* info,
        /* [out] */ Boolean* result);

    CARAPI AddRouteStatic(
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI DispatchRouteSelected(
        /* [in] */ Int32 type,
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI DispatchRouteUnselected(
        /* [in] */ Int32 type,
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI DispatchRouteChanged(
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI DispatchRouteChanged(
        /* [in] */ IMediaRouterRouteInfo* info,
        /* [in] */ Int32 oldSupportedTypes);

    CARAPI DispatchRouteAdded(
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI DispatchRouteRemoved(
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI DispatchRoutePresentationDisplayChanged(
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI SystemVolumeChanged(
        /* [in] */ Int32 newValue);

    CARAPI UpdateWifiDisplayStatus(
        /* [in] */ IWifiDisplayStatus* status);

    CARAPI GetWifiDisplayStatusCode(
        /* [in] */ IWifiDisplay* d,
        /* [in] */ IWifiDisplayStatus* wfdStatus,
        /* [out] */ Int32* result);

    CARAPI IsWifiDisplayEnabled(
        /* [in] */ IWifiDisplay* d,
        /* [in] */ IWifiDisplayStatus* wfdStatus,
        /* [out] */ Boolean* result);

    CARAPI MakeWifiDisplayRoute(
        /* [in] */ IWifiDisplay* display,
        /* [in] */ IWifiDisplayStatus* wfdStatus,
        /* [out] */ IMediaRouterRouteInfo** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAROUTERHELPER_H__