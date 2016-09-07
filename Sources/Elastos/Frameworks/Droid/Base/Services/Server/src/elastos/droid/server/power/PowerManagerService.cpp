
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Telephony.h>
#include "elastos/droid/server/power/PowerManagerService.h"
#include "elastos/droid/server/power/CPowerManagerServiceBinderService.h"
#include "elastos/droid/server/power/ShutdownThread.h"
#include "elastos/droid/server/am/BatteryStatsService.h"
#include "elastos/droid/server/Watchdog.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/Manifest.h"
#include <elastos/droid/utility/TimeUtils.h>
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/Objects.h>
#include <fcntl.h>
#include <hardware/power.h>
#include <hardware_legacy/power.h>
#include <powermanager/PowerManager.h>
#include <suspend/autosuspend.h>
#include <utils/Log.h>
#include <utils/Errors.h>

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerRunningAppProcessInfo;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Hardware::EIID_ISensorEventListener;
using Elastos::Droid::Hardware::Display::CDisplayPowerRequest;
using Elastos::Droid::Hardware::Display::EIID_IDisplayPowerCallbacks;
using Elastos::Droid::Hardware::Display::EIID_IDisplayManagerInternal;
using Elastos::Droid::Hardware::ISystemSensorManager;
using Elastos::Droid::Hardware::CSystemSensorManager;
using Elastos::Droid::Internal::Os::CBackgroundThreadHelper;
using Elastos::Droid::Internal::Os::IBackgroundThreadHelper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Os::CPowerManagerHelper;
using Elastos::Droid::Os::IPowerManagerHelper;
using Elastos::Droid::Os::EIID_IBatteryManagerInternal;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IIPowerManager;
using Elastos::Droid::Os::EIID_IPowerManagerInternal;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Server::Am::BatteryStatsService;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Server::Lights::EIID_ILightsManager;
using Elastos::Droid::Service::Dreams::EIID_IDreamManagerInternal;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::EIID_IWindowManagerPolicy;
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::Thread;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Objects;
using Elastos::Utility::IList;
using android::LogIfSlow;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Power {

static AutoPtr<PowerManagerService> sPowerManagerService;
static struct power_module* sPowerModule = NULL;

static nsecs_t sLastEventTime[android::USER_ACTIVITY_EVENT_LAST + 1];

// Throttling interval for user activity calls.
static const nsecs_t MIN_TIME_BETWEEN_USERACTIVITIES = 500 * 1000000LL; // 500ms

const String PowerManagerService::TAG("PowerManagerService");
const Boolean PowerManagerService::DEBUG = FALSE;
const Boolean PowerManagerService::DEBUG_SPEW = DEBUG || FALSE;
const Int32 PowerManagerService::MSG_USER_ACTIVITY_TIMEOUT = 1;
const Int32 PowerManagerService::MSG_SANDMAN = 2;
const Int32 PowerManagerService::MSG_WAKE_UP = 5;
const Int32 PowerManagerService::DIRTY_WAKE_LOCKS = 1 << 0;
const Int32 PowerManagerService::DIRTY_WAKEFULNESS = 1 << 1;
const Int32 PowerManagerService::DIRTY_USER_ACTIVITY = 1 << 2;
const Int32 PowerManagerService::DIRTY_ACTUAL_DISPLAY_POWER_STATE_UPDATED = 1 << 3;
const Int32 PowerManagerService::DIRTY_BOOT_COMPLETED = 1 << 4;
const Int32 PowerManagerService::DIRTY_SETTINGS = 1 << 5;
const Int32 PowerManagerService::DIRTY_IS_POWERED = 1 << 6;
const Int32 PowerManagerService::DIRTY_STAY_ON = 1 << 7;
const Int32 PowerManagerService::DIRTY_BATTERY_STATE = 1 << 8;
const Int32 PowerManagerService::DIRTY_PROXIMITY_POSITIVE = 1 << 9;
const Int32 PowerManagerService::DIRTY_DOCK_STATE = 1 << 10;
const Int32 PowerManagerService::WAKEFULNESS_ASLEEP = 0;
const Int32 PowerManagerService::WAKEFULNESS_AWAKE = 1;
const Int32 PowerManagerService::WAKEFULNESS_DREAMING = 2;
const Int32 PowerManagerService::WAKEFULNESS_DOZING = 3;
const Int32 PowerManagerService::WAKE_LOCK_CPU = 1 << 0;
const Int32 PowerManagerService::WAKE_LOCK_SCREEN_BRIGHT = 1 << 1;
const Int32 PowerManagerService::WAKE_LOCK_SCREEN_DIM = 1 << 2;
const Int32 PowerManagerService::WAKE_LOCK_BUTTON_BRIGHT = 1 << 3;
const Int32 PowerManagerService::WAKE_LOCK_PROXIMITY_SCREEN_OFF = 1 << 4;
const Int32 PowerManagerService::WAKE_LOCK_STAY_AWAKE = 1 << 5;
const Int32 PowerManagerService::WAKE_LOCK_DOZE = 1 << 6;
const Int32 PowerManagerService::USER_ACTIVITY_SCREEN_BRIGHT = 1 << 0;
const Int32 PowerManagerService::USER_ACTIVITY_SCREEN_DIM = 1 << 1;
const Int32 PowerManagerService::USER_ACTIVITY_SCREEN_DREAM = 1 << 2;
const Int32 PowerManagerService::DEFAULT_SCREEN_OFF_TIMEOUT = 15 * 1000;
const Int32 PowerManagerService::DEFAULT_SLEEP_TIMEOUT = -1;
const Int32 PowerManagerService::POWER_HINT_INTERACTION = 2;
const Int32 PowerManagerService::POWER_HINT_LOW_POWER = 5;
const Int32 PowerManagerService::DEFAULT_BUTTON_ON_DURATION = 5 * 1000;
const Int32 PowerManagerService::DPM_CONFIG_FEATURE_MASK_NSRM = 0x00000004;
const Int32 PowerManagerService::MAX_CPU_BOOST_TIME = 5000000;
const Float PowerManagerService::PROXIMITY_NEAR_THRESHOLD = 5.0;


//==============================================================================
//          PowerManagerService::BinderService::WakeUpRunnable
//==============================================================================

ECode PowerManagerService::BinderService::WakeUpRunnable::Run()
{
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mHost->WakeUpInternal(mEventTime, mUid);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}


//==============================================================================
//          PowerManagerService::BinderService::SensorEventListener
//==============================================================================

CAR_INTERFACE_IMPL(PowerManagerService::BinderService::SensorEventListener, Object, ISensorEventListener)

ECode PowerManagerService::BinderService::SensorEventListener::OnSensorChanged(
    /* [in] */ ISensorEvent* event)
{
    mHost->CleanupProximity();
    Boolean hasMessages;
    if (mHost->mHandler->HasMessages(MSG_WAKE_UP, &hasMessages), !hasMessages) {
        Slogger::W(TAG, "The proximity sensor took too long, wake event already triggered!");
        return NOERROR;
    }
    mHost->mHandler->RemoveMessages(MSG_WAKE_UP);
    AutoPtr<ArrayOf<Float> > values;
    event->GetValues((ArrayOf<Float>**)&values);
    Float distance = (*values)[0];
    Float maxRange;
    if (distance >= PROXIMITY_NEAR_THRESHOLD ||
            (mHost->mProximitySensor->GetMaximumRange(&maxRange), distance >= maxRange)) {
        mR->Run();
    }
    return NOERROR;
}

ECode PowerManagerService::BinderService::SensorEventListener::OnAccuracyChanged(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 accuracy)
{
    return NOERROR;
}


//==============================================================================
//          PowerManagerService::BinderService
//==============================================================================

CAR_INTERFACE_IMPL_2(PowerManagerService::BinderService, Object, IIPowerManager, IBinder);

PowerManagerService::BinderService::BinderService()
{}

PowerManagerService::BinderService::~BinderService()
{}


ECode PowerManagerService::BinderService::UpdateBlockedUids(
    /* [in] */ Int32 uid,
    /* [in] */ Boolean isBlocked)
{
    if (DEBUG_SPEW) Slogger::V(TAG, "updateBlockedUids: uid = %d, disBlocked = %d", uid, isBlocked);

    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        if (DEBUG_SPEW) Slogger::V(TAG, "UpdateBlockedUids is not allowed");
        return NOERROR;
    }

    {    AutoLock syncLock(mHost->mLock);
        if(isBlocked) {
            AutoPtr<IInteger32> integer;
            CInteger32::New(uid, (IInteger32**)&integer);
            mHost->mBlockedUids->Add(integer);
            Int32 size;
            mHost->mWakeLocks->GetSize(&size);
            for (Int32 index = 0; index < size; index++) {
                AutoPtr<IInterface> obj;
                mHost->mWakeLocks->Get(index, (IInterface**)&obj);
                AutoPtr<WakeLock> wl = (WakeLock*)(IObject*)obj.Get();
                if(wl != NULL) {
                    if(wl->mTag.StartWith("*sync*") && wl->mOwnerUid == IProcess::SYSTEM_UID) {
                        mHost->ReleaseWakeLockInternal(wl->mLock, wl->mFlags);
                        index--;
                        if (DEBUG_SPEW) Slogger::V(TAG, "Internally releasing the wakelock acquired by SyncManager");
                        continue;
                    }
                    // release the wakelock for the blocked uid
                    if (wl->mOwnerUid == uid || mHost->CheckWorkSourceObjectId(uid, wl)) {
                        mHost->ReleaseWakeLockInternal(wl->mLock, wl->mFlags);
                        index--;
                        if (DEBUG_SPEW) Slogger::V(TAG, "Internally releasing it");
                    }
                }
            }
        }
        else {
            AutoPtr<IInteger32> integer;
            CInteger32::New(uid, (IInteger32**)&integer);
            mHost->mBlockedUids->Remove(integer);
        }
    }
    return NOERROR;
}

ECode PowerManagerService::BinderService::CpuBoost(
    /* [in] */ Int32 duration)
{
    if (duration > 0 && duration <= MAX_CPU_BOOST_TIME) {
        // Don't send boosts if we're in another power profile
        String profile = mHost->mPerformanceManager->GetPowerProfile();
        if (profile.IsNull() || profile.Equals(IPowerManager::PROFILE_BALANCED)) {
            mHost->NativeCpuBoost(duration);
        }
    }
    else {
        Slogger::E(TAG, "Invalid boost duration: %d", duration);
    }
    return NOERROR;
}

ECode PowerManagerService::BinderService::SetKeyboardVisibility(
    /* [in] */ Boolean visible)
{
    AutoLock lock(mHost->mLock);
    if (DEBUG_SPEW) {
        Slogger::D(TAG, "setKeyboardVisibility: %d", visible);
    }
    if (mHost->mKeyboardVisible != visible) {
        mHost->mKeyboardVisible = visible;
        if (!visible) {
            // If hiding keyboard, turn off leds
            SetKeyboardLight(FALSE, 1);
            SetKeyboardLight(FALSE, 2);
        }
        {
            AutoLock lock(mHost->mLock);
            mHost->mDirty |= DIRTY_USER_ACTIVITY;
            mHost->UpdatePowerStateLocked();
        }
    }
    return NOERROR;
}

ECode PowerManagerService::BinderService::SetKeyboardLight(
    /* [in] */ Boolean on,
    /* [in] */ Int32 key)
{
    if (key == 1) {
        if (on)
            mHost->mCapsLight->SetColor(0x00ffffff);
        else
            mHost->mCapsLight->TurnOff();
    }
    else if (key == 2) {
        if (on)
            mHost->mFnLight->SetColor(0x00ffffff);
        else
            mHost->mFnLight->TurnOff();
    }
    return NOERROR;
}

ECode PowerManagerService::BinderService::WakeUpWithProximityCheck(
    /* [in] */ Int64 time)
{
    return WakeUp(time);
}

ECode PowerManagerService::BinderService::SetPowerProfile(
    /* [in] */ const String& profile,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::DEVICE_POWER, String(NULL)))
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mHost->SetLowPowerModeInternal(IPowerManager::PROFILE_POWER_SAVE.Equals(profile));
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    *result = mHost->mPerformanceManager->SetPowerProfile(profile);
    return NOERROR;
}

ECode PowerManagerService::BinderService::GetPowerProfile(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mPerformanceManager->GetPowerProfile();
    return NOERROR;
}

ECode PowerManagerService::BinderService::ActivityResumed(
    /* [in] */ const String& componentName)
{
    mHost->mPerformanceManager->ActivityResumed(componentName);
    return NOERROR;
}

ECode PowerManagerService::BinderService::constructor(
    /* [in] */ ISystemService* service)
{
    mHost = (PowerManagerService*)service;
    return NOERROR;
}

ECode PowerManagerService::BinderService::AcquireWakeLockWithUid(
    /* [in] */ IBinder* lock,
    /* [in] */ Int32 flags,
    /* [in] */ const String& tag,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    if (uid < 0) {
        Int32 id = Binder::GetCallingUid();
        uid = id;
    }

    AutoPtr<IWorkSource> workSource;
    CWorkSource::New(uid, (IWorkSource**)&workSource);
    return AcquireWakeLock(lock, flags, tag, packageName, workSource, String(NULL));
}

ECode PowerManagerService::BinderService::PowerHint(
    /* [in] */ Int32 hintId,
    /* [in] */ Int32 data)
{
    if (!(mHost->mSystemReady)) {
        // Service not ready yet, so who the heck cares about power hints, bah.
        return NOERROR;
    }
    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::DEVICE_POWER, String(NULL)));
    mHost->PowerHintInternal(hintId, data);
    return NOERROR;
}

ECode PowerManagerService::BinderService::AcquireWakeLock(
    /* [in] */ IBinder* lock,
    /* [in] */ Int32 flags,
    /* [in] */ const String& tag,
    /* [in] */ const String& packageName,
    /* [in] */ IWorkSource* ws,
    /* [in] */ const String& historyTag)
{
    if (lock == NULL) {
        Slogger::E(TAG, "lock must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("lock must not be null");
    }

    if (packageName.IsNull()) {
        Slogger::E(TAG, "packageName must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("packageName must not be null");
    }

    AutoPtr<IPowerManagerHelper> helper;
    CPowerManagerHelper::AcquireSingleton((IPowerManagerHelper**)&helper);
    helper->ValidateWakeLockParameters(flags, tag);
    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::WAKE_LOCK, String(NULL)));
    if ((flags & IPowerManager::DOZE_WAKE_LOCK) != 0) {
        FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
                Elastos::Droid::Manifest::permission::DEVICE_POWER, String(NULL)));
    }
    Int32 size;
    if (ws != NULL && (ws->GetSize(&size), size != 0)) {
        FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
                Elastos::Droid::Manifest::permission::UPDATE_DEVICE_STATS, String(NULL)));
    }
    else {
        ws = NULL;
    }

    Int32 uid = Binder::GetCallingUid();
    Int32 pid = Binder::GetCallingPid();

    // try {
    Int32 result;
    if (mHost->mAppOps != NULL &&
            (mHost->mAppOps->CheckOperation(IAppOpsManager::OP_WAKE_LOCK, uid, packageName, &result),
                result != IAppOpsManager::MODE_ALLOWED)) {
        Slogger::D(TAG, "acquireWakeLock: ignoring request from %s", packageName.string());
        // For (ignore) accounting purposes
        Int32 v;
        mHost->mAppOps->NoteOperation(IAppOpsManager::OP_WAKE_LOCK, uid, packageName, &v);
        // silent return
        return NOERROR;
    }
    // } catch (RemoteException e) {
    // }

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    ECode ec = mHost->AcquireWakeLockInternal(lock, flags, tag, packageName, ws, historyTag, uid, pid);
    Binder::RestoreCallingIdentity(ident);
    return ec;
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
}

ECode PowerManagerService::BinderService::ReleaseWakeLock(
    /* [in] */ IBinder* lock,
    /* [in] */ Int32 flags)
{
    if (lock == NULL) {
        Slogger::E(TAG, "lock must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("lock must not be null");
    }

    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::WAKE_LOCK, String(NULL)));

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mHost->ReleaseWakeLockInternal(lock, flags);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode PowerManagerService::BinderService::UpdateWakeLockUids(
    /* [in] */ IBinder* lock,
    /* [in] */ ArrayOf<Int32>* uids)
{
    AutoPtr<IWorkSource> ws;

    if (uids != NULL) {
        CWorkSource::New((IWorkSource**)&ws);
        // XXX should WorkSource have a way to set uids as an int[] instead of adding them
        // one at a time?
        for (Int32 i = 0; i < uids->GetLength(); i++) {
            Boolean res;
            ws->Add((*uids)[i], &res);
        }
    }
    return UpdateWakeLockWorkSource(lock, ws, String(NULL));
}

ECode PowerManagerService::BinderService::UpdateWakeLockWorkSource(
    /* [in] */ IBinder* lock,
    /* [in] */ IWorkSource* ws,
    /* [in] */ const String& historyTag)
{
    if (lock == NULL) {
        Slogger::E(TAG, "lock must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("lock must not be null");
    }

    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::WAKE_LOCK, String(NULL)));
    Int32 size;
    if (ws != NULL && (ws->GetSize(&size), size != 0)) {
        FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
                Elastos::Droid::Manifest::permission::UPDATE_DEVICE_STATS, String(NULL)));
    }
    else {
        ws = NULL;
    }

    Int32 callingUid = Binder::GetCallingUid();
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    ECode ec = mHost->UpdateWakeLockWorkSourceInternal(lock, ws, historyTag, callingUid);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return ec;
}

ECode PowerManagerService::BinderService::IsWakeLockLevelSupported(
    /* [in] */ Int32 level,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    *res = mHost->IsWakeLockLevelSupportedInternal(level);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode PowerManagerService::BinderService::UserActivity(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 event,
    /* [in] */ Int32 flags)
{
    Int64 now = SystemClock::GetUptimeMillis();
    Int32 result1, result2;
    FAIL_RETURN(mHost->mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DEVICE_POWER, &result1));
    FAIL_RETURN(mHost->mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::USER_ACTIVITY, &result2));
    if (result1 != IPackageManager::PERMISSION_GRANTED
            && result2 != IPackageManager::PERMISSION_GRANTED) {
        // Once upon a time applications could call userActivity().
        // Now we require the DEVICE_POWER permission.  Log a warning and ignore the
        // request instead of throwing a SecurityException so we don't break old apps.
        {
            AutoLock syncLock(mHost->mLock);
            if (now >= mHost->mLastWarningAboutUserActivityPermission + (5 * 60 * 1000)) {
                mHost->mLastWarningAboutUserActivityPermission = now;
                Slogger::W(TAG, "Ignoring call to PowerManager.userActivity() because the caller"
                    " does not have DEVICE_POWER permission.  Please fix your app!   pid=%d uid=%d",
                    Binder::GetCallingPid(), Binder::GetCallingUid());
            }
        }
        return NOERROR;
    }

    if (eventTime > SystemClock::GetUptimeMillis()) {
        Slogger::E(TAG, "event time must not be in the future");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("event time must not be in the future");
    }

    Int32 uid = Binder::GetCallingUid();
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mHost->UserActivityInternal(eventTime, event, flags, uid);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode PowerManagerService::BinderService::WakeUp(
    /* [in] */ Int64 eventTime)
{
    if (eventTime > SystemClock::GetUptimeMillis()) {
        Slogger::E(TAG, "event time must not be in the future");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("event time must not be in the future");
    }

    // check wakeup caller under QuickBoot mode
    AutoPtr<ISystemProperties> props;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&props);
    Int32 value;
    if (props->GetInt32(String("sys.quickboot.enable"), 0, &value), value == 1) {
        if (!mHost->IsQuickBootCall()) {
            Slogger::D(TAG, "ignore wakeup request under QuickBoot");
            return NOERROR;
        }
    }

    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::DEVICE_POWER, String(NULL)));

    Int32 uid = Binder::GetCallingUid();
    AutoPtr<IRunnable> r = new WakeUpRunnable(mHost, eventTime, uid);
    RunWithProximityCheck(r);
    return NOERROR;
}

