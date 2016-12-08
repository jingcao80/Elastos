
#ifndef __ELASTOS_DROID_OS_BATTERYSTATS_H__
#define __ELASTOS_DROID_OS_BATTERYSTATS_H__

#include "elastos/droid/ext/frameworkhash.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Internal::Os::BatterySipperDrainType;
using Elastos::Droid::Os::IBatteryStatsHistoryItem;
using Elastos::Droid::Os::IBatteryStatsUid;
using Elastos::Droid::Os::IBatteryStatsUidProcExcessivePower;
using Elastos::Droid::Utility::IPrinter;
using Elastos::Core::StringBuilder;
using Elastos::Core::IComparator;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IFormatter;

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
        class Pid
            : public Object
            , public IBatteryStatsUidPid
        {
        public:
            TO_STRING_IMPL("BatteryStats::BatteryStatsUid::Pid");

            CAR_INTERFACE_DECL()

            Pid();

            CARAPI GetWakeNesting(
                /* [out] */ Int32* wakeNesting);

            CARAPI SetWakeNesting(
                /* [in] */ Int32 wakeNesting);

            CARAPI GetWakeSumMs(
                /* [out] */ Int64* wakeSumMs);

            CARAPI SetWakeSumMs(
                /* [in] */ Int64 wakeSumMs);

            CARAPI GetWakeStartMs(
                /* [out] */ Int64* wakeStartMs);

            CARAPI SetWakeStartMs(
                /* [in] */ Int64 wakeStartMs);

        public:
            Int32 mWakeNesting;
            Int64 mWakeSumMs;
            Int64 mWakeStartMs;
        };

        /**
         * The statistics associated with a particular process.
         */
        class ExcessivePower
            : public Object
            , public IBatteryStatsUidProcExcessivePower
        {
        public:
            TO_STRING_IMPL("BatteryStats::BatteryStatsUid::ExcessivePower");

            CAR_INTERFACE_DECL()

            ExcessivePower();

            CARAPI GetType(
                /* [out] */ Int32* type);

            CARAPI SetType(
                /* [in] */ Int32 type);

            CARAPI GetOverTime(
                /* [out] */ Int64* overTime);

            CARAPI SetOverTime(
                /* [in] */ Int64 overTime);

            CARAPI GetUsedTime(
                /* [out] */ Int64* usedTime);

            CARAPI SetUsedTime(
                /* [in] */ Int64 usedTime);

        public:
            Int32 mType;
            Int64 mOverTime;
            Int64 mUsedTime;
        };

    public:
        TO_STRING_IMPL("BatteryStats::BatteryStatsUid");

        CAR_INTERFACE_DECL()

    public:
        static const AutoPtr<ArrayOf<String> > PROCESS_STATE_NAMES;

        /**
         * Note that these must match the constants in android.os.PowerManager.
         * Also, if the user activity types change, the BatteryStatsImpl.VERSION must
         * also be bumped.
         */
        static const AutoPtr<ArrayOf<String> > USER_ACTIVITY_TYPES;
    };

    class HistoryTag : public Object
    {
    public:
        TO_STRING_IMPL("BatteryStats::HistoryTag")

        HistoryTag();

        CARAPI_(void) SetTo(
            /* [in] */ HistoryTag* o);

        CARAPI_(void) SetTo(
            /* [in] */ const String& _string,
            /* [in] */ Int32 _uid);

        CARAPI_(void) WriteToParcel(
            /* [in] */ IParcel* dest);

        CARAPI_(void) ReadFromParcel(
            /* [in] */ IParcel* src);

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* other,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

    public:
        String mString;
        Int32 mUid;
        Int32 mPoolIdx;
    };

    class HistoryItem
        : public Object
        , public IBatteryStatsHistoryItem
        , public IParcelable
    {
    public:
        TO_STRING_IMPL("BatteryStats::HistoryItem")

        CAR_INTERFACE_DECL()

        HistoryItem();

        HistoryItem(
            /* [in] */ Int64 time,
            /* [in] */ IParcel* src);

        /**
         * Return whether the command code is a delta data update.
         */
        CARAPI_(Boolean) IsDeltaData();

        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        CARAPI_(void) Clear();

        CARAPI_(void) SetTo(
            /* [in] */ HistoryItem* o);

        CARAPI_(void) SetTo(
            /* [in] */ Int64 time,
            /* [in] */ Byte cmd,
            /* [in] */ HistoryItem* o);

        CARAPI_(Boolean) SameNonEvent(
            /* [in] */ HistoryItem* o);

        CARAPI_(Boolean) Same(
            /* [in] */ HistoryItem* o);

    private:
        CARAPI_(void) SetToCommon(
            /* [in] */ HistoryItem* o);

    public:
        AutoPtr<HistoryItem> mNext;

        Int64 mTime;

        Byte mCmd;

        Byte mBatteryLevel;
        Byte mBatteryStatus;
        Byte mBatteryHealth;
        Byte mBatteryPlugType;

        Int16 mBatteryTemperature;
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
        Int32 mNumReadInts;

        // Pre-allocated objects.
        AutoPtr<HistoryTag> mLocalWakelockTag;// = new HistoryTag();
        AutoPtr<HistoryTag> mLocalWakeReasonTag;// = new HistoryTag();
        AutoPtr<HistoryTag> mLocalEventTag;// = new HistoryTag();
    };

    class HistoryEventTracker : public Object
    {
    public:
        TO_STRING_IMPL("BatteryStats::HistoryEventTracker")

        HistoryEventTracker();

        CARAPI_(Boolean) UpdateState(
            /* [in] */ Int32 code,
            /* [in] */ const String& name,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 poolIdx);

        CARAPI_(void) RemoveEvents(
            /* [in] */ Int32 code);

        CARAPI_(AutoPtr<IHashMap>) GetStateForEvent(
            /* [in] */ Int32 code);

    private:
        AutoPtr<ArrayOf<IHashMap*> > mActiveEvents;
    };

    class BitDescription : public Object
    {
    public:
        TO_STRING_IMPL("BatteryStats::BitDescription")

        BitDescription(
            /* [in] */ Int32 mask,
            /* [in] */ const String& name,
            /* [in] */ const String& shortName)
            : mMask(mask)
            , mShift(-1)
            , mName(name)
            , mShortName(shortName)
        {}

        BitDescription(
            /* [in] */ Int32 mask,
            /* [in] */ Int32 shift,
            /* [in] */ const String& name,
            /* [in] */ const String& shortName,
            /* [in] */ ArrayOf<String>* values,
            /* [in] */ ArrayOf<String>* shortValues)
            : mMask(mask)
            , mShift(shift)
            , mName(name)
            , mShortName(shortName)
            , mValues(values)
            , mShortValues(shortValues)
        {}

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
        TO_STRING_IMPL("BatteryStats::TimerEntry")

        TimerEntry(
            /* [in] */ const String& name,
            /* [in] */ Int32 id,
            /* [in] */ IBatteryStatsTimer* timer,
            /* [in] */ Int64 time)
            : mName(name)
            , mId(id)
            , mTimer(timer)
            , mTime(time)
        {}

    public:
        String mName;
        Int32 mId;
        AutoPtr<IBatteryStatsTimer> mTimer;
        Int64 mTime;
    };

    class HistoryPrinter
        : public Object
    {
    public:
        TO_STRING_IMPL("BatteryStats::HistoryPrinter")

        HistoryPrinter()
            : mOldState(0)
            , mOldState2(0)
            , mOldLevel(-1)
            , mOldStatus(-1)
            , mOldHealth(-1)
            , mOldPlug(-1)
            , mOldTemp(-1)
            , mOldVolt(-1)
            , mLastTime(-1)
        {}

        CARAPI_(void) PrintNextItem(
            /* [in] */ IPrintWriter* pw,
            /* [in] */ HistoryItem* rec,
            /* [in] */ Int64 baseTime,
            /* [in] */ Boolean checkin,
            /* [in] */ Boolean verbose);

    private:
        CARAPI_(void) Reset();

    public:
        Int32 mOldState;
        Int32 mOldState2;
        Int32 mOldLevel;
        Int32 mOldStatus;
        Int32 mOldHealth;
        Int32 mOldPlug;
        Int32 mOldTemp;
        Int32 mOldVolt;
        Int64 mLastTime;
    };

    class DumpLockedComparator
        : public Object
        , public IComparator
    {
    public:
        TO_STRING_IMPL("BatteryStats::DumpLockedComparator")

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Compare(
            /* [in] */ IInterface* _lhs,
            /* [in] */ IInterface* _rhs,
            /* [out] */ Int32* result);
    };

