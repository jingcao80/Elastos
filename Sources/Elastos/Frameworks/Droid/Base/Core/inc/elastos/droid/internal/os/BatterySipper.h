#ifndef __ELASTOS_DROID_INTERNAL_OS_BATTERYSIPPER_H__
#define __ELASTOS_DROID_INTERNAL_OS_BATTERYSIPPER_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBatteryStatsUid;
using Elastos::Core::IComparable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

class BatterySipper
    : public Object
    , public IComparable
{
public:
    enum DrainType {
        IDLE,
        CELL,
        PHONE,
        WIFI,
        BLUETOOTH,
        FLASHLIGHT,
        SCREEN,
        APP,
        USER,
        UNACCOUNTED,
        OVERCOUNTED,
    };

public:
    BatterySipper(
        /* [in] */ DrainType drainType,
        /* [in] */ IBatteryStatsUid* uid,
        /* [in] */ ArrayOf<Double>* values);

    CAR_INTERFACE_DECL()

    CARAPI_(AutoPtr<ArrayOf<Double> >) GetValues();

    CARAPI_(void) ComputeMobilemspp();

    CARAPI CompareTo(
        /* [in] */ IInterface* _other,
        /* [out] */ Int32* result);

    /**
     * Gets a list of packages associated with the current user
     */
    CARAPI_(AutoPtr<ArrayOf<String> >) GetPackages();

    CARAPI_(Int32) GetUid();

public:
    Int32 mUserId;
    AutoPtr<IBatteryStatsUid> mUidObj;
    Double mValue;
    AutoPtr<ArrayOf<Double> > mValues;
    DrainType mDrainType;
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

#endif // __ELASTOS_DROID_INTERNAL_OS_BATTERYSIPPER_H__
