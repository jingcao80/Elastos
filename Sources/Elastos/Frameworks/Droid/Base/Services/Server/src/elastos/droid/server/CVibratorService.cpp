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

#include "elastos/droid/server/CVibratorService.h"
#include "elastos/droid/server/LocalServices.h"

#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/droid/Manifest.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Hardware.h>
#include <Elastos.Droid.Internal.h>
#include <android/hardware/vibrator/1.0/IVibrator.h>
#include <inttypes.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IIVibratorService;
using Elastos::Droid::Os::EIID_IPowerManagerInternal;
using Elastos::Droid::Os::EIID_ILowPowerModeListener;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IAppOpsManagerHelper;
using Elastos::Droid::App::CAppOpsManagerHelper;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Hardware::Input::EIID_IInputDeviceListener;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CThread;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CVibratorService::TAG("VibratorService");
const Boolean CVibratorService::DBG = FALSE;

ECode CVibratorService::VibrationRunnable::Run()
{
    AutoLock Lock(mHost->mVibrationsLock);
    mHost->DoCancelVibrateLocked();
    mHost->StartNextVibrationLocked();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CVibratorService::LowPowerModeListener, Object, ILowPowerModeListener)

//====================================================================
// CVibratorService::Vibration
//====================================================================

CAR_INTERFACE_IMPL(CVibratorService::Vibration, Object, IProxyDeathRecipient);

CVibratorService::Vibration::Vibration(
    /* [in] */ IBinder* token,
    /* [in] */ Int64 millis,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 usageHint,
    /* [in] */ const String& opPkg,
    /* [in] */ CVibratorService* owner)
    : mToken(token)
    , mTimeout(millis)
    , mStartTime(SystemClock::GetUptimeMillis())
    , mRepeat(0)
    , mUsageHint(usageHint)
    , mUid(uid)
    , mOpPkg(opPkg)
    , mHost(owner)
{}

CVibratorService::Vibration::Vibration(
    /* [in] */ IBinder* token,
    /* [in] */ ArrayOf<Int64>* pattern,
    /* [in] */ Int32 repeat,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 usageHint,
    /* [in] */ const String& opPkg,
    /* [in] */ CVibratorService* owner)
    : mToken(token)
    , mTimeout(0)
    , mStartTime(SystemClock::GetUptimeMillis())
    , mPattern(pattern)
    , mRepeat(repeat)
    , mUsageHint(usageHint)
    , mUid(uid)
    , mOpPkg(opPkg)
    , mHost(owner)
{}

ECode CVibratorService::Vibration::ProxyDied()
{
    AutoLock Lock(mHost->mVibrationsLock);

    mHost->mVibrations.Remove(this);
    if (this == mHost->mCurrentVibration) {
        mHost->DoCancelVibrateLocked();
        mHost->StartNextVibrationLocked();
    }
    return NOERROR;
}

Boolean CVibratorService::Vibration::HasLongerTimeout(
    /* [in] */ Int64 millis)
{
    if (mTimeout == 0) {
        // This is a pattern, return false to play the simple
        // vibration.
        return FALSE;
    }
    if ((mStartTime + mTimeout)
            < (SystemClock::GetUptimeMillis() + millis)) {
        // If this vibration will end before the time passed in, let
        // the new vibration play.
        return FALSE;
    }
    return TRUE;
}

Boolean CVibratorService::Vibration::IsSystemHapticFeedback()
{
    return (mUid == IProcess::SYSTEM_UID || mUid == 0) && mRepeat < 0;
}

ECode CVibratorService::Vibration::ToString(
    /* [in] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder sb("vibration{0x");
    sb += StringUtils::ToHexString((Int32)this);
    if (mOpPkg != NULL) {
        sb += ", opPkg=";
        sb += mOpPkg;
    }
    sb += ", timeout=";
    sb += mTimeout;
    sb += ", startTime=";
    sb += mStartTime;
    sb += ", repeat=";
    sb += mRepeat;
    sb += ", usageHint=";
    sb += mUsageHint;
    sb += ", uid=";
    sb += mUid;
    *str = sb.ToString();
    return NOERROR;
}

//====================================================================
// CVibratorService::SettingsObserver
//====================================================================
CVibratorService::SettingsObserver::SettingsObserver(
    /* [in] */ CVibratorService* owner)
    : mHost(owner)
{}

ECode CVibratorService::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->UpdateInputDeviceVibrators();
    return NOERROR;
}