void PowerManagerService::BinderService::RunWithProximityCheck(
    /* [in] */ IRunnable* r)
{
    Boolean hasMessages;
    if (mHost->mHandler->HasMessages(MSG_WAKE_UP, &hasMessages), hasMessages) {
        // There is already a message queued;
        return;
    }

    Boolean hasIncomingCall = FALSE;
    AutoPtr<IInterface> service;
    mHost->mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&service);
    AutoPtr<ITelephonyManager> tm = ITelephonyManager::Probe(service);
    if (tm != NULL) {
        Int32 callState;
        tm->GetCallState(&callState);
        hasIncomingCall = callState == ITelephonyManager::CALL_STATE_RINGING;
    }

    if (mHost->mProximityWakeSupported && mHost->mProximityWakeEnabled && mHost->mProximitySensor != NULL
            && !hasIncomingCall) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_WAKE_UP, (IMessage**)&msg);
        msg->SetObj(r);
        Boolean result;
        mHost->mHandler->SendMessageDelayed(msg, mHost->mProximityTimeOut, &result);
        RunPostProximityCheck(r);
    }
    else {
        r->Run();
    }
}

void PowerManagerService::BinderService::RunPostProximityCheck(
    /* [in] */ IRunnable* r)
{
    if (mHost->mSensorManager == NULL) {
        r->Run();
        return;
    }
    mHost->mProximityWakeLock->AcquireLock();
    mHost->mProximityListener = new SensorEventListener(mHost, r);
    Boolean result;
    mHost->mSensorManager->RegisterListener(mHost->mProximityListener,
           mHost->mProximitySensor, ISensorManager::SENSOR_DELAY_FASTEST, &result);
}

ECode PowerManagerService::BinderService::GoToSleep(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 reason,
    /* [in] */ Int32 flags)
{
    if (eventTime > SystemClock::GetUptimeMillis()) {
        Slogger::E(TAG, "event time must not be in the future");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::DEVICE_POWER, String(NULL)));

    Int32 uid = Binder::GetCallingUid();
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mHost->GoToSleepInternal(eventTime, reason, flags, uid);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode PowerManagerService::BinderService::Nap(
    /* [in] */ Int64 eventTime)
{
    if (eventTime > SystemClock::GetUptimeMillis()) {
        Slogger::E(TAG, "event time must not be in the future");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("event time must not be in the future");
    }

    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::DEVICE_POWER, String(NULL)));

    Int32 uid = Binder::GetCallingUid();
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mHost->NapInternal(eventTime, uid);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode PowerManagerService::BinderService::IsInteractive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    *result = mHost->IsInteractiveInternal();
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode PowerManagerService::BinderService::IsPowerSaveMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    *result = mHost->IsLowPowerModeInternal();
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode PowerManagerService::BinderService::SetPowerSaveMode(
    /* [in] */ Boolean mode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::DEVICE_POWER, String(NULL)));

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    Boolean changed = mHost->SetLowPowerModeInternal(mode);
    if (changed) {
        mHost->mPerformanceManager->SetPowerProfile(mHost->mLowPowerModeEnabled ?
                IPowerManager::PROFILE_POWER_SAVE : IPowerManager::PROFILE_BALANCED);
    }
    *result = changed;
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode PowerManagerService::BinderService::Reboot(
    /* [in] */ Boolean confirm,
    /* [in] */ const String& reason,
    /* [in] */ Boolean wait)
{
    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::REBOOT, String(NULL)));

    if (IPowerManager::REBOOT_RECOVERY.Equals(reason)) {
        FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
                Elastos::Droid::Manifest::permission::RECOVERY, String(NULL)));
    }

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    ECode ec = mHost->ShutdownOrRebootInternal(FALSE, confirm, reason, wait);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return ec;
}

ECode PowerManagerService::BinderService::Shutdown(
    /* [in] */ Boolean confirm,
    /* [in] */ Boolean wait)
{
    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::REBOOT, String(NULL)));

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    ECode ec = mHost->ShutdownOrRebootInternal(TRUE, confirm, String(NULL), wait);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return ec;
}

ECode PowerManagerService::BinderService::Crash(
    /* [in] */ const String& message)
{
    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::REBOOT, String(NULL)));

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    ECode ec = mHost->CrashInternal(message);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return ec;
}

ECode PowerManagerService::BinderService::SetStayOnSetting(
    /* [in] */ Int32 val)
{
    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::WRITE_SETTINGS, String(NULL)));

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mHost->SetStayOnSettingInternal(val);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode PowerManagerService::BinderService::SetMaximumScreenOffTimeoutFromDeviceAdmin(
    /* [in] */ Int32 timeMs)
{
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mHost->SetMaximumScreenOffTimeoutFromDeviceAdminInternal(timeMs);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode PowerManagerService::BinderService::SetTemporaryScreenBrightnessSettingOverride(
    /* [in] */ Int32 brightness)
{
    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::DEVICE_POWER, String(NULL)));

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mHost->SetTemporaryScreenBrightnessSettingOverrideInternal(brightness);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode PowerManagerService::BinderService::SetTemporaryScreenAutoBrightnessAdjustmentSettingOverride(
    /* [in] */ Float adj)
{
    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::DEVICE_POWER, String(NULL)));

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mHost->SetTemporaryScreenAutoBrightnessAdjustmentSettingOverrideInternal(adj);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode PowerManagerService::BinderService::SetAttentionLight(
    /* [in] */ Boolean on,
    /* [in] */ Int32 color)
{
    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::DEVICE_POWER, String(NULL)));

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mHost->SetAttentionLightInternal(on, color);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode PowerManagerService::BinderService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "PowerManagerService::BinderService";
    return NOERROR;
}

ECode PowerManagerService::BinderService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Int32 result;
    FAIL_RETURN(mHost->mContext->CheckCallingOrSelfPermission(/*Elastos::Droid::*/Manifest::permission::DUMP, &result));
    if (result != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder buider;
        buider += "Permission Denial: can't dump PowerManager from from pid=";
        buider += Binder::GetCallingPid();
        buider += ", uid=";
        buider += Binder::GetCallingUid();
        pw->Println(buider.ToString());
        return NOERROR;
    }

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mHost->DumpInternal(pw);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

//==============================================================================
//          PowerManagerService::BatteryReceiver
//==============================================================================

PowerManagerService::BatteryReceiver::BatteryReceiver(
    /* [in] */ PowerManagerService* host)
    : mHost(host)
{}

ECode PowerManagerService::BatteryReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    {
        AutoLock syncLock(mHost->mLock);
        mHost->HandleBatteryStateChangedLocked();
    }
    return NOERROR;
}

//==============================================================================
//          PowerManagerService::DreamReceiver
//==============================================================================

PowerManagerService::DreamReceiver::DreamReceiver(
    /* [in] */ PowerManagerService* host)
    : mHost(host)
{}

ECode PowerManagerService::DreamReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    {
        AutoLock syncLock(mHost->mLock);
        mHost->ScheduleSandmanLocked();
    }
    return NOERROR;
}

//==============================================================================
//          PowerManagerService::UserSwitchedReceiver
//==============================================================================

PowerManagerService::UserSwitchedReceiver::UserSwitchedReceiver(
    /* [in] */ PowerManagerService* host)
    : mHost(host)
{}

ECode PowerManagerService::UserSwitchedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    {
        AutoLock syncLock(mHost->mLock);
        mHost->HandleSettingsChangedLocked();
    }
    return NOERROR;
}

//==============================================================================
//          PowerManagerService::DockReceiver
//==============================================================================

PowerManagerService::DockReceiver::DockReceiver(
    /* [in] */ PowerManagerService* host)
    : mHost(host)
{}

ECode PowerManagerService::DockReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    {
        AutoLock syncLock(mHost->mLock);
        Int32 dockState;
        intent->GetInt32Extra(IIntent::EXTRA_DOCK_STATE,
                IIntent::EXTRA_DOCK_STATE_UNDOCKED, &dockState);
        if (mHost->mDockState != dockState) {
            mHost->mDockState = dockState;
            mHost->mDirty |= DIRTY_DOCK_STATE;
            mHost->UpdatePowerStateLocked();
        }
    }

    return NOERROR;
}

//==============================================================================
//          PowerManagerService::SettingsObserver
//==============================================================================

ECode PowerManagerService::SettingsObserver::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ PowerManagerService* host)
{
    mHost = host;
    return ContentObserver::constructor(handler);
}

ECode PowerManagerService::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    {
        AutoLock syncLock(mHost->mLock);
        mHost->HandleSettingsChangedLocked();
    }
    return NOERROR;
}

//==============================================================================
//          PowerManagerService::PowerManagerHandler
//==============================================================================

ECode PowerManagerService::PowerManagerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case PowerManagerService::MSG_USER_ACTIVITY_TIMEOUT:
            mHost->HandleUserActivityTimeout();
            break;
        case PowerManagerService::MSG_SANDMAN:
            mHost->HandleSandman();
            break;
        case MSG_WAKE_UP:
            mHost->CleanupProximity();
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IRunnable::Probe(obj)->Run();
            break;
    }

    return NOERROR;
}

//==============================================================================
//          PowerManagerService::WakeLock
//==============================================================================

CAR_INTERFACE_IMPL(PowerManagerService::WakeLock, Object, IProxyDeathRecipient);

PowerManagerService::WakeLock::WakeLock(
    /* [in] */ IBinder* lock,
    /* [in] */ Int32 flags,
    /* [in] */ const String& tag,
    /* [in] */ const String& packageName,
    /* [in] */ IWorkSource* workSource,
    /* [in] */ const String& historyTag,
    /* [in] */ Int32 ownerUid,
    /* [in] */ Int32 ownerPid,
    /* [in] */ PowerManagerService* host)
    : mLock(lock)
    , mFlags(flags)
    , mTag(tag)
    , mPackageName(packageName)
    , mHistoryTag(historyTag)
    , mOwnerUid(ownerUid)
    , mOwnerPid(ownerPid)
    , mHost(host)
{
    mWorkSource = PowerManagerService::CopyWorkSource(workSource);
}

ECode PowerManagerService::WakeLock::ProxyDied()
{
    mHost->HandleWakeLockDeath(this);
    return NOERROR;
}

Boolean PowerManagerService::WakeLock::HasSameProperties(
    /* [in] */ Int32 flags,
    /* [in] */ const String& tag,
    /* [in] */ IWorkSource* workSource,
    /* [in] */ Int32 ownerUid,
    /* [in] */ Int32 ownerPid)
{
    return mFlags == flags
            && mTag.Equals(tag)
            && HasSameWorkSource(workSource)
            && mOwnerUid == ownerUid
            && mOwnerPid == ownerPid;
}

ECode PowerManagerService::WakeLock::UpdateProperties(
    /* [in] */ Int32 flags,
    /* [in] */ const String& tag,
    /* [in] */ const String& packageName,
    /* [in] */ IWorkSource* workSource,
    /* [in] */ const String& historyTag,
    /* [in] */ Int32 ownerUid,
    /* [in] */ Int32 ownerPid)
{
    if (!mPackageName.Equals(packageName)) {
        Slogger::E("PowerManagerService::WakeLock", "Existing wake lock package name changed: %s to %s",
                mPackageName.string(), packageName.string());
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Existing wake lock package name changed: "
        //         + mPackageName + " to " + packageName);
    }
    if (mOwnerUid != ownerUid) {
        Slogger::E("PowerManagerService::WakeLock", "Existing wake lock uid changed: %d to %d",
                mOwnerUid, ownerUid);
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Existing wake lock uid changed: "
        //         + mOwnerUid + " to " + ownerUid);
    }
    if (mOwnerPid != ownerPid) {
        Slogger::E("PowerManagerService::WakeLock", "Existing wake lock uid changed: %d to %d",
                mOwnerPid, ownerPid);
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Existing wake lock pid changed: "
        //         + mOwnerPid + " to " + ownerPid);
    }

    mFlags = flags;
    mTag = tag;
    UpdateWorkSource(workSource);
    mHistoryTag = historyTag;
    return NOERROR;
}

Boolean PowerManagerService::WakeLock::HasSameWorkSource(
    /* [in] */ IWorkSource* workSource)
{
    return Object::Equals(mWorkSource, workSource);
}

void PowerManagerService::WakeLock::UpdateWorkSource(
    /* [in] */ IWorkSource* workSource)
{
    mWorkSource = PowerManagerService::CopyWorkSource(workSource);
}

ECode PowerManagerService::WakeLock::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder buider;
    buider += GetLockLevelString();
    buider += " '";
    buider += mTag;
    buider += "'";
    buider += GetLockFlagsString();
    buider += " (uid=";
    buider += mOwnerUid;
    buider += ", pid=";
    buider += mOwnerPid;
    buider += ", ws=";
    buider += mWorkSource;
    buider += ")";

    *str = buider.ToString();
    return NOERROR;
}

String PowerManagerService::WakeLock::GetLockLevelString()
{
    switch (mFlags & IPowerManager::WAKE_LOCK_LEVEL_MASK) {
        case IPowerManager::FULL_WAKE_LOCK:
            return String("FULL_WAKE_LOCK                ");
        case IPowerManager::SCREEN_BRIGHT_WAKE_LOCK:
            return String("SCREEN_BRIGHT_WAKE_LOCK       ");
        case IPowerManager::SCREEN_DIM_WAKE_LOCK:
            return String("SCREEN_DIM_WAKE_LOCK          ");
        case IPowerManager::PARTIAL_WAKE_LOCK:
            return String("PARTIAL_WAKE_LOCK             ");
        case IPowerManager::PROXIMITY_SCREEN_OFF_WAKE_LOCK:
            return String("PROXIMITY_SCREEN_OFF_WAKE_LOCK");
        case IPowerManager::DOZE_WAKE_LOCK:
            return String("DOZE_WAKE_LOCK                ");
        default:
            return String("???                           ");
    }
}

String PowerManagerService::WakeLock::GetLockFlagsString()
{
    String result("");
    if ((mFlags & IPowerManager::ACQUIRE_CAUSES_WAKEUP) != 0) {
        result += " ACQUIRE_CAUSES_WAKEUP";
    }
    if ((mFlags & IPowerManager::ON_AFTER_RELEASE) != 0) {
        result += " ON_AFTER_RELEASE";
    }
    return result;
}

//==============================================================================
//          PowerManagerService::SuspendBlockerImpl
//==============================================================================

CAR_INTERFACE_IMPL(PowerManagerService::SuspendBlockerImpl, Object, ISuspendBlocker);

PowerManagerService::SuspendBlockerImpl::SuspendBlockerImpl(
    /* [in] */ const String& name)
    : mName(name)
    , mReferenceCount(0)
{
    StringBuilder buider;
    buider += "SuspendBlocker (";
    buider += name;
    buider += ")";
    mTraceName = buider.ToString();
}

PowerManagerService::SuspendBlockerImpl::~SuspendBlockerImpl()
{
    // try {
    if (mReferenceCount != 0) {
        Slogger::E(TAG, "Suspend blocker %s was finalized without being released!",
                mName.string());
        // Log.wtf(TAG, "Suspend blocker \"" + mName
        //         + "\" was finalized without being released!");
        mReferenceCount = 0;
        PowerManagerService::NativeReleaseSuspendBlocker(mName);
        // Trace.asyncTraceEnd(Trace.TRACE_TAG_POWER, mTraceName, 0);
    }
    // } finally {
    //     super.finalize();
    // }
}

ECode PowerManagerService::SuspendBlockerImpl::AcquireBlocker()
{
    {    AutoLock syncLock(this);
        mReferenceCount += 1;
        if (mReferenceCount == 1) {
            if (DEBUG_SPEW) {
                Slogger::D(TAG, "Acquiring suspend blocker %s.", mName.string());
            }
            // Trace.asyncTraceBegin(Trace.TRACE_TAG_POWER, mTraceName, 0);
            PowerManagerService::NativeAcquireSuspendBlocker(mName);
        }
    }
    return NOERROR;
}

ECode PowerManagerService::SuspendBlockerImpl::ReleaseBlocker()
{
    {    AutoLock syncLock(this);
        mReferenceCount -= 1;
        if (mReferenceCount == 0) {
            if (DEBUG_SPEW) {
                Slogger::D(TAG, "Releasing suspend blocker %s.", mName.string());
            }
            PowerManagerService::NativeReleaseSuspendBlocker(mName);
            // Trace.asyncTraceEnd(Trace.TRACE_TAG_POWER, mTraceName, 0);
        }
        else if (mReferenceCount < 0) {
            Slogger::E(TAG, "Suspend blocker %s was released without being acquired!",
                    mName.string());
            mReferenceCount = 0;
            return E_INVALID_ARGUMENT;
        }
    }
    return NOERROR;
}

ECode PowerManagerService::SuspendBlockerImpl::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    {    AutoLock syncLock(this);
        StringBuilder buider;
        buider += mName;
        buider += ": ref count=";
        buider += mReferenceCount;
        *str = buider.ToString();
    }
    return NOERROR;
}

//==============================================================================
//          PowerManagerService::LocalService
//==============================================================================

CAR_INTERFACE_IMPL(PowerManagerService::LocalService, Object, IPowerManagerInternal);

PowerManagerService::LocalService::LocalService(
    /* [in] */ PowerManagerService* host)
    : mHost(host)
{}

PowerManagerService::LocalService::~LocalService()
{}

ECode PowerManagerService::LocalService::SetScreenBrightnessOverrideFromWindowManager(
    /* [in] */ Int32 screenBrightness)
{
    if (screenBrightness < IPowerManager::BRIGHTNESS_DEFAULT
            || screenBrightness > IPowerManager::BRIGHTNESS_ON) {
        screenBrightness = IPowerManager::BRIGHTNESS_DEFAULT;
    }
    mHost->SetScreenBrightnessOverrideFromWindowManagerInternal(screenBrightness);
    return NOERROR;
}

ECode PowerManagerService::LocalService::SetButtonBrightnessOverrideFromWindowManager(
    /* [in] */ Int32 screenBrightness)
{
    FAIL_RETURN(mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::DEVICE_POWER, String(NULL)))

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mHost->SetButtonBrightnessOverrideFromWindowManagerInternal(screenBrightness);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode PowerManagerService::LocalService::SetDozeOverrideFromDreamManager(
    /* [in] */ Int32 screenState,
    /* [in] */ Int32 screenBrightness)
{
    switch (screenState) {
        case IDisplay::STATE_UNKNOWN:
        case IDisplay::STATE_OFF:
        case IDisplay::STATE_DOZE:
        case IDisplay::STATE_DOZE_SUSPEND:
        case IDisplay::STATE_ON:
            break;
        default:
            screenState = IDisplay::STATE_UNKNOWN;
            break;
    }
    if (screenBrightness < IPowerManager::BRIGHTNESS_DEFAULT
            || screenBrightness > IPowerManager::BRIGHTNESS_ON) {
        screenBrightness = IPowerManager::BRIGHTNESS_DEFAULT;
    }
    mHost->SetDozeOverrideFromDreamManagerInternal(screenState, screenBrightness);
    return NOERROR;
}

ECode PowerManagerService::LocalService::SetUserActivityTimeoutOverrideFromWindowManager(
    /* [in] */ Int64 timeoutMillis)
{
    mHost->SetUserActivityTimeoutOverrideFromWindowManagerInternal(timeoutMillis);
    return NOERROR;
}

ECode PowerManagerService::LocalService::GetLowPowerModeEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    {
        AutoLock syncLock(mHost->mLock);
        *result = mHost->mLowPowerModeEnabled;
        return NOERROR;
    }
    return NOERROR;
}

ECode PowerManagerService::LocalService::RegisterLowPowerModeObserver(
    /* [in] */ ILowPowerModeListener* listener)
{
    {
        AutoLock syncLock(mHost->mLock);
        mHost->mLowPowerModeListeners->Add(listener);
    }
    return NOERROR;
}