public:
    CAR_INTERFACE_DECL()

    BatteryStats();

    static CARAPI_(void) FormatTime(
        /* [in] */ StringBuilder& out,
        /* [in] */ Int64 seconds);

    static CARAPI_(void) FormatTimeMs(
        /* [in] */ StringBuilder& out,
        /* [in] */ Int64 seconds);

    static CARAPI_(void) FormatTimeMsNoSpace(
        /* [in] */ StringBuilder& out,
        /* [in] */ Int64 seconds);

    CARAPI_(String) FormatRatioLocked(
        /* [in] */ Int64 num,
        /* [in] */ Int64 seconds);

    CARAPI_(String) FormatBytesLocked(
        /* [in] */ Int64 bytes);

    /**
     * Temporary for settings.
     */
    CARAPI_(void) DumpCheckinLocked(
        /* [in] */ IContext* context,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 which,
        /* [in] */ Int32 reqUid);

    /**
     * Checkin server version of dump to produce more compact, computer-readable log.
     *
     * NOTE: all times are expressed in 'ms'.
     */
    CARAPI_(void) DumpCheckinLocked(
        /* [in] */ IContext* context,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 which,
        /* [in] */ Int32 reqUid,
        /* [in] */ Boolean wifiOnly);

    /**
     * Temporary for settings.
     */
    CARAPI_(void) DumpLocked(
        /* [in] */ IContext* context,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ Int32 which,
        /* [in] */ Int32 reqUid);

    // @SuppressWarnings("unused")
    CARAPI_(void) DumpLocked(
        /* [in] */ IContext* context,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ Int32 which,
        /* [in] */ Int32 reqUid,
        /* [in] */ Boolean wifiOnly);

    CARAPI_(void) PrepareForDumpLocked();

    /**
     * Dumps a human-readable summary of the battery statistics to the given PrintWriter.
     *
     * @param pw a Printer to receive the dump output.
     */
    // @SuppressWarnings("unused")
    CARAPI_(void) DumpLocked(
        /* [in] */ IContext* context,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 reqUid,
        /* [in] */ Int64 histStart);

    // @SuppressWarnings("unused")
    CARAPI_(void) DumpCheckinLocked(
        /* [in] */ IContext* context,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ IList* apps,
        /* [in] */ Int32 flags,
        /* [in] */ Int64 histStart);