//====================================================================
// CVibratorService::MyBroadcastReceiver
//====================================================================
CVibratorService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CVibratorService* owner)
    : mHost(owner)
{}

ECode CVibratorService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->UpdateInputDeviceVibrators();
    return NOERROR;
}


//====================================================================
// CVibratorService::VibrateThread
//====================================================================
CVibratorService::VibrateThread::VibrateThread(
    /* [in] */ Vibration* vib,
    /* [in] */ CVibratorService* owner)
    : mVibration(vib)
    , mDone(FALSE)
    , mHost(owner)
{
    mHost->mTmpWorkSource->Set(vib->mUid);
    mHost->mWakeLock->SetWorkSource(mHost->mTmpWorkSource);
    mHost->mWakeLock->AcquireLock();
}

void CVibratorService::VibrateThread::Delay(
    /* [in] */ Int64 duration)
{
    if (duration > 0) {
        Int64 bedtime = duration + SystemClock::GetUptimeMillis();
        do {
            //try {
            Wait(duration);
            //}
            //catch (InterruptedException e) {
            //}
            if (mDone) {
                break;
            }
            duration = bedtime - SystemClock::GetUptimeMillis();
        } while (duration > 0);
    }
}

ECode CVibratorService::VibrateThread::Run()
{
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_URGENT_DISPLAY);
    {
        AutoLock lock(this);
        Int32 index = 0;
        AutoPtr<ArrayOf<Int64> > pattern = mVibration->mPattern;
        Int32 len = pattern->GetLength();
        Int32 repeat = mVibration->mRepeat;
        Int32 uid = mVibration->mUid;
        Int32 usageHint = mVibration->mUsageHint;
        Int64 duration = 0;

        while (!mDone) {
            // add off-time duration to any accumulated on-time duration
            if (index < len) {
                duration += (*pattern)[index++];
            }

            // sleep until it is time to start the vibrator
            Delay(duration);
            if (mDone) {
                break;
            }

            if (index < len) {
                // read on-time duration and start the vibrator
                // duration is saved for delay() at top of loop
                duration = (*pattern)[index++];
                if (duration > 0) {
                    mHost->DoVibratorOn(duration, uid, usageHint);
                }
            }
            else {
                if (repeat < 0) {
                    break;
                }
                else {
                    index = repeat;
                    duration = 0;
                }
            }
        }
        mHost->mWakeLock->ReleaseLock();
    }

    AutoLock Lock(mHost->mVibrationsLock);
    AutoPtr<VibrateThread> holdThis(this);
    if (mHost->mThread.Get() == this) {
        mHost->mThread = NULL;
    }
    if (!mDone) {
        // If this vibration finished naturally, start the next
        // vibration.
        mHost->mVibrations.Remove(mVibration);
        mHost->UnlinkVibration(mVibration);
        mHost->StartNextVibrationLocked();
    }
    return NOERROR;
}


//====================================================================
// CVibratorService::IntentReceiver
//====================================================================
ECode CVibratorService::IntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        AutoLock Lock(mHost->mVibrationsLock);
        // When the system is entering a non-interactive state, we want
        // to cancel vibrations in case a misbehaving app has abandoned
        // them.  However it may happen that the system is currently playing
        // haptic feedback as part of the transition.  So we don't cancel
        // system vibrations.
        if (mHost->mCurrentVibration != NULL
                && !mHost->mCurrentVibration->IsSystemHapticFeedback()) {
            mHost->DoCancelVibrateLocked();
        }

        // Clear all remaining vibrations.
        List<AutoPtr<Vibration> >::Iterator it;
        for (it = mHost->mVibrations.Begin(); it != mHost->mVibrations.End();) {
            AutoPtr<Vibration> vibration = *it;
            if (vibration.Get() != mHost->mCurrentVibration.Get()) {
                mHost->UnlinkVibration(vibration);
                it = mHost->mVibrations.Erase(it);
            }
            else {
                ++it;
            }
        }
    }
    return NOERROR;
}


//====================================================================
// CVibratorService
//====================================================================
CAR_INTERFACE_IMPL_3(CVibratorService, Object, IIVibratorService, IInputDeviceListener, IBinder)

CAR_OBJECT_IMPL(CVibratorService)

CVibratorService::CVibratorService()
    : mVibrateInputDevicesSetting(FALSE)
    , mInputDeviceListenerRegistered(FALSE)
    , mCurVibUid(-1)
    , mLowPowerMode(0)
{
}

