
#ifndef __ELASTOS_DROID_SERVER_MEDIA_CMEDIAROUTERSERVICE_H__
#define __ELASTOS_DROID_SERVER_MEDIA_CMEDIAROUTERSERVICE_H__

#include "_Elastos_Droid_Server_Media_CMediaRouterService.h"
#include <Elastos.Droid.Utility.h>
#define HASH_FOR_OS
#include "elastos/droid/ext/frameworkhash.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/media/RemoteDisplayProviderWatcher.h"
#include "elastos/droid/server/media/RemoteDisplayProviderProxy.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Media::IIMediaRouterService;
using Elastos::Droid::Media::IMediaRouterClientState;
using Elastos::Droid::Media::IMediaRouterClientStateRouteInfo;
using Elastos::Droid::Media::IIMediaRouterClient;
using Elastos::Droid::Media::IRemoteDisplayState;
using Elastos::Droid::Media::IRemoteDisplayStateRemoteDisplayInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Server::IWatchdogMonitor;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

/**
 * Provides a mechanism for discovering media routes and manages media playback
 * behalf of applications.
 * <p>
 * Currently supports discovering remote displays via remote display provider
 * services that have been registered by applications.
 * </p>
 */
CarClass(CMediaRouterService)
    , public Object
    , public IIMediaRouterService
    , public IWatchdogMonitor
    , public IBinder
{
public:
    class UserRecord;

    /**
     * Information about a particular client of the media router.
     * The contents of this object is guarded by mLock.
     */
    class ClientRecord
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        ClientRecord(
            /* [in] */ UserRecord* userRecord,
            /* [in] */ IIMediaRouterClient* client,
            /* [in] */ Int32 pid,
            /* [in] */ const String& packageName,
            /* [in] */ Boolean trusted,
            /* [in] */ CMediaRouterService* host)
            : mUserRecord(userRecord)
            , mClient(client)
            , mPid(pid)
            , mPackageName(packageName)
            , mTrusted(trusted)
            , mRouteTypes(0)
            , mActiveScan(FALSE)
            , mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI_(void) Dispose();

        // @Override
        CARAPI ProxyDied();

        CARAPI_(AutoPtr<IMediaRouterClientState>) GetState();

        // public void dump(PrintWriter pw, String prefix);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        AutoPtr<UserRecord> mUserRecord;
        AutoPtr<IIMediaRouterClient> mClient;
        Int32 mPid;
        String mPackageName;
        Boolean mTrusted;

        Int32 mRouteTypes;
        Boolean mActiveScan;
        String mSelectedRouteId;

    private:
        CMediaRouterService* mHost;
    };

    class UserHandler;
    /**
     * Information about a particular user.
     * The contents of this object is guarded by mLock.
     */
    class UserRecord : public Object
    {
    public:
        UserRecord(
            /* [in] */ Int32 userId,
            /* [in] */ CMediaRouterService* host);

        // public void dump(final PrintWriter pw, String prefix);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        Int32 mUserId;
        List<AutoPtr<ClientRecord> > mClientRecords;
        AutoPtr<UserHandler> mHandler;
        AutoPtr<IMediaRouterClientState> mTrustedState;
        AutoPtr<IMediaRouterClientState> mUntrustedState;
    };

    /**
     * Media router handler
     * <p>
     * Since remote display providers are designed to be single-threaded by nature,
     * this class encapsulates all of the associated functionality and exports state
     * to the service as it evolves.
     * </p><p>
     * One important task of this class is to keep track of the current globally selected
     * route id for certain routes that have global effects, such as remote displays.
     * Global route selections override local selections made within apps.  The change
     * is propagated to all apps so that they are all in sync.  Synchronization works
     * both ways.  Whenever the globally selected route is explicitly unselected by any
     * app, then it becomes unselected globally and all apps are informed.
     * </p><p>
     * This class is currently hardcoded to work with remote display providers but
     * it is intended to be eventually extended to support more general route providers
     * similar to the support library media router.
     * </p>
     */
    class UserHandler
        : public Handler
        , public RemoteDisplayProviderWatcher::ICallback
        , public RemoteDisplayProviderProxy::ICallback
    {
    public:
        class RouteRecord;

        class ProviderRecord : public Object
        {
        public:
            ProviderRecord(
                /* [in] */ RemoteDisplayProviderProxy* provider);

            CARAPI_(AutoPtr<RemoteDisplayProviderProxy>) GetProvider();

            CARAPI_(String) GetUniquePrefix();

            CARAPI_(Boolean) UpdateDescriptor(
                /* [in] */ IRemoteDisplayState* descriptor);

            CARAPI_(void) AppendClientState(
                /* [in] */ IMediaRouterClientState* state);

            CARAPI_(AutoPtr<RouteRecord>) FindRouteByUniqueId(
                /* [in] */ const String& uniqueId);

            CARAPI_(Int32) FindRouteByDescriptorId(
                /* [in] */ const String& descriptorId);

            // public void dump(PrintWriter pw, String prefix);

            // @Override
            CARAPI ToString(
                /* [out] */ String* str);

        private:
            CARAPI_(String) AssignRouteUniqueId(
                /* [in] */ const String& descriptorId);

        private:
            AutoPtr<RemoteDisplayProviderProxy> mProvider;
            String mUniquePrefix;
            AutoPtr<IArrayList> mRoutes;
            AutoPtr<IRemoteDisplayState> mDescriptor;
        };

        class RouteRecord : public Object
        {
        public:
            RouteRecord(
                /* [in] */ ProviderRecord* providerRecord,
                /* [in] */ const String& descriptorId,
                /* [in] */ const String& uniqueId);

            CARAPI_(AutoPtr<RemoteDisplayProviderProxy>) GetProvider();

            CARAPI_(AutoPtr<ProviderRecord>) GetProviderRecord();

            CARAPI_(String) GetDescriptorId();

            CARAPI_(String) GetUniqueId();

            CARAPI_(AutoPtr<IMediaRouterClientStateRouteInfo>) GetInfo();

            CARAPI_(Boolean) IsValid();

            CARAPI_(Boolean) IsEnabled();

            CARAPI_(Int32) GetStatus();

            CARAPI_(Boolean) UpdateDescriptor(
                /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor);

            // public void dump(PrintWriter pw, String prefix);

            // @Override
            CARAPI ToString(
                /* [out] */ String* str);

        private:
            static CARAPI_(String) ComputeName(
                /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor);

            static CARAPI_(String) ComputeDescription(
                /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor);

            static CARAPI_(Int32) ComputeSupportedTypes(
                /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor);

            static CARAPI_(Boolean) ComputeEnabled(
                /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor);

            static CARAPI_(Int32) ComputeStatusCode(
                /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor);

            static CARAPI_(Int32) ComputePlaybackType(
                /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor);

            static CARAPI_(Int32) ComputePlaybackStream(
                /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor);

            static CARAPI_(Int32) ComputeVolume(
                /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor);

            static CARAPI_(Int32) ComputeVolumeMax(
                /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor);

            static CARAPI_(Int32) ComputeVolumeHandling(
                /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor);

            static CARAPI_(Int32) ComputePresentationDisplayId(
                /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor);

        private:
            AutoPtr<ProviderRecord> mProviderRecord;
            String mDescriptorId;
            AutoPtr<IMediaRouterClientStateRouteInfo> mMutableInfo;
            AutoPtr<IMediaRouterClientStateRouteInfo> mImmutableInfo;
            AutoPtr<IRemoteDisplayStateRemoteDisplayInfo> mDescriptor;
        };

    public:
        UserHandler(
            /* [in] */ CMediaRouterService* service,
            /* [in] */ UserRecord* userRecord);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        // public void dump(PrintWriter pw, String prefix);

        // @Override
        CARAPI AddProvider(
            /* [in] */ RemoteDisplayProviderProxy* provider);

        // @Override
        CARAPI RemoveProvider(
            /* [in] */ RemoteDisplayProviderProxy* provider);

        // @Override
        CARAPI OnDisplayStateChanged(
            /* [in] */ RemoteDisplayProviderProxy* provider,
            /* [in] */ IRemoteDisplayState* state);

        CARAPI_(List<AutoPtr<ProviderRecord> >::Iterator) FindProviderRecord(
            /* [in] */ RemoteDisplayProviderProxy* provider);

    private:
        CARAPI_(void) Start();

        CARAPI_(void) Stop();

        CARAPI_(void) UpdateDiscoveryRequest();

        CARAPI_(void) SelectRoute(
            /* [in] */ const String& routeId);

        CARAPI_(void) UnselectRoute(
            /* [in] */ const String& routeId);

        CARAPI_(void) UnselectGloballySelectedRoute();

        CARAPI_(void) RequestSetVolume(
            /* [in] */ const String& routeId,
            /* [in] */ Int32 volume);

        CARAPI_(void) RequestUpdateVolume(
            /* [in] */ const String& routeId,
            /* [in] */ Int32 direction);

        CARAPI_(void) UpdateProvider(
            /* [in] */ RemoteDisplayProviderProxy* provider,
            /* [in] */ IRemoteDisplayState* state);

        /**
         * This function is called whenever the state of the globally selected route
         * may have changed.  It checks the state and updates timeouts or unselects
         * the route as appropriate.
         */
        CARAPI_(void) CheckGloballySelectedRouteState();

        CARAPI_(void) UpdateConnectionTimeout(
            /* [in] */ Int32 reason);

        CARAPI_(void) ConnectionTimedOut();

        CARAPI_(void) ScheduleUpdateClientState();

        CARAPI_(void) UpdateClientState();

        CARAPI_(AutoPtr<RouteRecord>) FindRouteRecord(
            /* [in] */ const String& uniqueId);

        static CARAPI_(Int32) GetConnectionPhase(
            /* [in] */ Int32 status);

    public:
        static const Int32 MSG_START = 1;
        static const Int32 MSG_STOP = 2;
        static const Int32 MSG_UPDATE_DISCOVERY_REQUEST = 3;
        static const Int32 MSG_SELECT_ROUTE = 4;
        static const Int32 MSG_UNSELECT_ROUTE = 5;
        static const Int32 MSG_REQUEST_SET_VOLUME = 6;
        static const Int32 MSG_REQUEST_UPDATE_VOLUME = 7;

    private:
        static const Int32 MSG_UPDATE_CLIENT_STATE = 8;
        static const Int32 MSG_CONNECTION_TIMED_OUT = 9;

        static const Int32 TIMEOUT_REASON_NOT_AVAILABLE = 1;
        static const Int32 TIMEOUT_REASON_CONNECTION_LOST = 2;
        static const Int32 TIMEOUT_REASON_WAITING_FOR_CONNECTING = 3;
        static const Int32 TIMEOUT_REASON_WAITING_FOR_CONNECTED = 4;

        // The relative order of these constants is important and expresses progress
        // through the process of connecting to a route.
        static const Int32 PHASE_NOT_AVAILABLE = -1;
        static const Int32 PHASE_NOT_CONNECTED = 0;
        static const Int32 PHASE_CONNECTING = 1;
        static const Int32 PHASE_CONNECTED = 2;

        CMediaRouterService* mService;
        UserRecord* mUserRecord;
        AutoPtr<RemoteDisplayProviderWatcher> mWatcher;
        List<AutoPtr<ProviderRecord> > mProviderRecords;
        List<AutoPtr<IIMediaRouterClient> > mTempClients;

        Boolean mRunning;
        Int32 mDiscoveryMode;
        AutoPtr<RouteRecord> mGloballySelectedRouteRecord;
        Int32 mConnectionPhase;
        Int32 mConnectionTimeoutReason;
        Int64 mConnectionTimeoutStartTime;
        Boolean mClientStateUpdateScheduled;
    };

