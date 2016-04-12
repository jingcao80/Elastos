
#ifndef __ELASTOS_DROID_SERVER_LOCATION_GPSLOCATIONPROVIDER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GPSLOCATIONPROVIDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/server/location/GpsMeasurementsProvider.h"
#include "elastos/droid/server/location/GpsNavigationMessageProvider.h"
#include "elastos/droid/server/location/GpsStatusListenerHelper.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Hardware::Location::IGeofenceHardwareImpl;
using Elastos::Droid::Internal::App::IIAppOpsService;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Internal::Location::IGpsNetInitiatedHandler;
using Elastos::Droid::Internal::Location::IProviderProperties;
using Elastos::Droid::Internal::Location::IProviderRequest;
using Elastos::Droid::Location::IGpsMeasurementsEvent;
using Elastos::Droid::Location::IGpsNavigationMessageEvent;
using Elastos::Droid::Location::IIGpsGeofenceHardware;
using Elastos::Droid::Location::IIGpsStatusListener;
using Elastos::Droid::Location::IIGpsStatusProvider;
using Elastos::Droid::Location::IILocationManager;
using Elastos::Droid::Location::IINetInitiatedListener;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationListener;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Server::Location::GpsMeasurementsProvider;
using Elastos::Droid::Server::Location::GpsNavigationMessageProvider;
using Elastos::Droid::Server::Location::GpsStatusListenerHelper;
using Elastos::Droid::Utility::INtpTrustedTime;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Net::IInetAddress;
using Elastos::Utility::IProperties;



namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * A GPS implementation of LocationProvider used by LocationManager.
 *
 * {@hide}
 */
class GpsLocationProvider
    : public Object
    , public ILocationProviderInterface
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ GpsLocationProvider* host);

        CARAPI Run();

    private:
        GpsLocationProvider* mHost;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        MyRunnable2(
            /* [in] */ GpsLocationProvider* host);

        CARAPI Run();

    private:
        GpsLocationProvider* mHost;
    };

    class MyRunnable3
        : public Runnable
    {
    public:
        MyRunnable3(
            /* [in] */ GpsLocationProvider* host);

        CARAPI Run();

    private:
        GpsLocationProvider* mHost;
    };

    /** simpler wrapper for ProviderRequest + Worksource */
    class GpsRequest
        : public Object
    {
    public:
        GpsRequest(
            /* [in] */ IProviderRequest* request,
            /* [in] */ IWorkSource* source);

    public:
        AutoPtr<IProviderRequest> mRequest;
        AutoPtr<IWorkSource> mSource;

    };

    class MyListenerHelper
        : public GpsStatusListenerHelper
    {
    protected:
        // @Override
        CARAPI IsSupported(
            /* [out] */ Boolean* isSupported);

        // @Override
        CARAPI RegisterWithService(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI UnregisterFromService();
    };

public:
    class GpsStatusProviderService
        : public Object
        , public IIGpsStatusProvider
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ILocationProviderInterface* host);

        // @Override
        CARAPI AddGpsStatusListener(
            /* [in] */ IIGpsStatusListener* listener);

        // @Override
        CARAPI RemoveGpsStatusListener(
            /* [in] */ IIGpsStatusListener* listener);

        //TODO
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return Object::ToString(str);
        }

    public:
        GpsLocationProvider* mHost;
    };

