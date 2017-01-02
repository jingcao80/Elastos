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

#include "elastos/droid/bluetooth/CBluetoothHidDeviceAppQosSettings.h"
#include "Elastos.CoreLibrary.Utility.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_OBJECT_IMPL(CBluetoothHidDeviceAppQosSettings)

CAR_INTERFACE_IMPL_2(CBluetoothHidDeviceAppQosSettings, Object, IBluetoothHidDeviceAppQosSettings, IParcelable)

CBluetoothHidDeviceAppQosSettings::CBluetoothHidDeviceAppQosSettings()
    : mServiceType(0)
    , mTokenRate(0)
    , mTokenBucketSize(0)
    , mPeakBandwidth(0)
    , mLatency(0)
    , mDelayVariation(0)
{}

ECode CBluetoothHidDeviceAppQosSettings::constructor()
{
    return NOERROR;
}

ECode CBluetoothHidDeviceAppQosSettings::constructor(
    /* [in] */ Int32 serviceType,
    /* [in] */ Int32 tokenRate,
    /* [in] */ Int32 tokenBucketSize,
    /* [in] */ Int32 peakBandwidth,
    /* [in] */ Int32 latency,
    /* [in] */ Int32 delayVariation)
{
    mServiceType = serviceType;
    mTokenRate = tokenRate;
    mTokenBucketSize = tokenBucketSize;
    mPeakBandwidth = peakBandwidth;
    mLatency = latency;
    mDelayVariation = delayVariation;
    return NOERROR;
}

ECode CBluetoothHidDeviceAppQosSettings::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mServiceType);
    source->ReadInt32(&mTokenRate);
    source->ReadInt32(&mTokenBucketSize);
    source->ReadInt32(&mPeakBandwidth);
    source->ReadInt32(&mLatency);
    source->ReadInt32(&mDelayVariation);
    return NOERROR;
}

ECode CBluetoothHidDeviceAppQosSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mServiceType);
    dest->WriteInt32(mTokenRate);
    dest->WriteInt32(mTokenBucketSize);
    dest->WriteInt32(mPeakBandwidth);
    dest->WriteInt32(mLatency);
    dest->WriteInt32(mDelayVariation);
    return NOERROR;
}

ECode CBluetoothHidDeviceAppQosSettings::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    *result = FALSE;
    return NOERROR;
}

ECode CBluetoothHidDeviceAppQosSettings::ToArray(
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(6);
    (*result)[0] = mServiceType;
    (*result)[1] = mTokenRate;
    (*result)[2] = mTokenBucketSize;
    (*result)[3] = mPeakBandwidth;
    (*result)[4] = mLatency;
    (*result)[5] = mDelayVariation;
    *array = result;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos