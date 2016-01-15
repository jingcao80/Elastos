
#ifndef __ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWARE_H__
#define __ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWARE_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Etl::HashMap;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Hardware::Location::IGeofenceHardwareCallback);
DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Hardware::Location::IGeofenceHardwareMonitorCallback);

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

class GeofenceHardware
    : public Object
    , public IGeofenceHardware
{
private:
    class GeofenceHardwareMonitorCallbackWrapper
        : public Object
        , public IIGeofenceHardwareMonitorCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        GeofenceHardwareMonitorCallbackWrapper(
            /* [in] */ IGeofenceHardwareMonitorCallback* c);

        CARAPI OnMonitoringSystemChange(
            /* [in] */ IGeofenceHardwareMonitorEvent* event);

        CARAPI ToString(
            /* [out] */ String* info);

    private:
        AutoPtr<IWeakReference> mCallback;
    };

    class GeofenceHardwareCallbackWrapper
        : public Object
        , public IIGeofenceHardwareCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        GeofenceHardwareCallbackWrapper(
            /* [in] */ IGeofenceHardwareCallback* c,
            /* [in] */ GeofenceHardware* host);

        CARAPI OnGeofenceTransition(
            /* [in] */ Int32 geofenceId,
            /* [in] */ Int32 transition,
            /* [in] */ ILocation* location,
            /* [in] */ Int64 timestamp,
            /* [in] */ Int32 monitoringType);

        CARAPI OnGeofenceAdd(
            /* [in] */ Int32 geofenceId,
            /* [in] */ Int32 status);

        CARAPI OnGeofenceRemove(
            /* [in] */ Int32 geofenceId,
            /* [in] */ Int32 status);

        CARAPI OnGeofencePause(
            /* [in] */ Int32 geofenceId,
            /* [in] */ Int32 status);

        CARAPI OnGeofenceResume(
            /* [in] */ Int32 geofenceId,
            /* [in] */ Int32 status);

        CARAPI ToString(
            /* [out] */ String* info);

    private:
        AutoPtr<IWeakReference> mCallback;
        GeofenceHardware* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    GeofenceHardware(
        /* [in] */ IIGeofenceHardware* service);

    virtual ~GeofenceHardware();

    /**
     * Returns all the hardware geofence monitoring systems which are supported
     *
     * <p> Call {@link #getStatusOfMonitoringType(int)} to know the current state
     * of a monitoring system.
     *
     * <p> Requires {@link android.Manifest.permission#LOCATION_HARDWARE} permission to access
     * geofencing in hardware.
     *
     * @return An array of all the monitoring types.
     *         An array of length 0 is returned in case of errors.
     */
    CARAPI GetMonitoringTypes(
        /* [out, callee] */ ArrayOf<Int32>** types);

    /**
     * Returns current status of a hardware geofence monitoring system.
     *
     * <p>Status can be one of {@link #MONITOR_CURRENTLY_AVAILABLE},
     * {@link #MONITOR_CURRENTLY_UNAVAILABLE} or {@link #MONITOR_UNSUPPORTED}
     *
     * <p> Some supported hardware monitoring systems might not be available
     * for monitoring geofences in certain scenarios. For example, when a user
     * enters a building, the GPS hardware subsystem might not be able monitor
     * geofences and will change from {@link #MONITOR_CURRENTLY_AVAILABLE} to
     * {@link #MONITOR_CURRENTLY_UNAVAILABLE}.
     *
     * @param monitoringType
     * @return Current status of the monitoring type.
     */
    CARAPI GetStatusOfMonitoringType(
        /* [in] */ Int32 monitoringType,
        /* [out] */ Int32* type);

    /**
     * Creates a circular geofence which is monitored by subsystems in the hardware.
     *
     * <p> When the device detects that is has entered, exited or is uncertain
     * about the area specified by the geofence, the given callback will be called.
     *
     * <p> If this call returns true, it means that the geofence has been sent to the hardware.
     * {@link GeofenceHardwareCallback#onGeofenceAdd} will be called with the result of the
     * add call from the hardware. The {@link GeofenceHardwareCallback#onGeofenceAdd} will be
     * called with the following parameters when a transition event occurs.
     * <ul>
     * <li> The geofence Id
     * <li> The location object indicating the last known location.
     * <li> The transition associated with the geofence. One of
     *      {@link #GEOFENCE_ENTERED}, {@link #GEOFENCE_EXITED}, {@link #GEOFENCE_UNCERTAIN}
     * <li> The timestamp when the geofence transition occured.
     * <li> The monitoring type ({@link #MONITORING_TYPE_GPS_HARDWARE} is one such example)
     *      that was used.
     * </ul>
     *
     * <p> The geofence will be monitored by the subsystem specified by monitoring_type parameter.
     * The application does not need to hold a wakelock when the monitoring
     * is being done by the underlying hardware subsystem. If the same geofence Id is being
     * monitored by two different monitoring systems, the same id can be used for both calls, as
     * long as the same callback object is used.
     *
     * <p> Requires {@link android.Manifest.permission#ACCESS_FINE_LOCATION} permission when
     * {@link #MONITORING_TYPE_GPS_HARDWARE} is used.
     *
     * <p> Requires {@link android.Manifest.permission#LOCATION_HARDWARE} permission to access
     * geofencing in hardware.
     *
     * <p>This API should not be called directly by the app developers. A higher level api
     * which abstracts the hardware should be used instead. All the checks are done by the higher
     * level public API. Any needed locking should be handled by the higher level API.
     *
     * <p> Create a geofence request object using the methods in {@link GeofenceHardwareRequest} to
     * set all the characteristics of the geofence. Use the created GeofenceHardwareRequest object
     * in this call.
     *
     * @param geofenceId The id associated with the geofence.
     * @param monitoringType The type of the hardware subsystem that should be used
     *        to monitor the geofence.
     * @param geofenceRequest The {@link GeofenceHardwareRequest} object associated with the
     *        geofence.
     * @param callback {@link GeofenceHardwareCallback} that will be use to notify the
     *        transition.
     * @return true when the geofence is successfully sent to the hardware for addition.
     * @throws IllegalArgumentException when the geofence request type is not supported.
     */
    CARAPI AddGeofence(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 monitoringType,
        /* [in] */ IGeofenceHardwareRequest* geofenceRequest,
        /* [in] */ IGeofenceHardwareCallback* _callback,
        /* [out] */ Boolean* result);

    /**
     * Removes a geofence added by {@link #addGeofence} call.
     *
     * <p> If this call returns true, it means that the geofence has been sent to the hardware.
     * {@link GeofenceHardwareCallback#onGeofenceRemove} will be called with the result of the
     * remove call from the hardware.
     *
     * <p> Requires {@link android.Manifest.permission#ACCESS_FINE_LOCATION} permission when
     * {@link #MONITORING_TYPE_GPS_HARDWARE} is used.
     *
     * <p> Requires {@link android.Manifest.permission#LOCATION_HARDWARE} permission to access
     * geofencing in hardware.
     *
     * <p>This API should not be called directly by the app developers. A higher level api
     * which abstracts the hardware should be used instead. All the checks are done by the higher
     * level public API. Any needed locking should be handled by the higher level API.
     *
     * @param geofenceId The id of the geofence.
     * @param monitoringType The type of the hardware subsystem that should be used
     *        to monitor the geofence.
     * @return true when the geofence is successfully sent to the hardware for removal.                     .
     */
    CARAPI RemoveGeofence(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 monitoringType,
        /* [out] */ Boolean* result);

    /**
     * Pauses the monitoring of a geofence added by {@link #addGeofence} call.
     *
     * <p> If this call returns true, it means that the geofence has been sent to the hardware.
     * {@link GeofenceHardwareCallback#onGeofencePause} will be called with the result of the
     * pause call from the hardware.
     *
     * <p> Requires {@link android.Manifest.permission#ACCESS_FINE_LOCATION} permission when
     * {@link #MONITORING_TYPE_GPS_HARDWARE} is used.
     *
     * <p> Requires {@link android.Manifest.permission#LOCATION_HARDWARE} permission to access
     * geofencing in hardware.
     *
     * <p>This API should not be called directly by the app developers. A higher level api
     * which abstracts the hardware should be used instead. All the checks are done by the higher
     * level public API. Any needed locking should be handled by the higher level API.
     *
     * @param geofenceId The id of the geofence.
     * @param monitoringType The type of the hardware subsystem that should be used
     *        to monitor the geofence.
     * @return true when the geofence is successfully sent to the hardware for pausing.
     */
    CARAPI PauseGeofence(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 monitoringType,
        /* [out] */ Boolean* result);

    /**
     * Resumes the monitoring of a geofence added by {@link #pauseGeofence} call.
     *
     * <p> If this call returns true, it means that the geofence has been sent to the hardware.
     * {@link GeofenceHardwareCallback#onGeofenceResume} will be called with the result of the
     * resume call from the hardware.
     *
     * <p> Requires {@link android.Manifest.permission#ACCESS_FINE_LOCATION} permission when
     * {@link #MONITORING_TYPE_GPS_HARDWARE} is used.
     *
     * <p> Requires {@link android.Manifest.permission#LOCATION_HARDWARE} permission to access
     * geofencing in hardware.
     *
     * <p>This API should not be called directly by the app developers. A higher level api
     * which abstracts the hardware should be used instead. All the checks are done by the higher
     * level public API. Any needed locking should be handled by the higher level API.
     *
     * @param geofenceId The id of the geofence.
     * @param monitoringType The type of the hardware subsystem that should be used
     *        to monitor the geofence.
     * @param monitorTransition Bitwise OR of {@link #GEOFENCE_ENTERED},
     *        {@link #GEOFENCE_EXITED}, {@link #GEOFENCE_UNCERTAIN}
     * @return true when the geofence is successfully sent to the hardware for resumption.
     */
    CARAPI ResumeGeofence(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 monitoringType,
        /* [in] */ Int32 monitorTransition,
        /* [out] */ Boolean* result);

    /**
     * Register the callback to be notified when the state of a hardware geofence
     * monitoring system changes. For instance, it can change from
     * {@link #MONITOR_CURRENTLY_AVAILABLE} to {@link #MONITOR_CURRENTLY_UNAVAILABLE}
     *
     * <p> Requires {@link android.Manifest.permission#ACCESS_FINE_LOCATION} permission when
     * {@link #MONITORING_TYPE_GPS_HARDWARE} is used.
     *
     * <p> Requires {@link android.Manifest.permission#LOCATION_HARDWARE} permission to access
     * geofencing in hardware.
     *
     * <p>This API should not be called directly by the app developers. A higher level api
     * which abstracts the hardware should be used instead. All the checks are done by the higher
     * level public API. Any needed locking should be handled by the higher level API.
     *
     * <p> The same callback object can be used to be informed of geofence transitions
     * and state changes of the underlying hardware subsystem.
     *
     * @param monitoringType Type of the monitor
     * @param callback Callback that will be called.
     * @return true on success
     */
    CARAPI RegisterForMonitorStateChangeCallback(
        /* [in] */ Int32 monitoringType,
        /* [in] */ IGeofenceHardwareMonitorCallback* _callback,
        /* [out] */ Boolean* result);

    /**
     * Unregister the callback that was used with {@link #registerForMonitorStateChangeCallback}
     * to notify when the state of the hardware geofence monitoring system changes.
     *
     * <p> Requires {@link android.Manifest.permission#ACCESS_FINE_LOCATION} permission when
     * {@link #MONITORING_TYPE_GPS_HARDWARE} is used.
     *
     * <p> Requires {@link android.Manifest.permission#LOCATION_HARDWARE} permission to access
     * geofencing in hardware.
     *
     * <p>This API should not be called directly by the app developers. A higher level api
     * which abstracts the hardware should be used instead. All the checks are done by the higher
     * level public API. Any needed locking should be handled by the higher level API.
     *
     * @param monitoringType Type of the monitor
     * @param callback Callback that will be called.
     * @return true on success
     */
    CARAPI UnregisterForMonitorStateChangeCallback(
        /* [in] */ Int32 monitoringType,
        /* [in] */ IGeofenceHardwareMonitorCallback* _callback,
        /* [out] */ Boolean* result);

private:
    CARAPI_(void) RemoveCallback(
        /* [in] */ IGeofenceHardwareCallback* _callback);

    CARAPI GetCallbackWrapper(
        /* [in] */ IGeofenceHardwareCallback* _callback,
        /* [out] */ GeofenceHardwareCallbackWrapper** wrapper);

    CARAPI_(void) RemoveMonitorCallback(
        /* [in] */ IGeofenceHardwareMonitorCallback* _callback);

    CARAPI GetMonitorCallbackWrapper(
        /* [in] */ IGeofenceHardwareMonitorCallback* _callback,
        /* [out] */ GeofenceHardwareMonitorCallbackWrapper** wrapper);

private:
    friend class GeofenceHardwareImpl;

    AutoPtr<IIGeofenceHardware> mService;

    // Hardware systems that do geofence monitoring.
    static const Int32 NUM_MONITORS;

    HashMap<AutoPtr<IGeofenceHardwareCallback>, AutoPtr<GeofenceHardwareCallbackWrapper> > mCallbacks;
    Object mCallbacksLock;

    HashMap<AutoPtr<IGeofenceHardwareMonitorCallback>, AutoPtr<GeofenceHardwareMonitorCallbackWrapper> > mMonitorCallbacks;
    Object mMonitorCallbacksLock;
};

} //Location
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWARE_H__
