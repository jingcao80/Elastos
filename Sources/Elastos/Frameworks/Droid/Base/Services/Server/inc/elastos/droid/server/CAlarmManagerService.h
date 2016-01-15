
#ifndef __ELASTOS_DROID_SERVER_CALARMMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CALARMMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CAlarmManagerService.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/Pair.h>
#include <elastos/core/StringBuffer.h>
#include "elastos/droid/os/HandlerBase.h"

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Pair;
using Elastos::Core::StringBuffer;
using Elastos::Core::IComparator;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentOnFinished;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::IIntentFilterComparison;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Net::IUri;

_ETL_NAMESPACE_BEGIN

// Hash functor for Pair<String, AutoPtr<IComponentName> >
template<> struct Hash< Pair<String, AutoPtr<IComponentName> > >
{
    size_t operator()(const Pair<String, AutoPtr<IComponentName> >& p) const
    {
        if (p.mFirst.IsNull()) {
            return -1;
        }

        Int32 firstHashCode = p.mFirst.GetHashCode();
        Int32 secondHashCode = 0;
        if (p.mSecond != NULL) {
            p.mSecond->GetHashCode(&secondHashCode);
        }

        const Int32 prime = 31;
        Int32 result = 1;
        result = prime * result + firstHashCode;
        result = prime * result + secondHashCode;
        return result;
    }
};

// EqulaTo functor for Pair<String, AutoPtr<IComponentName> >
template<> struct EqualTo< Pair<String, AutoPtr<IComponentName> > >
{
    size_t operator()(
        const Pair<String, AutoPtr<IComponentName> >& x,
        const Pair<String, AutoPtr<IComponentName> >& y) const
    {
        if (!x.mFirst.Equals(y.mFirst)) {
            return FALSE;
        }

        Boolean equals = (x.mSecond == NULL && x.mSecond == NULL);
        if (!equals && x.mSecond != NULL && y.mSecond != NULL) {
            x.mSecond->Equals(y.mSecond, &equals);
        }

        return equals;
    }
};