ECode CVibratorService::constructor(
    /* [in] */ IContext* context)
{
    CWorkSource::New((IWorkSource**)&mTmpWorkSource);
    CHandler::New((IHandler**)&mH);
    mIntentReceiver = new IntentReceiver(this);

    mVibrationRunnable = new VibrationRunnable(this);

    VibratorInit();
    // Reset the hardware to a default state, in case this is a runtime
    // restart instead of a fresh boot.
    VibratorOff();

    mContext = context;
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(service);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String("*vibrator*"), (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->SetReferenceCounted(TRUE);

    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);

    service = NULL;
    sm->GetService(IContext::APP_OPS_SERVICE, (IInterface**)&service);
    mAppOpsService = IIAppOpsService::Probe(service);

    service = NULL;
    sm->GetService(IBatteryStats::SERVICE_NAME, (IInterface**)&service);
    mBatteryStatsService = IIBatteryStats::Probe(service);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver((IBroadcastReceiver*)mIntentReceiver, filter, (IIntent**)&intent);
    return NOERROR;
}

static android::sp<android::hardware::vibrator::V1_0::IVibrator> mHal;

void CVibratorService::VibratorInit()
{
    if (mHal != nullptr) {
        return;
    }
    mHal = android::hardware::vibrator::V1_0::IVibrator::getService();
}

Boolean CVibratorService::VibratorExists()
{
    if (mHal != nullptr) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void CVibratorService::VibratorOn(
    /* [in] */ Int64 milliseconds)
{
    if (mHal != nullptr) {
        android::hardware::vibrator::V1_0::Status retStatus = mHal->on(milliseconds);
        if (retStatus != android::hardware::vibrator::V1_0::Status::OK) {
            Slogger::E(TAG, "vibratorOn command failed (%" PRIu32 ").", static_cast<uint32_t>(retStatus));
        }
    }
    else {
        Slogger::W(TAG, "Tried to vibrate but there is no vibrator device.");
    }
}

void CVibratorService::VibratorOff()
{
    if (mHal != nullptr) {
        android::hardware::vibrator::V1_0::Status retStatus = mHal->off();
        if (retStatus != android::hardware::vibrator::V1_0::Status::OK) {
            Slogger::E(TAG, "vibratorOff command failed (%" PRIu32 ").", static_cast<uint32_t>(retStatus));
        }
    }
    else {
        Slogger::W(TAG, "Tried to stop vibrating but there is no vibrator device.");
    }
}

ECode CVibratorService::SystemReady()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::INPUT_SERVICE, (IInterface**)&obj);
    mIm = IInputManager::Probe(obj);

    mSettingObserver = new SettingsObserver(this);
    mSettingObserver->constructor(mH);

    mPowerManagerInternal = IPowerManagerInternal::Probe(LocalServices::GetService(EIID_IPowerManagerInternal));
    AutoPtr<ILowPowerModeListener> listener = (LowPowerModeListener*)new LowPowerModeListener(this);
    mPowerManagerInternal->RegisterLowPowerModeObserver(listener);

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUri> uri;
    Settings::System::GetUriFor(ISettingsSystem::VIBRATE_INPUT_DEVICES, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, TRUE, mSettingObserver, UserHandle::USER_ALL);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_USER_SWITCHED, (IIntentFilter**)&filter);
    AutoPtr<MyBroadcastReceiver> myBR = new MyBroadcastReceiver(this);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(myBR.Get(), filter, String(NULL), mH, (IIntent**)&intent);

    UpdateInputDeviceVibrators();
    return NOERROR;
}

ECode CVibratorService::HasVibrator(
    /* [out] */ Boolean* hasVibrator)
{
    VALIDATE_NOT_NULL(hasVibrator);
    *hasVibrator = DoVibratorExists();
    return NOERROR;
}

ECode CVibratorService::VerifyIncomingUid(
    /* [in] */ Int32 uid)
{
    if (uid == Binder::GetCallingUid()) {
        return NOERROR;
    }
    if (Binder::GetCallingPid() == Process::MyPid()) {
        return NOERROR;
    }
    return mContext->EnforcePermission(
        Manifest::permission::UPDATE_APP_OPS_STATS,
        Binder::GetCallingPid(), Binder::GetCallingUid(), String(NULL));
}

