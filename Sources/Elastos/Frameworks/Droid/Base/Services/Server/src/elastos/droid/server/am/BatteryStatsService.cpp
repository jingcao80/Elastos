#include "elastos/droid/server/am/BatteryStatsService.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"

#include "elastos/core/AutoLock.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Bluetooth::EIID_IBluetoothProfileServiceListener;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothAdapterHelper;
using Elastos::Droid::Bluetooth::CBluetoothAdapterHelper;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::App::EIID_IIBatteryStats;
using Elastos::Droid::Internal::Os::CBatteryStatsHelperHelper;
using Elastos::Droid::Internal::Os::CPowerProfile;
using Elastos::Droid::Internal::Os::IBatteryStatsHelperHelper;
using Elastos::Droid::Internal::Os::IPowerProfile;
using Elastos::Droid::Internal::Os::CBatteryStatsImpl;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IPowerManagerInternal;
using Elastos::Droid::Os::EIID_ILowPowerModeListener;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Server::LocalServices;
// using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

//Native method implementation begin
// base/services/core/jni/com_android_server_am_BatteryStatsService.cpp

#include <suspend/autosuspend.h>
#define LAST_RESUME_REASON "/sys/kernel/wakeup_reasons/last_resume_reason"
#define MAX_REASON_SIZE 512


static Boolean wakeup_init = FALSE;
static sem_t wakeup_sem;

static void wakeup_callback(void)
{
    //ALOGV("In wakeup_callback");
    Int32 ret = sem_post(&wakeup_sem);
    if (ret < 0) {
        char buf[80];
        strerror_r(errno, buf, sizeof(buf));
        //ALOGE("Error posting wakeup sem: %s\n", buf);
        Slogger::E("BatteryStatsService", "Error posting wakeup sem: %s\n", buf);
    }
}

static Int32 nativeWaitWakeup(
    /* [in] */ ArrayOf<Int32>* outIrqs,
    /* [in] */ ArrayOf<String>* outReasons)
{
    Boolean first_time = FALSE;

    if (outIrqs == NULL || outReasons == NULL) {
        //jniThrowException(env, "java/lang/NullPointerException", "null argument");
        Slogger::E("BatteryStatsService", "null argument");
        return -1;
    }

    // Register our wakeup callback if not yet done.
    if (!wakeup_init) {
        wakeup_init = TRUE;
        //ALOGV("Creating semaphore...");
        Int32 ret = sem_init(&wakeup_sem, 0, 0);
        if (ret < 0) {
            char buf[80];
            strerror_r(errno, buf, sizeof(buf));
            Slogger::E("BatteryStatsService", "Error creating semaphore: %s\n", buf);
            //jniThrowException(env, "java/lang/IllegalStateException", buf);
            return -1;
        }
        //ALOGV("Registering callback...");
        set_wakeup_callback(&wakeup_callback);
        // First time through, we will just drain the current wakeup reasons.
        first_time = TRUE;
    } else {
        // On following calls, we need to wait for wakeup.
        //ALOGV("Waiting for wakeup...");
        Int32 ret = sem_wait(&wakeup_sem);
        if (ret < 0) {
            char buf[80];
            strerror_r(errno, buf, sizeof(buf));
            Slogger::E("BatteryStatsService", "Error waiting on semaphore: %s\n", buf);
            // Return 0 here to let it continue looping but not return results.
            return 0;
        }
    }

    FILE *fp = fopen(LAST_RESUME_REASON, "r");
    if (fp == NULL) {
        Slogger::E("BatteryStatsService", "Failed to open %s", LAST_RESUME_REASON);
        return -1;
    }

    Int32 numOut = outIrqs->GetLength();

    //ALOGV("Reading up to %d wakeup reasons", numOut);

    char mergedreason[MAX_REASON_SIZE];
    char* mergedreasonpos = mergedreason;
    Int32 remainreasonlen = MAX_REASON_SIZE;
    Int32 firstirq = 0;
    char reasonline[128];
    Int32 i = 0;
    while (fgets(reasonline, sizeof(reasonline), fp) != NULL && i < numOut) {
        char* pos = reasonline;
        char* endPos;
        // First field is the index.
        Int32 irq = (Int32)strtol(pos, &endPos, 10);
        if (pos == endPos) {
            // Ooops.
            Slogger::E("BatteryStatsService", "Bad reason line: %s", reasonline);
            continue;
        }
        pos = endPos;
        // Skip whitespace; rest of the buffer is the reason string.
        while (*pos == ' ') {
            pos++;
        }
        // Chop newline at end.
        char* endpos = pos;
        while (*endpos != 0) {
            if (*endpos == '\n') {
                *endpos = 0;
                break;
            }
            endpos++;
        }
        // For now we are not separating out the first irq.
        // This is because in practice there are always multiple
        // lines of wakeup reasons, so it is better to just treat
        // them all together as a single string.
        if (FALSE && i == 0) {
            firstirq = irq;
        } else {
            Int32 len = snprintf(mergedreasonpos, remainreasonlen,
                    i == 0 ? "%d" : ":%d", irq);
            if (len >= 0 && len < remainreasonlen) {
                mergedreasonpos += len;
                remainreasonlen -= len;
            }
        }
        Int32 len = snprintf(mergedreasonpos, remainreasonlen, ":%s", pos);
        if (len >= 0 && len < remainreasonlen) {
            mergedreasonpos += len;
            remainreasonlen -= len;
        }
        // For now it is better to combine all of these in to one entry in the
        // battery history.  In the future, it might be nice to figure out a way
        // to efficiently store multiple lines as a single entry in the history.
        //irqs[i] = irq;
        //ScopedLocalRef<jstring> reasonString(env, env->NewStringUTF(pos));
        //env->SetObjectArrayElement(outReasons, i, reasonString.get());
        //ALOGV("Wakeup reason #%d: irw %d reason %s", i, irq, pos);
        i++;
    }

    //ALOGV("Got %d reasons", i);
    if (first_time) {
        i = 0;
    }
    if (i > 0) {
        (*outIrqs)[0] = firstirq;
        *mergedreasonpos = 0;
        //ScopedLocalRef<jstring> reasonString(env, env->NewStringUTF(mergedreason));
        //env->SetObjectArrayElement(outReasons, 0, reasonString.get());
        String reasonString(mergedreason);
        outReasons->Set(0, reasonString);
        i = 1;
    }

    if (fclose(fp) != 0) {
        Slogger::E("BatteryStatsService", "Failed to close %s", LAST_RESUME_REASON);
        return -1;
    }

    return first_time ? 0 : i;
}

