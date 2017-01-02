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

#include "elastos/droid/location/GpsClock.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

const String GpsClock::TAG("GpsClock");

const Byte GpsClock::TYPE_UNKNOWN = 0;

const Byte GpsClock::TYPE_LOCAL_HW_TIME = 1;

const Byte GpsClock::TYPE_GPS_TIME = 2;

const Int16 GpsClock::HAS_NO_FLAGS = 0;
const Int16 GpsClock::HAS_LEAP_SECOND = 1 << 0;
const Int16 GpsClock::HAS_TIME_UNCERTAINTY = 1 << 1;
const Int16 GpsClock::HAS_FULL_BIAS = 1 << 2;
const Int16 GpsClock::HAS_BIAS = 1 << 3;
const Int16 GpsClock::HAS_BIAS_UNCERTAINTY = 1 << 4;
const Int16 GpsClock::HAS_DRIFT = 1 << 5;
const Int16 GpsClock::HAS_DRIFT_UNCERTAINTY = 1 << 6;

CAR_INTERFACE_IMPL_2(GpsClock, Object, IGpsClock, IParcelable)

GpsClock::GpsClock()
{
    Initialize();
}

ECode GpsClock::constructor()
{
    return NOERROR;
}

ECode GpsClock::Set(
    /* [in] */ IGpsClock* clock)
{
    clock->GetFlag(&mFlags);
    clock->GetLeapSecond(&mLeapSecond);
    clock->GetType(&mType);
    clock->GetTimeInNs(&mTimeInNs);
    clock->GetTimeUncertaintyInNs(&mTimeUncertaintyInNs);
    clock->GetFullBiasInNs(&mFullBiasInNs);
    clock->GetBiasInNs(&mBiasInNs);
    clock->GetBiasUncertaintyInNs(&mBiasUncertaintyInNs);
    clock->GetDriftInNsPerSec(&mDriftInNsPerSec);
    clock->GetDriftUncertaintyInNsPerSec(&mDriftUncertaintyInNsPerSec);
    return NOERROR;
}

ECode GpsClock::Reset()
{
    Initialize();
    return NOERROR;
}

ECode GpsClock::GetType(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mType;
    return NOERROR;
}

ECode GpsClock::SetType(
    /* [in] */ Byte value)
{
    switch (value) {
        case IGpsClock::TYPE_UNKNOWN:
        case IGpsClock::TYPE_GPS_TIME:
        case IGpsClock::TYPE_LOCAL_HW_TIME:
            mType = value;
            break;
        default:
            StringBuilder sb("Sanitizing invalid 'type': ");
            sb += value;
            Logger::D(TAG, sb.ToString());
            mType = IGpsClock::TYPE_UNKNOWN;
            break;
    }
    return NOERROR;
}

String GpsClock::GetTypeString()
{
    switch (mType) {
        case IGpsClock::TYPE_UNKNOWN:
            return String("Unknown");
        case IGpsClock::TYPE_GPS_TIME:
            return String("GpsTime");
        case IGpsClock::TYPE_LOCAL_HW_TIME:
            return String("LocalHwClock");
        default:
            return String("<Invalid>");
    }
}

ECode GpsClock::HasLeapSecond(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_LEAP_SECOND);
    return NOERROR;
}

ECode GpsClock::GetLeapSecond(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mLeapSecond;
    return NOERROR;
}

ECode GpsClock::SetLeapSecond(
    /* [in] */ Int16 leapSecond)
{
    SetFlag(HAS_LEAP_SECOND);
    return NOERROR;
}

ECode GpsClock::ResetLeapSecond()
{
    ResetFlag(HAS_LEAP_SECOND);
    mLeapSecond = Elastos::Core::Math::INT16_MIN_VALUE;
    return NOERROR;
}

ECode GpsClock::GetTimeInNs(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTimeInNs;
    return NOERROR;
}

ECode GpsClock::SetTimeInNs(
    /* [in] */ Int64 timeInNs)
{
    mTimeInNs = timeInNs;
    return NOERROR;
}

