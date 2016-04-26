
#ifndef __ELASTOS_DROID_OS_BATTERYSTATS_H__
#define __ELASTOS_DROID_OS_BATTERYSTATS_H__

#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::IInteger32;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IFormatter;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Utility::IPrinter;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * A class providing access to battery usage statistics, including information on
 * wakelocks, processes, packages, and services.  All times are represented in microseconds
 * except where indicated otherwise.
 * @hide
 */
class BatteryStats
    : public Object
    , public IBatteryStats
{
public:

    /**
     * The statistics associated with a particular uid.
     */
    class BatteryStatsUid
        : public Object
        , public IBatteryStatsUid
    {
    public:

        class Pid : public Object
        {
        public:
            Int32 mWakeNesting;
            Int64 mWakeSumMs;
            Int64 mWakeStartMs;
        };

        /**
         * The statistics associated with a particular process.
         */
        class BatteryStatsProcExcessivePower
            : public Object
            , public IBatteryStatsProcExcessivePower
        {
        public:
            BatteryStatsProcExcessivePower()
                : mType(0)
                , mOverTime(0)
                , mUsedTime(0)
            {}

        public:
            static const Int32 TYPE_WAKE = 1;
            static const Int32 TYPE_CPU = 2;

            Int32 mType;
            Int64 mOverTime;
            Int64 mUsedTime;
        };

    public:
        static const AutoPtr<ArrayOf<String> > PROCESS_STATE_NAMES;

        /**
         * Note that these must match the constants in android.os.PowerManager.
         * Also, if the user activity types change, the BatteryStatsImpl.VERSION must
         * also be bumped.
         */
        static const AutoPtr<ArrayOf<String> > USER_ACTIVITY_TYPES;
    };

    class HistoryTag
        : public Object
        , public IBatteryStatsHistoryTag
    {
    public:
        CAR_INTERFACE_DECL()

        HistoryTag();

        constructor();

        CARAPI SetTo(
            /* [in] */ IBatteryStatsHistoryTag* o);

        CARAPI SetTo(
            /* [in] */ const String& string,
            /* [in] */ Int32 uid);

        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest,
            /* [in] */ Int32 flags);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* src);

        CARAPI GetString(
            /* [out] */ String* string);

        CARAPI SetString(
            /* [in] */ const String& string);

        CARAPI GetUid(
            /* [out] */ Int32* uid);

        CARAPI SetUid(
            /* [in] */ Int32 uid);

        CARAPI GetPoolIdx(
            /* [out] */ Int32* poolIdx);

        CARAPI SetPoolIdx(
            /* [in] */ Int32 poolIdx);

        CARAPI GetHashCode(
            /* [out] */ Int32* hash);

        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result);

    public:
        String mString;
        Int32 mUid;
        Int32 mPoolIndex;
    };

    class HistoryItem
        : public Object
        , public IBatteryStatsHistoryItem
        , public IParcelable
    {
    public:
        CAR_INTERFACE_DECL()

        HistoryItem()
            : mTime(0)
            , mCmd(CMD_NULL)
            , mBatteryLevel(0)
            , mBatteryStatus(0)
            , mBatteryHealth(0)
            , mBatteryPlugType(0)
            , mStates(0)
        {}

        constructor();

        constructor(
            /* [in] */ Int64 time);

        /**
         * Return whether the command code is a delta data update.
         */
        CARAPI IsDeltaData(
            /* [out] */ Boolean* result);

        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        CARAPI Clear();

        CARAPI SetTo(
            /* [in] */ IBatteryStatsHistoryItem* o);

        CARAPI SetTo(
            /* [in] */ Int64 time,
            /* [in] */ Byte cmd,
            /* [in] */ IBatteryStatsHistoryItem* o);

        CARAPI SameNonEvent(
            /* [in] */ IBatteryStatsHistoryItem* o,
            /* [out] */ Boolean* result);

        CARAPI Same(
            /* [in] */ IBatteryStatsHistoryItem* o,
            /* [out] */ Boolean* result);

    private:
        CARAPI SetToCommon(
            /* [in] */ IBatteryStatsHistoryItem* o);

    public:
        AutoPtr<HistoryItem> mNext;

        Int64 mTime;

        Byte mCmd;

        Byte mBatteryLevel;
        Byte mBatteryStatus;
        Byte mBatteryHealth;
        Byte mBatteryPlugType;

        Char32 mBatteryTemperature;
        Char32 mBatteryVoltage;

        Int32 mStates;
        Int32 mStates2;

        // The wake lock that was acquired at this point.
        AutoPtr<HistoryTag> mWakelockTag;

        // Kernel wakeup reason at this point.
        AutoPtr<HistoryTag> mWakeReasonTag;

        // For CMD_EVENT.
        Int32 mEventCode;
        AutoPtr<HistoryTag> mEventTag;

        // Only set for CMD_CURRENT_TIME or CMD_RESET, as per System.currentTimeMillis().
        Int64 mCurrentTime;

        // Meta-data when reading.
        Int64 mNumReadInts;

        // Pre-allocated objects.
        AutoPtr<HistoryTag> mLocalWakelockTag;// = new HistoryTag();
        AutoPtr<HistoryTag> mLocalWakeReasonTag;// = new HistoryTag();
        AutoPtr<HistoryTag> mLocalEventTag;// = new HistoryTag();
    };

    class HistoryEventTracker
        : public Object
        , public IBatteryStatsHistoryEventTracker
    {
    public:
        CAR_INTERFACE_DECL()

        UpdateState(
            /* [in] */ Int32 code,
            /* [in] */ const String& name,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 poolIdx,
            /* [out] */ Boolean* result);

        RemoveEvents(
            /* [in] */ Int32 code);

        GetStateForEvent(
            /* [in] */ Int32 code,
            /* [out] */ IHashMap** hashMap);
    private:
        AutoPtr<IHashMap> mActiveEvents; // = (HashMap<String, SparseIntArray>[]) new HashMap[HistoryItem.EVENT_COUNT];
    };

    class BitDescription
        : public Object
        , public IBatteryStatsBitDescription
    {
    public:
        CAR_INTERFACE_DECL()

        BitDescription();

        CARAPI constructor(
            /* [in] */ Int32 mask,
            /* [in] */ const String& name,
            /* [in] */ const String& shortName));

        CARAPI constructor(
            /* [in] */ Int32 mask,
            /* [in] */ Int32 shift,
            /* [in] */ const String& name,
            /* [in] */ ArrayOf<String>* values,
            /* [in] */ ArrayOf<String>* mShortValues);

    public:
        Int32 mMask;
        Int32 mShift;
        String mName;
        String mShortName;
        AutoPtr< ArrayOf<String> > mValues;
        AutoPtr< ArrayOf<String> > mShortValues;
    };

    class TimerEntry : public Object
    {
    public:
        TimerEntry(
            /* [in] */ const String& name,
            /* [in] */ Int32 id,
            /* [in] */ BatteryStatsTimer* timer,
            /* [in] */ Int64 time)
            : mName(name)
            , mId(id)
            , mTimer(timer)
            , mTime(time)
        {}

    public:
        String mName;
        Int32 mId;
        AutoPtr<BatteryStatsTimer> mTimer;
        Int64 mTime;
    };

    class HistoryPrinter
        : public Object
        , public IBatteryStatsHistoryPrinter
    {
    public:
        CAR_INTERFACE_DECL()

        HistoryPrinter();

        CARAPI PrintNextItem(
            /* [in] */ IPrintWriter* pw,
            /* [in] */ HistoryItem* rec,
            /* [in] */ Int64 now);

    public:
        Int32 mOldState = 0;
        Int32 mOldState2 = 0;
        Int32 mOldLevel = -1;
        Int32 mOldStatus = -1;
        Int32 mOldHealth = -1;
        Int32 mOldPlug = -1;
        Int32 mOldTemp = -1;
        Int32 mOldVolt = -1;
    };

