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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/os/CBatterySipper.h"
#include <elastos/core/Math.h>

using Elastos::Core::EIID_IComparable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CAR_INTERFACE_IMPL_2(CBatterySipper, Object, IBatterySipper, IComparable)

CAR_OBJECT_IMPL(CBatterySipper)

CBatterySipper::CBatterySipper()
    : mUserId(0)
    , mValue(0.0)
    , mUsageTime(0)
    , mCpuTime(0)
    , mGpsTime(0)
    , mWifiRunningTime(0)
    , mCpuFgTime(0)
    , mWakeLockTime(0)
    , mMobileRxPackets(0)
    , mMobileTxPackets(0)
    , mMobileActive(0)
    , mMobileActiveCount(0)
    , mMobilemspp(0.0)
    , mWifiRxPackets(0)
    , mWifiTxPackets(0)
    , mMobileRxBytes(0)
    , mMobileTxBytes(0)
    , mWifiRxBytes(0)
    , mWifiTxBytes(0)
    , mPercent(0.0)
    , mNoCoveragePercent(0.0)
{}

ECode CBatterySipper::constructor(
    /* [in] */ BatterySipperDrainType drainType,
    /* [in] */ IBatteryStatsUid* uid,
    /* [in] */ ArrayOf<Double>* values)
{
    mValues = values;
    if (values != NULL) {
        mValue = (*values)[0];
    }
    mDrainType = drainType;
    mUidObj = uid;
    return NOERROR;
}

ECode CBatterySipper::GetValues(
    /* [out, callee] */ ArrayOf<Double>** values)
{
    VALIDATE_NOT_NULL(values)
    *values = mValues;
    REFCOUNT_ADD(*values)
    return NOERROR;
}

ECode CBatterySipper::ComputeMobilemspp()
{
    Int64 packets = mMobileRxPackets + mMobileTxPackets;
    mMobilemspp = packets > 0 ? (mMobileActive / (Double)packets) : 0;
    return NOERROR;
}

ECode CBatterySipper::CompareTo(
    /* [in] */ IInterface* _other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    CBatterySipper* other = (CBatterySipper*)IBatterySipper::Probe(_other);
    // Over-counted always goes to the bottom.
    if (mDrainType != other->mDrainType) {
        if (mDrainType == BatterySipperDrainType_OVERCOUNTED) {
            // This is "larger"
            *result = 1;
            return NOERROR;
        }
        else if (other->mDrainType == BatterySipperDrainType_OVERCOUNTED) {
            *result = -1;
            return NOERROR;
        }
    }
    // Return the flipped value because we want the items in descending order
    *result = Elastos::Core::Math::Compare(other->mValue, mValue);
    return NOERROR;
}

ECode CBatterySipper::GetPackages(
    /* [out, callee] */ ArrayOf<String>** packages)
{
    VALIDATE_NOT_NULL(packages)
    *packages = mPackages;
    REFCOUNT_ADD(*packages);
    return NOERROR;
}

ECode CBatterySipper::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid)
    *uid = 0;

    // Bail out if the current sipper is not an App sipper.
    if (mUidObj == NULL) {
        return NOERROR;
    }
    return mUidObj->GetUid(uid);
}

ECode CBatterySipper::GetUserId(
    /* [out] */ Int32* userId)
{
    VALIDATE_NOT_NULL(userId)
    *userId = mUserId;
    return NOERROR;
}

ECode CBatterySipper::SetUserId(
    /* [in] */ Int32 userId)
{
    mUserId = userId;
    return NOERROR;
}

ECode CBatterySipper::GetUidObj(
    /* [out] */ IBatteryStatsUid** uidObj)
{
    VALIDATE_NOT_NULL(uidObj)
    *uidObj = mUidObj;
    REFCOUNT_ADD(*uidObj)
    return NOERROR;
}

ECode CBatterySipper::SetUidObj(
    /* [in] */ IBatteryStatsUid* uidObj)
{
    mUidObj = uidObj;
    return NOERROR;
}

ECode CBatterySipper::GetValue(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mValue;
    return NOERROR;
}

ECode CBatterySipper::SetValue(
    /* [in] */ Double value)
{
    mValue = value;
    return NOERROR;
}

ECode CBatterySipper::SetValues(
    /* [in] */ ArrayOf<Double>* values)
{
    mValues = values;
    return NOERROR;
}

ECode CBatterySipper::GetDrainType(
    /* [out] */ BatterySipperDrainType* drainType)
{
    VALIDATE_NOT_NULL(drainType)
    *drainType = mDrainType;
    return NOERROR;
}

