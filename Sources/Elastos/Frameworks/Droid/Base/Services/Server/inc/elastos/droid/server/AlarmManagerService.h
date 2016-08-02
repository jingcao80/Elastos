
#ifndef __ELASTOS_DROID_SERVER_ALARMMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_ALARMMANAGERSERVICE_H__

#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/Intent.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/Pair.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Thread.h>
#include <Elastos.Droid.App.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IAlarmClockInfo;
using Elastos::Droid::App::IIAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentOnFinished;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Intent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::Internal::Utility::ILocalLog;

using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::Core::IComparator;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICollections;
using Elastos::Utility::IDate;
using Elastos::Utility::IHashMap;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::ILocale;
using Elastos::Utility::ITimeZone;

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Pair;

namespace Elastos {
namespace Droid {
namespace Server {


class AlarmManagerService
    : public SystemService
{
public:
    friend class Alarm;
    friend class FilterStats;
    friend class BroadcastStats;

    class BinderService
        : public Object
        , public IIAlarmManager
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ISystemService* alarmManagerService);

        //@Override
        CARAPI Set(
            /* [in] */ Int32 type,
            /* [in] */ Int64 triggerAtTime,
            /* [in] */ Int64 windowLength,
            /* [in] */ Int64 interval,
            /* [in] */ IPendingIntent* operation,
            /* [in] */ IWorkSource* workSource,
            /* [in] */ IAlarmClockInfo* alarmClock);

        //@Override
        CARAPI SetTime(
            /* [in] */ Int64 millis,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI SetTimeZone(
            /* [in] */ const String& tz);

        //@Override
        CARAPI Remove(
            /* [in] */ IPendingIntent* operation);

        //@Override
        CARAPI GetNextAlarmClock(
            /* [in] */ Int32 userId,
            /* [out] */IAlarmClockInfo** result);

        //@Override
        /* updates the blocked uids, so if a wake lock is acquired to only fire
         * alarm for it, it can be released.
         */
        CARAPI UpdateBlockedUids(
            /* [in] */ Int32 uid,
            /* [in] */ Boolean isBlocked);

        CARAPI ToString(
            /* [out] */ String* str);

    protected:
        //@Override
        CARAPI Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* pw,
            /* [in] */ ArrayOf<String>* args);
    private:
        AlarmManagerService* mHost;
    };

private:
    class PriorityClass
        : public Object
    {
    public:
        PriorityClass(
            /* [in] */ AlarmManagerService* host);

    public:
        Int32 mSeq;
        Int32 mPriority;

    private:
        AlarmManagerService* mHost;
    };

    class WakeupEvent
        : public Object
    {
    public:
        WakeupEvent(
            /* [in] */ Int64 theTime,
            /* [in] */ Int32 theUid,
            /* [in] */ const String& theAction);

    public:
        Int64 mWhen;
        Int32 mUid;
        String mAction;
    };

    class Alarm
        : public Object
    {
    public:
        Alarm(
            /* [in] */ Int32 _type,
            /* [in] */ Int64 _when,
            /* [in] */ Int64 _whenElapsed,
            /* [in] */ Int64 _windowLength,
            /* [in] */ Int64 _maxWhen,
            /* [in] */ Int64 _interval,
            /* [in] */ IPendingIntent* _op,
            /* [in] */ IWorkSource* _ws,
            /* [in] */ IAlarmClockInfo* _info,
            /* [in] */ Int32 _userId);

        static String MakeTag(
            /* [in] */ IPendingIntent* pi,
            /* [in] */ Int32 type);

        //@Override
        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI Dump(
            /* [in] */ IPrintWriter* pw,
            /* [in] */ const String& prefix,
            /* [in] */ Int64 nowRTC,
            /* [in] */ Int64 nowELAPSED,
            /* [in] */ ISimpleDateFormat* sdf);

    public:
        Int32 mType;
        Boolean mWakeup;
        AutoPtr<IPendingIntent> mOperation;
        String mTag;
        AutoPtr<IWorkSource> mWorkSource;
        Int32 mCount;
        Int64 mWhen;
        Int64 mWindowLength;
        Int64 mWhenElapsed;    // 'when' in the elapsed time base
        Int64 mMaxWhen;        // also in the elapsed time base
        Int64 mRepeatInterval;
        AutoPtr<IAlarmClockInfo> mAlarmClock;
        Int32 mUserId;
        AutoPtr<PriorityClass> mPriorityClass;
        Int32 mUid;
        Int32 mPid;
    };

