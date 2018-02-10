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

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include "elastos/droid/hardware/location/GeofenceHardwareMonitorEvent.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

CAR_INTERFACE_IMPL_2(GeofenceHardwareMonitorEvent, Object, IGeofenceHardwareMonitorEvent, IParcelable)

GeofenceHardwareMonitorEvent::GeofenceHardwareMonitorEvent()
    : mMonitoringType(0)
    , mMonitoringStatus(0)
    , mSourceTechnologies(0)
{
}

GeofenceHardwareMonitorEvent::~GeofenceHardwareMonitorEvent()
{
}

ECode GeofenceHardwareMonitorEvent::constructor()
{
    return NOERROR;
}

ECode GeofenceHardwareMonitorEvent::constructor(
    /* [in] */ Int32 monitoringType,
    /* [in] */ Int32 monitoringStatus,
    /* [in] */ Int32 sourceTechnologies,
    /* [in] */ ILocation* location)
{
    mMonitoringType = monitoringType;
    mMonitoringStatus = monitoringStatus;
    mSourceTechnologies = sourceTechnologies;
    mLocation = location;
    return NOERROR;
}

ECode GeofenceHardwareMonitorEvent::GetMonitoringType(
        /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    *type = mMonitoringType;
    return NOERROR;
}

ECode GeofenceHardwareMonitorEvent::GetMonitoringStatus(
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);

    *status = mMonitoringStatus;
    return NOERROR;
}

ECode GeofenceHardwareMonitorEvent::GetSourceTechnologies(
    /* [out] */ Int32* technologies)
{
    VALIDATE_NOT_NULL(technologies);

    *technologies = mSourceTechnologies;
    return NOERROR;
}

ECode GeofenceHardwareMonitorEvent::GetLocation(
    /* [out] */ ILocation** location)
{
    VALIDATE_NOT_NULL(location);

    *location = mLocation;
    REFCOUNT_ADD(*location);
    return NOERROR;
}

ECode GeofenceHardwareMonitorEvent::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mMonitoringType);
    dest->WriteInt32(mMonitoringStatus);
    dest->WriteInt32(mSourceTechnologies);
    dest->WriteInterfacePtr(mLocation);
    return NOERROR;
}

ECode GeofenceHardwareMonitorEvent::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mMonitoringType);
    source->ReadInt32(&mMonitoringStatus);
    source->ReadInt32(&mSourceTechnologies);
    source->ReadInterfacePtr((HANDLE*)&mLocation);
    return NOERROR;
}

ECode GeofenceHardwareMonitorEvent::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder builder;
    builder += "GeofenceHardwareMonitorEvent: type=";
    builder += mMonitoringType;
    builder += ", status=";
    builder += mMonitoringStatus;
    builder += ", sources=";
    builder += mSourceTechnologies;
    builder += ", location=";
    builder += Object::ToString(mLocation);

    return builder.ToString(str);
}

} //Location
} //Hardware
} //Droid
} //Elastos