private:
    static CARAPI_(AutoPtr< ArrayOf<BitDescription*> >) InitHistoryStateDescriptions();

    static CARAPI_(AutoPtr< ArrayOf<BitDescription*> >) InitHistoryState2Descriptions();

    static CARAPI_(void) FormatTimeRaw(
        /* [in] */ StringBuilder& out,
        /* [in] */ Int64 seconds);

    static CARAPI_(Int64) ComputeWakeLock(
        /* [in] */ IBatteryStatsTimer* timer,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    /**
     *
     * @param sb a StringBuilder object.
     * @param timer a Timer object contining the wakelock times.
     * @param elapsedRealtimeUs the current on-battery time in microseconds.
     * @param name the name of the wakelock.
     * @param which which one of STATS_SINCE_CHARGED, STATS_SINCE_UNPLUGGED, or STATS_CURRENT.
     * @param linePrefix a String to be prepended to each line of output.
     * @return the line prefix
     */
    static CARAPI_(String) PrintWakeLock(
        /* [in] */ StringBuilder& sb,
        /* [in] */ IBatteryStatsTimer* timer,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ const String& name,
        /* [in] */ Int32 which,
        /* [in] */ const String& linePrefix);

    static CARAPI_(String) PrintWakeLockCheckin(
        /* [in] */ StringBuilder& sb,
        /* [in] */ IBatteryStatsTimer* timer,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ const String& name,
        /* [in] */ Int32 which,
        /* [in] */ const String& linePrefix);

    /**
     * Dump a comma-separated line of values for terse checkin mode.
     *
     * @param pw the PageWriter to dump log to
     * @param category category of data (e.g. "total", "last", "unplugged", "current" )
     * @param type type of data (e.g. "wakelock", "sensor", "process", "apk" ,  "process", "network")
     * @param args type-dependent data arguments
     */
    static CARAPI_(void) DumpLine(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 uid,
        /* [in] */ const String& category,
        /* [in] */ const String& type,
        /* [in] */ ArrayOf<IInterface*>* args);

    CARAPI_(void) PrintmAh(
        /* [in] */ IPrintWriter* printer,
        /* [in] */ Double power);

    static CARAPI_(void) PrintBitDescriptions(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 oldval,
        /* [in] */ Int32 newval,
        /* [in] */ HistoryTag* wakelockTag,
        /* [in] */ ArrayOf<BitDescription*>* descriptions,
        /* [in] */ Boolean longNames);

    CARAPI_(void) PrintSizeValue(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int64 size);

    static CARAPI_(Boolean) DumpDurationSteps(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& header,
        /* [in] */ ArrayOf<Int64>* steps,
        /* [in] */ Int32 count,
        /* [in] */ Boolean checkin);

    CARAPI_(void) DumpHistoryLocked(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 flags,
        /* [in] */ Int64 histStart,
        /* [in] */ Boolean checkin);

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

    static const String VERSION_DATA;
    static const String UID_DATA;
    static const String APK_DATA;
    static const String PROCESS_DATA;
    static const String SENSOR_DATA;
    static const String VIBRATOR_DATA;
    static const String FOREGROUND_DATA;
    static const String STATE_TIME_DATA;
    static const String WAKELOCK_DATA;
    static const String SYNC_DATA;
    static const String JOB_DATA;
    static const String KERNEL_WAKELOCK_DATA;
    static const String WAKEUP_REASON_DATA;
    static const String NETWORK_DATA;
    static const String USER_ACTIVITY_DATA;
    static const String BATTERY_DATA;
    static const String BATTERY_DISCHARGE_DATA;
    static const String BATTERY_LEVEL_DATA;
    static const String WIFI_DATA;
    static const String MISC_DATA;
    static const String GLOBAL_NETWORK_DATA;
    static const String HISTORY_STRING_POOL;
    static const String HISTORY_DATA;
    static const String SCREEN_BRIGHTNESS_DATA;
    static const String SIGNAL_STRENGTH_TIME_DATA;
    static const String SIGNAL_SCANNING_TIME_DATA;
    static const String SIGNAL_STRENGTH_COUNT_DATA;
    static const String DATA_CONNECTION_TIME_DATA;
    static const String DATA_CONNECTION_COUNT_DATA;
    static const String WIFI_STATE_TIME_DATA;
    static const String WIFI_STATE_COUNT_DATA;
    static const String WIFI_SUPPL_STATE_TIME_DATA;
    static const String WIFI_SUPPL_STATE_COUNT_DATA;
    static const String WIFI_SIGNAL_STRENGTH_TIME_DATA;
    static const String WIFI_SIGNAL_STRENGTH_COUNT_DATA;
    static const String BLUETOOTH_STATE_TIME_DATA;
    static const String BLUETOOTH_STATE_COUNT_DATA;
    static const String POWER_USE_SUMMARY_DATA;
    static const String POWER_USE_ITEM_DATA;
    static const String DISCHARGE_STEP_DATA;
    static const String CHARGE_STEP_DATA;
    static const String DISCHARGE_TIME_REMAIN_DATA;
    static const String CHARGE_TIME_REMAIN_DATA;

    static const AutoPtr< ArrayOf<String> > SCREEN_BRIGHTNESS_NAMES;

    static const AutoPtr< ArrayOf<String> > SCREEN_BRIGHTNESS_SHORT_NAMES;

    static const AutoPtr< ArrayOf<String> > DATA_CONNECTION_NAMES;

    static const AutoPtr< ArrayOf<String> > WIFI_SUPPL_STATE_NAMES;

    static const AutoPtr< ArrayOf<String> > WIFI_SUPPL_STATE_SHORT_NAMES;

    static const AutoPtr< ArrayOf<BitDescription*> > HISTORY_STATE_DESCRIPTIONS;

    static const AutoPtr< ArrayOf<BitDescription*> > HISTORY_STATE2_DESCRIPTIONS;

    static const AutoPtr< ArrayOf<String> > HISTORY_EVENT_NAMES;

    static const AutoPtr< ArrayOf<String> > HISTORY_EVENT_CHECKIN_NAMES;

    static const AutoPtr< ArrayOf<String> > WIFI_STATE_NAMES;

    static const AutoPtr< ArrayOf<String> > BLUETOOTH_STATE_NAMES;

    AutoPtr<StringBuilder> mFormatBuilder;
    AutoPtr<IFormatter> mFormatter;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Os::BatteryStats::HistoryTag)

template <>
struct Conversion<Elastos::Droid::Os::BatteryStats::BitDescription*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif //__ELASTOS_DROID_OS_BATTERYSTATS_H__