private:
    class MyGpsMeasurementsProvider
        : public GpsMeasurementsProvider
    {
    public:
        MyGpsMeasurementsProvider(
            /* [in] */ GpsLocationProvider* host);

        // @Override
        CARAPI IsSupported(
            /* [out] */ Boolean* isSupported);

        // @Override
        CARAPI RegisterWithService(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI UnregisterFromService();

    private:
        GpsLocationProvider* mHost;
    };

    class MyGpsNavigationMessageProvider
        : public GpsNavigationMessageProvider
    {
    public:
        MyGpsNavigationMessageProvider(
            /* [in] */ GpsLocationProvider* host);

        // @Override
        CARAPI IsSupported(
            /* [out] */ Boolean* isSupported);

        // @Override
        CARAPI RegisterWithService(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI UnregisterFromService();

    private:
        GpsLocationProvider* mHost;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ GpsLocationProvider* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        GpsLocationProvider* mHost;
    };

public:
    class GpsGeofenceHardwareService
        : public Object
        , public IIGpsGeofenceHardware
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ILocationProviderInterface* host);

        CARAPI IsHardwareGeofenceSupported(
            /* [out] */ Boolean* result);

        CARAPI AddCircularHardwareGeofence(
            /* [in] */ Int32 geofenceId,
            /* [in] */ Double latitude,
            /* [in] */ Double longitude,
            /* [in] */ Double radius,
            /* [in] */ Int32 lastTransition,
            /* [in] */ Int32 monitorTransitions,
            /* [in] */ Int32 notificationResponsiveness,
            /* [in] */ Int32 unknownTimer,
            /* [out] */ Boolean* result);

        CARAPI RemoveHardwareGeofence(
            /* [in] */ Int32 geofenceId,
            /* [out] */ Boolean* result);

        CARAPI PauseHardwareGeofence(
            /* [in] */ Int32 geofenceId,
            /* [out] */ Boolean* result);

        CARAPI ResumeHardwareGeofence(
            /* [in] */ Int32 geofenceId,
            /* [in] */ Int32 monitorTransition,
            /* [out] */ Boolean* result);


        CARAPI ToString(
            /* [out] */ String* str)
        {
            return Object::ToString(str);
        }

    private:
        GpsLocationProvider* mHost;
    };

    class NetInitiatedListener
        : public Object
        , public IINetInitiatedListener
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ILocationProviderInterface* host);

        // Sends a response for an NI reqeust to HAL.
        // @Override
        CARAPI SendNiResponse(
            /* [in] */ Int32 notificationId,
            /* [in] */ Int32 userResponse,
            /* [out] */ Boolean* result);

        CARAPI ToString(
            /* [out] */ String* s);

    private:
        GpsLocationProvider* mHost;
    };