//==============================================================================
//          PowerManagerService::UpdateLowPowerModeLockedRunnable
//==============================================================================

PowerManagerService::UpdateLowPowerModeLockedRunnable::UpdateLowPowerModeLockedRunnable(
    /* [in] */ PowerManagerService* host,
    /* [in] */ Boolean arg)
    : mHost(host)
    , mArg(arg)
{}

ECode PowerManagerService::UpdateLowPowerModeLockedRunnable::Run()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IPowerManager::ACTION_POWER_SAVE_MODE_CHANGING, (IIntent**)&intent);
    intent->PutExtra(IPowerManager::EXTRA_POWER_SAVE_MODE, mHost->mLowPowerModeEnabled);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
    mHost->mContext->SendBroadcast(intent);
    // ArrayList<ILowPowerModeListener> listeners;
    AutoPtr<IArrayList> listeners;
    {
        AutoLock syncLock(mHost->mLock);
        CArrayList::New(ICollection::Probe(mHost->mLowPowerModeListeners), (IArrayList**)&listeners);
    }
    Int32 size;
    listeners->GetSize(&size);
    for (Int32 i=0; i < size; i++) {
        AutoPtr<IInterface> obj;
        listeners->Get(i, (IInterface**)&obj);
        AutoPtr<ILowPowerModeListener> listener = ILowPowerModeListener::Probe(obj);
        listener->OnLowPowerModeChanged(mArg);
    }
    intent = NULL;
    CIntent::New(IPowerManager::ACTION_POWER_SAVE_MODE_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
    mHost->mContext->SendBroadcast(intent);

    return NOERROR;
}

//==============================================================================
//          PowerManagerService::
//==============================================================================

CAR_INTERFACE_IMPL(PowerManagerService::MyDisplayPowerCallbacks, Object, IDisplayPowerCallbacks);

PowerManagerService::MyDisplayPowerCallbacks::MyDisplayPowerCallbacks(
    /* [in] */ PowerManagerService* host)
    : mHost(host)
    , mDisplayState(IDisplay::STATE_UNKNOWN)
{
}

ECode PowerManagerService::MyDisplayPowerCallbacks::OnStateChanged()
{
    {
        AutoLock syncLock(mHost->mLock);
        mHost->mDirty |= PowerManagerService::DIRTY_ACTUAL_DISPLAY_POWER_STATE_UPDATED;
        mHost->UpdatePowerStateLocked();
    }
    return NOERROR;
}

ECode PowerManagerService::MyDisplayPowerCallbacks::OnProximityPositive()
{
    {
        AutoLock syncLock(mHost->mLock);
        mHost->mProximityPositive = TRUE;
        mHost->mDirty |= PowerManagerService::DIRTY_PROXIMITY_POSITIVE;
        mHost->UpdatePowerStateLocked();
    }
    return NOERROR;
}

ECode PowerManagerService::MyDisplayPowerCallbacks::OnProximityNegative()
{
    {
        AutoLock syncLock(mHost->mLock);
        mHost->mProximityPositive = FALSE;
        mHost->mDirty |= PowerManagerService::DIRTY_PROXIMITY_POSITIVE;
        mHost->UserActivityNoUpdateLocked(SystemClock::GetUptimeMillis(),
                IPowerManager::USER_ACTIVITY_EVENT_OTHER, 0, IProcess::SYSTEM_UID);
        mHost->UpdatePowerStateLocked();
    }
    return NOERROR;
}

ECode PowerManagerService::MyDisplayPowerCallbacks::OnDisplayStateChange(
    /* [in] */ Int32 state)
{
    // This method is only needed to support legacy display blanking behavior
    // where the display's power state is coupled to suspend or to the power HAL.
    // The order of operations matters here.
    {
        AutoLock syncLock(mHost->mLock);
        if (mDisplayState != state) {
            mDisplayState = state;
            if (state == IDisplay::STATE_OFF) {
                if (!mHost->mDecoupleHalInteractiveModeFromDisplayConfig) {
                    mHost->SetHalInteractiveModeLocked(FALSE);
                }
                if (!mHost->mDecoupleHalAutoSuspendModeFromDisplayConfig) {
                    mHost->SetHalAutoSuspendModeLocked(TRUE);
                }
            }
            else {
                if (!mHost->mDecoupleHalAutoSuspendModeFromDisplayConfig) {
                    mHost->SetHalAutoSuspendModeLocked(FALSE);
                }
                if (!mHost->mDecoupleHalInteractiveModeFromDisplayConfig) {
                    mHost->SetHalInteractiveModeLocked(TRUE);
                }
            }
        }
    }
    return NOERROR;
}

ECode PowerManagerService::MyDisplayPowerCallbacks::AcquireSuspendBlocker()
{
    return mHost->mDisplaySuspendBlocker->AcquireBlocker();
}

ECode PowerManagerService::MyDisplayPowerCallbacks::ReleaseSuspendBlocker()
{
    return mHost->mDisplaySuspendBlocker->ReleaseBlocker();
}

static String StateToString(
    /* [in] */ Int32 state)
{
    switch (state) {
        case IDisplay::STATE_UNKNOWN:
            return String("UNKNOWN");
        case IDisplay::STATE_OFF:
            return String("OFF");
        case IDisplay::STATE_ON:
            return String("ON");
        case IDisplay::STATE_DOZE:
            return String("DOZE");
        case IDisplay::STATE_DOZE_SUSPEND:
            return String("DOZE_SUSPEND");
        default:
            return StringUtils::ToString(state);
    }
}

ECode PowerManagerService::MyDisplayPowerCallbacks::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    {
        AutoLock syncLock(this);
        StringBuilder sb("state=");
        sb += StateToString(mDisplayState);
        *str = sb.ToString();
        return NOERROR;
    }
    return NOERROR;
}

//==============================================================================
//          PowerManagerService::ShutdownOrRebootRunnable
//==============================================================================

PowerManagerService::ShutdownOrRebootRunnable::ShutdownOrRebootRunnable(
    /* [in] */ Boolean shutdown,
    /* [in] */ Boolean confirm,
    /* [in] */ const String& reason,
    /* [in] */ IContext* ctx)
    : mShutdown(shutdown)
    , mConfirm(confirm)
    , mReason(reason)
    , mContext(ctx)
{}

ECode PowerManagerService::ShutdownOrRebootRunnable::Run()
{
    {    AutoLock syncLock(this);
        if (mShutdown) {
            ShutdownThread::Shutdown(mContext, mConfirm);
        }
        else {
            ShutdownThread::Reboot(mContext, mReason, mConfirm);
        }
    }

    return NOERROR;
}

//==============================================================================
//          PowerManagerService::CrashThread
//==============================================================================

PowerManagerService::CrashThread::CrashThread(
    /* [in] */ const String& s)
{
    Thread::constructor(s);
}

ECode PowerManagerService::CrashThread::Run()
{
    Slogger::E("PowerManagerService", "PowerManagerService.crash()");
    return E_RUNTIME_EXCEPTION;
}

//==============================================================================
//          PowerManagerService
//==============================================================================

CAR_INTERFACE_IMPL(PowerManagerService, SystemService, IWatchdogMonitor);

PowerManagerService::PowerManagerService()
    : mButtonTimeout(0)
    , mButtonBrightness(0)
    , mButtonBrightnessSettingDefault(0)
    , mKeyboardBrightness(0)
    , mKeyboardBrightnessSettingDefault(0)
    , mDirty(0)
    , mWakefulness(0)
    , mSandmanSummoned(FALSE)
    , mSandmanScheduled(FALSE)
    , mWakeLockSummary(0)
    , mInteractive(FALSE)
    , mInteractiveChanging(FALSE)
    , mRequestWaitForNegativeProximity(FALSE)
    , mLastWakeTime(0LL)
    , mLastSleepTime(0LL)
    , mLastUserActivityTime(0LL)
    , mLastUserActivityTimeNoChangeLights(0LL)
    , mLastInteractivePowerHintTime(0LL)
    , mUserActivitySummary(0)
    , mDisplayReady(FALSE)
    , mHoldingWakeLockSuspendBlocker(FALSE)
    , mHoldingDisplaySuspendBlocker(FALSE)
    , mSystemReady(FALSE)
    , mBootCompleted(FALSE)
    , mHalAutoSuspendModeEnabled(FALSE)
    , mHalInteractiveModeEnabled(FALSE)
    , mIsPowered(FALSE)
    , mPlugType(0)
    , mBatteryLevel(0)
    , mBatteryLevelWhenDreamStarted(0)
    , mDockState(IIntent::EXTRA_DOCK_STATE_UNDOCKED)
    , mDecoupleHalAutoSuspendModeFromDisplayConfig(FALSE)
    , mDecoupleHalInteractiveModeFromDisplayConfig(FALSE)
    , mWakeUpWhenPluggedOrUnpluggedConfig(FALSE)
    , mSuspendWhenScreenOffDueToProximityConfig(FALSE)
    , mDreamsSupportedConfig(FALSE)
    , mDreamsEnabledByDefaultConfig(FALSE)
    , mDreamsActivatedOnSleepByDefaultConfig(FALSE)
    , mDreamsActivatedOnDockByDefaultConfig(FALSE)
    , mDreamsEnabledOnBatteryConfig(FALSE)
    , mDreamsBatteryLevelMinimumWhenPoweredConfig(0)
    , mDreamsBatteryLevelMinimumWhenNotPoweredConfig(0)
    , mDreamsBatteryLevelDrainCutoffConfig(0)
    , mDreamsEnabledSetting(FALSE)
    , mDreamsActivateOnSleepSetting(FALSE)
    , mDreamsActivateOnDockSetting(FALSE)
    , mDozeAfterScreenOffConfig(FALSE)
    , mMinimumScreenOffTimeoutConfig(0)
    , mMaximumScreenDimDurationConfig(0)
    , mMaximumScreenDimRatioConfig(0.f)
    , mScreenOffTimeoutSetting(0)
    , mSleepTimeoutSetting(0)
    , mMaximumScreenOffTimeoutFromDeviceAdmin(Elastos::Core::Math::INT32_MAX_VALUE)
    , mStayOnWhilePluggedInSetting(0)
    , mWakeUpWhenPluggedOrUnpluggedSetting(0)
    , mStayOn(FALSE)
    , mProximityPositive(FALSE)
    , mScreenBrightnessSettingMinimum(0)
    , mScreenBrightnessSettingMaximum(0)
    , mScreenBrightnessSettingDefault(0)
    , mScreenBrightnessSetting(0)
    , mScreenAutoBrightnessAdjustmentSetting(0.0f)
    , mScreenBrightnessModeSetting(0)
    , mScreenBrightnessOverrideFromWindowManager(-1)
    , mButtonBrightnessOverrideFromWindowManager(-1)
    , mUserActivityTimeoutOverrideFromWindowManager(-1)
    , mTemporaryScreenBrightnessSettingOverride(-1)
    , mTemporaryScreenAutoBrightnessAdjustmentSettingOverride(Elastos::Core::Math::FLOAT_NAN)
    , mDozeScreenStateOverrideFromDreamManager(IDisplay::STATE_UNKNOWN)
    , mDozeScreenBrightnessOverrideFromDreamManager(IPowerManager::BRIGHTNESS_DEFAULT)
    , mLastWarningAboutUserActivityPermission(Elastos::Core::Math::INT64_MAX_VALUE)
    , mLowPowerModeEnabled(FALSE)
    , mLowPowerModeSetting(FALSE)
    , mAutoLowPowerModeConfigured(FALSE)
    , mAutoLowPowerModeSnoozing(FALSE)
    , mBatteryLevelLow(FALSE)
    , mKeyboardVisible(FALSE)
    , mProximityWakeEnabled(FALSE)
    , mProximityTimeOut(0)
    , mProximityWakeSupported(FALSE)
{
    CArrayList::New((IArrayList**)&mSuspendBlockers);
    CArrayList::New((IArrayList**)&mWakeLocks);
    CDisplayPowerRequest::New((IDisplayPowerRequest**)&mDisplayPowerRequest);
    CArrayList::New((IArrayList**)&mLowPowerModeListeners);
    CArrayList::New((IArrayList**)&mBlockedUids);
    mDisplayPowerCallbacks = (IDisplayPowerCallbacks*)new MyDisplayPowerCallbacks(this);
}

PowerManagerService::~PowerManagerService()
{}

ECode PowerManagerService::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(SystemService::constructor(context));
    mContext = context;

    mHandlerThread = new ServiceThread(TAG,
            IProcess::THREAD_PRIORITY_DISPLAY, FALSE /*allowIo*/);
    mHandlerThread->Start();
    AutoPtr<ILooper> looper;
    ASSERT_SUCCEEDED(mHandlerThread->GetLooper((ILooper**)&looper));
    mHandler = new PowerManagerHandler(looper, this);
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::SENSOR_SERVICE, (IInterface**)&service);
    mSensorManager = ISensorManager::Probe(service);
    mSensorManager->GetDefaultSensor(ISensor::TYPE_PROXIMITY, (ISensor**)&mProximitySensor);
    mPerformanceManager = new PerformanceManager(context);

    {
        AutoLock syncLock(mLock);
        mWakeLockSuspendBlocker = CreateSuspendBlockerLocked(String("PowerManagerService.WakeLocks"));
        mDisplaySuspendBlocker = CreateSuspendBlockerLocked(String("PowerManagerService.Display"));
        mDisplaySuspendBlocker->AcquireBlocker();
        mHoldingDisplaySuspendBlocker = TRUE;
        mHalAutoSuspendModeEnabled = FALSE;
        mHalInteractiveModeEnabled = TRUE;

        mWakefulness = WAKEFULNESS_AWAKE;
        mInteractive = TRUE;

        NativeInit();
        NativeSetAutoSuspend(FALSE);
        NativeSetInteractive(TRUE);
    }

    return NOERROR;
}

ECode PowerManagerService::OnStart()
{
    AutoPtr<IBinder> binderServide;
    CPowerManagerServiceBinderService::New(this, (IBinder**)&binderServide);

    PublishBinderService(IContext::POWER_SERVICE, binderServide);
    AutoPtr<LocalService> service = new LocalService(this);
    PublishLocalService(EIID_IPowerManagerInternal, (IPowerManagerInternal*)service);

    AutoPtr<Watchdog> watchdog = Watchdog::GetInstance();
    watchdog->AddMonitor(this);
    watchdog->AddThread(mHandler);

    return NOERROR;
}

ECode PowerManagerService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    {
        AutoLock syncLock(mLock);
        if (phase == ISystemService::PHASE_BOOT_COMPLETED) {
            const Int64 now = SystemClock::GetUptimeMillis();
            mBootCompleted = TRUE;
            mDirty |= DIRTY_BOOT_COMPLETED;
            UserActivityNoUpdateLocked(
                    now, IPowerManager::USER_ACTIVITY_EVENT_OTHER, 0, IProcess::SYSTEM_UID);
            UpdatePowerStateLocked();
        }
    }
    return NOERROR;
}