ECode CBatterySipper::SetDrainType(
    /* [in] */ BatterySipperDrainType drainType)
{
    mDrainType = drainType;
    return NOERROR;
}

ECode CBatterySipper::GetUsageTime(
    /* [out] */ Int64* usageTime)
{
    VALIDATE_NOT_NULL(usageTime)
    *usageTime = mUsageTime;
    return NOERROR;
}

ECode CBatterySipper::SetUsageTime(
    /* [in] */ Int64 usageTime)
{
    mUsageTime = usageTime;
    return NOERROR;
}

ECode CBatterySipper::GetCpuTime(
    /* [out] */ Int64* cpuTime)
{
    VALIDATE_NOT_NULL(cpuTime)
    *cpuTime = mCpuTime;
    return NOERROR;
}

ECode CBatterySipper::SetCpuTime(
    /* [in] */ Int64 cpuTime)
{
    mCpuTime = cpuTime;
    return NOERROR;
}

ECode CBatterySipper::GetGpsTime(
    /* [out] */ Int64* gpsTime)
{
    VALIDATE_NOT_NULL(gpsTime)
    *gpsTime = mGpsTime;
    return NOERROR;
}

ECode CBatterySipper::SetGpsTime(
    /* [in] */ Int64 gpsTime)
{
    mGpsTime = gpsTime;
    return NOERROR;
}

ECode CBatterySipper::GetWifiRunningTime(
    /* [out] */ Int64* wifiRunningTime)
{
    VALIDATE_NOT_NULL(wifiRunningTime)
    *wifiRunningTime = mWifiRunningTime;
    return NOERROR;
}

ECode CBatterySipper::SetWifiRunningTime(
    /* [in] */ Int64 wifiRunningTime)
{
    mWifiRunningTime = wifiRunningTime;
    return NOERROR;
}

ECode CBatterySipper::GetCpuFgTime(
    /* [out] */ Int64* cpuFgTime)
{
    VALIDATE_NOT_NULL(cpuFgTime)
    *cpuFgTime = mCpuFgTime;
    return NOERROR;
}

ECode CBatterySipper::SetCpuFgTime(
    /* [in] */ Int64 cpuFgTime)
{
    mCpuFgTime = cpuFgTime;
    return NOERROR;
}

ECode CBatterySipper::GetWakeLockTime(
    /* [out] */ Int64* wakeLockTime)
{
    VALIDATE_NOT_NULL(wakeLockTime)
    *wakeLockTime = mWakeLockTime;
    return NOERROR;
}

ECode CBatterySipper::SetWakeLockTime(
    /* [in] */ Int64 wakeLockTime)
{
    mWakeLockTime = wakeLockTime;
    return NOERROR;
}

ECode CBatterySipper::GetMobileRxPackets(
    /* [out] */ Int64* mobileRxPackets)
{
    VALIDATE_NOT_NULL(mobileRxPackets)
    *mobileRxPackets = mMobileRxPackets;
    return NOERROR;
}

ECode CBatterySipper::SetMobileRxPackets(
    /* [in] */ Int64 mobileRxPackets)
{
    mMobileRxPackets = mobileRxPackets;
    return NOERROR;
}

ECode CBatterySipper::GetMobileTxPackets(
    /* [out] */ Int64* mobileTxPackets)
{
    VALIDATE_NOT_NULL(mobileTxPackets)
    *mobileTxPackets = mMobileTxPackets;
    return NOERROR;
}

ECode CBatterySipper::SetMobileTxPackets(
    /* [in] */ Int64 mobileTxPackets)
{
    mMobileTxPackets = mobileTxPackets;
    return NOERROR;
}

ECode CBatterySipper::GetMobileActive(
    /* [out] */ Int64* mobileActive)
{
    VALIDATE_NOT_NULL(mobileActive)
    *mobileActive = mMobileActive;
    return NOERROR;
}

ECode CBatterySipper::SetMobileActive(
    /* [in] */ Int64 mobileActive)
{
    mMobileActive = mobileActive;
    return NOERROR;
}

ECode CBatterySipper::GetMobileActiveCount(
    /* [out] */ Int32* mobileActiveCount)
{
    VALIDATE_NOT_NULL(mobileActiveCount)
    *mobileActiveCount = mMobileActiveCount;
    return NOERROR;
}

ECode CBatterySipper::SetMobileActiveCount(
    /* [in] */ Int32 mobileActiveCount)
{
    mMobileActiveCount = mobileActiveCount;
    return NOERROR;
}

ECode CBatterySipper::GetMobilemspp(
    /* [out] */ Double* mobilemspp)
{
    VALIDATE_NOT_NULL(mobilemspp)
    *mobilemspp = mMobilemspp;
    return NOERROR;
}

