
#ifndef __ELASTOS_DROID_SERVER_LOCATION_CLOCATIONMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_LOCATION_CLOCATIONMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_Location_CLocationManagerService.h"
#include "location/LocationFudger.h"
#include "location/GeofenceManager.h"
#include "location/PassiveProvider.h"
#include "location/GeocoderProxy.h"
#include "location/LocationBlacklist.h"
#include "location/MockProvider.h"
#include "location/LocationProviderProxy.h"
#include "elastos/droid/os/HandlerBase.h"
#include "elastos/droid/database/ContentObserver.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Location::ICriteria;
using Elastos::Droid::Location::IGpsStatusListener;
using Elastos::Droid::Location::IIGpsStatusProvider;
using Elastos::Droid::Location::IINetInitiatedListener;
using Elastos::Droid::Database::EIID_IContentObserver;
using Elastos::Droid::Location::IILocationListener;
using Elastos::Droid::Location::IIGpsStatusListener;


namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

CarClass(CLocationManagerService)
{
    class Receiver;
    class UpdateRecord;

private:
    class LocationWorkerHandler : public HandlerBase
    {
    public:
        LocationWorkerHandler(
            /* [in] */ CLocationManagerService* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CLocationManagerService* mHost;
    };

    typedef HashMap<AutoPtr<IInterface>, AutoPtr<Receiver> > InterfaceReceiverMap;
    typedef HashMap<AutoPtr<IInterface>, AutoPtr<Receiver> >::Iterator InterfaceReceiverIterator;
    class LocationPackageMonitor : public PackageMonitor
    {
    public:
        LocationPackageMonitor(
            /* [in] */ CLocationManagerService* host);

        CARAPI OnPackageDisappeared(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 reason);

    private:

        Object mLock;
        CLocationManagerService* mHost;

    };

    class LocationContentObserver : public ContentObserver
    {
    public:
        LocationContentObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ CLocationManagerService* host);

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface* pObject,
            /* [in] */ InterfaceID* pIID);

        CARAPI OnChange(
            /* [in] */ Boolean selfUpdate);

    private:
        CLocationManagerService* mHost;
    };

    /**
     * A wrapper class holding either an ILocationListener or a PendingIntent to receive
     * location updates.
     */
    class Receiver
        : public ElRefBase
        , public IProxyDeathRecipient
        , public IPendingIntentOnFinished
    {
    public:
        Receiver(
            /* [in] */ IILocationListener* listener,
            /* [in] */ IPendingIntent* intent,
            /* [in] */ Int32 pid,
            /* [in] */ Int32 uid,
            /* [in] */ const String& packageName,
            /* [in] */ CLocationManagerService* host);

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI_(String) ToString();

        CARAPI_(Boolean) IsListener();

        CARAPI_(Boolean) IsPendingIntent();

        CARAPI GetListener(
            /* [out] */ IILocationListener** l);

        CARAPI GetPendingIntent(
            /* [out] */ IPendingIntent** pendingIntent);

        CARAPI_(Boolean) CallStatusChangedLocked(
            /* [in] */ const String& provider,
            /* [in] */ Int32 status,
            /* [in] */ IBundle* extras);

        CARAPI_(Boolean) CallLocationChangedLocked(
            /* [in] */ ILocation* location);

        CARAPI_(Boolean) CallProviderEnabledLocked(
            /* [in] */ const String& provider,
            /* [in] */ Boolean enabled);

        CARAPI ProxyDied();

        CARAPI OnSendFinished(
            /* [in] */ IPendingIntent* pendingIntent,
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 resultCode,
            /* [in] */ const String& resultData,
            /* [in] */ IBundle* resultExtras);

        CARAPI_(void) IncrementPendingBroadcastsLocked();

        CARAPI_(void) DecrementPendingBroadcastsLocked();

    private:
        CLocationManagerService* mHost;
        Int32 mUid;  // uid of receiver
        Int32 mPid;  // pid of receiver
        String mPackageName;  // package name of receiver
        Int32 mAllowedResolutionLevel;  // resolution level allowed to receiver
        AutoPtr<IILocationListener> mListener;
        AutoPtr<IPendingIntent> mPendingIntent;
        AutoPtr<IInterface> mKey;
        HashMap<String, AutoPtr<UpdateRecord> > mUpdateRecords;
        Int32 mPendingBroadcasts;
        Object mLock;
    friend class CLocationManagerService;
    };

    class MyBroadcastReceiver : public BroadcastReceiver
    {
    public:

        MyBroadcastReceiver(
            /* [in] */ CLocationManagerService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CLocationManagerService::MyBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CLocationManagerService* mHost;
    };

    class UpdateRecord : public ElRefBase
    {
    public:
         /**
         * Note: must be constructed with lock held.
         */
        UpdateRecord(
            /* [in] */ const String& provider,
            /* [in] */ ILocationRequest* request,
            /* [in] */ Receiver* receiver,
            /* [in] */ CLocationManagerService* host);

        /**
         * Method to be called when a record will no longer be used.  Calling this multiple times
         * must have the same effect as calling it once.
         */
        CARAPI_(void) DisposeLocked(
            /* [in] */ Boolean removeReceiver);

        CARAPI_(String) ToString();

    public:

        String mProvider;
        AutoPtr<ILocationRequest> mRequest;
        AutoPtr<Receiver> mReceiver;
        AutoPtr<ILocation> mLastFixBroadcast;
        Int64 mLastStatusBroadcast;
    private:
        CLocationManagerService* mHost;
    };

public:
    CLocationManagerService();

    ~CLocationManagerService();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI SystemReady();

    CARAPI Run();

    CARAPI LocationCallbackFinished(
        /* [in] */ IILocationListener* listener);

    CARAPI GetAllProviders(
        /* [out] */ IObjectContainer** allProviders);

    CARAPI GetProviders(
        /* [in] */ ICriteria* criteria,
        /* [in] */ Boolean enabledOnly,
        /* [out] */ IObjectContainer** providers);

    CARAPI GetBestProvider(
        /* [in] */ ICriteria* criteria,
        /* [in] */ Boolean enabledOnly,
        /* [out] */ String* provider);

    CARAPI ProviderMeetsCriteria(
        /* [in] */ const String& provider,
        /* [in] */ ICriteria* criteria,
        /* [out] */ Boolean* result);

    CARAPI RequestLocationUpdates(
        /* [in] */ ILocationRequest* request,
        /* [in] */ IILocationListener* listener,
        /* [in] */ IPendingIntent* intent,
        /* [in] */ const String& packageName);

    CARAPI RemoveUpdates(
        /* [in] */ IILocationListener* listener,
        /* [in] */ IPendingIntent* intent,
        /* [in] */ const String& packageName);

    CARAPI GetLastLocation(
        /* [in] */ ILocationRequest* request,
        /* [in] */ const String& packageName,
        /* [out] */ ILocation** location);

    CARAPI RequestGeofence(
        /* [in] */ ILocationRequest* request,
        /* [in] */ IGeofence* geofence,
        /* [in] */ IPendingIntent* intent,
        /* [in] */ const String& packageName);

    CARAPI RemoveGeofence(
        /* [in] */ IGeofence* geofence,
        /* [in] */ IPendingIntent* intent,
        /* [in] */ const String& packageName);

    CARAPI AddGpsStatusListener(
        /* [in] */ IIGpsStatusListener* listener,
        /* [out] */ Boolean* result);

    CARAPI RemoveGpsStatusListener(
        /* [in] */ IIGpsStatusListener* listener);

    CARAPI SendExtraCommand(
        /* [in] */ const String& provider,
        /* [in] */ const String& command,
        /* [out] */ IBundle** outExtras,
        /* [out] */ Boolean* result);

    // for NI support
    CARAPI SendNiResponse(
        /* [in] */ Int32 notifId,
        /* [in] */ Int32 userResponse,
        /* [out] */ Boolean* result);

    /**
     * @return null if the provider does not exist
     * @throws SecurityException if the provider is not allowed to be
     * accessed by the caller
     */
    CARAPI GetProviderProperties(
        /* [in] */ const String& provider,
        /* [out] */ IProviderProperties** properties);

    CARAPI IsProviderEnabled(
        /* [in] */ const String& provider,
        /* [out] */ Boolean* isEnabled);

    CARAPI ReportLocation(
        /* [in] */ ILocation* location,
        /* [in] */ Boolean passive);

    CARAPI GeocoderIsPresent(
        /* [out] */ Boolean* result);

    CARAPI GetFromLocation(
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Int32 maxResults,
        /* [in] */ IGeocoderParams* params,
        /* [out] */ IObjectContainer** addrs,
        /* [out] */ String* result);

    CARAPI GetFromLocationName(
        /* [in] */ const String& locationName,
        /* [in] */ Double lowerLeftLatitude,
        /* [in] */ Double lowerLeftLongitude,
        /* [in] */ Double upperRightLatitude,
        /* [in] */ Double upperRightLongitude,
        /* [in] */ Int32 maxResults,
        /* [in] */ IGeocoderParams* params,
        /* [out] */ IObjectContainer** addrs,
        /* [out] */ String* result);

    CARAPI AddTestProvider(
        /* [in] */ const String& name,
        /* [in] */ IProviderProperties* properties);

    CARAPI RemoveTestProvider(
        /* [in] */ const String& provider);

    CARAPI SetTestProviderLocation(
        /* [in] */ const String& provider,
        /* [in] */ ILocation* loc);

    CARAPI ClearTestProviderLocation(
        /* [in] */ const String& provider);

    CARAPI SetTestProviderEnabled(
        /* [in] */ const String& provider,
        /* [in] */ Boolean enabled);

    CARAPI ClearTestProviderEnabled(
        /* [in] */ const String& provider);


    CARAPI SetTestProviderStatus(
        /* [in] */ const String& provider,
        /* [in] */ Int32 status,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 updateTime);

    CARAPI ClearTestProviderStatus(
        /* [in] */ const String& provider);

private:
    CARAPI_(void) Init();

    CARAPI EnsureFallbackFusedProviderPresentLocked(
        /* [in] */ List<String>* pkgs);

    CARAPI_(void) LoadProvidersLocked();

    /**
     * Called when the device's active user changes.
     * @param userId the new active user's UserId
     */
    CARAPI_(void) SwitchUser(
        /* [in] */ Int32 userId);

    CARAPI_(void) AddProviderLocked(
        /* [in] */ ILocationProviderInterface* provider);

    CARAPI_(void) RemoveProviderLocked(
        /* [in] */ ILocationProviderInterface* provider);

    CARAPI_(Boolean) IsAllowedBySettingsLocked(
        /* [in] */ const String& provider,
        /* [in] */ Int32 userId);

    /**
     * Returns the permission string associated with the specified resolution level.
     *
     * @param resolutionLevel the resolution level
     * @return the permission string
     */
    CARAPI_(String) GetResolutionPermission(
        /* [in] */ Int32 resolutionLevel);

    /**
     * Returns the resolution level allowed to the given PID/UID pair.
     *
     * @param pid the PID
     * @param uid the UID
     * @return resolution level allowed to the pid/uid pair
     */
    CARAPI_(Int32) GetAllowedResolutionLevel(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid);

    /**
     * Returns the resolution level allowed to the caller
     *
     * @return resolution level allowed to caller
     */
    CARAPI_(Int32) GetCallerAllowedResolutionLevel();

    /**
     * Throw SecurityException if specified resolution level is insufficient to use geofences.
     *
     * @param allowedResolutionLevel resolution level allowed to caller
     */
    CARAPI CheckResolutionLevelIsSufficientForGeofenceUse(
        /* [in] */ Int32 allowedResolutionLevel);

    /**
     * Return the minimum resolution level required to use the specified location provider.
     *
     * @param provider the name of the location provider
     * @return minimum resolution level required for provider
     */
    CARAPI_(Int32) GetMinimumResolutionLevelForProviderUse(
        /* [in] */ const String& provider);

    /**
     * Throw SecurityException if specified resolution level is insufficient to use the named
     * location provider.
     *
     * @param allowedResolutionLevel resolution level allowed to caller
     * @param providerName the name of the location provider
     */
    CARAPI CheckResolutionLevelIsSufficientForProviderUse(
        /* [in] */ Int32 allowedResolutionLevel,
        /* [in] */ const String& providerName);

    CARAPI_(String) PickBest(
        /* [in] */ IObjectContainer* providers);

    CARAPI_(void) UpdateProvidersLocked();

    CARAPI_(void) UpdateProviderListenersLocked(
        /* [in] */ const String& provider,
        /* [in] */ Boolean enabled,
        /* [in] */ Int32 userId);

    CARAPI_(void) ApplyRequirementsLocked(
        /* [in] */ const String& provider);

    CARAPI_(AutoPtr<Receiver>) GetReceiver(
        /* [in] */ IILocationListener* listener,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

    CARAPI_(AutoPtr<Receiver>) GetReceiver(
        /* [in] */ IPendingIntent* intent,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

    /**
     * Creates a LocationRequest based upon the supplied LocationRequest that to meets resolution
     * and consistency requirements.
     *
     * @param request the LocationRequest from which to create a sanitized version
     * @param shouldBeCoarse whether the sanitized version should be held to coarse resolution
     * constraints
     * @param fastestCoarseIntervalMS minimum interval allowed for coarse resolution
     * @return a version of request that meets the given resolution and consistency requirements
     * @hide
     */
    CARAPI_(AutoPtr<ILocationRequest>) CreateSanitizedRequest(
        /* [in] */ ILocationRequest* request,
        /* [in] */ Int32 resolutionLevel);

    CARAPI CheckPackageName(
        /* [in] */ const String& packageName);

    CARAPI CheckPendingIntent(
        /* [in] */ IPendingIntent* intent);

    CARAPI CheckListenerOrIntent(
        /* [in] */ IILocationListener* listener,
        /* [in] */ IPendingIntent* intent,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [out] */ Receiver** receiver);

    CARAPI RequestLocationUpdatesLocked(
        /* [in] */ ILocationRequest* request,
        /* [in] */ Receiver* receiver,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

    CARAPI_(void) RemoveUpdatesLocked(
        /* [in] */ Receiver* receiver);

    CARAPI CheckCallerIsProvider();

    CARAPI_(Boolean) DoesPackageHaveUid(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

    static CARAPI_(Boolean) ShouldBroadcastSafe(
        /* [in] */ ILocation* loc,
        /* [in] */ ILocation* lastLoc,
        /* [in] */ UpdateRecord* record,
        /* [in] */ Int64 now);

    CARAPI_(void) HandleLocationChangedLocked(
        /* [in] */ ILocation* location,
        /* [in] */ Boolean passive);

    CARAPI_(void) HandleLocationChanged(
        /* [in] */ ILocation* location,
        /* [in] */ Boolean passive);

    CARAPI_(void) IncrementPendingBroadcasts();

    CARAPI_(void) DecrementPendingBroadcasts();

    // Mock Providers
    CARAPI CheckMockPermissionsSafe();

public:
    static const Boolean D;

private:
    static const String TAG;

    static const String WAKELOCK_KEY;
    static const String THREAD_NAME;

    // Location resolution level: no location data whatsoever
    static const Int32 RESOLUTION_LEVEL_NONE;
    // Location resolution level: coarse location data only
    static const Int32 RESOLUTION_LEVEL_COARSE;
    // Location resolution level: fine location data
    static const Int32 RESOLUTION_LEVEL_FINE;

    static const String ACCESS_MOCK_LOCATION;
    static const String ACCESS_LOCATION_EXTRA_COMMANDS;
    static const String INSTALL_LOCATION_PROVIDER;

    static const String NETWORK_LOCATION_SERVICE_ACTION;
    static const String FUSED_LOCATION_SERVICE_ACTION;

    static const Int32 MSG_LOCATION_CHANGED;

    // Location Providers may sometimes deliver location updates
    // slightly faster that requested - provide grace period so
    // we don't unnecessarily filter events that are otherwise on
    // time
    static const Int32 MAX_PROVIDER_SCHEDULING_JITTER_MS;

    static const AutoPtr<ILocationRequest> DEFAULT_LOCATION_REQUEST;

private:

    AutoPtr<IContext> mContext;
    // used internally for synchronization
    Object mLock;
    Object mLockForWakeLock;

    // --- fields below are final after init() ---
    AutoPtr<LocationFudger> mLocationFudger;
    AutoPtr<GeofenceManager> mGeofenceManager;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    AutoPtr<IPackageManager> mPackageManager;
    AutoPtr<GeocoderProxy> mGeocodeProvider;
    AutoPtr<IIGpsStatusProvider> mGpsStatusProvider;
    AutoPtr<IINetInitiatedListener> mNetInitiatedListener;
    AutoPtr<IHandler> mLocationHandler;
    AutoPtr<PassiveProvider> mPassiveProvider;  // track passive provider for special cases
    AutoPtr<LocationBlacklist> mBlacklist;

    // --- fields below are protected by mWakeLock ---
    Int32 mPendingBroadcasts;

    // --- fields below are protected by mLock ---
    // Set of providers that are explicitly enabled
    HashSet<String> mEnabledProviders;

    // Set of providers that are explicitly disabled
    HashSet<String> mDisabledProviders;

    // Mock (test) providers
    HashMap<String, AutoPtr<MockProvider> > mMockProviders;

    // all receivers
    InterfaceReceiverMap mReceivers;

    // currently installed providers (with mocks replacing real providers)
    List<AutoPtr<ILocationProviderInterface> > mProviders;

    // real providers, saved here when mocked out
    HashMap<String, AutoPtr<ILocationProviderInterface> > mRealProviders;

    // mapping from provider name to provider
    HashMap<String, AutoPtr<ILocationProviderInterface> > mProvidersByName;

    // mapping from provider name to all its UpdateRecords
    typedef List<AutoPtr<UpdateRecord> > UpdateRecordList;
    typedef typename UpdateRecordList::Iterator UpdateRecordListIterator;
    HashMap<String, AutoPtr< UpdateRecordList > > mRecordsByProvider;

    // mapping from provider name to last known location
    HashMap<String, AutoPtr<ILocation> > mLastLocation;

    // all providers that operate over proxy, for authorizing incoming location
    List<AutoPtr<LocationProviderProxy> > mProxyProviders;

    // current active user on the device - other users are denied location data
    Int32 mCurrentUserId;

    AutoPtr<IPackageMonitor> mPackageMonitor;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_CLOCATIONMANAGERSERVICE_H__
