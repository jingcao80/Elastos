
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/os/BatterySipper.h"
#include <elastos/core/Math.h>

using Elastos::Core::EIID_IComparable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

BatterySipper::BatterySipper(
    /* [in] */ DrainType drainType,
    /* [in] */ IBatteryStatsUid* uid,
    /* [in] */ ArrayOf<Double>* values)
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
{
    mValues = values;
    if (values != NULL)
        mValue = (*values)[0];
    mDrainType = drainType;
    mUidObj = uid;
}

CAR_INTERFACE_IMPL(BatterySipper, Object, IComparable)

AutoPtr<ArrayOf<Double> > BatterySipper::GetValues()
{
    return mValues;
}

void BatterySipper::ComputeMobilemspp()
{
    Int64 packets = mMobileRxPackets + mMobileTxPackets;
    mMobilemspp = packets > 0 ? (mMobileActive / (Double)packets) : 0;
}

ECode BatterySipper::CompareTo(
    /* [in] */ IInterface* _other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    BatterySipper* other = (BatterySipper*)IComparable::Probe(_other);
    // Over-counted always goes to the bottom.
    if (mDrainType != other->mDrainType) {
        if (mDrainType == OVERCOUNTED) {
            // This is "larger"
            *result = 1;
            return NOERROR;
        }
        else if (other->mDrainType == OVERCOUNTED) {
            *result = -1;
            return NOERROR;
        }
    }
    // Return the flipped value because we want the items in descending order
    *result = Elastos::Core::Math::Compare(other->mValue, mValue);
    return NOERROR;
}

AutoPtr<ArrayOf<String> > BatterySipper::GetPackages()
{
    return mPackages;
}

Int32 BatterySipper::GetUid()
{
    // Bail out if the current sipper is not an App sipper.
    if (mUidObj == NULL) {
        return 0;
    }
    Int32 uid;
    mUidObj->GetUid(&uid);
    return uid;
}

} // Os
} // Internal
} // Droid
} // Elastos