//====================================================================================

CAR_INTERFACE_IMPL(BatteryStatsService::BluetoothProfileServiceListener, Object, IBluetoothProfileServiceListener)

ECode BatteryStatsService::BluetoothProfileServiceListener::OnServiceConnected(
    /* [in] */ Int32 profile,
    /* [in] */ IBluetoothProfile* proxy)
{
    mHost->mBluetoothHeadset = IBluetoothHeadset::Probe(proxy);
    AutoLock lock(mHost->mStats);
    if (mHost->mBluetoothPendingStats && mHost->mStats) {
        mHost->mStats->NoteBluetoothOnLocked();
        mHost->mStats->SetBtHeadset(mHost->mBluetoothHeadset);
        mHost->mBluetoothPendingStats = FALSE;
    }
    return NOERROR;
}

ECode BatteryStatsService::BluetoothProfileServiceListener::OnServiceDisconnected(
    /* [in] */ Int32 profile)
{
    mHost->mBluetoothHeadset = NULL;
    return NOERROR;
}

BatteryStatsService::WakeupReasonThread::WakeupReasonThread(
    /* [in] */ BatteryStatsService* owner)
{
    mHost = owner;
    mIrqs = ArrayOf<Int32>::Alloc(32);
    mReasons = ArrayOf<String>::Alloc(32);
    Thread::constructor(String("BatteryStats_wakeupReason"));
}

ECode BatteryStatsService::WakeupReasonThread::Run()
{
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_FOREGROUND);

    //try {
    if (mHost->mStats) {
        Int32 num;
        while ((num = BatteryStatsService::NativeWaitWakeup(mIrqs, mReasons)) >= 0) {
            {
                AutoLock lock(mHost->mStats);
                if (num > 0) {
                    for (Int32 i=0; i<num; i++) {
                        mHost->mStats->NoteWakeupReasonLocked((*mReasons)[i]);
                    }
                }
                else {
                    mHost->mStats->NoteWakeupReasonLocked(String("unknown"));
                }
            }
        }
    }
    //} catch (RuntimeException e) {
    //    Slog.e(TAG, "Failure reading wakeup reasons", e);
    //}
    return NOERROR;
}

const String BatteryStatsService::TAG("BatteryStatsService");
AutoPtr<IIBatteryStats> BatteryStatsService::sService;

CAR_INTERFACE_IMPL_3(BatteryStatsService, Object, IIBatteryStats, ILowPowerModeListener, IBinder)

BatteryStatsService::BatteryStatsService()
    : mBluetoothPendingStats(FALSE)
{
}

