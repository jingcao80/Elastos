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

#include "elastos/droid/internal/location/CProviderProperties.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Location {

CAR_OBJECT_IMPL(CProviderProperties);

CAR_INTERFACE_IMPL_2(CProviderProperties, Object, IProviderProperties, IParcelable);

CProviderProperties::CProviderProperties()
     : mRequiresNetwork(FALSE)
     , mRequiresSatellite(FALSE)
     , mRequiresCell(FALSE)
     , mHasMonetaryCost(FALSE)
     , mSupportsAltitude(FALSE)
     , mSupportsSpeed(FALSE)
     , mSupportsBearing(FALSE)
     , mPowerRequirement(0)
     , mAccuracy(0)
{}

ECode CProviderProperties::constructor()
{
    return NOERROR;
}

ECode CProviderProperties::constructor(
    /* [in] */ Boolean requiresNetwork,
    /* [in] */ Boolean requiresSatellite,
    /* [in] */ Boolean requiresCell,
    /* [in] */ Boolean hasMonetaryCost,
    /* [in] */ Boolean supportsAltitude,
    /* [in] */ Boolean supportsSpeed,
    /* [in] */ Boolean supportsBearing,
    /* [in] */ Int32 powerRequirement,
    /* [in] */ Int32 accuracy)
{
    mRequiresNetwork = requiresNetwork;
    mRequiresSatellite = requiresSatellite;
    mRequiresCell = requiresCell;
    mHasMonetaryCost = hasMonetaryCost;
    mSupportsAltitude = supportsAltitude;
    mSupportsSpeed = supportsSpeed;
    mSupportsBearing = supportsBearing;
    mPowerRequirement = powerRequirement;
    mAccuracy = accuracy;

    return NOERROR;
}

//@Override
ECode CProviderProperties::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);
    parcel->WriteBoolean(mRequiresNetwork);
    parcel->WriteBoolean(mRequiresSatellite);
    parcel->WriteBoolean(mRequiresCell);
    parcel->WriteBoolean(mHasMonetaryCost);
    parcel->WriteBoolean(mSupportsAltitude);
    parcel->WriteBoolean(mSupportsSpeed);
    parcel->WriteBoolean(mSupportsBearing);
    parcel->WriteInt32(mPowerRequirement);
    parcel->WriteInt32(mAccuracy);

    return NOERROR;
}

ECode CProviderProperties::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);

    parcel->ReadBoolean(&mRequiresNetwork);
    parcel->ReadBoolean(&mRequiresSatellite);
    parcel->ReadBoolean(&mRequiresCell);
    parcel->ReadBoolean(&mHasMonetaryCost);
    parcel->ReadBoolean(&mSupportsAltitude);
    parcel->ReadBoolean(&mSupportsSpeed);
    parcel->ReadBoolean(&mSupportsBearing);
    parcel->ReadInt32(&mPowerRequirement);
    parcel->ReadInt32(&mAccuracy);

    return NOERROR;
}

ECode CProviderProperties::SetRequiresNetwork(
    /* [in] */ Boolean network)
{
    mRequiresNetwork = network;
    return NOERROR;
}

ECode CProviderProperties::GetRequiresNetwork(
    /* [out] */ Boolean* network)
{
    VALIDATE_NOT_NULL(network);
    *network = mRequiresNetwork;
    return NOERROR;
}

ECode CProviderProperties::SetRequiresSatellite(
    /* [in] */ Boolean satellite)
{
    mRequiresSatellite = satellite;
    return NOERROR;
}

ECode CProviderProperties::GetRequiresSatellite(
    /* [out] */ Boolean* satellite)
{
    VALIDATE_NOT_NULL(satellite);
    *satellite = mRequiresSatellite;
    return NOERROR;
}

ECode CProviderProperties::SetRequiresCell(
    /* [in] */ Boolean cell)
{
    mRequiresCell = cell;
    return NOERROR;
}

ECode CProviderProperties::GetRequiresCell(
    /* [out] */ Boolean* cell)
{
    VALIDATE_NOT_NULL(cell);
    *cell = mRequiresCell;
    return NOERROR;
}

ECode CProviderProperties::SetHasMonetaryCost(
    /* [in] */ Boolean hasMonetaryCost)
{
    mHasMonetaryCost = hasMonetaryCost;
    return NOERROR;
}

ECode CProviderProperties::GetHasMonetaryCost(
    /* [out] */ Boolean* hasMonetaryCost)
{
    VALIDATE_NOT_NULL(hasMonetaryCost);
    *hasMonetaryCost = mHasMonetaryCost;
    return NOERROR;
}

ECode CProviderProperties::SetSupportsAltitude(
    /* [in] */ Boolean supportsAltitude)
{
    mSupportsAltitude = supportsAltitude;
    return NOERROR;
}

ECode CProviderProperties::GetSupportsAltitude(
    /* [out] */ Boolean* supportsAltitude)
{
    VALIDATE_NOT_NULL(supportsAltitude);
    *supportsAltitude = mSupportsAltitude;
    return NOERROR;
}

ECode CProviderProperties::SetSupportsSpeed(
    /* [in] */ Boolean supportsSpeed)
{
    mSupportsSpeed = supportsSpeed;
    return NOERROR;
}

ECode CProviderProperties::GetSupportsSpeed(
    /* [out] */ Boolean* supportsSpeed)
{
    VALIDATE_NOT_NULL(supportsSpeed);
    *supportsSpeed = mSupportsSpeed;
    return NOERROR;
}

ECode CProviderProperties::SetSupportsBearing(
    /* [in] */ Boolean supportsBearing)
{
    mSupportsBearing = supportsBearing;
    return NOERROR;
}

ECode CProviderProperties::GetSupportsBearing(
    /* [out] */ Boolean* supportsBearing)
{
    VALIDATE_NOT_NULL(supportsBearing);
    *supportsBearing = mSupportsBearing;
    return NOERROR;
}

ECode CProviderProperties::SetPowerRequirement(
    /* [in] */ Int32 powerRequirement)
{
    mPowerRequirement = powerRequirement;
    return NOERROR;
}

ECode CProviderProperties::GetPowerRequirement(
    /* [out] */ Int32* powerRequirement)
{
    VALIDATE_NOT_NULL(powerRequirement);
    *powerRequirement = mPowerRequirement;
    return NOERROR;
}

ECode CProviderProperties::SetAccuracy(
    /* [in] */ Int32 accuracy)
{
    mAccuracy = accuracy;
    return NOERROR;
}

ECode CProviderProperties::GetAccuracy(
    /* [out] */ Int32* accuracy)
{
    VALIDATE_NOT_NULL(accuracy);
    *accuracy = mAccuracy;
    return NOERROR;
}

} // namespace Location
} // namespace Internal
} // namespace Droid
} // namespace Elastos