ECode CVibratorService::Vibrate(
    /* [in] */ Int32 uid,
    /* [in] */ const String& opPkg,
    /* [in] */ Int64 milliseconds,
    /* [in] */ Int32 usageHint,
    /* [in] */ IBinder* token)
{
    Int32 result = 0;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::VIBRATE, &result));
    if (result != IPackageManager::PERMISSION_GRANTED) {
        //throw new SecurityException("Requires VIBRATE permission");
        Slogger::E(TAG, "Requires VIBRATE permission");
        return E_SECURITY_EXCEPTION;
    }
    FAIL_RETURN(VerifyIncomingUid(uid))
    // We're running in the system server so we cannot crash. Check for a
    // timeout of 0 or negative. This will ensure that a vibration has
    // either a timeout of > 0 or a non-null pattern.
    if (milliseconds <= 0 || (mCurrentVibration != NULL && mCurrentVibration->HasLongerTimeout(milliseconds))) {
        // Ignore this vibration since the current vibration will play for
        // longer than milliseconds.
        return NOERROR;
    }

    if (DBG) {
        Slogger::D(TAG, "Vibrating for %lld ms.", milliseconds);
    }

    AutoPtr<Vibration> vib = new Vibration(token, milliseconds, uid, usageHint, opPkg, this);

    Int64 ident = Binder::ClearCallingIdentity();
    {
        AutoLock syncLock(mVibrationsLock);
        RemoveVibrationLocked(token);
        DoCancelVibrateLocked();
        mCurrentVibration = vib;
        StartVibrationLocked(vib);
    }

    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

Boolean CVibratorService::IsAll0(
    /* [in] */ const ArrayOf<Int64>& pattern)
{
    Int32 N = pattern.GetLength();
    for (Int32 i = 0; i < N; i++) {
        if (pattern[i] != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

ECode CVibratorService::VibratePattern(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ ArrayOf<Int64>* pattern,
    /* [in] */ Int32 repeat,
    /* [in] */ Int32 usageHint,
    /* [in] */ IBinder* token)
{
    Int32 result = 0;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::VIBRATE, &result));
    if (result != IPackageManager::PERMISSION_GRANTED) {
        //throw new SecurityException("Requires VIBRATE permission");
        Slogger::E(TAG, "Requires VIBRATE permission");
        return E_SECURITY_EXCEPTION;
    }
    FAIL_RETURN(VerifyIncomingUid(uid))
    // so wakelock calls will succeed
    Int64 identity = Binder::ClearCallingIdentity();
    //try {
    if (DBG) {
        StringBuilder sb("");
        Int32 N = pattern->GetLength();
        for (Int32 i = 0; i < N; i++) {
            sb += " ";
            sb += (*pattern)[i];
        }
        Slogger::D(TAG, "vibrating with pattern: {%s}", sb.ToString().string());
    }

    // we're running in the server so we can't fail
    if (pattern == NULL || pattern->GetLength() == 0
        || IsAll0(*pattern)
        || repeat >= pattern->GetLength() || token == NULL) {
        Binder::RestoreCallingIdentity(identity);
        return NOERROR;
    }

    AutoPtr<Vibration> vib = new Vibration(token, pattern, repeat,
        uid, usageHint, packageName, this);
    AutoPtr<IProxy> proxy = (IProxy*)token->Probe(EIID_IProxy);
    //try {
    if (proxy != NULL) proxy->LinkToDeath(vib, 0);
    //} catch (RemoteException e) {
    //    return;
    //}

    {
        AutoLock Lock(mVibrationsLock);
        RemoveVibrationLocked(token);
        DoCancelVibrateLocked();
        if (repeat >= 0) {
            mVibrations.PushFront(vib);
            StartNextVibrationLocked();
        }
        else {
            // A negative repeat means that this pattern is not meant
            // to repeat. Treat it like a simple vibration.
            mCurrentVibration = vib;
            StartVibrationLocked(vib);
        }
    }
    //}
    //finally {
    Binder::RestoreCallingIdentity(identity);
    //}
    return NOERROR;
}

ECode CVibratorService::CancelVibrate(
    /* [in] */ IBinder* token)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::VIBRATE, String("cancelVibrate")));

    // so wakelock calls will succeed
    Int64 identity = Binder::ClearCallingIdentity();
    //try {
    AutoLock Lock(mVibrationsLock);
    AutoPtr<Vibration> vib = RemoveVibrationLocked(token);
    if (vib == mCurrentVibration) {
        if (DBG) {
            Slogger::D(TAG, "Canceling vibration.");
        }
        DoCancelVibrateLocked();
        StartNextVibrationLocked();
    }
    //}
    //finally {
    Binder::RestoreCallingIdentity(identity);
    //}
    return NOERROR;
}