    class BroadcastStats
        : public Object
    {
    public:
        BroadcastStats(
            /* [in] */ Int32 uid,
            /* [in] */ const String& packageName);

    public:
        Int32 mUid;
        String mPackageName;

        Int64 mAggregateTime;
        Int32 mCount;
        Int32 mNumWakeup;
        Int64 mStartTime;
        Int32 mNesting;
        AutoPtr<IArrayMap> mFilterStats;// new ArrayMap<String, FilterStats>();
    };

    class FilterStats
        : public Object
    {
    public:
        FilterStats(
            /* [in] */ BroadcastStats* broadcastStats,
            /* [in]i */ const String& tag);

    public:
        AutoPtr<BroadcastStats> mBroadcastStats;
        String mTag;

        Int64 mAggregateTime;
        Int32 mCount;
        Int32 mNumWakeup;
        Int64 mStartTime;
        Int32 mNesting;
    };

    class InFlight
        : public Intent
    {
    public:
        InFlight();

        CARAPI constructor(
            /* [in] */ AlarmManagerService* service,
            /* [in] */ IPendingIntent* pendingIntent,
            /* [in] */ IWorkSource* workSource,
            /* [in] */ Int32 alarmType,
            /* [in] */ const String& tag,
            /* [in] */ Int32 uid);

    public:
        AutoPtr<IPendingIntent> mPendingIntent;
        AutoPtr<IWorkSource> mWorkSource;
        String mTag;
        AutoPtr<BroadcastStats> mBroadcastStats;
        AutoPtr<FilterStats> mFilterStats;
        Int32 mAlarmType;
        Int32 mUid;
    };

    class Batch
        : public Object
    {
    public:
        Batch(
            /* [in] */ AlarmManagerService* host);

        Batch(
            /* [in] */ Alarm* seed,
            /* [in] */ AlarmManagerService* host);

        Int32 Size();

        AutoPtr<Alarm> Get(
            /* [in] */ Int32 index);

        Int64 GetWhenByElapsedTime(
            /* [in] */ Int64 whenElapsed);

        Boolean CanHold(
            /* [in] */ Int64 whenElapsed,
            /* [in] */ Int64 maxWhen);

        Boolean Add(
            /* [in] */ Alarm* alarm);

        Boolean Remove(
            /* [in] */ IPendingIntent* operation);

        Boolean Remove(
            /* [in] */ const String& packageName);

        Boolean Remove(
            /* [in] */ Int32 userHandle);

        Boolean HasPackage(
            /* [in] */ const String& packageName);

        Boolean HasWakeups();

        Boolean IsRtcPowerOffWakeup();

        //@Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        Int64 mStart;     // These endpoints are always in ELAPSED
        Int64 mEnd;
        Boolean mStandalone; // certain "batches" don't participate in coalescing

        AutoPtr<IArrayList> mAlarms; // new ArrayList<Alarm>();
        AlarmManagerService* mHost;

    };

    class BatchTimeOrder
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* o1, //Batch b1,
            /* [in] */ IInterface* o2, //Batch b2
            /* [out] */ Int32* result);
    };

    class AlarmDispatchComparator
        : public Object
        , public IComparator
    {
        CAR_INTERFACE_DECL()

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* o1, //Alarm b1,
            /* [in] */ IInterface* o2, //Alarm b2
            /* [out] */ Int32* result);
    };

    class AlarmThread
        : public Thread
    {
    public:
        AlarmThread(
            /* [in] */ AlarmManagerService* host);

        CARAPI Run();

    private:
        AlarmManagerService* mHost;
    };

    class AlarmHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("AlarmManagerService::AlarmHandler")

        AlarmHandler(
            /* [in] */ AlarmManagerService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        static const Int32 ALARM_EVENT;
        static const Int32 MINUTE_CHANGE_EVENT;
        static const Int32 DATE_CHANGE_EVENT;
        static const Int32 SEND_NEXT_ALARM_CLOCK_CHANGED;

    private:
        AlarmManagerService* mHost;
    };

    class QuickBootReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("AlarmManagerService::QuickBootReceiver")

        QuickBootReceiver(
            /* [in] */ AlarmManagerService* host);

        CARAPI constructor();

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    public:
        static const String ACTION_APP_KILL;
        AlarmManagerService* mHost;
    };

    class ClockReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("AlarmManagerService::ClockReceiver")

        ClockReceiver(
            /* [in] */ AlarmManagerService* host);

        CARAPI constructor();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ScheduleTimeTickEvent();

        CARAPI ScheduleDateChangedEvent();

    private:
        AlarmManagerService* mHost;
    };

    class InteractiveStateReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("AlarmManagerService::InteractiveStateReceiver")

        InteractiveStateReceiver(
            /* [in] */ AlarmManagerService* host);

        CARAPI constructor();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        AlarmManagerService* mHost;
    };

    class UninstallReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("AlarmManagerService::UninstallReceiver")

        UninstallReceiver(
            /* [in] */ AlarmManagerService* host);

        CARAPI constructor();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        AlarmManagerService* mHost;
    };

    class ResultReceiver
        : public Object
        , public IPendingIntentOnFinished
    {
    public:
        ResultReceiver(
            /* [in] */ AlarmManagerService* host);

        CAR_INTERFACE_DECL()

        CARAPI OnSendFinished(
            /* [in] */ IPendingIntent* pi,
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 resultCode,
            /* [in] */ const String& resultData,
            /* [in] */ IBundle* resultExtras);

    private:
        AlarmManagerService* mHost;
    };

    /**
     * This Comparator sorts Alarms into increasing time order.
     */
    class IncreasingTimeOrder
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* o1,
            /* [in] */ IInterface* o2,
            /* [out] */ Int32* result);
    };

public:

    CARAPI CalculateDeliveryPriorities(
        /* [in] */ IArrayList* alarms);

    AlarmManagerService();

    virtual ~AlarmManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    static CARAPI_(Int64) ConvertToElapsed(
        /* [in] */ Int64 when,
        /* [in] */ Int32 type);

    // Apply a heuristic to { recurrence interval, futurity of the trigger time } to
    // calculate the end of our nominal delivery window for the alarm.
    static CARAPI_(Int64) MaxTriggerTime(
        /* [in] */ Int64 now,
        /* [in] */ Int64 triggerAtTime,
        /* [in] */ Int64 interval);

    // returns TRUE if the batch was added at the head
    static CARAPI_(Boolean) AddBatchLocked(
        /* [in] */ IArrayList* list,
        /* [in] */ Batch* newBatch);

    // Return the index of the matching batch, or -1 if none found.
    CARAPI_(Int32) AttemptCoalesceLocked(
        /* [in] */ Int64 whenElapsed,
        /* [in] */ Int64 maxWhen);

    // The RTC clock has moved arbitrarily, so we need to recalculate all the batching
    CARAPI_(void) RebatchAllAlarms();

    CARAPI_(void) RebatchAllAlarmsLocked(
        /* [in] */ Boolean doValidate);

    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI Finalize();

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(void) SetTimeZoneImpl(
        /* [in] */ const String& tz);

    CARAPI_(void) RemoveImpl(
        /* [in] */ IPendingIntent* operation);

    CARAPI SetImpl(
        /* [in] */ Int32 type,
        /* [in] */ Int64 triggerAtTime,
        /* [in] */ Int64 windowLength,
        /* [in] */ Int64 interval,
        /* [in] */ IPendingIntent* operation,
        /* [in] */ Boolean isStandalone,
        /* [in] */ IWorkSource* workSource,
        /* [in] */ IAlarmClockInfo* alarmClock);

    CARAPI_(void) SetImplLocked(
        /* [in] */ Int32 type,
        /* [in] */ Int64 when,
        /* [in] */ Int64 whenElapsed,
        /* [in] */ Int64 windowLength,
        /* [in] */ Int64 maxWhen,
        /* [in] */ Int64 interval,
        /* [in] */ IPendingIntent* operation,
        /* [in] */ Boolean isStandalone,
        /* [in] */ Boolean doValidate,
        /* [in] */ IWorkSource* workSource,
        /* [in] */ IAlarmClockInfo* alarmClock,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean wakeupFiltered);

    CARAPI_(void) DumpImpl(
        /* [in] */ IPrintWriter* pw);

    CARAPI_(void) LogBatchesLocked(
        /* [in] */ ISimpleDateFormat* sdf);

    CARAPI_(Boolean) ValidateConsistencyLocked();

    CARAPI_(AutoPtr<Batch>) FindFirstWakeupBatchLocked();

    CARAPI_(AutoPtr<Batch>) FindFirstRtcWakeupBatchLocked();

    CARAPI_(AutoPtr<IAlarmClockInfo>) GetNextAlarmClockImpl(
        /* [in] */ Int32 userId);

    /**
     * Recomputes the next alarm clock for all users.
     */
    CARAPI_(void) UpdateNextAlarmClockLocked();

    CARAPI_(void) UpdateNextAlarmInfoForUserLocked(
        /* [in] */ Int32 userId,
        /* [in] */ IAlarmClockInfo* alarmClock);

    /**
     * Updates NEXT_ALARM_FORMATTED and sends NEXT_ALARM_CLOCK_CHANGED_INTENT for all users
     * for which alarm clocks have changed since the last call to this.
     *
     * Do not call with a lock held. Only call from mHandler's thread.
     *
     * @see AlarmHandler#SEND_NEXT_ALARM_CLOCK_CHANGED
     */
    CARAPI_(void) SendNextAlarmClockChanged();

    /**
     * Formats an alarm like platform/packages/apps/DeskClock used to.
     */
    static CARAPI_(String) FormatNextAlarm(
        /* [in] */ IContext* context,
        /* [in] */ IAlarmClockInfo* info);

    CARAPI_(void) RescheduleKernelAlarmsLocked();

    CARAPI_(Boolean) CheckReleaseWakeLock();

    CARAPI_(void) RemoveLocked(
        /* [in] */ IPendingIntent* operation);

    CARAPI_(Boolean) RemoveWithStatusLocked(
        /* [in] */ IPendingIntent* operation);

    CARAPI_(void) RemoveLocked(
        /* [in] */ const String& packageName);

    CARAPI_(void) RemoveUserLocked(
        /* [in] */ Int32 userHandle);

    CARAPI_(void) InteractiveStateChangedLocked(
        /* [in] */ Boolean interactive);

    CARAPI_(Boolean) LookForPackageLocked(
        /* [in] */ const String& packageName);

    CARAPI_(void) SetLocked(
        /* [in] */ Int32 type,
        /* [in] */ Int64 when);

    static CARAPI_(void) DumpAlarmList(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ IArrayList* list,
        /* [in] */ const String& prefix,
        /* [in] */ const String& label,
        /* [in] */ Int64 nowRTC,
        /* [in] */ Int64 nowELAPSED,
        /* [in] */ ISimpleDateFormat* sdf);

    static CARAPI_(String) LabelForType(
        /* [in] */ Int32 type);

    static CARAPI_(void) DumpAlarmList(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ IArrayList* list,
        /* [in] */ const String& prefix,
        /* [in] */ Int64 nowELAPSED,
        /* [in] */ Int64 nowRTC,
        /* [in] */ ISimpleDateFormat* sdf);

    CARAPI_(Boolean) TriggerAlarmsLocked(
        /* [in] */ IArrayList* triggerList,
        /* [in] */ Int64 nowELAPSED,
        /* [in] */ Int64 nowRTC);

    CARAPI_(void) RecordWakeupAlarms(
        /* [in] */ IArrayList* batches,
        /* [in] */ Int64 nowELAPSED,
        /* [in] */ Int64 nowRTC);

    CARAPI_(Int64) CurrentNonWakeupFuzzLocked(
        /* [in] */ Int64 nowELAPSED);

    CARAPI_(Boolean) CheckAllowNonWakeupDelayLocked(
        /* [in] */ Int64 nowELAPSED);

    CARAPI_(void) DeliverAlarmsLocked(
        /* [in] */ IArrayList* triggerList,
        /* [in] */ Int64 nowELAPSED);

    CARAPI_(void) FiltQuickBootAlarms(
        /* [in] */ IArrayList* triggerList);

    /**
     * Attribute blame for a WakeLock.
     * @param pi PendingIntent to attribute blame to if ws is NULL.
     * @param ws WorkSource to attribute blame.
     */
    CARAPI_(void) SetWakelockWorkSource(
        /* [in] */ IPendingIntent* pi,
        /* [in] */ IWorkSource* ws,
        /* [in] */ Int32 type,
        /* [in] */ const String& tag,
        /* [in] */ Boolean first);

    CARAPI_(AutoPtr<BroadcastStats>) GetStatsLocked(
        /* [in] */ IPendingIntent* pi);

    Int64 Native_Init();
    void Native_Close(Int64 nativeData);
    void Native_Set(Int64 nativeData, Int32 type, Int64 seconds, Int64 nanoseconds);
    void Native_Clear(Int64 nativeData, Int32 type, Int64 seconds, Int64 nanoseconds);
    Int32 Native_WaitForAlarm(Int64 nativeData);
    Int32 Native_SetKernelTime(Int64 nativeData, Int64 millis);
    Int32 Native_SetKernelTimezone(Int64 nativeData, Int32 minuteswest);