private:
    class ProviderHandler
        : public Handler
    {
    public:
        ProviderHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ GpsLocationProvider* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        GpsLocationProvider* mHost;
    };

    class DefaultApnObserver : public ContentObserver
    {
    public:
        DefaultApnObserver(
            /* [in] */ GpsLocationProvider* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        GpsLocationProvider* mHost;
    };

    class NetworkLocationListener
        : public Object
        , public ILocationListener
    {
    public:
        CAR_INTERFACE_DECL()

        NetworkLocationListener(
            /* [in] */ GpsLocationProvider* host);

        // @Override
        CARAPI OnLocationChanged(
            /* [in] */ ILocation* location);

        // @Override
        CARAPI OnStatusChanged(
            /* [in] */ const String& provider,
            /* [in] */ Int32 status,
            /* [in] */ IBundle* extras);
        // @Override
        CARAPI OnProviderEnabled(
            /* [in] */ const String& provider);
        // @Override
        CARAPI OnProviderDisabled(
            /* [in] */ const String& provider);

    private:
        GpsLocationProvider* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    GpsLocationProvider();

    CARAPI GetGpsStatusProvider(
        /* [out] */ IIGpsStatusProvider** gsp);

    CARAPI GetGpsGeofenceProxy(
        /* [out] */ IIGpsGeofenceHardware** ggh);

    CARAPI GetGpsMeasurementsProvider(
        /* [out] */ GpsMeasurementsProvider** gmp);

    CARAPI GetGpsNavigationMessageProvider(
        /* [out] */ GpsNavigationMessageProvider** gnmp);

    static CARAPI_(Boolean) IsSupported();

    GpsLocationProvider(
        /* [in] */ IContext* context,
        /* [in] */ IILocationManager* ilocationManager,
        /* [in] */ ILooper* looper);

    /**
     * Returns the name of this provider.
     */
    // @Override
    CARAPI GetName(
        /* [out] */ String* name);

    // @Override
    CARAPI GetProperties(
        /* [out] */ IProviderProperties** properties);

    CARAPI UpdateNetworkState(
        /* [in] */ Int32 state,
        /* [in] */ INetworkInfo* info);

    /**
     * Enables this provider.  When enabled, calls to getStatus()
     * must be handled.  Hardware may be started up
     * when the provider is enabled.
     */
    // @Override
    CARAPI Enable();

    /**
     * Disables this provider.  When disabled, calls to getStatus()
     * need not be handled.  Hardware may be shut
     * down while the provider is disabled.
     */
    // @Override
    CARAPI Disable();

    // @Override
    CARAPI IsEnabled(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetStatus(
        /* [in] */ IBundle* extras,
        /* [out] */ Int32* status);

    // @Override
    CARAPI GetStatusUpdateTime(
        /* [out] */ Int64* time);

    // @Override
    CARAPI SetRequest(
        /* [in] */ IProviderRequest* request,
        /* [in] */ IWorkSource* source);

    // @Override
    CARAPI SendExtraCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle* extras,
        /* [out] */ Boolean* result);

    CARAPI GetNetInitiatedListener(
        /* [out] */ IINetInitiatedListener** nil);

    // Called by JNI function to report an NI request.
    CARAPI ReportNiNotification(
        /* [in] */ Int32 notificationId,
        /* [in] */ Int32 niType,
        /* [in] */ Int32 notifyFlags,
        /* [in] */ Int32 timeout,
        /* [in] */ Int32 defaultResponse,
        /* [in] */ const String& requestorId,
        /* [in] */ const String& text,
        /* [in] */ Int32 requestorIdEncoding,
        /* [in] */ Int32 textEncoding,
        /* [in] */ const String& extras);  // Encoded extra data);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(void) CheckSmsSuplInit(
        /* [in] */ IIntent* intent);

    CARAPI_(void) CheckWapSuplInit(
        /* [in] */ IIntent* intent);

    CARAPI_(void) UpdateLowPowerMode();

    CARAPI_(void) ReloadGpsProperties(
        /* [in] */ IContext* context,
        /* [in] */ IProperties* properties);

    CARAPI_(void) LoadPropertiesFromResource(
        /* [in] */ IContext* context,
        /* [in] */ IProperties* properties);

    CARAPI_(Boolean) LoadPropertiesFromFile(
        /* [in] */ const String& filename,
        /* [in] */ IProperties* properties);

    CARAPI_(void) ListenForBroadcasts();

    CARAPI_(void) HandleUpdateNetworkState(
        /* [in] */ Int32 state,
        /* [in] */ INetworkInfo* info);

    CARAPI_(void) HandleInjectNtpTime();

    CARAPI_(void) HandleDownloadXtraData();

    CARAPI_(void) HandleUpdateLocation(
        /* [in] */ ILocation* location);

    CARAPI_(void) SetSuplHostPort(
        /* [in] */ const String& hostString,
        /* [in] */ const String& portString);

    /**
     * Checks what SUPL mode to use, according to the AGPS mode as well as the
     * allowed mode from properties.
     *
     * @param properties GPS properties
     * @param agpsEnabled whether AGPS is enabled by settings value
     * @param singleShot whether "singleshot" is needed
     * @return SUPL mode (MSA vs MSB vs STANDALONE)
     */
    CARAPI_(Int32) GetSuplMode(
        /* [in] */ IProperties* properties,
        /* [in] */ Boolean agpsEnabled,
        /* [in] */ Boolean singleShot);

    CARAPI_(void) HandleEnable();

    CARAPI_(void) HandleDisable();

    CARAPI_(void) UpdateStatus(
        /* [in] */ Int32 status,
        /* [in] */ Int32 svCount);

    CARAPI_(void) HandleSetRequest(
        /* [in] */ IProviderRequest* request,
        /* [in] */ IWorkSource* source);

    // Called when the requirements for GPS may have changed
    CARAPI_(void) UpdateRequirements();

    CARAPI_(void) UpdateClientUids(
        /* [in] */ IWorkSource* source);

    CARAPI_(Boolean) DeleteAidingData(
        /* [in] */ IBundle* extras);

    CARAPI_(void) StartNavigating(
        /* [in] */ Boolean singleShot);

    CARAPI_(void) StopNavigating();

    CARAPI_(void) Hibernate();

    CARAPI_(Boolean) HasCapability(
        /* [in] */ Int32 capability);


    /**
     * called from native code to update our position.
     */
    CARAPI_(void) ReportLocation(
        /* [in] */ Int32 flags,
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Double altitude,
        /* [in] */ Float speed,
        /* [in] */ Float bearing,
        /* [in] */ Float accuracy,
        /* [in] */ Int64 timestamp);

    /**
     * called from native code to update our status
     */
    CARAPI_(void) ReportStatus(
        /* [in] */ Int32 status);

    /**
     * called from native code to update SV info
     */
    CARAPI_(void) ReportSvStatus();

    /**
     * called from native code to update AGPS status
     */
    CARAPI_(void) ReportAGpsStatus(
        /* [in] */ Int32 type,
        /* [in] */ Int32 status,
        /* [in] */ ArrayOf<Byte>* ipaddr);

    /**
     * called from native code to report NMEA data received
     */
    CARAPI_(void) ReportNmea(
        /* [in] */ Int64 timestamp);

    /**
     * called from native code - Gps measurements callback
     */
    CARAPI_(void) ReportMeasurementData(
        /* [in] */ IGpsMeasurementsEvent* event);

    /**
     * called from native code - GPS navigation message callback
     */
    CARAPI_(void) ReportNavigationMessage(
        /* [in] */ IGpsNavigationMessageEvent* event);

    /**
     * called from native code to inform us what the GPS engine capabilities are
     */
    CARAPI_(void) SetEngineCapabilities(
        /* [in] */ Int32 capabilities);

    /**
     * called from native code to request XTRA data
     */
    CARAPI_(void) XtraDownloadRequest();

    /**
     * Helper method to construct a location object.
     */
    CARAPI_(AutoPtr<ILocation>) BuildLocation(
        /* [in] */ Int32 flags,
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Double altitude,
        /* [in] */ Float speed,
        /* [in] */ Float bearing,
        /* [in] */ Float accuracy,
        /* [in] */ Int64 timestamp);

    /**
     * Converts the GPS HAL status to the internal Geofence Hardware status.
     */
    CARAPI_(Int64) GetGeofenceStatus(
        /* [in] */ Int32 status);

    /**
     * Called from native to report GPS Geofence transition
     * All geofence callbacks are called on the same thread
     */
    CARAPI_(void) ReportGeofenceTransition(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 flags,
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Double altitude,
        /* [in] */ Float speed,
        /* [in] */ Float bearing,
        /* [in] */ Float accuracy,
        /* [in] */ Int64 timestamp,
        /* [in] */ Int32 transition,
        /* [in] */ Int64 transitionTimestamp);

    /**
     * called from native code to report GPS status change.
     */
    CARAPI_(void) ReportGeofenceStatus(
        /* [in] */ Int32 status,
        /* [in] */ Int32 flags,
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Double altitude,
        /* [in] */ Float speed,
        /* [in] */ Float bearing,
        /* [in] */ Float accuracy,
        /* [in] */ Int64 timestamp);

    /**
     * called from native code - Geofence Add callback
     */
    CARAPI_(void) ReportGeofenceAddStatus(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 status);

    /**
     * called from native code - Geofence Remove callback
     */
    CARAPI_(void) ReportGeofenceRemoveStatus(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 status);

    /**
     * called from native code - Geofence Pause callback
     */
    CARAPI_(void) ReportGeofencePauseStatus(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 status);

    /**
     * called from native code - Geofence Resume callback
     */
    CARAPI_(void) ReportGeofenceResumeStatus(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 status);

    /**
     * Called from native code to request set id info.
     * We should be careful about receiving null string from the TelephonyManager,
     * because sending null String to JNI function would cause a crash.
     */
    CARAPI_(void) RequestSetID(
        /* [in] */ Int32 flags);

    /**
     * Called from native code to request utc time info
     */
    CARAPI_(void) RequestUtcTime();

    /**
     * Called from native code to request reference location info
     */

    CARAPI_(void) requestRefLocation(
        /* [in] */ Int32 flags);

    CARAPI_(void) SendMessage(
        /* [in] */ Int32 message,
        /* [in] */ Int32 arg,
        /* [in] */ IInterface* obj);

    CARAPI_(String) GetDefaultApn();

    CARAPI_(Int32) GetApnIpType(
        /* [in] */ const String& apn);

    CARAPI_(Int32) TranslateToApnIpType(
        /* [in] */ const String& ipProtocol,
        /* [in] */ const String& apn);

    CARAPI_(void) SetRouting();

    // native methods
    // static CARAPI_(Boolean) Class_init_native();
    static CARAPI_(Boolean) Native_is_supported();

    CARAPI_(Boolean) Native_init();
    CARAPI_(void) Native_cleanup();
    CARAPI_(Boolean) Native_set_position_mode(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 recurrence,
        /* [in] */ Int32 min_interval,
        /* [in] */ Int32 preferred_accuracy,
        /* [in] */ Int32 preferred_time);
    CARAPI_(Boolean) Native_start();
    CARAPI_(Boolean) Native_stop();
    CARAPI_(void) Native_delete_aiding_data(
        /* [in] */ Int32 flags);
    // returns number of SVs
    // mask[0] is ephemeris mask and mask[1] is almanac mask
    CARAPI_(Int32) Native_read_sv_status(
        /* [in] */ ArrayOf<Int32>* svs,
        /* [in] */ ArrayOf<Float>* snrs,
        /* [in] */ ArrayOf<Float>* elevations,
        /* [in] */ ArrayOf<Float>* azimuths,
        /* [in] */ ArrayOf<Int32>* masks);
    CARAPI_(Int32) Native_read_nmea(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 bufferSize);
    CARAPI_(void) Native_inject_location(
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Float accuracy);

    // XTRA Support
    CARAPI_(void) Native_inject_time(
        /* [in] */ Int64 time,
        /* [in] */ Int64 timeReference,
        /* [in] */ Int32 uncertainty);
    CARAPI_(Boolean) Native_supports_xtra();
    CARAPI_(void) Native_inject_xtra_data(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 length);

    // DEBUG Support
    CARAPI_(String) Native_get_internal_state();

    // AGPS Support
    CARAPI_(void) Native_agps_data_conn_open(
        /* [in] */ const String& apn,
        /* [in] */ Int32 apnIpType);
    CARAPI_(void) Native_agps_data_conn_closed();
    CARAPI_(void) Native_agps_data_conn_failed();
    CARAPI_(void) Native_agps_ni_message(
        /* [in] */ ArrayOf<Byte>* msg,
        /* [in] */ Int32 length);
    CARAPI_(void) Native_set_agps_server(
        /* [in] */ Int32 type,
        /* [in] */ const String& hostname,
        /* [in] */ Int32 port);

    // Network-initiated (NI) Support
    CARAPI_(void) Native_send_ni_response(
        /* [in] */ Int32 notificationId,
        /* [in] */ Int32 userResponse);

    // AGPS ril suport
    CARAPI_(void) Native_agps_set_ref_location_cellid(
        /* [in] */ Int32 type,
        /* [in] */ Int32 mcc,
        /* [in] */ Int32 mnc,
        /* [in] */ Int32 lac,
        /* [in] */ Int32 cid);
    CARAPI_(void) Native_agps_set_id(
        /* [in] */ Int32 type,
        /* [in] */ const String& setid);

    CARAPI_(void) Native_update_network_state(
        /* [in] */ Boolean connected,
        /* [in] */ Int32 type,
        /* [in] */ Boolean roaming,
        /* [in] */ Boolean available,
        /* [in] */ const String& extraInfo,
        /* [in] */ const String& defaultAPN);

    // Hardware Geofence support.
    static CARAPI_(Boolean) Native_is_geofence_supported();
    static CARAPI_(Boolean) Native_add_geofence(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Double radius,
        /* [in] */ Int32 lastTransition,
        /* [in] */ Int32 monitorTransitions,
        /* [in] */ Int32 notificationResponsivenes,
        /* [in] */ Int32 unknownTimer);
    static CARAPI_(Boolean) Native_remove_geofence(
        /* [in] */ Int32 geofenceId);
    static CARAPI_(Boolean) Native_resume_geofence(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 transitions);
    static CARAPI_(Boolean) Native_pause_geofence(
        /* [in] */ Int32 geofenceId);

    // Gps Hal measurements support.
    static CARAPI_(Boolean) Native_is_measurement_supported();
    CARAPI_(Boolean) Native_start_measurement_collection();
    CARAPI_(Boolean) Native_stop_measurement_collection();

    // Gps Navigation message support.
    static CARAPI_(Boolean) Native_is_navigation_message_supported();
    CARAPI_(Boolean) Native_start_navigation_message_collection();
    CARAPI_(Boolean) Native_stop_navigation_message_collection();

    // GNSS Configuration
    static CARAPI_(void) Native_configuration_update(
        /* [in] */ const String& configData);

private:
    const static String TAG;

    const static Boolean DEBUG;
    const static Boolean VERBOSE;

    const static AutoPtr<IProviderProperties> PROPERTIES;

    // these need to match GpsPositionMode enum in gps.h
    const static Int32 GPS_POSITION_MODE_STANDALONE = 0;
    const static Int32 GPS_POSITION_MODE_MS_BASED = 1;
    const static Int32 GPS_POSITION_MODE_MS_ASSISTED = 2;

    // these need to match GpsPositionRecurrence enum in gps.h
    const static Int32 GPS_POSITION_RECURRENCE_PERIODIC = 0;
    const static Int32 GPS_POSITION_RECURRENCE_SINGLE = 1;

    // these need to match GpsStatusValue defines in gps.h
    const static Int32 GPS_STATUS_NONE = 0;
    const static Int32 GPS_STATUS_SESSION_BEGIN = 1;
    const static Int32 GPS_STATUS_SESSION_END = 2;
    const static Int32 GPS_STATUS_ENGINE_ON = 3;
    const static Int32 GPS_STATUS_ENGINE_OFF = 4;

    // these need to match GpsApgsStatusValue defines in gps.h
    /** AGPS status event values. */
    const static Int32 GPS_REQUEST_AGPS_DATA_CONN = 1;
    const static Int32 GPS_RELEASE_AGPS_DATA_CONN = 2;
    const static Int32 GPS_AGPS_DATA_CONNECTED = 3;
    const static Int32 GPS_AGPS_DATA_CONN_DONE = 4;
    const static Int32 GPS_AGPS_DATA_CONN_FAILED = 5;

    // these need to match GpsLocationFlags enum in gps.h
    const static Int32 LOCATION_INVALID = 0;
    const static Int32 LOCATION_HAS_LAT_LONG = 1;
    const static Int32 LOCATION_HAS_ALTITUDE = 2;
    const static Int32 LOCATION_HAS_SPEED = 4;
    const static Int32 LOCATION_HAS_BEARING = 8;
    const static Int32 LOCATION_HAS_ACCURACY = 16;

    // IMPORTANT - the GPS_DELETE_* symbols here must match constants in gps.h
    static const Int32 GPS_DELETE_EPHEMERIS = 0x00000001;
    static const Int32 GPS_DELETE_ALMANAC = 0x00000002;
    static const Int32 GPS_DELETE_POSITION = 0x00000004;
    static const Int32 GPS_DELETE_TIME = 0x00000008;
    static const Int32 GPS_DELETE_IONO = 0x00000010;
    static const Int32 GPS_DELETE_UTC = 0x00000020;
    static const Int32 GPS_DELETE_HEALTH = 0x00000040;
    static const Int32 GPS_DELETE_SVDIR = 0x00000080;
    static const Int32 GPS_DELETE_SVSTEER = 0x00000100;
    static const Int32 GPS_DELETE_SADATA = 0x00000200;
    static const Int32 GPS_DELETE_RTI = 0x00000400;
    static const Int32 GPS_DELETE_CELLDB_INFO = 0x00000800;
    static const Int32 GPS_DELETE_ALMANAC_CORR = 0x00001000;
    static const Int32 GPS_DELETE_FREQ_BIAS_EST = 0x00002000;
    static const Int32 GLO_DELETE_EPHEMERIS = 0x00004000;
    static const Int32 GLO_DELETE_ALMANAC = 0x00008000;
    static const Int32 GLO_DELETE_SVDIR = 0x00010000;
    static const Int32 GLO_DELETE_SVSTEER = 0x00020000;
    static const Int32 GLO_DELETE_ALMANAC_CORR = 0x00040000;
    static const Int32 GPS_DELETE_TIME_GPS = 0x00080000;
    static const Int32 GLO_DELETE_TIME = 0x00100000;
    static const Int32 BDS_DELETE_SVDIR =  0X00200000;
    static const Int32 BDS_DELETE_SVSTEER = 0X00400000;
    static const Int32 BDS_DELETE_TIME = 0X00800000;
    static const Int32 BDS_DELETE_ALMANAC_CORR = 0X01000000;
    static const Int32 BDS_DELETE_EPHEMERIS = 0X02000000;
    static const Int32 BDS_DELETE_ALMANAC = 0X04000000;
    static const Int32 GPS_DELETE_ALL = 0xFFFFFFFF;

    // The GPS_CAPABILITY_* flags must match the values in gps.h
    const static Int32 GPS_CAPABILITY_SCHEDULING = 0x0000001;
    const static Int32 GPS_CAPABILITY_MSB = 0x0000002;
    const static Int32 GPS_CAPABILITY_MSA = 0x0000004;
    const static Int32 GPS_CAPABILITY_SINGLE_SHOT = 0x0000008;
    const static Int32 GPS_CAPABILITY_ON_DEMAND_TIME = 0x0000010;

    // The AGPS SUPL mode
    const static Int32 AGPS_SUPL_MODE_MSA = 0x02;
    const static Int32 AGPS_SUPL_MODE_MSB = 0x01;

    // these need to match AGpsType enum in gps.h
    const static Int32 AGPS_TYPE_SUPL = 1;
    const static Int32 AGPS_TYPE_C2K = 2;

    // these must match the definitions in gps.h
    const static Int32 APN_INVALID = 0;
    const static Int32 APN_IPV4 = 1;
    const static Int32 APN_IPV6 = 2;
    const static Int32 APN_IPV4V6 = 3;

    // for mAGpsDataConnectionState
    const static Int32 AGPS_DATA_CONNECTION_CLOSED = 0;
    const static Int32 AGPS_DATA_CONNECTION_OPENING = 1;
    const static Int32 AGPS_DATA_CONNECTION_OPEN = 2;

    // Handler messages
    const static Int32 CHECK_LOCATION = 1;
    const static Int32 ENABLE = 2;
    const static Int32 SET_REQUEST = 3;
    const static Int32 UPDATE_NETWORK_STATE = 4;
    const static Int32 INJECT_NTP_TIME = 5;
    const static Int32 DOWNLOAD_XTRA_DATA = 6;
    const static Int32 UPDATE_LOCATION = 7;
    const static Int32 ADD_LISTENER = 8;
    const static Int32 REMOVE_LISTENER = 9;
    const static Int32 INJECT_NTP_TIME_FINISHED = 10;
    const static Int32 DOWNLOAD_XTRA_DATA_FINISHED = 11;

    // Request setid
    const static Int32 AGPS_RIL_REQUEST_SETID_IMSI = 1;
    const static Int32 AGPS_RIL_REQUEST_SETID_MSISDN = 2;

    // Request ref location
    const static Int32 AGPS_RIL_REQUEST_REFLOC_CELLID = 1;
    const static Int32 AGPS_RIL_REQUEST_REFLOC_MAC = 2;

    // ref. location info
    const static Int32 AGPS_REF_LOCATION_TYPE_GSM_CELLID = 1;
    const static Int32 AGPS_REF_LOCATION_TYPE_UMTS_CELLID = 2;
    const static Int32 AGPS_REG_LOCATION_TYPE_MAC        = 3;

    // set id info
    const static Int32 AGPS_SETID_TYPE_NONE = 0;
    const static Int32 AGPS_SETID_TYPE_IMSI = 1;
    const static Int32 AGPS_SETID_TYPE_MSISDN = 2;

    const static String PROPERTIES_FILE_PREFIX;
    const static String PROPERTIES_FILE_SUFFIX;
    const static String DEFAULT_PROPERTIES_FILE;

    const static Int32 GPS_GEOFENCE_UNAVAILABLE = 1<<0L;
    const static Int32 GPS_GEOFENCE_AVAILABLE = 1<<1L;

    // GPS Geofence errors. Should match gps.h constants.
    const static Int32 GPS_GEOFENCE_OPERATION_SUCCESS = 0;
    const static Int32 GPS_GEOFENCE_ERROR_TOO_MANY_GEOFENCES = 100;
    const static Int32 GPS_GEOFENCE_ERROR_ID_EXISTS  = -101;
    const static Int32 GPS_GEOFENCE_ERROR_ID_UNKNOWN = -102;
    const static Int32 GPS_GEOFENCE_ERROR_INVALID_TRANSITION = -103;
    const static Int32 GPS_GEOFENCE_ERROR_GENERIC = -149;

    // TCP/IP constants.
    // Valid TCP/UDP port range is (0, 65535].
    const static Int32 TCP_MIN_PORT = 0;
    const static Int32 TCP_MAX_PORT = 0xffff;

    // Value of batterySaverGpsMode such that GPS isn't affected by battery saver mode.
    const static Int32 BATTERY_SAVER_MODE_NO_CHANGE = 0;
    // Value of batterySaverGpsMode such that GPS is disabled when battery saver mode
    // is enabled and the screen is off.
    const static Int32 BATTERY_SAVER_MODE_DISABLED_WHEN_SCREEN_OFF = 1;
    // Secure setting for GPS behavior when battery saver mode is on.
    const static String BATTERY_SAVER_GPS_MODE;

    Int32 mLocationFlags;

    // current status
    Int32 mStatus;

    // time for last status update
    Int64 mStatusUpdateTime;

    // turn off GPS fix icon if we haven't received a fix in 10 seconds
    const static Int64 RECENT_FIX_TIMEOUT = 10 * 1000;

    // stop trying if we do not receive a fix within 60 seconds
    const static Int32 NO_FIX_TIMEOUT = 60 * 1000;

    // if the fix interval is below this we leave GPS on,
    // if above then we cycle the GPS driver.
    // Typical hot TTTF is ~5 seconds, so 10 seconds seems sane.
    const static Int32 GPS_POLLING_THRESHOLD_INTERVAL = 10 * 1000;

    // how often to request NTP time, in milliseconds
    // current setting 24 hours
    const static Int64 NTP_INTERVAL = 24*60*60*1000;
    // how long to wait if we have a network error in NTP or XTRA downloading
    // current setting - 5 minutes
    const static Int64 RETRY_INTERVAL = 5*60*1000;

    // true if we are enabled, protected by this
    Boolean mEnabled;

    // true if we have network connectivity
    Boolean mNetworkAvailable;

    // states for injecting ntp and downloading xtra data
    const static Int32 STATE_PENDING_NETWORK = 0;
    const static Int32 STATE_DOWNLOADING = 1;
    const static Int32 STATE_IDLE = 2;

    // flags to trigger NTP or XTRA data download when network becomes available
    // initialized to true so we do NTP and XTRA when the network comes up after booting
    Int32 mInjectNtpTimePending;
    Int32 mDownloadXtraDataPending;

    // set to true if the GPS engine does not do on-demand NTP time requests
    Boolean mPeriodicTimeInjection;

    // true if GPS is navigating
    Boolean mNavigating;

    // true if GPS engine is on
    Boolean mEngineOn;

    // requested frequency of fixes, in milliseconds
    Int32 mFixInterval;

    // true if we started navigation
    Boolean mStarted;

    // true if single shot request is in progress
    Boolean mSingleShot;

    // capabilities of the GPS engine
    Int32 mEngineCapabilities;

    // true if XTRA is supported
    Boolean mSupportsXtra;

    // for calculating time to first fix
    Int64 mFixRequestTime;
    // time to first fix for most recent session
    Int32 mTimeToFirstFix;
    // time we received our last fix
    Int64 mLastFixTime;

    Int32 mPositionMode;

    // Current request from underlying location clients.
    AutoPtr<IProviderRequest> mProviderRequest;
    // Current list of underlying location clients.
    AutoPtr<IWorkSource> mWorkSource;
    // True if gps should be disabled (used to support battery saver mode in settings).
    Boolean mDisableGps;

    // properties loaded from PROPERTIES_FILE
    AutoPtr<IProperties> mProperties;
    String mSuplServerHost;
    Int32 mSuplServerPort;
    String mC2KServerHost;
    Int32 mC2KServerPort;
    Boolean mSuplEsEnabled;

    AutoPtr<IContext> mContext;
    AutoPtr<INtpTrustedTime> mNtpTime;
    AutoPtr<IILocationManager> mILocationManager;
    AutoPtr<ILocation> mLocation;
    AutoPtr<IBundle> mLocationExtras;
    AutoPtr<GpsStatusListenerHelper> mListenerHelper;

    // Handler for processing events
    AutoPtr<IHandler> mHandler;

    String mAGpsApn;
    Int32 mApnIpType;
    Int32 mAGpsDataConnectionState;
    AutoPtr<IInetAddress> mAGpsDataConnectionIpAddr;
    AutoPtr<IConnectivityManager> mConnMgr;
    AutoPtr<IGpsNetInitiatedHandler> mNIHandler;

    String mDefaultApn;

    // Wakelocks
    const static String WAKELOCK_KEY;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    // Alarms
    const static String ALARM_WAKEUP;
    const static String ALARM_TIMEOUT;

    // SIM/Carrier info.
    const static String SIM_STATE_CHANGED;

    AutoPtr<IPowerManager> mPowerManager;
    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<IPendingIntent> mWakeupIntent;
    AutoPtr<IPendingIntent> mTimeoutIntent;

    AutoPtr<IIAppOpsService> mAppOpsService;
    AutoPtr<IIBatteryStats> mBatteryStats;

    // only modified on handler thread
    AutoPtr<IWorkSource> mClientSource;

    AutoPtr<IGeofenceHardwareImpl> mGeofenceHardwareImpl;
    AutoPtr<IIGpsStatusProvider> mGpsStatusProvider;
    AutoPtr<GpsMeasurementsProvider> mGpsMeasurementsProvider;
    AutoPtr<GpsNavigationMessageProvider> mGpsNavigationMessageProvider;

    AutoPtr<BroadcastReceiver> mBroadcastReceiver;

    AutoPtr<IIGpsGeofenceHardware> mGpsGeofenceBinder;

    //=============================================================
    // NI Client support
    //=============================================================
    AutoPtr<IINetInitiatedListener> mNetInitiatedListener;

    // for GPS SV statistics
    const static Int32 MAX_SVS = 32;
    const static Int32 EPHEMERIS_MASK = 0;
    const static Int32 ALMANAC_MASK = 1;
    const static Int32 USED_FOR_FIX_MASK = 2;

    // preallocated arrays, to avoid memory allocation in reportStatus()
    AutoPtr<ArrayOf<Int32> > mSvs;
    AutoPtr<ArrayOf<Float> > mSnrs;
    AutoPtr<ArrayOf<Float> > mSvElevations;
    AutoPtr<ArrayOf<Float> > mSvAzimuths;
    AutoPtr<ArrayOf<Int32> > mSvMasks;
    Int32 mSvCount;
    // preallocated to avoid memory allocation in reportNmea()
    AutoPtr<ArrayOf<Byte> > mNmeaBuffer;

    // static Boolean class_init_native_value;

    AutoPtr<ContentObserver> mDefaultApnObserver;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_GPSLOCATIONPROVIDER_H__