private:
    class SystemRunningBroadReceiver : public BroadcastReceiver
    {
    public:
        SystemRunningBroadReceiver(
            /* [in] */ CMediaRouterService* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info);

    private:
        CMediaRouterService* mHost;
    };

public:
    CMediaRouterService();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI_(void) SystemRunning();

    // @Override
    CARAPI Monitor();

    CARAPI RegisterClientAsUser(
        /* [in] */ IIMediaRouterClient* client,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId);

    CARAPI UnregisterClient(
        /* [in] */ IIMediaRouterClient* client);

    CARAPI GetState(
        /* [in] */ IIMediaRouterClient* client,
        /* [out] */ IMediaRouterClientState** result);

    CARAPI SetDiscoveryRequest(
        /* [in] */ IIMediaRouterClient* client,
        /* [in] */ Int32 routeTypes,
        /* [in] */ Boolean activeScan);

    // Binder call
    // A null routeId means that the client wants to unselect its current route.
    // The explicit flag indicates whether the change was explicitly requested by the
    // user or the application which may cause changes to propagate out to the rest
    // of the system.  Should be false when the change is in response to a new globally
    // selected route or a default selection.
    CARAPI SetSelectedRoute(
        /* [in] */ IIMediaRouterClient* client,
        /* [in] */ const String& routeId,
        /* [in] */ Boolean isExplicit);

    CARAPI RequestSetVolume(
        /* [in] */ IIMediaRouterClient* client,
        /* [in] */ const String& routeId,
        /* [in] */ Int32 volume);

    CARAPI RequestUpdateVolume(
        /* [in] */ IIMediaRouterClient* client,
        /* [in] */ const String& routeId,
        /* [in] */ Int32 direction);

    // public void dump(FileDescriptor fd, final PrintWriter pw, String[] args);

    CARAPI_(void) SwitchUser();

    CARAPI_(void) ClientDied(
        /* [in] */ ClientRecord* clientRecord);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI RegisterClientLocked(
        /* [in] */ IIMediaRouterClient* client,
        /* [in] */ Int32 pid,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean trusted);

    CARAPI_(void) UnregisterClientLocked(
        /* [in] */ IIMediaRouterClient* client,
        /* [in] */ Boolean died);

    CARAPI_(AutoPtr<IMediaRouterClientState>) GetStateLocked(
        /* [in] */ IIMediaRouterClient* client);

    CARAPI_(void) SetDiscoveryRequestLocked(
        /* [in] */ IIMediaRouterClient* client,
        /* [in] */ Int32 routeTypes,
        /* [in] */ Boolean activeScan);

    CARAPI_(void) SetSelectedRouteLocked(
        /* [in] */ IIMediaRouterClient* client,
        /* [in] */ const String& routeId,
        /* [in] */ Boolean isExplicit);

    CARAPI_(void) RequestSetVolumeLocked(
        /* [in] */ IIMediaRouterClient* client,
        /* [in] */ const String& routeId,
        /* [in] */ Int32 volume);

    CARAPI_(void) RequestUpdateVolumeLocked(
        /* [in] */ IIMediaRouterClient* client,
        /* [in] */ const String& routeId,
        /* [in] */ Int32 direction);

    CARAPI_(void) InitializeUserLocked(
        /* [in] */ UserRecord* userRecord);

    CARAPI_(void) DisposeUserIfNeededLocked(
        /* [in] */ UserRecord* userRecord);

    CARAPI_(void) InitializeClientLocked(
        /* [in] */ ClientRecord* clientRecord);

    CARAPI_(void) DisposeClientLocked(
        /* [in] */ ClientRecord* clientRecord,
        /* [in] */ Boolean died);

    CARAPI_(Boolean) ValidatePackageName(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

public:
    /**
     * Timeout in milliseconds for a selected route to transition from a
     * disconnected state to a connecting state.  If we don't observe any
     * progress within this interval, then we will give up and unselect the route.
     */
    static const Int64 CONNECTING_TIMEOUT = 5000;

    /**
     * Timeout in milliseconds for a selected route to transition from a
     * connecting state to a connected state.  If we don't observe any
     * progress within this interval, then we will give up and unselect the route.
     */
    static const Int64 CONNECTED_TIMEOUT = 60000;

private:
    static const String TAG;
    static const Boolean DEBUG = FALSE;

    AutoPtr<IContext> mContext;

    // State guarded by mLock.
    Object mLock;
    AutoPtr<ISparseArray> mUserRecords;
    HashMap<AutoPtr<IBinder>, AutoPtr<ClientRecord> > mAllClientRecords;
    Int32 mCurrentUserId;
};

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_MEDIA_CMEDIAROUTERSERVICE_H__