void CVibratorService::DoCancelVibrateLocked()
{
    if (mThread != NULL) {
        AutoLock lock(mThread.Get());
        mThread->mDone = TRUE;
        mThread->Notify();
        mThread = NULL;
    }
    DoVibratorOff();
    mH->RemoveCallbacks(mVibrationRunnable);
    ReportFinishVibrationLocked();
}

// Lock held on mVibrations
void CVibratorService::StartNextVibrationLocked()
{
    if (mVibrations.IsEmpty()) {
        ReportFinishVibrationLocked();
        mCurrentVibration = NULL;
        return;
    }
    mCurrentVibration = mVibrations.GetFront();
    StartVibrationLocked(mCurrentVibration);
}

// Lock held on mVibrations
void CVibratorService::StartVibrationLocked(
    /* [in] */ Vibration* vib)
{
    if (mLowPowerMode &&
        vib->mUsageHint != IAudioAttributes::USAGE_NOTIFICATION_RINGTONE) {
        return;
    }

    AutoPtr<IAppOpsManagerHelper> helper;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&helper);
    AutoPtr<IBinder> token;
    helper->GetToken(mAppOpsService, (IBinder**)&token);

    Boolean result;
    Int32 mode;
    mAppOpsService->CheckAudioOperation(IAppOpsManager::OP_VIBRATE,
        vib->mUsageHint, vib->mUid, vib->mOpPkg, &mode);
    if (mode == IAppOpsManager::MODE_ALLOWED) {
        mAppOpsService->StartOperation(token,
            IAppOpsManager::OP_VIBRATE, vib->mUid, vib->mOpPkg, &mode);
    }

    if (mode != IAppOpsManager::MODE_ALLOWED) {
        if (mode == IAppOpsManager::MODE_ERRORED) {
            Slogger::W(TAG, "Would be an error: vibrate from uid %d", vib->mUid);
        }
        mH->Post(mVibrationRunnable, &result);
        return;
    }

    if (vib->mTimeout != 0) {
        DoVibratorOn(vib->mTimeout, vib->mUid, vib->mUsageHint);

        mH->PostDelayed(mVibrationRunnable, vib->mTimeout, &result);
    }
    else {
        // mThread better be null here. doCancelVibrate should always be
        // called before startNextVibrationLocked or startVibrationLocked.
        mThread = new VibrateThread(vib, this);
        mThread->constructor();
        mThread->Start();
    }
}

void CVibratorService::ReportFinishVibrationLocked()
{
    if (mCurrentVibration != NULL) {
        AutoPtr<IAppOpsManagerHelper> helper;
        CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&helper);
        AutoPtr<IBinder> token;
        helper->GetToken(mAppOpsService, (IBinder**)&token);
        mAppOpsService->FinishOperation(token,
            IAppOpsManager::OP_VIBRATE, mCurrentVibration->mUid, mCurrentVibration->mOpPkg);
        mCurrentVibration = NULL;
    }
}

// Lock held on mVibrations
AutoPtr<CVibratorService::Vibration> CVibratorService::RemoveVibrationLocked(
    /* [in] */ IBinder* token)
{
    List<AutoPtr<Vibration> >::Iterator it;
    for (it = mVibrations.Begin(); it != mVibrations.End(); ++it) {
        AutoPtr<Vibration> vib = *it;
        if (vib->mToken.Get() == token) {
            mVibrations.Erase(it);
            UnlinkVibration(vib);
            return vib;
        }
    }
    // We might be looking for a simple vibration which is only stored in
    // mCurrentVibration.
    if (mCurrentVibration != NULL && mCurrentVibration->mToken.Get() == token) {
        UnlinkVibration(mCurrentVibration);
        return mCurrentVibration;
    }
    return NULL;
}

void CVibratorService::UnlinkVibration(
    /* [in] */ Vibration* vib)
{
    if (vib->mPattern != NULL) {
        // If Vibration object has a pattern,
        // the Vibration object has also been linkedToDeath.
        AutoPtr<IProxy> proxy = (IProxy*)vib->mToken->Probe(EIID_IProxy);
        if (proxy != NULL) {
            Boolean result;
            proxy->UnlinkToDeath(vib, 0, &result);
        }
    }
}