ECode GpsClock::HasTimeUncertaintyInNs(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_TIME_UNCERTAINTY);
    return NOERROR;
}

ECode GpsClock::GetTimeUncertaintyInNs(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTimeUncertaintyInNs;
    return NOERROR;
}

ECode GpsClock::SetTimeUncertaintyInNs(
    /* [in] */ Double timeUncertaintyInNs)
{
    SetFlag(HAS_TIME_UNCERTAINTY);
    mTimeUncertaintyInNs = timeUncertaintyInNs;
    return NOERROR;
}

ECode GpsClock::ResetTimeUncertaintyInNs()
{
    ResetFlag(HAS_TIME_UNCERTAINTY);
    mTimeUncertaintyInNs = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsClock::HasFullBiasInNs(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_FULL_BIAS);
    return NOERROR;
}

ECode GpsClock::GetFullBiasInNs(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFullBiasInNs;
    return NOERROR;
}

ECode GpsClock::SetFullBiasInNs(
    /* [in] */ Int64 value)
{
    SetFlag(HAS_FULL_BIAS);
    mFullBiasInNs = value;
    return NOERROR;
}

ECode GpsClock::ResetFullBiasInNs()
{
    ResetFlag(HAS_FULL_BIAS);
    mFullBiasInNs = Elastos::Core::Math::INT64_MIN_VALUE;
    return NOERROR;
}

ECode GpsClock::HasBiasInNs(
    /* [out] */ Boolean* result)
{
    *result = IsFlagSet(HAS_BIAS);
    return NOERROR;
}

ECode GpsClock::GetBiasInNs(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBiasInNs;
    return NOERROR;
}

ECode GpsClock::SetBiasInNs(
    /* [in] */ Double biasInNs)
{
    SetFlag(HAS_BIAS);
    mBiasInNs = biasInNs;
    return NOERROR;
}

ECode GpsClock::ResetBiasInNs()
{
    ResetFlag(HAS_BIAS);
    mBiasInNs = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsClock::HasBiasUncertaintyInNs(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_BIAS_UNCERTAINTY);
    return NOERROR;
}

ECode GpsClock::GetBiasUncertaintyInNs(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBiasUncertaintyInNs;
    return NOERROR;
}

ECode GpsClock::SetBiasUncertaintyInNs(
    /* [in] */ Double biasUncertaintyInNs)
{
    SetFlag(HAS_BIAS_UNCERTAINTY);
    mBiasUncertaintyInNs = biasUncertaintyInNs;
    return NOERROR;
}

ECode GpsClock::ResetBiasUncertaintyInNs()
{
    ResetFlag(HAS_BIAS_UNCERTAINTY);
    mBiasUncertaintyInNs = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsClock::HasDriftInNsPerSec(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_DRIFT);
    return NOERROR;
}

ECode GpsClock::GetDriftInNsPerSec(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDriftInNsPerSec;
    return NOERROR;
}

ECode GpsClock::SetDriftInNsPerSec(
    /* [in] */ Double driftInNsPerSec)
{
    SetFlag(HAS_DRIFT);
    mDriftInNsPerSec = driftInNsPerSec;
    return NOERROR;
}

ECode GpsClock::ResetDriftInNsPerSec()
{
    ResetFlag(HAS_DRIFT);
    mDriftInNsPerSec = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsClock::HasDriftUncertaintyInNsPerSec(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_DRIFT_UNCERTAINTY);
    return NOERROR;
}

ECode GpsClock::GetDriftUncertaintyInNsPerSec(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDriftUncertaintyInNsPerSec;
    return NOERROR;
}

ECode GpsClock::SetDriftUncertaintyInNsPerSec(
    /* [in] */ Double driftUncertaintyInNsPerSec)
{
    SetFlag(HAS_DRIFT_UNCERTAINTY);
    mDriftUncertaintyInNsPerSec = driftUncertaintyInNsPerSec;
    return NOERROR;
}

