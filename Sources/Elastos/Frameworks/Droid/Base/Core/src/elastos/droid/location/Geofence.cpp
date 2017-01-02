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

#include "elastos/droid/location/Geofence.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

const String Geofence::TAG("Geofence");

CAR_INTERFACE_IMPL_2(Geofence, Object, IGeofence, IParcelable)

Geofence::Geofence()
    : mType(0)
    , mLatitude(0.0)
    , mLongitude(0.0)
    , mRadius(0.0f)
{}

ECode Geofence::constructor()
{
    return NOERROR;
}

ECode Geofence::CreateCircle(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Float radius,
    /* [out] */ IGeofence** geofence)
{
    VALIDATE_NOT_NULL(geofence)
    AutoPtr<Geofence> geo = new Geofence(latitude, longitude, radius);
    *geofence = IGeofence::Probe(geo);
    REFCOUNT_ADD(*geofence)
    return NOERROR;
}

Geofence::Geofence(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Float radius)
    : mType(IGeofence::TYPE_HORIZONTAL_CIRCLE)
    , mLatitude(latitude)
    , mLongitude(longitude)
    , mRadius(radius)
{
    CheckRadius(radius);
    CheckLatLong(latitude, longitude);
}

ECode Geofence::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

ECode Geofence::GetLatitude(
    /* [out] */ Double* latitude)
{
    VALIDATE_NOT_NULL(latitude);
    *latitude = mLatitude;
    return NOERROR;
}

ECode Geofence::GetLongitude(
    /* [out] */ Double* longitude)
{
    VALIDATE_NOT_NULL(longitude);
    *longitude = mLongitude;
    return NOERROR;
}

ECode Geofence::GetRadius(
    /* [out] */ Float* radius)
{
    VALIDATE_NOT_NULL(radius);
    *radius = mRadius;
    return NOERROR;
}

ECode Geofence::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    Int32 type;
    Double latitude;
    Double longitude;
    Float radius;

    in->ReadInt32(&type);
    in->ReadDouble(&latitude);
    in->ReadDouble(&longitude);
    in->ReadFloat(&radius);

    CheckType(type);

    CheckRadius(radius);
    CheckLatLong(latitude, longitude);
    return NOERROR;
}

ECode Geofence::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);
    parcel->WriteInt32(mType);
    parcel->WriteDouble(mLatitude);
    parcel->WriteDouble(mLongitude);
    parcel->WriteFloat(mRadius);
    return NOERROR;
}

ECode Geofence::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst);

    if (TO_IINTERFACE(this) == IInterface::Probe(o)) {
        *rst = TRUE;
        return NOERROR;
    }
    if (o == NULL) {
        *rst = FALSE;
        return NOERROR;
    }
    if (IGeofence::Probe(o) == NULL) {
        *rst = FALSE;
        return NOERROR;
    }
    AutoPtr<IGeofence> other = IGeofence::Probe(o);
    Float other_mRadius;
    other->GetRadius(&other_mRadius);
    Double other_mLatitude, other_mLongitude;
    other->GetLatitude(&other_mLatitude);
    other->GetLongitude(&other_mLongitude);
    Int32 other_mType;
    other->GetType(&other_mType);
    if (mRadius != other_mRadius) {
        *rst = FALSE;
        return NOERROR;
    }
    if (mLatitude != other_mLatitude) {
        *rst = FALSE;
        return NOERROR;
    }
    if (mLongitude != other_mLongitude) {
        *rst = FALSE;
        return NOERROR;
    }
    if (mType != other_mType) {
        *rst = FALSE;
        return NOERROR;
    }
    *rst = TRUE;
    return NOERROR;
}

ECode Geofence::GetHashCode(
    /* [out] */ Int32* hash)
{
    using Elastos::Core::Math;

    VALIDATE_NOT_NULL(hash);
    Int32 prime = 31;
    Int32 result = 1;
    Int64 temp;
    temp = Math::DoubleToInt64Bits(mLatitude);
    result = prime * result + (Int32) (temp ^ (temp >> 32));
    temp = Math::DoubleToInt64Bits(mLongitude);
    result = prime * result + (Int32) (temp ^ (temp >> 32));
    result = prime * result + Math::FloatToInt32Bits(mRadius);
    result = prime * result + mType;
    *hash = result;
    return NOERROR;
}

ECode Geofence::ToString(
    /* [in] */ String* strOut)
{
    VALIDATE_NOT_NULL(strOut);
    StringBuilder sbc("Geofence[");
    String str;
    TypeToString(mType, &str);
    sbc += str;
    sbc += " ";
    sbc += mLatitude;
    sbc += ", ";
    sbc += mLongitude;
    sbc += " ";
    sbc += mRadius;
    sbc += "m]";
    sbc += *strOut;

    //return String.format("Geofence[%s %.6f, %.6f %.0fm]",
    //            TypeToString(mType), mLatitude, mLongitude, mRadius);

    return NOERROR;
}

ECode Geofence::CheckRadius(
    /* [in] */ Float radius)
{
    if (radius <= 0) {
        Logger::E(TAG, "invalid radius: %f", radius);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Geofence::CheckLatLong(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude)
{
    if (latitude > 90.0 || latitude < -90.0) {
        Logger::E(TAG, "invalid latitude: %d", latitude);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (longitude > 180.0 || longitude < -180.0) {
        Logger::E(TAG, "invalid longitude: %d", longitude);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Geofence::CheckType(
    /* [in] */ Int32 type)
{
    if (type != TYPE_HORIZONTAL_CIRCLE) {
        Logger::E(TAG, "invalid type: %d", type);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Geofence::TypeToString(
    /* [in] */ Int32 type,
    /* [out] */ String* result)
{
    switch (type) {
        case IGeofence::TYPE_HORIZONTAL_CIRCLE:
        {
            *result = "CIRCLE";
            return NOERROR;
        }
        default:
        {
            CheckType(type);
            *result = "";
            return NOERROR;
        }
    }
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
