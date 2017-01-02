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

#ifndef __ELASTOS_DROID_INTERNAL_OS_CBATTERYSIPPER_H__
#define __ELASTOS_DROID_INTERNAL_OS_CBATTERYSIPPER_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Internal_Os_CBatterySipper.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBatteryStatsUid;
using Elastos::Core::IComparable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CarClass(CBatterySipper)
    , public Object
    , public IBatterySipper
    , public IComparable
{
public:
    TO_STRING_IMPL("CBatterySipper")

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBatterySipper();

    CARAPI constructor(
        /* [in] */ BatterySipperDrainType drainType,
        /* [in] */ IBatteryStatsUid* uid,
        /* [in] */ ArrayOf<Double>* values);

    CARAPI GetValues(
        /* [out, callee] */ ArrayOf<Double>** values);

    CARAPI ComputeMobilemspp();

    CARAPI CompareTo(
        /* [in] */ IInterface* _other,
        /* [out] */ Int32* result);

    /**
     * Gets a list of packages associated with the current user
     */
    CARAPI GetPackages(
        /* [out, callee] */ ArrayOf<String>** packages);

    CARAPI GetUid(
        /* [out] */ Int32* uid);

    CARAPI GetUserId(
        /* [out] */ Int32* userId);

    CARAPI SetUserId(
        /* [in] */ Int32 userId);

    CARAPI GetUidObj(
        /* [out] */ IBatteryStatsUid** uidObj);

    CARAPI SetUidObj(
        /* [in] */ IBatteryStatsUid* uidObj);

    CARAPI GetValue(
        /* [out] */ Double* value);

    CARAPI SetValue(
        /* [in] */ Double value);

    CARAPI SetValues(
        /* [in] */ ArrayOf<Double>* values);

    CARAPI GetDrainType(
        /* [out] */ BatterySipperDrainType* drainType);

    CARAPI SetDrainType(
        /* [in] */ BatterySipperDrainType drainType);

    CARAPI GetUsageTime(
        /* [out] */ Int64* usageTime);

    CARAPI SetUsageTime(
        /* [in] */ Int64 usageTime);

    CARAPI GetCpuTime(
        /* [out] */ Int64* cpuTime);

    CARAPI SetCpuTime(
        /* [in] */ Int64 cpuTime);

    CARAPI GetGpsTime(
        /* [out] */ Int64* gpsTime);

    CARAPI SetGpsTime(
        /* [in] */ Int64 gpsTime);

    CARAPI GetWifiRunningTime(
        /* [out] */ Int64* wifiRunningTime);

    CARAPI SetWifiRunningTime(
        /* [in] */ Int64 wifiRunningTime);

    CARAPI GetCpuFgTime(
        /* [out] */ Int64* cpuFgTime);

    CARAPI SetCpuFgTime(
        /* [in] */ Int64 cpuFgTime);

    CARAPI GetWakeLockTime(
        /* [out] */ Int64* wakeLockTime);

    CARAPI SetWakeLockTime(
        /* [in] */ Int64 wakeLockTime);

    CARAPI GetMobileRxPackets(
        /* [out] */ Int64* mobileRxPackets);

    CARAPI SetMobileRxPackets(
        /* [in] */ Int64 mobileRxPackets);

    CARAPI GetMobileTxPackets(
        /* [out] */ Int64* mobileTxPackets);

    CARAPI SetMobileTxPackets(
        /* [in] */ Int64 mobileTxPackets);

    CARAPI GetMobileActive(
        /* [out] */ Int64* mobileActive);

    CARAPI SetMobileActive(
        /* [in] */ Int64 mobileActive);

    CARAPI GetMobileActiveCount(
        /* [out] */ Int32* mobileActiveCount);

    CARAPI SetMobileActiveCount(
        /* [in] */ Int32 mobileActiveCount);

    CARAPI GetMobilemspp(
        /* [out] */ Double* mobilemspp);

    CARAPI SetMobilemspp(
        /* [in] */ Double mobilemspp);

    CARAPI GetWifiRxPackets(
        /* [out] */ Int64* wifiRxPackets);

    CARAPI SetWifiRxPackets(
        /* [in] */ Int64 wifiRxPackets);

    CARAPI GetWifiTxPackets(
        /* [out] */ Int64* wifiTxPackets);

    CARAPI SetWifiTxPackets(
        /* [in] */ Int64 wifiTxPackets);

    CARAPI GetMobileRxBytes(
        /* [out] */ Int64* mobileRxBytes);

    CARAPI SetMobileRxBytes(
        /* [in] */ Int64 mobileRxBytes);

    CARAPI GetMobileTxBytes(
        /* [out] */ Int64* mobileTxBytes);

    CARAPI SetMobileTxBytes(
        /* [in] */ Int64 mobileTxBytes);

    CARAPI GetWifiRxBytes(
        /* [out] */ Int64* wifiRxBytes);

    CARAPI SetWifiRxBytes(
        /* [in] */ Int64 wifiRxBytes);

    CARAPI GetWifiTxBytes(
        /* [out] */ Int64* wifiTxBytes);

    CARAPI SetWifiTxBytes(
        /* [in] */ Int64 wifiTxBytes);

    CARAPI GetPercent(
        /* [out] */ Double* percent);

    CARAPI SetPercent(
        /* [in] */ Double percent);

    CARAPI GetNoCoveragePercent(
        /* [out] */ Double* noCoveragePercent);

    CARAPI SetNoCoveragePercent(
        /* [in] */ Double noCoveragePercent);

    CARAPI SetPackages(
        /* [in] */ ArrayOf<String>* packages);

    CARAPI GetPackageWithHighestDrain(
        /* [out] */ String* packageWithHighestDrain);

    CARAPI SetPackageWithHighestDrain(
        /* [in] */ const String& packageWithHighestDrain);

public:
    Int32 mUserId;
    AutoPtr<IBatteryStatsUid> mUidObj;
    Double mValue;
    AutoPtr<ArrayOf<Double> > mValues;
    BatterySipperDrainType mDrainType;
    Int64 mUsageTime;
    Int64 mCpuTime;
    Int64 mGpsTime;
    Int64 mWifiRunningTime;
    Int64 mCpuFgTime;
    Int64 mWakeLockTime;
    Int64 mMobileRxPackets;
    Int64 mMobileTxPackets;
    Int64 mMobileActive;
    Int32 mMobileActiveCount;
    Double mMobilemspp;         // milliseconds per packet
    Int64 mWifiRxPackets;
    Int64 mWifiTxPackets;
    Int64 mMobileRxBytes;
    Int64 mMobileTxBytes;
    Int64 mWifiRxBytes;
    Int64 mWifiTxBytes;
    Double mPercent;
    Double mNoCoveragePercent;
    AutoPtr<ArrayOf<String> > mPackages;
    String mPackageWithHighestDrain;
};

} // Os
} // Internal
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_INTERNAL_OS_CBATTERYSIPPER_H__
