
#ifndef __ELASTOS_DROID_SERVER_LOCATION_GPSLOCATIONPROVIDER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GPSLOCATIONPROVIDER_H__

#ifdef DROID_CORE
#include "Elastos.Droid.Core_server.h"
#elif defined(DROID_SERVER)
#include "Elastos.Droid.Core.h"
#endif
#include "location/LocationProviderInterface.h"
#include "elastos/droid/content/BroadcastReceiver.h"

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::IProperties;
using Elastos::Utility::CProperties;
using Elastos::Droid::Utility::INtpTrustedTime;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::EIID_IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::EIID_IConnectivityManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Location::IGpsNetInitiatedHandler;
using Elastos::Droid::Location::IIGpsStatusProvider;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationListener;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::IILocationManager;
using Elastos::Droid::Location::IINetInitiatedListener;
using Elastos::Droid::Location::IIGpsStatusListener;
using Elastos::Droid::Location::IProviderProperties;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * A GPS implementation of LocationProvider used by LocationManager.
 *
 * {@hide}
 */
AutoPtr<IProviderProperties> InitGpsLocationProvider();

class GpsLocationProvider
        : public ElRefBase
        , public ILocationProviderInterface
{
private:

    class NetworkLocationListener
            : public ILocationListener
            , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        NetworkLocationListener(
            /* [in] */ GpsLocationProvider* host);

        CARAPI OnLocationChanged(
            /* [in] */ ILocation* location);

        CARAPI OnStatusChanged(
            /* [in] */ const String& provider,
            /* [in] */ Int32 status,
            /* [in] */ IBundle* extras);

        CARAPI OnProviderEnabled(
            /* [in] */ const String& provider);

        CARAPI OnProviderDisabled(
            /* [in] */ const String& provider);
    private:
        GpsLocationProvider* mHost;
    };
    class ConstructorRunnable
            : public IRunnable
            , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        ConstructorRunnable(
            /* [in] */ GpsLocationProvider* host);

        CARAPI Run();
    private:
        GpsLocationProvider* mHost;
    };

    /** simpler wrapper for ProviderRequest + Worksource */
    class GpsRequest
    {
    public:
        GpsRequest(
            /* [in] */ IProviderRequest* _request,
            /* [in] */ IWorkSource* _source)
              : request(_request)
              , source(_source)
        {}

    public:
        AutoPtr<IProviderRequest> request;
        AutoPtr<IWorkSource> source;
    };

    class Listener : public IProxyDeathRecipient
    {
    public:
        Listener(
            /* [in] */ IIGpsStatusListener* listener);

        CARAPI ProxyDied();
    private:
        GpsLocationProvider* mHost;
    };

    class MyBroadcastReceiver : public BroadcastReceiver
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

    static const String TAG;// = "GpsLocationProvider";

