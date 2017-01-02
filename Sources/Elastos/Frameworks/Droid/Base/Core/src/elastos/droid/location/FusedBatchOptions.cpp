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

#include "elastos/droid/location/FusedBatchOptions.h"

namespace Elastos {
namespace Droid {
namespace Location {

//=========================
//FusedBatchOptions::SourceTechnologies
//=========================

Int32 FusedBatchOptions::SourceTechnologies::GNSS = 1 << 0;
Int32 FusedBatchOptions::SourceTechnologies::WIFI = 1 << 1;
Int32 FusedBatchOptions::SourceTechnologies::SENSORS = 1 << 2;
Int32 FusedBatchOptions::SourceTechnologies::CELL = 1 << 3;
Int32 FusedBatchOptions::SourceTechnologies::BLUETOOTH = 1 << 4;

//====================
//FusedBatchOptions::BatchFlags
//====================

Int32 FusedBatchOptions::BatchFlags::WAKEUP_ON_FIFO_FULL = 0x0000001;
Int32 FusedBatchOptions::BatchFlags::CALLBACK_ON_LOCATION_FIX = 0x0000002;

//============
//FusedBatchOptions
//============

CAR_INTERFACE_IMPL_2(FusedBatchOptions, Object, IFusedBatchOptions, IParcelable)

FusedBatchOptions::FusedBatchOptions()
    : mPeriodInNS(0)
    , mSourcesToUse(0)
    , mFlags(0)
    , mMaxPowerAllocationInMW(0)
{}

ECode FusedBatchOptions::constructor()
{
    return NOERROR;
}

ECode FusedBatchOptions::SetMaxPowerAllocationInMW(
    /* [in] */ Double value)
{
    mMaxPowerAllocationInMW = value;
    return NOERROR;
}

ECode FusedBatchOptions::GetMaxPowerAllocationInMW(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMaxPowerAllocationInMW;
    return NOERROR;
}

ECode FusedBatchOptions::SetPeriodInNS(
    /* [in] */ Int64 value)
{
    mPeriodInNS = value;
    return NOERROR;
}

ECode FusedBatchOptions::GetPeriodInNS(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPeriodInNS;
    return NOERROR;
}

ECode FusedBatchOptions::SetSourceToUse(
    /* [in] */ Int32 source)
{
    mSourcesToUse |= source;
    return NOERROR;
}

ECode FusedBatchOptions::ResetSourceToUse(
    /* [in] */ Int32 source)
{
    mSourcesToUse &= ~source;
    return NOERROR;
}

ECode FusedBatchOptions::IsSourceToUseSet(
    /* [in] */ Int32 source,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mSourcesToUse & source) != 0;
    return NOERROR;
}

ECode FusedBatchOptions::GetSourcesToUse(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSourcesToUse;
    return NOERROR;
}

ECode FusedBatchOptions::SetFlag(
    /* [in] */ Int32 flag)
{
    mFlags |= flag;
    return NOERROR;
}

ECode FusedBatchOptions::ResetFlag(
    /* [in] */ Int32 flag)
{
    mFlags &= ~flag;
    return NOERROR;
}

ECode FusedBatchOptions::IsFlagSet(
    /* [in] */ Int32 flag,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mFlags & flag) != 0;
    return NOERROR;
}

ECode FusedBatchOptions::GetFlags(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFlags;
    return NOERROR;
}

ECode FusedBatchOptions::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadDouble(&mMaxPowerAllocationInMW);
    source->ReadInt64(&mPeriodInNS);
    source->ReadInt32(&mSourcesToUse);
    source->ReadInt32(&mFlags);
    return NOERROR;
}

ECode FusedBatchOptions::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteDouble(mMaxPowerAllocationInMW);
    dest->WriteInt64(mPeriodInNS);
    dest->WriteInt32(mSourcesToUse);
    dest->WriteInt32(mFlags);
    return NOERROR;
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
