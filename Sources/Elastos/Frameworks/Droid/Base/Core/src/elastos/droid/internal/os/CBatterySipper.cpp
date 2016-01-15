
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
    if (values != NULL)
        mValue = (*values)[0];
    mDrainType = drainType;
    mUidObj = uid;
    return NOERROR;
}

ECode CBatterySipper::GetValues(
    /* [out] */ ArrayOf<Double>** values)
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
    if (IBatterySipper::Probe(_other) == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
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

/**
 * Gets a list of packages associated with the current user
 */
ECode CBatterySipper::GetPackages(
    /* [out] */ ArrayOf<String>** packages)
{
    VALIDATE_NOT_NULL(packages)
    *packages = mPackages;
    REFCOUNT_ADD(*packages)
    return NOERROR;
}

ECode CBatterySipper::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid)
    // Bail out if the current sipper is not an App sipper.
    if (mUidObj == NULL) {
        *uid = 0;
    }
    return mUidObj->GetUid(uid);
}

} // Os
} // Internal
} // Droid
} // Elastos
