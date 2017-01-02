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

#include "elastos/droid/server/AlarmManagerService.h"
#include "elastos/droid/server/CAlarmManagerBinderService.h"
#include <elastos/droid/Manifest.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/droid/utility/TimeUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Text.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>

// for navtive
//
#include <utils/Log.h>
#include <utils/misc.h>

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <linux/ioctl.h>
#include <linux/android_alarm.h>
#include <linux/rtc.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::EIID_IIAlarmManager;
using Elastos::Droid::App::EIID_IPendingIntentOnFinished;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Format::IDateFormat;
using Elastos::Droid::Text::Format::CDateFormat;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::CSparseBooleanArray;
using Elastos::Droid::Internal::Utility::CLocalLog;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::ICloneable;
using Elastos::Core::IAppendable;
using Elastos::IO::ICloseable;
using Elastos::IO::IWriter;
using Elastos::IO::IOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CPrintWriter;
using Elastos::IO::IFlushable;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::IDate;
using Elastos::Utility::CDate;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::Logging::Slogger;


namespace android {

static const size_t N_ANDROID_TIMERFDS = ANDROID_ALARM_TYPE_COUNT + 1;
static const clockid_t android_alarm_to_clockid[N_ANDROID_TIMERFDS] = {
    CLOCK_REALTIME_ALARM,
    CLOCK_REALTIME,
    CLOCK_BOOTTIME_ALARM,
    CLOCK_BOOTTIME,
    CLOCK_MONOTONIC,
    CLOCK_REALTIME,
};
/* to match the legacy alarm driver implementation, we need an extra
   CLOCK_REALTIME fd which exists specifically to be canceled on RTC changes */

class AlarmImpl
{
public:
    AlarmImpl(int *fds, size_t n_fds);
    virtual ~AlarmImpl();

    virtual int set(int type, struct timespec *ts) = 0;
    virtual int setTime(struct timeval *tv) = 0;
    virtual int waitForAlarm() = 0;

protected:
    int *fds;
    size_t n_fds;
};

class AlarmImplAlarmDriver : public AlarmImpl
{
public:
    AlarmImplAlarmDriver(int fd) : AlarmImpl(&fd, 1) { }

    int set(int type, struct timespec *ts);
    int clear(int type, struct timespec *ts);
    int setTime(struct timeval *tv);
    int waitForAlarm();
};

class AlarmImplTimerFd : public AlarmImpl
{
public:
    AlarmImplTimerFd(int fds[N_ANDROID_TIMERFDS], int epollfd) :
        AlarmImpl(fds, N_ANDROID_TIMERFDS), epollfd(epollfd) { }
    ~AlarmImplTimerFd();

    int set(int type, struct timespec *ts);
    int setTime(struct timeval *tv);
    int waitForAlarm();

private:
    int epollfd;
};

AlarmImpl::AlarmImpl(int *fds_, size_t n_fds) : fds(new int[n_fds]),
        n_fds(n_fds)
{
    memcpy(fds, fds_, n_fds * sizeof(fds[0]));
}

AlarmImpl::~AlarmImpl()
{
    for (size_t i = 0; i < n_fds; i++) {
        close(fds[i]);
    }
    delete [] fds;
}

int AlarmImplAlarmDriver::set(int type, struct timespec *ts)
{
    return ioctl(fds[0], ANDROID_ALARM_SET(type), ts);
}

int AlarmImplAlarmDriver::clear(int type, struct timespec *ts)
{
    return ioctl(fds[0], ANDROID_ALARM_CLEAR(type), ts);
}

int AlarmImplAlarmDriver::setTime(struct timeval *tv)
{
    struct timespec ts;
    int res;

    ts.tv_sec = tv->tv_sec;
    ts.tv_nsec = tv->tv_usec * 1000;
    res = ioctl(fds[0], ANDROID_ALARM_SET_RTC, &ts);
    if (res < 0)
        ALOGV("ANDROID_ALARM_SET_RTC ioctl failed: %s\n", strerror(errno));
    return res;
}

int AlarmImplAlarmDriver::waitForAlarm()
{
    return ioctl(fds[0], ANDROID_ALARM_WAIT);
}

AlarmImplTimerFd::~AlarmImplTimerFd()
{
    for (size_t i = 0; i < N_ANDROID_TIMERFDS; i++) {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, fds[i], NULL);
    }
    close(epollfd);
}

int AlarmImplTimerFd::set(int type, struct timespec *ts)
{
    if (type > ANDROID_ALARM_TYPE_COUNT) {
        errno = EINVAL;
        return -1;
    }

    if (!ts->tv_nsec && !ts->tv_sec) {
        ts->tv_nsec = 1;
    }
    /* timerfd interprets 0 = disarm, so replace with a practically
       equivalent deadline of 1 ns */

    struct itimerspec spec;
    memset(&spec, 0, sizeof(spec));
    memcpy(&spec.it_value, ts, sizeof(spec.it_value));

    return timerfd_settime(fds[type], TFD_TIMER_ABSTIME, &spec, NULL);
}

int AlarmImplTimerFd::setTime(struct timeval *tv)
{
    struct rtc_time rtc;
    struct tm tm, *gmtime_res;
    int fd;
    int res;

    res = settimeofday(tv, NULL);
    if (res < 0) {
        ALOGV("settimeofday() failed: %s\n", strerror(errno));
        return -1;
    }

    fd = open("/dev/rtc0", O_RDWR);
    if (fd < 0) {
        ALOGV("Unable to open RTC driver: %s\n", strerror(errno));
        return res;
    }

    gmtime_res = gmtime_r(&tv->tv_sec, &tm);
    if (!gmtime_res) {
        ALOGV("gmtime_r() failed: %s\n", strerror(errno));
        res = -1;
        goto done;
    }

    memset(&rtc, 0, sizeof(rtc));
    rtc.tm_sec = tm.tm_sec;
    rtc.tm_min = tm.tm_min;
    rtc.tm_hour = tm.tm_hour;
    rtc.tm_mday = tm.tm_mday;
    rtc.tm_mon = tm.tm_mon;
    rtc.tm_year = tm.tm_year;
    rtc.tm_wday = tm.tm_wday;
    rtc.tm_yday = tm.tm_yday;
    rtc.tm_isdst = tm.tm_isdst;
    res = ioctl(fd, RTC_SET_TIME, &rtc);
    if (res < 0)
        ALOGV("RTC_SET_TIME ioctl failed: %s\n", strerror(errno));
done:
    close(fd);
    return res;
}

int AlarmImplTimerFd::waitForAlarm()
{
    epoll_event events[N_ANDROID_TIMERFDS];

    int nevents = epoll_wait(epollfd, events, N_ANDROID_TIMERFDS, -1);
    if (nevents < 0) {
        return nevents;
    }

    int result = 0;
    for (int i = 0; i < nevents; i++) {
        uint32_t alarm_idx = events[i].data.u32;
        uint64_t unused;
        ssize_t err = read(fds[alarm_idx], &unused, sizeof(unused));
        if (err < 0) {
            if (alarm_idx == ANDROID_ALARM_TYPE_COUNT && errno == ECANCELED) {
                result |= ANDROID_ALARM_TIME_CHANGE_MASK;
            } else {
                return err;
            }
        } else {
            result |= (1 << alarm_idx);
        }
    }

    return result;
}

} //namespace android


namespace Elastos {
namespace Droid {
namespace Server {


// The threshold for how Int64 an alarm can be late before we print a
// warning message.  The time duration is in milliseconds.
const Int64 AlarmManagerService::LATE_ALARM_THRESHOLD = 10 * 1000;

const Int64 AlarmManagerService::POWER_OFF_ALARM_THRESHOLD = 120 * 1000;

const Int32 AlarmManagerService::RTC_WAKEUP_MASK = 1 << IAlarmManager::RTC_WAKEUP;
const Int32 AlarmManagerService::RTC_MASK = 1 << IAlarmManager::RTC;
const Int32 AlarmManagerService::ELAPSED_REALTIME_WAKEUP_MASK = 1 << IAlarmManager::ELAPSED_REALTIME_WAKEUP;
const Int32 AlarmManagerService::ELAPSED_REALTIME_MASK = 1 << IAlarmManager::ELAPSED_REALTIME;
const Int32 AlarmManagerService::RTC_POWEROFF_WAKEUP_MASK = 1 << IAlarmManager::RTC_POWEROFF_WAKEUP;
const Int32 AlarmManagerService::TIME_CHANGED_MASK = 1 << 16;
const Int32 AlarmManagerService::IS_WAKEUP_MASK = AlarmManagerService::RTC_WAKEUP_MASK
                                                    | AlarmManagerService::ELAPSED_REALTIME_WAKEUP_MASK
                                                    | AlarmManagerService::RTC_POWEROFF_WAKEUP_MASK;

// Mask for testing whether a given alarm type is wakeup vs non-wakeup
const Int32 AlarmManagerService::TYPE_NONWAKEUP_MASK = 0x1; // low bit => non-wakeup

const String AlarmManagerService::TAG("AlarmManagerService");
const String AlarmManagerService::ClockReceiver_TAG("ClockReceiver");
const Boolean AlarmManagerService::localLOGV = FALSE;
const Boolean AlarmManagerService::DEBUG_BATCH = AlarmManagerService::localLOGV;
const Boolean AlarmManagerService::DEBUG_VALIDATE = AlarmManagerService::localLOGV;
const Boolean AlarmManagerService::DEBUG_ALARM_CLOCK = AlarmManagerService::localLOGV;
const Int32 AlarmManagerService::ALARM_EVENT = 1;
const String AlarmManagerService::TIMEZONE_PROPERTY("persist.sys.timezone");

static AutoPtr<IIntent> InitBackgroundIntent()
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_FROM_BACKGROUND);
    return intent;
}
const AutoPtr<IIntent> AlarmManagerService::mBackgroundIntent = InitBackgroundIntent();
const AutoPtr<AlarmManagerService::IncreasingTimeOrder> AlarmManagerService::sIncreasingTimeOrder = new AlarmManagerService::IncreasingTimeOrder();

const Boolean AlarmManagerService::WAKEUP_STATS = FALSE;

static AutoPtr<IIntent> InitNEXT_ALARM_CLOCK_CHANGED_INTENT()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAlarmManager::ACTION_NEXT_ALARM_CLOCK_CHANGED, (IIntent**)&intent);
    return intent;
}
const AutoPtr<IIntent> AlarmManagerService::NEXT_ALARM_CLOCK_CHANGED_INTENT = InitNEXT_ALARM_CLOCK_CHANGED_INTENT();

// Alarm delivery ordering bookkeeping
const Int32 AlarmManagerService::PRIO_TICK = 0;
const Int32 AlarmManagerService::PRIO_WAKEUP = 1;
const Int32 AlarmManagerService::PRIO_NORMAL = 2;

// minimum recurrence period or alarm futurity for us to be able to fuzz it
const Int64 AlarmManagerService::MIN_FUZZABLE_INTERVAL = 10000;
const AutoPtr<AlarmManagerService::BatchTimeOrder> AlarmManagerService::sBatchOrder = new AlarmManagerService::BatchTimeOrder();

//=======================================================================================
// AlarmManagerService::BinderService
//=======================================================================================

CAR_INTERFACE_IMPL_2(AlarmManagerService::BinderService, Object, IIAlarmManager, IBinder)

ECode AlarmManagerService::BinderService::constructor(
    /* [in] */ ISystemService* alarmManagerService)
{
    mHost = (AlarmManagerService*)alarmManagerService;
    return NOERROR;
}

ECode AlarmManagerService::BinderService::Set(
    /* [in] */ Int32 type,
    /* [in] */ Int64 triggerAtTime,
    /* [in] */ Int64 windowLength,
    /* [in] */ Int64 interval,
    /* [in] */ IPendingIntent* operation,
    /* [in] */ IWorkSource* workSource,
    /* [in] */ IAlarmClockInfo* alarmClock)
{
    if (workSource != NULL) {
        AutoPtr<IContext> context;
        mHost->GetContext((IContext**)&context);
        FAIL_RETURN(context->EnforceCallingPermission(
            Manifest::permission::UPDATE_DEVICE_STATS,
            String("AlarmManager.set")))
    }

    return mHost->SetImpl(type, triggerAtTime, windowLength, interval, operation,
            windowLength == IAlarmManager::WINDOW_EXACT, workSource, alarmClock);
}

ECode AlarmManagerService::BinderService::SetTime(
    /* [in] */ Int64 millis,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    FAIL_RETURN(context->EnforceCallingOrSelfPermission(
        String("android.permission.SET_TIME"),
        String("SetTime")))

    if (mHost->mNativeData == 0) {
        Slogger::W(TAG, "Not setting time since no alarm driver is available.");
        return NOERROR;
    }

    {
        AutoLock syncLock(mHost->mLock);
        *result = mHost->Native_SetKernelTime(mHost->mNativeData, millis) == 0;
    }
    return NOERROR;
}

ECode AlarmManagerService::BinderService::SetTimeZone(
    /* [in] */ const String& tz)
{
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    FAIL_RETURN(context->EnforceCallingOrSelfPermission(
        String("android.permission.SET_TIME_ZONE"),
        String("SetTimeZone")))

    Int64 oldId = Binder::ClearCallingIdentity();
    mHost->SetTimeZoneImpl(tz);
    Binder::RestoreCallingIdentity(oldId);
    return NOERROR;
}

ECode AlarmManagerService::BinderService::Remove(
    /* [in] */ IPendingIntent* operation)
{
    mHost->RemoveImpl(operation);
    return NOERROR;
}

ECode AlarmManagerService::BinderService::GetNextAlarmClock(
    /* [in] */ Int32 userId,
    /* [out] */IAlarmClockInfo** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);

    Int32 id;
    helper->HandleIncomingUser(Binder::GetCallingPid(),
        Binder::GetCallingUid(), userId, FALSE /* allowAll */, FALSE /* requireFull */,
        String("getNextAlarmClock"), String(NULL), &id);

    AutoPtr<IAlarmClockInfo> info = mHost->GetNextAlarmClockImpl(id);
    *result = info;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode AlarmManagerService::BinderService::UpdateBlockedUids(
    /* [in] */ Int32 uid,
    /* [in] */ Boolean isBlocked)
{
    if (AlarmManagerService::localLOGV) {
        Slogger::V(TAG, "UpdateBlockedUids: uid = %d isBlocked = %d", uid, isBlocked);
    }

    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        if (AlarmManagerService::localLOGV) {
            Slogger::V(TAG, "UpdateBlockedUids is not allowed");
        }
        return NOERROR;
    }

    {    AutoLock syncLock(mHost->mLock);
        if(isBlocked) {
            mHost->mBlockedUids->Add(CoreUtils::Convert(uid));
            if (mHost->CheckReleaseWakeLock()) {
                /* all the uids for which the alarms are triggered
                 * are either blocked or have called onSendFinished.
                 */
                Boolean bIsHeld = FALSE;
                mHost->mWakeLock->IsHeld(&bIsHeld);
                if (bIsHeld) {
                    mHost->mWakeLock->ReleaseLock();
                    if (AlarmManagerService::localLOGV) {
                        Slogger::V(TAG, "AM WakeLock Released Internally in updateBlockedUids");
                    }
                }
            }
        }
        else {
            mHost->mBlockedUids->Clear();
        }
    }
    return NOERROR;
}

