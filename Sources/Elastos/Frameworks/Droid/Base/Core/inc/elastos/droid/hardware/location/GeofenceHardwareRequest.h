//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWAREREQUEST_H__
#define __ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWAREREQUEST_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

class GeofenceHardwareRequest
    : public Object
    , public IGeofenceHardwareRequest
{
public:
    CAR_INTERFACE_DECL()

    GeofenceHardwareRequest();

    /**
     * Create a circular geofence.
     *
     * @param latitude Latitude of the geofence
     * @param longitude Longitude of the geofence
     * @param radius Radius of the geofence (in meters)
     */
    static CARAPI CreateCircularGeofence(
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Double radius,
        /* [out] */ IGeofenceHardwareRequest** geofence);

    /**
     * Set the last known transition of the geofence.
     *
     * @param lastTransition The current state of the geofence. Can be one of
     *        {@link GeofenceHardware#GEOFENCE_ENTERED}, {@link GeofenceHardware#GEOFENCE_EXITED},
     *        {@link GeofenceHardware#GEOFENCE_UNCERTAIN}.
     */
    CARAPI SetLastTransition(
        /* [in] */ Int32 lastTransition);

    /**
     * Set the unknown timer for this geofence.
     *
     * @param unknownTimer  The time limit after which the
     *        {@link GeofenceHardware#GEOFENCE_UNCERTAIN} transition
     *        should be triggered. This paramter is defined in milliseconds.
     */
    CARAPI SetUnknownTimer(
        /* [in] */ Int32 unknownTimer);

    /**
     * Set the transitions to be monitored.
     *
     * @param monitorTransitions Bitwise OR of {@link GeofenceHardware#GEOFENCE_ENTERED},
     *        {@link GeofenceHardware#GEOFENCE_EXITED}, {@link GeofenceHardware#GEOFENCE_UNCERTAIN}
     */
    CARAPI SetMonitorTransitions(
        /* [in] */ Int32 monitorTransitions);

    /**
     * Set the notification responsiveness of the geofence.
     *
     * @param notificationResponsiveness (milliseconds) Defines the best-effort description
     *        of how soon should the callback be called when the transition
     *        associated with the Geofence is triggered. For instance, if
     *        set to 1000 millseconds with {@link GeofenceHardware#GEOFENCE_ENTERED},
     *        the callback will be called 1000 milliseconds within entering
     *        the geofence.
     */
    CARAPI SetNotificationResponsiveness(
        /* [in] */ Int32 notificationResponsiveness);

    /**
     * Set the source technologies to use while tracking the geofence.
     * The value is the bit-wise of one or several source fields defined in
     * {@link GeofenceHardware}.
     *
     * @param sourceTechnologies The set of source technologies to use.
     */
    CARAPI SetSourceTechnologies(
        /* [in] */ Int32 sourceTechnologies);

    /**
     * Returns the latitude of this geofence.
     */
    CARAPI GetLatitude(
        /* [out] */ Double* latitude);

    /**
     * Returns the longitude of this geofence.
     */
    CARAPI GetLongitude(
        /* [out] */ Double* longitude);

    /**
     * Returns the radius of this geofence.
     */
    CARAPI GetRadius(
        /* [out] */ Double* radius);

    /**
     * Returns transitions monitored for this geofence.
     */
    CARAPI GetMonitorTransitions(
        /* [out] */ Int32* transitions);

    /**
     * Returns the unknownTimer of this geofence.
     */
    CARAPI GetUnknownTimer(
        /* [out] */ Int32* timer);

    /**
     * Returns the notification responsiveness of this geofence.
     */
    CARAPI GetNotificationResponsiveness(
        /* [out] */ Int32* responsiveness);

    /**
     * Returns the last transition of this geofence.
     */
    CARAPI GetLastTransition(
        /* [out] */ Int32* transition);

    /**
     * Returns the source technologies to track this geofence.
     */
    CARAPI GetSourceTechnologies(
        /* [out] */ Int32* technologies);

    CARAPI GetType(
        /* [out] */ Int32* type);

private:
    CARAPI_(void) SetCircularGeofence(
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Double radius);

protected:
    static const Int32 GEOFENCE_TYPE_CIRCLE;

private:
    friend class GeofenceHardware;
    friend class GeofenceHardwareRequestParcelable;

    Int32 mType;
    Double mLatitude;
    Double mLongitude;
    Double mRadius;
    Int32 mLastTransition;
    Int32 mUnknownTimer;
    Int32 mMonitorTransitions;
    Int32 mNotificationResponsiveness;
    Int32 mSourceTechnologies;
};

} //Location
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWAREREQUEST_H__
