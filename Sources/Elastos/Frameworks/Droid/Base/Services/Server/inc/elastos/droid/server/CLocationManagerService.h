
#ifndef __ELASTOS_DROID_SERVER_CLOCATIONMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CLOCATIONMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CLocationManagerService.h"
#include "elastos/droid/server/location/LocationFudger.h"
#include "elastos/droid/server/location/GeofenceManager.h"
#include "elastos/droid/server/location/PassiveProvider.h"
#include "elastos/droid/server/location/GeocoderProxy.h"
#include "elastos/droid/server/location/LocationBlacklist.h"
#include "elastos/droid/server/location/MockProvider.h"
#include "elastos/droid/server/location/LocationProviderProxy.h"
#include "elastos/droid/server/location/LocationFudger.h"
#include "elastos/droid/server/location/GeoFencerBase.h"
#include "elastos/droid/server/location/GpsMeasurementsProvider.h"
#include "elastos/droid/server/location/LocationRequestStatistics.h"
#include "elastos/droid/server/location/GpsNavigationMessageProvider.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IAppOpsManagerOnOpChangedInternalListener;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::EIID_IContentObserver;
using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Location::ICriteria;
using Elastos::Droid::Location::IGpsStatusListener;
using Elastos::Droid::Location::IIGpsStatusProvider;
using Elastos::Droid::Location::IINetInitiatedListener;
using Elastos::Droid::Location::IILocationListener;
using Elastos::Droid::Location::IIGpsStatusListener;
using Elastos::Droid::Location::IIGpsMeasurementsListener;
using Elastos::Droid::Location::IIGpsNavigationMessageListener;
using Elastos::Droid::Server::Location::LocationFudger;
using Elastos::Droid::Server::Location::GeofenceManager;
using Elastos::Droid::Server::Location::GeoFencerBase;
using Elastos::Droid::Server::Location::GeocoderProxy;
using Elastos::Droid::Server::Location::PassiveProvider;
using Elastos::Droid::Server::Location::LocationBlacklist;
using Elastos::Droid::Server::Location::GpsMeasurementsProvider;
using Elastos::Droid::Server::Location::LocationRequestStatistics;
using Elastos::Droid::Server::Location::GpsNavigationMessageProvider;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IUserManager;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CLocationManagerService)
    , public Object
    , public IILocationManager
{
    class Receiver;
    class UpdateRecord;

private:
    class AppOpsManagerOnOpChangedInternalListener
        : public Object
        , public IAppOpsManagerOnOpChangedInternalListener
    {
    public:
        CAR_INTERFACE_DECL()

        AppOpsManagerOnOpChangedInternalListener(
            /* [in] */ CLocationManagerService* host);

        CARAPI OnOpChanged(
            /* [in] */ const String& op,
            /* [in] */ const String& packageName);

        CARAPI OnOpChanged(
            /* [in] */ Int32 op,
            /* [in] */ const String& packageName);

    public:
        CLocationManagerService* mHost;
    };

    class MyContentObserver
        : public ContentObserver
    {
    public:
        MyContentObserver(
            /* [in] */ CLocationManagerService* host);

        CARAPI constructor(
            /* [in] */ IHandler* handler);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    public:
        CLocationManagerService* mHost;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
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

    public:
        CLocationManagerService* mHost;
    };

    class LocationWorkerHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CLocationManagerService::LocationWorkerHandler")

        LocationWorkerHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CLocationManagerService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CLocationManagerService* mHost;
    };

    class LocationPackageMonitor
        : public PackageMonitor
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

    /**
     * A wrapper class holding either an ILocationListener or a PendingIntent to receive
     * location updates.
     */
    class Receiver
        : public Object
        , public IProxyDeathRecipient
        , public IPendingIntentOnFinished
    {
        friend class CLocationManagerService;
    public:
        CAR_INTERFACE_DECL()

        Receiver(
            /* [in] */ IILocationListener* listener,
            /* [in] */ IPendingIntent* intent,
            /* [in] */ Int32 pid,
            /* [in] */ Int32 uid,
            /* [in] */ const String& packageName,
            /* [in] */ IWorkSource* workSource,
            /* [in] */ Boolean hideFromAppOps,
            /* [in] */ CLocationManagerService* host);

        CARAPI ToString(
            /* [out] */ String* result);

        /**
         * Update AppOp monitoring for this receiver.
         *
         * @param allow If true receiver is currently active, if false it's been removed.
         */
        void UpdateMonitoring(
            /* [in] */ Boolean allow);

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

        CARAPI_(void) ClearPendingBroadcastsLocked();

    private:
        /**
         * Update AppOps monitoring for a single location request and op type.
         *
         * @param allowMonitoring True if monitoring is allowed for this request/op.
         * @param currentlyMonitoring True if AppOps is currently monitoring this request/op.
         * @param op AppOps code for the op to update.
         * @return True if monitoring is on for this request/op after updating.
         */
        Boolean UpdateMonitoring(
            /* [in] */ Boolean allowMonitoring,
            /* [in] */ Boolean currentlyMonitoring,
            /* [in] */ Int32 op);

    private:
        CLocationManagerService* mHost;
        Int32 mUid;  // uid of receiver
        Int32 mPid;  // pid of receiver
        String mPackageName;  // package name of receiver
        Int32 mAllowedResolutionLevel;  // resolution level allowed to receiver

        AutoPtr<IILocationListener> mListener;
        AutoPtr<IPendingIntent> mPendingIntent;
        AutoPtr<IWorkSource> mWorkSource; // WorkSource for battery blame, or null to assign to caller.
        Boolean mHideFromAppOps; // True if AppOps should not monitor this receiver.
        AutoPtr<IInterface> mKey;

        AutoPtr<IHashMap> mUpdateRecords;

        // True if app ops has started monitoring this receiver for locations.
        Boolean mOpMonitoring;
        // True if app ops has started monitoring this receiver for high power (gps) locations.
        Boolean mOpHighPowerMonitoring;
        Int32 mPendingBroadcasts;
        AutoPtr<IPowerManagerWakeLock> mWakeLock;

        Object mLock;
    };

    class UpdateRecord
        : public Object
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
         * Method to be called when a record will no longer be used.
         */
        CARAPI_(void) DisposeLocked(
            /* [in] */ Boolean removeReceiver);

        CARAPI ToString(
            /* [out] */ String* result);

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
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CLocationManagerService();

    ~CLocationManagerService();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI SystemRunning();

    CARAPI LocationCallbackFinished(
        /* [in] */ IILocationListener* listener);

    static CARAPI ResolutionLevelToOp(
        /* [in] */ Int32 allowedResolutionLevel,
        /* [out] */ Int32* result);

    CARAPI GetAllProviders(
        /* [out] */ IList** allProviders);

    CARAPI GetProviders(
        /* [in] */ ICriteria* criteria,
        /* [in] */ Boolean enabledOnly,
        /* [out] */ IList** providers);

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
        /* [out] */ ILocation** result);

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
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI RemoveGpsStatusListener(
        /* [in] */ IIGpsStatusListener* listener);

    CARAPI AddGpsMeasurementsListener(
        /* [in] */ IIGpsMeasurementsListener* listener,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI RemoveGpsMeasurementsListener(
        /* [in] */ IIGpsMeasurementsListener* listener,
        /* [out] */ Boolean* result);

    CARAPI AddGpsNavigationMessageListener(
        /* [in] */ IIGpsNavigationMessageListener* listener,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI RemoveGpsNavigationMessageListener(
        /* [in] */ IIGpsNavigationMessageListener* listener,
        /* [out] */ Boolean* result);

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
        /* [out] */ IList** addrs,
        /* [out] */ String* result);

    CARAPI GetFromLocationName(
        /* [in] */ const String& locationName,
        /* [in] */ Double lowerLeftLatitude,
        /* [in] */ Double lowerLeftLongitude,
        /* [in] */ Double upperRightLatitude,
        /* [in] */ Double upperRightLongitude,
        /* [in] */ Int32 maxResults,
        /* [in] */ IGeocoderParams* params,
        /* [out] */ IList** addrs,
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

    CARAPI_(Boolean) ReportLocationAccessNoThrow(
        /* [in] */ Int32 uid,
        /* [in] */ String packageName,
        /* [in] */ Int32 allowedResolutionLevel);

    CARAPI_(Boolean) CheckLocationAccess(
        /* [in] */ Int32 uid,
        /* [in] */ String packageName,
        /* [in] */ Int32 allowedResolutionLevel);

    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);


    /**
     * Makes a list of userids that are related to the current user. This is
     * relevant when using managed profiles. Otherwise the list only contains
     * the current user.
     *
     * @param currentUserId the current user, who might have an alter-ego.
     */
    void UpdateUserProfiles(
        /* [in] */ Int32 currentUserId);

private:
    CARAPI EnsureFallbackFusedProviderPresentLocked(
        /* [in] */ IArrayList* pkgs);

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

    /**
     * Returns "true" if access to the specified location provider is allowed by the current
     * user's settings. Access to all location providers is forbidden to non-location-provider
     * processes belonging to background users.
     *
     * @param provider the name of the location provider
     * @return
     */
    CARAPI_(Boolean) IsAllowedByCurrentUserSettingsLocked(
        /* [in] */ const String& provider);

    /**
     * Returns "true" if access to the specified location provider is allowed by the specified
     * user's settings. Access to all location providers is forbidden to non-location-provider
     * processes belonging to background users.
     *
     * @param provider the name of the location provider
     * @param uid the requestor's UID
     * @return
     */
    CARAPI_(Boolean) IsAllowedByUserSettingsLocked(
        /* [in] */ String provider,
        /* [in] */ Int32 uid);

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

    /**
     * Throw SecurityException if WorkSource use is not allowed (i.e. can't blame other packages
     * for battery).
     */
    CARAPI_(void) CheckDeviceStatsAllowed();

    CARAPI_(void) CheckUpdateAppOpsAllowed();

    CARAPI_(String) PickBest(
        /* [in] */ IList* providers);

    CARAPI_(void) UpdateProvidersLocked();

    CARAPI_(void) UpdateProviderListenersLocked(
        /* [in] */ const String& provider,
        /* [in] */ Boolean enabled);

    CARAPI_(void) ApplyRequirementsLocked(
        /* [in] */ const String& provider);

    CARAPI_(AutoPtr<Receiver>) GetReceiverLocked(
        /* [in] */ IILocationListener* listener,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ IWorkSource* workSource,
        /* [in] */ Boolean hideFromAppOps);

    CARAPI_(AutoPtr<Receiver>) GetReceiverLocked(
        /* [in] */ IPendingIntent* intent,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ IWorkSource* workSource,
        /* [in] */ Boolean hideFromAppOps);

    /**
     * Creates a LocationRequest based upon the supplied LocationRequest that to meets resolution
     * and consistency requirements.
     *
     * @param request the LocationRequest from which to create a sanitized version
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

    CARAPI CheckListenerOrIntentLocked(
        /* [in] */ IILocationListener* listener,
        /* [in] */ IPendingIntent* intent,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ IWorkSource* workSource,
        /* [in] */ Boolean hideFromAppOps,
        /* [out] */ Receiver** receiver);

    CARAPI RequestLocationUpdatesLocked(
        /* [in] */ ILocationRequest* request,
        /* [in] */ Receiver* receiver,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

    CARAPI_(void) RemoveUpdatesLocked(
        /* [in] */ Receiver* receiver);

    CARAPI_(void) ApplyAllProviderRequirementsLocked();

    /**
     * Returns "true" if the UID belongs to a bound location provider.
     *
     * @param uid the uid
     * @return true if uid belongs to a bound location provider
     */
    CARAPI_(Boolean) IsUidALocationProvider(
        /* [in] */ Int32 uid);

    CARAPI CheckCallerIsProvider();

    /**
     * Returns true if the given package belongs to the given uid.
     */
    CARAPI_(Boolean) DoesUidHavePackage(
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

    CARAPI_(Boolean) IsMockProvider(
        /* [in] */ String provider);

    CARAPI_(AutoPtr<ILocation>) ScreenLocationLocked(
        /* [in] */ ILocation* location,
        /* [in] */ String provider);

    CARAPI_(void) HandleLocationChanged(
        /* [in] */ ILocation* location,
        /* [in] */ Boolean passive);

    // Mock Providers
    CARAPI CheckMockPermissionsSafe();

    CARAPI_(void) AddTestProviderLocked(
        /* [in] */ String name,
        /* [in] */ IProviderProperties* properties);

    /**
     * Checks if the specified userId matches any of the current foreground
     * users stored in mCurrentUserProfiles.
     */
    CARAPI_(Boolean) IsCurrentProfile(
        /* [in] */ Int32 userId);

public:
    static const Boolean D;

private:
    static const String TAG;

    static const String WAKELOCK_KEY;

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

    static const Int64 NANOS_PER_MILLI;

    // The maximum interval a location request can have and still be considered "high power".
    static const Int64 HIGH_POWER_INTERVAL_MS;

    // Location Providers may sometimes deliver location updates
    // slightly faster that requested - provide grace period so
    // we don't unnecessarily filter events that are otherwise on
    // time
    static const Int32 MAX_PROVIDER_SCHEDULING_JITTER_MS;

    static const AutoPtr<ILocationRequest> DEFAULT_LOCATION_REQUEST;

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IAppOpsManager> mAppOps;

    // used internally for synchronization
    Object mLock;
    Object mLockForWakeLock;

    // --- fields below are final after systemReady() ---
    AutoPtr<LocationFudger> mLocationFudger;
    AutoPtr<GeofenceManager> mGeofenceManager;
    AutoPtr<IPackageManager> mPackageManager;
    String mGeoFencerPackageName;
    String mComboNlpPackageName;
    String mComboNlpReadyMarker;
    String mComboNlpScreenMarker;
    AutoPtr<GeoFencerBase> mGeoFencer;
    Boolean mGeoFencerEnabled;
    AutoPtr<IPowerManager> mPowerManager;
    AutoPtr<IUserManager> mUserManager;
    AutoPtr<GeocoderProxy> mGeocodeProvider;
    AutoPtr<IIGpsStatusProvider> mGpsStatusProvider;
    AutoPtr<IINetInitiatedListener> mNetInitiatedListener;
    AutoPtr<IHandler> mLocationHandler;
    AutoPtr<PassiveProvider> mPassiveProvider;  // track passive provider for special cases
    AutoPtr<LocationBlacklist> mBlacklist;
    AutoPtr<GpsMeasurementsProvider> mGpsMeasurementsProvider;
    AutoPtr<GpsNavigationMessageProvider> mGpsNavigationMessageProvider;

    // --- fields below are protected by mLock ---
    // Set of providers that are explicitly enabled
    AutoPtr<ISet> mEnabledProviders;

    // Set of providers that are explicitly disabled
    AutoPtr<ISet> mDisabledProviders;

    // Mock (test) providers
    AutoPtr<IHashMap> mMockProviders;

    // all receivers
    AutoPtr<IHashMap> mReceivers;

    // currently installed providers (with mocks replacing real providers)
    AutoPtr<IArrayList> mProviders;

    // real providers, saved here when mocked out
    AutoPtr<IHashMap> mRealProviders;

    // mapping from provider name to provider
    AutoPtr<IHashMap> mProvidersByName;

    // mapping from provider name to all its UpdateRecords
    AutoPtr<IHashMap> mRecordsByProvider;

    AutoPtr<LocationRequestStatistics> mRequestStatistics;

    // mapping from provider name to last known location
    AutoPtr<IHashMap> mLastLocation;

    // same as mLastLocation, but is not updated faster than LocationFudger.FASTEST_INTERVAL_MS.
    // locations stored here are not fudged for coarse permissions.
    AutoPtr<IHashMap> mLastLocationCoarseInterval;

    // all providers that operate over proxy, for authorizing incoming location
    AutoPtr<IArrayList> mProxyProviders;

    // current active user on the device - other users are denied location data
    Int32 mCurrentUserId;
    AutoPtr<ArrayOf<Int32> > mCurrentUserProfiles;

    // AutoPtr<IPackageMonitor> mPackageMonitor;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CLOCATIONMANAGERSERVICE_H__
