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

#ifndef __ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWAREREQUESTPARCELABLE_H__
#define __ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWAREREQUESTPARCELABLE_H__

#include "elastos/droid/hardware/location/GeofenceHardwareRequest.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Location::IGeofenceHardwareRequest;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

class GeofenceHardwareRequestParcelable
    : public Object
    , public IGeofenceHardwareRequestParcelable
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    GeofenceHardwareRequestParcelable();

    virtual ~GeofenceHardwareRequestParcelable();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ GeofenceHardwareRequest* request);

    /**
     * Returns the id of this request.
     */
    CARAPI GetId(
        /* [out] */ Int32* id);

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
     * Returns the type of the geofence for the current request.
     */
    CARAPI GetType(
        /* [out] */ Int32* type);

    /**
     * Returns the source technologies to track this geofence.
     */
    CARAPI GetSourceTechnologies(
        /* [out] */ Int32* technologies);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Method definitions to support Parcelable operations.
     */
    // public static final Parcelable.Creator<GeofenceHardwareRequestParcelable> CREATOR =
    //         new Parcelable.Creator<GeofenceHardwareRequestParcelable>() {
    //     @Override
    //     public GeofenceHardwareRequestParcelable createFromParcel(Parcel parcel) {
    //         int geofenceType = parcel.readInt();
    //         if(geofenceType != GeofenceHardwareRequest.GEOFENCE_TYPE_CIRCLE) {
    //             Log.e(
    //                     "GeofenceHardwareRequest",
    //                     String.format("Invalid Geofence type: %d", geofenceType));
    //             return null;
    //         }

    //         GeofenceHardwareRequest request = GeofenceHardwareRequest.createCircularGeofence(
    //                 parcel.readDouble(),
    //                 parcel.readDouble(),
    //                 parcel.readDouble());
    //         request.setLastTransition(parcel.readInt());
    //         request.setMonitorTransitions(parcel.readInt());
    //         request.setUnknownTimer(parcel.readInt());
    //         request.setNotificationResponsiveness(parcel.readInt());
    //         request.setSourceTechnologies(parcel.readInt());

    //         int id = parcel.readInt();
    //         return new GeofenceHardwareRequestParcelable(id, request);
    //     }

    //     @Override
    //     public GeofenceHardwareRequestParcelable[] newArray(int size) {
    //         return new GeofenceHardwareRequestParcelable[size];
    //     }
    // };

private:
    AutoPtr<GeofenceHardwareRequest> mRequest;

    Int32 mId;
};

} //Location
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWAREREQUESTPARCELABLE_H__
