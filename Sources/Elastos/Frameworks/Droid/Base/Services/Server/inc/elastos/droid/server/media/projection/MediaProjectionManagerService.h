
#ifndef __ELASTOS_DROID_SERVER_MEDIA_PROJECTION_MEDIAPROJECTIONMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_MEDIA_PROJECTION_MEDIAPROJECTIONMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#define HASH_FOR_OS
#include "elastos/droid/ext/frameworkhash.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Server.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.App.h>
#include <_Elastos.Droid.Server.h>
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/media/projection/CMediaProjection.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IMediaRouter;
using Elastos::Droid::Media::IMediaRouterRouteInfo;
using Elastos::Droid::Media::IMediaRouterRouteGroup;
using Elastos::Droid::Media::IMediaRouterCallback;
using Elastos::Droid::Media::IMediaRouterSimpleCallback;
using Elastos::Droid::Media::Projection::IMediaProjectionInfo;
using Elastos::Droid::Media::Projection::IIMediaProjectionWatcherCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Server::IWatchdogMonitor;
using Elastos::Droid::Server::SystemService;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {
namespace Projection {

class CMediaProjectionManager;

/**
 * Manages MediaProjection sessions.
 *
 * The {@link MediaProjectionManagerService} manages the creation and lifetime of MediaProjections,
 * as well as the capabilities they grant. Any service using MediaProjection tokens as permission
 * grants <b>must</b> validate the token before use by calling {@link
 * IMediaProjectionService#isValidMediaProjection}.
 */
class MediaProjectionManagerService
    : public SystemService
    , public IWatchdogMonitor
{
private:
    class AddCallbackDeathRecipient
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        AddCallbackDeathRecipient(
            /* [in] */ IIMediaProjectionWatcherCallback* callback,
            /* [in] */ MediaProjectionManagerService* host)
            : mCallback(callback)
            , mHost(host)
        {}

        CAR_INTERFACE_DECL()

        //@Override
        CARAPI ProxyDied();

    private:
        AutoPtr<IIMediaProjectionWatcherCallback> mCallback;
        MediaProjectionManagerService* mHost;
    };

    class MediaRouterCallback
        : public Object
        , public IMediaRouterCallback
        , public IMediaRouterSimpleCallback
    {
    public:
        MediaRouterCallback(
            /* [in] */ MediaProjectionManagerService* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnRouteSelected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IMediaRouterRouteInfo* info);

        CARAPI OnRouteUnselected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IMediaRouterRouteInfo* info);

        CARAPI OnRouteAdded(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

        CARAPI OnRouteRemoved(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

        CARAPI OnRouteChanged(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

        CARAPI OnRouteGrouped(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info,
            /* [in] */ IMediaRouterRouteGroup* group,
            /* [in] */ Int32 index);

        CARAPI OnRouteUngrouped(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info,
            /* [in] */ IMediaRouterRouteGroup* group);

        CARAPI OnRouteVolumeChanged(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

        CARAPI OnRoutePresentationDisplayChanged(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

    private:
        MediaProjectionManagerService* mHost;
    };

    class CallbackDelegate : public Object
    {
    public:
        CallbackDelegate();

        CARAPI_(void) Add(
            /* [in] */ IIMediaProjectionCallback* callback);

        CARAPI_(void) Add(
            /* [in] */ IIMediaProjectionWatcherCallback* callback);

        CARAPI_(void) Remove(
            /* [in] */ IIMediaProjectionCallback* callback);

        CARAPI_(void) Remove(
            /* [in] */ IIMediaProjectionWatcherCallback* callback);

        CARAPI_(void) DispatchStart(
            /* [in] */ CMediaProjection* projection);

        CARAPI_(void) DispatchStop(
            /* [in] */ CMediaProjection* projection);

    private:
        HashMap<AutoPtr<IBinder>, AutoPtr<IIMediaProjectionCallback> > mClientCallbacks;
        HashMap<AutoPtr<IBinder>, AutoPtr<IIMediaProjectionWatcherCallback> > mWatcherCallbacks;
        AutoPtr<IHandler> mHandler;
        Object mLock;
    };

    class WatcherStartCallback : public Runnable
    {
    public:
        WatcherStartCallback(
            /* [in] */ IMediaProjectionInfo* info,
            /* [in] */ IIMediaProjectionWatcherCallback* callback)
            : mCallback(callback)
            , mInfo(info)
        {}

        CARAPI Run();

    private:
        AutoPtr<IIMediaProjectionWatcherCallback> mCallback;
        AutoPtr<IMediaProjectionInfo> mInfo;
    };

    class WatcherStopCallback : public Runnable
    {
    public:
        WatcherStopCallback(
            /* [in] */ IMediaProjectionInfo* info,
            /* [in] */ IIMediaProjectionWatcherCallback* callback)
            : mCallback(callback)
            , mInfo(info)
        {}

        CARAPI Run();

    private:
        AutoPtr<IIMediaProjectionWatcherCallback> mCallback;
        AutoPtr<IMediaProjectionInfo> mInfo;
    };

    class ClientStopCallback : public Runnable
    {
    public:
        ClientStopCallback(
            /* [in] */ IIMediaProjectionCallback* callback)
            : mCallback(callback)
        {}

        CARAPI Run();

    private:
        AutoPtr<IIMediaProjectionCallback> mCallback;
    };

public:
    CAR_INTERFACE_DECL()

    MediaProjectionManagerService(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnSwitchUser(
        /* [in] */ Int32 userId);

    // @Override
    CARAPI Monitor();

private:
    CARAPI_(void) StartProjectionLocked(
        /* [in] */ CMediaProjection* projection);

    CARAPI_(void) StopProjectionLocked(
        /* [in] */ CMediaProjection* projection);

    CARAPI_(void) AddCallback(
        /* [in] */ IIMediaProjectionWatcherCallback* callback);

    CARAPI_(void) RemoveCallback(
        /* [in] */ IIMediaProjectionWatcherCallback* callback);

    CARAPI_(void) LinkDeathRecipientLocked(
        /* [in] */ IIMediaProjectionWatcherCallback* callback,
        /* [in] */ IProxyDeathRecipient* deathRecipient);

    CARAPI_(void) UnlinkDeathRecipientLocked(
        /* [in] */ IIMediaProjectionWatcherCallback* callback);

    CARAPI_(void) DispatchStart(
        /* [in] */ CMediaProjection* projection);

    CARAPI_(void) DispatchStop(
        /* [in] */ CMediaProjection* projection);

    CARAPI_(Boolean) IsValidMediaProjection(
        /* [in] */ IBinder* token);

    CARAPI_(AutoPtr<IMediaProjectionInfo>) GetActiveProjectionInfo();

    // CARAPI_(void) dump(final PrintWriter pw);

    static CARAPI_(String) TypeToString(
        /* [in] */ Int32 type);

private:
    static const String TAG;;

    Object mLock; // Protects the list of media projections
    HashMap<AutoPtr<IBinder>, AutoPtr<IProxyDeathRecipient> > mDeathEaters;
    AutoPtr<CallbackDelegate> mCallbackDelegate;

    AutoPtr<IContext> mContext;
    AutoPtr<IAppOpsManager> mAppOps;

    AutoPtr<IMediaRouter> mMediaRouter;
    AutoPtr<MediaRouterCallback> mMediaRouterCallback;
    AutoPtr<IMediaRouterRouteInfo> mMediaRouteInfo;

    AutoPtr<IBinder> mProjectionToken;
    AutoPtr<CMediaProjection> mProjectionGrant;

    friend class MediaRouterCallback;
    friend class CMediaProjection;
    friend class CMediaProjectionManager;
    friend class AddCallbackDeathRecipient;
};

} // namespace Projection
} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_POWER_MEDIAPROJECTIONMANAGERSERVICE_H__