void CVibratorService::UpdateInputDeviceVibrators()
{
    AutoLock Lock(mVibrationsLock);
    DoCancelVibrateLocked();

    {
        AutoLock Lock(mInputDeviceVibratorsLock);

        mVibrateInputDevicesSetting = FALSE;
        //try {
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        Int32 value = 0;
        Settings::System::GetInt32ForUser(
                resolver,
                String("vibrate_input_devices"),
                IUserHandle::USER_CURRENT,
                &value);
        mVibrateInputDevicesSetting = (value > 0);
        //} catch (SettingNotFoundException snfe) {
        //}

        if (mVibrateInputDevicesSetting) {
            if (!mInputDeviceListenerRegistered) {
                mInputDeviceListenerRegistered = TRUE;
                mIm->RegisterInputDeviceListener(this, mH);
            }
        }
        else {
            if (mInputDeviceListenerRegistered) {
                mInputDeviceListenerRegistered = FALSE;
                mIm->UnregisterInputDeviceListener(this);
            }
        }

        mInputDeviceVibrators.Clear();
        if (mVibrateInputDevicesSetting) {
            AutoPtr< ArrayOf<Int32> > ids;
            mIm->GetInputDeviceIds((ArrayOf<Int32>**)&ids);
            Int32 len = ids->GetLength();
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInputDevice> device;
                mIm->GetInputDevice((*ids)[i], (IInputDevice**)&device);
                AutoPtr<IVibrator> vibrator;
                device->GetVibrator((IVibrator**)&vibrator);
                Boolean hasVibrator = FALSE;
                vibrator->HasVibrator(&hasVibrator);
                if (hasVibrator) {
                    mInputDeviceVibrators.PushBack(vibrator);
                }
            }
        }
    }

    StartNextVibrationLocked();
}

ECode CVibratorService::OnInputDeviceAdded(
    /* [in] */ Int32 deviceId)
{
    UpdateInputDeviceVibrators();
    return NOERROR;
}

//@Override
ECode CVibratorService::OnInputDeviceChanged(
    /* [in] */ Int32 deviceId)
{
    UpdateInputDeviceVibrators();
    return NOERROR;
}

//@Override
ECode CVibratorService::OnInputDeviceRemoved(
    /* [in] */ Int32 deviceId)
{
    UpdateInputDeviceVibrators();
    return NOERROR;
}

Boolean CVibratorService::DoVibratorExists() {
    // For now, we choose to ignore the presence of input devices that have vibrators
    // when reporting whether the device has a vibrator.  Applications often use this
    // information to decide whether to enable certain features so they expect the
    // result of hasVibrator() to be constant.  For now, just report whether
    // the device has a built-in vibrator.
    //{    AutoLock syncLock(mInputDeviceVibrators);
    //    return !mInputDeviceVibrators.isEmpty() || vibratorExists();
    //}
    return VibratorExists();
}

void CVibratorService::DoVibratorOn(
    /* [in] */ Int64 millis,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 usageHint)
{
    AutoLock Lock(mInputDeviceVibratorsLock);

    mBatteryStatsService->NoteVibratorOn(uid, millis);
    mCurVibUid = uid;

    if (!mInputDeviceVibrators.IsEmpty()) {
        AutoPtr<IAudioAttributesBuilder> builder;
        CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&builder);
        builder->SetUsage(usageHint);
        AutoPtr<IAudioAttributes> attributes;
        builder->Build((IAudioAttributes**)&attributes);

        List<AutoPtr<IVibrator> >::Iterator it;
        for (it = mInputDeviceVibrators.Begin(); it != mInputDeviceVibrators.End(); ++it) {
            (*it)->Vibrate(millis, attributes);
        }
    }
    else {
        VibratorOn(millis);
    }
}

void CVibratorService::DoVibratorOff()
{
    AutoLock Lock(mInputDeviceVibratorsLock);
    if (mCurVibUid >= 0) {
        mBatteryStatsService->NoteVibratorOff(mCurVibUid);
        mCurVibUid = -1;
    }

    if (!mInputDeviceVibrators.IsEmpty()) {
        List<AutoPtr<IVibrator> >::Iterator it;
        for (it = mInputDeviceVibrators.Begin(); it != mInputDeviceVibrators.End(); ++it) {
            (*it)->Cancel();
        }
    }
    else {
        VibratorOff();
    }
}

ECode CVibratorService::ToString(
        /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
}

}//namespace Server
}//namespace Droid
}//namespace Elastos