public:
    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *object,
            /* [out] */ InterfaceID *IID);

    CARAPI_(AutoPtr<IIGpsStatusProvider>) GetGpsStatusProvider();

    static CARAPI_(Boolean) IsSupported();

    GpsLocationProvider(
        /* [in] */ IContext* context,
        /* [in] */ IILocationManager* ilocationManager);

    /**
     * Returns the name of this provider.
     */
    //@Override
    CARAPI GetName(
        /* [out] */ String* name);

    //@Override
    CARAPI GetProperties(
        /* [out] */ IProviderProperties** properties);

    CARAPI_(void) UpdateNetworkState(
        /* [in] */ Int32 state,
        /* [in] */ INetworkInfo* info);

    /**
     * Enables this provider.  When enabled, calls to getStatus()
     * must be handled.  Hardware may be started up
     * when the provider is enabled.
     */
    //@Override
    CARAPI Enable();

    /**
     * Disables this provider.  When disabled, calls to getStatus()
     * need not be handled.  Hardware may be shut
     * down while the provider is disabled.
     */
    //@Override
    CARAPI Disable();

    //@Override
    CARAPI IsEnabled(
        /* [out] */ Boolean* enable);

    //@Override
    CARAPI GetStatus(
        /* [in] */ IBundle* extras,
        /* [out] */ Int32* status);

    //@Override
    CARAPI GetStatusUpdateTime(
        /* [out] */ Int64* time);

    //@Override
    CARAPI SetRequest(
        /* [in] */ IProviderRequest* request,
        /* [in] */ IWorkSource* source);

    //@Override
    CARAPI SwitchUser(
        /* [in] */ Int32 userId);

    //@Override
    CARAPI SendExtraCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle* extras,
        /* [out] */ Boolean* result);

    CARAPI_(AutoPtr<IINetInitiatedListener>) GetNetInitiatedListener();

    // Called by JNI function to report an NI request.
    CARAPI_(void) ReportNiNotification(
        /* [in] */ Int32 notificationId,
        /* [in] */ Int32 niType,
        /* [in] */ Int32 notifyFlags,
        /* [in] */ Int32 timeout,
        /* [in] */ Int32 defaultResponse,
        /* [in] */ const String& requestorId,
        /* [in] */ const String& text,
        /* [in] */ Int32 requestorIdEncoding,
        /* [in] */ Int32 textEncoding,
        /* [in] */ const String& extras  // Encoded extra data
        );

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:

    static const Boolean DEBUG;// = Log.isLoggable(TAG, Log.DEBUG);
    static const Boolean VERBOSE;// = Log.isLoggable(TAG, Log.VERBOSE);

    static const AutoPtr<IProviderProperties> PROPERTIES;// = new ProviderProperties(
            //true, true, false, false, true, true, true,
            //Criteria.POWER_HIGH, Criteria.ACCURACY_FINE);

    // these need to match GpsPositionMode enum in gps.h
    static const Int32 GPS_POSITION_MODE_STANDALONE;// = 0;
    static const Int32 GPS_POSITION_MODE_MS_BASED;// = 1;
    static const Int32 GPS_POSITION_MODE_MS_ASSISTED;// = 2;

    // these need to match GpsPositionRecurrence enum in gps.h
    static const Int32 GPS_POSITION_RECURRENCE_PERIODIC;// = 0;
    static const Int32 GPS_POSITION_RECURRENCE_SINGLE;// = 1;

    // these need to match GpsStatusValue defines in gps.h
    static const Int32 GPS_STATUS_NONE;// = 0;
    static const Int32 GPS_STATUS_SESSION_BEGIN;// = 1;
    static const Int32 GPS_STATUS_SESSION_END;// = 2;
    static const Int32 GPS_STATUS_ENGINE_ON;// = 3;
    static const Int32 GPS_STATUS_ENGINE_OFF;// = 4;

    // these need to match GpsApgsStatusValue defines in gps.h
    /** AGPS status event values. */
    static const Int32 GPS_REQUEST_AGPS_DATA_CONN;// = 1;
    static const Int32 GPS_RELEASE_AGPS_DATA_CONN;// = 2;
    static const Int32 GPS_AGPS_DATA_CONNECTED;// = 3;
    static const Int32 GPS_AGPS_DATA_CONN_DONE;// = 4;
    static const Int32 GPS_AGPS_DATA_CONN_FAILED;// = 5;

    // these need to match GpsLocationFlags enum in gps.h
    static const Int32 LOCATION_INVALID;// = 0;
    static const Int32 LOCATION_HAS_LAT_LONG;// = 1;
    static const Int32 LOCATION_HAS_ALTITUDE;// = 2;
    static const Int32 LOCATION_HAS_SPEED;// = 4;
    static const Int32 LOCATION_HAS_BEARING;// = 8;
    static const Int32 LOCATION_HAS_ACCURACY;// = 16;

