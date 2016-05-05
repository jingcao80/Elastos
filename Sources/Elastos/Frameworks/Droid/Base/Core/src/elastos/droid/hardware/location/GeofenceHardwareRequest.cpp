
#include "elastos/droid/hardware/location/GeofenceHardwareRequest.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

const Int32 GeofenceHardwareRequest::GEOFENCE_TYPE_CIRCLE = 0;

CAR_INTERFACE_IMPL(GeofenceHardwareRequest, Object, IGeofenceHardwareRequest)

GeofenceHardwareRequest::GeofenceHardwareRequest()
    : mType(0)
    , mLatitude(0.0)
    , mLongitude(0.0)
    , mRadius(0.0)
    , mLastTransition(IGeofenceHardware::GEOFENCE_UNCERTAIN)
    , mUnknownTimer(30000) // 30 secs
    , mMonitorTransitions(IGeofenceHardware::GEOFENCE_UNCERTAIN |
        IGeofenceHardware::GEOFENCE_ENTERED | IGeofenceHardware::GEOFENCE_EXITED)
    , mNotificationResponsiveness(5000) // 5 secs
    , mSourceTechnologies(IGeofenceHardware::SOURCE_TECHNOLOGY_GNSS)
{
}

void GeofenceHardwareRequest::SetCircularGeofence(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Double radius)
{
    mLatitude = latitude;
    mLongitude = longitude;
    mRadius = radius;
    mType  = GEOFENCE_TYPE_CIRCLE;
}

ECode GeofenceHardwareRequest::CreateCircularGeofence(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Double radius,
    /* [out] */ IGeofenceHardwareRequest** geofence)
{
    AutoPtr<GeofenceHardwareRequest> geofenceRequest = new GeofenceHardwareRequest();
    geofenceRequest->SetCircularGeofence(latitude, longitude, radius);
    *geofence = geofenceRequest.Get();
    REFCOUNT_ADD(*geofence);
    return NOERROR;
}

ECode GeofenceHardwareRequest::SetLastTransition(
    /* [in] */ Int32 lastTransition)
{
    mLastTransition = lastTransition;
    return NOERROR;
}

ECode GeofenceHardwareRequest::SetUnknownTimer(
    /* [in] */ Int32 unknownTimer)
{
    mUnknownTimer = unknownTimer;
    return NOERROR;
}

ECode GeofenceHardwareRequest::SetMonitorTransitions(
    /* [in] */ Int32 monitorTransitions)
{
    mMonitorTransitions = monitorTransitions;
    return NOERROR;
}

ECode GeofenceHardwareRequest::SetNotificationResponsiveness(
    /* [in] */ Int32 notificationResponsiveness)
{
    mNotificationResponsiveness = notificationResponsiveness;
    return NOERROR;
}

ECode GeofenceHardwareRequest::SetSourceTechnologies(
    /* [in] */ Int32 sourceTechnologies)
{
    Int32 sourceTechnologiesAll = IGeofenceHardware::SOURCE_TECHNOLOGY_GNSS
            | IGeofenceHardware::SOURCE_TECHNOLOGY_WIFI
            | IGeofenceHardware::SOURCE_TECHNOLOGY_SENSORS
            | IGeofenceHardware::SOURCE_TECHNOLOGY_CELL
            | IGeofenceHardware::SOURCE_TECHNOLOGY_BLUETOOTH;

    Int32 sanitizedSourceTechnologies = (sourceTechnologies & sourceTechnologiesAll);
    if (sanitizedSourceTechnologies == 0) {
        //throw new IllegalArgumentException("At least one valid source technology must be set.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mSourceTechnologies = sanitizedSourceTechnologies;
    return NOERROR;
}

ECode GeofenceHardwareRequest::GetLatitude(
    /* [out] */ Double* latitude)
{
    VALIDATE_NOT_NULL(latitude);

    *latitude = mLatitude;
    return NOERROR;
}

ECode GeofenceHardwareRequest::GetLongitude(
    /* [out] */ Double* longitude)
{
    VALIDATE_NOT_NULL(longitude);

    *longitude = mLongitude;
    return NOERROR;
}

ECode GeofenceHardwareRequest::GetRadius(
    /* [out] */ Double* radius)
{
    VALIDATE_NOT_NULL(radius);

    *radius = mRadius;
    return NOERROR;
}

ECode GeofenceHardwareRequest::GetMonitorTransitions(
    /* [out] */ Int32* transitions)
{
    VALIDATE_NOT_NULL(transitions);

    *transitions = mMonitorTransitions;
    return NOERROR;
}

ECode GeofenceHardwareRequest::GetUnknownTimer(
    /* [out] */ Int32* timer)
{
    VALIDATE_NOT_NULL(timer);

    *timer = mUnknownTimer;
    return NOERROR;
}

ECode GeofenceHardwareRequest::GetNotificationResponsiveness(
    /* [out] */ Int32* responsiveness)
{
    VALIDATE_NOT_NULL(responsiveness);

    *responsiveness = mNotificationResponsiveness;
    return NOERROR;
}

ECode GeofenceHardwareRequest::GetLastTransition(
    /* [out] */ Int32* transition)
{
    VALIDATE_NOT_NULL(transition);

    *transition = mLastTransition;
    return NOERROR;
}

ECode GeofenceHardwareRequest::GetSourceTechnologies(
    /* [out] */ Int32* technologies)
{
    VALIDATE_NOT_NULL(technologies);

    *technologies = mSourceTechnologies;
    return NOERROR;
}

ECode GeofenceHardwareRequest::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    *type = mType;
    return NOERROR;
}

} //Location
} //Hardware
} //Droid
} //Elastos