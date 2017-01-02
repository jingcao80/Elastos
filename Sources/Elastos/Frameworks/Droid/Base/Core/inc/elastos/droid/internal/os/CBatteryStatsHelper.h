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

#ifndef __ELASTOS_DROID_INTERNAL_OS_CBATTERYSTATSHELPER_H__
#define __ELASTOS_DROID_INTERNAL_OS_CBATTERYSTATSHELPER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Internal_Os_CBatteryStatsHelper.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Core::Object;
using Elastos::Core::IComparator;
using Elastos::IO::IFile;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFileInputStream;
using Elastos::Utility::IList;

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
        TO_STRING_IMPL("CBatteryStatsHelper::BatterySipperComparator")

        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* _lhs,
            /* [in] */ IInterface* _rhs,
            /* [out] */ Int32* result);
    };

public:
    TO_STRING_IMPL("CBatteryStatsHelper")

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

    static CARAPI_(Boolean) CheckWifiOnly(
        /* [in] */ IContext* context);

    CARAPI StoreStatsHistoryInFile(
        /* [in] */ const String& fname);

    static CARAPI_(AutoPtr<IBatteryStats>) StatsFromFile(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname);

    static CARAPI_(void) DropFile(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname);

    /** Clears the current stats and forces recreating for future use. */
    CARAPI ClearStats();

    CARAPI GetStats(
        /* [out] */ IBatteryStats** stats);

    CARAPI GetBatteryBroadcast(
        /* [out] */ IIntent** batteryBroadcast);

    CARAPI GetPowerProfile(
        /* [out] */ IPowerProfile** powerProfile);

    CARAPI Create(
        /* [in] */ IBatteryStats* stats);

    CARAPI Create(
        /* [in] */ IBundle* icicle);

    CARAPI StoreState();

    static CARAPI_(String) MakemAh(
        /* [in] */ Double power);

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
        /* [out] */ IList** usageList);

    CARAPI GetMobilemsppList(
        /* [out] */ IList** mobilemsppList);

    CARAPI GetStatsPeriod(
        /* [out] */ Int64* statsPeriod);

    CARAPI GetStatsType(
        /* [out] */ Int32* statsType);

    CARAPI GetMaxPower(
        /* [out] */ Double* maxPower);

    CARAPI GetMaxRealPower(
        /* [out] */ Double* maxRealPower);

    CARAPI GetTotalPower(
        /* [out] */ Double* totalPower);

    CARAPI GetComputedPower(
        /* [out] */ Double* computedPower);

    CARAPI GetMinDrainedPower(
        /* [out] */ Double* minDrainedPower);

    CARAPI GetMaxDrainedPower(
        /* [out] */ Double* maxDrainedPower);

    CARAPI GetBatteryTimeRemaining(
        /* [out] */ Int64* batteryTimeRemaining);

    CARAPI GetChargeTimeRemaining(
        /* [out] */ Int64* chargeTimeRemaining);

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
        /* [in] */ IBatterySipper* bs,
        /* [in] */ IList* from, // List<BatterySipper>
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

    CARAPI_(AutoPtr<IBatterySipper>) AddEntry(
        /* [in] */ BatterySipperDrainType drainType,
        /* [in] */ Int64 time,
        /* [in] */ Double power);

    CARAPI_(AutoPtr<IBatterySipper>) AddEntryNoTotal(
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

    AutoPtr<IList> mUsageList; // List<BatterySipper>
    AutoPtr<IList> mWifiSippers; // List<BatterySipper>
    AutoPtr<IList> mBluetoothSippers; // List<BatterySipper>
    AutoPtr<ISparseArray> mUserSippers; //SparseArray<List<BatterySipper>>
    AutoPtr<ISparseArray> mUserPower; // SparseArray<Double>

    AutoPtr<IList> mMobilemsppList; // List<BatterySipper>

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
