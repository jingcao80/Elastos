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

#include "elastos/droid/location/Criteria.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL_2(Criteria, Object, ICriteria, IParcelable)

Criteria::Criteria()
    : mHorizontalAccuracy(Criteria_NO_REQUIREMENT)
    , mVerticalAccuracy(Criteria_NO_REQUIREMENT)
    , mSpeedAccuracy(Criteria_NO_REQUIREMENT)
    , mBearingAccuracy(Criteria_NO_REQUIREMENT)
    , mPowerRequirement(Criteria_NO_REQUIREMENT)
    , mAltitudeRequired(FALSE)
    , mBearingRequired(FALSE)
    , mSpeedRequired(FALSE)
    , mCostAllowed(FALSE)
{
}

ECode Criteria::constructor()
{
    return NOERROR;
}

ECode Criteria::constructor(
    /* [in] */ ICriteria* criteria)
{
    criteria->GetHorizontalAccuracy(&mHorizontalAccuracy);
    criteria->GetVerticalAccuracy(&mVerticalAccuracy);
    criteria->GetSpeedAccuracy(&mSpeedAccuracy);
    criteria->GetBearingAccuracy(&mBearingAccuracy);
    criteria->GetPowerRequirement(&mPowerRequirement);
    criteria->IsAltitudeRequired(&mAltitudeRequired);
    criteria->IsBearingRequired(&mBearingRequired);
    criteria->IsSpeedRequired(&mSpeedRequired);
    criteria->IsCostAllowed(&mCostAllowed);
    return NOERROR;
}