ECode CBatterySipper::SetMobilemspp(
    /* [in] */ Double mobilemspp)
{
    mMobilemspp = mobilemspp;
    return NOERROR;
}

ECode CBatterySipper::GetWifiRxPackets(
    /* [out] */ Int64* wifiRxPackets)
{
    VALIDATE_NOT_NULL(wifiRxPackets)
    *wifiRxPackets = mWifiRxPackets;
    return NOERROR;
}

ECode CBatterySipper::SetWifiRxPackets(
    /* [in] */ Int64 wifiRxPackets)
{
    mWifiRxPackets = wifiRxPackets;
    return NOERROR;
}

ECode CBatterySipper::GetWifiTxPackets(
    /* [out] */ Int64* wifiTxPackets)
{
    VALIDATE_NOT_NULL(wifiTxPackets)
    *wifiTxPackets = mWifiTxPackets;
    return NOERROR;
}

ECode CBatterySipper::SetWifiTxPackets(
    /* [in] */ Int64 wifiTxPackets)
{
    mWifiTxPackets = wifiTxPackets;
    return NOERROR;
}

ECode CBatterySipper::GetMobileRxBytes(
    /* [out] */ Int64* mobileRxBytes)
{
    VALIDATE_NOT_NULL(mobileRxBytes)
    *mobileRxBytes = mMobileRxBytes;
    return NOERROR;
}

ECode CBatterySipper::SetMobileRxBytes(
    /* [in] */ Int64 mobileRxBytes)
{
    mMobileRxBytes = mobileRxBytes;
    return NOERROR;
}

ECode CBatterySipper::GetMobileTxBytes(
    /* [out] */ Int64* mobileTxBytes)
{
    VALIDATE_NOT_NULL(mobileTxBytes)
    *mobileTxBytes = mMobileTxBytes;
    return NOERROR;
}

ECode CBatterySipper::SetMobileTxBytes(
    /* [in] */ Int64 mobileTxBytes)
{
    mMobileTxBytes = mobileTxBytes;
    return NOERROR;
}

ECode CBatterySipper::GetWifiRxBytes(
    /* [out] */ Int64* wifiRxBytes)
{
    VALIDATE_NOT_NULL(wifiRxBytes)
    *wifiRxBytes = mWifiRxBytes;
    return NOERROR;
}

ECode CBatterySipper::SetWifiRxBytes(
    /* [in] */ Int64 wifiRxBytes)
{
    mWifiRxBytes = wifiRxBytes;
    return NOERROR;
}

ECode CBatterySipper::GetWifiTxBytes(
    /* [out] */ Int64* wifiTxBytes)
{
    VALIDATE_NOT_NULL(wifiTxBytes)
    *wifiTxBytes = mWifiTxBytes;
    return NOERROR;
}

ECode CBatterySipper::SetWifiTxBytes(
    /* [in] */ Int64 wifiTxBytes)
{
    mWifiTxBytes = wifiTxBytes;
    return NOERROR;
}

ECode CBatterySipper::GetPercent(
    /* [out] */ Double* percent)
{
    VALIDATE_NOT_NULL(percent)
    *percent = mPercent;
    return NOERROR;
}

ECode CBatterySipper::SetPercent(
    /* [in] */ Double percent)
{
    mPercent = percent;
    return NOERROR;
}

ECode CBatterySipper::GetNoCoveragePercent(
    /* [out] */ Double* noCoveragePercent)
{
    VALIDATE_NOT_NULL(noCoveragePercent)
    *noCoveragePercent = mNoCoveragePercent;
    return NOERROR;
}

ECode CBatterySipper::SetNoCoveragePercent(
    /* [in] */ Double noCoveragePercent)
{
    mNoCoveragePercent = noCoveragePercent;
    return NOERROR;
}

ECode CBatterySipper::SetPackages(
    /* [in] */ ArrayOf<String>* packages)
{
    mPackages = packages;
    return NOERROR;
}

ECode CBatterySipper::GetPackageWithHighestDrain(
    /* [out] */ String* packageWithHighestDrain)
{
    VALIDATE_NOT_NULL(packageWithHighestDrain)
    *packageWithHighestDrain = mPackageWithHighestDrain;
    return NOERROR;
}

ECode CBatterySipper::SetPackageWithHighestDrain(
    /* [in] */ const String& packageWithHighestDrain)
{
    mPackageWithHighestDrain = packageWithHighestDrain;
    return NOERROR;
}

} // Os
} // Internal
} // Droid
} // Elastos