ECode GpsClock::ResetDriftUncertaintyInNsPerSec()
{
    ResetFlag(HAS_DRIFT_UNCERTAINTY);
    mDriftUncertaintyInNsPerSec = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}


ECode GpsClock::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadInt16(&mFlags);
    parcel->ReadInt16(&mLeapSecond);
    parcel->ReadByte(&mType);
    parcel->ReadInt64(&mTimeInNs);
    parcel->ReadDouble(&mTimeUncertaintyInNs);
    parcel->ReadInt64(&mFullBiasInNs);
    parcel->ReadDouble(&mBiasInNs);
    parcel->ReadDouble(&mBiasUncertaintyInNs);
    parcel->ReadDouble(&mDriftInNsPerSec);
    parcel->ReadDouble(&mDriftUncertaintyInNsPerSec);
    return NOERROR;
}

ECode GpsClock::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteInt32(mFlags);
    parcel->WriteInt32(mLeapSecond);
    parcel->WriteByte(mType);
    parcel->WriteInt64(mTimeInNs);
    parcel->WriteDouble(mTimeUncertaintyInNs);
    parcel->WriteInt64(mFullBiasInNs);
    parcel->WriteDouble(mBiasInNs);
    parcel->WriteDouble(mBiasUncertaintyInNs);
    parcel->WriteDouble(mDriftInNsPerSec);
    parcel->WriteDouble(mDriftUncertaintyInNsPerSec);
    return NOERROR;
}

ECode GpsClock::GetFlag(
    /* [out] */ Int16* flag)
{
    VALIDATE_NOT_NULL(flag)
    *flag = mFlags;
    return NOERROR;
}

ECode GpsClock::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)
#if 0 //To be translated
    const String format("   %-15s = %s\n");
    const String formatWithUncertainty("   %-15s = %-25s   %-26s = %s\n");
    StringBuilder builder("GpsClock:\n");

    builder.append(String.format(format, "Type", getTypeString()));

    builder.append(String.format(format, "LeapSecond", hasLeapSecond() ? mLeapSecond : null));

    builder.append(String.format(
            formatWithUncertainty,
            "TimeInNs",
            mTimeInNs,
            "TimeUncertaintyInNs",
            hasTimeUncertaintyInNs() ? mTimeUncertaintyInNs : null));

    builder.append(String.format(
            format,
            "FullBiasInNs",
            hasFullBiasInNs() ? mFullBiasInNs : null));

    builder.append(String.format(
            formatWithUncertainty,
            "BiasInNs",
            hasBiasInNs() ? mBiasInNs : null,
            "BiasUncertaintyInNs",
            hasBiasUncertaintyInNs() ? mBiasUncertaintyInNs : null));

    builder.append(String.format(
            formatWithUncertainty,
            "DriftInNsPerSec",
            hasDriftInNsPerSec() ? mDriftInNsPerSec : null,
            "DriftUncertaintyInNsPerSec",
            hasDriftUncertaintyInNsPerSec() ? mDriftUncertaintyInNsPerSec : null));

    return builder.toString();
#endif
    return NOERROR;
}

void GpsClock::Initialize()
{
    mFlags = HAS_NO_FLAGS;
    ResetLeapSecond();
    SetType(TYPE_UNKNOWN);
    SetTimeInNs(Elastos::Core::Math::Math::INT64_MIN_VALUE);
    ResetTimeUncertaintyInNs();
    ResetBiasInNs();
    ResetBiasUncertaintyInNs();
    ResetDriftInNsPerSec();
    ResetDriftUncertaintyInNsPerSec();
}

void GpsClock::SetFlag(
    /* [in] */ Int16 flag)
{
    mFlags |= flag;
}

void GpsClock::ResetFlag(
    /* [in] */ Int16 flag)
{
    mFlags &= ~flag;
}

Boolean GpsClock::IsFlagSet(
    /* [in] */ Int16 flag)
{
    return (mFlags & flag) == flag;
}

} // namespace Location
} // namepsace Droid
} // namespace Elastos