ECode AlarmManagerService::BinderService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    Int32 perm = 0;
    context->CheckCallingOrSelfPermission(Manifest::permission::DUMP, &perm);
    if (perm != IPackageManager::PERMISSION_GRANTED) {
        pw->Print(String("Permission Denial: can't dump AlarmManager from from pid="));
        pw->Print(Binder::GetCallingPid());
        pw->Print(String(", uid="));
        pw->Println(Binder::GetCallingUid());
        return NOERROR;
    }

    mHost->DumpImpl(pw);
    return NOERROR;
}

ECode AlarmManagerService::BinderService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "AlarmManagerService::BinderService";
    return NOERROR;
}

//=======================================================================================
// AlarmManagerService::PriorityClass
//=======================================================================================
AlarmManagerService::PriorityClass::PriorityClass(
    /* [in] */ AlarmManagerService* host)
    : mHost(host)
{
    mSeq = mHost->mCurrentSeq - 1;
    mPriority = PRIO_NORMAL;
}

//=======================================================================================
// AlarmManagerService::WakeupEvent
//=======================================================================================
AlarmManagerService::WakeupEvent::WakeupEvent(
    /* [in] */ Int64 theTime,
    /* [in] */ Int32 theUid,
    /* [in] */ const String& theAction)
    : mWhen(theTime)
    , mUid(theUid)
    , mAction(theAction)
{
}

//=======================================================================================
// AlarmManagerService::Batch
//=======================================================================================
AlarmManagerService::Batch::Batch(
    /* [in] */ AlarmManagerService* host)
    : mHost(host)
{
    mStart = 0;
    mEnd = Elastos::Core::Math::INT64_MAX_VALUE;
    mStandalone = FALSE;
    CArrayList::New((IArrayList**)&mAlarms);
}

AlarmManagerService::Batch::Batch(
    /* [in] */ Alarm* seed,
    /* [in] */ AlarmManagerService* host)
    : mHost(host)
{
    mStart = seed->mWhenElapsed;
    mEnd = seed->mMaxWhen;
    mStandalone = FALSE;
    CArrayList::New((IArrayList**)&mAlarms);
    mAlarms->Add(TO_IINTERFACE(seed));
}

Int32 AlarmManagerService::Batch::Size()
{
    Int32 size;
    mAlarms->GetSize(&size);
    return size;
}

AutoPtr<AlarmManagerService::Alarm> AlarmManagerService::Batch::Get(
    /* [in] */ Int32 index)
{
    AutoPtr<IInterface> obj;
    mAlarms->Get(index, (IInterface**)&obj);
    if (obj) {
        return (Alarm*)IObject::Probe(obj);
    }
    return NULL;
}

Int64 AlarmManagerService::Batch::GetWhenByElapsedTime(
    /* [in] */ Int64 whenElapsed)
{
    Int32 size = 0;
    mAlarms->GetSize(&size);
    for(Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mAlarms->Get(i, (IInterface**)&obj);
        Alarm* a = (Alarm*)IObject::Probe(obj);
        if(a->mWhenElapsed == whenElapsed)
            return a->mWhen;
    }
    return 0;
}

Boolean AlarmManagerService::Batch::CanHold(
    /* [in] */ Int64 whenElapsed,
    /* [in] */ Int64 maxWhen)
{
    return (mEnd >= whenElapsed) && (mStart <= maxWhen);
}

Boolean AlarmManagerService::Batch::Add(
    /* [in] */ Alarm* alarm)
{
    IInterface* alarmObj = TO_IINTERFACE(alarm);
    Boolean newStart = FALSE;
    // narrows the batch if necessary; presumes that CanHold(alarm) is TRUE
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    Int32 index;
    collections->BinarySearch(IList::Probe(mAlarms), alarmObj, (IComparator*)sIncreasingTimeOrder.Get(), &index);
    if (index < 0) {
        index = 0 - index - 1;
    }
    mAlarms->Add(index, alarmObj);
    if (DEBUG_BATCH) {
        Slogger::V(TAG, "Adding %s to %s", TO_CSTR(alarm), TO_CSTR(this));
    }
    if (alarm->mWhenElapsed > mStart) {
        mStart = alarm->mWhenElapsed;
        newStart = TRUE;
    }
    if (alarm->mMaxWhen < mEnd) {
        mEnd = alarm->mMaxWhen;
    }

    if (DEBUG_BATCH) {
        Slogger::V(TAG, "    => now %s", TO_CSTR(this));
    }
    return newStart;
}

Boolean AlarmManagerService::Batch::Remove(
    /* [in] */ IPendingIntent* operation)
{
    Boolean didRemove = FALSE;
    Int64 newStart = 0;  // recalculate endpoints as we go
    Int64 newEnd = Elastos::Core::Math::INT64_MAX_VALUE;
    Int32 N = 0;
    for (Int32 i = 0; i < (mAlarms->GetSize(&N), N);) {
        AutoPtr<IInterface> obj;
        mAlarms->Get(i, (IInterface**)&obj);
        Alarm* alarm = (Alarm*)IObject::Probe(obj);
        if (Object::Equals(alarm->mOperation, operation)) {
            mAlarms->Remove(i);
            N--;
            didRemove = TRUE;
            if (alarm->mAlarmClock != NULL) {
                mHost->mNextAlarmClockMayChange = TRUE;
            }
        }
        else {
            if (alarm->mWhenElapsed > newStart) {
                newStart = alarm->mWhenElapsed;
            }
            if (alarm->mMaxWhen < newEnd) {
                newEnd = alarm->mMaxWhen;
            }
            i++;
        }
    }
    if (didRemove) {
        // commit the new batch bounds
        mStart = newStart;
        mEnd = newEnd;
    }
    return didRemove;
}

Boolean AlarmManagerService::Batch::Remove(
    /* [in] */ const String& packageName)
{
    Boolean didRemove = FALSE;
    Int64 newStart = 0;  // recalculate endpoints as we go
    Int64 newEnd = Elastos::Core::Math::INT64_MAX_VALUE;
    Int32 N;
    mAlarms->GetSize(&N);
    String pkg;
    for (Int32 i = 0; i < N;) {
        AutoPtr<IInterface> obj;
        mAlarms->Get(i, (IInterface**)&obj);
        Alarm* alarm = (Alarm*)IObject::Probe(obj);
        alarm->mOperation->GetTargetPackage(&pkg);
        if (pkg.Equals(packageName)) {
            mAlarms->Remove(i);
            N--;
            didRemove = TRUE;
            if (alarm->mAlarmClock != NULL) {
                mHost->mNextAlarmClockMayChange = TRUE;
            }
        }
        else {
            if (alarm->mWhenElapsed > newStart) {
                newStart = alarm->mWhenElapsed;
            }
            if (alarm->mMaxWhen < newEnd) {
                newEnd = alarm->mMaxWhen;
            }
            i++;
        }
    }
    if (didRemove) {
        // commit the new batch bounds
        mStart = newStart;
        mEnd = newEnd;
    }
    return didRemove;
}

Boolean AlarmManagerService::Batch::Remove(
    /* [in] */ Int32 userHandle)
{
    Boolean didRemove = FALSE;
    Int64 newStart = 0;  // recalculate endpoints as we go
    Int64 newEnd = Elastos::Core::Math::INT64_MAX_VALUE;
    Int32 N;
    mAlarms->GetSize(&N);
    for (Int32 i = 0; i < N;) {
        AutoPtr<IInterface> obj;
        mAlarms->Get(i, (IInterface**)&obj);
        Alarm* alarm = (Alarm*)IObject::Probe(obj);
        Int32 id;
        alarm->mOperation->GetCreatorUid(&id);
        if (UserHandle::GetUserId(id) == userHandle) {
            mAlarms->Remove(i);
            N--;
            didRemove = TRUE;
            if (alarm->mAlarmClock != NULL) {
                mHost->mNextAlarmClockMayChange = TRUE;
            }
        }
        else {
            if (alarm->mWhenElapsed > newStart) {
                newStart = alarm->mWhenElapsed;
            }
            if (alarm->mMaxWhen < newEnd) {
                newEnd = alarm->mMaxWhen;
            }
            i++;
        }
    }
    if (didRemove) {
        // commit the new batch bounds
        mStart = newStart;
        mEnd = newEnd;
    }
    return didRemove;
}

Boolean AlarmManagerService::Batch::HasPackage(
    /* [in] */ const String& packageName)
{
    Int32 N;
    mAlarms->GetSize(&N);
    String pkg;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mAlarms->Get(i, (IInterface**)&obj);
        Alarm* a = (Alarm*)IObject::Probe(obj);
        a->mOperation->GetTargetPackage(&pkg);
        if (pkg.Equals(packageName)) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean AlarmManagerService::Batch::HasWakeups()
{
    Int32 N = 0;
    mAlarms->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mAlarms->Get(i, (IInterface**)&obj);
        Alarm* a = (Alarm*)IObject::Probe(obj);
        // non-wakeup mAlarms are types 1 and 3, i.e. have the low bit set
        if ((a->mType & TYPE_NONWAKEUP_MASK) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean AlarmManagerService::Batch::IsRtcPowerOffWakeup()
{
    Int32 N = 0;
    mAlarms->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mAlarms->Get(i, (IInterface**)&obj);
        Alarm* a = (Alarm*)IObject::Probe(obj);
        // non-wakeup alarms are types 1 and 3, i.e. have the low bit set
        if (a->mType == IAlarmManager::RTC_POWEROFF_WAKEUP) {
            return TRUE;
        }
    }
    return FALSE;
}

ECode AlarmManagerService::Batch::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder b(40);
    b.Append("Batch{");
    b.Append(StringUtils::ToHexString((Int32)this));
    b.Append(" num=");
    b.Append(Size());
    b.Append(" start=");
    b.Append(mStart);
    b.Append(" end=");
    b.Append(mEnd);
    if (mStandalone) {
        b.Append(" STANDALONE");
    }
    b.AppendChar('}');
    *result = b.ToString();
    return NOERROR;
}

//=======================================================================================
// AlarmManagerService::BatchTimeOrder
//=======================================================================================

CAR_INTERFACE_IMPL(AlarmManagerService::BatchTimeOrder, Object, IComparator)

ECode AlarmManagerService::BatchTimeOrder::Compare(
    /* [in] */ IInterface* o1, //Batch b1,
    /* [in] */ IInterface* o2, //Batch b2
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    Batch* b1 = (Batch*)IObject::Probe(o1);
    Batch* b2 = (Batch*)IObject::Probe(o2);

    Int64 when1 = b1->mStart;
    Int64 when2 = b2->mStart;
    if (when1 - when2 > 0) {
        *result = 1;
        return NOERROR;
    }
    if (when1 - when2 < 0) {
        *result = -1;
        return NOERROR;
    }
    return NOERROR;
}

//=======================================================================================
// AlarmManagerService::AlarmDispatchComparator
//=======================================================================================

CAR_INTERFACE_IMPL(AlarmManagerService::AlarmDispatchComparator, Object, IComparator)

ECode AlarmManagerService::AlarmDispatchComparator::Compare(
    /* [in] */ IInterface* o1, //Alarm b1,
    /* [in] */ IInterface* o2, //Alarm b2
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    Alarm* lhs = (Alarm*)IObject::Probe(o1);
    Alarm* rhs = (Alarm*)IObject::Probe(o2);

    // priority class trumps everything.  TICK < WAKEUP < NORMAL
    if (lhs->mPriorityClass->mPriority < rhs->mPriorityClass->mPriority) {
        *result = -1;
        return NOERROR;
    }
    else if (lhs->mPriorityClass->mPriority > rhs->mPriorityClass->mPriority) {
        *result = 1;
        return NOERROR;
    }

    // within each class, sort by nominal delivery time
    if (lhs->mWhenElapsed < rhs->mWhenElapsed) {
        *result = -1;
        return NOERROR;
    }
    else if (lhs->mWhenElapsed > rhs->mWhenElapsed) {
        *result = 1;
        return NOERROR;
    }

    // same priority class + same target delivery time
    return NOERROR;
}

//=======================================================================================
// AlarmManagerService::AlarmThread
//=======================================================================================

AlarmManagerService::AlarmThread::AlarmThread(
    /* [in] */ AlarmManagerService* host)
    : mHost(host)
{
}

ECode AlarmManagerService::AlarmThread::Run()
{
    AutoPtr<IArrayList> triggerList;
    CArrayList::New((IArrayList**)&triggerList);

    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);

    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);

    while (TRUE) {
        Int32 result = mHost->Native_WaitForAlarm(mHost->mNativeData);

        triggerList->Clear();

        if ((result & TIME_CHANGED_MASK) != 0) {
            if (DEBUG_BATCH) {
                Slogger::V(AlarmManagerService::TAG, "Time changed notification from kernel; rebatching");
            }
            mHost->RemoveImpl(mHost->mTimeTickSender);
            mHost->RebatchAllAlarms();
            mHost->mClockReceiver->ScheduleTimeTickEvent();
            {    AutoLock syncLock(mHost->mLock);
                mHost->mNumTimeChanged++;
            }

            AutoPtr<IIntent> intent;
            CIntent::New(IIntent::ACTION_TIME_CHANGED, (IIntent**)&intent);
            intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING
                    | IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT
                    | IIntent::FLAG_RECEIVER_FOREGROUND);
            context->SendBroadcastAsUser(intent, UserHandle::ALL);
        }

        {    AutoLock syncLock(mHost->mLock);
            Int64 nowRTC;
            system->GetCurrentTimeMillis(&nowRTC);
            Int64 nowELAPSED = SystemClock::GetElapsedRealtime();
            if (AlarmManagerService::localLOGV)
                Slogger::V(AlarmManagerService::TAG, "Checking for mAlarms... rtc=%lld, elapsed=%lld",
                    nowRTC, nowELAPSED);

            if (AlarmManagerService::WAKEUP_STATS) {
                if ((result & AlarmManagerService::IS_WAKEUP_MASK) != 0) {
                    Int64 newEarliest = nowRTC - mHost->RECENT_WAKEUP_PERIOD;
                    Int32 n = 0;
                    AutoPtr<IIterator> it;
                    mHost->mRecentWakeups->GetIterator((IIterator**)&it);
                    Boolean hasNext;
                    while (it->HasNext(&hasNext), hasNext) {
                        AutoPtr<IInterface> obj;
                        it->GetNext((IInterface**)&obj);
                        WakeupEvent* event = (WakeupEvent*)IObject::Probe(obj);
                        if (event->mWhen > newEarliest) break;
                        n++; // number of now-stale entries at the list head
                    }
                    for (Int32 i = 0; i < n; i++) {
                        AutoPtr<IInterface> tmp;
                        mHost->mRecentWakeups->Remove((IInterface**)&tmp);
                    }

                    mHost->RecordWakeupAlarms(mHost->mAlarmBatches, nowELAPSED, nowRTC);
                }
            }

            Int32 size = 0;
            Boolean hasWakeup = mHost->TriggerAlarmsLocked(triggerList, nowELAPSED, nowRTC);

            AutoPtr<ISystemProperties> prop;
            CSystemProperties::AcquireSingleton((ISystemProperties**)&prop);
            Int32 in = 0;
            prop->GetInt32(String("sys.quickboot.enable"), 0, &in);
            if (in == 1) {
                mHost->FiltQuickBootAlarms(triggerList);
            }

            if (!hasWakeup && mHost->CheckAllowNonWakeupDelayLocked(nowELAPSED)) {
                // if there are no wakeup mAlarms and the screen is off, we can
                // delay what we have so far until the future.
                if (mHost->mPendingNonWakeupAlarms->GetSize(&size) == 0) {
                    mHost->mStartCurrentDelayTime = nowELAPSED;
                    mHost->mNextNonWakeupDeliveryTime = nowELAPSED
                            + ((mHost->CurrentNonWakeupFuzzLocked(nowELAPSED)*3)/2);
                }
                mHost->mPendingNonWakeupAlarms->AddAll(ICollection::Probe(triggerList));
                triggerList->GetSize(&size);
                mHost->mNumDelayedAlarms += size;
                mHost->RescheduleKernelAlarmsLocked();
                mHost->UpdateNextAlarmClockLocked();
            }
            else {
                // now deliver the alarm intents; if there are pending non-wakeup
                // mAlarms, we need to merge them in to the list.  note we don't
                // just deliver them first because we generally want non-wakeup
                // mAlarms delivered after wakeup mAlarms.
                mHost->RescheduleKernelAlarmsLocked();
                mHost->UpdateNextAlarmClockLocked();
                if (mHost->mPendingNonWakeupAlarms->GetSize(&size) > 0) {
                    mHost->CalculateDeliveryPriorities(mHost->mPendingNonWakeupAlarms);
                    triggerList->AddAll(ICollection::Probe(mHost->mPendingNonWakeupAlarms));
                    collections->Sort(IList::Probe(triggerList), (IComparator*)mHost->mAlarmDispatchComparator.Get());
                    Int64 thisDelayTime = nowELAPSED - mHost->mStartCurrentDelayTime;
                    mHost->mTotalDelayTime += thisDelayTime;
                    if (mHost->mMaxDelayTime < thisDelayTime) {
                        mHost->mMaxDelayTime = thisDelayTime;
                    }
                    mHost->mPendingNonWakeupAlarms->Clear();
                }
                mHost->DeliverAlarmsLocked(triggerList, nowELAPSED);
            }
        }
    }

    return NOERROR;
}

//=======================================================================================
// AlarmManagerService::AlarmHandler
//=======================================================================================

const Int32 AlarmManagerService::AlarmHandler::ALARM_EVENT = 1;
const Int32 AlarmManagerService::AlarmHandler::MINUTE_CHANGE_EVENT = 2;
const Int32 AlarmManagerService::AlarmHandler::DATE_CHANGE_EVENT = 3;
const Int32 AlarmManagerService::AlarmHandler::SEND_NEXT_ALARM_CLOCK_CHANGED = 4;

AlarmManagerService::AlarmHandler::AlarmHandler(
    /* [in] */ AlarmManagerService* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode AlarmManagerService::AlarmHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);

    if (what == ALARM_EVENT) {
        AutoPtr<IArrayList> triggerList;
        CArrayList::New((IArrayList**)&triggerList);
        {    AutoLock syncLock(mHost->mLock);
            Int64 nowRTC;
            system->GetCurrentTimeMillis(&nowRTC);
            Int64 nowELAPSED = SystemClock::GetElapsedRealtime();
            mHost->TriggerAlarmsLocked(triggerList, nowELAPSED, nowRTC);
            mHost->UpdateNextAlarmClockLocked();
        }

        // now trigger the mAlarms without the lock held
        AutoPtr<IIterator> it;
        triggerList->GetIterator((IIterator**)&it);
        Boolean hasNext;
        ECode ec = NOERROR;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            Alarm* alarm = (Alarm*)IObject::Probe(obj);
            ec = alarm->mOperation->Send();
            if (ec == (ECode)E_PENDING_INTENT_CANCELED_EXCEPTION) {
                if (alarm->mRepeatInterval > 0) {
                    // This IntentSender is no longer valid, but this
                    // is a repeating alarm, so toss the hoser.
                    mHost->RemoveImpl(alarm->mOperation);
                }
            }
        }
    }
    else if (what == SEND_NEXT_ALARM_CLOCK_CHANGED) {
        mHost->SendNextAlarmClockChanged();
    }
    return NOERROR;
}

//=======================================================================================
// AlarmManagerService::QuickBootReceiver
//=======================================================================================

const String AlarmManagerService::QuickBootReceiver::ACTION_APP_KILL("org.codeaurora.quickboot.appkilled");

AlarmManagerService::QuickBootReceiver::QuickBootReceiver(
    /* [in] */ AlarmManagerService* host)
    : mHost(host)
{
}

ECode AlarmManagerService::QuickBootReceiver::constructor()
{
    BroadcastReceiver::constructor();

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ACTION_APP_KILL);
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<IIntent> intent;
    return context->RegisterReceiver(this, filter, String("android.permission.DEVICE_POWER"), NULL, (IIntent**)&intent);
}

ECode AlarmManagerService::QuickBootReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    AutoPtr<ArrayOf<String> > pkgList;
    if (ACTION_APP_KILL.Equals(action)) {
        intent->GetStringArrayExtra(IIntent::EXTRA_PACKAGES, (ArrayOf<String>**)&pkgList);
        if (pkgList != NULL && (pkgList->GetLength() > 0)) {
            for (Int32 j = 0; j < pkgList->GetLength(); j++) {
                String pkg = (*pkgList)[j];
                mHost->RemoveLocked(pkg);
                Int32 size = 0;
                mHost->mBroadcastStats->GetSize(&size);
                for (Int32 i = size - 1; i >= 0; i--) {
                    AutoPtr<IInterface> val;
                    mHost->mBroadcastStats->ValueAt(i, (IInterface**)&val);
                    AutoPtr<IArrayMap> uidStats = IArrayMap::Probe(val);
                    AutoPtr<IInterface> res;
                    uidStats->Remove(CoreUtils::Convert(pkg), (IInterface**)&res);
                    if (res != NULL) {
                        Int32 s = 0;
                        uidStats->GetSize(&s);
                        if (s <= 0) {
                            mHost->mBroadcastStats->RemoveAt(i);
                        }
                    }
                }
            }
        }
    }
    return NOERROR;
}

//=======================================================================================
// AlarmManagerService::ClockReceiver
//=======================================================================================
AlarmManagerService::ClockReceiver::ClockReceiver(
    /* [in] */ AlarmManagerService* host)
    : mHost(host)
{}

ECode AlarmManagerService::ClockReceiver::constructor()
{
    BroadcastReceiver::constructor();

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_TIME_TICK);
    filter->AddAction(IIntent::ACTION_DATE_CHANGED);
    filter->AddAction(IIntent::ACTION_TIME_CHANGED);
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<IIntent> intent;
    return context->RegisterReceiver(this, filter, (IIntent**)&intent);
}

ECode AlarmManagerService::ClockReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_TIME_TICK)) {
        if (AlarmManagerService::DEBUG_BATCH) {
            Slogger::V(AlarmManagerService::TAG, "Received TIME_TICK alarm; rescheduling");
        }
        ScheduleTimeTickEvent();
    }
    else if (action.Equals(IIntent::ACTION_TIME_CHANGED)) {
        if (DEBUG_BATCH) {
            Slogger::V(TAG, "Received TIME_CHANGED alarm; reschedule date changed event.");
        }
        ScheduleDateChangedEvent();
    }
    else if (action.Equals(IIntent::ACTION_DATE_CHANGED)) {
        // Since the kernel does not keep track of DST, we need to
        // reset the TZ information at the beginning of each day
        // based off of the current Zone gmt offset + userspace tracked
        // daylight savings information.
        AutoPtr<ITimeZoneHelper> helper;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);

        String str;
        sysProp->Get(AlarmManagerService::TIMEZONE_PROPERTY, &str);
        AutoPtr<ITimeZone> zone;
        helper->GetTimeZone(str, (ITimeZone**)&zone);
        Int64 ms = 0;
        system->GetCurrentTimeMillis(&ms);
        Int32 gmtOffset;
        zone->GetOffset(ms, &gmtOffset);
        mHost->Native_SetKernelTimezone(mHost->mNativeData, -(gmtOffset / 60000));
        ScheduleDateChangedEvent();
    }
    return NOERROR;
}

ECode AlarmManagerService::ClockReceiver::ScheduleTimeTickEvent()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 currentTime;
    system->GetCurrentTimeMillis(&currentTime);
    Int64 nextTime = 60000 * ((currentTime / 60000) + 1);

    // Schedule this event for the amount of time that it would take to get to
    // the top of the next minute.
    Int64 tickEventDelay = nextTime - currentTime;

    AutoPtr<IWorkSource> workSource; // Let system take blame for time tick events.
    return mHost->SetImpl(IAlarmManager::ELAPSED_REALTIME,
        SystemClock::GetElapsedRealtime() + tickEventDelay, 0,
        0, mHost->mTimeTickSender, TRUE, workSource, NULL);
}

ECode AlarmManagerService::ClockReceiver::ScheduleDateChangedEvent()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 currentTime;
    system->GetCurrentTimeMillis(&currentTime);

    AutoPtr<ICalendarHelper> calendarHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calendarHelper);
    AutoPtr<ICalendar> calendar;
    calendarHelper->GetInstance((ICalendar**)&calendar);
    calendar->SetTimeInMillis(currentTime);
    calendar->Set(ICalendar::HOUR_OF_DAY, 0);
    calendar->Set(ICalendar::MINUTE, 0);
    calendar->Set(ICalendar::SECOND, 0);
    calendar->Set(ICalendar::MILLISECOND, 0);
    calendar->Add(ICalendar::DAY_OF_MONTH, 1);

    Int64 ms = 0;
    calendar->GetTimeInMillis(&ms);
    AutoPtr<IWorkSource> workSource; // Let system take blame for date change events.
    return mHost->SetImpl(IAlarmManager::RTC, ms, 0, 0, mHost->mDateChangeSender, TRUE, workSource, NULL);
}

//=======================================================================================
// AlarmManagerService::InteractiveStateReceiver
//=======================================================================================
AlarmManagerService::InteractiveStateReceiver::InteractiveStateReceiver(
    /* [in] */ AlarmManagerService* host)
    : mHost(host)
{}

ECode AlarmManagerService::InteractiveStateReceiver::constructor()
{
    BroadcastReceiver::constructor();

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    filter->AddAction(IIntent::ACTION_SCREEN_ON);
    filter->SetPriority(IIntentFilter::SYSTEM_HIGH_PRIORITY);
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<IIntent> intent;
    return context->RegisterReceiver(this, filter, (IIntent**)&intent);
}

ECode AlarmManagerService::InteractiveStateReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    {    AutoLock syncLock(mHost->mLock);
        String action;
        intent->GetAction(&action);
        mHost->InteractiveStateChangedLocked(action.Equals(IIntent::ACTION_SCREEN_ON));
    }
    return NOERROR;
}

//=======================================================================================
// AlarmManagerService::UninstallReceiver
//=======================================================================================

AlarmManagerService::UninstallReceiver::UninstallReceiver(
    /* [in] */ AlarmManagerService* host)
    : mHost(host)
{
}

ECode AlarmManagerService::UninstallReceiver::constructor()
{
    BroadcastReceiver::constructor();

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    filter->AddAction(IIntent::ACTION_PACKAGE_RESTARTED);
    filter->AddAction(IIntent::ACTION_QUERY_PACKAGE_RESTART);
    filter->AddDataScheme(String("package"));
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(this, filter, (IIntent**)&intent);

     // Register for events related to sdcard installation.
    intent = NULL;
    AutoPtr<IIntentFilter> sdFilter;
    CIntentFilter::New((IIntentFilter**)&sdFilter);
    sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
    sdFilter->AddAction(IIntent::ACTION_USER_STOPPED);
    return context->RegisterReceiver(this, sdFilter, (IIntent**)&intent);
}

ECode AlarmManagerService::UninstallReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    {    AutoLock syncLock(mHost->mLock);
        String action;
        intent->GetAction(&action);
        AutoPtr<ArrayOf<String> > pkgList;
        if (IIntent::ACTION_QUERY_PACKAGE_RESTART.Equals(action)) {
            intent->GetStringArrayExtra(IIntent::EXTRA_PACKAGES, (ArrayOf<String>**)&pkgList);
            if (pkgList) {
                for (Int32 i = 0; i < pkgList->GetLength(); ++i) {
                    String packageName = (*pkgList)[i];
                    if (mHost->LookForPackageLocked(packageName)) {
                        SetResultCode(IActivity::RESULT_OK);
                        return NOERROR;
                    }
                }
            }
            return NOERROR;
        }
        else if (IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE.Equals(action)) {
            intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_PACKAGE_LIST, (ArrayOf<String>**)&pkgList);
        }
        else if (IIntent::ACTION_USER_STOPPED.Equals(action)) {
            Int32 userHandle;
            intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &userHandle);
            if (userHandle >= 0) {
                mHost->RemoveUserLocked(userHandle);
            }
        }
        else {
            Boolean bval;
            if (IIntent::ACTION_PACKAGE_REMOVED.Equals(action)
                    && (intent->GetBooleanExtra(IIntent::EXTRA_REPLACING, FALSE, &bval), bval)) {
                // This package is being updated; don't kill its mAlarms.
                return NOERROR;
            }
            AutoPtr<IUri> data;
            intent->GetData((IUri**)&data);
            if (data != NULL) {
                String pkg;
                data->GetSchemeSpecificPart(&pkg);
                if (pkg != NULL) {
                    pkgList = ArrayOf<String>::Alloc(1);
                    pkgList->Set(0, pkg);
                }
            }
        }

        if (pkgList != NULL && (pkgList->GetLength() > 0)) {
            Int32 size;
            HashMap<String, AutoPtr<PriorityClass> >::Iterator it;
            for (Int32 i = 0; i < pkgList->GetLength(); ++i) {
                String pkg = (*pkgList)[i];
                mHost->RemoveLocked(pkg);
                it = mHost->mPriorities.Find(pkg);
                if (it != mHost->mPriorities.End()) {
                    mHost->mPriorities.Erase(it);
                }
                mHost->mBroadcastStats->GetSize(&size);
                for (Int32 i = size -1; i >= 0; i--) {
                    AutoPtr<IInterface> obj;
                    mHost->mBroadcastStats->ValueAt(i, (IInterface**)&obj);
                    AutoPtr<IArrayMap> uidStats = IArrayMap::Probe(obj);
                    AutoPtr<IInterface> valueObj;
                    uidStats->Remove(CoreUtils::Convert(pkg).Get(), (IInterface**)&valueObj);
                    if (valueObj != NULL) {
                        Int32 uidSize;
                        uidStats->GetSize(&uidSize);
                        if (uidSize <= 0) {
                            mHost->mBroadcastStats->RemoveAt(i);
                        }
                    }
                }
            }
        }
    }
    return NOERROR;
}

//=======================================================================================
// AlarmManagerService::ResultReceiver
//=======================================================================================

CAR_INTERFACE_IMPL(AlarmManagerService::ResultReceiver, Object, IPendingIntentOnFinished)

AlarmManagerService::ResultReceiver::ResultReceiver(
    /* [in] */ AlarmManagerService* host)
    : mHost(host)
{}

ECode AlarmManagerService::ResultReceiver::OnSendFinished(
    /* [in] */ IPendingIntent* pi,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* resultExtras)
{
    {    AutoLock syncLock(mHost->mLock);
        Int32 uid = 0;
        Int32 size = 0;
        mHost->mInFlight->GetSize(&size);

        AutoPtr<InFlight> inflight;
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mHost->mInFlight->Get(i, (IInterface**)&obj);
            AutoPtr<InFlight> flight = (InFlight*)IObject::Probe(obj);
            if (flight->mPendingIntent.Get() == pi) {
                uid = flight->mUid;
                obj = NULL;
                mHost->mInFlight->Remove(i, (IInterface**)&obj);
                inflight = (InFlight*)IObject::Probe(obj);
                break;
            }
        }
        if (inflight != NULL) {
            Int64 nowELAPSED = SystemClock::GetElapsedRealtime();
            AutoPtr<BroadcastStats> bs = inflight->mBroadcastStats;
            bs->mNesting--;
            if (bs->mNesting <= 0) {
                bs->mNesting = 0;
                bs->mAggregateTime += nowELAPSED - bs->mStartTime;
            }
            AutoPtr<FilterStats> fs = inflight->mFilterStats;
            fs->mNesting--;
            if (fs->mNesting <= 0) {
                fs->mNesting = 0;
                fs->mAggregateTime += nowELAPSED - fs->mStartTime;
            }
        }
        else {
            Slogger::W("AlarmManagerService", "No in-flight alarm for %s %s",
                TO_CSTR(pi), TO_CSTR(intent));
        }

        mHost->mBroadcastRefCount--;
        mHost->mTriggeredUids->Remove(CoreUtils::Convert(uid));

        if (mHost->CheckReleaseWakeLock()) {
            Boolean bIsHeld = FALSE;
            mHost->mWakeLock->IsHeld(&bIsHeld);
            if (bIsHeld) {
                mHost->mWakeLock->ReleaseLock();
                if (localLOGV) {
                    Slogger::V(TAG, "AM WakeLock Released Internally onSendFinish");
                }
            }
        }

        if (mHost->mBroadcastRefCount == 0) {
            Boolean bIsHeld = FALSE;
            mHost->mWakeLock->IsHeld(&bIsHeld);
            if (bIsHeld) {
                mHost->mWakeLock->ReleaseLock();
            }
            mHost->mInFlight->GetSize(&size);
            if (size > 0) {
                Slogger::W("AlarmManagerService",  "Finished all broadcasts with %d remaining inflights", size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> obj;
                    mHost->mInFlight->Get(i, (IInterface**)&obj);
                    Slogger::W("AlarmManagerService", "  Remaining #%d: %s", i, TO_CSTR(obj));
                }
                mHost->mInFlight->Clear();
            }
        }
        else {
            // the next of our mAlarms is now in flight.  reattribute the wakelock.
            mHost->mInFlight->GetSize(&size);
            if (size > 0) {
                AutoPtr<IInterface> obj;
                mHost->mInFlight->Get(0, (IInterface**)&obj);
                InFlight* inFlight = (InFlight*)IObject::Probe(obj);
                mHost->SetWakelockWorkSource(inFlight->mPendingIntent, inFlight->mWorkSource,
                    inFlight->mAlarmType, inFlight->mTag, FALSE);
            }
            else {
                // should never happen
                Slogger::W("AlarmManagerService", "Alarm wakelock still held but sent queue empty");
                mHost->mWakeLock->SetWorkSource(NULL);
            }
        }
    }
    return NOERROR;
}

//=======================================================================================
// AlarmManagerService::IncreasingTimeOrder
//=======================================================================================

CAR_INTERFACE_IMPL(AlarmManagerService::IncreasingTimeOrder, Object, IComparator)

ECode AlarmManagerService::IncreasingTimeOrder::Compare(
    /* [in] */ IInterface* o1,
    /* [in] */ IInterface* o2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    Alarm* a1 = (Alarm*)IObject::Probe(o1);
    Alarm* a2 = (Alarm*)IObject::Probe(o2);

    Int64 when1 = a1->mWhen;
    Int64 when2 = a2->mWhen;
    if (when1 - when2 > 0) {
        *result = 1;
        return NOERROR;
    }
    if (when1 - when2 < 0) {
        *result = -1;
        return NOERROR;
    }
    return NOERROR;
}

//=======================================================================================
// AlarmManagerService::Alarm
//=======================================================================================
AlarmManagerService::Alarm::Alarm(
    /* [in] */ Int32 _type,
    /* [in] */ Int64 _when,
    /* [in] */ Int64 _whenElapsed,
    /* [in] */ Int64 _windowLength,
    /* [in] */ Int64 _maxWhen,
    /* [in] */ Int64 _interval,
    /* [in] */ IPendingIntent* _op,
    /* [in] */ IWorkSource* _ws,
    /* [in] */ IAlarmClockInfo* _info,
    /* [in] */ Int32 _userId)
    : mCount(0)
{
    mType = _type;
    mWakeup = _type == IAlarmManager::ELAPSED_REALTIME_WAKEUP
            || _type == IAlarmManager::RTC_WAKEUP
            || _type == IAlarmManager::RTC_POWEROFF_WAKEUP;
    mWhen = _when;
    mWhenElapsed = _whenElapsed;
    mWindowLength = _windowLength;
    mMaxWhen = _maxWhen;
    mRepeatInterval = _interval;
    mOperation = _op;
    mTag = MakeTag(_op, _type);
    mWorkSource = _ws;
    mAlarmClock = _info;
    mUserId = _userId;
    mUid = Binder::GetCallingUid();
    mPid = Binder::GetCallingPid();
}

String AlarmManagerService::Alarm::MakeTag(
    /* [in] */ IPendingIntent* pi,
    /* [in] */ Int32 type)
{
    String value;
    String typeStr(type == IAlarmManager::ELAPSED_REALTIME_WAKEUP
        || type == IAlarmManager::RTC_WAKEUP || type == IAlarmManager::RTC_POWEROFF_WAKEUP ? "*walarm*:" : "*alarm*:");
    pi->GetTag(typeStr, &value);
    return value;
}

//@Override
ECode AlarmManagerService::Alarm::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder sb(128);
    sb.Append("Alarm{");
    sb.Append(StringUtils::ToHexString((Int32)this));
    sb.Append(" type ");
    sb.Append(mType);
    sb.Append(" when ");
    sb.Append(mWhen);
    sb.Append(" ");
    String pkg;
    mOperation->GetTargetPackage(&pkg);
    sb.Append(pkg);
    sb.Append("}");
    *str = sb.ToString();
    return NOERROR;
}

ECode AlarmManagerService::Alarm::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ Int64 nowRTC,
    /* [in] */ Int64 nowELAPSED,
    /* [in] */ ISimpleDateFormat* sdf)
{
    Boolean isRtc = (mType == IAlarmManager::RTC
                    || mType == IAlarmManager::RTC_WAKEUP
                    || mType == IAlarmManager::RTC_POWEROFF_WAKEUP);
    pw->Print(prefix); pw->Print(String("tag=")); pw->Println(mTag);
    pw->Print(prefix); pw->Print(String("type=")); pw->Print(mType);
    pw->Print(String(" whenElapsed="));
    TimeUtils::FormatDuration(mWhenElapsed, nowELAPSED, pw);
    if (isRtc) {
        AutoPtr<IDate> date;
        CDate::New(mWhen, (IDate**)&date);
        String str;
        Elastos::Text::IDateFormat::Probe(sdf)->Format(date, &str);
        pw->Print(String(" when=")); pw->Print(str);
    }
    else {
        pw->Print(String(" when=")); TimeUtils::FormatDuration(mWhen, nowELAPSED, pw);
    }
    pw->Println();
    pw->Print(prefix); pw->Print(String("window=")); pw->Print(mWindowLength);
    pw->Print(String(" repeatInterval=")); pw->Print(mRepeatInterval);
    pw->Print(String(" count=")); pw->Println(mCount);
    pw->Print(prefix); pw->Print(String("operation="));
    pw->Println(Object::ToString(mOperation));
    return NOERROR;
}

//=======================================================================================
// AlarmManagerService::InFlight
//=======================================================================================
AlarmManagerService::InFlight::InFlight()
    : mAlarmType(0)
{}

ECode AlarmManagerService::InFlight::constructor(
    /* [in] */ AlarmManagerService* service,
    /* [in] */ IPendingIntent* pendingIntent,
    /* [in] */ IWorkSource* workSource,
    /* [in] */ Int32 alarmType,
    /* [in] */ const String& tag,
    /* [in] */ Int32 uid)
{
    Intent::constructor();

    mPendingIntent = pendingIntent;
    mWorkSource = workSource;
    mTag = tag;
    mBroadcastStats = service->GetStatsLocked(pendingIntent);
    AutoPtr<ICharSequence> seq = CoreUtils::Convert(mTag);
    AutoPtr<IInterface> obj;
    mBroadcastStats->mFilterStats->Get(seq.Get(), (IInterface**)&obj);
    AutoPtr<FilterStats> fs = (FilterStats*)IObject::Probe(obj);
    if (fs == NULL) {
        fs = new FilterStats(mBroadcastStats, mTag);
        mBroadcastStats->mFilterStats->Put(seq.Get(), TO_IINTERFACE(fs));
    }
    mFilterStats = fs;
    mAlarmType = alarmType;
    mUid = uid;
    return NOERROR;
}

//=======================================================================================
// AlarmManagerService::FilterStats
//=======================================================================================
AlarmManagerService::FilterStats::FilterStats(
    /* [in] */ AlarmManagerService::BroadcastStats* broadcastStats,
    /* [in] */ const String& tag)
    : mAggregateTime(0)
    , mCount(0)
    , mNumWakeup(0)
    , mStartTime(0)
    , mNesting(0)
{
    mBroadcastStats = broadcastStats;
    mTag = tag;
}

//=======================================================================================
// AlarmManagerService::BroadcastStats
//=======================================================================================

AlarmManagerService::BroadcastStats::BroadcastStats(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
    : mUid(uid)
    , mPackageName(packageName)
    , mAggregateTime(0)
    , mCount(0)
    , mNumWakeup(0)
    , mStartTime(0)
    , mNesting(0)
{
    CArrayMap::New((IArrayMap**)&mFilterStats);
}

//=======================================================================================
// AlarmManagerService::
//=======================================================================================

ECode AlarmManagerService::CalculateDeliveryPriorities(
    /* [in] */ IArrayList* mAlarms)
{
    Int32 alarmPrio;
    HashMap<String, AutoPtr<PriorityClass> >::Iterator mit;
    Int32 N;
    mAlarms->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mAlarms->Get(i, (IInterface**)&obj);
        Alarm* a = (Alarm*)IObject::Probe(obj);

        AutoPtr<IIntent> intent;
        a->mOperation->GetIntent((IIntent**)&intent);
        String action;
        intent->GetAction(&action);
        if (IIntent::ACTION_TIME_TICK.Equals(action)) {
            alarmPrio = PRIO_TICK;
        }
        else if (a->mWakeup) {
            alarmPrio = PRIO_WAKEUP;
        }
        else {
            alarmPrio = PRIO_NORMAL;
        }

        AutoPtr<PriorityClass> packagePrio = a->mPriorityClass;
        String pkg;
        a->mOperation->GetCreatorPackage(&pkg);
        if (packagePrio == NULL) {
            mit = mPriorities.Find(pkg);
            if (mit != mPriorities.End()) {
                packagePrio = mit->mSecond;
            }
        }
        if (packagePrio == NULL) {
            packagePrio = a->mPriorityClass = new PriorityClass(this); // lowest prio & stale sequence
            mPriorities[pkg] = packagePrio;
        }
        a->mPriorityClass = packagePrio;

        if (packagePrio->mSeq != mCurrentSeq) {
            // first alarm we've seen in the current delivery generation from this package
            packagePrio->mPriority = alarmPrio;
            packagePrio->mSeq = mCurrentSeq;
        }
        else {
            // Multiple mAlarms from this package being delivered in this generation;
            // bump the package's delivery class if it's warranted.
            // TICK < WAKEUP < NORMAL
            if (alarmPrio < packagePrio->mPriority) {
                packagePrio->mPriority = alarmPrio;
            }
        }
    }
    return NOERROR;
}

AlarmManagerService::AlarmManagerService()
    : mNativeData(0)
    , mNextWakeup(0)
    , mNextNonWakeup(0)
    , mBroadcastRefCount(0)
    , mLastWakeLockUnimportantForLogging(0)
    , mInteractive(TRUE)
    , mNonInteractiveStartTime(0)
    , mNonInteractiveTime(0)
    , mLastAlarmDeliveryTime(0)
    , mStartCurrentDelayTime(0)
    , mNextNonWakeupDeliveryTime(0)
    , mNumTimeChanged(0)
    , mNextAlarmClockMayChange(0)
    , mCurrentSeq(0)
    , RECENT_WAKEUP_PERIOD (1000L * 60 * 60 * 24) // one day
    , mNumDelayedAlarms(0)
    , mTotalDelayTime(0)
    , mMaxDelayTime(0)
{
}

AlarmManagerService::~AlarmManagerService()
{
    Finalize();
}

ECode AlarmManagerService::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(SystemService::constructor(context))

    CLocalLog::New(TAG, (ILocalLog**)&mLog);

    CArrayList::New((IArrayList**)&mPendingNonWakeupAlarms);
    CArrayList::New((IArrayList**)&mInFlight);

    CArrayList::New((IArrayList**)&mTriggeredUids);
    CArrayList::New((IArrayList**)&mBlockedUids);

    mHandler = new AlarmHandler(this);
    mHandler->constructor();
    mResultReceiver = new ResultReceiver(this);
    mResultReceiver->constructor();

    CSparseArray::New((ISparseArray**)&mNextAlarmClockForUser);
    CSparseArray::New((ISparseArray**)&mTmpSparseAlarmClockArray);
    CSparseBooleanArray::New((ISparseBooleanArray**)&mPendingSendNextAlarmClockChangedForUser);

    CSparseArray::New((ISparseArray**)&mHandlerSparseAlarmClockArray);

    CLinkedList::New((ILinkedList**)&mRecentWakeups);
    CArrayList::New((IArrayList**)&mAlarmBatches);

    CSparseArray::New((ISparseArray**)&mBroadcastStats);

    CAlarmManagerBinderService::New(this, (IBinder**)&mService);
    return NOERROR;
}

Int64 AlarmManagerService::ConvertToElapsed(
    /* [in] */ Int64 when,
    /* [in] */ Int32 type)
{
    Boolean isRtc = (type == IAlarmManager::RTC || type == IAlarmManager::RTC_WAKEUP  || type == IAlarmManager::RTC_POWEROFF_WAKEUP);
    if (isRtc) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 ms;
        system->GetCurrentTimeMillis(&ms);
        when -= ms - SystemClock::GetElapsedRealtime();
    }
    return when;
}

Int64 AlarmManagerService::MaxTriggerTime(
    /* [in] */ Int64 now,
    /* [in] */ Int64 triggerAtTime,
    /* [in] */ Int64 interval)
{
    // Current heuristic: batchable window is 75% of either the recurrence interval
    // [for a periodic alarm] or of the time from now to the desired delivery time,
    // with a minimum delay/interval of 10 seconds, under which we will simply not
    // defer the alarm.
    Int64 futurity = (interval == 0)
            ? (triggerAtTime - now)
            : interval;
    if (futurity < MIN_FUZZABLE_INTERVAL) {
        futurity = 0;
    }
    return triggerAtTime + (Int64)(0.75 * futurity);
}

Boolean AlarmManagerService::AddBatchLocked(
    /* [in] */ IArrayList* list,
    /* [in] */ Batch* newBatch)
{
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    Int32 index;
    collections->BinarySearch(
        IList::Probe(list), TO_IINTERFACE(newBatch), (IComparator*)sBatchOrder.Get(), &index);
    if (index < 0) {
        index = 0 - index - 1;
    }
    list->Add(index, TO_IINTERFACE(newBatch));
    return (index == 0);
}

Int32 AlarmManagerService::AttemptCoalesceLocked(
    /* [in] */ Int64 whenElapsed,
    /* [in] */ Int64 maxWhen)
{
    Int32 N;
    mAlarmBatches->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mAlarmBatches->Get(i, (IInterface**)&obj);
        Batch* b = (Batch*)IObject::Probe(obj);
        if (!b->mStandalone && b->CanHold(whenElapsed, maxWhen)) {
            return i;
        }
    }
    return -1;
}

void AlarmManagerService::RebatchAllAlarms()
{
    AutoLock syncLock(mLock);
    RebatchAllAlarmsLocked(TRUE);
}

void AlarmManagerService::RebatchAllAlarmsLocked(
    /* [in] */ Boolean doValidate)
{
    AutoPtr<IInterface> obj;
    ICloneable::Probe(mAlarmBatches)->Clone((IInterface**)&obj);
    AutoPtr<IArrayList> oldSet = IArrayList::Probe(obj);
    mAlarmBatches->Clear();
    Int64 nowElapsed = SystemClock::GetElapsedRealtime();
    Int32 oldBatches;
    oldSet->GetSize(&oldBatches);
    for (Int32 batchNum = 0; batchNum < oldBatches; batchNum++) {
        obj = NULL;
        oldSet->Get(batchNum, (IInterface**)&obj);
        AutoPtr<Batch> batch = (Batch*)IObject::Probe(obj);
        Int32 N = batch->Size();
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<Alarm> a = batch->Get(i);
            Int64 whenElapsed = ConvertToElapsed(a->mWhen, a->mType);
            Int64 maxElapsed;
            if (a->mWhenElapsed == a->mMaxWhen) {
                // Exact
                maxElapsed = whenElapsed;
            }
            else {
                // Not exact.  Preserve any explicit window, otherwise recalculate
                // the window based on the alarm's new futurity.  Note that this
                // reflects a policy of preferring timely to deferred delivery.
                maxElapsed = (a->mWindowLength > 0)
                        ? (whenElapsed + a->mWindowLength)
                        : MaxTriggerTime(nowElapsed, whenElapsed, a->mRepeatInterval);
            }
            SetImplLocked(a->mType, a->mWhen, whenElapsed, a->mWindowLength, maxElapsed,
                a->mRepeatInterval, a->mOperation, batch->mStandalone, doValidate, a->mWorkSource,
                a->mAlarmClock, a->mUserId, FALSE);
        }
    }
}

ECode AlarmManagerService::OnStart()
{
    mNativeData = Native_Init();
    mNextWakeup = mNextRtcWakeup = mNextNonWakeup = 0;

    // We have to set current TimeZone info to kernel
    // because kernel doesn't keep this after reboot
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String tz;
    sysProp->Get(TIMEZONE_PROPERTY, &tz);
    SetTimeZoneImpl(tz);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(service);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String("*alarm*"),
        (IPowerManagerWakeLock**)&mWakeLock);

    AutoPtr<IPendingIntentHelper> piHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_TIME_TICK, (IIntent**)&intent);
    intent->AddFlags(
        IIntent::FLAG_RECEIVER_REGISTERED_ONLY | IIntent::FLAG_RECEIVER_FOREGROUND);
    piHelper->GetBroadcastAsUser(context, 0,
        intent, 0, UserHandle::ALL, (IPendingIntent**)&mTimeTickSender);

    intent = NULL;
    CIntent::New(IIntent::ACTION_DATE_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    piHelper->GetBroadcastAsUser(context, 0, intent,
        IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT, UserHandle::ALL,
        (IPendingIntent**)&mDateChangeSender);

    AutoPtr<IInterface> ops;
    context->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&ops);
    mAppOps = IAppOpsManager::Probe(ops);

    // now that we have initied the driver schedule the alarm
    mClockReceiver = new ClockReceiver(this);
    FAIL_RETURN(mClockReceiver->constructor())

    mClockReceiver->ScheduleTimeTickEvent();
    mClockReceiver->ScheduleDateChangedEvent();
    mInteractiveStateReceiver = new InteractiveStateReceiver(this);
    FAIL_RETURN(mInteractiveStateReceiver->constructor())
    mUninstallReceiver = new UninstallReceiver(this);
    mQuickBootReceiver = new QuickBootReceiver(this);
    FAIL_RETURN(mUninstallReceiver->constructor())

    if (mNativeData != 0) {
        AutoPtr<AlarmThread> waitThread = new AlarmThread(this);
        waitThread->constructor(String("AlarmManager"));
        waitThread->Start();
    }
    else {
        Slogger::W(TAG, "Failed to open alarm driver. Falling back to a handler.");
    }

    PublishBinderService(IContext::ALARM_SERVICE, mService);
    return NOERROR;
}

ECode AlarmManagerService::Finalize()
{
    Native_Close(mNativeData);
    return NOERROR;
}

void AlarmManagerService::SetTimeZoneImpl(
    /* [in] */ const String& tz)
{
    if (TextUtils::IsEmpty(tz)) {
        return;
    }

    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ITimeZone> zone;
    tzHelper->GetTimeZone(tz, (ITimeZone**)&zone);
    String id;
    zone->GetID(&id);
    // Prevent reentrant calls from stepping on each other when writing
    // the time zone property
    Boolean timeZoneWasChanged = FALSE;
    {    AutoLock syncLock(this);
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        String current;
        sysProp->Get(TIMEZONE_PROPERTY, &current);
        if (current == NULL || !current.Equals(id)) {
            if (localLOGV) {
                Slogger::V(TAG, "timezone changed: %s, new=%s", current.string(), id.string());
            }
            timeZoneWasChanged = TRUE;
            sysProp->Set(TIMEZONE_PROPERTY, id);
        }

        // Update the kernel timezone information
        // Kernel tracks time offsets as 'minutes west of GMT'
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 ms;
        system->GetCurrentTimeMillis(&ms);
        Int32 gmtOffset;
        zone->GetOffset(ms, &gmtOffset);
        Native_SetKernelTimezone(mNativeData, -(gmtOffset / 60000));
    }

    tzHelper->SetDefault(NULL);

    if (timeZoneWasChanged) {
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_TIMEZONE_CHANGED, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        intent->PutExtra(String("time-zone"), id);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        context->SendBroadcastAsUser(intent, UserHandle::ALL);
    }
}

void AlarmManagerService::RemoveImpl(
    /* [in] */ IPendingIntent* operation)
{
    if (operation == NULL) {
        return;
    }

    AutoLock syncLock(mLock);
    RemoveLocked(operation);
}

ECode AlarmManagerService::SetImpl(
    /* [in] */ Int32 type,
    /* [in] */ Int64 triggerAtTime,
    /* [in] */ Int64 windowLength,
    /* [in] */ Int64 interval,
    /* [in] */ IPendingIntent* operation,
    /* [in] */ Boolean isStandalone,
    /* [in] */ IWorkSource* workSource,
    /* [in] */ IAlarmClockInfo* alarmClock)
{
    if (operation == NULL) {
        Slogger::W(TAG, "set/setRepeating ignored because there is no intent");
        return NOERROR;
    }

    // Sanity check the window length.  This will catch people mistakenly
    // trying to pass an end-of-window timestamp rather than a duration.
    if (windowLength > IAlarmManager::INTERVAL_HALF_DAY) {
        Slogger::W(TAG, "Window length %lld ms suspiciously Int64; limiting to 1 hour", windowLength);
        windowLength = IAlarmManager::INTERVAL_HOUR;
    }

    if (type < IAlarmManager::RTC_WAKEUP || type > IAlarmManager::RTC_POWEROFF_WAKEUP) {
        Slogger::E(TAG, "Invalid alarm type %d", type);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (triggerAtTime < 0) {
        Int64 who = Binder::GetCallingUid();
        Int64 what = Binder::GetCallingPid();
        Slogger::W(TAG, "Invalid alarm trigger time! %lld from uid=%d pid=%d",
            triggerAtTime, who, what);
        triggerAtTime = 0;
    }

    Int64 nowElapsed = SystemClock::GetElapsedRealtime();
    Int64 triggerElapsed = ConvertToElapsed(triggerAtTime, type);
    Int64 maxElapsed;
    if (windowLength == IAlarmManager::WINDOW_EXACT) {
        maxElapsed = triggerElapsed;
    }
    else if (windowLength < 0) {
        maxElapsed = MaxTriggerTime(nowElapsed, triggerElapsed, interval);
    }
    else {
        maxElapsed = triggerElapsed + windowLength;
    }

    Int32 userId = UserHandle::GetCallingUserId();

    Boolean wakeupFiltered = FALSE;
    Int32 id = 0;
    operation->GetCreatorUid(&id);
    String pakName;
    operation->GetCreatorPackage(&pakName);
    if (id >= IProcess::FIRST_APPLICATION_UID &&
        (type == IAlarmManager::RTC_WAKEUP || type == IAlarmManager::ELAPSED_REALTIME_WAKEUP)) {
        Int32 cop = 0;
        mAppOps->CheckOpNoThrow(IAppOpsManager::OP_ALARM_WAKEUP, id, pakName, &cop);
        if (cop != IAppOpsManager::MODE_ALLOWED) {
            if (type == IAlarmManager::RTC_WAKEUP) {
                type = IAlarmManager::RTC;
            }
            else {
                type = IAlarmManager::ELAPSED_REALTIME;
            }

            wakeupFiltered = TRUE;
        }
    }

    {    AutoLock syncLock(mLock);
        // if (DEBUG_BATCH) {
        //     Slogger::V(TAG, "Set(" + operation + ") : type=" + type
        //             + " triggerAtTime=" + triggerAtTime + " win=" + windowLength
        //             + " tElapsed=" + triggerElapsed + " maxElapsed=" + maxElapsed
        //             + " interval=" + interval + " standalone=" + isStandalone);
        // }
        SetImplLocked(type, triggerAtTime, triggerElapsed, windowLength, maxElapsed,
            interval, operation, isStandalone, TRUE, workSource, alarmClock, userId, wakeupFiltered);
    }
    return NOERROR;
}

void AlarmManagerService::SetImplLocked(
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
    /* [in] */ Boolean wakeupFiltered)
{
    AutoPtr<Alarm> a = new Alarm(type, when, whenElapsed, windowLength, maxWhen, interval,
        operation, workSource, alarmClock, userId);
    // Remove this alarm if already scheduled.
    Boolean foundExistingWakeup = RemoveWithStatusLocked(operation);

    // note AppOp for accounting purposes
    // skip if the alarm already existed
    if (!foundExistingWakeup && wakeupFiltered) {
        Int32 id = 0;
        operation->GetCreatorUid(&id);
        String pakName;
        operation->GetCreatorPackage(&pakName);
        Int32 res = 0;
        mAppOps->NoteOpNoThrow(IAppOpsManager::OP_ALARM_WAKEUP,
                id,
                pakName,
                &res);
    }

    Int32 whichBatch = (isStandalone) ? -1 : AttemptCoalesceLocked(whenElapsed, maxWhen);
    if (whichBatch < 0) {
        AutoPtr<Batch> batch = new Batch(a, this);
        batch->mStandalone = isStandalone;
        AddBatchLocked(mAlarmBatches, batch);
    }
    else {
        AutoPtr<IInterface> obj;
        mAlarmBatches->Get(whichBatch, (IInterface**)&obj);
        Batch* batch = (Batch*)IObject::Probe(obj);
        if (batch->Add(a)) {
            // The start time of this batch advanced, so batch ordering may
            // have just been broken.  Move it to where it now belongs.
            mAlarmBatches->Remove(whichBatch);
            AddBatchLocked(mAlarmBatches, batch);
        }
    }

    if (alarmClock != NULL) {
        mNextAlarmClockMayChange = TRUE;
        UpdateNextAlarmClockLocked();
    }

    if (DEBUG_VALIDATE) {
        if (doValidate && !ValidateConsistencyLocked()) {
            // Slogger::V(TAG, "Tipping-point operation: type=" + type + " when=" + when
            //         + " When(hex)=" + Long->ToHexString(when)
            //         + " whenElapsed=" + whenElapsed + " maxWhen=" + maxWhen
            //         + " interval=" + interval + " op=" + operation
            //         + " standalone=" + isStandalone);
            RebatchAllAlarmsLocked(FALSE);
        }
    }

    RescheduleKernelAlarmsLocked();
}

void AlarmManagerService::DumpImpl(
    /* [in] */ IPrintWriter* pw)
{
    // {    AutoLock syncLock(mLock);
    //     AutoPtr<ICollections> collections;
    //     CCollections::AcquireSingleton((ICollections**)&collections);
    //     pw->Println("Current Alarm Manager state:");
    //     Int64 nowRTC = system->GetCurrentTimeMillis();
    //     Int64 nowELAPSED = SystemClock::GetElapsedRealtime();
    //     SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

    //     pw->Print("nowRTC="); pw->Print(nowRTC);
    //     pw->Print("="); pw->Print(sdf->Format(new Date(nowRTC)));
    //     pw->Print(" nowELAPSED="); TimeUtils->FormatDuration(nowELAPSED, pw);
    //     pw->Println();
    //     if (!mInteractive) {
    //         pw->Print("Time since non-interactive: ");
    //         TimeUtils->FormatDuration(nowELAPSED - mNonInteractiveStartTime, pw);
    //         pw->Println();
    //         pw->Print("Max wakeup delay: ");
    //         TimeUtils->FormatDuration(CurrentNonWakeupFuzzLocked(nowELAPSED), pw);
    //         pw->Println();
    //         pw->Print("Time since last dispatch: ");
    //         TimeUtils->FormatDuration(nowELAPSED - mLastAlarmDeliveryTime, pw);
    //         pw->Println();
    //         pw->Print("Next non-wakeup delivery time: ");
    //         TimeUtils->FormatDuration(nowELAPSED - mNextNonWakeupDeliveryTime, pw);
    //         pw->Println();
    //     }

    //     Int64 nextWakeupRTC = mNextWakeup + (nowRTC - nowELAPSED);
    //     Int64 nextNonWakeupRTC = mNextNonWakeup + (nowRTC - nowELAPSED);
    //     pw->Print("Next non-wakeup alarm: ");
    //             TimeUtils->FormatDuration(mNextNonWakeup, nowELAPSED, pw);
    //             pw->Print(" = "); pw->Println(sdf->Format(new Date(nextNonWakeupRTC)));
    //     pw->Print("Next wakeup: "); TimeUtils->FormatDuration(mNextWakeup, nowELAPSED, pw);
    //             pw->Print(" = "); pw->Println(sdf->Format(new Date(nextWakeupRTC)));
    //     pw->Print("Num time change events: "); pw->Println(mNumTimeChanged);

    //     if (mAlarmBatches->GetSize(&size) > 0) {
    //         pw->Println();
    //         pw->Print("Pending alarm batches: ");
    //         pw->Println(mAlarmBatches->GetSize(&size));
    //         for (Batch b : mAlarmBatches) {
    //             pw->Print(b); pw->Println(':');
    //             DumpAlarmList(pw, b->mAlarms, "  ", nowELAPSED, nowRTC, sdf);
    //         }
    //     }

    //     pw->Println();
    //     pw->Print("Past-due non-wakeup mAlarms: ");
    //     if (mPendingNonWakeupAlarms->GetSize(&size) > 0) {
    //         pw->Println(mPendingNonWakeupAlarms->GetSize(&size));
    //         DumpAlarmList(pw, mPendingNonWakeupAlarms, "  ", nowELAPSED, nowRTC, sdf);
    //     } else {
    //         pw->Println("(none)");
    //     }
    //     pw->Print("  Number of delayed mAlarms: "); pw->Print(mNumDelayedAlarms);
    //     pw->Print(", total delay time: "); TimeUtils->FormatDuration(mTotalDelayTime, pw);
    //     pw->Println();
    //     pw->Print("  Max delay time: "); TimeUtils->FormatDuration(mMaxDelayTime, pw);
    //     pw->Print(", max non-interactive time: ");
    //     TimeUtils->FormatDuration(mNonInteractiveTime, pw);
    //     pw->Println();

    //     pw->Println();
    //     pw->Print("  Broadcast ref count: "); pw->Println(mBroadcastRefCount);
    //     pw->Println();

    //     if (mSlogger::Dump(pw, "  Recent problems", "    ")) {
    //         pw->Println();
    //     }

    //     FilterStats[] topFilters = new FilterStats[10];
    //     Comparator<FilterStats> comparator = new Comparator<FilterStats>() {
    //         //@Override
    //         public Int32 Compare(FilterStats lhs, FilterStats rhs) {
    //             if (lhs->mAggregateTime < rhs->mAggregateTime) {
    //                 return 1;
    //             } else if (lhs->mAggregateTime > rhs->mAggregateTime) {
    //                 return -1;
    //             }
    //             return 0;
    //         }
    //     };
    //     Int32 len = 0;
    //     for (Int32 iu=0; iu<mBroadcastStats->GetSize(&size); iu++) {
    //         ArrayMap<String, BroadcastStats> uidStats = mBroadcastStats->ValueAt(iu);
    //         for (Int32 ip=0; ip<uidStats->GetSize(&size); ip++) {
    //             BroadcastStats bs = uidStats->ValueAt(ip);
    //             for (Int32 is=0; is<bs.filterStats->GetSize(&size); is++) {
    //                 FilterStats fs = bs.filterStats->ValueAt(is);
    //                 Int32 pos = len > 0
    //                         ? Arrays->BinarySearch(topFilters, 0, len, fs, comparator) : 0;
    //                 if (pos < 0) {
    //                     pos = -pos - 1;
    //                 }
    //                 if (pos < topFilters.length) {
    //                     Int32 copylen = topFilters.length - pos - 1;
    //                     if (copylen > 0) {
    //                         System->Arraycopy(topFilters, pos, topFilters, pos+1, copylen);
    //                     }
    //                     topFilters[pos] = fs;
    //                     if (len < topFilters.length) {
    //                         len++;
    //                     }
    //                 }
    //             }
    //         }
    //     }
    //     if (len > 0) {
    //         pw->Println("  Top Alarms:");
    //         for (Int32 i=0; i<len; i++) {
    //             FilterStats fs = topFilters[i];
    //             pw->Print("    ");
    //             if (fs->mNesting > 0) pw->Print("*ACTIVE* ");
    //             TimeUtils->FormatDuration(fs->mAggregateTime, pw);
    //             pw->Print(" running, "); pw->Print(fs->mNumWakeup);
    //             pw->Print(" wakeups, "); pw->Print(fs->mCount);
    //             pw->Print(" mAlarms: "); UserHandle::FormatUid(pw, fs->mBroadcastStats.mUid);
    //             pw->Print(":"); pw->Print(fs->mBroadcastStats.mPackageName);
    //             pw->Println();
    //             pw->Print("      "); pw->Print(fs.mTag);
    //             pw->Println();
    //         }
    //     }

    //     pw->Println(" ");
    //     pw->Println("  Alarm Stats:");
    //     ArrayList<FilterStats> tmpFilters = new ArrayList<FilterStats>();
    //     for (Int32 iu=0; iu<mBroadcastStats->GetSize(&size); iu++) {
    //         ArrayMap<String, BroadcastStats> uidStats = mBroadcastStats->ValueAt(iu);
    //         for (Int32 ip=0; ip<uidStats->GetSize(&size); ip++) {
    //             BroadcastStats bs = uidStats->ValueAt(ip);
    //             pw->Print("  ");
    //             if (bs->mNesting > 0) pw->Print("*ACTIVE* ");
    //             UserHandle::FormatUid(pw, bs.mUid);
    //             pw->Print(":");
    //             pw->Print(bs.mPackageName);
    //             pw->Print(" "); TimeUtils->FormatDuration(bs->mAggregateTime, pw);
    //                     pw->Print(" running, "); pw->Print(bs->mNumWakeup);
    //                     pw->Println(" wakeups:");
    //             tmpFilters->Clear();
    //             for (Int32 is=0; is<bs.filterStats->GetSize(&size); is++) {
    //                 tmpFilters->Add(bs.filterStats->ValueAt(is));
    //             }
    //             collections->Sort(tmpFilters, comparator);
    //             for (Int32 i=0; i<tmpFilters->GetSize(&size); i++) {
    //                 FilterStats fs = tmpFilters->Get(i);
    //                 pw->Print("    ");
    //                         if (fs->mNesting > 0) pw->Print("*ACTIVE* ");
    //                         TimeUtils->FormatDuration(fs->mAggregateTime, pw);
    //                         pw->Print(" "); pw->Print(fs->mNumWakeup);
    //                         pw->Print(" wakes " ); pw->Print(fs->mCount);
    //                         pw->Print(" mAlarms: ");
    //                         pw->Print(fs.mTag);
    //                         pw->Println();
    //             }
    //         }
    //     }

    //     if (WAKEUP_STATS) {
    //         pw->Println();
    //         pw->Println("  Recent Wakeup History:");
    //         Int64 last = -1;
    //         for (WakeupEvent event : mRecentWakeups) {
    //             pw->Print("    "); pw->Print(sdf->Format(new Date(event->mWhen)));
    //             pw->Print('|');
    //             if (last < 0) {
    //                 pw->Print('0');
    //             } else {
    //                 pw->Print(event->mWhen - last);
    //             }
    //             last = event->mWhen;
    //             pw->Print('|'); pw->Print(event.uid);
    //             pw->Print('|'); pw->Print(event.action);
    //             pw->Println();
    //         }
    //         pw->Println();
    //     }
    // }
}

void AlarmManagerService::LogBatchesLocked(
    /* [in] */ ISimpleDateFormat* sdf)
{
    AutoPtr<IByteArrayOutputStream> bs;
    CByteArrayOutputStream::New(2048, (IByteArrayOutputStream**)&bs);
    AutoPtr<IPrintWriter> pw;
    CPrintWriter::New(IOutputStream::Probe(bs), (IPrintWriter**)&pw);
    IFlushable* wrter = IFlushable::Probe(pw);
    IAppendable* ap = IAppendable::Probe(pw);

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 nowRTC;
    system->GetCurrentTimeMillis(&nowRTC);
    Int64 nowELAPSED = SystemClock::GetElapsedRealtime();
    Int32 NZ;
    mAlarmBatches->GetSize(&NZ);
    for (Int32 iz = 0; iz < NZ; iz++) {
        AutoPtr<IInterface> obj;
        mAlarmBatches->Get(iz, (IInterface**)&obj);
        AutoPtr<Batch> bz = (Batch*)IObject::Probe(obj);
        ap->Append(CoreUtils::Convert("Batch "));
        pw->Print(iz);
        ap->Append(CoreUtils::Convert(": ")); pw->Println(Object::ToString(bz));
        DumpAlarmList(pw, bz->mAlarms, String("  "), nowELAPSED, nowRTC, sdf);
        wrter->Flush();
        Slogger::V(TAG, TO_CSTR(bs));
        bs->Reset();
    }
}

Boolean AlarmManagerService::ValidateConsistencyLocked()
{
    if (DEBUG_VALIDATE) {
        Int64 lastTime = Elastos::Core::Math::INT64_MIN_VALUE;
        Int32 N;
        mAlarmBatches->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mAlarmBatches->Get(i, (IInterface**)&obj);
            AutoPtr<Batch> b = (Batch*)IObject::Probe(obj);
            if (b->mStart >= lastTime) {
                // duplicate start times are okay because of standalone batches
                lastTime = b->mStart;
            }
            else {
                Slogger::E(TAG, "CONSISTENCY FAILURE: Batch %d is out of order", i);
                AutoPtr<ISimpleDateFormat> sdf;
                CSimpleDateFormat::New(String("yyyy-MM-dd HH:mm:ss"), (ISimpleDateFormat**)&sdf);
                LogBatchesLocked(sdf);
                return FALSE;
            }
        }
    }
    return TRUE;
}

AutoPtr<AlarmManagerService::Batch> AlarmManagerService::FindFirstWakeupBatchLocked()
{
    Int32 N = 0;
    mAlarmBatches->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mAlarmBatches->Get(i, (IInterface**)&obj);
        AutoPtr<Batch> b = (Batch*)IObject::Probe(obj);
        if (b->HasWakeups()) {
            return b;
        }
    }
    return NULL;
}

AutoPtr<AlarmManagerService::Batch> AlarmManagerService::FindFirstRtcWakeupBatchLocked()
{
    Int32 N = 0;
    mAlarmBatches->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mAlarmBatches->Get(i, (IInterface**)&obj);
        AutoPtr<Batch> b = (Batch*)IObject::Probe(obj);
        Int64 intervalTime  = b->mStart - SystemClock::GetElapsedRealtime();
        if (b->IsRtcPowerOffWakeup() && intervalTime > POWER_OFF_ALARM_THRESHOLD) {
            return b;
        }
    }
    return NULL;
}

AutoPtr<IAlarmClockInfo> AlarmManagerService::GetNextAlarmClockImpl(
    /* [in] */ Int32 userId)
{
    {    AutoLock syncLock(mLock);
        AutoPtr<IInterface> obj;
        mNextAlarmClockForUser->Get(userId, (IInterface**)&obj);
        IAlarmClockInfo* info = IAlarmClockInfo::Probe(obj);
        return info;
    }
    return NULL;
}

void AlarmManagerService::UpdateNextAlarmClockLocked()
{
    if (!mNextAlarmClockMayChange) {
        return;
    }
    mNextAlarmClockMayChange = FALSE;

    AutoPtr<ISparseArray> nextForUser = mTmpSparseAlarmClockArray;
    nextForUser->Clear();

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 N = 0;
    mAlarmBatches->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mAlarmBatches->Get(i, (IInterface**)&obj);
        AutoPtr<Batch> b = (Batch*)IObject::Probe(obj);
        AutoPtr<IArrayList> mAlarms = b->mAlarms;

        Int32 M = 0;
        mAlarms->GetSize(&M);
        for (Int32 j = 0; j < M; j++) {
            obj = NULL;
            mAlarms->Get(j, (IInterface**)&obj);
            AutoPtr<Alarm> a = (Alarm*)IObject::Probe(obj);
            if (a->mAlarmClock != NULL) {
                Int32 userId = a->mUserId;

                if (DEBUG_ALARM_CLOCK) {
                    Slogger::V(TAG, "Found AlarmClockInfo at %s for user %d",
                        FormatNextAlarm(context, a->mAlarmClock).string(), userId);
                }

                // Alarms and batches are sorted by time, no need to compare times here.
                obj = NULL;
                nextForUser->Get(userId, (IInterface**)&obj);
                if (obj == NULL) {
                    nextForUser->Put(userId, TO_IINTERFACE(a->mAlarmClock));
                }
            }
        }
    }

    // Update mNextAlarmForUser with new values.
    Int32 NN = 0;
    nextForUser->GetSize(&NN);
    for (Int32 i = 0; i < NN; i++) {
        AutoPtr<IInterface> obj;
        nextForUser->ValueAt(i, (IInterface**)&obj);
        AutoPtr<IAlarmClockInfo> newAlarm = IAlarmClockInfo::Probe(obj);
        Int32 userId;
        nextForUser->KeyAt(i, &userId);

        obj = NULL;
        mNextAlarmClockForUser->Get(userId, (IInterface**)&obj);
        AutoPtr<IAlarmClockInfo> currentAlarm = IAlarmClockInfo::Probe(obj);
        if (!Object::Equals(newAlarm, currentAlarm)) {
            UpdateNextAlarmInfoForUserLocked(userId, newAlarm);
        }
    }

    // Remove users without any alarm clocks scheduled.
    Int32 NNN = 0;
    mNextAlarmClockForUser->GetSize(&NNN);
    for (Int32 i = NNN - 1; i >= 0; i--) {
        Int32 userId;
        mNextAlarmClockForUser->KeyAt(i, &userId);
        AutoPtr<IInterface> obj;
        nextForUser->ValueAt(userId, (IInterface**)&obj);
        if (obj == NULL) {
            UpdateNextAlarmInfoForUserLocked(userId, NULL);
        }
    }
}

void AlarmManagerService::UpdateNextAlarmInfoForUserLocked(
    /* [in] */ Int32 userId,
    /* [in] */ IAlarmClockInfo* alarmClock)
{
    if (alarmClock != NULL) {
        if (DEBUG_ALARM_CLOCK) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            Slogger::V(TAG, "Next AlarmClockInfoForUser(%d): %s",
                userId, FormatNextAlarm(context, alarmClock).string());
        }
        mNextAlarmClockForUser->Put(userId, alarmClock);
    }
    else {
        if (DEBUG_ALARM_CLOCK) {
            Slogger::V(TAG, "Next AlarmClockInfoForUser(%d): None", userId);
        }
        mNextAlarmClockForUser->Remove(userId);
    }

    mPendingSendNextAlarmClockChangedForUser->Put(userId, TRUE);
    mHandler->RemoveMessages(AlarmHandler::SEND_NEXT_ALARM_CLOCK_CHANGED);
    Boolean bval;
    mHandler->SendEmptyMessage(AlarmHandler::SEND_NEXT_ALARM_CLOCK_CHANGED, &bval);
}

void AlarmManagerService::SendNextAlarmClockChanged()
{
    AutoPtr<ISparseArray> pendingUsers = mHandlerSparseAlarmClockArray;
    pendingUsers->Clear();

    {    AutoLock syncLock(mLock);
        Int32 N;
        mPendingSendNextAlarmClockChangedForUser->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            Int32 userId;
            mPendingSendNextAlarmClockChangedForUser->KeyAt(i, &userId);
            AutoPtr<IInterface> obj;
            mNextAlarmClockForUser->Get(userId, (IInterface**)&obj);
            pendingUsers->Append(userId, obj);
        }
        mPendingSendNextAlarmClockChangedForUser->Clear();
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);

    Boolean bval;
    Int32 N;
    pendingUsers->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 userId;
        pendingUsers->KeyAt(i, &userId);
        AutoPtr<IInterface> obj;
        pendingUsers->ValueAt(userId, (IInterface**)&obj);
        IAlarmClockInfo* alarmClock = IAlarmClockInfo::Probe(obj);
        String alarmStr = FormatNextAlarm(context, alarmClock);
        Settings::System::PutStringForUser(cr,
            ISettingsSystem::NEXT_ALARM_FORMATTED,
            alarmStr, userId, &bval);

        AutoPtr<IUserHandle> user;
        CUserHandle::New(userId, (IUserHandle**)&user);
        context->SendBroadcastAsUser(NEXT_ALARM_CLOCK_CHANGED_INTENT, user);
    }
}

String AlarmManagerService::FormatNextAlarm(
    /* [in] */ IContext* context,
    /* [in] */ IAlarmClockInfo* info)
{
    AutoPtr<IDateFormat> dfHelper;
    CDateFormat::AcquireSingleton((IDateFormat**)&dfHelper);
    AutoPtr<ILocaleHelper> locale;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&locale);
    Boolean bval;
    dfHelper->Is24HourFormat(context, &bval);
    String skeleton(bval ? "EHm" : "Ehma");
    String pattern;
    AutoPtr<ILocale> dl;
    locale->GetDefault((ILocale**)&dl);
    dfHelper->GetBestDateTimePattern(dl, skeleton, &pattern);
    String result("");
    if (info != NULL) {
        Int64 tt;
        info->GetTriggerTime(&tt);
        AutoPtr<ICharSequence> csq;
        dfHelper->Format(CoreUtils::Convert(pattern), tt, (ICharSequence**)&csq);
        result = Object::ToString(csq);
    }
    return result;
}

void AlarmManagerService::RescheduleKernelAlarmsLocked()
{
    // Schedule the next upcoming wakeup alarm.  If there is a deliverable batch
    // prior to that which contains no wakeups, we schedule that as well.
    Int64 nextNonWakeup = 0;
    Int32 size = 0;
    mAlarmBatches->GetSize(&size);
    if (size > 0) {
        AutoPtr<Batch> firstWakeup = FindFirstWakeupBatchLocked();
        AutoPtr<IInterface> obj;
        mAlarmBatches->Get(0, (IInterface**)&obj);
        Batch* firstBatch = (Batch*)IObject::Probe(obj);
        AutoPtr<Batch> firstRtcWakeup = FindFirstRtcWakeupBatchLocked();
        if (firstWakeup != NULL && mNextWakeup != firstWakeup->mStart) {
            mNextWakeup = firstWakeup->mStart;
            SetLocked(IAlarmManager::ELAPSED_REALTIME_WAKEUP, firstWakeup->mStart);
        }
        if (firstRtcWakeup != NULL && mNextRtcWakeup != firstRtcWakeup->mStart) {
            mNextRtcWakeup = firstRtcWakeup->mStart;
            Int64 when = firstRtcWakeup->GetWhenByElapsedTime(mNextRtcWakeup);

            if (when != 0) {
                SetLocked(IAlarmManager::RTC_POWEROFF_WAKEUP, when);
            }
        }
        if (firstBatch != firstWakeup) {
            nextNonWakeup = firstBatch->mStart;
        }
    }
    mPendingNonWakeupAlarms->GetSize(&size);
    if (size > 0) {
        if (nextNonWakeup == 0 || mNextNonWakeupDeliveryTime < nextNonWakeup) {
            nextNonWakeup = mNextNonWakeupDeliveryTime;
        }
    }
    if (nextNonWakeup != 0 && mNextNonWakeup != nextNonWakeup) {
        mNextNonWakeup = nextNonWakeup;
        SetLocked(IAlarmManager::ELAPSED_REALTIME, nextNonWakeup);
    }
}

Boolean AlarmManagerService::CheckReleaseWakeLock()
{
    Int32 tSize = 0, bSize = 0;
    mTriggeredUids->GetSize(&tSize);
    mBlockedUids->GetSize(&bSize);
    if (tSize == 0 || bSize == 0) {
        return FALSE;
    }

    for (Int32 i = 0; i < tSize; i++) {
        AutoPtr<IInterface> uid;
        mTriggeredUids->Get(i, (IInterface**)&uid);
        Boolean b = FALSE;
        mBlockedUids->Contains(uid, &b);
        if (!b) {
            return FALSE;
        }
    }
    return TRUE;
}

void AlarmManagerService::RemoveLocked(
    /* [in] */ IPendingIntent* operation)
{
    RemoveWithStatusLocked(operation);
}

Boolean AlarmManagerService::RemoveWithStatusLocked(
    /* [in] */ IPendingIntent* operation)
{
    IArrayList* alarmList;
    Alarm* alarm;
    Int64 alarmSeconds = 0, alarmNanoseconds = 0;
    Boolean didRemove = FALSE;

    Int32 size = 0;
    for (Int32 i = (mAlarmBatches->GetSize(&size), size) - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mAlarmBatches->Get(i, (IInterface**)&obj);
        Batch* b = (Batch*)IObject::Probe(obj);
        alarmList = b->mAlarms;
        Int32 lSize = 0;
        for (Int32 j = (alarmList->GetSize(&lSize), lSize) - 1; j >= 0; j--) {
            AutoPtr<IInterface> p;
            alarmList->Get(j, (IInterface**)&p);
            alarm = (Alarm*)IObject::Probe(p);
            if (alarm->mType == IAlarmManager::RTC_POWEROFF_WAKEUP
                && Object::Equals(alarm->mOperation, operation)) {
                alarmSeconds = alarm->mWhen / 1000;
                alarmNanoseconds = (alarm->mWhen % 1000) * 1000 * 1000;
                Slogger::W(TAG,"Clear alarm type=%d,alarmSeconds=%lld", alarm->mType, alarmSeconds);
                Native_Clear(mNativeData, alarm->mType, alarmSeconds, alarmNanoseconds);
                mNextRtcWakeup = 0;
            }
        }
        didRemove |= b->Remove(operation);
        if (b->Size() == 0) {
            mAlarmBatches->Remove(i);
        }
    }

    if (didRemove) {
        if (DEBUG_BATCH) {
            Slogger::V(TAG, "Remove(operation) changed bounds; rebatching");
        }
        RebatchAllAlarmsLocked(TRUE);
        RescheduleKernelAlarmsLocked();
        UpdateNextAlarmClockLocked();
    }

    return didRemove;
}

void AlarmManagerService::RemoveLocked(
    /* [in] */ const String& packageName)
{
    Boolean didRemove = FALSE;
    Int32 size;
    mAlarmBatches->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mAlarmBatches->Get(i, (IInterface**)&obj);
        Batch* b = (Batch*)IObject::Probe(obj);
        didRemove |= b->Remove(packageName);
        if (b->Size() == 0) {
            mAlarmBatches->Remove(i);
        }
    }

    if (didRemove) {
        if (DEBUG_BATCH) {
            Slogger::V(TAG, "Remove(package) changed bounds; rebatching");
        }
        RebatchAllAlarmsLocked(TRUE);
        RescheduleKernelAlarmsLocked();
        UpdateNextAlarmClockLocked();
    }
}

void AlarmManagerService::RemoveUserLocked(
    /* [in] */ Int32 userHandle)
{
    Boolean didRemove = FALSE;
    Int32 size;
    mAlarmBatches->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mAlarmBatches->Get(i, (IInterface**)&obj);
        Batch* b = (Batch*)IObject::Probe(obj);
        didRemove |= b->Remove(userHandle);
        if (b->Size() == 0) {
            mAlarmBatches->Remove(i);
        }
    }

    if (didRemove) {
        if (DEBUG_BATCH) {
            Slogger::V(TAG, "Remove(user) changed bounds; rebatching");
        }
        RebatchAllAlarmsLocked(TRUE);
        RescheduleKernelAlarmsLocked();
        UpdateNextAlarmClockLocked();
    }
}

void AlarmManagerService::InteractiveStateChangedLocked(
    /* [in] */ Boolean interactive)
{
    if (mInteractive != interactive) {
        mInteractive = interactive;
        Int64 nowELAPSED = SystemClock::GetElapsedRealtime();
        if (interactive) {
            Int32 size;
            mPendingNonWakeupAlarms->GetSize(&size);
            if (size > 0) {
                Int64 thisDelayTime = nowELAPSED - mStartCurrentDelayTime;
                mTotalDelayTime += thisDelayTime;
                if (mMaxDelayTime < thisDelayTime) {
                    mMaxDelayTime = thisDelayTime;
                }
                DeliverAlarmsLocked(mPendingNonWakeupAlarms, nowELAPSED);
                mPendingNonWakeupAlarms->Clear();
            }
            if (mNonInteractiveStartTime > 0) {
                Int64 dur = nowELAPSED - mNonInteractiveStartTime;
                if (dur > mNonInteractiveTime) {
                    mNonInteractiveTime = dur;
                }
            }
        }
        else {
            mNonInteractiveStartTime = nowELAPSED;
        }
    }
}

Boolean AlarmManagerService::LookForPackageLocked(
    /* [in] */ const String& packageName)
{
    Int32 size;
    mAlarmBatches->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mAlarmBatches->Get(i, (IInterface**)&obj);
        Batch* b = (Batch*)IObject::Probe(obj);
        if (b->HasPackage(packageName)) {
            return TRUE;
        }
    }
    return FALSE;
}

void AlarmManagerService::SetLocked(
    /* [in] */ Int32 type, Int64 when)
{
    if (mNativeData != 0) {
        // The kernel never triggers mAlarms with negative wakeup times
        // so we ensure they are positive.
        Int64 alarmSeconds, alarmNanoseconds;
        if (when < 0) {
            alarmSeconds = 0;
            alarmNanoseconds = 0;
        }
        else {
            alarmSeconds = when / 1000;
            alarmNanoseconds = (when % 1000) * 1000 * 1000;
        }

        Native_Set(mNativeData, type, alarmSeconds, alarmNanoseconds);
    }
    else {
        AutoPtr<IMessageHelper> msgHelper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&msgHelper);
        AutoPtr<IMessage> msg;
        msgHelper->Obtain((IMessage**)&msg);
        msg->SetWhat(ALARM_EVENT);

        Boolean bval;
        mHandler->RemoveMessages(ALARM_EVENT);
        mHandler->SendMessageAtTime(msg, when, &bval);
    }
}

void AlarmManagerService::DumpAlarmList(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ IArrayList* list,
    /* [in] */ const String& prefix,
    /* [in] */ const String& label,
    /* [in] */ Int64 nowRTC,
    /* [in] */ Int64 nowELAPSED,
    /* [in] */ ISimpleDateFormat* sdf)
{
    StringBuilder sb(prefix);
    sb += "  ";
    String prefixex = sb.ToString();

    Int32 size = 0;
    list->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        Alarm* a = (Alarm*)IObject::Probe(obj);
        pw->Print(prefix);
        pw->Print(label);
        pw->Print(String(" #"));
        pw->Print(i);
        pw->Print(String(": ")); pw->Println(Object::ToString(a));
        a->Dump(pw, prefixex, nowRTC, nowELAPSED, sdf);
    }
}

String AlarmManagerService::LabelForType(
    /* [in] */ Int32 type)
{
    switch (type) {
    case IAlarmManager::RTC: return String("RTC");
    case IAlarmManager::RTC_WAKEUP : return String("RTC_WAKEUP");
    case IAlarmManager::ELAPSED_REALTIME : return String("ELAPSED");
    case IAlarmManager::ELAPSED_REALTIME_WAKEUP: return String("ELAPSED_WAKEUP");
    case IAlarmManager::RTC_POWEROFF_WAKEUP : return String("RTC_POWEROFF_WAKEUP");
    default:
        break;
    }
    return String("--unknown--");
}

void AlarmManagerService::DumpAlarmList(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ IArrayList* list,
    /* [in] */ const String& prefix,
    /* [in] */ Int64 nowELAPSED,
    /* [in] */ Int64 nowRTC,
    /* [in] */ ISimpleDateFormat* sdf)
{
    StringBuilder sb(prefix);
    sb += "  ";
    String prefixex = sb.ToString();

    Int32 size = 0;
    list->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        Alarm* a = (Alarm*)IObject::Probe(obj);

        String label = LabelForType(a->mType);
        pw->Print(prefix); pw->Print(label); pw->Print(String(" #")); pw->Print(i);
                pw->Print(String(": ")); pw->Println(Object::ToString(a));
        a->Dump(pw, prefixex, nowRTC, nowELAPSED, sdf);
    }
}

Boolean AlarmManagerService::TriggerAlarmsLocked(
    /* [in] */ IArrayList* triggerList,
    /* [in] */ Int64 nowELAPSED,
    /* [in] */ Int64 nowRTC)
{
    Boolean hasWakeup = FALSE;
    // batches are temporally sorted, so we need only pull from the
    // start of the list until we either empty it or hit a batch
    // that is not yet deliverable
    Int32 size;
    while ((mAlarmBatches->GetSize(&size), size) > 0) {
        AutoPtr<IInterface> obj;
        mAlarmBatches->Get(0, (IInterface**)&obj);
        Batch* batch = (Batch*)IObject::Probe(obj);
        if (batch->mStart > nowELAPSED) {
            // Everything else is scheduled for the future
            break;
        }

        // We will (re)schedule some mAlarms now; don't let that interfere
        // with delivery of this current batch
        mAlarmBatches->Remove(0);

        Int32 N = batch->Size();
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<Alarm> alarm = batch->Get(i);
            alarm->mCount = 1;
            triggerList->Add(TO_IINTERFACE(alarm));

            // Recurring mAlarms may have passed several alarm intervals while the
            // phone was asleep or off, so pass a trigger count when sending them.
            if (alarm->mRepeatInterval > 0) {
                // this adjustment will be zero if we're late by
                // less than one full repeat interval
                alarm->mCount += (nowELAPSED - alarm->mWhenElapsed) / alarm->mRepeatInterval;

                // Also schedule its next recurrence
                Int64 delta = alarm->mCount * alarm->mRepeatInterval;
                Int64 nextElapsed = alarm->mWhenElapsed + delta;
                SetImplLocked(alarm->mType, alarm->mWhen + delta, nextElapsed, alarm->mWindowLength,
                        MaxTriggerTime(nowELAPSED, nextElapsed, alarm->mRepeatInterval),
                        alarm->mRepeatInterval, alarm->mOperation, batch->mStandalone, TRUE,
                        alarm->mWorkSource, alarm->mAlarmClock, alarm->mUserId, FALSE);
            }

            if (alarm->mWakeup) {
                hasWakeup = TRUE;
            }

            // We removed an alarm clock. Let the caller recompute the next alarm clock.
            if (alarm->mAlarmClock != NULL) {
                mNextAlarmClockMayChange = TRUE;
            }
        }
    }

    // This is a new alarm delivery set; bump the sequence number to indicate that
    // all apps' alarm delivery classes should be recalculated.
    mCurrentSeq++;
    CalculateDeliveryPriorities(triggerList);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Sort(IList::Probe(triggerList), (IComparator*)mAlarmDispatchComparator.Get());

    if (localLOGV) {
        triggerList->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            triggerList->Get(i, (IInterface**)&obj);
            Slogger::V(TAG, "Triggering alarm #%d: %s", i, TO_CSTR(obj));
        }
    }

    return hasWakeup;
}

void AlarmManagerService::RecordWakeupAlarms(
    /* [in] */ IArrayList* batches,
    /* [in] */ Int64 nowELAPSED,
    /* [in] */ Int64 nowRTC)
{
    Int32 numBatches;
    batches->GetSize(&numBatches);
    for (Int32 nextBatch = 0; nextBatch < numBatches; nextBatch++) {
        AutoPtr<IInterface> obj;
        batches->Get(nextBatch, (IInterface**)&obj);
        Batch* b = (Batch*)IObject::Probe(obj);
        if (b->mStart > nowELAPSED) {
            break;
        }

        Int32 numAlarms;
        b->mAlarms->GetSize(&numAlarms);
        for (Int32 nextAlarm = 0; nextAlarm < numAlarms; nextAlarm++) {
            AutoPtr<IInterface> aObj;
            b->mAlarms->Get(nextAlarm, (IInterface**)&aObj);
            Alarm* a = (Alarm*)IObject::Probe(aObj);
            AutoPtr<IIntent> intent;
            a->mOperation->GetIntent((IIntent**)&intent);
            String action;
            intent->GetAction(&action);
            Int32 uid;
            a->mOperation->GetCreatorUid(&uid);
            AutoPtr<WakeupEvent> e = new WakeupEvent(nowRTC, uid, action);
            mRecentWakeups->Add(TO_IINTERFACE(e));
        }
    }
}

Int64 AlarmManagerService::CurrentNonWakeupFuzzLocked(
    /* [in] */ Int64 nowELAPSED)
{
    Int64 timeSinceOn = nowELAPSED - mNonInteractiveStartTime;
    if (timeSinceOn < 5 * 60 * 1000) {
        // If the screen has been off for 5 minutes, only delay by at most two minutes.
        return 2 * 60 * 1000;
    }
    else if (timeSinceOn < 30 * 60 * 1000) {
        // If the screen has been off for 30 minutes, only delay by at most 15 minutes.
        return 15 * 60 * 1000;
    }

    // Otherwise, we will delay by at most an hour.
    return 60 * 60 * 1000;
}

Boolean AlarmManagerService::CheckAllowNonWakeupDelayLocked(
    /* [in] */ Int64 nowELAPSED)
{
    if (mInteractive) {
        return FALSE;
    }
    if (mLastAlarmDeliveryTime <= 0) {
        return FALSE;
    }
    Int32 size;
    mPendingNonWakeupAlarms->GetSize(&size);
    if (size > 0 && mNextNonWakeupDeliveryTime > nowELAPSED) {
        // This is just a little paranoia, if somehow we have pending non-wakeup mAlarms
        // and the next delivery time is in the past, then just deliver them all.  This
        // avoids bugs where we get stuck in a loop trying to poll for mAlarms.
        return FALSE;
    }
    Int64 timeSinceLast = nowELAPSED - mLastAlarmDeliveryTime;
    return timeSinceLast <= CurrentNonWakeupFuzzLocked(nowELAPSED);
}

void AlarmManagerService::DeliverAlarmsLocked(
    /* [in] */ IArrayList* triggerList,
    /* [in] */ Int64 nowELAPSED)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    ECode ec = NOERROR;
    String name;
    Boolean bIsHeld = FALSE;

    mLastAlarmDeliveryTime = nowELAPSED;
    Int32 size = 0, ival, ival2;
    triggerList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> aObj;
        triggerList->Get(i, (IInterface**)&aObj);
        Alarm* alarm = (Alarm*)IObject::Probe(aObj);
        AutoPtr<InFlight> inflight;
        AutoPtr<BroadcastStats> bs;
        AutoPtr<FilterStats> fs;

        // try {
        if (localLOGV) {
            Slogger::V(TAG, "sending alarm %s", TO_CSTR(alarm));
        }
        ec = mBackgroundIntent->PutExtra(IIntent::EXTRA_ALARM_COUNT, alarm->mCount);
        FAIL_GOTO(ec, _EXIT_)

        ec = alarm->mOperation->Send(context, 0, mBackgroundIntent, mResultReceiver, mHandler);
        FAIL_GOTO(ec, _EXIT_)

        // we have an active broadcast so stay awake.
        mWakeLock->IsHeld(&bIsHeld);
        if (mBroadcastRefCount == 0 || !bIsHeld) {
            SetWakelockWorkSource(alarm->mOperation, alarm->mWorkSource, alarm->mType, alarm->mTag, TRUE);
            mWakeLock->AcquireLock();
        }

        inflight = new InFlight();
        inflight->constructor(this,
                alarm->mOperation, alarm->mWorkSource, alarm->mType, alarm->mTag, alarm->mUid);
        mInFlight->Add(TO_IINTERFACE(inflight));
        mBroadcastRefCount++;
        mTriggeredUids->Add(CoreUtils::Convert(alarm->mUid));

        bs = inflight->mBroadcastStats;
        bs->mCount++;
        if (bs->mNesting == 0) {
            bs->mNesting = 1;
            bs->mStartTime = nowELAPSED;
        }
        else {
            bs->mNesting++;
        }

        fs = inflight->mFilterStats;
        fs->mCount++;
        if (fs->mNesting == 0) {
            fs->mNesting = 1;
            fs->mStartTime = nowELAPSED;
        }
        else {
            fs->mNesting++;
        }

        if (alarm->mType == IAlarmManager::ELAPSED_REALTIME_WAKEUP
                || alarm->mType == IAlarmManager::RTC_WAKEUP
                || alarm->mType == IAlarmManager::RTC_POWEROFF_WAKEUP) {
            bs->mNumWakeup++;
            fs->mNumWakeup++;
            if (alarm->mWorkSource != NULL && (alarm->mWorkSource->GetSize(&ival), ival) > 0) {
                for (Int32 wi = 0; wi < ival; wi++) {
                    alarm->mWorkSource->Get(wi, &ival2);
                    alarm->mWorkSource->GetName(wi, &name);
                    ActivityManagerNative::NoteWakeupAlarm(alarm->mOperation, ival2, name);
                }
            }
            else {
                ActivityManagerNative::NoteWakeupAlarm(alarm->mOperation, -1, String(NULL));
            }
            // AppOps accounting
            Int32 uid = 0;
            alarm->mOperation->GetCreatorUid(&uid);
            String pakName;
            alarm->mOperation->GetCreatorPackage(&pakName);
            Int32 res = 0;
            mAppOps->NoteOpNoThrow(IAppOpsManager::OP_ALARM_WAKEUP,
                    uid,
                    pakName,
                    &res);
        }

_EXIT_:
        if (ec == (ECode)E_PENDING_INTENT_CANCELED_EXCEPTION) {
            if (alarm->mRepeatInterval > 0) {
                // This IntentSender is no longer valid, but this
                // is a repeating alarm, so toss the hoser.
                RemoveImpl(alarm->mOperation);
            }
        }
        else if (ec == (ECode)E_RUNTIME_EXCEPTION) {
            Slogger::W(TAG, "Failure sending alarm.");
        }
    }
}

void AlarmManagerService::FiltQuickBootAlarms(
    /* [in] */ IArrayList* triggerList)
{
    AutoPtr<IArrayList> whiteList;
    CArrayList::New((IArrayList**)&whiteList);
    whiteList->Add(CoreUtils::Convert(String("android")));
    whiteList->Add(CoreUtils::Convert(String("com.android.deskclock")));

    Int32 size = 0;
    triggerList->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> p;
        triggerList->Get(i, (IInterface**)&p);
        AutoPtr<Alarm> alarm = (Alarm*)IObject::Probe(p);

        String pak;
        alarm->mOperation->GetTargetPackage(&pak);
        Boolean bContain = FALSE;
        whiteList->Contains(CoreUtils::Convert(pak), &bContain);
        if (!bContain) {
            triggerList->Remove(i);
            Slogger::V(TAG, "ignore -> %s", (const char*)pak);
        }
    }
}

void AlarmManagerService::SetWakelockWorkSource(
    /* [in] */ IPendingIntent* pi,
    /* [in] */ IWorkSource* ws,
    /* [in] */ Int32 type,
    /* [in] */ const String& tag,
    /* [in] */ Boolean first)
{
    Boolean unimportant = pi == mTimeTickSender;
    mWakeLock->SetUnimportantForLogging(unimportant);
    if (first || mLastWakeLockUnimportantForLogging) {
        mWakeLock->SetHistoryTag(tag);
    }
    else {
        mWakeLock->SetHistoryTag(String(NULL));
    }
    mLastWakeLockUnimportantForLogging = unimportant;
    if (ws != NULL) {
        mWakeLock->SetWorkSource(ws);
        return;
    }

    AutoPtr<IIIntentSender> target;
    pi->GetTarget((IIIntentSender**)&target);

    Int32 uid;
    ActivityManagerNative::GetDefault()->GetUidForIntentSender(target, &uid);
    if (uid >= 0) {
        AutoPtr<IWorkSource> ws;
        CWorkSource::New(uid, (IWorkSource**)&ws);
        mWakeLock->SetWorkSource(ws);
        return;
    }

    // Something went wrong; fall back to attributing the lock to the OS
    mWakeLock->SetWorkSource(NULL);
}

AutoPtr<AlarmManagerService::BroadcastStats> AlarmManagerService::GetStatsLocked(
    /* [in] */ IPendingIntent* pi)
{
    String pkg;
    pi->GetCreatorPackage(&pkg);
    Int32 uid;
    pi->GetCreatorUid(&uid);

    AutoPtr<IInterface> obj;
    mBroadcastStats->Get(uid, (IInterface**)&obj);
    AutoPtr<IArrayMap> uidStats = IArrayMap::Probe(obj); //ArrayMap<String, BroadcastStats>
    if (uidStats == NULL) {
        CArrayMap::New((IArrayMap**)&uidStats);
        mBroadcastStats->Put(uid, TO_IINTERFACE(uidStats));
    }

    AutoPtr<IInterface> key = (IInterface*)CoreUtils::Convert(pkg).Get();
    obj = NULL;
    uidStats->Get(key, (IInterface**)&obj);
    AutoPtr<BroadcastStats> bs = (BroadcastStats*)IObject::Probe(obj);
    if (bs == NULL) {
        bs = new BroadcastStats(uid, pkg);
        uidStats->Put(key, TO_IINTERFACE(bs));
    }
    return bs;
};

ECode AlarmManagerService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "AlarmManagerService";
    return NOERROR;
}

//====================================================================================
// native codes
//====================================================================================

static Int64 init_alarm_driver()
{
    int fd = open("/dev/alarm", O_RDWR);
    if (fd < 0) {
        ALOGV("opening alarm driver failed: %s", strerror(errno));
        return 0;
    }

    android::AlarmImpl *ret = new android::AlarmImplAlarmDriver(fd);
    return reinterpret_cast<Int64>(ret);
}

static Int64 init_timerfd()
{
    int epollfd;
    int fds[android::N_ANDROID_TIMERFDS];

    epollfd = epoll_create(android::N_ANDROID_TIMERFDS);
    if (epollfd < 0) {
        ALOGV("epoll_create(%u) failed: %s", android::N_ANDROID_TIMERFDS,
                strerror(errno));
        return 0;
    }

    for (size_t i = 0; i < android::N_ANDROID_TIMERFDS; i++) {
        fds[i] = timerfd_create(android::android_alarm_to_clockid[i], 0);
        if (fds[i] < 0) {
            ALOGV("timerfd_create(%u) failed: %s",  android::android_alarm_to_clockid[i],
                    strerror(errno));
            close(epollfd);
            for (size_t j = 0; j < i; j++) {
                close(fds[j]);
            }
            return 0;
        }
    }

    android::AlarmImpl *ret = new android::AlarmImplTimerFd(fds, epollfd);

    for (size_t i = 0; i < android::N_ANDROID_TIMERFDS; i++) {
        epoll_event event;
        event.events = EPOLLIN | EPOLLWAKEUP;
        event.data.u32 = i;

        int err = epoll_ctl(epollfd, EPOLL_CTL_ADD, fds[i], &event);
        if (err < 0) {
            ALOGV("epoll_ctl(EPOLL_CTL_ADD) failed: %s", strerror(errno));
            delete ret;
            return 0;
        }
    }

    struct itimerspec spec;
    memset(&spec, 0, sizeof(spec));
    /* 0 = disarmed; the timerfd doesn't need to be armed to get
       RTC change notifications, just set up as cancelable */

    int err = timerfd_settime(fds[ANDROID_ALARM_TYPE_COUNT],
            TFD_TIMER_ABSTIME | TFD_TIMER_CANCEL_ON_SET, &spec, NULL);
    if (err < 0) {
        ALOGV("timerfd_settime() failed: %s", strerror(errno));
        delete ret;
        return 0;
    }

    return reinterpret_cast<Int64>(ret);
}

//=========================================================================

Int64 AlarmManagerService::Native_Init()
{
    Int64 ret = init_alarm_driver();
    if (ret) {
        return ret;
    }

    return init_timerfd();
}

void AlarmManagerService::Native_Close(
    /* [in] */ Int64 nativeData)
{
    android::AlarmImpl *impl = reinterpret_cast<android::AlarmImpl *>(nativeData);
    delete impl;
}

void AlarmManagerService::Native_Set(
    /* [in] */ Int64 nativeData,
    /* [in] */ Int32 type,
    /* [in] */ Int64 seconds,
    /* [in] */ Int64 nanoseconds)
{
    android::AlarmImpl *impl = reinterpret_cast<android::AlarmImpl *>(nativeData);
    struct timespec ts;
    ts.tv_sec = seconds;
    ts.tv_nsec = nanoseconds;

    int result = impl->set(type, &ts);
    if (result < 0)
    {
        ALOGE("Unable to set alarm to %lld.%09lld: %s\n",
                seconds, nanoseconds, strerror(errno));
    }
}

void AlarmManagerService::Native_Clear(
    /* [in] */ Int64 nativeData,
    /* [in] */ Int32 type,
    /* [in] */ Int64 seconds,
    /* [in] */ Int64 nanoseconds)
{
    android::AlarmImplAlarmDriver *impl = reinterpret_cast<android::AlarmImplAlarmDriver *>(nativeData);
    struct timespec ts;
    ts.tv_sec = seconds;
    ts.tv_nsec = nanoseconds;

    int result = impl->clear(type, &ts);
    if (result < 0)
    {
        ALOGE("Unable to clear alarm  %lld.%09lld: %s\n",
                seconds, nanoseconds, strerror(errno));
    }
}

Int32 AlarmManagerService::Native_WaitForAlarm(
    /* [in] */ Int64 nativeData)
{
    android::AlarmImpl *impl = reinterpret_cast<android::AlarmImpl *>(nativeData);
    int result = 0;

    do
    {
        result = impl->waitForAlarm();
    } while (result < 0 && errno == EINTR);

    if (result < 0)
    {
        ALOGE("Unable to wait on alarm: %s\n", strerror(errno));
        return 0;
    }

    return result;
}

Int32 AlarmManagerService::Native_SetKernelTime(
    /* [in] */ Int64 nativeData,
    /* [in] */ Int64 millis)
{
    android::AlarmImpl *impl = reinterpret_cast<android::AlarmImpl *>(nativeData);
    struct timeval tv;
    int ret;

    if (millis <= 0 || millis / 1000LL >= INT_MAX) {
        return -1;
    }

    tv.tv_sec = (time_t) (millis / 1000LL);
    tv.tv_usec = (suseconds_t) ((millis % 1000LL) * 1000LL);

    ALOGD("Setting time of day to sec=%d\n", (int) tv.tv_sec);

    ret = impl->setTime(&tv);

    if (ret < 0) {
        ALOGW("Unable to set rtc to %d: %s\n", (int) tv.tv_sec, strerror(errno));
        ret = -1;
    }
    return ret;
}

Int32 AlarmManagerService::Native_SetKernelTimezone(
    /* [in] */ Int64 nativeData,
    /* [in] */ Int32 minswest)
{
    struct timezone tz;

    tz.tz_minuteswest = minswest;
    tz.tz_dsttime = 0;

    int result = settimeofday(NULL, &tz);
    if (result < 0) {
        ALOGE("Unable to set kernel timezone to %d: %s\n", minswest, strerror(errno));
        return -1;
    } else {
        ALOGD("Kernel timezone updated to %d minutes west of GMT\n", minswest);
    }

    return 0;
}

} // Server
} // Droid
} // Elastos