ECode Criteria::SetHorizontalAccuracy(
    /* [in] */ Int32 accuracy)
{
    if (accuracy < Criteria_NO_REQUIREMENT || accuracy > Criteria_ACCURACY_HIGH) {
        Logger::E("Criteria", "accuracy=%d", accuracy);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mHorizontalAccuracy = accuracy;
    return NOERROR;
}

ECode Criteria::GetHorizontalAccuracy(
    /* [out] */ Int32* accuracy)
{
    VALIDATE_NOT_NULL(accuracy);
    *accuracy = mHorizontalAccuracy;
    return NOERROR;
}

ECode Criteria::SetVerticalAccuracy(
    /* [in] */ Int32 accuracy)
{
    if (accuracy < Criteria_NO_REQUIREMENT || accuracy > Criteria_ACCURACY_HIGH) {
        Logger::E("Criteria", "accuracy=%d", accuracy);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mVerticalAccuracy = accuracy;
    return NOERROR;
}

ECode Criteria::GetVerticalAccuracy(
    /* [out] */ Int32* accuracy)
{
    VALIDATE_NOT_NULL(accuracy);
    *accuracy = mVerticalAccuracy;
    return NOERROR;
}

ECode Criteria::SetSpeedAccuracy(
    /* [in] */ Int32 accuracy)
{
    if (accuracy < Criteria_NO_REQUIREMENT || accuracy > Criteria_ACCURACY_HIGH) {
        Logger::E("Criteria", "accuracy=%d", accuracy);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSpeedAccuracy = accuracy;
    return NOERROR;
}

ECode Criteria::GetSpeedAccuracy(
    /* [out] */ Int32* accuracy)
{
    VALIDATE_NOT_NULL(accuracy);
    *accuracy = mSpeedAccuracy;
    return NOERROR;
}

ECode Criteria::SetBearingAccuracy(
    /* [in] */ Int32 accuracy)
{
    if (accuracy < Criteria_NO_REQUIREMENT || accuracy > Criteria_ACCURACY_HIGH) {
        Logger::E("Criteria", "accuracy=%d", accuracy);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mBearingAccuracy = accuracy;
    return NOERROR;
}

ECode Criteria::GetBearingAccuracy(
    /* [out] */ Int32* accuracy)
{
    VALIDATE_NOT_NULL(accuracy);
    *accuracy = mBearingAccuracy;
    return NOERROR;
}

ECode Criteria::SetAccuracy(
    /* [in] */ Int32 accuracy)
{
    if (accuracy < Criteria_NO_REQUIREMENT || accuracy > Criteria_ACCURACY_COARSE) {
        Logger::E("Criteria", "accuracy=%d", accuracy);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (accuracy == Criteria_ACCURACY_FINE) {
        mHorizontalAccuracy = Criteria_ACCURACY_HIGH;
    }
    else {
        mHorizontalAccuracy = Criteria_ACCURACY_LOW;
    }
    return NOERROR;
}

ECode Criteria::GetAccuracy(
    /* [out] */ Int32* accuracy)
{
    VALIDATE_NOT_NULL(accuracy);

    if (mHorizontalAccuracy >= Criteria_ACCURACY_HIGH) {
        *accuracy = Criteria_ACCURACY_FINE;
    }
    else {
        *accuracy = Criteria_ACCURACY_COARSE;
    }
    return NOERROR;
}

ECode Criteria::SetPowerRequirement(
    /* [in] */ Int32 level)
{
    if (level < Criteria_NO_REQUIREMENT || level > Criteria_POWER_HIGH) {
        Logger::E("Criteria", "level=%d", level);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mPowerRequirement = level;
    return NOERROR;
}

ECode Criteria::GetPowerRequirement(
    /* [out] */ Int32* requirement)
{
    VALIDATE_NOT_NULL(requirement);
    *requirement = mPowerRequirement;
    return NOERROR;
}

ECode Criteria::SetCostAllowed(
    /* [in] */ Boolean costAllowed)
{
    mCostAllowed = costAllowed;
    return NOERROR;
}

ECode Criteria::IsCostAllowed(
    /* [out] */ Boolean* costAllowed)
{
    VALIDATE_NOT_NULL(costAllowed);
    *costAllowed = mCostAllowed;
    return NOERROR;
}

ECode Criteria::SetAltitudeRequired(
    /* [in] */ Boolean altitudeRequired)
{
    mAltitudeRequired = altitudeRequired;
    return NOERROR;
}

ECode Criteria::IsAltitudeRequired(
    /* [out] */ Boolean* altitudeRequired)
{
    VALIDATE_NOT_NULL(altitudeRequired);
    *altitudeRequired = mAltitudeRequired;
    return NOERROR;
}

ECode Criteria::SetSpeedRequired(
    /* [in] */ Boolean speedRequired)
{
    mSpeedRequired = speedRequired;
    return NOERROR;
}

ECode Criteria::IsSpeedRequired(
    /* [out] */ Boolean* speedRequired)
{
    VALIDATE_NOT_NULL(speedRequired);
    *speedRequired = mSpeedRequired;
    return NOERROR;
}

ECode Criteria::SetBearingRequired(
    /* [in] */ Boolean bearingRequired)
{
    mBearingRequired = bearingRequired;
    return NOERROR;
}

ECode Criteria::IsBearingRequired(
    /* [out] */ Boolean* bearingRequired)
{
    VALIDATE_NOT_NULL(bearingRequired);
    *bearingRequired = mBearingRequired;
    return NOERROR;
}

ECode Criteria::ReadFromParcel(
    /* [in] */ IParcel* source)
{

    source->ReadInt32(&mHorizontalAccuracy);
    source->ReadInt32(&mVerticalAccuracy);
    source->ReadInt32(&mSpeedAccuracy);
    source->ReadInt32(&mBearingAccuracy);
    source->ReadInt32(&mPowerRequirement);
    return NOERROR;
}

ECode Criteria::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mHorizontalAccuracy);
    dest->WriteInt32(mVerticalAccuracy);
    dest->WriteInt32(mSpeedAccuracy);
    dest->WriteInt32(mBearingAccuracy);
    dest->WriteInt32(mPowerRequirement);
    dest->WriteInt32(mAltitudeRequired ? 1 : 0);
    dest->WriteInt32(mBearingRequired ? 1 : 0);
    dest->WriteInt32(mSpeedRequired ? 1 : 0);
    dest->WriteInt32(mCostAllowed ? 1 : 0);
    return NOERROR;
}

ECode Criteria::PowerToString(
    /* [in] */ Int32 power,
    /* [out] */ String* strOut)
{
    VALIDATE_NOT_NULL(strOut);

    switch (power) {
        case Criteria_NO_REQUIREMENT:
            {
                *strOut = "NO_REQ";
                return NOERROR;
            }
        case Criteria_POWER_LOW:
            {
                *strOut = "LOW";
                return NOERROR;
            }
        case Criteria_POWER_MEDIUM:
            {
                *strOut = "MEDIUM";
                return NOERROR;
            }
        case Criteria_POWER_HIGH:
            {
                *strOut = "HIGH";
                return NOERROR;
            }
        default:
            {
                *strOut = "???";
                return NOERROR;
            }
    }
}

ECode Criteria::AccuracyToString(
    /* [in] */ Int32 accuracy,
    /* [out] */ String* strOut)
{
    VALIDATE_NOT_NULL(strOut);
    switch (accuracy) {
        case Criteria_NO_REQUIREMENT:
            {
                *strOut = "---";
                return NOERROR;
            }
        case Criteria_ACCURACY_HIGH:
            {
                *strOut = "HIGH";
                return NOERROR;
            }
        case Criteria_ACCURACY_MEDIUM:
            {
                *strOut = "MEDIUM";
                return NOERROR;
            }
        case Criteria_ACCURACY_LOW:
            {
                *strOut = "LOW";
                return NOERROR;
            }
        default:
            {
                *strOut = "???";
                return NOERROR;
            }
    }
}

ECode Criteria::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)
    StringBuilder s("Criteria[power=");
    String str;
    PowerToString(mPowerRequirement, &str);
    s += str;
    s += " acc=";
    AccuracyToString(mHorizontalAccuracy, &str);
    s += str;
    s += ']';
    *info = s.ToString();
    return NOERROR;
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
