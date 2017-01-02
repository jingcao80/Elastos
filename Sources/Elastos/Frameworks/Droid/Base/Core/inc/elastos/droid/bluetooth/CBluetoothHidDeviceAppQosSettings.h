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

#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHIDDEVICEAPPQOSSETTINGS_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHIDDEVICEAPPQOSSETTINGS_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothHidDeviceAppQosSettings.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHidDeviceAppQosSettings)
    , public Object
    , public IParcelable
    , public IBluetoothHidDeviceAppQosSettings
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CBluetoothHidDeviceAppQosSettings();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 serviceType,
        /* [in] */ Int32 tokenRate,
        /* [in] */ Int32 tokenBucketSize,
        /* [in] */ Int32 peakBandwidth,
        /* [in] */ Int32 latency,
        /* [in] */ Int32 delayVariation);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<Int32>** array);

private:
    Int32 mServiceType;
    Int32 mTokenRate;
    Int32 mTokenBucketSize;
    Int32 mPeakBandwidth;
    Int32 mLatency;
    Int32 mDelayVariation;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
#endif