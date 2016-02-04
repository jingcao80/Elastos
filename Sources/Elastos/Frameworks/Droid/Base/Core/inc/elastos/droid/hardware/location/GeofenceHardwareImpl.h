
#ifndef __ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWAREIMPL_H__
#define __ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWAREIMPL_H__

#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::IIFusedGeofenceHardware;
using Elastos::Droid::Location::IIGpsGeofenceHardware;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

class ECO_PUBLIC GeofenceHardwareImpl
    : public Object
    , public IGeofenceHardwareImpl
{
private:
    class ECO_LOCAL GeofenceTransition
        : public Object
    {
    public:
        GeofenceTransition(
            /* [in] */ Int32 geofenceId,
            /* [in] */ Int32 transition,
            /* [in] */ Int64 timestamp,
            /* [in] */ ILocation* location,
            /* [in] */ Int32 monitoringType,
            /* [in] */ Int32 sourcesUsed);

    public:
        Int32 mGeofenceId;
        Int32 mTransition;
        Int64 mTimestamp;
        AutoPtr<ILocation> mLocation;
        Int32 mMonitoringType;
        Int32 mSourcesUsed;
    };

    class ECO_LOCAL Reaper
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        Reaper(
            /* [in] */ IIGeofenceHardwareCallback* c,
            /* [in] */ Int32 monitoringType,
            /* [in] */ GeofenceHardwareImpl* host);

        Reaper(
            /* [in] */ IIGeofenceHardwareMonitorCallback* c,
            /* [in] */ Int32 monitoringType,
            /* [in] */ GeofenceHardwareImpl* host);

        //@Override
        CARAPI ProxyDied();

        //@Override
        CARAPI GetHashCode(
            /* [out] */ Int32* code);

        //@Override
        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IIGeofenceHardwareMonitorCallback> mMonitorCallback;
        AutoPtr<IIGeofenceHardwareCallback> mCallback;
        Int32 mMonitoringType;
        GeofenceHardwareImpl* mHost;
    };

    class ECO_LOCAL GeofenceHandler : public Handler
    {
    public:
        GeofenceHandler(
            /* [in] */ GeofenceHardwareImpl* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        GeofenceHardwareImpl* mHost;
    };

    class ECO_LOCAL CallbacksHandler : public Handler
    {
    public:
        CallbacksHandler(
            /* [in] */ GeofenceHardwareImpl* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        GeofenceHardwareImpl* mHost;
    };

    class ECO_LOCAL ReaperHandler : public Handler
    {
    public:
        ReaperHandler(
            /* [in] */ GeofenceHardwareImpl* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        GeofenceHardwareImpl* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    GeofenceHardwareImpl();

    virtual ~GeofenceHardwareImpl();

    static CARAPI_(AutoPtr<IGeofenceHardwareImpl>) GetInstance(
        /* [in] */ IContext* context);

    CARAPI SetGpsHardwareGeofence(
        /* [in] */ IIGpsGeofenceHardware* service);

    CARAPI SetFusedGeofenceHardware(
        /* [in] */ IIFusedGeofenceHardware* service);

    CARAPI GetMonitoringTypes(
        /* [out, callee] */ ArrayOf<Int32>** types);

    CARAPI GetStatusOfMonitoringType(
        /* [in] */ Int32 monitoringType,
        /* [out] */ Int32* type);

    CARAPI AddCircularFence(
        /* [in] */ Int32 monitoringType,
        /* [in] */ IGeofenceHardwareRequestParcelable* request,
        /* [in] */ IIGeofenceHardwareCallback* _callback,
        /* [out] */ Boolean* result);

    CARAPI RemoveGeofence(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 monitoringType,
        /* [out] */ Boolean* result);

    CARAPI PauseGeofence(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 monitoringType,
        /* [out] */ Boolean* result);

    CARAPI ResumeGeofence(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 monitoringType,
        /* [in] */ Int32 monitorTransition,
        /* [out] */ Boolean* result);

    CARAPI RegisterForMonitorStateChangeCallback(
        /* [in] */ Int32 monitoringType,
        /* [in] */ IIGeofenceHardwareMonitorCallback* _callback,
        /* [out] */ Boolean* result);

    CARAPI UnregisterForMonitorStateChangeCallback(
        /* [in] */ Int32 monitoringType,
        /* [in] */ IIGeofenceHardwareMonitorCallback* _callback,
        /* [out] */ Boolean* result);

    /**
     * Used to report geofence transitions
     */
    CARAPI ReportGeofenceTransition(
        /* [in] */ Int32 geofenceId,
        /* [in] */ ILocation* location,
        /* [in] */ Int32 transition,
        /* [in] */ Int64 transitionTimestamp,
        /* [in] */ Int32 monitoringType,
        /* [in] */ Int32 sourcesUsed);

    /**
     * Used to report Monitor status changes.
     */
    CARAPI ReportGeofenceMonitorStatus(
        /* [in] */ Int32 monitoringType,
        /* [in] */ Int32 monitoringStatus,
        /* [in] */ ILocation* location,
        /* [in] */ Int32 source);

    /**
     * Used to report the status of a Geofence Add operation.
     */
    CARAPI ReportGeofenceAddStatus(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 status);

    /**
     * Used to report the status of a Geofence Remove operation.
     */
    CARAPI ReportGeofenceRemoveStatus(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 status);

    /**
     * Used to report the status of a Geofence Pause operation.
     */
    CARAPI ReportGeofencePauseStatus(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 status);

    /**
     * Used to report the status of a Geofence Resume operation.
     */
    CARAPI ReportGeofenceResumeStatus(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 status);

    CARAPI GetMonitoringResolutionLevel(
        /* [in] */ Int32 monitoringType,
        /* [out] */ Int32* level);

    CARAPI GetAllowedResolutionLevel(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [out] */ Int32* level);

private:
    ECO_LOCAL GeofenceHardwareImpl(
        /* [in] */ IContext* context);

    ECO_LOCAL CARAPI AcquireWakeLock();

    ECO_LOCAL CARAPI ReleaseWakeLock();

    ECO_LOCAL CARAPI_(void) UpdateGpsHardwareAvailability();

    ECO_LOCAL CARAPI_(void) UpdateFusedHardwareAvailability();

    /**
     * Internal generic status report function for Geofence operations.
     *
     * @param operation The operation to be reported as defined internally.
     * @param geofenceId The id of the geofence the operation is related to.
     * @param operationStatus The status of the operation as defined in GeofenceHardware class. This
     *                        status is independent of the statuses reported by different HALs.
     */
    ECO_LOCAL CARAPI ReportGeofenceOperationStatus(
        /* [in] */ Int32 operation,
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 operationStatus);

    ECO_LOCAL CARAPI_(void) SetMonitorAvailability(
        /* [in] */ Int32 monitor,
        /* [in] */ Int32 val);

private:
    ECO_LOCAL static const String TAG;
    ECO_LOCAL static const Boolean DEBUG;

    ECO_LOCAL static Object sLock;

    AutoPtr<IContext> mContext;
    static AutoPtr<IGeofenceHardwareImpl> sInstance;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    HashMap<Int32, AutoPtr<IIGeofenceHardwareCallback> > mGeofences;
    Object mGeofencesLock;

    AutoPtr<ArrayOf<IArrayList*> > mCallbacks;
    AutoPtr<IArrayList> mReapers;

    AutoPtr<IIFusedGeofenceHardware> mFusedService;
    AutoPtr<IIGpsGeofenceHardware> mGpsService;

    AutoPtr<ArrayOf<Int32> > mSupportedMonitorTypes;
    Object mSupportedMonitorTypesLock;

    // mGeofenceHandler message types
    AutoPtr<GeofenceHandler> mGeofenceHandler;
    ECO_LOCAL static const Int32 GEOFENCE_TRANSITION_CALLBACK;
    ECO_LOCAL static const Int32 ADD_GEOFENCE_CALLBACK;
    ECO_LOCAL static const Int32 REMOVE_GEOFENCE_CALLBACK;
    ECO_LOCAL static const Int32 PAUSE_GEOFENCE_CALLBACK;
    ECO_LOCAL static const Int32 RESUME_GEOFENCE_CALLBACK;
    ECO_LOCAL static const Int32 GEOFENCE_CALLBACK_BINDER_DIED;

    // mCallbacksHandler message types
    AutoPtr<CallbacksHandler> mCallbacksHandler;
    ECO_LOCAL static const Int32 GEOFENCE_STATUS;
    ECO_LOCAL static const Int32 CALLBACK_ADD;
    ECO_LOCAL static const Int32 CALLBACK_REMOVE;
    ECO_LOCAL static const Int32 MONITOR_CALLBACK_BINDER_DIED;

    // mReaperHandler message types
    AutoPtr<ReaperHandler> mReaperHandler;
    ECO_LOCAL static const Int32 REAPER_GEOFENCE_ADDED;
    ECO_LOCAL static const Int32 REAPER_MONITOR_CALLBACK_ADDED;
    ECO_LOCAL static const Int32 REAPER_REMOVED;

    // The following constants need to match GpsLocationFlags enum in gps.h
    ECO_LOCAL static const Int32 LOCATION_INVALID;
    ECO_LOCAL static const Int32 LOCATION_HAS_LAT_LONG;
    ECO_LOCAL static const Int32 LOCATION_HAS_ALTITUDE;
    ECO_LOCAL static const Int32 LOCATION_HAS_SPEED;
    ECO_LOCAL static const Int32 LOCATION_HAS_BEARING;
    ECO_LOCAL static const Int32 LOCATION_HAS_ACCURACY;

    // Resolution level constants used for permission checks.
    // These constants must be in increasing order of finer resolution.
    ECO_LOCAL static const Int32 RESOLUTION_LEVEL_NONE;
    ECO_LOCAL static const Int32 RESOLUTION_LEVEL_COARSE;
    ECO_LOCAL static const Int32 RESOLUTION_LEVEL_FINE;
};

} //Location
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWAREIMPL_H__
