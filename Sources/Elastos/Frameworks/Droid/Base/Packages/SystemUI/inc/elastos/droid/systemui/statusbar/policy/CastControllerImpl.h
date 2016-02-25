
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CASTCONTROLLERIMPL_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CASTCONTROLLERIMPL_H__

#include "_SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/media/MediaRouterSimpleCallback.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IMediaRouter;
using Elastos::Droid::Media::IMediaRouterRouteInfo;
using Elastos::Droid::Media::IMediaRouterCallback;
using Elastos::Droid::Media::IMediaRouterSimpleCallback;
using Elastos::Droid::Media::MediaRouterSimpleCallback;
using Elastos::Droid::Media::Projection::IMediaProjectionInfo;
using Elastos::Droid::Media::Projection::IMediaProjectionManager;
using Elastos::Droid::Media::Projection::IMediaProjectionManagerCallback;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

/** Platform implementation of the cast controller. **/
class CastControllerImpl
    : public Object
    , public ICastController
{
private:
    class MediaCallback
        : public MediaRouterSimpleCallback
    {
    public:
        MediaCallback(
            /* [in] */ CastControllerImpl* host);

        // @Override
        CARAPI OnRouteAdded(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* route);

        // @Override
        CARAPI OnRouteChanged(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* route);

        // @Override
        CARAPI OnRouteRemoved(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* route);

        // @Override
        CARAPI OnRouteSelected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IMediaRouterRouteInfo* route);

        // @Override
        CARAPI OnRouteUnselected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IMediaRouterRouteInfo* route);

    private:
        CastControllerImpl* mHost;
    };

    class MediaProjectionManagerCallback
        : public Object
        , public IMediaProjectionManagerCallback
    {
    public:
        CAR_INTERFACE_DECL();

        MediaProjectionManagerCallback(
            /* [in] */ CastControllerImpl* host);

        // @Override
        CARAPI OnStart(
            /* [in] */ IMediaProjectionInfo* info);

        // @Override
        CARAPI OnStop(
            /* [in] */ IMediaProjectionInfo* info);

    private:
        CastControllerImpl* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CastControllerImpl(
        /* [in] */ IContext* context);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    // @Override
    CARAPI AddCallback(
        /* [in] */ ICastControllerCallback* callback);

    // @Override
    CARAPI RemoveCallback(
        /* [in] */ ICastControllerCallback* callback);

    // @Override
    CARAPI SetDiscovering(
        /* [in] */ Boolean request);

    // @Override
    CARAPI SetCurrentUserId(
        /* [in] */ Int32 currentUserId);

    // @Override
    CARAPI GetCastDevices(
        /* [out] */ ISet** devices);

    // @Override
    CARAPI StartCasting(
        /* [in] */ ICastControllerCastDevice* device);

    // @Override
    CARAPI StopCasting(
        /* [in] */ ICastControllerCastDevice* device);

private:
    CARAPI_(void) HandleDiscoveryChangeLocked();

    CARAPI_(void) SetProjection(
        /* [in] */ IMediaProjectionInfo* projection,
        /* [in] */ Boolean started);

    CARAPI_(String) GetAppName(
        /* [in] */ const String& packageName);

    CARAPI_(void) UpdateRemoteDisplays();

    CARAPI_(void) EnsureTagExists(
        /* [in] */ IMediaRouterRouteInfo* route);

    CARAPI_(void) FireOnCastDevicesChanged();

    CARAPI_(void) FireOnCastDevicesChanged(
        /* [in] */ ICastControllerCallback* callback);

    static CARAPI_(String) RouteToString(
        /* [in] */ IMediaRouterRouteInfo* route);

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IContext> mContext;
    AutoPtr<IArrayList/*<Callback*/> mCallbacks;
    AutoPtr<IMediaRouter> mMediaRouter;
    AutoPtr<IArrayMap/*<String, RouteInfo*/> mRoutes;
    Object mDiscoveringLock/* = new Object()*/;
    AutoPtr<IMediaProjectionManager> mProjectionManager;
    Object mProjectionLock/* = new Object()*/;

    Boolean mDiscovering;
    Boolean mCallbackRegistered;
    AutoPtr<IMediaProjectionInfo> mProjection;
    AutoPtr<IMediaRouterSimpleCallback> mMediaCallback;
    AutoPtr<IMediaProjectionManagerCallback> mProjectionCallback;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CASTCONTROLLERIMPL_H__