ECode BatteryStatsService::constructor(
    /* [in] */ IFile* systemDir,
    /* [in] */ IHandler* handler)
{
    mBluetoothProfileServiceListener = new BluetoothProfileServiceListener(this);
    CBatteryStatsImpl::New(systemDir, handler, (IBatteryStatsImpl**)&mStats);
    Slogger::W(TAG, " === TODO constructor: BatteryStatsImpl %p.", mStats.Get());
    return NOERROR;
}

ECode BatteryStatsService::Publish(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    sm->AddService(IBatteryStats::SERVICE_NAME, (IIBatteryStats*)this);
    Int32 steps;
    AutoPtr<IPowerProfile> pp;
    CPowerProfile::New(mContext, (IPowerProfile**)&pp);
    pp->GetNumSpeedSteps(&steps);
    mStats->SetNumSpeedSteps(steps);
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Int32 value;
    resources->GetInteger(
            R::integer::config_radioScanningTimeout, &value);
    mStats->SetRadioScanningTimeout(value * 1000L);
    return NOERROR;
}

void BatteryStatsService::InitPowerManagement()
{
    AutoPtr<IInterface> service = LocalServices::GetService(EIID_IPowerManagerInternal);
    mPowerManagerInternal = IPowerManagerInternal::Probe(service);
    mPowerManagerInternal->RegisterLowPowerModeObserver(this);
    Boolean enabled = FALSE;
    mPowerManagerInternal->GetLowPowerModeEnabled(&enabled);
    mStats->NoteLowPowerMode(enabled);
    AutoPtr<WakeupReasonThread> wrt = new WakeupReasonThread(this);
    wrt->Start();
}

void BatteryStatsService::Shutdown()
{
    Slogger::W("BatteryStats", "Writing battery stats before shutdown...");
    AutoLock lock(mStats);
    mStats->ShutdownLocked();
}

AutoPtr<IIBatteryStats> BatteryStatsService::GetService()
{
    if (sService != NULL) {
        return sService;
    }
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> batteryService;
    sm->GetService(IBatteryStats::SERVICE_NAME, (IInterface**)&batteryService);
    sService = IIBatteryStats::Probe(batteryService);
    return sService;
}

ECode BatteryStatsService::OnLowPowerModeChanged(
    /* [in] */ Boolean enabled)
{
    AutoLock lock(mStats);
    mStats->NoteLowPowerMode(enabled);
    return NOERROR;
}

AutoPtr<IBatteryStatsImpl> BatteryStatsService::GetActiveStatistics()
{
    return mStats;
}

void BatteryStatsService::AddIsolatedUid(
    /* [in] */ Int32 isolatedUid,
    /* [in] */ Int32 appUid)
{
    AutoLock lock(mStats);
    mStats->AddIsolatedUidLocked(isolatedUid, appUid);
}

void BatteryStatsService::RemoveIsolatedUid(
    /* [in] */ Int32 isolatedUid,
    /* [in] */ Int32 appUid)
{
    AutoLock lock(mStats);
    mStats->RemoveIsolatedUidLocked(isolatedUid, appUid);
}

void BatteryStatsService::NoteProcessStart(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    AutoLock lock(mStats);
    mStats->NoteProcessStartLocked(name, uid);
}

void BatteryStatsService::NoteProcessState(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 state)
{
    AutoLock lock(mStats);
    mStats->NoteProcessStateLocked(name, uid, state);
}

void BatteryStatsService::NoteProcessFinish(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    AutoLock lock(mStats);
    mStats->NoteProcessFinishLocked(name, uid);
}

ECode BatteryStatsService::GetStatistics(
    /* [out, callee] */ ArrayOf<Byte>** statistics)
{
    VALIDATE_NOT_NULL(statistics)
    FAIL_RETURN(mContext->EnforceCallingPermission(Elastos::Droid::Manifest::permission::BATTERY_STATS, String(NULL)));
    //Slog.i("foo", "SENDING BATTERY INFO:");
    //mStats.dumpLocked(new LogPrinter(Log.INFO, "foo", Log.LOG_ID_SYSTEM));
    AutoPtr<IParcel> out;
    CParcel::New((IParcel**)&out);
    IParcelable::Probe(mStats)->WriteToParcel(out);
    AutoPtr< ArrayOf<Byte> > data;
    out->Marshall((ArrayOf<Byte>**)&data);
    *statistics = data;
    REFCOUNT_ADD(*statistics);
    return NOERROR;
}