_ETL_NAMESPACE_END

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CAlarmManagerService)
{
private:
    class Alarm : public ElRefBase
    {
    public:
        Alarm();

    public:
        Int32 mType;
        Int32 mCount;
        Int64 mWhen;
        Int64 mRepeatInterval;
        AutoPtr<IPendingIntent> mOperation;
    };

    /**
     * This Comparator sorts Alarms into increasing time order.
     */
    // class IncreasingTimeOrder
    //     //: public IComparator
    // {
    // public:
    //     CARAPI Compare(
    //         ///* [in] */ IInterface* a1,
    //         ///* [in] */ IInterface* a2,
    //         /* [in] */ Alarm* a1,
    //         /* [in] */ Alarm* a2,
    //         /* [out] */ Int32* result);
    // };

    class BroadcastStats;
    class FilterStats;

    class InFlight
        : public ElRefBase
    {
    public:
        InFlight(
            /* [in] */ CAlarmManagerService* service,
            /* [in] */ IPendingIntent* pendingIntent);

        AutoPtr<IPendingIntent> mPendingIntent;
        Pair<String, AutoPtr<IComponentName> > mTarget;
        AutoPtr<BroadcastStats> mBroadcastStats;
        AutoPtr<FilterStats> mFilterStats;
    };

    class FilterStats : public ElRefBase
    {
    public:
        FilterStats(
            /* [in] */ BroadcastStats* broadcastStats,
            /* [in] */ Pair<String, AutoPtr<IComponentName> > target);

        AutoPtr<BroadcastStats> mBroadcastStats;
        Pair<String, AutoPtr<IComponentName> > mTarget;

        Int64 mAggregateTime;
        Int32 mCount;
        Int32 mNumWakeup;
        Int64 mStartTime;
        Int32 mNesting;
    };

    class BroadcastStats : public ElRefBase
    {
    public:
        BroadcastStats(
            /* [in] */ const String& packageName);

        virtual ~BroadcastStats();

        String mPackageName;
        Int64 mAggregateTime;
        Int32 mCount;
        Int32 mNumWakeup;
        Int64 mStartTime;
        Int32 mNesting;
        HashMap<Pair<String, AutoPtr<IComponentName> >, AutoPtr<FilterStats> > mFilterStats;
    };

    class AlarmThread
        : public ThreadBase
    {
    public:
        AlarmThread(
            /* [in] */ CAlarmManagerService* host);

        virtual CARAPI Run();

    private:
        CAlarmManagerService* mHost;
    };

    class AlarmHandler
        : public HandlerBase
    {
    public:
        static const Int32 ALARM_EVENT = 1;
        static const Int32 MINUTE_CHANGE_EVENT = 2;
        static const Int32 DATE_CHANGE_EVENT = 3;

        AlarmHandler(
            /* [in] */ CAlarmManagerService* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CAlarmManagerService* mHost;
    };

    class ClockReceiver
        : public BroadcastReceiver
    {
    public:
        ClockReceiver(
            /* [in] */ CAlarmManagerService* host);

        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CAlarmManagerService::ClockReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }

        CARAPI Register();

        CARAPI_(void) ScheduleTimeTickEvent();

        CARAPI_(void) ScheduleDateChangedEvent();

    private:
        CAlarmManagerService* mHost;
    };

    class UninstallReceiver : public BroadcastReceiver
    {
    public:
        UninstallReceiver(
            /* [in] */ CAlarmManagerService* host);

        CARAPI Register();

        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CAlarmManagerService::UninstallReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CAlarmManagerService* mHost;
    };

    class ResultReceiver
        : public ElRefBase
        , public IPendingIntentOnFinished

    {
    public:
        ResultReceiver(
            /* [in] */ CAlarmManagerService* host);

        CAR_INTERFACE_DECL();

        CARAPI OnSendFinished(
            /* [in] */ IPendingIntent* pendingIntent,
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 resultCode,
            /* [in] */ const String& resultData,
            /* [in] */ IBundle* resultExtras);

    private:
        CAlarmManagerService* mHost;
    };

public:
    CAlarmManagerService();

    ~CAlarmManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI Set(
        /* [in] */ Int32 type,
        /* [in] */ Int64 triggerAtTime,
        /* [in] */ IPendingIntent* operation);

    CARAPI SetRepeating(
        /* [in] */ Int32 type,
        /* [in] */ Int64 triggerAtTime,
        /* [in] */ Int64 interval,
        /* [in] */ IPendingIntent* operation);

    CARAPI SetInexactRepeating(
        /* [in] */ Int32 type,
        /* [in] */ Int64 triggerAtTime,
        /* [in] */ Int64 interval,
        /* [in] */ IPendingIntent* operation);

    CARAPI SetTime(
        /* [in] */ Int64 millis);

    CARAPI SetTimeZone(
        /* [in] */ const String& tz);

    CARAPI Remove(
        /* [in] */ IPendingIntent* operation);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    CARAPI_(void) RemoveLocked(
        /* [in] */ IPendingIntent* operation);

    CARAPI_(void) RemoveLocked(
        /* [in] */ const String& packageName);

    CARAPI_(void) RemoveUserLocked(
        /* [in] */ Int32 userHandle);

    CARAPI_(Boolean) LookForPackageLocked(
        /* [in] */ const String& packageName);

    CARAPI_(Int64) TimeToNextAlarm();

private:
    CARAPI_(void) RemoveLocked(
        /* [in] */ List<AutoPtr<CAlarmManagerService::Alarm> >*  alarmList,
        /* [in] */ IPendingIntent* operation);

    CARAPI_(void) RemoveLocked(
        /* [in] */ List<AutoPtr<CAlarmManagerService::Alarm> >*  alarmList,
        /* [in] */ const String& packageName);

    CARAPI_(void) RemoveUserLocked(
        /* [in] */ List<AutoPtr<CAlarmManagerService::Alarm> >* alarmList,
        /* [in] */ Int32 userHandle);

    CARAPI_(Boolean) LookForPackageLocked(
        /* [in] */ List<AutoPtr<CAlarmManagerService::Alarm> >* alarmList,
        /* [in] */ const String& packageName);

    CARAPI_(AutoPtr< List<AutoPtr<CAlarmManagerService::Alarm> > >) GetAlarmList(
        /* [in] */ Int32 type);

    CARAPI_(Int32) AddAlarmLocked(
        /* [in] */ AutoPtr<CAlarmManagerService::Alarm> alarm);

    CARAPI_(void) SetLocked(
        /* [in] */ AutoPtr<CAlarmManagerService::Alarm> alarm);

    CARAPI_(void) TriggerAlarmsLocked(
        /* [in] */ List<AutoPtr<CAlarmManagerService::Alarm> >* alarmList,
        /* [in] */ List<AutoPtr<CAlarmManagerService::Alarm> >* triggerList,
        /* [in] */ Int64 now);

    CARAPI_(void) SetWakelockWorkSource(
        /* [in] */ IPendingIntent* pi);

    CARAPI_(AutoPtr<CAlarmManagerService::BroadcastStats>) GetStatsLocked(
        /* [in] */ IPendingIntent* pi);

    CARAPI_(Int32) NativeInit();

    CARAPI_(void) NativeClose(
        /* [in] */ Int32 fd);

    CARAPI_(void) NativeSet(
        /* [in] */ Int32 fd,
        /* [in] */ Int32 type,
        /* [in] */ Int64 seconds,
        /* [in] */ Int64 nanoseconds);

    CARAPI_(Int32) NativeWaitForAlarm(
        /* [in] */ Int32 fd);

    CARAPI_(Int32) NativeSetKernelTimezone(
        /* [in] */ Int32 fd,
        /* [in] */ Int32 minuteswest);

private:
    // The threshold for how long an alarm can be late before we print a
    // warning message.  The time duration is in milliseconds.
    static const Int64 LATE_ALARM_THRESHOLD;

    static const Int32 RTC_WAKEUP_MASK;
    static const Int32 RTC_MASK;
    static const Int32 ELAPSED_REALTIME_WAKEUP_MASK;
    static const Int32 ELAPSED_REALTIME_MASK;
    static const Int32 TIME_CHANGED_MASK;
    static const Int32 RTC_SHUTDOWN_WAKEUP_MASK;

    // Alignment quantum for inexact repeating alarms
    static const Int64 QUANTUM;

    static const String TAG;
    static const String ClockReceiver_TAG;
    static const Boolean localLOGV;
    static const Int32 ALARM_EVENT;
    static const String TIMEZONE_PROPERTY;

    static const AutoPtr<IIntent> mBackgroundIntent;

private:
    AutoPtr<IContext> mContext;

    Object mLock;
    Object mThisLock;

    typedef List<AutoPtr<Alarm> > AlarmList;
    typedef typename AlarmList::Iterator AlarmListIterator;
    AutoPtr<AlarmList> mRtcWakeupShutdownAlarms;
    AutoPtr<AlarmList> mRtcWakeupAlarms;
    AutoPtr<AlarmList> mRtcAlarms;
    AutoPtr<AlarmList> mElapsedRealtimeWakeupAlarms;
    AutoPtr<AlarmList> mElapsedRealtimeAlarms;

    //AutoPtr<IncreasingTimeOrder> mIncreasingTimeOrder;

    Int32 mDescriptor;
    Int32 mBroadcastRefCount;

    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    List<AutoPtr<InFlight> > mInFlight;
    // List<AutoPtr<IPendingIntent> > mInFlight;
    AutoPtr<AlarmThread> mWaitThread;
    AutoPtr<AlarmHandler> mHandler;
    AutoPtr<ClockReceiver> mClockReceiver;
    AutoPtr<UninstallReceiver> mUninstallReceiver;
    AutoPtr<ResultReceiver> mResultReceiver;

    AutoPtr<IPendingIntent> mTimeTickSender;
    AutoPtr<IPendingIntent> mDateChangeSender;

    HashMap<String, AutoPtr<BroadcastStats> > mBroadcastStats;
};


} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CALARMMANAGERSERVICE_H__