// IMPORTANT - the GPS_DELETE_* symbols here must match constants in gps.h
    static const Int32 GPS_DELETE_EPHEMERIS;// = 0x0001;
    static const Int32 GPS_DELETE_ALMANAC;// = 0x0002;
    static const Int32 GPS_DELETE_POSITION;// = 0x0004;
    static const Int32 GPS_DELETE_TIME;// = 0x0008;
    static const Int32 GPS_DELETE_IONO;// = 0x0010;
    static const Int32 GPS_DELETE_UTC;// = 0x0020;
    static const Int32 GPS_DELETE_HEALTH;// = 0x0040;
    static const Int32 GPS_DELETE_SVDIR;// = 0x0080;
    static const Int32 GPS_DELETE_SVSTEER;// = 0x0100;
    static const Int32 GPS_DELETE_SADATA;// = 0x0200;
    static const Int32 GPS_DELETE_RTI;// = 0x0400;
    static const Int32 GPS_DELETE_CELLDB_INFO;// = 0x8000;
    static const Int32 GPS_DELETE_ALL;// = 0xFFFF;

    // The GPS_CAPABILITY_* flags must match the values in gps.h
    static const Int32 GPS_CAPABILITY_SCHEDULING;// = 0x0000001;
    static const Int32 GPS_CAPABILITY_MSB;// = 0x0000002;
    static const Int32 GPS_CAPABILITY_MSA;// = 0x0000004;
    static const Int32 GPS_CAPABILITY_SINGLE_SHOT;// = 0x0000008;
    static const Int32 GPS_CAPABILITY_ON_DEMAND_TIME;// = 0x0000010;

    // these need to match AGpsType enum in gps.h
    static const Int32 AGPS_TYPE_SUPL;// = 1;
    static const Int32 AGPS_TYPE_C2K;// = 2;

    // for mAGpsDataConnectionState
    static const Int32 AGPS_DATA_CONNECTION_CLOSED;// = 0;
    static const Int32 AGPS_DATA_CONNECTION_OPENING;// = 1;
    static const Int32 AGPS_DATA_CONNECTION_OPEN;// = 2;

    // Handler messages
    static const Int32 CHECK_LOCATION;// = 1;
    static const Int32 ENABLE;// = 2;
    static const Int32 SET_REQUEST;// = 3;
    static const Int32 UPDATE_NETWORK_STATE;// = 4;
    static const Int32 INJECT_NTP_TIME;// = 5;
    static const Int32 DOWNLOAD_XTRA_DATA;// = 6;
    static const Int32 UPDATE_LOCATION;// = 7;
    static const Int32 ADD_LISTENER;// = 8;
    static const Int32 REMOVE_LISTENER;// = 9;
    static const Int32 INJECT_NTP_TIME_FINISHED;// = 10;
    static const Int32 DOWNLOAD_XTRA_DATA_FINISHED;// = 11;

    // Request setid
    static const Int32 AGPS_RIL_REQUEST_SETID_IMSI;// = 1;
    static const Int32 AGPS_RIL_REQUEST_SETID_MSISDN;// = 2;

    // Request ref location
    static const Int32 AGPS_RIL_REQUEST_REFLOC_CELLID;// = 1;
    static const Int32 AGPS_RIL_REQUEST_REFLOC_MAC;// = 2;

    // ref. location info
    static const Int32 AGPS_REF_LOCATION_TYPE_GSM_CELLID;// = 1;
    static const Int32 AGPS_REF_LOCATION_TYPE_UMTS_CELLID;// = 2;
    static const Int32 AGPS_REG_LOCATION_TYPE_MAC;//        = 3;

    // set id info
    static const Int32 AGPS_SETID_TYPE_NONE;// = 0;
    static const Int32 AGPS_SETID_TYPE_IMSI;// = 1;
    static const Int32 AGPS_SETID_TYPE_MSISDN;// = 2;

    static const String PROPERTIES_FILE;// = "/etc/gps.conf";