ECode PowerManagerService::SystemReady(
    /* [in] */ IIAppOpsService* appOps)
{
    {    AutoLock syncLock(mLock);
        mSystemReady = TRUE;
        mAppOps = appOps;
        AutoPtr<IInterface> obj = GetLocalService(EIID_IDreamManagerInternal);
        mDreamManager = IDreamManagerInternal::Probe(obj);
        obj = GetLocalService(EIID_IDisplayManagerInternal);
        mDisplayManagerInternal = IDisplayManagerInternal::Probe(obj);
        obj = GetLocalService(EIID_IWindowManagerPolicy);
        mPolicy = IWindowManagerPolicy::Probe(obj);
        obj = GetLocalService(EIID_IBatteryManagerInternal);
        mBatteryManagerInternal = IBatteryManagerInternal::Probe(obj);

        obj = NULL;
        ASSERT_SUCCEEDED(mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj));
        AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
        pm->GetMinimumScreenBrightnessSetting(&mScreenBrightnessSettingMinimum);
        pm->GetMaximumScreenBrightnessSetting(&mScreenBrightnessSettingMaximum);
        pm->GetDefaultScreenBrightnessSetting(&mScreenBrightnessSettingDefault);
        pm->GetDefaultButtonBrightness(&mButtonBrightnessSettingDefault);
        pm->GetDefaultKeyboardBrightness(&mKeyboardBrightnessSettingDefault);

        AutoPtr<ILooper> looper;
        mHandler->GetLooper((ILooper**)&looper);
        AutoPtr<ISystemSensorManager> systemSensorManager;
        CSystemSensorManager::New(mContext, looper, (ISystemSensorManager**)&systemSensorManager);
        AutoPtr<ISensorManager> sensorManager = ISensorManager::Probe(systemSensorManager);

        // The notifier runs on the system server's main looper so as not to interfere
        // with the animations and other critical functions of the power manager.
        mBatteryStats = BatteryStatsService::GetService();
        AutoPtr<ILooperHelper> helper;
        CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
        AutoPtr<ILooper> lop;
        helper->GetMainLooper((ILooper**)&lop);
        mNotifier = new Notifier(lop, mContext, mBatteryStats,
                mAppOps, CreateSuspendBlockerLocked(String("PowerManagerService.Broadcasts")),
                mPolicy);

        mWirelessChargerDetector = new WirelessChargerDetector(sensorManager,
                CreateSuspendBlockerLocked(String("PowerManagerService.WirelessChargerDetector")),
                mHandler);
        mSettingsObserver = new SettingsObserver();
        mSettingsObserver->constructor(mHandler, this);

        obj = GetLocalService(EIID_ILightsManager);
        AutoPtr<ILightsManager> manager = ILightsManager::Probe(obj);
        mLightsManager = (LightsManager*)manager.Get();
        mAttentionLight = mLightsManager->GetLight(LightsManager::LIGHT_ID_ATTENTION);
        mButtonsLight = mLightsManager->GetLight(LightsManager::LIGHT_ID_BUTTONS);
        mKeyboardLight = mLightsManager->GetLight(LightsManager::LIGHT_ID_KEYBOARD);
        mCapsLight = mLightsManager->GetLight(LightsManager::LIGHT_ID_CAPS);
        mFnLight = mLightsManager->GetLight(LightsManager::LIGHT_ID_FUNC);

        // Initialize display power management.
        mDisplayManagerInternal->InitPowerManagement(
                mDisplayPowerCallbacks, mHandler, sensorManager);

        // Register for broadcasts from other components of the system.
        AutoPtr<IIntentFilter> filter;
        ASSERT_SUCCEEDED(CIntentFilter::New((IIntentFilter**)&filter));
        filter->AddAction(IIntent::ACTION_BATTERY_CHANGED);
        filter->SetPriority(IIntentFilter::SYSTEM_HIGH_PRIORITY);
        AutoPtr<IBroadcastReceiver> batteryR = (IBroadcastReceiver*)new BatteryReceiver(this);
        AutoPtr<IIntent> intent;
        mContext->RegisterReceiver(batteryR, filter, String(NULL), mHandler, (IIntent**)&intent);

        filter = NULL;
        ASSERT_SUCCEEDED(CIntentFilter::New((IIntentFilter**)&filter));
        filter->AddAction(IIntent::ACTION_DREAMING_STARTED);
        filter->AddAction(IIntent::ACTION_DREAMING_STOPPED);
        AutoPtr<IBroadcastReceiver> dreamR = (IBroadcastReceiver*)new DreamReceiver(this);
        intent = NULL;
        mContext->RegisterReceiver(dreamR, filter, String(NULL), mHandler, (IIntent**)&intent);

        filter = NULL;
        ASSERT_SUCCEEDED(CIntentFilter::New((IIntentFilter**)&filter));
        filter->AddAction(IIntent::ACTION_USER_SWITCHED);
        AutoPtr<IBroadcastReceiver> userR = (IBroadcastReceiver*)new UserSwitchedReceiver(this);
        intent = NULL;
        mContext->RegisterReceiver(userR, filter, String(NULL), mHandler, (IIntent**)&intent);

        filter = NULL;
        ASSERT_SUCCEEDED(CIntentFilter::New((IIntentFilter**)&filter));
        filter->AddAction(IIntent::ACTION_DOCK_EVENT);
        AutoPtr<IBroadcastReceiver> dockR = (IBroadcastReceiver*)new DockReceiver(this);
        intent = NULL;
        mContext->RegisterReceiver(dockR, filter, String(NULL), mHandler, (IIntent**)&intent);

        // Register for settings changes.
        AutoPtr<IContentResolver> resolver;
        ASSERT_SUCCEEDED(mContext->GetContentResolver((IContentResolver**)&resolver));

        AutoPtr<IUri> uri;
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);

        settingsSecure->GetUriFor(ISettingsSecure::SCREENSAVER_ENABLED, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        uri = NULL;
        settingsSecure->GetUriFor(ISettingsSecure::SCREENSAVER_ACTIVATE_ON_SLEEP, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        uri = NULL;
        settingsSecure->GetUriFor(ISettingsSecure::SCREENSAVER_ACTIVATE_ON_DOCK, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        AutoPtr<ISettingsSystem> settingsSystem;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
        uri = NULL;
        settingsSystem->GetUriFor(ISettingsSystem::SCREEN_OFF_TIMEOUT, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        uri = NULL;
        settingsSecure->GetUriFor(ISettingsSecure::SLEEP_TIMEOUT, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        AutoPtr<ISettingsGlobal> settingsGlobal;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
        uri = NULL;
        settingsGlobal->GetUriFor(ISettingsGlobal::STAY_ON_WHILE_PLUGGED_IN, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        uri = NULL;
        settingsSystem->GetUriFor(ISettingsSystem::SCREEN_BRIGHTNESS, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        uri = NULL;
        settingsSystem->GetUriFor(ISettingsSystem::SCREEN_BRIGHTNESS_MODE, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        uri = NULL;
        settingsSystem->GetUriFor(ISettingsSystem::SCREEN_AUTO_BRIGHTNESS_ADJ, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        uri = NULL;
        settingsGlobal->GetUriFor(ISettingsGlobal::LOW_POWER_MODE, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        uri = NULL;
        settingsGlobal->GetUriFor(ISettingsGlobal::LOW_POWER_MODE_TRIGGER_LEVEL, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        uri = NULL;
        settingsSystem->GetUriFor(ISettingsSystem::PROXIMITY_ON_WAKE, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        uri = NULL;
        settingsSecure->GetUriFor(ISettingsSecure::BUTTON_BRIGHTNESS, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        uri = NULL;
        settingsSecure->GetUriFor(ISettingsSecure::KEYBOARD_BRIGHTNESS, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        uri = NULL;
        settingsSecure->GetUriFor(ISettingsSecure::BUTTON_BACKLIGHT_TIMEOUT, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        uri = NULL;
        settingsGlobal->GetUriFor(ISettingsGlobal::WAKE_WHEN_PLUGGED_OR_UNPLUGGED, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE,
                (IContentObserver*)mSettingsObserver.Get(), IUserHandle::USER_ALL);

        // Go.
        ReadConfigurationLocked();
        UpdateSettingsLocked();
        mDirty |= DIRTY_BATTERY_STATE;
        UpdatePowerStateLocked();
    }
    return NOERROR;
}

void PowerManagerService::ReadConfigurationLocked()
{
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);

    resources->GetBoolean(
            R::bool_::config_powerDecoupleAutoSuspendModeFromDisplay,
            &mDecoupleHalAutoSuspendModeFromDisplayConfig);
    resources->GetBoolean(
            R::bool_::config_powerDecoupleInteractiveModeFromDisplay,
            &mDecoupleHalInteractiveModeFromDisplayConfig);

    resources->GetBoolean(R::bool_::config_unplugTurnsOnScreen,
            &mWakeUpWhenPluggedOrUnpluggedConfig);
    resources->GetBoolean(
            R::bool_::config_suspendWhenScreenOffDueToProximity,
            &mSuspendWhenScreenOffDueToProximityConfig);
    resources->GetBoolean(R::bool_::config_dreamsSupported,
            &mDreamsSupportedConfig);
    resources->GetBoolean(R::bool_::config_dreamsEnabledByDefault,
            &mDreamsEnabledByDefaultConfig);
    resources->GetBoolean(R::bool_::config_dreamsActivatedOnSleepByDefault,
            &mDreamsActivatedOnSleepByDefaultConfig);
    resources->GetBoolean(R::bool_::config_dreamsActivatedOnDockByDefault,
            &mDreamsActivatedOnDockByDefaultConfig);
    resources->GetBoolean(
            R::bool_::config_dreamsEnabledOnBattery,
            &mDreamsEnabledOnBatteryConfig);
    resources->GetInteger(
            R::integer::config_dreamsBatteryLevelMinimumWhenPowered,
            &mDreamsBatteryLevelMinimumWhenPoweredConfig);
    resources->GetInteger(
            R::integer::config_dreamsBatteryLevelMinimumWhenNotPowered,
            &mDreamsBatteryLevelMinimumWhenNotPoweredConfig);
    resources->GetInteger(
            R::integer::config_dreamsBatteryLevelDrainCutoff,
            &mDreamsBatteryLevelDrainCutoffConfig);
    resources->GetBoolean(
            R::bool_::config_dozeAfterScreenOff,
            &mDozeAfterScreenOffConfig);
    resources->GetInteger(
            R::integer::config_minimumScreenOffTimeout,
            &mMinimumScreenOffTimeoutConfig);
    resources->GetInteger(
            R::integer::config_maximumScreenDimDuration,
            &mMaximumScreenDimDurationConfig);
    resources->GetFraction(
            R::fraction::config_maximumScreenDimRatio, 1, 1,
            &mMaximumScreenDimRatioConfig);
    resources->GetInteger(
            R::integer::config_proximityCheckTimeout,
            &mProximityTimeOut);
    resources->GetBoolean(
            R::bool_::config_proximityCheckOnWake,
            &mProximityWakeSupported);
    if (mProximityWakeSupported) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
        AutoPtr<IPowerManager> powerManager = IPowerManager::Probe(service);
        powerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK,
                String("ProximityWakeLock"), (IPowerManagerWakeLock**)&mProximityWakeLock);
    }
}

void PowerManagerService::UpdateSettingsLocked()
{
    AutoPtr<IContentResolver> resolver;
    ASSERT_SUCCEEDED(mContext->GetContentResolver((IContentResolver**)&resolver));

    Int32 value;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetInt32ForUser(resolver,
            ISettingsSecure::SCREENSAVER_ENABLED,
            mDreamsEnabledByDefaultConfig ? 1 : 0, IUserHandle::USER_CURRENT, &value);
    mDreamsEnabledSetting = value != 0;
    settingsSecure->GetInt32ForUser(resolver,
            ISettingsSecure::SCREENSAVER_ACTIVATE_ON_SLEEP,
            mDreamsActivatedOnSleepByDefaultConfig ? 1 : 0,
            IUserHandle::USER_CURRENT, &value);
    mDreamsActivateOnSleepSetting = value != 0;
    settingsSecure->GetInt32ForUser(resolver,
            ISettingsSecure::SCREENSAVER_ACTIVATE_ON_DOCK,
            mDreamsActivatedOnDockByDefaultConfig ? 1 : 0,
            IUserHandle::USER_CURRENT, &value);
    mDreamsActivateOnDockSetting = value != 0;

    AutoPtr<ISettingsSystem> settingsSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
    settingsSystem->GetInt32ForUser(resolver,
            ISettingsSystem::SCREEN_OFF_TIMEOUT,
            DEFAULT_SCREEN_OFF_TIMEOUT, IUserHandle::USER_CURRENT,
            &mScreenOffTimeoutSetting);

    settingsSecure->GetInt32ForUser(resolver,
            ISettingsSecure::SLEEP_TIMEOUT,
            DEFAULT_SLEEP_TIMEOUT, IUserHandle::USER_CURRENT,
            &mSleepTimeoutSetting);

    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetInt32(resolver,
            ISettingsGlobal::STAY_ON_WHILE_PLUGGED_IN,
            IBatteryManager::BATTERY_PLUGGED_AC, &mStayOnWhilePluggedInSetting);

    settingsGlobal->GetInt32(resolver,
            ISettingsGlobal::WAKE_WHEN_PLUGGED_OR_UNPLUGGED,
            (mWakeUpWhenPluggedOrUnpluggedConfig ? 1 : 0), &mWakeUpWhenPluggedOrUnpluggedSetting);
    settingsSystem->GetInt32(resolver,
            ISettingsSystem::PROXIMITY_ON_WAKE, 0, &value);
    mProximityWakeEnabled = value == 1;

    Int32 oldScreenBrightnessSetting = mScreenBrightnessSetting;
    settingsSystem->GetInt32ForUser(resolver,
            ISettingsSystem::SCREEN_BRIGHTNESS,
            mScreenBrightnessSettingDefault, IUserHandle::USER_CURRENT,
            &mScreenBrightnessSetting);
    if (oldScreenBrightnessSetting != mScreenBrightnessSetting) {
        mTemporaryScreenBrightnessSettingOverride = -1;
    }

    Float oldScreenAutoBrightnessAdjustmentSetting = mScreenAutoBrightnessAdjustmentSetting;
    settingsSystem->GetFloatForUser(resolver,
            ISettingsSystem::SCREEN_AUTO_BRIGHTNESS_ADJ, 0.0f,
            IUserHandle::USER_CURRENT, &mScreenAutoBrightnessAdjustmentSetting);
    if (oldScreenAutoBrightnessAdjustmentSetting != mScreenAutoBrightnessAdjustmentSetting) {
        mTemporaryScreenAutoBrightnessAdjustmentSettingOverride = Elastos::Core::Math::FLOAT_NAN;
    }

    settingsSystem->GetInt32ForUser(resolver,
            ISettingsSystem::SCREEN_BRIGHTNESS_MODE,
            ISettingsSystem::SCREEN_BRIGHTNESS_MODE_MANUAL,
            IUserHandle::USER_CURRENT, &mScreenBrightnessModeSetting);

    settingsGlobal->GetInt32(resolver,
            ISettingsGlobal::LOW_POWER_MODE, 0,
            &value);
    Boolean lowPowerModeEnabled = value != 0;

    settingsGlobal->GetInt32(resolver,
            ISettingsGlobal::LOW_POWER_MODE_TRIGGER_LEVEL, 0,
            &value);
    Boolean autoLowPowerModeConfigured = value != 0;

    if (lowPowerModeEnabled != mLowPowerModeSetting
            || autoLowPowerModeConfigured != mAutoLowPowerModeConfigured) {
        mLowPowerModeSetting = lowPowerModeEnabled;
        mAutoLowPowerModeConfigured = autoLowPowerModeConfigured;
        UpdateLowPowerModeLocked();
    }

    settingsSecure->GetInt32ForUser(resolver,
            ISettingsSecure::BUTTON_BACKLIGHT_TIMEOUT,
            DEFAULT_BUTTON_ON_DURATION, IUserHandle::USER_CURRENT, &mButtonTimeout);

    settingsSecure->GetInt32ForUser(resolver,
            ISettingsSecure::BUTTON_BRIGHTNESS,
            mButtonBrightnessSettingDefault, IUserHandle::USER_CURRENT, &mButtonBrightness);
    settingsSecure->GetInt32ForUser(resolver,
            ISettingsSecure::KEYBOARD_BRIGHTNESS,
            mKeyboardBrightnessSettingDefault, IUserHandle::USER_CURRENT, &mKeyboardBrightness);

    mDirty |= DIRTY_SETTINGS;
}

void PowerManagerService::UpdateLowPowerModeLocked()
{
    Boolean res;
    if (mIsPowered && mLowPowerModeSetting) {
        if (DEBUG_SPEW) {
            Slogger::D(TAG, "updateLowPowerModeLocked: powered, turning setting off");
        }
        // Turn setting off if powered
        AutoPtr<ISettingsGlobal> settingsGlobal;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
        AutoPtr<IContentResolver> resolver;
        ASSERT_SUCCEEDED(mContext->GetContentResolver((IContentResolver**)&resolver));
        settingsGlobal->PutInt32(resolver,
                ISettingsGlobal::LOW_POWER_MODE, 0, &res);
        mLowPowerModeSetting = FALSE;
    }
    const Boolean autoLowPowerModeEnabled = !mIsPowered && mAutoLowPowerModeConfigured
            && !mAutoLowPowerModeSnoozing && mBatteryLevelLow;
    const Boolean lowPowerModeEnabled = mLowPowerModeSetting || autoLowPowerModeEnabled;

    if (mLowPowerModeEnabled != lowPowerModeEnabled) {
        mLowPowerModeEnabled = lowPowerModeEnabled;
        PowerHintInternal(POWER_HINT_LOW_POWER, lowPowerModeEnabled ? 1 : 0);
        AutoPtr<UpdateLowPowerModeLockedRunnable> runnable = new UpdateLowPowerModeLockedRunnable(this, lowPowerModeEnabled);
        AutoPtr<IBackgroundThreadHelper> helper;
        CBackgroundThreadHelper::AcquireSingleton((IBackgroundThreadHelper**)&helper);
        AutoPtr<IHandler> handler;
        helper->GetHandler((IHandler**)&handler);
        handler->Post(runnable, &res);
    }
}

void PowerManagerService::HandleSettingsChangedLocked()
{
    UpdateSettingsLocked();
    UpdatePowerStateLocked();
}

ECode PowerManagerService::AcquireWakeLockInternal(
    /* [in] */ IBinder* lock,
    /* [in] */ Int32 flags,
    /* [in] */ const String& tag,
    /* [in] */ const String& packageName,
    /* [in] */ IWorkSource* ws,
    /* [in] */ const String& historyTag,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid)
{
    {
        AutoLock syncLock(mLock);
        AutoPtr<IInteger32> integer;
        CInteger32::New(uid, (IInteger32**)&integer);
        Boolean contains;
        if((mBlockedUids->Contains(integer, &contains), contains) && uid != Process::MyUid()) {
            //wakelock acquisition for blocked uid, do not acquire.
            if (DEBUG_SPEW) {
                Slogger::D(TAG, "uid is blocked not acquiring wakeLock flags=0x%08x tag=%s uid=%d pid =%d",
                   flags, tag.string(), uid, pid);
            }
            return NOERROR;
        }

        if (DEBUG_SPEW) {
            Slogger::D(TAG, "acquireWakeLockInternal: lock=%d, flags=0x%08x, tag=%s, ws=%s, uid=%d, pid=%d",
                Objects::GetHashCode(lock), flags, tag.string(), TO_CSTR(ws), uid, pid);
        }

        AutoPtr<WakeLock> wakeLock;
        Int32 index = FindWakeLockIndexLocked(lock);
        Boolean notifyAcquire;
        if (index >= 0) {
            AutoPtr<IInterface> obj;
            mWakeLocks->Get(index, (IInterface**)&obj);
            wakeLock = (WakeLock*)(IObject*)obj.Get();
            if (!wakeLock->HasSameProperties(flags, tag, ws, uid, pid)) {
                // Update existing wake lock.  This shouldn't happen but is harmless.
                NotifyWakeLockChangingLocked(wakeLock, flags, tag, packageName,
                        uid, pid, ws, historyTag);
                wakeLock->UpdateProperties(flags, tag, packageName, ws, historyTag, uid, pid);
            }
            notifyAcquire = FALSE;
        }
        else {
            wakeLock = new WakeLock(lock, flags, tag, packageName, ws, historyTag, uid, pid, this);
            AutoPtr<IProxy> proxy = (IProxy*)lock->Probe(EIID_IProxy);
            if (proxy != NULL && FAILED(proxy->LinkToDeath(wakeLock, 0))) {
                Slogger::E(TAG, "Wake lock is already dead.");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            mWakeLocks->Add((IObject*)wakeLock);
            notifyAcquire = TRUE;
        }

        ApplyWakeLockFlagsOnAcquireLocked(wakeLock, uid);
        mDirty |= DIRTY_WAKE_LOCKS;
        UpdatePowerStateLocked();
        if (notifyAcquire) {
            // This needs to be done last so we are sure we have acquired the
            // kernel wake lock.  Otherwise we have a race where the system may
            // go to sleep between the time we start the accounting in battery
            // stats and when we actually get around to telling the kernel to
            // stay awake.
            NotifyWakeLockAcquiredLocked(wakeLock);
        }
    }
    return NOERROR;
}

Boolean PowerManagerService::IsScreenLock(
    /* [in] */ WakeLock* wakeLock)
{
    switch (wakeLock->mFlags & IPowerManager::WAKE_LOCK_LEVEL_MASK) {
        case IPowerManager::FULL_WAKE_LOCK:
        case IPowerManager::SCREEN_BRIGHT_WAKE_LOCK:
        case IPowerManager::SCREEN_DIM_WAKE_LOCK:
            return TRUE;
    }
    return FALSE;
}

void PowerManagerService::ApplyWakeLockFlagsOnAcquireLocked(
    /* [in] */ WakeLock* wakeLock,
    /* [in] */ Int32 uid)
{
    if ((wakeLock->mFlags & IPowerManager::ACQUIRE_CAUSES_WAKEUP) != 0
            && IsScreenLock(wakeLock)) {
        WakeUpNoUpdateLocked(SystemClock::GetUptimeMillis(), uid);
    }
}

void PowerManagerService::ReleaseWakeLockInternal(
    /* [in] */ IBinder* lock,
    /* [in] */ Int32 flags)
{
    {
        AutoLock syncLock(mLock);
        Int32 index = FindWakeLockIndexLocked(lock);
        if (index < 0) {
            if (DEBUG_SPEW) {
                Slogger::D(TAG, "releaseWakeLockInternal: lock=%d [not found], flags=0x%08x",
                        Objects::GetHashCode(lock), flags);
            }
            return;
        }

        AutoPtr<IInterface> obj;
        mWakeLocks->Get(index, (IInterface**)&obj);
        AutoPtr<WakeLock> wakeLock = (WakeLock*)(IObject*)obj.Get();
        if (DEBUG_SPEW) {
            Slogger::D(TAG, "releaseWakeLockInternal: lock=%d [%s], flags=0x%08x",
                    Objects::GetHashCode(lock), wakeLock->mTag.string(), flags);
        }

        if ((flags & IPowerManager::RELEASE_FLAG_WAIT_FOR_NO_PROXIMITY) != 0) {
            mRequestWaitForNegativeProximity = TRUE;
        }

        AutoPtr<IProxy> proxy = (IProxy*)wakeLock->mLock->Probe(EIID_IProxy);
        if (proxy) {
            Boolean res;
            proxy->UnlinkToDeath(wakeLock, 0, &res);
        }
        RemoveWakeLockLocked(wakeLock, index);
    }
}

void PowerManagerService::HandleWakeLockDeath(
    /* [in] */ WakeLock* wakeLock)
{
    {
        AutoLock syncLock(mLock);
        if (DEBUG_SPEW) {
            Slogger::D(TAG, "handleWakeLockDeath: lock=%d [%s]",
                    Objects::GetHashCode(wakeLock->mLock), wakeLock->mTag.string());
        }

        Int32 index;
        mWakeLocks->IndexOf((IObject*)wakeLock, &index);
        if (index < 0) {
            return;
        }

        RemoveWakeLockLocked(wakeLock, index);
    }
}

void PowerManagerService::RemoveWakeLockLocked(
    /* [in] */ WakeLock* wakeLock,
    /* [in] */ Int32 index)
{
    mWakeLocks->Remove(index);
    NotifyWakeLockReleasedLocked(wakeLock);

    ApplyWakeLockFlagsOnReleaseLocked(wakeLock);
    mDirty |= DIRTY_WAKE_LOCKS;
    UpdatePowerStateLocked();
}

void PowerManagerService::ApplyWakeLockFlagsOnReleaseLocked(
    /* [in] */ WakeLock* wakeLock)
{
    if ((wakeLock->mFlags & IPowerManager::ON_AFTER_RELEASE) != 0
            && IsScreenLock(wakeLock)) {
        UserActivityNoUpdateLocked(SystemClock::GetUptimeMillis(),
                IPowerManager::USER_ACTIVITY_EVENT_OTHER,
                IPowerManager::USER_ACTIVITY_FLAG_NO_CHANGE_LIGHTS,
                wakeLock->mOwnerUid);
    }
}

ECode PowerManagerService::UpdateWakeLockWorkSourceInternal(
    /* [in] */IBinder* lock,
    /* [in] */IWorkSource* ws,
    /* [in] */ const String& historyTag,
    /* [in] */ Int32 callingUid)
{
    {
        AutoLock syncLock(mLock);
        Int32 index = FindWakeLockIndexLocked(lock);
        AutoPtr<ISystemProperties> sp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
        Int32 value;
        sp->GetInt32(String("persist.dpm.feature"), 0, &value);
        Boolean isNsrmEnabled = FALSE;

        if ((value & DPM_CONFIG_FEATURE_MASK_NSRM) == DPM_CONFIG_FEATURE_MASK_NSRM)
            isNsrmEnabled = TRUE;

        if (index < 0) {
            if (DEBUG_SPEW) {
                Slogger::D(TAG, "UpdateWakeLockWorkSourceInternal: lock=%d [not found], ws=%s",
                        Objects::GetHashCode(lock), TO_CSTR(ws));
            }
            if (!isNsrmEnabled) {
                Slogger::E(TAG, "Wake lock not active: %p from uid %d", lock, callingUid);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            else {
                return NOERROR;
            }
        }

        AutoPtr<IInterface> obj;
        mWakeLocks->Get(index, (IInterface**)&obj);
        AutoPtr<WakeLock> wakeLock = (WakeLock*)(IObject*)obj.Get();
        if (DEBUG_SPEW) {
            Slogger::D(TAG, "UpdateWakeLockWorkSourceInternal: lock=%d [%s], ws=%s",
                    Objects::GetHashCode(lock), wakeLock->mTag.string(), TO_CSTR(ws));
        }

        if (!wakeLock->HasSameWorkSource(ws)) {
            NotifyWakeLockChangingLocked(wakeLock, wakeLock->mFlags, wakeLock->mTag,
                    wakeLock->mPackageName, wakeLock->mOwnerUid, wakeLock->mOwnerPid,
                    ws, historyTag);
            wakeLock->mHistoryTag = historyTag;
            wakeLock->UpdateWorkSource(ws);
        }
        return NOERROR;
    }
    return NOERROR;
}

Boolean PowerManagerService::CheckWorkSourceObjectId(
    /* [in] */ Int32 uid,
    /* [in] */ WakeLock* wl)
{
    // try {
    Int32 size;
    wl->mWorkSource->GetSize(&size);
    for (Int32 index = 0; index < size; index++) {
        Int32 value;
        if (wl->mWorkSource->Get(index, &value), uid == value) {
            if (DEBUG_SPEW) Slogger::V(TAG, "WS uid matched");
            return TRUE;
        }
    }
    // }
    // catch (Exception e) {
    //     return false;
    // }
    return FALSE;
}

Int32 PowerManagerService::FindWakeLockIndexLocked(
    /* [in] */IBinder* lock)
{
    Int32 count;
    mWakeLocks->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        mWakeLocks->Get(i, (IInterface**)&obj);
        AutoPtr<WakeLock> wakeLock = (WakeLock*)(IObject*)obj.Get();
        if (IBinder::Probe(wakeLock->mLock) == IBinder::Probe(lock)) {
            return i;
        }
    }

    return -1;
}

void PowerManagerService::NotifyWakeLockAcquiredLocked(
    /* [in] */ WakeLock* wakeLock)
{
    if (mSystemReady) {
        wakeLock->mNotifiedAcquired = TRUE;
        mNotifier->OnWakeLockAcquired(wakeLock->mFlags, wakeLock->mTag, wakeLock->mPackageName,
                wakeLock->mOwnerUid, wakeLock->mOwnerPid, wakeLock->mWorkSource,
                wakeLock->mHistoryTag);
    }
}

void PowerManagerService::NotifyWakeLockChangingLocked(
    /* [in] */ WakeLock* wakeLock,
    /* [in] */ Int32 flags,
    /* [in] */ const String& tag,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ IWorkSource* ws,
    /* [in] */ const String& historyTag)
{
    if (mSystemReady && wakeLock->mNotifiedAcquired) {
        mNotifier->OnWakeLockChanging(wakeLock->mFlags, wakeLock->mTag, wakeLock->mPackageName,
                wakeLock->mOwnerUid, wakeLock->mOwnerPid, wakeLock->mWorkSource,
                wakeLock->mHistoryTag, flags, tag, packageName, uid, pid, ws, historyTag);
    }
}

void PowerManagerService::NotifyWakeLockReleasedLocked(
    /* [in] */ WakeLock* wakeLock)
{
    if (mSystemReady && wakeLock->mNotifiedAcquired) {
        wakeLock->mNotifiedAcquired = FALSE;
        mNotifier->OnWakeLockReleased(wakeLock->mFlags, wakeLock->mTag,
                wakeLock->mPackageName, wakeLock->mOwnerUid, wakeLock->mOwnerPid,
                wakeLock->mWorkSource, wakeLock->mHistoryTag);
    }
}

Boolean PowerManagerService::IsWakeLockLevelSupportedInternal(
    /* [in] */ Int32 level)
{
    {
        AutoLock syncLock(mLock);
        switch (level) {
            case IPowerManager::PARTIAL_WAKE_LOCK:
            case IPowerManager::SCREEN_DIM_WAKE_LOCK:
            case IPowerManager::SCREEN_BRIGHT_WAKE_LOCK:
            case IPowerManager::FULL_WAKE_LOCK:
            case IPowerManager::DOZE_WAKE_LOCK:
                return TRUE;

            case IPowerManager::PROXIMITY_SCREEN_OFF_WAKE_LOCK: {
                Boolean res;
                mDisplayManagerInternal->IsProximitySensorAvailable(&res);
                return mSystemReady && res;
            }

            default:
                return FALSE;
        }
    }
    return FALSE;
}

Boolean PowerManagerService::IsQuickBootCall()
{
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&service);
    AutoPtr<IActivityManager> activityManager = IActivityManager::Probe(service);

    AutoPtr<IList> runningList;
    activityManager->GetRunningAppProcesses((IList**)&runningList);
    Int32 callingPid = Binder::GetCallingPid();
    AutoPtr<IIterator> iterator;
    runningList->GetIterator((IIterator**)&iterator);
    Boolean hasNext;
    while (iterator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        iterator->GetNext((IInterface**)&value);
        AutoPtr<IActivityManagerRunningAppProcessInfo> processInfo = IActivityManagerRunningAppProcessInfo::Probe(value);
        Int32 pid;
        if (processInfo->GetPid(&pid), pid == callingPid) {
            String process;
            processInfo->GetProcessName(&process);
            if (process.Equals("com.qapp.quickboot"))
                return TRUE;
        }
    }
    return FALSE;
}

void PowerManagerService::UserActivityFromNative(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 event,
    /* [in] */ Int32 flags)
{
    UserActivityInternal(eventTime, event, flags, IProcess::SYSTEM_UID);
}

void PowerManagerService::UserActivityInternal(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 event,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 uid)
{
    AutoLock syncLock(mLock);
    if (UserActivityNoUpdateLocked(eventTime, event, flags, uid)) {
        UpdatePowerStateLocked();
    }
}

Boolean PowerManagerService::UserActivityNoUpdateLocked(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 event,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 uid)
{
    if (DEBUG_SPEW) {
        Slogger::D(TAG, "enter userActivityNoUpdateLocked: eventTime=%lld, event=%d, flags=0x%08x, uid=%d",
                eventTime, event, flags, uid);
    }

    if (eventTime < mLastSleepTime || eventTime < mLastWakeTime
            || !mBootCompleted || !mSystemReady) {
        return FALSE;
    }

    // Trace.traceBegin(Trace.TRACE_TAG_POWER, "userActivity"); // TODO
    // try {
    if (eventTime > mLastInteractivePowerHintTime) {
        PowerHintInternal(POWER_HINT_INTERACTION, 0);
        mLastInteractivePowerHintTime = eventTime;
    }

    mNotifier->OnUserActivity(event, uid);

    if (mWakefulness == WAKEFULNESS_ASLEEP
            || mWakefulness == WAKEFULNESS_DOZING
            || (flags & IPowerManager::USER_ACTIVITY_FLAG_INDIRECT) != 0) {
        return FALSE;
    }

    if ((flags & IPowerManager::USER_ACTIVITY_FLAG_NO_CHANGE_LIGHTS) != 0) {
        if (eventTime > mLastUserActivityTimeNoChangeLights
                && eventTime > mLastUserActivityTime) {
            mLastUserActivityTimeNoChangeLights = eventTime;
            mDirty |= DIRTY_USER_ACTIVITY;
            return TRUE;
        }
    }
    else {
        if (eventTime > mLastUserActivityTime) {
            mLastUserActivityTime = eventTime;
            mDirty |= DIRTY_USER_ACTIVITY;
            return TRUE;
        }
    }

    if (DEBUG_SPEW) {
        Slogger::D(TAG, "exit userActivityNoUpdateLocked: mLastUserActivityTime=%lld",
                mLastUserActivityTime);
    }
    // } finally {
    // Trace.traceEnd(Trace.TRACE_TAG_POWER);
    // }

    return FALSE;
}

void PowerManagerService::WakeUpInternal(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 uid)
{
    {
        AutoLock syncLock(mLock);
        if (WakeUpNoUpdateLocked(eventTime, uid)) {
            UpdatePowerStateLocked();
        }
    }
}

Boolean PowerManagerService::WakeUpNoUpdateLocked(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 uid)
{
    if (DEBUG_SPEW) {
        Slogger::D(TAG, "wakeUpNoUpdateLocked: eventTime=%lld, uid=%d", eventTime, uid);
    }

    if (eventTime < mLastSleepTime || mWakefulness == WAKEFULNESS_AWAKE
            || !mBootCompleted || !mSystemReady) {
        return FALSE;
    }

    // Trace.traceBegin(Trace.TRACE_TAG_POWER, "wakeUp");
    // try {
    switch (mWakefulness) {
        case WAKEFULNESS_ASLEEP:
            Slogger::I(TAG, "Waking up from sleep (uid %d)...", uid);
            break;
        case WAKEFULNESS_DREAMING:
            Slogger::I(TAG, "Waking up from dream (uid %d)...", uid);
            break;
        case WAKEFULNESS_DOZING:
            Slogger::I(TAG, "Waking up from dozing (uid %d)...", uid);
            break;
    }

    mLastWakeTime = eventTime;
    mDirty |= DIRTY_WAKEFULNESS;
    mWakefulness = WAKEFULNESS_AWAKE;
    SetInteractiveStateLocked(TRUE, 0);

    UserActivityNoUpdateLocked(
            eventTime, IPowerManager::USER_ACTIVITY_EVENT_OTHER, 0, uid);
    // } finally {
    // Trace.traceEnd(Trace.TRACE_TAG_POWER);
    // }
    return TRUE;
}

void PowerManagerService::EnableQbCharger(
    /* [in] */ Boolean enable)
{
    AutoPtr<ISystemProperties> props;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&props);
    Int32 value, value1;
    if ((props->GetInt32(String("sys.quickboot.enable"), 0, &value), value == 1) &&
            (props->GetInt32(String("sys.quickboot.poweroff"), 0, &value1), value1 != 1)) {
        // only handle "charged" event, qbcharger process will handle
        // "uncharged" event itself
        if (enable && mIsPowered && !IsInteractiveInternal()) {
            props->Set(String("sys.qbcharger.enable"), String("true"));
        }
    }
}

void PowerManagerService::GoToSleepInternal(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 reason,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 uid)
{
    {
        AutoLock syncLock(mLock);
        if (GoToSleepNoUpdateLocked(eventTime, reason, flags, uid)) {
            UpdatePowerStateLocked();
        }
    }
}

Boolean PowerManagerService::GoToSleepNoUpdateLocked(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 reason,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 uid)
{
    if (DEBUG_SPEW) {
        Slogger::D(TAG, "goToSleepNoUpdateLocked: eventTime=%lld, reason=%d, flags=%d, uid=%d",
                eventTime, reason, flags, uid);
    }

    if (eventTime < mLastWakeTime
            || mWakefulness == WAKEFULNESS_ASLEEP
            || mWakefulness == WAKEFULNESS_DOZING
            || !mBootCompleted || !mSystemReady) {
        return FALSE;
    }

    // Trace.traceBegin(Trace.TRACE_TAG_POWER, "goToSleep");
    // try {
    switch (reason) {
        case IPowerManager::GO_TO_SLEEP_REASON_DEVICE_ADMIN:
            Slogger::I(TAG, "Going to sleep due to device administration policy (uid %d)...", uid);
            break;
        case IPowerManager::GO_TO_SLEEP_REASON_TIMEOUT:
            Slogger::I(TAG, "Going to sleep due to screen timeout (uid %d)...", uid);
            break;
        case IPowerManager::GO_TO_SLEEP_REASON_LID_SWITCH:
            Slogger::I(TAG, "Going to sleep due to lid switch (uid %d)...", uid);
            break;
        case IPowerManager::GO_TO_SLEEP_REASON_POWER_BUTTON:
            Slogger::I(TAG, "Going to sleep due to power button (uid %d)...", uid);
            break;
        case IPowerManager::GO_TO_SLEEP_REASON_HDMI:
            Slogger::I(TAG, "Going to sleep due to HDMI standby (uid %d)...", uid);
            break;
        default:
            Slogger::I(TAG, "Going to sleep by application request (uid %d)...", uid);
            reason = IPowerManager::GO_TO_SLEEP_REASON_APPLICATION;
            break;
    }

    mLastSleepTime = eventTime;
    mDirty |= DIRTY_WAKEFULNESS;
    mWakefulness = WAKEFULNESS_DOZING;
    mSandmanSummoned = TRUE;
    SetInteractiveStateLocked(FALSE, reason);

    // Report the number of wake locks that will be cleared by going to sleep.
    Int32 numWakeLocksCleared = 0;
    Int32 numWakeLocks;
    mWakeLocks->GetSize(&numWakeLocks);
    for (Int32 i = 0; i < numWakeLocks; i++) {
        AutoPtr<IInterface> obj;
        mWakeLocks->Get(i, (IInterface**)&obj);
        AutoPtr<WakeLock> wakeLock = (WakeLock*)(IObject*)obj.Get();
        switch (wakeLock->mFlags & IPowerManager::WAKE_LOCK_LEVEL_MASK) {
            case IPowerManager::FULL_WAKE_LOCK:
            case IPowerManager::SCREEN_BRIGHT_WAKE_LOCK:
            case IPowerManager::SCREEN_DIM_WAKE_LOCK:
                numWakeLocksCleared += 1;
                break;
        }
    }
    // EventLog.writeEvent(EventLogTags.POWER_SLEEP_REQUESTED, numWakeLocksCleared);

    // Skip dozing if requested.
    if ((flags & IPowerManager::GO_TO_SLEEP_FLAG_NO_DOZE) != 0) {
        ReallyGoToSleepNoUpdateLocked(eventTime, uid);
    }
    // } finally {
    //     Trace.traceEnd(Trace.TRACE_TAG_POWER);
    // }

    return TRUE;
}

void PowerManagerService::NapInternal(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 uid)
{
    {    AutoLock syncLock(mLock);
        if (NapNoUpdateLocked(eventTime, uid)) {
            UpdatePowerStateLocked();
        }
    }
}

Boolean PowerManagerService::NapNoUpdateLocked(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 uid)
{
    if (DEBUG_SPEW) {
        Slogger::D(TAG, "napNoUpdateLocked: eventTime=%lld, uid=%d", eventTime, uid);
    }

    if (eventTime < mLastWakeTime || mWakefulness != WAKEFULNESS_AWAKE
            || !mBootCompleted || !mSystemReady) {
        return FALSE;
    }

    // Trace.traceBegin(Trace.TRACE_TAG_POWER, "nap");
    // try {
    Slogger::I(TAG, "Nap time (uid %d)...", uid);

    mDirty |= DIRTY_WAKEFULNESS;
    mWakefulness = WAKEFULNESS_DREAMING;
    mSandmanSummoned = TRUE;
    SetInteractiveStateLocked(TRUE, 0);
    // } finally {
    //     Trace.traceEnd(Trace.TRACE_TAG_POWER);
    // }
    return TRUE;
}

Boolean PowerManagerService::ReallyGoToSleepNoUpdateLocked(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 uid)
{
    if (DEBUG_SPEW) {
        Slogger::D(TAG, "reallyGoToSleepNoUpdateLocked: eventTime=%lld, uid=%d", eventTime, uid);
    }

    if (eventTime < mLastWakeTime || mWakefulness == WAKEFULNESS_ASLEEP
            || !mBootCompleted || !mSystemReady) {
        return FALSE;
    }

    // Trace.traceBegin(Trace.TRACE_TAG_POWER, "reallyGoToSleep");
    // try {
    Slogger::I(TAG, "Sleeping (uid %d)...", uid);

    mDirty |= DIRTY_WAKEFULNESS;
    mWakefulness = WAKEFULNESS_ASLEEP;
    SetInteractiveStateLocked(FALSE, IPowerManager::GO_TO_SLEEP_REASON_TIMEOUT);
    // } finally {
    // Trace.traceEnd(Trace.TRACE_TAG_POWER);
    // }
    return TRUE;
}

void PowerManagerService::SetInteractiveStateLocked(
    /* [in] */ Boolean interactive,
    /* [in] */ Int32 reason)
{
    if (mInteractive != interactive) {
        FinishInteractiveStateChangeLocked();

        mInteractive = interactive;
        mInteractiveChanging = TRUE;
        mNotifier->OnInteractiveStateChangeStarted(interactive, reason);
    }
}

void PowerManagerService::FinishInteractiveStateChangeLocked()
{
    if (mInteractiveChanging) {
        mNotifier->OnInteractiveStateChangeFinished(mInteractive);
        mInteractiveChanging = FALSE;
    }
}

void PowerManagerService::UpdatePowerStateLocked()
{
    if (!mSystemReady || mDirty == 0) {
        return;
    }

    Boolean isLocked = TRUE;
    // TODO
    // isLocked = Thread::HoldsLock(&mLock;
    if (!isLocked) {
        Slogger::W(TAG, "Power manager lock was not held when calling updatePowerStateLocked");
    }

    // Trace.traceBegin(Trace.TRACE_TAG_POWER, "updatePowerState");
    // try {

    // Phase 0: Basic state updates.
    UpdateIsPoweredLocked(mDirty);
    UpdateStayOnLocked(mDirty);

    // Phase 1: Update wakefulness.
    // Loop because the wake lock and user activity computations are influenced
    // by changes in wakefulness.
    Int64 now = SystemClock::GetUptimeMillis();
    Int32 dirtyPhase2 = 0;
    for (;;) {
        Int32 dirtyPhase1 = mDirty;
        dirtyPhase2 |= dirtyPhase1;
        mDirty = 0;

        UpdateWakeLockSummaryLocked(dirtyPhase1);
        UpdateUserActivitySummaryLocked(now, dirtyPhase1);
        if (!UpdateWakefulnessLocked(dirtyPhase1)) {
            break;
        }
    }

    // Phase 2: Update display power state.
    Boolean displayBecameReady = UpdateDisplayPowerStateLocked(dirtyPhase2);

    // Phase 3: Update dream state (depends on display ready signal).
    UpdateDreamLocked(dirtyPhase2, displayBecameReady);

    // Phase 4: Send notifications, if needed.
    if (mDisplayReady) {
        FinishInteractiveStateChangeLocked();
    }

    // Phase 5: Update suspend blocker.
    // Because we might release the last suspend blocker here, we need to make sure
    // we finished everything else first!
    UpdateSuspendBlockerLocked();
    // } finally {
    // Trace.traceEnd(Trace.TRACE_TAG_POWER);
    // }
}

void PowerManagerService::UpdateIsPoweredLocked(
    /* [in] */ Int32 dirty)
{
    if ((dirty & DIRTY_BATTERY_STATE) != 0) {
        Boolean wasPowered = mIsPowered;
        Int32 oldPlugType = mPlugType;
        Boolean oldLevelLow = mBatteryLevelLow;
        mBatteryManagerInternal->IsPowered(IBatteryManager::BATTERY_PLUGGED_ANY, &mIsPowered);
        mBatteryManagerInternal->GetPlugType(&mPlugType);
        mBatteryManagerInternal->GetBatteryLevel(&mBatteryLevel);
        mBatteryManagerInternal->GetBatteryLevelLow(&mBatteryLevelLow);

        if (DEBUG_SPEW) {
            Slogger::D(TAG, "updateIsPoweredLocked: wasPowered=%d, mIsPowered=%d, oldPlugType=%d, mBatteryLevel=%d",
                wasPowered, mIsPowered, oldPlugType, mPlugType, mBatteryLevel);
        }

        EnableQbCharger(mIsPowered);
        if (wasPowered != mIsPowered || oldPlugType != mPlugType) {
            mDirty |= DIRTY_IS_POWERED;

            // Update wireless dock detection state.
            Boolean dockedOnWirelessCharger = mWirelessChargerDetector->Update(
                    mIsPowered, mPlugType, mBatteryLevel);

            // Treat plugging and unplugging the devices as a user activity.
            // Users find it disconcerting when they plug or unplug the device
            // and it shuts off right away.
            // Some devices also wake the device when plugged or unplugged because
            // they don't have a charging LED.
            Int64 now = SystemClock::GetUptimeMillis();
            if (ShouldWakeUpWhenPluggedOrUnpluggedLocked(wasPowered, oldPlugType, dockedOnWirelessCharger)) {
                WakeUpNoUpdateLocked(now, IProcess::SYSTEM_UID);
            }
            UserActivityNoUpdateLocked(
                    now, IPowerManager::USER_ACTIVITY_EVENT_OTHER, 0, IProcess::SYSTEM_UID);

            // Tell the notifier whether wireless charging has started so that
            // it can provide feedback to the user.
            if (dockedOnWirelessCharger) {
                mNotifier->OnWirelessChargingStarted();
            }
        }

        if (wasPowered != mIsPowered || oldLevelLow != mBatteryLevelLow) {
            if (oldLevelLow != mBatteryLevelLow && !mBatteryLevelLow) {
                if (DEBUG_SPEW) {
                    Slogger::D(TAG, "updateIsPoweredLocked: resetting low power snooze");
                }
                mAutoLowPowerModeSnoozing = FALSE;
            }
            UpdateLowPowerModeLocked();
        }
    }
}

Boolean PowerManagerService::ShouldWakeUpWhenPluggedOrUnpluggedLocked(
    /* [in] */ Boolean wasPowered,
    /* [in] */ Int32 oldPlugType,
    /* [in] */ Boolean dockedOnWirelessCharger)
{
    // Don't wake when powered unless configured to do so.
    if (mWakeUpWhenPluggedOrUnpluggedSetting == 0) {
        return FALSE;
    }

    AutoPtr<ISystemProperties> props;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&props);
    Int32 value;
    if (props->GetInt32(String("sys.quickboot.enable"), 0, &value), value == 1) {
        return FALSE;
    }

    // Don't wake when undocked from wireless charger.
    // See WirelessChargerDetector for justification.
    if (wasPowered && !mIsPowered
            && oldPlugType == IBatteryManager::BATTERY_PLUGGED_WIRELESS) {
        return FALSE;
    }

    // Don't wake when docked on wireless charger unless we are certain of it.
    // See WirelessChargerDetector for justification.
    if (!wasPowered && mIsPowered
            && mPlugType == IBatteryManager::BATTERY_PLUGGED_WIRELESS
            && !dockedOnWirelessCharger) {
        return FALSE;
    }

    // If already dreaming and becoming powered, then don't wake.
    if (mIsPowered && mWakefulness == WAKEFULNESS_DREAMING) {
        return FALSE;
    }

    // Otherwise wake up!
    return TRUE;
}

void PowerManagerService::UpdateStayOnLocked(
    /* [in] */ Int32 dirty)
{
    if ((dirty & (DIRTY_BATTERY_STATE | DIRTY_SETTINGS)) != 0) {
        Boolean wasStayOn = mStayOn;
        if (mStayOnWhilePluggedInSetting != 0
                && !IsMaximumScreenOffTimeoutFromDeviceAdminEnforcedLocked()) {
            if (mBatteryManagerInternal) {
                mBatteryManagerInternal->IsPowered(mStayOnWhilePluggedInSetting, &mStayOn);
            }
            else {
                Slogger::E(TAG, "UpdateStayOnLocked, mBatteryManagerInternal is %s", TO_CSTR(mBatteryManagerInternal));
            }
        }
        else {
            mStayOn = FALSE;
        }

        if (mStayOn != wasStayOn) {
            mDirty |= DIRTY_STAY_ON;
        }
    }
}

void PowerManagerService::UpdateWakeLockSummaryLocked(
    /* [in] */ Int32 dirty)
{
    if ((dirty & (DIRTY_WAKE_LOCKS | DIRTY_WAKEFULNESS)) != 0) {
        mWakeLockSummary = 0;

        Int32 numWakeLocks;
        mWakeLocks->GetSize(&numWakeLocks);
        for (Int32 i = 0; i < numWakeLocks; i++) {
            AutoPtr<IInterface> obj;
            mWakeLocks->Get(i, (IInterface**)&obj);
            AutoPtr<WakeLock> wakeLock = (WakeLock*)(IObject*)obj.Get();
            switch (wakeLock->mFlags & IPowerManager::WAKE_LOCK_LEVEL_MASK) {
                case IPowerManager::PARTIAL_WAKE_LOCK:
                    mWakeLockSummary |= WAKE_LOCK_CPU;
                    break;
                case IPowerManager::FULL_WAKE_LOCK:
                    mWakeLockSummary |= WAKE_LOCK_SCREEN_BRIGHT | WAKE_LOCK_BUTTON_BRIGHT;
                    break;
                case IPowerManager::SCREEN_BRIGHT_WAKE_LOCK:
                    mWakeLockSummary |= WAKE_LOCK_SCREEN_BRIGHT;
                    break;
                case IPowerManager::SCREEN_DIM_WAKE_LOCK:
                    mWakeLockSummary |= WAKE_LOCK_SCREEN_DIM;
                    break;
                case IPowerManager::PROXIMITY_SCREEN_OFF_WAKE_LOCK:
                    mWakeLockSummary |= WAKE_LOCK_PROXIMITY_SCREEN_OFF;
                    break;
                case IPowerManager::DOZE_WAKE_LOCK:
                    mWakeLockSummary |= WAKE_LOCK_DOZE;
                    break;
            }
        }

        // Cancel wake locks that make no sense based on the current state.
        if (mWakefulness != WAKEFULNESS_DOZING) {
            mWakeLockSummary &= ~WAKE_LOCK_DOZE;
        }
        if (mWakefulness == WAKEFULNESS_ASLEEP
                || (mWakeLockSummary & WAKE_LOCK_DOZE) != 0) {
            mWakeLockSummary &= ~(WAKE_LOCK_SCREEN_BRIGHT | WAKE_LOCK_SCREEN_DIM
                    | WAKE_LOCK_BUTTON_BRIGHT);
            if (mWakefulness == WAKEFULNESS_ASLEEP) {
                mWakeLockSummary &= ~WAKE_LOCK_PROXIMITY_SCREEN_OFF;
            }
        }

        // Infer implied wake locks where necessary based on the current state.
        if ((mWakeLockSummary & (WAKE_LOCK_SCREEN_BRIGHT | WAKE_LOCK_SCREEN_DIM)) != 0) {
            if (mWakefulness == WAKEFULNESS_AWAKE) {
                mWakeLockSummary |= WAKE_LOCK_CPU | WAKE_LOCK_STAY_AWAKE;
            }
            else if (mWakefulness == WAKEFULNESS_DREAMING) {
                mWakeLockSummary |= WAKE_LOCK_CPU;
            }
        }

        if (DEBUG_SPEW) {
            Slogger::D(TAG, "updateWakeLockSummaryLocked: mWakefulness=%s, mWakeLockSummary=0x%08x",
                 WakefulnessToString(mWakefulness).string(), mWakeLockSummary);
        }
    }
}

void PowerManagerService::UpdateUserActivitySummaryLocked(
    /* [in] */ Int64 now,
    /* [in] */ Int32 dirty)
{
    // Update the status of the user activity timeout timer.
    if ((dirty & (DIRTY_WAKE_LOCKS | DIRTY_USER_ACTIVITY
            | DIRTY_WAKEFULNESS | DIRTY_SETTINGS)) != 0) {
        mHandler->RemoveMessages(MSG_USER_ACTIVITY_TIMEOUT);

        Int64 nextTimeout = 0;
        if (mWakefulness == WAKEFULNESS_AWAKE
                || mWakefulness == WAKEFULNESS_DREAMING
                || mWakefulness == WAKEFULNESS_DOZING) {
            Int32 sleepTimeout = GetSleepTimeoutLocked();
            Int32 screenOffTimeout = GetScreenOffTimeoutLocked(sleepTimeout);
            Int32 screenDimDuration = GetScreenDimDurationLocked(screenOffTimeout);

            mUserActivitySummary = 0;
            if (mWakefulness == WAKEFULNESS_AWAKE && mLastUserActivityTime >= mLastWakeTime) {
                nextTimeout = mLastUserActivityTime + screenOffTimeout - screenDimDuration;
                if (DEBUG) {
                    Slogger::D(TAG, "line:%d, func:%s, mLastUserActivityTime:%lld, screenOffTimeout:%d, screenDimDuration:%d,nextTimeout:%lld\n"
                        ,__LINE__, __func__, mLastUserActivityTime, screenOffTimeout, screenDimDuration, nextTimeout);
                }
                if (now < nextTimeout) {
                    Int32 buttonBrightness, keyboardBrightness;
                    if (mButtonBrightnessOverrideFromWindowManager >= 0) {
                        buttonBrightness = mButtonBrightnessOverrideFromWindowManager;
                        keyboardBrightness = mButtonBrightnessOverrideFromWindowManager;
                    }
                    else {
                        buttonBrightness = mButtonBrightness;
                        keyboardBrightness = mKeyboardBrightness;
                    }

                    mKeyboardLight->SetBrightness(mKeyboardVisible ? keyboardBrightness : 0);
                    if (mButtonTimeout != 0 && now > mLastUserActivityTime + mButtonTimeout) {
                         mButtonsLight->SetBrightness(0);
                    }
                    else {
                        // The proximity sensor during a call may indicate positive,
                        // the screen & buttons should stay off until it indicates negative
                        if (!mProximityPositive) {
                            mButtonsLight->SetBrightness(buttonBrightness);
                            if (buttonBrightness != 0 && mButtonTimeout != 0) {
                                nextTimeout = now + mButtonTimeout;
                            }
                        }
                    }

                    mUserActivitySummary = USER_ACTIVITY_SCREEN_BRIGHT;
                }
                else {
                    nextTimeout = mLastUserActivityTime + screenOffTimeout;
                    if (now < nextTimeout) {
                        mButtonsLight->SetBrightness(0);
                        mKeyboardLight->SetBrightness(0);
                        mUserActivitySummary = USER_ACTIVITY_SCREEN_DIM;
                    }
                }
            }
            if (mUserActivitySummary == 0
                    && mLastUserActivityTimeNoChangeLights >= mLastWakeTime) {
                nextTimeout = mLastUserActivityTimeNoChangeLights + screenOffTimeout;
                if (now < nextTimeout) {
                    Int32 policy;
                    mDisplayPowerRequest->GetPolicy(&policy);
                    if (policy == IDisplayPowerRequest::POLICY_BRIGHT) {
                        mUserActivitySummary = USER_ACTIVITY_SCREEN_BRIGHT;
                    }
                    else if (policy == IDisplayPowerRequest::POLICY_DIM) {
                        mUserActivitySummary = USER_ACTIVITY_SCREEN_DIM;
                    }
                }
            }
            if (DEBUG) {
                Slogger::D(TAG, "line:%d, func:%s, mUserActivitySummary:%d, sleepTimeout:%d\n",
                    __LINE__, __func__, mUserActivitySummary, sleepTimeout);
            }
            if (mUserActivitySummary == 0) {
                if (sleepTimeout >= 0) {
                    const Int64 anyUserActivity = Elastos::Core::Math::Max(mLastUserActivityTime,
                            mLastUserActivityTimeNoChangeLights);
                    if (anyUserActivity >= mLastWakeTime) {
                        nextTimeout = anyUserActivity + sleepTimeout;
                        if (now < nextTimeout) {
                            mUserActivitySummary = USER_ACTIVITY_SCREEN_DREAM;
                        }
                    }
                }
                else {
                    mUserActivitySummary = USER_ACTIVITY_SCREEN_DREAM;
                    nextTimeout = -1;
                }
            }
            if (mUserActivitySummary != 0 && nextTimeout >= 0) {
                AutoPtr<IMessage> msg;
                mHandler->ObtainMessage(MSG_USER_ACTIVITY_TIMEOUT, (IMessage**)&msg);
                msg->SetAsynchronous(TRUE);
                Boolean result;
                mHandler->SendMessageAtTime(msg, nextTimeout, &result);
            }
        }
        else {
            mUserActivitySummary = 0;
        }

        if (DEBUG_SPEW) {
            Slogger::D(TAG, "updateUserActivitySummaryLocked: mWakefulness=%s, mUserActivitySummary=0x%08x, "
                "now:%lld, nextTimeout=%s",
                WakefulnessToString(mWakefulness).string(), mUserActivitySummary, now,
                TimeUtils::FormatUptime(nextTimeout).string());
        }
    }
}

void PowerManagerService::HandleUserActivityTimeout()
{
    // runs on handler thread
    {
        AutoLock syncLock(mLock);
        if (DEBUG_SPEW) {
            Slogger::D(TAG, "handleUserActivityTimeout");
        }

        mDirty |= DIRTY_USER_ACTIVITY;
        UpdatePowerStateLocked();
    }
}

Int32 PowerManagerService::GetSleepTimeoutLocked()
{
    Int32 timeout = mSleepTimeoutSetting;
    if (timeout <= 0) {
        return -1;
    }
    return Elastos::Core::Math::Max(timeout, mMinimumScreenOffTimeoutConfig);
}

Int32 PowerManagerService::GetScreenOffTimeoutLocked(
    /* [in] */ Int32 sleepTimeout)
{
    using Elastos::Core::Math;
    Int32 timeout = mScreenOffTimeoutSetting;
    if (IsMaximumScreenOffTimeoutFromDeviceAdminEnforcedLocked()) {
        timeout = Math::Min(timeout, mMaximumScreenOffTimeoutFromDeviceAdmin);
    }
    if (mUserActivityTimeoutOverrideFromWindowManager >= 0) {
        timeout = (Int32)Math::Min((Int64)timeout, mUserActivityTimeoutOverrideFromWindowManager);
    }

    if (sleepTimeout >= 0) {
        timeout = Math::Min(timeout, sleepTimeout);
    }
    return Math::Max(timeout, mMinimumScreenOffTimeoutConfig);
}

Int32 PowerManagerService::GetScreenDimDurationLocked(
    /* [in] */ Int32 screenOffTimeout)
{
    return Elastos::Core::Math::Min(mMaximumScreenDimDurationConfig,
            (Int32)(screenOffTimeout * mMaximumScreenDimRatioConfig));
}

Boolean PowerManagerService::UpdateWakefulnessLocked(
    /* [in] */ Int32 dirty)
{
    Boolean changed = FALSE;
    if ((dirty & (DIRTY_WAKE_LOCKS | DIRTY_USER_ACTIVITY | DIRTY_BOOT_COMPLETED
            | DIRTY_WAKEFULNESS | DIRTY_STAY_ON | DIRTY_PROXIMITY_POSITIVE
            | DIRTY_DOCK_STATE)) != 0) {
        if (mWakefulness == WAKEFULNESS_AWAKE && IsItBedTimeYetLocked()) {
            if (DEBUG_SPEW) {
                Slogger::D(TAG, "updateWakefulnessLocked: Bed time...");
            }
            Int64 time = SystemClock::GetUptimeMillis();
            if (ShouldNapAtBedTimeLocked()) {
                changed = NapNoUpdateLocked(time, IProcess::SYSTEM_UID);
            }
            else {
                changed = GoToSleepNoUpdateLocked(time,
                        IPowerManager::GO_TO_SLEEP_REASON_TIMEOUT, 0, IProcess::SYSTEM_UID);
            }
        }
    }
    return changed;
}

Boolean PowerManagerService::ShouldNapAtBedTimeLocked()
{
    return mDreamsActivateOnSleepSetting
            || (mDreamsActivateOnDockSetting
                    && mDockState != IIntent::EXTRA_DOCK_STATE_UNDOCKED);
}

Boolean PowerManagerService::IsItBedTimeYetLocked()
{
    return mBootCompleted && !IsBeingKeptAwakeLocked();
}

Boolean PowerManagerService::IsBeingKeptAwakeLocked()
{
    return mStayOn
            || mProximityPositive
            || (mWakeLockSummary & WAKE_LOCK_STAY_AWAKE) != 0
            || (mUserActivitySummary & (USER_ACTIVITY_SCREEN_BRIGHT
                    | USER_ACTIVITY_SCREEN_DIM)) != 0;
}

void PowerManagerService::UpdateDreamLocked(
    /* [in] */ Int32 dirty,
    /* [in] */ Boolean displayBecameReady)
{
    if ((dirty & (DIRTY_WAKEFULNESS
            | DIRTY_USER_ACTIVITY
            | DIRTY_WAKE_LOCKS
            | DIRTY_BOOT_COMPLETED
            | DIRTY_SETTINGS
            | DIRTY_IS_POWERED
            | DIRTY_STAY_ON
            | DIRTY_PROXIMITY_POSITIVE
            | DIRTY_BATTERY_STATE)) != 0 || displayBecameReady) {
        if (mDisplayReady) {
            ScheduleSandmanLocked();
        }
    }
}

void PowerManagerService::ScheduleSandmanLocked()
{
    if (!mSandmanScheduled) {
        mSandmanScheduled = TRUE;
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_SANDMAN, (IMessage**)&msg);
        msg->SetAsynchronous(TRUE);
        Boolean result;
        mHandler->SendMessage(msg, &result);
    }
}

void PowerManagerService::HandleSandman()
{
    // Handle preconditions.
    Boolean startDreaming;
    Int32 wakefulness;
    {
        AutoLock syncLock(mLock);
        mSandmanScheduled = FALSE;
        wakefulness = mWakefulness;
        if (mSandmanSummoned && mDisplayReady) {
            startDreaming = CanDreamLocked() || CanDozeLocked();
            mSandmanSummoned = FALSE;
        }
        else {
            startDreaming = FALSE;
        }
    }

    // Start dreaming if needed.
    // We only control the dream on the handler thread, so we don't need to worry about
    // concurrent attempts to start or stop the dream.
    Boolean isDreaming;
    if (mDreamManager != NULL) {
        // Restart the dream whenever the sandman is summoned.
        if (startDreaming) {
            mDreamManager->StopDream(FALSE /*immediate*/);
            mDreamManager->StartDream(wakefulness == WAKEFULNESS_DOZING);
        }
        mDreamManager->IsDreaming(&isDreaming);
    }
    else {
        isDreaming = FALSE;
    }

    // Update dream state.
    {
        AutoLock syncLock(mLock);
        // Remember the initial battery level when the dream started.
        if (startDreaming && isDreaming) {
            mBatteryLevelWhenDreamStarted = mBatteryLevel;
            if (wakefulness == WAKEFULNESS_DOZING) {
                Slogger::I(TAG, "Dozing...");
            }
            else {
                Slogger::I(TAG, "Dreaming...");
            }
        }

        // If preconditions changed, wait for the next iteration to determine
        // whether the dream should continue (or be restarted).
        if (mSandmanSummoned || mWakefulness != wakefulness) {
            return; // wait for next cycle
        }

        // Determine whether the dream should continue.
        if (wakefulness == WAKEFULNESS_DREAMING) {
            if (isDreaming && CanDreamLocked()) {
                if (mDreamsBatteryLevelDrainCutoffConfig >= 0
                        && mBatteryLevel < mBatteryLevelWhenDreamStarted
                                - mDreamsBatteryLevelDrainCutoffConfig
                        && !IsBeingKeptAwakeLocked()) {
                    // If the user activity timeout expired and the battery appears
                    // to be draining faster than it is charging then stop dreaming
                    // and go to sleep.
                    Slogger::I(TAG, "Stopping dream because the battery appears to be draining faster than it is charging.  Battery level when dream started: %d%.  Battery level now: %d%.",
                            mBatteryLevelWhenDreamStarted, mBatteryLevel);
                }
                else {
                    return; // continue dreaming
                }
            }

            // Dream has ended or will be stopped.  Update the power state.
            if (IsItBedTimeYetLocked()) {
                GoToSleepNoUpdateLocked(SystemClock::GetUptimeMillis(),
                        IPowerManager::GO_TO_SLEEP_REASON_TIMEOUT, 0, IProcess::SYSTEM_UID);
                UpdatePowerStateLocked();
            }
            else {
                WakeUpNoUpdateLocked(SystemClock::GetUptimeMillis(), IProcess::SYSTEM_UID);
                UpdatePowerStateLocked();
            }
        }
        else if (wakefulness == WAKEFULNESS_DOZING) {
            if (isDreaming) {
                return; // continue dozing
            }

            // Doze has ended or will be stopped.  Update the power state.
            ReallyGoToSleepNoUpdateLocked(SystemClock::GetUptimeMillis(), IProcess::SYSTEM_UID);
            UpdatePowerStateLocked();
        }
    }

    // Stop dream.
    if (isDreaming) {
        mDreamManager->StopDream(FALSE /*immediate*/);
    }
}

Boolean PowerManagerService::CanDreamLocked()
{
    Boolean res;
    if (mWakefulness != WAKEFULNESS_DREAMING
            || !mDreamsSupportedConfig
            || !mDreamsEnabledSetting
            || (mDisplayPowerRequest->IsBrightOrDim(&res), !res)
            || (mUserActivitySummary & (USER_ACTIVITY_SCREEN_BRIGHT
                    | USER_ACTIVITY_SCREEN_DIM | USER_ACTIVITY_SCREEN_DREAM)) == 0
            || !mBootCompleted) {
        return FALSE;
    }
    if (!IsBeingKeptAwakeLocked()) {
        if (!mIsPowered && !mDreamsEnabledOnBatteryConfig) {
            return FALSE;
        }
        if (!mIsPowered
                && mDreamsBatteryLevelMinimumWhenNotPoweredConfig >= 0
                && mBatteryLevel < mDreamsBatteryLevelMinimumWhenNotPoweredConfig) {
            return FALSE;
        }
        if (mIsPowered
                && mDreamsBatteryLevelMinimumWhenPoweredConfig >= 0
                && mBatteryLevel < mDreamsBatteryLevelMinimumWhenPoweredConfig) {
            return FALSE;
        }
    }
    return TRUE;
}

Boolean PowerManagerService::CanDozeLocked()
{
    return mWakefulness == WAKEFULNESS_DOZING;
}

Boolean PowerManagerService::UpdateDisplayPowerStateLocked(
    /* [in] */ Int32 dirty)
{
    using Elastos::Core::Math;
    Boolean oldDisplayReady = mDisplayReady;
    if ((dirty & (DIRTY_WAKE_LOCKS | DIRTY_USER_ACTIVITY | DIRTY_WAKEFULNESS
            | DIRTY_ACTUAL_DISPLAY_POWER_STATE_UPDATED | DIRTY_BOOT_COMPLETED
            | DIRTY_SETTINGS)) != 0) {
        Int32 value = GetDesiredScreenPolicyLocked();
        mDisplayPowerRequest->SetPolicy(value);

        Int32 screenBrightness = mScreenBrightnessSettingDefault;
        Float screenAutoBrightnessAdjustment = 0.0f;
        Boolean autoBrightness = (mScreenBrightnessModeSetting == ISettingsSystem::SCREEN_BRIGHTNESS_MODE_AUTOMATIC);
        if (IsValidBrightness(mScreenBrightnessOverrideFromWindowManager)) {
            screenBrightness = mScreenBrightnessOverrideFromWindowManager;
            autoBrightness = FALSE;
        }
        else if (IsValidBrightness(mTemporaryScreenBrightnessSettingOverride)) {
            screenBrightness = mTemporaryScreenBrightnessSettingOverride;
        }
        else if (IsValidBrightness(mScreenBrightnessSetting)) {
            screenBrightness = mScreenBrightnessSetting;
        }
        if (autoBrightness) {
            screenBrightness = mScreenBrightnessSettingDefault;
            if (IsValidAutoBrightnessAdjustment(
                    mTemporaryScreenAutoBrightnessAdjustmentSettingOverride)) {
                screenAutoBrightnessAdjustment =
                        mTemporaryScreenAutoBrightnessAdjustmentSettingOverride;
            }
            else if (IsValidAutoBrightnessAdjustment(
                    mScreenAutoBrightnessAdjustmentSetting)) {
                screenAutoBrightnessAdjustment = mScreenAutoBrightnessAdjustmentSetting;
            }
        }
        screenBrightness = Math::Max(Math::Min(screenBrightness,
            mScreenBrightnessSettingMaximum), mScreenBrightnessSettingMinimum);
        screenAutoBrightnessAdjustment = Math::Max(Math::Min(
            screenAutoBrightnessAdjustment, 1.0f), -1.0f);
        mDisplayPowerRequest->SetScreenBrightness(screenBrightness);
        mDisplayPowerRequest->SetScreenAutoBrightnessAdjustment(screenAutoBrightnessAdjustment);
        mDisplayPowerRequest->SetUseAutoBrightness(autoBrightness);
        mDisplayPowerRequest->SetUseProximitySensor(ShouldUseProximitySensorLocked());
        mDisplayPowerRequest->SetLowPowerMode(mLowPowerModeEnabled);

        Int32 policy;
        mDisplayPowerRequest->GetPolicy(&policy);
        if (policy == IDisplayPowerRequest::POLICY_DOZE) {
            mDisplayPowerRequest->SetDozeScreenState(mDozeScreenStateOverrideFromDreamManager);
            mDisplayPowerRequest->SetDozeScreenBrightness(mDozeScreenBrightnessOverrideFromDreamManager);
        }
        else {
            mDisplayPowerRequest->SetDozeScreenState(IDisplay::STATE_UNKNOWN);
            mDisplayPowerRequest->SetDozeScreenBrightness(IPowerManager::BRIGHTNESS_DEFAULT);
        }

        mDisplayManagerInternal->RequestPowerState(mDisplayPowerRequest,
            mRequestWaitForNegativeProximity, &mDisplayReady);
        mRequestWaitForNegativeProximity = FALSE;
        if (DEBUG_SPEW) {
            Slogger::D(TAG, "updateScreenStateLocked: mDisplayReady=%d, policy=%d" \
                ", mWakefulness=%d, mWakeLockSummary=0x%08x, mUserActivitySummary=0x%08x, mBootCompleted=%d"
                , mDisplayReady, policy, mWakefulness
                , mWakeLockSummary, mUserActivitySummary, mBootCompleted);
        }
    }
    return mDisplayReady && !oldDisplayReady;
}

Boolean PowerManagerService::IsValidBrightness(
    /* [in] */ Int32 value)
{
    return value >= 0 && value <= 255;
}

Boolean PowerManagerService::IsValidAutoBrightnessAdjustment(
    /* [in] */ Float value)
{
    // Handles NaN by always returning false.
    return value >= -1.0f && value <= 1.0f;
}

Int32 PowerManagerService::GetDesiredScreenPolicyLocked()
{
    if (mWakefulness == WAKEFULNESS_ASLEEP) {
        return IDisplayPowerRequest::POLICY_OFF;
    }

    if (mWakefulness == WAKEFULNESS_DOZING) {
        if ((mWakeLockSummary & WAKE_LOCK_DOZE) != 0) {
            return IDisplayPowerRequest::POLICY_DOZE;
        }
        if (mDozeAfterScreenOffConfig) {
            return IDisplayPowerRequest::POLICY_OFF;
        }
        // Fall through and preserve the current screen policy if not configured to
        // doze after screen off.  This causes the screen off transition to be skipped.
    }

    if ((mWakeLockSummary & WAKE_LOCK_SCREEN_BRIGHT) != 0
            || (mUserActivitySummary & USER_ACTIVITY_SCREEN_BRIGHT) != 0
            || !mBootCompleted) {
        return IDisplayPowerRequest::POLICY_BRIGHT;
    }

    return IDisplayPowerRequest::POLICY_DIM;
}

Boolean PowerManagerService::ShouldUseProximitySensorLocked()
{
    return (mWakeLockSummary & WAKE_LOCK_PROXIMITY_SCREEN_OFF) != 0;
}

void PowerManagerService::UpdateSuspendBlockerLocked()
{
    Boolean needWakeLockSuspendBlocker = ((mWakeLockSummary & WAKE_LOCK_CPU) != 0);
    Boolean needDisplaySuspendBlocker = NeedDisplaySuspendBlockerLocked();
    Boolean autoSuspend = !needDisplaySuspendBlocker;

    // Disable auto-suspend if needed.
    if (!autoSuspend) {
        if (mDecoupleHalAutoSuspendModeFromDisplayConfig) {
            SetHalAutoSuspendModeLocked(FALSE);
        }
        if (mDecoupleHalInteractiveModeFromDisplayConfig) {
            SetHalInteractiveModeLocked(TRUE);
        }
    }

    // First acquire suspend blockers if needed.
    if (needWakeLockSuspendBlocker && !mHoldingWakeLockSuspendBlocker) {
        mWakeLockSuspendBlocker->AcquireBlocker();
        mHoldingWakeLockSuspendBlocker = TRUE;
    }
    if (needDisplaySuspendBlocker && !mHoldingDisplaySuspendBlocker) {
        mDisplaySuspendBlocker->AcquireBlocker();
        mHoldingDisplaySuspendBlocker = TRUE;
    }

    // Then release suspend blockers if needed.
    if (!needWakeLockSuspendBlocker && mHoldingWakeLockSuspendBlocker) {
        mWakeLockSuspendBlocker->ReleaseBlocker();
        mHoldingWakeLockSuspendBlocker = FALSE;
    }
    if (!needDisplaySuspendBlocker && mHoldingDisplaySuspendBlocker) {
        mDisplaySuspendBlocker->ReleaseBlocker();
        mHoldingDisplaySuspendBlocker = FALSE;
    }

    // Enable auto-suspend if needed.
    if (autoSuspend) {
        if (mDecoupleHalInteractiveModeFromDisplayConfig) {
            SetHalInteractiveModeLocked(FALSE);
        }
        if (mDecoupleHalAutoSuspendModeFromDisplayConfig) {
            SetHalAutoSuspendModeLocked(TRUE);
        }
    }
}

Boolean PowerManagerService::NeedDisplaySuspendBlockerLocked()
{
    if (!mDisplayReady) {
        return TRUE;
    }
    Boolean res;
    if (mDisplayPowerRequest->IsBrightOrDim(&res), res) {
        // If we asked for the screen to be on but it is off due to the proximity
        // sensor then we may suspend but only if the configuration allows it.
        // On some hardware it may not be safe to suspend because the proximity
        // sensor may not be correctly configured as a wake-up source.
        Boolean value;
        mDisplayPowerRequest->GetUseProximitySensor(&value);
        if (!value || !mProximityPositive
                || !mSuspendWhenScreenOffDueToProximityConfig) {
            return TRUE;
        }
    }
    // Let the system suspend if the screen is off or dozing.
    return FALSE;
}

void PowerManagerService::SetHalAutoSuspendModeLocked(
    /* [in] */ Boolean enable)
{
    if (enable != mHalAutoSuspendModeEnabled) {
        if (DEBUG) {
            Slogger::D(TAG, "Setting HAL auto-suspend mode to %d", enable);
        }
        mHalAutoSuspendModeEnabled = enable;
        // Trace.traceBegin(Trace.TRACE_TAG_POWER, "setHalAutoSuspend(" + enable + ")");
        // try {
        NativeSetAutoSuspend(enable);
        // } finally {
        //     Trace.traceEnd(Trace.TRACE_TAG_POWER);
        // }
    }
}

void PowerManagerService::SetHalInteractiveModeLocked(
    /* [in] */ Boolean enable)
{
    if (enable != mHalInteractiveModeEnabled) {
        if (DEBUG) {
            Slogger::D(TAG, "Setting HAL interactive mode to %d", enable);
        }
        mHalInteractiveModeEnabled = enable;
        // Trace.traceBegin(Trace.TRACE_TAG_POWER, "setHalInteractive(" + enable + ")");
        // try {
        NativeSetInteractive(enable);
        // } finally {
        //     Trace.traceEnd(Trace.TRACE_TAG_POWER);
        // }
    }
}

Boolean PowerManagerService::IsInteractiveInternal()
{
    AutoLock syncLock(mLock);
    return mInteractive;
}

Boolean PowerManagerService::IsLowPowerModeInternal()
{
    AutoLock syncLock(mLock);
    return mLowPowerModeEnabled;
}

Boolean PowerManagerService::SetLowPowerModeInternal(
    /* [in] */ Boolean mode)
{
    AutoLock syncLock(mLock);
    if (DEBUG) {
        Slogger::D(TAG, "setLowPowerModeInternal %d mIsPowered=%d", mode, mIsPowered);
    }

    if (mIsPowered) {
        return FALSE;
    }
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    Boolean res;
    settingsGlobal->PutInt32(resolver,
            ISettingsGlobal::LOW_POWER_MODE, mode ? 1 : 0, &res);
    mLowPowerModeSetting = mode;

    if (mAutoLowPowerModeConfigured && mBatteryLevelLow) {
        if (mode && mAutoLowPowerModeSnoozing) {
            if (DEBUG_SPEW) {
                Slogger::D(TAG, "setLowPowerModeInternal: clearing low power mode snooze");
            }
            mAutoLowPowerModeSnoozing = FALSE;
        }
        else if (!mode && !mAutoLowPowerModeSnoozing) {
            if (DEBUG_SPEW) {
                Slogger::D(TAG, "setLowPowerModeInternal: snoozing low power mode");
            }
            mAutoLowPowerModeSnoozing = TRUE;
        }
    }

    UpdateLowPowerModeLocked();

    return TRUE;
}

void PowerManagerService::HandleBatteryStateChangedLocked()
{
    mDirty |= DIRTY_BATTERY_STATE;
    UpdatePowerStateLocked();
}

ECode PowerManagerService::ShutdownOrRebootInternal(
    /* [in] */ const Boolean shutdown,
    /* [in] */ const Boolean confirm,
    /* [in] */ const String& reason,
    /* [in] */ Boolean wait)
{
    if (mHandler == NULL || !mSystemReady) {
        Slogger::E(TAG, "Too early to call shutdown() or reboot(), "
            "mHandler:%s, mSystemReady:%d", TO_CSTR(mHandler), mSystemReady);
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Too early to call shutdown() or reboot()");
    }

    AutoPtr<ShutdownOrRebootRunnable> runnable = new ShutdownOrRebootRunnable(
            shutdown, confirm, reason, mContext);

    // ShutdownThread must run on a looper capable of displaying the UI.
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain(mHandler, runnable, (IMessage**)&msg);
    msg->SetAsynchronous(TRUE);
    Boolean res;
    mHandler->SendMessage(msg, &res);

    // PowerManager.reboot() is documented not to return so just wait for the inevitable.
    if (wait) {
        AutoLock syncLock(runnable);
        while (TRUE) {
            // try {
            runnable->Wait();
            // } catch (InterruptedException e) {
            // }
        }
    }
    return NOERROR;
}

ECode PowerManagerService::CrashInternal(
    /* [in] */ const String& message)
{
    AutoPtr<IThread> t = (IThread*)new CrashThread(String("PowerManagerService.crash()"));
    // try {
    FAIL_RETURN(t->Start());
    return t->Join();
    // } catch (InterruptedException e) {
    //     Slog.wtf(TAG, e);
    // }
}

void PowerManagerService::SetStayOnSettingInternal(
    /* [in] */ Int32 val)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    Boolean result;
    settingsGlobal->PutInt32(resolver,
            ISettingsGlobal::STAY_ON_WHILE_PLUGGED_IN,
            val, &result);
}

void PowerManagerService::SetMaximumScreenOffTimeoutFromDeviceAdminInternal(
    /* [in] */ Int32 timeMs)
{
    AutoLock syncLock(mLock);
    mMaximumScreenOffTimeoutFromDeviceAdmin = timeMs;
    mDirty |= DIRTY_SETTINGS;
    UpdatePowerStateLocked();
}

Boolean PowerManagerService::IsMaximumScreenOffTimeoutFromDeviceAdminEnforcedLocked()
{
    return mMaximumScreenOffTimeoutFromDeviceAdmin >= 0
            && mMaximumScreenOffTimeoutFromDeviceAdmin < Elastos::Core::Math::INT32_MAX_VALUE;
}

void PowerManagerService::SetAttentionLightInternal(
    /* [in] */ Boolean on,
    /* [in] */ Int32 color)
{
    AutoPtr<Light> light;
    {
        AutoLock syncLock(mLock);
        if (!mSystemReady) {
            return;
        }
        light = mAttentionLight;
    }

    // Control light outside of lock.
    light->SetFlashing(color, Light::LIGHT_FLASH_HARDWARE,
            (on ? 3 : 0), 0);
}

void PowerManagerService::SetScreenBrightnessOverrideFromWindowManagerInternal(
    /* [in] */ Int32 brightness)
{
    {
        AutoLock syncLock(mLock);
        if (mScreenBrightnessOverrideFromWindowManager != brightness) {
            mScreenBrightnessOverrideFromWindowManager = brightness;
            mDirty |= DIRTY_SETTINGS;
            UpdatePowerStateLocked();
        }
    }
}

void PowerManagerService::SetUserActivityTimeoutOverrideFromWindowManagerInternal(
    /* [in] */ Int64 timeoutMillis)
{
    {
        AutoLock syncLock(mLock);
        if (mUserActivityTimeoutOverrideFromWindowManager != timeoutMillis) {
            mUserActivityTimeoutOverrideFromWindowManager = timeoutMillis;
            mDirty |= DIRTY_SETTINGS;
            UpdatePowerStateLocked();
        }
    }
}

void PowerManagerService::SetTemporaryScreenBrightnessSettingOverrideInternal(
    /* [in] */ Int32 brightness)
{
    {
        AutoLock syncLock(mLock);
        if (mTemporaryScreenBrightnessSettingOverride != brightness) {
            mTemporaryScreenBrightnessSettingOverride = brightness;
            mDirty |= DIRTY_SETTINGS;
            UpdatePowerStateLocked();
        }
    }
}

void PowerManagerService::SetTemporaryScreenAutoBrightnessAdjustmentSettingOverrideInternal(
    /* [in] */ Float adj)
{
    {
         AutoLock syncLock(mLock);
        // Note: This condition handles NaN because NaN is not equal to any other
        // value, including itself.
        if (mTemporaryScreenAutoBrightnessAdjustmentSettingOverride != adj) {
            mTemporaryScreenAutoBrightnessAdjustmentSettingOverride = adj;
            mDirty |= DIRTY_SETTINGS;
            UpdatePowerStateLocked();
        }
    }
}

void PowerManagerService::SetDozeOverrideFromDreamManagerInternal(
    /* [in] */ Int32 screenState,
    /* [in] */ Int32 screenBrightness)
{
    {
        AutoLock syncLock(mLock);
        if (mDozeScreenStateOverrideFromDreamManager != screenState
                || mDozeScreenBrightnessOverrideFromDreamManager != screenBrightness) {
            mDozeScreenStateOverrideFromDreamManager = screenState;
            mDozeScreenBrightnessOverrideFromDreamManager = screenBrightness;
            mDirty |= DIRTY_SETTINGS;
            UpdatePowerStateLocked();
        }
    }
}

void PowerManagerService::PowerHintInternal(
    /* [in] */ Int32 hintId,
    /* [in] */ Int32 data)
{
    NativeSendPowerHint(hintId, data);
}

void PowerManagerService::LowLevelShutdown()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->Set(String("sys.powerctl"), String("shutdown"));
}

ECode PowerManagerService::LowLevelReboot(
    /* [in] */ const String& reason)
{
    String str = reason;
    if (reason.IsNull()) {
        str = "";
    }

    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Int64 duration;
    if (str.Equals(IPowerManager::REBOOT_RECOVERY)) {
        // If we are rebooting to go into recovery, instead of
        // setting sys.powerctl directly we'll start the
        // pre-recovery service which will do some preparation for
        // recovery and then reboot for us.
        //
        // This preparation can take more than 20 seconds if
        // there's a very large update package, so lengthen the
        // timeout.
        sp->Set(String("ctl.start"), String("pre-recovery"));
        duration = 120 * 1000LL;
    }
    else {
        sp->Set(String("sys.powerctl"), String("reboot,") + str);
        duration = 20 * 1000LL;
    }
    // try {
    Thread::Sleep(duration);
    // } catch (InterruptedException e) {
    //     Thread.currentThread().interrupt();
    // }
    return NOERROR;
}

ECode PowerManagerService::Monitor()
{
    // Grab and release lock for watchdog monitor to detect deadlocks.
    {    AutoLock syncLock(mLock);
        return NOERROR;
    }
    return NOERROR;
}

void PowerManagerService::DumpInternal(
    /* [in] */ IPrintWriter* pw)
{
    pw->Println(String("POWER MANAGER (dumpsys power)\n"));

    AutoPtr<WirelessChargerDetector> wcd;
    {
        AutoLock syncLock(mLock);
        pw->Println(String("Power Manager State:"));
        pw->Println(String("  mDirty=0x") + StringUtils::ToHexString(mDirty));
        pw->Println(String("  mWakefulness=") + WakefulnessToString(mWakefulness));
        pw->Println(String("  mInteractive=") + StringUtils::BooleanToString(mInteractive));
        pw->Println(String("  mIsPowered=") + StringUtils::BooleanToString(mIsPowered));
        pw->Println(String("  mPlugType=") + StringUtils::ToString(mPlugType));
        pw->Println(String("  mBatteryLevel=") + StringUtils::ToString(mBatteryLevel));
        pw->Println(String("  mBatteryLevelWhenDreamStarted=") + StringUtils::ToString(mBatteryLevelWhenDreamStarted));
        pw->Println(String("  mDockState=") + StringUtils::ToString(mDockState));
        pw->Println(String("  mStayOn=") + StringUtils::BooleanToString(mStayOn));
        pw->Println(String("  mProximityPositive=") + StringUtils::BooleanToString(mProximityPositive));
        pw->Println(String("  mBootCompleted=") + StringUtils::BooleanToString(mBootCompleted));
        pw->Println(String("  mSystemReady=") + StringUtils::BooleanToString(mSystemReady));
        pw->Println(String("  mHalAutoSuspendModeEnabled=") + StringUtils::BooleanToString(mHalAutoSuspendModeEnabled));
        pw->Println(String("  mHalInteractiveModeEnabled=") + StringUtils::BooleanToString(mHalInteractiveModeEnabled));
        pw->Println(String("  mWakeLockSummary=0x") + StringUtils::ToHexString(mWakeLockSummary));
        pw->Println(String("  mUserActivitySummary=0x") + StringUtils::ToHexString(mUserActivitySummary));
        pw->Println(String("  mRequestWaitForNegativeProximity=") + StringUtils::BooleanToString(mRequestWaitForNegativeProximity));
        pw->Println(String("  mSandmanScheduled=") + StringUtils::BooleanToString(mSandmanScheduled));
        pw->Println(String("  mSandmanSummoned=") + StringUtils::BooleanToString(mSandmanSummoned));
        pw->Println(String("  mLowPowerModeEnabled=") + StringUtils::BooleanToString(mLowPowerModeEnabled));
        pw->Println(String("  mBatteryLevelLow=") + StringUtils::BooleanToString(mBatteryLevelLow));
        pw->Println(String("  mLastWakeTime=") + TimeUtils::FormatUptime(mLastWakeTime));
        pw->Println(String("  mLastSleepTime=") + TimeUtils::FormatUptime(mLastSleepTime));
        pw->Println(String("  mLastUserActivityTime=") + TimeUtils::FormatUptime(mLastUserActivityTime));
        pw->Println(String("  mLastUserActivityTimeNoChangeLights=")
                + TimeUtils::FormatUptime(mLastUserActivityTimeNoChangeLights));
        pw->Println(String("  mLastInteractivePowerHintTime=")
                + TimeUtils::FormatUptime(mLastInteractivePowerHintTime));
        pw->Println(String("  mDisplayReady=") + StringUtils::BooleanToString(mDisplayReady));
        pw->Println(String("  mHoldingWakeLockSuspendBlocker=") + StringUtils::BooleanToString(mHoldingWakeLockSuspendBlocker));
        pw->Println(String("  mHoldingDisplaySuspendBlocker=") + StringUtils::BooleanToString(mHoldingDisplaySuspendBlocker));

        pw->Println();
        pw->Println(String("Settings and Configuration:"));
        pw->Println(String("  mDecoupleHalAutoSuspendModeFromDisplayConfig=")
                + StringUtils::BooleanToString(mDecoupleHalAutoSuspendModeFromDisplayConfig));
        pw->Println(String("  mDecoupleHalInteractiveModeFromDisplayConfig=")
                + StringUtils::BooleanToString(mDecoupleHalInteractiveModeFromDisplayConfig));
        pw->Println(String("  mWakeUpWhenPluggedOrUnpluggedConfig=")
                + StringUtils::BooleanToString(mWakeUpWhenPluggedOrUnpluggedConfig));
        pw->Println(String("  mSuspendWhenScreenOffDueToProximityConfig=")
                + StringUtils::BooleanToString(mSuspendWhenScreenOffDueToProximityConfig));
        pw->Println(String("  mDreamsSupportedConfig=") + StringUtils::BooleanToString(mDreamsSupportedConfig));
        pw->Println(String("  mDreamsEnabledByDefaultConfig=") + StringUtils::BooleanToString(mDreamsEnabledByDefaultConfig));
        pw->Println(String("  mDreamsActivatedOnSleepByDefaultConfig=")
                + StringUtils::BooleanToString(mDreamsActivatedOnSleepByDefaultConfig));
        pw->Println(String("  mDreamsActivatedOnDockByDefaultConfig=")
                + StringUtils::BooleanToString(mDreamsActivatedOnDockByDefaultConfig));
        pw->Println(String("  mDreamsEnabledOnBatteryConfig=")
                + StringUtils::BooleanToString(mDreamsEnabledOnBatteryConfig));
        pw->Println(String("  mDreamsBatteryLevelMinimumWhenPoweredConfig=")
                + StringUtils::ToString(mDreamsBatteryLevelMinimumWhenPoweredConfig));
        pw->Println(String("  mDreamsBatteryLevelMinimumWhenNotPoweredConfig=")
                + StringUtils::ToString(mDreamsBatteryLevelMinimumWhenNotPoweredConfig));
        pw->Println(String("  mDreamsBatteryLevelDrainCutoffConfig=")
                + StringUtils::ToString(mDreamsBatteryLevelDrainCutoffConfig));
        pw->Println(String("  mDreamsEnabledSetting=") + StringUtils::BooleanToString(mDreamsEnabledSetting));
        pw->Println(String("  mDreamsActivateOnSleepSetting=") + StringUtils::BooleanToString(mDreamsActivateOnSleepSetting));
        pw->Println(String("  mDreamsActivateOnDockSetting=") + StringUtils::BooleanToString(mDreamsActivateOnDockSetting));
        pw->Println(String("  mDozeAfterScreenOffConfig=") + StringUtils::BooleanToString(mDozeAfterScreenOffConfig));
        pw->Println(String("  mLowPowerModeSetting=") + StringUtils::BooleanToString(mLowPowerModeSetting));
        pw->Println(String("  mAutoLowPowerModeConfigured=") + StringUtils::BooleanToString(mAutoLowPowerModeConfigured));
        pw->Println(String("  mAutoLowPowerModeSnoozing=") + StringUtils::BooleanToString(mAutoLowPowerModeSnoozing));
        pw->Println(String("  mMinimumScreenOffTimeoutConfig=") + StringUtils::ToString(mMinimumScreenOffTimeoutConfig));
        pw->Println(String("  mMaximumScreenDimDurationConfig=") + StringUtils::ToString(mMaximumScreenDimDurationConfig));
        pw->Println(String("  mMaximumScreenDimRatioConfig=") + StringUtils::ToString(mMaximumScreenDimRatioConfig));
        pw->Println(String("  mScreenOffTimeoutSetting=") + StringUtils::ToString(mScreenOffTimeoutSetting));
        pw->Println(String("  mSleepTimeoutSetting=") + StringUtils::ToString(mSleepTimeoutSetting));
        StringBuilder buider;
        buider += "  mMaximumScreenOffTimeoutFromDeviceAdmin=";
        buider += mMaximumScreenOffTimeoutFromDeviceAdmin;
        buider += " (enforced=";
        buider += IsMaximumScreenOffTimeoutFromDeviceAdminEnforcedLocked();
        buider += ")";
        pw->Println(buider.ToString());
        pw->Println(String("  mStayOnWhilePluggedInSetting=") + StringUtils::ToString(mStayOnWhilePluggedInSetting));
        pw->Println(String("  mScreenBrightnessSetting=") + StringUtils::ToString(mScreenBrightnessSetting));
        pw->Println(String("  mScreenAutoBrightnessAdjustmentSetting=")
                + StringUtils::ToString(mScreenAutoBrightnessAdjustmentSetting));
        pw->Println(String("  mScreenBrightnessModeSetting=") + StringUtils::ToString(mScreenBrightnessModeSetting));
        pw->Println(String("  mScreenBrightnessOverrideFromWindowManager=")
                + StringUtils::ToString(mScreenBrightnessOverrideFromWindowManager));
        pw->Println(String("  mUserActivityTimeoutOverrideFromWindowManager=")
                + StringUtils::ToString(mUserActivityTimeoutOverrideFromWindowManager));
        pw->Println(String("  mTemporaryScreenBrightnessSettingOverride=")
                + StringUtils::ToString(mTemporaryScreenBrightnessSettingOverride));
        pw->Println(String("  mTemporaryScreenAutoBrightnessAdjustmentSettingOverride=")
                + StringUtils::ToString(mTemporaryScreenAutoBrightnessAdjustmentSettingOverride));
        pw->Println(String("  mDozeScreenStateOverrideFromDreamManager=")
                + StringUtils::ToString(mDozeScreenStateOverrideFromDreamManager));
        pw->Println(String("  mDozeScreenBrightnessOverrideFromDreamManager=")
                + StringUtils::ToString(mDozeScreenBrightnessOverrideFromDreamManager));
        pw->Println(String("  mScreenBrightnessSettingMinimum=") + StringUtils::ToString(mScreenBrightnessSettingMinimum));
        pw->Println(String("  mScreenBrightnessSettingMaximum=") + StringUtils::ToString(mScreenBrightnessSettingMaximum));
        pw->Println(String("  mScreenBrightnessSettingDefault=") + StringUtils::ToString(mScreenBrightnessSettingDefault));

        const Int32 sleepTimeout = GetSleepTimeoutLocked();
        const Int32 screenOffTimeout = GetScreenOffTimeoutLocked(sleepTimeout);
        const Int32 screenDimDuration = GetScreenDimDurationLocked(screenOffTimeout);
        pw->Println();
        buider.Reset();
        buider += "Sleep timeout: ";
        buider += sleepTimeout;
        buider += " ms";
        pw->Println(buider.ToString());
        buider.Reset();
        buider += "Screen off timeout: ";
        buider += screenOffTimeout;
        buider += " ms";
        pw->Println(buider.ToString());
        buider.Reset();
        buider += "Screen dim duration: ";
        buider += screenDimDuration;
        buider += " ms";
        pw->Println(buider.ToString());

        pw->Println();
        Int32 size;
        mWakeLocks->GetSize(&size);
        pw->Println(String("Wake Locks: size=") + StringUtils::ToString(size));
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mWakeLocks->Get(i, (IInterface**)&obj);
            AutoPtr<WakeLock> wl = (WakeLock*)(IObject*)obj.Get();
            buider.Reset();
            buider += "  ";
            buider += TO_IINTERFACE(wl);
            pw->Println(buider.ToString());
        }

        pw->Println();
        mSuspendBlockers->GetSize(&size);
        pw->Println(String("Suspend Blockers: size=") + StringUtils::ToString(size));
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mSuspendBlockers->Get(i, (IInterface**)&obj);
            AutoPtr<ISuspendBlocker> sb = ISuspendBlocker::Probe(obj);
            buider.Reset();
            buider += "  ";
            buider += sb;
            pw->Println(buider.ToString());
        }

        pw->Println();
        buider.Reset();
        buider += "Display Power: ";
        buider += mDisplayPowerCallbacks;
        pw->Println(buider.ToString());

        wcd = mWirelessChargerDetector;
    }

    if (wcd != NULL) {
        wcd->Dump(pw);
    }
}

AutoPtr<ISuspendBlocker> PowerManagerService::CreateSuspendBlockerLocked(
    /* [in] */ const String& name)
{
    AutoPtr<ISuspendBlocker> suspendBlocker = (ISuspendBlocker*)new SuspendBlockerImpl(name);
    mSuspendBlockers->Add(suspendBlocker);
    return suspendBlocker;
}

String PowerManagerService::WakefulnessToString(
    /* [in] */ Int32 wakefulness)
{
    switch (wakefulness) {
        case WAKEFULNESS_ASLEEP:
            return String("Asleep");
        case WAKEFULNESS_AWAKE:
            return String("Awake");
        case WAKEFULNESS_DREAMING:
            return String("Dreaming");
        case WAKEFULNESS_DOZING:
            return String("Dozing");
        default:
            return StringUtils::ToString(wakefulness);
    }
}

AutoPtr<IWorkSource> PowerManagerService::CopyWorkSource(
    /* [in] */ IWorkSource* workSource)
{
    if (workSource != NULL) {
        AutoPtr<IWorkSource> newWorkSource;
        CWorkSource::New(workSource, (IWorkSource**)&newWorkSource);
        return newWorkSource;
    }
    else {
        return NULL;
    }
}

// method for NativeInputManager
void PowerManagerService::UserActivity(
    /* [in] */ nsecs_t eventTime,
    /* [in] */ int32_t eventType)
{
    // Tell the power HAL when user activity occurs.
    if (sPowerModule && sPowerModule->powerHint) {
        sPowerModule->powerHint(sPowerModule, (power_hint_t)POWER_HINT_INTERACTION, NULL);
    }

    if (sPowerManagerService) {
        // Throttle calls into user activity by event type.
        // We're a little conservative about argument checking here in case the caller
        // passes in bad data which could corrupt system state.
        if (eventType >= 0 && eventType <= android::USER_ACTIVITY_EVENT_LAST) {
            nsecs_t now = systemTime(SYSTEM_TIME_MONOTONIC);
            if (eventTime > now) {
                eventTime = now;
            }

            if (sLastEventTime[eventType] + MIN_TIME_BETWEEN_USERACTIVITIES > eventTime) {
                return;
            }
            sLastEventTime[eventType] = eventTime;
        }
        sPowerManagerService->UserActivityFromNative(nanoseconds_to_milliseconds(eventTime), eventType, 0);
    }
}

/* ================================= native ============================================= */

void PowerManagerService::NativeInit()
{
    sPowerManagerService = this;

    Int32 err = hw_get_module(POWER_HARDWARE_MODULE_ID,
            (hw_module_t const**)&sPowerModule);
    if (!err) {
        sPowerModule->init(sPowerModule);
    }
    else {
        ALOGE("Couldn't load %s module (%s)", POWER_HARDWARE_MODULE_ID, strerror(-err));
    }

    // Initialize
    for (Int32 i = 0; i <= android::USER_ACTIVITY_EVENT_LAST; i++) {
        sLastEventTime[i] = LLONG_MIN;
    }
}

void PowerManagerService::NativeAcquireSuspendBlocker(
    /* [in] */ const String& name)
{
    acquire_wake_lock(PARTIAL_WAKE_LOCK, name.string());
}

void PowerManagerService::NativeReleaseSuspendBlocker(
    /* [in] */ const String& name)
{
    release_wake_lock(name.string());
}

void PowerManagerService::NativeSetInteractive(
    /* [in] */ Boolean enable)
{
    if (sPowerModule) {
        if (enable) {
            ALOGD_IF_SLOW(20, "Excessive delay in setInteractive(true) while turning screen on");
            sPowerModule->setInteractive(sPowerModule, TRUE);
        }
        else {
            ALOGD_IF_SLOW(20, "Excessive delay in setInteractive(false) while turning screen off");
            sPowerModule->setInteractive(sPowerModule, FALSE);
        }
    }
}

void PowerManagerService::NativeSetAutoSuspend(
    /* [in] */ Boolean enable)
{
    if (enable) {
        ALOGD_IF_SLOW(100, "Excessive delay in autosuspend_enable() while turning screen off");
        autosuspend_enable();
    }
    else {
        ALOGD_IF_SLOW(100, "Excessive delay in autosuspend_disable() while turning screen on");
        autosuspend_disable();
    }
}

void PowerManagerService::NativeSendPowerHint(
    /* [in] */ Int32 hintId,
    /* [in] */ Int32 data)
{
    int data_param = data;

    if (sPowerModule && sPowerModule->powerHint) {
        if(data)
            sPowerModule->powerHint(sPowerModule, (power_hint_t)hintId, &data_param);
        else {
            sPowerModule->powerHint(sPowerModule, (power_hint_t)hintId, NULL);
        }
    }
}

void PowerManagerService::NativeCpuBoost(
    /* [in] */ Int32 duration)
{
    // Tell the Power HAL to boost the CPU
    if (sPowerModule && sPowerModule->powerHint) {
        sPowerModule->powerHint(sPowerModule, POWER_HINT_CPU_BOOST, (void *)(static_cast<int64_t>(duration)));
    }
}

void PowerManagerService::NativeSetPowerProfile(
    /* [in] */ Int32 profile)
{
    // Tell the Power HAL to select a power profile
    if (sPowerModule && sPowerModule->powerHint) {
        sPowerModule->powerHint(sPowerModule, POWER_HINT_SET_PROFILE, (void *)(static_cast<int64_t>(profile)));
    }
}

void PowerManagerService::CleanupProximity()
{
    Boolean isHeld;
    if (mProximityWakeLock->IsHeld(&isHeld), isHeld) {
        mProximityWakeLock->ReleaseLock();
    }
    if (mProximityListener != NULL) {
        mSensorManager->UnregisterListener(mProximityListener);
        mProximityListener = NULL;
    }
}

void PowerManagerService::SetButtonBrightnessOverrideFromWindowManagerInternal(
    /* [in] */ Int32 brightness)
{
    {
        AutoLock syncLock(mLock);
        if (mButtonBrightnessOverrideFromWindowManager != brightness) {
            mButtonBrightnessOverrideFromWindowManager = brightness;
            mDirty |= DIRTY_SETTINGS;
            UpdatePowerStateLocked();
        }
    }
}

} // namespace Power
} // namespace Server
} // namespace Droid
} // namespace Elastos