public:
    CAR_INTERFACE_DECL()

    BatteryStats();


    static CARAPI FormatTime(
        /* [in ]*/ StringBuilder* out,
        /* [in] */ Int64 seconds);

    static CARAPI FormatTimeMs(
        /* [in ]*/ StringBuilder* out,
        /* [in] */ Int64 seconds);

    static CARAPI FormatTimeMsNoSpace(
        /* [in ]*/ StringBuilder* out,
        /* [in] */ Int64 seconds);

    CARAPI FormatRatioLocked(
        /* [Int64 ]*/ Int32 num,
        /* [in] */ Int64 seconds);

    String FormatBytesLocked(
        /* [in] */ Int64 bytes);

private:
    static void FormatTimeRaw(
        /* [in ]*/ StringBuilder* out,
        /* [in] */ Int64 seconds);

    static Int64 ComputeWakeLock(
        /* [in] */ Timer* timer,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);


private:
    static const Boolean LOCAL_LOGV = FALSE;

    // NOTE: Update this list if you add/change any stats above.
    // These characters are supposed to represent "total", "last", "current",
    // and "unplugged". They were shortened for efficiency sake.
    static const AutoPtr< ArrayOf<String> > STAT_NAMES;

    /**
     * Bump the version on this if the checkin format changes.
     */
    static const Int32 BATTERY_STATS_CHECKIN_VERSION = 9;

    static const Int64 BYTES_PER_KB = 1024;
    static const Int64 BYTES_PER_MB = 1048576; // 1024^2
    static const Int64 BYTES_PER_GB = 1073741824; //1024^3

    static const String VERSION_DATA = "vers";
    static const String UID_DATA = "uid";
    static const String APK_DATA = "apk";
    static const String PROCESS_DATA = "pr";
    static const String SENSOR_DATA = "sr";
    static const String VIBRATOR_DATA = "vib";
    static const String FOREGROUND_DATA = "fg";
    static const String STATE_TIME_DATA = "st";
    static const String WAKELOCK_DATA = "wl";
    static const String SYNC_DATA = "sy";
    static const String JOB_DATA = "jb";
    static const String KERNEL_WAKELOCK_DATA = "kwl";
    static const String WAKEUP_REASON_DATA = "wr";
    static const String NETWORK_DATA = "nt";
    static const String USER_ACTIVITY_DATA = "ua";
    static const String BATTERY_DATA = "bt";
    static const String BATTERY_DISCHARGE_DATA = "dc";
    static const String BATTERY_LEVEL_DATA = "lv";
    static const String WIFI_DATA = "wfl";
    static const String MISC_DATA = "m";
    static const String GLOBAL_NETWORK_DATA = "gn";
    static const String HISTORY_STRING_POOL = "hsp";
    static const String HISTORY_DATA = "h";
    static const String SCREEN_BRIGHTNESS_DATA = "br";
    static const String SIGNAL_STRENGTH_TIME_DATA = "sgt";
    static const String SIGNAL_SCANNING_TIME_DATA = "sst";
    static const String SIGNAL_STRENGTH_COUNT_DATA = "sgc";
    static const String DATA_CONNECTION_TIME_DATA = "dct";
    static const String DATA_CONNECTION_COUNT_DATA = "dcc";
    static const String WIFI_STATE_TIME_DATA = "wst";
    static const String WIFI_STATE_COUNT_DATA = "wsc";
    static const String WIFI_SUPPL_STATE_TIME_DATA = "wsst";
    static const String WIFI_SUPPL_STATE_COUNT_DATA = "wssc";
    static const String WIFI_SIGNAL_STRENGTH_TIME_DATA = "wsgt";
    static const String WIFI_SIGNAL_STRENGTH_COUNT_DATA = "wsgc";
    static const String BLUETOOTH_STATE_TIME_DATA = "bst";
    static const String BLUETOOTH_STATE_COUNT_DATA = "bsc";
    static const String POWER_USE_SUMMARY_DATA = "pws";
    static const String POWER_USE_ITEM_DATA = "pwi";
    static const String DISCHARGE_STEP_DATA = "dsd";
    static const String CHARGE_STEP_DATA = "csd";
    static const String DISCHARGE_TIME_REMAIN_DATA = "dtr";
    static const String CHARGE_TIME_REMAIN_DATA = "ctr";

    StringBuilder mFormatBuilder(32);
    AutoPtr<IFormatter> mFormatter = new Formatter(mFormatBuilder);

    static const AutoPtr< ArrayOf<String> > SCREEN_BRIGHTNESS_NAMES;

    static const AutoPtr< ArrayOf<String> > DATA_CONNECTION_NAMES;

    static const AutoPtr< ArrayOf<BitDescription*> > HISTORY_STATE_DESCRIPTIONS;

    static const AutoPtr< ArrayOf<BitDescription*> > HISTORY_STATE2_DESCRIPTIONS;

    static const AutoPtr< ArrayOf<String> > WIFI_SUPPL_STATE_NAMES;

    static const AutoPtr< ArrayOf<String> > WIFI_SUPPL_STATE_SHORT_NAMES;

    static const AutoPtr< ArrayOf<String> > WIFI_SUPPL_STATE_NAMES;

    static const AutoPtr< ArrayOf<String> > WIFI_STATE_NAMES;

    static const AutoPtr< ArrayOf<String> > BLUETOOTH_STATE_NAMES;

    static const AutoPtr< ArrayOf<String> > HISTORY_EVENT_CHECKIN_NAMES;

    AutoPtr<StringBuilder> mFormatBuilder;
    AutoPtr<IFormatter> mFormatter;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_BATTERYSTATS_H__