ECode BatteryStatsService::GetStatisticsStream(
    /* [out] */ IParcelFileDescriptor** pfd)
{
    VALIDATE_NOT_NULL(pfd);
    *pfd = NULL;
    FAIL_RETURN(mContext->EnforceCallingPermission(Elastos::Droid::Manifest::permission::BATTERY_STATS, String(NULL)));
    //Slog.i("foo", "SENDING BATTERY INFO:");
    //mStats.dumpLocked(new LogPrinter(Log.INFO, "foo", Log.LOG_ID_SYSTEM));
    AutoPtr<IParcel> out;
    CParcel::New((IParcel**)&out);
    IParcelable::Probe(mStats)->WriteToParcel(out);
    AutoPtr< ArrayOf<Byte> > data;
    out->Marshall((ArrayOf<Byte>**)&data);
    //try {
    AutoPtr<IParcelFileDescriptorHelper> pfdHelper;
    CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&pfdHelper);
    pfdHelper->FromData(data, String("battery-stats"), pfd);
    //} catch (IOException e) {
    //    Slog.w(TAG, "Unable to create shared memory", e);
    //    return null;
    //}
    return NOERROR;
}

ECode BatteryStatsService::ComputeBatteryTimeRemaining(
    /* [out] */ Int64* timeRemaining)
{
    VALIDATE_NOT_NULL(timeRemaining);
    {
        AutoLock lock(mStats);
        Int64 time = 0;
        Int64 realTime = SystemClock::GetElapsedRealtime();
        IBatteryStats::Probe(mStats)->ComputeBatteryTimeRemaining(realTime, &time);
        *timeRemaining = time >= 0 ? (time / 1000) : time;
    }
    return NOERROR;
}

ECode BatteryStatsService::ComputeChargeTimeRemaining(
    /* [out] */ Int64* timeRemaining)
{
    VALIDATE_NOT_NULL(timeRemaining);
    {
        AutoLock lock(mStats);
        Int64 time = 0;
        Int64 realTime = SystemClock::GetElapsedRealtime();
        IBatteryStats::Probe(mStats)->ComputeChargeTimeRemaining(realTime, &time);
        *timeRemaining = time >= 0 ? (time / 1000) : time;
    }
    return NOERROR;
}

ECode BatteryStatsService::NoteEvent(
    /* [in] */ Int32 code,
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteEventLocked(code, name, uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteSyncStart(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteSyncStartLocked(name, uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteSyncFinish(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteSyncFinishLocked(name, uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteJobStart(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteJobStartLocked(name, uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteJobFinish(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteJobFinishLocked(name, uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteStartWakelock(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ const String& historyName,
    /* [in] */ Int32 type,
    /* [in] */ Boolean unimportantForLogging)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    Int64 realTime = SystemClock::GetElapsedRealtime();
    Int64 upTime = SystemClock::GetUptimeMillis();
    mStats->NoteStartWakeLocked(uid, pid, name, historyName, type, unimportantForLogging, realTime, upTime);
    return NOERROR;
}

ECode BatteryStatsService::NoteStopWakelock(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ const String& historyName,
    /* [in] */ Int32 type)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    Int64 realTime = SystemClock::GetElapsedRealtime();
    Int64 upTime = SystemClock::GetUptimeMillis();
    mStats->NoteStopWakeLocked(uid, pid, name, historyName, type, realTime, upTime);
    return NOERROR;
}

ECode BatteryStatsService::NoteStartWakelockFromSource(
    /* [in] */ IWorkSource* ws,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ const String& historyName,
    /* [in] */ Int32 type,
    /* [in] */ Boolean unimportantForLogging)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteStartWakeFromSourceLocked(ws, pid, name, historyName, type, unimportantForLogging);
    return NOERROR;
}

ECode BatteryStatsService::NoteChangeWakelockFromSource(
    /* [in] */ IWorkSource* ws,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ const String& historyName,
    /* [in] */ Int32 type,
    /* [in] */ IWorkSource* newWs,
    /* [in] */ Int32 newPid,
    /* [in] */ const String& newName,
    /* [in] */ const String& newHistoryName,
    /* [in] */ Int32 newType,
    /* [in] */ Boolean newUnimportantForLogging)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteChangeWakelockFromSourceLocked(ws, pid, name, historyName, type,
            newWs, newPid, newName, newHistoryName, newType, newUnimportantForLogging);
    return NOERROR;
}


ECode BatteryStatsService::NoteStopWakelockFromSource(
    /* [in] */ IWorkSource* ws,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ const String& historyName,
    /* [in] */ Int32 type)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteStopWakeFromSourceLocked(ws, pid, name, historyName, type);
    return NOERROR;
}

ECode BatteryStatsService::NoteStartSensor(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 sensor)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteStartSensorLocked(uid, sensor);
    return NOERROR;
}

ECode BatteryStatsService::NoteStopSensor(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 sensor)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteStopSensorLocked(uid, sensor);
    return NOERROR;
}

ECode BatteryStatsService::NoteVibratorOn(
    /* [in] */ Int32 uid,
    /* [in] */ Int64 durationMillis)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteVibratorOnLocked(uid, durationMillis);
    return NOERROR;
}

ECode BatteryStatsService::NoteVibratorOff(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteVibratorOffLocked(uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteStartGps(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteStartGpsLocked(uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteStopGps(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteStopGpsLocked(uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteScreenState(
    /* [in] */ Int32 state)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteScreenStateLocked(state);
    return NOERROR;
}

ECode BatteryStatsService::NoteScreenBrightness(
    /* [in] */ Int32 brightness)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteScreenBrightnessLocked(brightness);
    return NOERROR;
}

ECode BatteryStatsService::NoteUserActivity(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 event)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteUserActivityLocked(uid, event);
    return NOERROR;
}

ECode BatteryStatsService::NoteInteractive(
    /* [in] */ Boolean interactive)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteInteractiveLocked(interactive);
    return NOERROR;
}

ECode BatteryStatsService::NoteMobileRadioPowerState(
    /* [in] */ Int32 powerState,
    /* [in] */ Int64 timestampNs)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteMobileRadioPowerState(powerState, timestampNs);
    return NOERROR;
}

ECode BatteryStatsService::NotePhoneOn()
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NotePhoneOnLocked();
    return NOERROR;
}

ECode BatteryStatsService::NotePhoneOff()
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NotePhoneOffLocked();
    return NOERROR;
}

ECode BatteryStatsService::NotePhoneSignalStrength(
   /* [in] */ ISignalStrength* signalStrength)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NotePhoneSignalStrengthLocked(signalStrength);
    return NOERROR;
}