private:
    // The threshold for how Int64 an alarm can be late before we print a
    // warning message.  The time duration is in milliseconds.
    static const Int64 LATE_ALARM_THRESHOLD;

    // The threshold for the power off alarm time can be set. The time
    // duration is in milliseconds.
    static const Int64 POWER_OFF_ALARM_THRESHOLD;

    static const Int32 RTC_WAKEUP_MASK;
    static const Int32 RTC_MASK;
    static const Int32 ELAPSED_REALTIME_WAKEUP_MASK;
    static const Int32 ELAPSED_REALTIME_MASK;
    static const Int32 RTC_POWEROFF_WAKEUP_MASK;
    static const Int32 TIME_CHANGED_MASK;
    static const Int32 IS_WAKEUP_MASK;

    // Mask for testing whether a given alarm type is wakeup vs non-wakeup
    static const Int32 TYPE_NONWAKEUP_MASK; // low bit => non-wakeup

    static const String TAG;
    static const String ClockReceiver_TAG;
    static const Boolean localLOGV;
    static const Boolean DEBUG_BATCH;
    static const Boolean DEBUG_VALIDATE;
    static const Boolean DEBUG_ALARM_CLOCK;
    static const Int32 ALARM_EVENT;
    static const String TIMEZONE_PROPERTY ;

    static const AutoPtr<IIntent> mBackgroundIntent;
    static const AutoPtr<IncreasingTimeOrder> sIncreasingTimeOrder;

    static const Boolean WAKEUP_STATS;

    static const AutoPtr<IIntent> NEXT_ALARM_CLOCK_CHANGED_INTENT;

    AutoPtr<ILocalLog> mLog;

    Object mLock;

    AutoPtr<IArrayList> mTriggeredUids;
    AutoPtr<IArrayList> mBlockedUids;

    Int64 mNativeData;
    Int64 mNextWakeup;
    Int64 mNextRtcWakeup;
    Int64 mNextNonWakeup;
    Int32 mBroadcastRefCount;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    Boolean mLastWakeLockUnimportantForLogging;

    AutoPtr<IArrayList> mPendingNonWakeupAlarms;//new ArrayList<Alarm>();
    AutoPtr<IArrayList> mInFlight;//new ArrayList<InFlight>();

    AutoPtr<AlarmHandler> mHandler;
    AutoPtr<ClockReceiver> mClockReceiver;
    AutoPtr<InteractiveStateReceiver> mInteractiveStateReceiver;
    AutoPtr<UninstallReceiver> mUninstallReceiver;
    AutoPtr<QuickBootReceiver> mQuickBootReceiver;
    AutoPtr<ResultReceiver> mResultReceiver;
    AutoPtr<IPendingIntent> mTimeTickSender;
    AutoPtr<IPendingIntent> mDateChangeSender;
    Boolean mInteractive;
    Int64 mNonInteractiveStartTime;
    Int64 mNonInteractiveTime;
    Int64 mLastAlarmDeliveryTime;
    Int64 mStartCurrentDelayTime;
    Int64 mNextNonWakeupDeliveryTime;
    Int32 mNumTimeChanged;

    AutoPtr<ISparseArray> mNextAlarmClockForUser;
    AutoPtr<ISparseArray> mTmpSparseAlarmClockArray;
    AutoPtr<ISparseBooleanArray> mPendingSendNextAlarmClockChangedForUser;
    Boolean mNextAlarmClockMayChange;

    // May only use on mHandler's thread, locking not required.
    AutoPtr<ISparseArray> mHandlerSparseAlarmClockArray;// = new SparseArray<>();

    AutoPtr<IAppOpsManager> mAppOps;

    // Alarm delivery ordering bookkeeping
    static const Int32 PRIO_TICK;
    static const Int32 PRIO_WAKEUP;
    static const Int32 PRIO_NORMAL;

    HashMap<String, AutoPtr<PriorityClass> > mPriorities;
    Int32 mCurrentSeq ;

    AutoPtr<ILinkedList> mRecentWakeups;// = new LinkedList<WakeupEvent>();
    Int64 RECENT_WAKEUP_PERIOD; // one day

    AutoPtr<IComparator> mAlarmDispatchComparator;

    // minimum recurrence period or alarm futurity for us to be able to fuzz it
    static const Int64 MIN_FUZZABLE_INTERVAL;
    static const AutoPtr<BatchTimeOrder> sBatchOrder;// = new BatchTimeOrder();
    AutoPtr<IArrayList> mAlarmBatches;// = new ArrayList<Batch>();

    AutoPtr<ISparseArray> mBroadcastStats;//= new SparseArray<ArrayMap<String, BroadcastStats>>();

    Int32 mNumDelayedAlarms;
    Int64 mTotalDelayTime;
    Int64 mMaxDelayTime;

    AutoPtr<IBinder> mService;
};


} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CALARMMANAGERSERVICE_H__
