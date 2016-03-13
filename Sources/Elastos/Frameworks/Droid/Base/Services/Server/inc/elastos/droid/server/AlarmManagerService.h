
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
        Int32 mSeq;
        Int32 mPriority;

        PriorityClass(
            /* [in] */ AlarmManagerService* host);

    private:
        AlarmManagerService* mHost;
    };

    class WakeupEvent
        : public Object
    {
    public:
        Int64 mWhen;
        Int32 mUid;
        String mAction;

        WakeupEvent(
            /* [in] */ Int64 theTime,
            /* [in] */ Int32 theUid,
            /* [in] */ const String& theAction);
    };

    class Alarm
        : public Object
    {
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
    };

    class BroadcastStats
        : public Object
    {
    public:
        Int32 mUid;
        String mPackageName;

        Int64 mAggregateTime;
        Int32 mCount;
        Int32 mNumWakeup;
        Int64 mStartTime;
        Int32 mNesting;
        AutoPtr<IArrayMap> mFilterStats;// new ArrayMap<String, FilterStats>();

        BroadcastStats(
            /* [in] */ Int32 uid,
            /* [in] */ const String& packageName);
    };

    class FilterStats
        : public Object
    {
    public:
        AutoPtr<BroadcastStats> mBroadcastStats;
        String mTag;

        Int64 mAggregateTime;
        Int32 mCount;
        Int32 mNumWakeup;
        Int64 mStartTime;
        Int32 mNesting;

        FilterStats(
            /* [in] */ BroadcastStats* broadcastStats,
            /* [in]i */ const String& tag);
    };

    class InFlight
        : public Intent
    {
    public:
        AutoPtr<IPendingIntent> mPendingIntent;
        AutoPtr<IWorkSource> mWorkSource;
        String mTag;
        AutoPtr<BroadcastStats> mBroadcastStats;
        AutoPtr<FilterStats> mFilterStats;
        Int32 mAlarmType;

        InFlight();

        CARAPI constructor(
            /* [in] */ AlarmManagerService* service,
            /* [in] */ IPendingIntent* pendingIntent,
            /* [in] */ IWorkSource* workSource,
            /* [in] */ Int32 alarmType,
            /* [in] */ const String& tag);
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

    class ClockReceiver
        : public BroadcastReceiver
    {
    public:
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

    static Int64 ConvertToElapsed(
        /* [in] */ Int64 when,
        /* [in] */ Int32 type);

    // Apply a heuristic to { recurrence interval, futurity of the trigger time } to
    // calculate the end of our nominal delivery window for the alarm.
    static Int64 MaxTriggerTime(
        /* [in] */ Int64 now,
        /* [in] */ Int64 triggerAtTime,
        /* [in] */ Int64 interval);

    // returns TRUE if the batch was added at the head
    static Boolean AddBatchLocked(
        /* [in] */ IArrayList* list,
        /* [in] */ Batch* newBatch);

    // Return the index of the matching batch, or -1 if none found.
    Int32 AttemptCoalesceLocked(
        /* [in] */ Int64 whenElapsed,
        /* [in] */ Int64 maxWhen);

    // The RTC clock has moved arbitrarily, so we need to recalculate all the batching
    void RebatchAllAlarms();

    void RebatchAllAlarmsLocked(
        /* [in] */ Boolean doValidate);

    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI Finalize();

    CARAPI ToString(
        /* [out] */ String* str);

private:

    void SetTimeZoneImpl(
        /* [in] */ const String& tz);

    void RemoveImpl(
        /* [in] */ IPendingIntent* operation);

    ECode SetImpl(
        /* [in] */ Int32 type,
        /* [in] */ Int64 triggerAtTime,
        /* [in] */ Int64 windowLength,
        /* [in] */ Int64 interval,
        /* [in] */ IPendingIntent* operation,
        /* [in] */ Boolean isStandalone,
        /* [in] */ IWorkSource* workSource,
        /* [in] */ IAlarmClockInfo* alarmClock);

    void SetImplLocked(
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
        /* [in] */ Int32 userId);

    void DumpImpl(
        /* [in] */ IPrintWriter* pw);

    void LogBatchesLocked(
        /* [in] */ ISimpleDateFormat* sdf);

    Boolean ValidateConsistencyLocked();

    AutoPtr<Batch> FindFirstWakeupBatchLocked();

    AutoPtr<IAlarmClockInfo> GetNextAlarmClockImpl(
        /* [in] */ Int32 userId);

    /**
     * Recomputes the next alarm clock for all users.
     */
    void UpdateNextAlarmClockLocked();

    void UpdateNextAlarmInfoForUserLocked(
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
    void SendNextAlarmClockChanged();

    /**
     * Formats an alarm like platform/packages/apps/DeskClock used to.
     */
    static String FormatNextAlarm(
        /* [in] */ IContext* context,
        /* [in] */ IAlarmClockInfo* info);

    void RescheduleKernelAlarmsLocked();

    void RemoveLocked(
        /* [in] */ IPendingIntent* operation);

    void RemoveLocked(
        /* [in] */ const String& packageName);

    void RemoveUserLocked(
        /* [in] */ Int32 userHandle);

    void InteractiveStateChangedLocked(
        /* [in] */ Boolean interactive);

    Boolean LookForPackageLocked(
        /* [in] */ const String& packageName);

    void SetLocked(
        /* [in] */ Int32 type, Int64 when);

    static void DumpAlarmList(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ IArrayList* list,
        /* [in] */ const String& prefix,
        /* [in] */ const String& label,
        /* [in] */ Int64 nowRTC,
        /* [in] */ Int64 nowELAPSED,
        /* [in] */ ISimpleDateFormat* sdf);

    static String LabelForType(
        /* [in] */ Int32 type);

    static void DumpAlarmList(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ IArrayList* list,
        /* [in] */ const String& prefix,
        /* [in] */ Int64 nowELAPSED,
        /* [in] */ Int64 nowRTC,
        /* [in] */ ISimpleDateFormat* sdf);

    Boolean TriggerAlarmsLocked(
        /* [in] */ IArrayList* triggerList,
        /* [in] */ Int64 nowELAPSED,
        /* [in] */ Int64 nowRTC);

    void RecordWakeupAlarms(
        /* [in] */ IArrayList* batches,
        /* [in] */ Int64 nowELAPSED,
        /* [in] */ Int64 nowRTC);

    Int64 CurrentNonWakeupFuzzLocked(
        /* [in] */ Int64 nowELAPSED);

    Boolean CheckAllowNonWakeupDelayLocked(
        /* [in] */ Int64 nowELAPSED);

    void DeliverAlarmsLocked(
        /* [in] */ IArrayList* triggerList,
        /* [in] */ Int64 nowELAPSED);

    /**
     * Attribute blame for a WakeLock.
     * @param pi PendingIntent to attribute blame to if ws is NULL.
     * @param ws WorkSource to attribute blame.
     */
    void SetWakelockWorkSource(
        /* [in] */ IPendingIntent* pi,
        /* [in] */ IWorkSource* ws,
        /* [in] */ Int32 type,
        /* [in] */ const String& tag,
        /* [in] */ Boolean first);

    AutoPtr<BroadcastStats> GetStatsLocked(
        /* [in] */ IPendingIntent* pi);

    Int64 Native_Init();
    void Native_Close(Int64 nativeData);
    void Native_Set(Int64 nativeData, Int32 type, Int64 seconds, Int64 nanoseconds);
    Int32 Native_WaitForAlarm(Int64 nativeData);
    Int32 Native_SetKernelTime(Int64 nativeData, Int64 millis);
    Int32 Native_SetKernelTimezone(Int64 nativeData, Int32 minuteswest);

private:
    // The threshold for how Int64 an alarm can be late before we print a
    // warning message.  The time duration is in milliseconds.
    static const Int64 LATE_ALARM_THRESHOLD;

    static const Int32 RTC_WAKEUP_MASK;
    static const Int32 RTC_MASK;
    static const Int32 ELAPSED_REALTIME_WAKEUP_MASK;
    static const Int32 ELAPSED_REALTIME_MASK;
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

    Int64 mNativeData;
    Int64 mNextWakeup;
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
