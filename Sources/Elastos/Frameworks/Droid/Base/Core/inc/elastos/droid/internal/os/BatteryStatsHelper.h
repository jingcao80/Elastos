#ifndef __ELASTOS_DROID_INTERNAL_OS_BATTERYSTATSHELPER_H__
#define __ELASTOS_DROID_INTERNAL_OS_BATTERYSTATSHELPER_H__

#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/os/BatterySipper.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

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
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

/**
 * A helper class for retrieving the power usage information for all applications and services.
 *
 * The caller must initialize this class as soon as activity object is ready to use (for example, in
 * onAttach() for Fragment), call create() in onCreate() and call destroy() in onDestroy().
 */
class BatteryStatsHelper : public Object
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
    BatteryStatsHelper(
        /* [in] */ IContext* context);

    BatteryStatsHelper(
        /* [in] */ IContext* context,
        /* [in] */ Boolean collectBatteryBroadcast);

    BatteryStatsHelper(
        /* [in] */ IContext* context,
        /* [in] */ Boolean collectBatteryBroadcast,
        /* [in] */ Boolean wifiOnly);

    static CARAPI_(Boolean) CheckWifiOnly(
        /* [in] */ IContext* context);

    CARAPI_(void) StoreStatsHistoryInFile(
        /* [in] */ const String& fname);

    static CARAPI_(AutoPtr<IBatteryStats>) StatsFromFile(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname);

    static CARAPI_(void) DropFile(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname);

    /** Clears the current stats and forces recreating for future use. */
    CARAPI_(void) ClearStats();

    CARAPI_(AutoPtr<IBatteryStats>) GetStats();

    CARAPI_(AutoPtr<IIntent>) GetBatteryBroadcast();

    CARAPI_(AutoPtr<IPowerProfile>) GetPowerProfile();

    CARAPI_(void) Create(
        /* [in] */ IBatteryStats* stats);

    CARAPI_(void) Create(
        /* [in] */ IBundle* icicle);

    CARAPI_(void) StoreState();

    static CARAPI_(String) MakemAh(
        /* [in] */ Double power);

    /**
     * Refreshes the power usage list.
     */
    CARAPI_(void) RefreshStats(
        /* [in] */ Int32 statsType,
        /* [in] */ Int32 asUser);

    /**
     * Refreshes the power usage list.
     */
    CARAPI_(void) RefreshStats(
        /* [in] */ Int32 statsType,
        /* [in] */ IList* asUsers);

    /**
     * Refreshes the power usage list.
     */
    CARAPI_(void) RefreshStats(
        /* [in] */ Int32 statsType,
        /* [in] */ ISparseArray* asUsers);

    CARAPI_(void) RefreshStats(
        /* [in] */ Int32 statsType,
        /* [in] */ ISparseArray* asUsers,
        /* [in] */ Int64 rawRealtimeUs,
        /* [in] */ Int64 rawUptimeUs);

    CARAPI_(AutoPtr<IList>) GetUsageList();

    CARAPI_(AutoPtr<IList>) GetMobilemsppList();

    CARAPI_(Int64) GetStatsPeriod();

    CARAPI_(Int32) GetStatsType();

    CARAPI_(Double) GetMaxPower();

    CARAPI_(Double) GetMaxRealPower();

    CARAPI_(Double) GetTotalPower();

    CARAPI_(Double) GetComputedPower();

    CARAPI_(Double) GetMinDrainedPower();

    CARAPI_(Double) GetMaxDrainedPower();

    CARAPI_(Int64) GetBatteryTimeRemaining();

    CARAPI_(Int64) GetChargeTimeRemaining();

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) ReadFully(
        /* [in] */ IFileInputStream* stream);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) ReadFully(
        /* [in] */ IFileInputStream* stream,
        /* [in] */ Int32 avail);

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
        /* [in] */ BatterySipper* bs,
        /* [in] */ List<AutoPtr<BatterySipper> >* from,
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

    CARAPI_(AutoPtr<BatterySipper>) AddEntry(
        /* [in] */ BatterySipper::DrainType drainType,
        /* [in] */ Int64 time,
        /* [in] */ Double power);

    CARAPI_(AutoPtr<BatterySipper>) AddEntryNoTotal(
        /* [in] */ BatterySipper::DrainType drainType,
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
    static const Boolean DEBUG = FALSE;

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
    List<AutoPtr<BatterySipper> > mWifiSippers;
    List<AutoPtr<BatterySipper> > mBluetoothSippers;
    HashMap<Int32, AutoPtr<List<AutoPtr<BatterySipper> > > > mUserSippers;
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

#endif // __ELASTOS_DROID_INTERNAL_OS_BATTERYSTATSHELPER_H__