private:

    CARAPI_(void) CheckSmsSuplInit(
        /* [in] */ IIntent* intent);

    CARAPI_(void) CheckWapSuplInit(
        /* [in] */ IIntent* intent);

    CARAPI_(void) ListenForBroadcasts();

    CARAPI_(void) HandleUpdateNetworkState(
        /* [in] */ Int32 state,
        /* [in] */ INetworkInfo* info);

    CARAPI_(void) HandleInjectNtpTime();

    CARAPI_(void) HandleDownloadXtraData();

    CARAPI_(void) HandleUpdateLocation(
        /* [in] */ ILocation* location);

    CARAPI_(void) HandleEnable();

    CARAPI_(void) HandleDisable();

    CARAPI_(void) UpdateStatus(
        /* [in] */ Int32 status,
        /* [in] */ Int32 svCount);

    CARAPI_(void) HandleSetRequest(
        /* [in] */ IProviderRequest* request,
        /* [in] */ IWorkSource* source);

    CARAPI_(void) UpdateClientUids(
        /* [in] */ ArrayOf<Int32>* uids);

    CARAPI_(Boolean) DeleteAidingData(
        /* [in] */ IBundle* extras);

    CARAPI_(void) StartNavigating();

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
        /* [in] */ Int32 ipaddr);

    /**
     * called from native code to report NMEA data received
     */
    CARAPI_(void) ReportNmea(
        /* [in] */ Int64 timestamp);

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

    CARAPI_(void) RequestRefLocation(
        /* [in] */ Int32 flags);

    CARAPI_(void) SendMessage(
        /* [in] */ Int32 message,
        /* [in] */ Int32 arg,
        /* [in] */ IInterface* obj);

    CARAPI_(String) GetSelectedApn();

    //static { class_init_native(); }
    static CARAPI_(void) Class_init_native();
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
        /* [in] */ Int32 uncertaInt32y);

    CARAPI_(Boolean) Native_supports_xtra();
    CARAPI_(void) Native_inject_xtra_data(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 length);

    // DEBUG Support
    CARAPI_(String) Native_get_internal_state();

    // AGPS Support
    CARAPI_(void) Native_agps_data_conn_open(
        /* [in] */ const String& apn);

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

