#ifndef __ELASTOS_DROID_INTERNAL_OS_CBATTERYSIPPER_H__
#define __ELASTOS_DROID_INTERNAL_OS_CBATTERYSIPPER_H__

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
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBatterySipper();

    CARAPI constructor(
        /* [in] */ BatterySipperDrainType drainType,
        /* [in] */ IBatteryStatsUid* uid,
        /* [in] */ ArrayOf<Double>* values);

    CARAPI GetValues(
        /* [out] */ ArrayOf<Double>** values);

    CARAPI ComputeMobilemspp();

    CARAPI CompareTo(
        /* [in] */ IInterface* _other,
        /* [out] */ Int32* result);

    /**
     * Gets a list of packages associated with the current user
     */
    CARAPI GetPackages(
        /* [out] */ ArrayOf<String>** packages);

    CARAPI GetUid(
        /* [out] */ Int32* uid);

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
