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

#ifndef __ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWAREMONITOREVENT_H__
#define __ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWAREMONITOREVENT_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Location::ILocation;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

class GeofenceHardwareMonitorEvent
    : public Object
    , public IGeofenceHardwareMonitorEvent
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    GeofenceHardwareMonitorEvent();

    virtual ~GeofenceHardwareMonitorEvent();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 monitoringType,
        /* [in] */ Int32 monitoringStatus,
        /* [in] */ Int32 sourceTechnologies,
        /* [in] */ ILocation* location);

    /**
     * Returns the type of the monitoring system that has a change on its state.
     */
    CARAPI GetMonitoringType(
        /* [out] */ Int32* type);

    /**
     * Returns the new status associated with the monitoring system.
     */
    CARAPI GetMonitoringStatus(
        /* [out] */ Int32* status);

    /**
     * Returns the source technologies that the status is associated to.
     */
    CARAPI GetSourceTechnologies(
        /* [out] */ Int32* technologies);

    /**
     * Returns the last known location according to the monitoring system.
     */
    CARAPI GetLocation(
        /* [out] */ ILocation** location);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    // public static final Creator<GeofenceHardwareMonitorEvent> CREATOR =
    //         new Creator<GeofenceHardwareMonitorEvent>() {
    //             @Override
    //             public GeofenceHardwareMonitorEvent createFromParcel(Parcel source) {
    //                 ClassLoader classLoader = GeofenceHardwareMonitorEvent.class.getClassLoader();
    //                 int monitoringType = source.readInt();
    //                 int monitoringStatus = source.readInt();
    //                 int sourceTechnologies = source.readInt();
    //                 Location location = source.readParcelable(classLoader);

    //                 return new GeofenceHardwareMonitorEvent(
    //                         monitoringType,
    //                         monitoringStatus,
    //                         sourceTechnologies,
    //                         location);
    //             }

    //             @Override
    //             public GeofenceHardwareMonitorEvent[] newArray(int size) {
    //                 return new GeofenceHardwareMonitorEvent[size];
    //             }
    //         };

private:
    Int32 mMonitoringType;

    Int32 mMonitoringStatus;

    Int32 mSourceTechnologies;

    AutoPtr<ILocation> mLocation;
};

} //Location
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWAREMONITOREVENT_H__