ECode BatteryStatsService::NotePhoneDataConnectionState(
    /* [in] */ Int32 dataType,
    /* [in] */ Boolean hasData)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NotePhoneDataConnectionStateLocked(dataType, hasData);
    return NOERROR;
}

ECode BatteryStatsService::NotePhoneState(
    /* [in] */ Int32 state)
{
    EnforceCallingPermission();
    AutoPtr<ITelephonyManagerHelper> helper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper);
    AutoPtr<ITelephonyManager> tm;
    helper->GetDefault((ITelephonyManager**)&tm);
    Int32 simState;
    tm->GetSimState(&simState);
    AutoLock lock(mStats);
    mStats->NotePhoneStateLocked(state, simState);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiOn()
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiOnLocked();
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiOff()
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiOffLocked();
    return NOERROR;
}

ECode BatteryStatsService::NoteStartAudio(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteAudioOnLocked(uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteStopAudio(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteAudioOffLocked(uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteStartVideo(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteVideoOnLocked(uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteStopVideo(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteVideoOffLocked(uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteResetAudio()
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteResetAudioLocked();
    return NOERROR;
}

ECode BatteryStatsService::NoteResetVideo()
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteResetVideoLocked();
    return NOERROR;
}

ECode BatteryStatsService::NoteFlashlightOn()
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteFlashlightOnLocked();
    return NOERROR;
}

ECode BatteryStatsService::NoteFlashlightOff()
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteFlashlightOffLocked();
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiRunning(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiRunningLocked(ws);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiRunningChanged(
    /* [in] */ IWorkSource* oldWs,
    /* [in] */ IWorkSource* newWs)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiRunningChangedLocked(oldWs, newWs);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiStopped(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiStoppedLocked(ws);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiState(
    /* [in] */ Int32 wifiState,
    /* [in] */ const String& accessPoint)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiStateLocked(wifiState, accessPoint);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiSupplicantStateChanged(
    /* [in] */ Int32 supplState,
    /* [in] */ Boolean failedAuth)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiSupplicantStateChangedLocked(supplState, failedAuth);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiRssiChanged(
    /* [in] */ Int32 newRssi)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiRssiChangedLocked(newRssi);
    return NOERROR;
}

ECode BatteryStatsService::NoteBluetoothOn()
{
    EnforceCallingPermission();
    AutoPtr<IBluetoothAdapterHelper> baHelper;
    CBluetoothAdapterHelper::AcquireSingleton((IBluetoothAdapterHelper**)&baHelper);
    AutoPtr<IBluetoothAdapter> adapter;
    baHelper->GetDefaultAdapter((IBluetoothAdapter**)&adapter);
    if (adapter != NULL) {
        Boolean result;
        adapter->GetProfileProxy(mContext, mBluetoothProfileServiceListener, IBluetoothProfile::HEADSET, &result);
    }

    AutoLock lock(mStats);
    if (mBluetoothHeadset != NULL) {
        mStats->NoteBluetoothOnLocked();
        mStats->SetBtHeadset(mBluetoothHeadset);
    }
    else {
        mBluetoothPendingStats = TRUE;
    }
    return NOERROR;
}

ECode BatteryStatsService::NoteBluetoothOff()
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mBluetoothPendingStats = FALSE;
    mStats->NoteBluetoothOffLocked();
    return NOERROR;
}

ECode BatteryStatsService::NoteBluetoothState(
    /* [in] */ Int32 bluetoothState)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteBluetoothStateLocked(bluetoothState);
    return NOERROR;
}

ECode BatteryStatsService::NoteFullWifiLockAcquired(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteFullWifiLockAcquiredLocked(uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteFullWifiLockReleased(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteFullWifiLockReleasedLocked(uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiScanStarted(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiScanStartedLocked(uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiScanStopped(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiScanStoppedLocked(uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiMulticastEnabled(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiMulticastEnabledLocked(uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiMulticastDisabled(
    /* [in] */ Int32 uid)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiMulticastDisabledLocked(uid);
    return NOERROR;
}

ECode BatteryStatsService::NoteFullWifiLockAcquiredFromSource(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteFullWifiLockAcquiredFromSourceLocked(ws);
    return NOERROR;
}

ECode BatteryStatsService::NoteFullWifiLockReleasedFromSource(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteFullWifiLockReleasedFromSourceLocked(ws);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiScanStartedFromSource(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiScanStartedFromSourceLocked(ws);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiScanStoppedFromSource(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiScanStoppedFromSourceLocked(ws);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiBatchedScanStartedFromSource(
    /* [in] */ IWorkSource* ws,
    /* [in] */ Int32 csph)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiBatchedScanStartedFromSourceLocked(ws, csph);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiBatchedScanStoppedFromSource(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiBatchedScanStoppedFromSourceLocked(ws);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiMulticastEnabledFromSource(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiMulticastEnabledFromSourceLocked(ws);
    return NOERROR;
}

ECode BatteryStatsService::NoteWifiMulticastDisabledFromSource(
    /* [in] */ IWorkSource* ws)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteWifiMulticastDisabledFromSourceLocked(ws);
    return NOERROR;
}

ECode BatteryStatsService::NoteNetworkInterfaceType(
    /* [in] */ const String& iface,
    /* [in] */ Int32 type)
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteNetworkInterfaceTypeLocked(iface, type);
    return NOERROR;
}

ECode BatteryStatsService::NoteNetworkStatsEnabled()
{
    EnforceCallingPermission();
    AutoLock lock(mStats);
    mStats->NoteNetworkStatsEnabledLocked();
    return NOERROR;
}

Boolean BatteryStatsService::IsOnBattery()
{
    Boolean result = FALSE;
    mStats->IsOnBattery(&result);
    return result;
}

ECode BatteryStatsService::SetBatteryState(
    /* [in] */ Int32 status,
    /* [in] */ Int32 health,
    /* [in] */ Int32 plugType,
    /* [in] */ Int32 level,
    /* [in] */ Int32 temp,
    /* [in] */ Int32 volt)
{
    EnforceCallingPermission();
    return mStats->SetBatteryState(status, health, plugType, level, temp, volt);
}

ECode BatteryStatsService::GetAwakeTimeBattery(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = 0;
    FAIL_RETURN( mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::BATTERY_STATS, String(NULL)) );
    return mStats->GetAwakeTimeBattery(time);
}

ECode BatteryStatsService::GetAwakeTimePlugged(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = 0;
    FAIL_RETURN( mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::BATTERY_STATS, String(NULL)) );
    return mStats->GetAwakeTimePlugged(time);
}

void BatteryStatsService::EnforceCallingPermission()
{
    if (Binder::GetCallingPid() == Process::MyPid()) return;
    mContext->EnforcePermission(Elastos::Droid::Manifest::permission::UPDATE_DEVICE_STATS,
            Binder::GetCallingPid(), Binder::GetCallingUid(), String(NULL));
}

Int32 BatteryStatsService::NativeWaitWakeup(
    /* [in] */ ArrayOf<Int32>* outIrqs,
    /* [in] */ ArrayOf<String>* outReasons)
{
    //TODO
    return nativeWaitWakeup(outIrqs, outReasons);
}

void BatteryStatsService::DumpHelp(
    /* [in] */ IPrintWriter* pw)
{
    pw->Println(String("Battery stats (batterystats) dump options:"));
    pw->Println(String("  [--checkin] [--history] [--history-start] [--unplugged] [--charged] [-c]"));
    pw->Println(String("  [--reset] [--write] [-h] [<package.name>]"));
    pw->Println(String("  --checkin: format output for a checkin report."));
    pw->Println(String("  --history: show only history data."));
    pw->Println(String("  --history-start <num>: show only history data starting at given time offset."));
    pw->Println(String("  --unplugged: only output data since last unplugged."));
    pw->Println(String("  --charged: only output data since last charged."));
    pw->Println(String("  --reset: reset the stats, clearing all current data."));
    pw->Println(String("  --write: force write current collected stats to disk."));
    pw->Println(String("  <package.name>: optional name of package to filter output by."));
    pw->Println(String("  -h: print this help text."));
    pw->Println(String("Battery stats (batterystats) commands:"));
    pw->Println(String("  enable|disable <option>"));
    pw->Println(String("    Enable or disable a running option.  Option state is not saved across boots."));
    pw->Println(String("    Options are:"));
    pw->Println(String("      full-history: include additional detailed events in battery history:"));
    pw->Println(String("          wake_lock_in and proc events"));
    pw->Println(String("      no-auto-reset: don't automatically reset stats when unplugged"));
}

Int32 BatteryStatsService::DoEnableOrDisable(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 i,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Boolean enable)
{
    i++;
    if (i >= args->GetLength()) {
        pw->Println(String("Missing option argument for ") + (enable ? String("--enable") : String("--disable")));
        DumpHelp(pw);
        return -1;
    }
    if (String("full-wake-history").Equals((*args)[i]) || String("full-history").Equals((*args)[i])) {
        AutoLock lock(mStats);
        mStats->SetRecordAllHistoryLocked(enable);
    }
    else if (String("no-auto-reset").Equals((*args)[i])) {
        AutoLock lock(mStats);
        mStats->SetNoAutoReset(enable);
    }
    else {
        pw->Println(String("Unknown enable/disable option: ") + (*args)[i]);
        DumpHelp(pw);
        return -1;
    }
    return i;
}

ECode BatteryStatsService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Int32 permission;
    if (mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, &permission),
            permission != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder sb("Permission Denial: can't dump BatteryStats from from pid=");
        sb += Binder::GetCallingPid();
        sb += ", uid=";
        sb += Binder::GetCallingUid();
        sb += " without permission ";
        sb += Elastos::Droid::Manifest::permission::DUMP;
        pw->Print(sb.ToString());
        return NOERROR;
    }

    Int32 flags = 0;
    Boolean useCheckinFormat = FALSE;
    Boolean isRealCheckin = FALSE;
    Boolean noOutput = FALSE;
    Boolean writeData = FALSE;
    Int64 historyStart = -1;
    Int32 reqUid = -1;
    if (args != NULL) {
        for (Int32 i = 0; i < args->GetLength(); ++i) {
            String arg = (*args)[i];
            if (arg.Equals("--checkin")) {
                useCheckinFormat = TRUE;
                isRealCheckin = TRUE;
            }
            else if (String("--history").Equals(arg)) {
                flags |= IBatteryStats::DUMP_HISTORY_ONLY;
            }
            else if (String("--history-start").Equals(arg)) {
                flags |= IBatteryStats::DUMP_HISTORY_ONLY;
                i++;
                if (i >= args->GetLength()) {
                    pw->Println(String("Missing time argument for --history-since"));
                    DumpHelp(pw);
                    return NOERROR;
                }
                historyStart = StringUtils::ParseInt64((*args)[i]);
                writeData = TRUE;
            }
            else if (String("-c").Equals(arg)) {
                useCheckinFormat = TRUE;
                flags |= IBatteryStats::DUMP_INCLUDE_HISTORY;
            }
            else if (String("--unplugged").Equals(arg)) {
                flags |= IBatteryStats::DUMP_UNPLUGGED_ONLY;
            }
            else if (String("--charged").Equals(arg)) {
                flags |= IBatteryStats::DUMP_CHARGED_ONLY;
            }
            else if (arg.Equals(String("--reset"))) {
                AutoLock lock(mStats);
                mStats->ResetAllStatsCmdLocked();
                pw->Print(String("Battery stats reset."));
                noOutput = TRUE;
            }
            else if (arg.Equals("--write")) {
                AutoLock lock(mStats);
                mStats->WriteSyncLocked();
                pw->Print(String("Battery stats written."));
                noOutput = TRUE;
            }
            else if (String("--enable").Equals(arg) || String("enable").Equals(arg)) {
                i = DoEnableOrDisable(pw, i, args, TRUE);
                if (i < 0) {
                    return NOERROR;
                }
                pw->Println(String("Enabled: ") + (*args)[i]);
                return NOERROR;
            }
            else if (String("--disable").Equals(arg) || String("disable").Equals(arg)) {
                i = DoEnableOrDisable(pw, i, args, FALSE);
                if (i < 0) {
                    return NOERROR;
                }
                pw->Println(String("Disabled: ") + (*args)[i]);
                return NOERROR;
            }
            else if (arg.Equals("-h")) {
                DumpHelp(pw);
                return NOERROR;
            }
            else if (arg.Equals("-a")) {
                flags |= IBatteryStats::DUMP_VERBOSE;
            }
            else if (arg.GetLength() > 0 && arg.GetChar(0) == '-'){
                pw->Println(String("Unknown option: ") + arg);
                DumpHelp(pw);
                return NOERROR;
            }
            else {
                // Not an option, last argument must be a package name.
                //try {
                AutoPtr<IPackageManager> pm;
                mContext->GetPackageManager((IPackageManager**)&pm);
                //reqUid = mContext.getPackageManager().getPackageUid(arg,
                //            UserHandle.getCallingUserId());
                AutoPtr<IUserHandleHelper> userHandleHelper;
                CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
                Int32 userId;
                userHandleHelper->GetCallingUserId(&userId);
                ECode ec = pm->GetPackageUid(arg, userId, &reqUid);
                //} catch (PackageManager.NameNotFoundException e) {
                if (FAILED(ec)) {
                    pw->Print(String("Unknown option: ") + arg);
                    DumpHelp(pw);
                    return NOERROR;
                }
                //}
            }
        }
    }
    if (noOutput) {
        return NOERROR;
    }

    AutoPtr<IBatteryStatsHelperHelper> bshh;
    CBatteryStatsHelperHelper::AcquireSingleton((IBatteryStatsHelperHelper**)&bshh);
    Boolean wifiOnly = FALSE;
    if (bshh->CheckWifiOnly(mContext, &wifiOnly), wifiOnly) {
        flags |= IBatteryStats::DUMP_DEVICE_WIFI_ONLY;
    }
    if (reqUid >= 0) {
        // By default, if the caller is only interested in a specific package, then
        // we only dump the aggregated data since charged.
        if ((flags&(IBatteryStats::DUMP_HISTORY_ONLY|IBatteryStats::DUMP_UNPLUGGED_ONLY
                        |IBatteryStats::DUMP_CHARGED_ONLY)) == 0) {
            flags |= IBatteryStats::DUMP_CHARGED_ONLY;
            // Also if they are doing -c, we don't want history.
            flags &= ~IBatteryStats::DUMP_INCLUDE_HISTORY;
        }
    }
    if (useCheckinFormat) {
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        //List<ApplicationInfo> apps = mContext.getPackageManager().getInstalledApplications(0);
        AutoPtr<IList> apps;
        pm->GetInstalledApplications(0, (IList**)&apps);
        if (isRealCheckin) {
            // For a real checkin, first we want to prefer to use the last complete checkin
            // file if there is one.
            // synchronized (mStats.mCheckinFile) {
            //     if (mStats.mCheckinFile.exists()) {
            //         try {
            //             byte[] raw = mStats.mCheckinFile.readFully();
            //             if (raw != null) {
            //                 Parcel in = Parcel.obtain();
            //                 in.unmarshall(raw, 0, raw.length);
            //                 in.setDataPosition(0);
            //                 BatteryStatsImpl checkinStats = new BatteryStatsImpl(
            //                         null, mStats.mHandler);
            //                 checkinStats.readSummaryFromParcel(in);
            //                 in.recycle();
            //                 checkinStats.dumpCheckinLocked(mContext, pw, apps, flags,
            //                         historyStart);
            //                 mStats.mCheckinFile.delete();
            //                 return;
            //             }
            //         } catch (IOException e) {
            //             Slog.w(TAG, "Failure reading checkin file "
            //                     + mStats.mCheckinFile.getBaseFile(), e);
            //         }
            //     }
            // }
        }
        {
            AutoLock lock(mStats);
            //TODO mStats->DumpCheckinLocked(mContext, pw, apps, flags, historyStart);
            if (writeData) {
                //mStats->WriteAsyncLocked();
            }
        }
    }
    else {
        AutoLock lock(mStats);
        //mStats->DumpLocked(mContext, pw, flags, reqUid, historyStart);
        if (writeData) {
            //mStats->WriteAsyncLocked();
        }
    }
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
