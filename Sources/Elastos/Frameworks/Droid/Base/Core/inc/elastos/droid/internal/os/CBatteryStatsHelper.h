#ifndef __ELASTOS_DROID_INTERNAL_OS_CBATTERYSTATSHELPER_H__
#define __ELASTOS_DROID_INTERNAL_OS_CBATTERYSTATSHELPER_H__

#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Internal_Os_CBatteryStatsHelper.h"
#include "elastos/droid/internal/os/CBatterySipper.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Core::IComparator;
using Elastos::IO::IFile;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFileInputStream;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CarClass(CBatteryStatsHelper)
    , public Object
    , public IBatteryStatsHelper
{
private:
    class BatterySipperComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* _lhs,
            /* [in] */ IInterface* _rhs,
            /* [out] */ Int32* result);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBatteryStatsHelper();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Boolean collectBatteryBroadcast);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Boolean collectBatteryBroadcast,
        /* [in] */ Boolean wifiOnly);

    CARAPI StoreStatsHistoryInFile(
        /* [in] */ const String& fname);

    /** Clears the current stats and forces recreating for future use. */
    CARAPI ClearStats();

    CARAPI GetStats(
        /* [out] */ IBatteryStats** stats);

    CARAPI GetBatteryBroadcast(
        /* [out] */ IIntent** intent);

    CARAPI GetPowerProfile(
        /* [out] */ IPowerProfile** profile);

    CARAPI Create(
        /* [in] */ IBatteryStats* stats);

    CARAPI Create(
        /* [in] */ IBundle* icicle);

    CARAPI StoreState();

    /**
     * Refreshes the power usage list.
     */
    CARAPI RefreshStats(
        /* [in] */ Int32 statsType,
        /* [in] */ Int32 asUser);

    /**
     * Refreshes the power usage list.
     */
    CARAPI RefreshStats(
        /* [in] */ Int32 statsType,
        /* [in] */ IList* asUsers);

    /**
     * Refreshes the power usage list.
     */
    CARAPI RefreshStats(
        /* [in] */ Int32 statsType,
        /* [in] */ ISparseArray* asUsers);

    CARAPI RefreshStats(
        /* [in] */ Int32 statsType,
        /* [in] */ ISparseArray* asUsers,
        /* [in] */ Int64 rawRealtimeUs,
        /* [in] */ Int64 rawUptimeUs);

    CARAPI GetUsageList(
        /* [out] */ IList** list);

    CARAPI GetMobilemsppList(
        /* [out] */ IList** list);

    CARAPI GetStatsPeriod(
        /* [out] */ Int64* result);

    CARAPI GetStatsType(
        /* [out] */ Int32* result);

    CARAPI GetMaxPower(
        /* [out] */ Double* result);

    CARAPI GetMaxRealPower(
        /* [out] */ Double* result);

    CARAPI GetTotalPower(
        /* [out] */ Double* result);

    CARAPI GetComputedPower(
        /* [out] */ Double* result);

    CARAPI GetMinDrainedPower(
        /* [out] */ Double* result);

    CARAPI GetMaxDrainedPower(
        /* [out] */ Double* result);

    CARAPI GetBatteryTimeRemaining(
        /* [out] */ Int64* result);

    CARAPI GetChargeTimeRemaining(
        /* [out] */ Int64* result);

public:
    static CARAPI_(Boolean) CheckWifiOnly(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<IBatteryStats>) StatsFromFile(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname);

    static CARAPI DropFile(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname);

    static CARAPI_(String) MakemAh(
        /* [in] */ Double power);

    static CARAPI ReadFully(
        /* [in] */ IFileInputStream* stream,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    static CARAPI ReadFully(
        /* [in] */ IFileInputStream* stream,
        /* [in] */ Int32 avail,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

private:
    static CARAPI_(AutoPtr<IFile>) MakeFilePath(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname);

    static CARAPI_(AutoPtr<IBatteryStatsImpl>) GetStats(
        /* [in] */ IIBatteryStats* service);


    CARAPI_(void) ProcessAppUsage(
        /* [in] */ ISparseArray* asUsers);

    CARAPI_(void) AddPhoneUsage();

    CARAPI_(void) AddScreenUsage();

    CARAPI_(void) AddRadioUsage();

    CARAPI_(void) AggregateSippers(
        /* [in] */ CBatterySipper* bs,
        /* [in] */ IList* from,
        /* [in] */ const String& tag);

    CARAPI_(void) AddWiFiUsage();

    CARAPI_(void) AddIdleUsage();

    CARAPI_(void) AddBluetoothUsage();

    CARAPI_(void) AddFlashlightUsage();

    CARAPI_(void) AddUserUsage();

    /**
     * Return estimated power (in mAs) of sending or receiving a packet with the mobile radio.
     */
    CARAPI_(Double) GetMobilePowerPerPacket();

    /**
     * Return estimated power (in mAs) of keeping the radio up
     */
    CARAPI_(Double) GetMobilePowerPerMs();

    /**
     * Return estimated power (in mAs) of sending a byte with the Wi-Fi radio.
     */
    CARAPI_(Double) GetWifiPowerPerPacket();

    CARAPI_(void) ProcessMiscUsage();

    CARAPI_(AutoPtr<CBatterySipper>) AddEntry(
        /* [in] */ BatterySipperDrainType drainType,
        /* [in] */ Int64 time,
        /* [in] */ Double power);

    CARAPI_(AutoPtr<CBatterySipper>) AddEntryNoTotal(
        /* [in] */ BatterySipperDrainType drainType,
        /* [in] */ Int64 time,
        /* [in] */ Double power);

    CARAPI_(void) Load();

public:
    Int64 mRawRealtime;
    Int64 mRawUptime;
    Int64 mBatteryRealtime;
    Int64 mBatteryUptime;
    Int64 mTypeBatteryRealtime;
    Int64 mTypeBatteryUptime;
    Int64 mBatteryTimeRemaining;
    Int64 mChargeTimeRemaining;

private:
    static const Boolean DEBUG;

    static const String TAG;

    static AutoPtr<IBatteryStats> sStatsXfer;
    static AutoPtr<IIntent> sBatteryBroadcastXfer;
    static AutoPtr<IArrayMap> sFileXfer;

    AutoPtr<IContext> mContext;
    Boolean mCollectBatteryBroadcast;
    Boolean mWifiOnly;

    AutoPtr<IIBatteryStats> mBatteryInfo;
    AutoPtr<IBatteryStats> mStats;
    AutoPtr<IIntent> mBatteryBroadcast;
    AutoPtr<IPowerProfile> mPowerProfile;

    AutoPtr<IList> mUsageList;
    AutoPtr<IList> mWifiSippers;
    AutoPtr<IList> mBluetoothSippers;
    HashMap<Int32, AutoPtr<IList> > mUserSippers;
    HashMap<Int32, Double> mUserPower;

    AutoPtr<IList> mMobilemsppList;

    Int32 mStatsType;

    Int64 mStatsPeriod;
    Double mMaxPower;
    Double mMaxRealPower;
    Double mComputedPower;
    Double mTotalPower;
    Double mWifiPower;
    Double mBluetoothPower;
    Double mMinDrainedPower;
    Double mMaxDrainedPower;

    // How much the apps together have kept the mobile radio active.
    Int64 mAppMobileActive;

    // How much the apps together have left WIFI running.
    Int64 mAppWifiRunning;
};

} // Os
} // Internal
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_INTERNAL_OS_CBATTERYSTATSHELPER_H__