private:

    Object mLock;

    Int32 mLocationFlags;// = LOCATION_INVALID;

    // current status
    Int32 mStatus;// = LocationProvider.TEMPORARILY_UNAVAILABLE;

    // time for last status update
    Int64 mStatusUpdateTime;// = SystemClock.elapsedRealtime();

    // turn off GPS fix icon if we haven't received a fix in 10 seconds
    static const Int64 RECENT_FIX_TIMEOUT;// = 10 * 1000;

    // stop trying if we do not receive a fix within 60 seconds
    static const Int32 NO_FIX_TIMEOUT;// = 60 * 1000;

    // if the fix interval is below this we leave GPS on,
    // if above then we cycle the GPS driver.
    // Typical hot TTTF is ~5 seconds, so 10 seconds seems sane.
    static const Int32 GPS_POLLING_THRESHOLD_INTERVAL;// = 10 * 1000;

    // how often to request NTP time, in milliseconds
    // current setting 24 hours
    static const Int64 NTP_INTERVAL;// = 24*60*60*1000;
    // how long to wait if we have a network error in NTP or XTRA downloading
    // current setting - 5 minutes
    static const Int64 RETRY_INTERVAL;// = 5*60*1000;

    // true if we are enabled, protected by this
    Boolean mEnabled;

    // true if we have network connectivity
    Boolean mNetworkAvailable;

    // states for injecting ntp and downloading xtra data
    static const Int32 STATE_PENDING_NETWORK;// = 0;
    static const Int32 STATE_DOWNLOADING;// = 1;
    static const Int32 STATE_IDLE;// = 2;

    // flags to trigger NTP or XTRA data download when network becomes available
    // initialized to true so we do NTP and XTRA when the network comes up after booting
    Int32 mInjectNtpTimePending;// = STATE_PENDING_NETWORK;
    Int32 mDownloadXtraDataPending;// = STATE_PENDING_NETWORK;

    // set to true if the GPS engine does not do on-demand NTP time requests
    Boolean mPeriodicTimeInjection;

    // true if GPS is navigating
    Boolean mNavigating;

    // true if GPS engine is on
    Boolean mEngineOn;

    // requested frequency of fixes, in milliseconds
    Int32 mFixInterval;// = 1000;

    // true if we started navigation
    Boolean mStarted;

    // capabilities of the GPS engine
    Int32 mEngineCapabilities;

    // true if XTRA is supported
    Boolean mSupportsXtra;

    // for calculating time to first fix
    Int64 mFixRequestTime;// = 0;
    // time to first fix for most recent session
    Int32 mTimeToFirstFix;// = 0;
    // time we received our last fix
    Int64 mLastFixTime;

    Int32 mPositionMode;

    // properties loaded from PROPERTIES_FILE
    AutoPtr<IProperties> mProperties;
    String mSuplServerHost;
    Int32 mSuplServerPort;
    String mC2KServerHost;
    Int32 mC2KServerPort;

    const AutoPtr<IContext> mContext;
    AutoPtr<INtpTrustedTime> mNtpTime;
    /*const*/ AutoPtr<IILocationManager> mILocationManager;
    AutoPtr<ILocation> mLocation;// = new Location(LocationManager.GPS_PROVIDER);
    Object mLocationMutex;
    AutoPtr<IBundle> mLocationExtras;// = new Bundle();
    //private ArrayList<Listener> mListeners = new ArrayList<Listener>();
    List<AutoPtr<Listener> > mListeners;
    Object mListenersMutex;

    // Handler for processing events
    AutoPtr<IHandler> mHandler;

    String mAGpsApn;
    Int32 mAGpsDataConnectionState;
    Int32 mAGpsDataConnectionIpAddr;
    AutoPtr<IConnectivityManager> mConnMgr;
    AutoPtr<IGpsNetInitiatedHandler> mNIHandler;

    // Wakelocks
    const static String WAKELOCK_KEY;// = "GpsLocationProvider";
    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    // Alarms
    const static String ALARM_WAKEUP;// = "com.android.internal.location.ALARM_WAKEUP";
    const static String ALARM_TIMEOUT;// = "com.android.internal.location.ALARM_TIMEOUT";
    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<IPendingIntent> mWakeupintent;
    AutoPtr<IPendingIntent> mTimeoutintent;

    AutoPtr<IBatteryStats> mBatteryStats;

    // only modified on handler thread
    AutoPtr<ArrayOf<Int32> > mClientUids;// = new Int32[0];

    AutoPtr<IIGpsStatusProvider> mGpsStatusProvider;

    AutoPtr<IBroadcastReceiver> mBroadcastReciever;

    //=============================================================
    // NI Client support
    //=============================================================
    const AutoPtr<IINetInitiatedListener> mNetInitiatedListener;

    // for GPS SV statistics
    static const Int32 MAX_SVS;// = 32;
    static const Int32 EPHEMERIS_MASK;// = 0;
    static const Int32 ALMANAC_MASK;// = 1;
    static const Int32 USED_FOR_FIX_MASK;// = 2;

    // preallocated arrays, to avoid memory allocation in reportStatus()
    AutoPtr<ArrayOf<Int32> > mSvs;// = new Int32[MAX_SVS];
    AutoPtr<ArrayOf<Float> > mSnrs;// = new float[MAX_SVS];
    AutoPtr<ArrayOf<Float> > mSvElevations;// = new float[MAX_SVS];
    AutoPtr<ArrayOf<Float> > mSvAzimuths;// = new float[MAX_SVS];
    AutoPtr<ArrayOf<Int32> > mSvMasks;// = new Int32[3];
    Int32 mSvCount;
    // preallocated to avoid memory allocation in reportNmea()
    AutoPtr<ArrayOf<Byte> > mNmeaBuffer;// = new byte[120];
    friend AutoPtr<IProviderProperties> InitGpsLocationProvider();
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_GPSLOCATIONPROVIDER_H__
